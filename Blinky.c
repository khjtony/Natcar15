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
	int left_ratio=30;
	int right_ratio=30;
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
	
	
	//************************************
	//	 Initialize
  //	 ADC,TPM,PIT
	//************************************
	right_PW=0;
	left_PW=0;
	servo_PW=4500;
	Init_PIT(10000);																		// count-down period = 100HZ
	Init_PWM_motor();
  Init_PWM_servo();
	Start_PIT();
	
	
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
			case 1:;
				left_PW=42000;
				right_PW=10000;
				
				left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
	      right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
						
				if(left_track<100){
					servo_PW=_servo_limit(4500+(110-left_track)*left_ratio);
					left_PW=38000;
					right_PW=8000;
				}else if(right_track<100){
					servo_PW=_servo_limit(4500-(110-right_track)*right_ratio);
					left_PW=40000;
					right_PW=18000;
				}else{
					servo_PW=_servo_limit(4500);
					left_PW=42000;
					right_PW=10000;
				
				}
			
				//analyze and control car
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
		 //DEBUG_print_track(buffer[0][1-buffer_sel]);
		// DEBUG_print_track(buffer[1][1-buffer_sel]);
			DEBUG_print_camera(buffer[1][1-buffer_sel]);
		 put("\n");
			
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

