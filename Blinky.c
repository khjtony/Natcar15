//EEC195A
//Chenguang Yin
//Ray lin
//Hengjiu Kang


// Simple test program using PIT interrupts
// Toggle output on PTB0 at f = 1/(2*100 us) = 5 kHz

#include <MKL25Z4.H>
#include <math.h>
#include <stdio.h>
#include "Blinkly.h"
#include "global_var.h"
#include "ADC_kit.h"
#include "Kit_chain.h"
#include "core_algorithm.h"
#include "./accel/GPIO_defs.h"
#include "./accel/LEDs.h"
#include "./accel/i2c.h"
#include "./accel/mma8451.h"
#include "./accel/delay.h"

#define uart_baud 57600
#define buffer_ceil 128


unsigned char POT1; 
unsigned char POT2;

typedef struct {
	float dState; // Last position input
	float iState; // Integrator state
	float iMax, iMin; // Maximum and minimum allowable integrator stat
	float iGain; // integral gain
	float pGain; // proportional gain
	float dGain; // derivative gain
	
}PIT_car;

PIT_car PID_servo;
PIT_car PID_speed_L;
PIT_car PID_speed_R;


void translator(char keyIn);  //a translator to convert input value to char
int current_read;

void put(char *ptr_str)  //copied put function
{
	while(*ptr_str){
		uart0_putchar(*ptr_str++);
	}
}

void PID_speed_set(PIT_car* PID_speed, float ig,float pg,float dg, float imax, float imin){
	PID_speed->dGain=dg;
	PID_speed->iGain=ig;
	PID_speed->pGain=pg;
	PID_speed->iMax=imax;
	PID_speed->iMin=imin;
	
	
}
void PID_servo_set(float ig,float pg,float dg, float imax, float imin){
	PID_servo.dGain=dg;
	PID_servo.iGain=ig;
	PID_servo.pGain=pg;
	PID_servo.iMax=imax;
	PID_servo.iMin=imin;
}


