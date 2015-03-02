//EEC195A
//Chenguang Yin
//Ray lin
//Hengjiu Kang


// Simple test program using PIT interrupts
// Toggle output on PTB0 at f = 1/(2*100 us) = 5 kHz

#include <MKL25Z4.H>
#include <stdio.h>
#include "Blinkly.h"
#include "global_var.h"
#include "ADC_kit.h"
#include "Kit_chain.h"
#include "core_algorithm.h"
#include <math.h>
#include "./accel/GPIO_defs.h"
#include "./accel/LEDs.h"
#include "./accel/i2c.h"
#include "./accel/mma8451.h"
#include "./accel/delay.h"


#define buffer_ceil 128


unsigned char POT1; 
unsigned char POT2;

void translator(char keyIn);  //a translator to convert input value to char
int current_read;

void put(char *ptr_str)  //copied put function
{
	while(*ptr_str){
		uart0_putchar(*ptr_str++);
	}
}


	
/*-----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint8_t next_state=0;
	int uart0_clk_khz;
	int left_track;
	int right_track;
	int left_ratio=20;
	int right_ratio=25;
	int middle_point=-20;
	int acc_flag=1;
	int turn_flag=0;
	int acc_count=0;
	int turning_high=25000;
	int turning_low=15000;
	SIM->SCGC5  |= (SIM_SCGC5_PORTA_MASK
									| SIM_SCGC5_PORTB_MASK
                  |	SIM_SCGC5_PORTC_MASK
									| SIM_SCGC5_PORTD_MASK
	                | SIM_SCGC5_PORTE_MASK);      		// Enable Clock to Port B & D 
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
	
	
	//************************************
	//	Setup uart TX
  //	Setup uart TX
	//************************************
	PORTA->PCR[1] |= PORT_PCR_MUX(0x2);		// Enable the UART0_RX function on PTA1
	PORTA->PCR[2] |= PORT_PCR_MUX(0x2);		// Enable the UART0_TX function on PTA2
	uart0_clk_khz = (48000000 / 1000); // UART0 clock frequency will equal half the PLL frequency	
	uart0_init (uart0_clk_khz, TERMINAL_BAUD);
	
	
	//************************************
	//	H-Bridge_A_IFB PTE23 ADC0_SE7a
  //	H-Bridge_B_IFB PTE22 	ADC0_SE3
	//************************************

	PORTE->PCR[22] |= PORT_PCR_MUX(0x0);		
	PORTE->PCR[23] |= PORT_PCR_MUX(0x0);		
	
	
	//************************************
	//	H-Bridge_A_IFB PTE23 ADC0_SE7a	PTE23 	ADC0_SE7a
  //	H-Bridge_B_IFB 	PTE22 	ADC0_SE3
	//************************************
	PORTC->PCR[2] |= (1UL << 8);												// Pin PTC2 is GPIO
	FPTC->PDOR |= 1UL<<2;																	// initialize PTC2
	FPTC->PDDR |= 1UL<<2;
	FPTC->PCOR &= 0UL<<2;
	FPTC->PDOR |= 1UL<<4;																	// initialize PTC4
	FPTC->PDDR |= 1UL<<4;
	FPTC->PCOR &= 0UL<<4;
	

	
	//************************************
	//	TPM1_CH0 PTB0
  //	Turning
	//************************************
	PORTB->PCR[0] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTB->PDOR |= 1;																	// initialize PTB0
	FPTB->PDDR |= 1;																	// configure PTB0 as output
	
	
	//************************************
	//	SI PTD7
  //	Camera
	//************************************
	PORTD->PCR[7] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTD->PDOR |= 1<<7;																	// initialize PTD7
	FPTD->PDDR |= 1<<7;																	// configure PTD7 as output
	

	//************************************
	//	 CLK PTE1
  //	 Camera
	//************************************
	
	PORTE->PCR[1] = (1UL << 8);												
	FPTE->PDOR |= 1<<1;																
	FPTE->PDDR |= 1<<1;		

	//************************************
	//	 AO
  //	 Camera 1
	//************************************
	//ADC0->SC1[0] |= AIEN_ON| DIFF_SINGLE | ADC_SC1_ADCH(6); 
	//start conversion on channel SE6b(PTD5)
	
	//************************************
	//	 AO 
  //	 Camera 2
	//************************************
	//ADC0->SC1[0] |= AIEN_ON| DIFF_SINGLE | ADC_SC1_ADCH(6); 
	//start conversion on channel SE6b(PTD5)

	//************************************
	//	Camera reading indicator PTB1
  //	Camera
	//************************************
	PORTB->PCR[1] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTB->PDOR |= 1<<1;																	// initialize PTB0
	FPTB->PDDR |= 1<<1;																	// configure PTB0 as output



	//************************************
	//	Unknonw PTE21
  //	Unknonw
	//************************************
	PORTE->PCR[21] = (1UL << 8);												// Pin PTE21 is GPIO
	FPTE->PDOR |= 1UL<<21;																	// initialize PTE21
	FPTE->PDDR |= 1UL<<21;
	FPTE->PCOR = 1UL<<21;
	
	//************************************
	//	 ?
  //	 ?
	//************************************
  FPTE->PSOR |= 1UL<<21;


	//************************************
	//	 setup clock PTC17 PTC17
  //	 ??what this talking about
	//************************************
	
	PORTC->PCR[17] |= PORT_PCR_MUX(1);
	PORTC->PCR[13] |= PORT_PCR_MUX(1);
	FPTC->PDDR &= ~(1UL<<17);
	FPTC->PDDR &= ~(1UL<<13);
	
	//





//
	
	
	
	//************************************
	//	 Initialize
  //	 ADC,TPM,PIT
	//************************************
	right_PW=0;
	left_PW=0;
	servo_PW=4500;
	Init_PIT(12000);																		// count-down period = 100HZ
	Init_PWM_motor();
  Init_PWM_servo();
	i2c_init();																/* init i2c	*/
	init_mma(); 												/* init mma peripheral */
	Start_PIT();
	Init_RGB_LEDs();
	
	
	//put("Hello World\n");
	//Application start
	//Enter state machine
	next_state=0;

	left_PW=60000;
	right_PW=0;
	




	while(1){
		switch (next_state){
			case 0:
				Start_PIT();
				//if input is SW1 enter running mode state 1
				if ((FPTC->PDIR & (1<<13))){
					put("Racing Mode\r\n");
					left_PW=35000;
					right_PW=25000;
					next_state=1;
					break;
				}
				//if input is sw2 enter debug mode state 2
				if ((FPTC->PDIR & (1<<17))){
					put("Debug Mode\r\n");
					next_state=2;
					break;
				}
				break;
			case 1:
			// 	put("CASE 1");
				read_full_xyz();
				convert_xyz_to_roll_pitch();
			
			
				
			//mid point cali
				middle_point=0;
			
			//read cameras
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
				right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
				
			//setup dead zone

		
			//Series of checking
			//out of track STOP!
				if (left_track>100 && right_track>100){
					left_PW=60000;
					right_PW=0;
				//	put("STOP!");
					continue;
				}
				
				
				//where is middle point
				middle_point=middle_point+right_track-left_track;
				
			//accel
				//hill!!
				if (pitch >30){
					servo_PW=_servo_limit(4500-(middle_point)*20);
					Control_RGB_LEDs(0, 0, 1);
					left_PW = 40000;
					right_PW = 20000;
					continue;
				}

				if (middle_point<15 && middle_point>-15){
					servo_PW=_servo_limit(4500-(middle_point-15)*3);
				//	if(turn_flag){
					//		turn_flag=0;
							left_PW = 15000;
							right_PW = 45000;
							//}
					//	else{
						//left_PW = _motor_limit(left_PW-3,0);
						//right_PW = _motor_limit(right_PW+3,1);
						//}
				}
			 else	if (middle_point<60 && middle_point>-60){
				  turn_flag=1;
					servo_PW=_servo_limit(4500-(middle_point-15)*30);
				 // left_PW = 25000;
					//right_PW = 35000;
				  left_PW = _motor_limit(30000-90*(middle_point),0);
					right_PW = _motor_limit(30000-90*(middle_point),1);
				}
				else{
					turn_flag=1;
					servo_PW=_servo_limit(4500-30*60*(middle_point < 0? -1:1)-(middle_point-60)*65);
					left_PW = _motor_limit(30000-90*(middle_point),0);
					right_PW = _motor_limit(30000-90*(middle_point),1);
				}
			
				//analyze and control car
				next_state = 1;
				break;
			case 2:		//debug mode
				_DEBUG_running();
				break;
			default:
				next_state=0;
	}
	}
}