float PID_kernel(PIT_car * pid, float error, float position){
	float pTerm, dTerm, iTerm;
	pTerm = pid->pGain * error; // calculate the proportional term
	// calculate the integral state with appropriate limiting
	pid->iState += error;
	if (pid->iState > pid->iMax) pid->iState = pid->iMax;
	else if (pid->iState < pid->iMin) pid->iState = pid->iMin;
	iTerm = pid->iGain * (pid->iState); // calculate the integral term
	dTerm = pid->dGain * (pid->dState - position);
	pid->dState = position;
	return pTerm + dTerm + iTerm;
		//return pTerm + dTerm;
}




	
/*-----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint8_t next_state=0;
	int uart0_clk_khz;
	int left_track;
	int right_track;
	int middle_point=-20;
	int acc_count=0;
	int speed_mod=0;
	int servo_err;
	int last;

	SIM->SCGC5  |= (SIM_SCGC5_PORTA_MASK
									| SIM_SCGC5_PORTB_MASK
                  |	SIM_SCGC5_PORTC_MASK
									| SIM_SCGC5_PORTD_MASK
	                | SIM_SCGC5_PORTE_MASK);      		// Enable Clock to Port B & D 
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
	
//Camera	
	//************************************
	//	SI PTD7
  //	Camera
	//************************************
	PORTD->PCR[7] = PORT_PCR_MUX(1);												// Pin PTB0 is GPIO
	FPTD->PDOR |= 1<<7;																	// initialize PTD7
	FPTD->PDDR |= 1<<7;																	// configure PTD7 as output
	

	//************************************
	//	 CLK PTE1
  //	 Camera
	//************************************
	
	PORTE->PCR[1] = PORT_PCR_MUX(1);												
	FPTE->PDOR |= 1<<1;																
	FPTE->PDDR |= 1<<1;		
	
	
	//************************************
	//	 Cmaera PTD5
  //	 Camera
	//************************************
	PORTD->PCR[5] = PORT_PCR_MUX(0);
	
	//************************************
	//	 Cmaera PTD5
  //	 Camera
	//************************************
	PORTD->PCR[5] = PORT_PCR_MUX(0);
	
	//************************************
	//	Camera reading indicator PTB1
  //	Camera
	//************************************
	PORTB->PCR[1] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTB->PDOR |= 1<<1;																	// initialize PTB0
	FPTB->PDDR |= 1<<1;		
	
	
	
	
	
//speed sensor
	//************************************
	//	
  //	speed sensor @ PTA1
	//************************************
	PORTA->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
	FPTA->PDDR &= 0UL<<1;																	// configure PTA2 as INPUT


	//************************************
	//	
  //	speed sensor @ PTA2
	//************************************
	PORTA->PCR[2] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
	FPTA->PDDR &= 0UL<<2;																	// configure PTA2 as INPUT
	
	
	
//4-pin switch



//SW switchs
	//************************************
	//	 SW  1 & SW 2  /PTC13 PTC17
  //
	//************************************
	
	PORTC->PCR[17] = PORT_PCR_MUX(1);
	PORTC->PCR[13] = PORT_PCR_MUX(1);
	FPTC->PDDR &= ~(1UL<<17);
	FPTC->PDDR &= ~(1UL<<13);
	
	

//battery
	//************************************
	//	Battery Light
  //	Battery Light
	//************************************
		PORTB->PCR[8] = (1UL << 8);												// Pin PTC9 is GPIO
	FPTB->PDOR |= 1UL<<8;																	// initialize PTB8
	FPTB->PDDR |= 1UL<<8;																	// configure PTB8 as INPUT
	FPTB->PCOR |= 1UL<<8;
		PORTB->PCR[9] = (1UL << 8);												// Pin PTC9 is GPIO
	FPTB->PDOR |= 1UL<<9;																	// initialize PTB9
	FPTB->PDDR |= 1UL<<9;																	// configure PTB9 as INPUT
	FPTB->PCOR |= 1UL<<9;
		PORTB->PCR[10] = (1UL << 8);												// Pin PTC9 is GPIO
	FPTB->PDOR |= 1UL<<10;																	// initialize PTB10
	FPTB->PDDR |= 1UL<<10;																	// configure PTB10 as INPUT
	FPTB->PCOR |= 1UL<<10;
		PORTB->PCR[11] = (1UL << 8);												// Pin PTC9 is GPIO
	FPTB->PDOR |= 1UL<<11;																	// initialize PTB11
	FPTB->PDDR |= 1UL<<11;																	// configure PTB11 as INPUT
	FPTB->PCOR |= 1UL<<11;



//H-bridge
	//************************************
	//	H-Bridge_A_IFB PTE23 	ADC0_SE7a
  //	H-Bridge_B_IFB PTE22 	ADC0_SE3
	//************************************

//	PORTE->PCR[22] = PORT_PCR_MUX(0x0);		
//	PORTE->PCR[23] = PORT_PCR_MUX(0x0);	
	
	//************************************
	//	H-Bridge_A_ FTM
  //	H-Bridge_B_	FTM
	//************************************
	//Done in Motor Initial
	//PORTC -> PCR[2] = PORT_PCR_MUX(1) |  PORT_PCR_PS_MASK;
	//FPTC -> PDOR |= 1UL <<2;
	//FPTC->PSOR |= 1UL <<2;
	//PORTC -> PCR[4] = PORT_PCR_MUX(1) |  PORT_PCR_PS_MASK;
	//FPTC -> PDOR |= 1UL <<4;
	//FPTC->PSOR |= 1UL <<4;
	
	//************************************
	//	H-bridge Enable
  //
	//************************************
	PORTE->PCR[21] = (1UL << 8);												// Pin PTE21 is GPIO
	FPTE->PDOR |= 1UL<<21;																	// initialize PTE21
	FPTE->PDDR |= 1UL<<21;
	FPTE->PSOR |= 1UL<<21;

	
	
//servo 
	//************************************
	//	TPM1_CH0 PTB0
  //	Turning
	//************************************
	//Done in Servo initial
	



	
//UART
	//************************************
	//	Setup uart RX
  //	Setup uart TX
	//************************************
	//PORTE->PCR[21] = PORT_PCR_MUX(0x4);		// Enable the UART0_RX function on PTB16
	//PORTE->PCR[20] = PORT_PCR_MUX(0x4);		// Enable the UART0_TX function on PTB17
	//PORTA->PCR[1] = PORT_PCR_MUX(0x2);		// Enable the UART0_RX function on PTB16
	//PORTA->PCR[2] = PORT_PCR_MUX(0x2);		// Enable the UART0_TX function on PTB17
	uart0_clk_khz = (48000000 / 1000); // UART0 clock frequency will equal half the PLL frequency	
	uart0_init (uart0_clk_khz, uart_baud);

//MMA7451Q

	//************************************
	//	
  //	Setup Accel INT @ PTA15
	//************************************
		/* Select GPIO and enable pull-up resistors and interrupts 
		on falling edges for pins connected to switches */