void _DEBUG_running(){
		int midpoint;
		char keyIn;
		//left_PW=10000;
	//	right_PW=50000;
/*		
			//ADC conversion and read value
	while((FPTC->PDIR & (1<<13))) {			// if users press SW1, this loop will be ended.
	POT1 = Read_ADC(0xD);
	POT2 = Read_ADC(0xC);
	left_PW = dutyCycle(POT1);
	right_PW = dutyCycle(POT2);
	FB1_sum=FB1_sum+left_fb;
	FB2_sum=FB2_sum+right_fb;
		if (POT_COUNT_DOWN<=0){
			put("PB1/FB1:  ");
			//translator(FB1_sum>>13);
			translator(left_fb);
	    put("/");
			translator_4(left_PW);
			
			put("  PB2/FB2:");
	    //translator(FB2_sum>>13);
			translator(right_fb);
	    put("/");
    	translator_4(right_PW);
			POT_COUNT_DOWN=8192;
			put("\r\n");
			
			FB1_sum=0;
			FB2_sum=0;
		}
		POT_COUNT_DOWN=POT_COUNT_DOWN-1;
	//calculate duty cycle
	}
*/

		if (Camera_DONE==1){
		 DEBUG_print_track(buffer[0][1-buffer_sel]);
		 DEBUG_print_track(buffer[1][1-buffer_sel]);
			put("\r\n");
		 //DEBUG_print_double_camera(buffer[0][1-buffer_sel],buffer[1][1-buffer_sel]);
		//uart0_putchar(0x00);
		//DEBUG_print_camera(buffer[0][1-buffer_sel]);
		//DEBUG_print_camera(buffer[1][1-buffer_sel]);
			
			//int left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
	    //int right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
			
		//DEBUG_print_midpoint(buffer[0][1-buffer_sel]);
		//DEBUG_print_midpoint(buffer[1][1-buffer_sel]);
		//	translator(SINGLE_TRACK_ANY(buffer[0][1-buffer_sel]));
		// translator(SINGLE_TRACK_ANY(buffer[1][1-buffer_sel]));	
		//SINGLE_TRACK_ANY(buffer[0][1-buffer_sel]);
		//SINGLE_TRACK_ANY(buffer[1][1-buffer_sel]);			
	//	midpoint=128+SINGLE_TRACK_ANY(buffer[0][1-buffer_sel])+SINGLE_TRACK_ANY(buffer[1][1-buffer_sel]);
	//	midpoint=(midpoint>>1)-64;
	//	PW3=(int)(((midpoint*3000)>>7)+3000);
	//		put("\r\n\r");
	/*	Camera_DONE=0;		
		}
		if (UART0->D == 'p')  //if user input p, enter menu
		{
		  Stop_PIT();
			
			if (buffer_sel){
				put("\r\nPing buffer\r\n");
			}else{
				put("\r\nPong buffer\r\n");
			}
			DEBUG_print_camera(buffer[0][1-buffer_sel]);
			put("\r\n This is data from Camera2 \r\n");
			DEBUG_print_camera(buffer[1][1-buffer_sel]);
			put("\r\n");
			translator(left_fb);
	    put("  FB1  ");
	    translator(right_fb);
			put("  FB2  ");
			
			put("\r\nEnter 'c' to continue or 'q'to quit\r\n");
			
			keyIn=uart0_getchar();
		  while(!keyIn=='c' && !keyIn=='q'){
				keyIn = uart0_getchar();
    	}
			if (keyIn == 'c') {
				Start_PIT();
				keyIn = 0;
			}
*/
}

	
	}