//	PORTA->PCR[15] =PORT_PCR_MUX(1)  | PORT_PCR_IRQC(0x09); 
//	FPTA->PDDR &= 0u<<15;																	// configure PTA14 as INPUT

	PORTC->PCR[9] = PORT_PCR_MUX(1);												// Pin PTC9 is GPIO
	FPTC->PDDR |= 1UL<<9;																	// configure PTC14 as INPUT
	FPTC->PCOR |= 1UL<<9;																	// configure PTC14 as INPUT
	


	
	

	//************************************
	//	
  //	enable PTA NVIC
	//************************************
	NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);

	
	
	//************************************
	//	 Initialize
  //	 ADC,TPM,PIT
	//************************************
	servo_PW=4500;
	Init_PIT1(100000);	
	Init_PIT(18000);																		// count-down period = 100HZ T=48Mhz/12000
	
	Init_PWM_motor();
  Init_PWM_servo();
	i2c_init();																/* init i2c	*/
	init_mma(); 												/* init mma peripheral */
	//init_origin_mma();
	
	Init_RGB_LEDs();
	accel_queue_init();
	Start_PIT1();
	Start_PIT();

	
	//PID_servo_set(float ig,float pg,float dg, float imax, float imin)
	//PID_servo_set(2,15,18, 50, 5);
	PID_servo_set(2,15,18, 50, 5);
	PID_speed_set(&PID_speed_R,20,25,30, 50, 0);
	PID_speed_set(&PID_speed_L,20,25,30, 50, 0);
	
	
	//put("Hello World\n");
	//Application start
	//Enter state machine
	next_state=0;

	left_PW=0;
	right_PW=0;
	

	

	Battery_ind(4);



	//****************
	//bare board test
	//****************



//FSM:
//0: initialized
//1: old age
//2: debugging
//3: straight line
//4: left turn
//5: right turn
//6: over hill



	while(1){
		//accel_Q();
		FPTC->PTOR |= 1UL <<9;
		switch (next_state){
			case 0:
				//if input is SW1 enter running mode state 1
				if ((FPTC->PDIR & (1<<13))){
					put("Racing Mode\r\n");
					Control_RGB_LEDs(0,0,0);
					left_PW=3000;
					right_PW=0;
					next_state=1;
					break;
				}
				//if input is sw2 enter debug mode state 2
				if ((FPTC->PDIR & (1<<17))){
					put("Debug Mode\r\n");
					Control_RGB_LEDs(0,0,0);
					next_state=2;
					break;
				}
				break;
			case 1:
				Battery_ind(0);
				PID_servo_set(3,17,27, 50, 5);
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
				right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
				
				speed_mod=lround(PID_kernel(&PID_speed_R,4-right_FB,right_FB));
			
				if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				right_PW=_motor_limit(speed_mod);
				
				
				
				speed_mod=lround(PID_kernel(&PID_speed_L,4-left_FB,left_FB));
					if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				left_PW=_motor_limit(speed_mod);
				
				//middle_point=(middle_point+last)>>2;
					
					middle_point=right_track-left_track;
<<<<<<< HEAD
				if (middle_point>6){
					servo_err=middle_point-6;
				}
				else if(middle_point<-6){
					servo_err=middle_point+6;
				}else{
					servo_err=0;
=======
				if (middle_point>7){
					servo_err=middle_point-3;
				}
				else if(middle_point<-7){
					servo_err=middle_point+3;
>>>>>>> PID
				}
					
					
					servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,servo_err,middle_point)));
				if (middle_point>45){
					next_state=5;
				}else if(middle_point<-45){
					next_state=4;
				}else if (fabs(current_roll)>25){
					next_state=9;
				}else{
					
				}
					
				last=middle_point;
				break;
			case 2:		//debug mode
			//	_DEBUG_running();
			//i,p,d,imax,imin
					PID_servo_set(1,24,35, 60, 5);
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
				right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
				
	
				speed_mod=lround(PID_kernel(&PID_speed_R,7-right_FB,right_FB));
	
			
				if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				right_PW=_motor_limit(speed_mod);
				
				
				
				speed_mod=lround(PID_kernel(&PID_speed_L,7-left_FB,left_FB));
					if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				left_PW=_motor_limit(speed_mod);
				
			
					
				middle_point=right_track-left_track;
				if (middle_point>5){
					middle_point-=3;
				}
				else if(middle_point<-5){
					middle_point+=3;
				}
				servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,middle_point,middle_point)));					
				break;
			case 3:		//experiment mode
				break;
				
				
			case 4:	//left_turn
				Battery_ind(1);
<<<<<<< HEAD
				PID_servo_set(2,15,18, 60, 5);
=======
				PID_servo_set(0.1,10,27, 50, 5);
>>>>>>> PID
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
				right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
				
				speed_mod=lround(PID_kernel(&PID_speed_R,5-right_FB,right_FB));
			
				if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				right_PW=_motor_limit(speed_mod);
				
				
				
				speed_mod=lround(PID_kernel(&PID_speed_L,3-left_FB,left_FB));
					if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				left_PW=_motor_limit(speed_mod);
				
					
<<<<<<< HEAD
				middle_point=(middle_point+last)>>2;
			
					
					middle_point=right_track-left_track;
				if (middle_point>5){
					servo_err=middle_point-4;
				}
				else if(middle_point<-5){
					servo_err=middle_point+4;
				}else{
					servo_err=0;
=======
				//middle_point=(middle_point+last)>>2;
			
					
					middle_point=right_track-left_track;
			/*	if (middle_point>4){
					servo_err=middle_point-2;
>>>>>>> PID
				}
				else if(middle_point<-4){
					servo_err=middle_point+2;
				} */
					
					
<<<<<<< HEAD
				servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,servo_err-9,middle_point)));
=======
				servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,middle_point-5,middle_point)));
>>>>>>> PID
				if (middle_point>30){
					next_state=5;
				}else if(middle_point<-30){
					next_state=4;
				}else if (fabs(current_roll)>20){
					next_state=9;
				}
				else {
					next_state=1;
				}
				last=middle_point;
				break;
				
				
				
			case 5:	//right_turn
				Battery_ind(4);
<<<<<<< HEAD
				PID_servo_set(2,15,18, 60, 5);
=======
				PID_servo_set(0.01,10,27, 50, 5);
>>>>>>> PID
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
				right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
				
				speed_mod=lround(PID_kernel(&PID_speed_R,3-right_FB,right_FB));
			
				if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				right_PW=_motor_limit(speed_mod);
				
				
				
				speed_mod=lround(PID_kernel(&PID_speed_L,5-left_FB,left_FB));
					if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				left_PW=_motor_limit(speed_mod);
				
			
					
					
<<<<<<< HEAD

				middle_point=(middle_point+last)>>2;
					
				middle_point=right_track-left_track;
				if (middle_point>5){
					servo_err=middle_point-4;
				}
				else if(middle_point<-5){
					servo_err=middle_point+4;
				}else{
					servo_err=0;
=======
				//middle_point=(middle_point+last)>>2;
					
				middle_point=right_track-left_track;
			/*	if (middle_point>4){
					servo_err=middle_point-2;
>>>>>>> PID
				}
				else if(middle_point<-4){
					servo_err=middle_point+2;
				}*/
					
					
<<<<<<< HEAD
				servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,servo_err+9,middle_point)));
				if (middle_point>30){
=======
				servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,middle_point+5,middle_point)));
				if (middle_point>40){
>>>>>>> PID
					next_state=5;
				}else if(middle_point<-40){
					next_state=4;
				}else if (fabs(current_roll)>20){
					next_state=9;
				}
				else {
					next_state=1;
				}
				last=middle_point;
				break;
				
			case 9:
				PID_servo_set(2,15,18, 50, 5);
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
				right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
				
				speed_mod=lround(PID_kernel(&PID_speed_R,2-right_FB,right_FB));
			
				if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				right_PW=_motor_limit(speed_mod);
				
				speed_mod=lround(PID_kernel(&PID_speed_L,2-left_FB,left_FB));
					if (left_track>100 && right_track >100){
					speed_mod=0;
					}
				left_PW=_motor_limit(speed_mod);
				
			
					
					
				middle_point=right_track-left_track;
				servo_PW=_servo_limit(4500-lround(PID_kernel(&PID_servo,middle_point-(middle_point>0? 7 :-7),middle_point)));
				if (middle_point>30){
					next_state=5;
				}else if(middle_point<-30){
					next_state=4;
				}else if (fabs(current_roll)>25){
					next_state=9;
				}else {
					next_state=1;
				}
				
			}
		//FPTC->PCOR |= 1UL <<9;
	}
}




void _DEBUG_running(){
		int mod;
		//FPTC->PTOR |= 1UL<<9;
		mod=lround(PID_kernel(&PID_speed_R,7-right_FB,right_FB));
		right_PW=_motor_limit(mod);
		
		//mod=lround(fabs(PID_kernel(&PID_speed_L,6-left_FB,left_FB)));
		left_PW=_motor_limit(mod);
	//	Battery_ind(left_FB/5);
		Control_RGB_LEDs(0, (fabs(current_roll) > 20)? 1:0,0);
	//	Battery_ind(fabs(roll)/10);
		//right_PW=_motor_limit(mod);
	

	//	if (Camera_DONE==1){
		// DEBUG_print_track(buffer[0][1-buffer_sel]);
		// DEBUG_print_track(buffer[1][1-buffer_sel]);
	//		put("\r\n");
		 //DEBUG_print_double_camera(buffer[0][1-buffer_sel],buffer[1][1-buffer_sel]);
		
			
//		uart0_putchar(0x00);
		//DEBUG_print_camera(buffer[0][1-buffer_sel]);
		//DEBUG_print_track(buffer[0][1-buffer_sel]);
		//uart0_putchar(0x01);
		//DEBUG_print_camera(buffer[1][1-buffer_sel]);
			//DEBUG_print_track(buffer[1][1-buffer_sel]);
		//	Control_RGB_LEDs(0, (fabs(get_roll()) > 30)? 1:0,0);

		

//}

	
}

