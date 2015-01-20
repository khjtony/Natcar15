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


volatile unsigned int tick = 0;
unsigned char POT1; 
unsigned char POT2;

void translator(char keyIn);  //a translator to convert input value to char
int current_read;




void _zergling(car_control* myCar){
	int adjust_base;
	int dead_zone=50;
	int left_track=SINGLE_TRACK_SIDE(buffer[0][1-buffer_sel]);
	int right_track=SINGLE_TRACK_SIDE(buffer[1][1-buffer_sel]);
	adjust_base=left_track<right_track? left_track:right_track;
	if(adjust_base<dead_zone && left_track<right_track){
		PW3=4500+(1500/dead_zone)*(dead_zone-adjust_base);
	}
	else{
		PW3=4500-(1500/dead_zone)*(dead_zone-adjust_base);
	}
	myCar->left_speed=5000;
	myCar->right_speed=5000;
	return;
}
void _nest(car_control* myCar){
	PW1=myCar->left_speed;
	PW2=myCar->right_speed;
	PW3=myCar->direction;
	return;
}











void TPM1_IRQHandler(void) {
//clear pending IRQ
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	
// clear the overflow mask by writing 1 to TOF
	
		TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
		TPM1->CONTROLS[0].CnV = PW3;
	tick++;
	if (tick % 50 == 0) {
		if (PW3 == 3000 || PW3 == 6000)  {
			FPTB->PTOR = led_mask[0];
			FPTB->PSOR = led_mask[1];
		}
		else {
		FPTB->PSOR = led_mask[0];
		FPTB->PTOR = led_mask[1];
		}
		
	}
}
	
void TPM0_IRQHandler(void) {
//clear pending IRQ
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	
// clear the overflow mask by writing 1 to TOF
// set PW value
		TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
		TPM0->CONTROLS[0].CnV = PW1;
		TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK;
		TPM0->CONTROLS[2].CnV = PW2;
		
	}


	
/*-----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint8_t next_state=0;
	car_control myCar;
	int uart0_clk_khz;
	
	
	PW1=0;
	PW2=0;
	PW3=4500;
	
	
	

	
	SIM->SCGC5  |= (SIM_SCGC5_PORTB_MASK
                  |	SIM_SCGC5_PORTC_MASK
									| SIM_SCGC5_PORTD_MASK
	                | SIM_SCGC5_PORTE_MASK);      		// Enable Clock to Port B & D 
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
	
	PORTA->PCR[1] |= PORT_PCR_MUX(0x2);		// Enable the UART0_RX function on PTA1
	PORTA->PCR[2] |= PORT_PCR_MUX(0x2);		// Enable the UART0_TX function on PTA2
	
	//Lab7 ADC PTE22 PTE 23
	PORTE->PCR[22] |= PORT_PCR_MUX(0x0);		// Enable the UART0_RX function on PTA1
	PORTE->PCR[23] |= PORT_PCR_MUX(0x0);		// Enable the UART0_TX function on PTA2
	
	PORTC->PCR[2] |= (1UL << 8);												// Pin PTC2 is GPIO
	FPTC->PDOR |= 1UL<<2;																	// initialize PTC2
	FPTC->PDDR |= 1UL<<2;
	FPTC->PCOR &= 0UL<<2;
	FPTC->PDOR |= 1UL<<4;																	// initialize PTC4
	FPTC->PDDR |= 1UL<<4;
	FPTC->PCOR &= 0UL<<4;
	
	PORTE->PCR[21] = (1UL << 8);												// Pin PTE21 is GPIO
	FPTE->PDOR |= 1UL<<21;																	// initialize PTE21
	FPTE->PDDR |= 1UL<<21;
	FPTE->PCOR = 1UL<<21;
	
	PORTB->PCR[0] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTB->PDOR |= 1;																	// initialize PTB0
	FPTB->PDDR |= 1;																	// configure PTB0 as output
	
	PORTD->PCR[7] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTD->PDOR |= 1<<7;																	// initialize PTD7
	FPTD->PDDR |= 1<<7;																	// configure PTD7 as output
	
	PORTE->PCR[1] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTE->PDOR |= 1<<1;																	// initialize PTE1
	FPTE->PDDR |= 1<<1;			


  FPTE->PSOR |= 1UL<<21;



	uart0_clk_khz = (48000000 / 1000); // UART0 clock frequency will equal half the PLL frequency	
	uart0_init (uart0_clk_khz, TERMINAL_BAUD);// configure PTE1 as output
	PORTC->PCR[17] |= PORT_PCR_MUX(1);
	PORTC->PCR[13] |= PORT_PCR_MUX(1);
	FPTC->PDDR &= ~(1UL<<17);
	FPTC->PDDR &= ~(1UL<<13);
	
	
	Init_ADC();
	Init_PWM_motor();
  Init_PWM_servo();
	Init_PIT(10000);																		// count-down period = 100HZ
	Start_PIT();
	
	//Application start
	//Enter state machine
	next_state=0;
	while(1){
		switch (next_state){
			case 0:
				Start_PIT();	//PIT only read and update data but not control anything (under change)
				myCar.direction=4500;
				myCar.left_speed=0;
				myCar.right_speed=0;
			  _nest(&myCar);
			//	puts("Hello World");
				//if input is SW1 enter running mode state 1
				if ((FPTC->PDIR & (1<<13))){
				next_state=1;
				}
				//if input is sw2 enter debug mode state 2
				if ((FPTC->PDIR & (1<<17))){
				next_state=2;
				}
				break;
			case 1:
				_zergling(&myCar);
				_nest(&myCar);
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
		
			//ADC conversion and read value
	while((FPTC->PDIR & (1<<13))) {			// if users press SW1, this loop will be ended.
	POT1 = Read_ADC(0xD);
	POT2 = Read_ADC(0xC);
	PW1 = dutyCycle(POT1);
	PW2 = dutyCycle(POT2);
	FB1_sum=FB1_sum+FB1;
	FB2_sum=FB2_sum+FB2;
		if (POT_COUNT_DOWN<=0){
			put("PB1/FB1:  ");
			//translator(FB1_sum>>13);
			translator(FB1);
	    put("/");
			translator_4(PW1);
			
			put("  PB2/FB2:");
	    //translator(FB2_sum>>13);
			translator(FB2);
	    put("/");
    	translator_4(PW2);
			POT_COUNT_DOWN=8192;
			put("\r\n");
			
			FB1_sum=0;
			FB2_sum=0;
		}
		POT_COUNT_DOWN=POT_COUNT_DOWN-1;
	//calculate duty cycle
	}
		
		if (DONE==1){
		 DEBUG_print_track(buffer[0][1-buffer_sel]);
		 DEBUG_print_track(buffer[1][1-buffer_sel]);
		 put("\r\n\r");
			
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
		DONE=0;		
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
			translator(FB1);
	    put("  FB1  ");
	    translator(FB2);
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
		}
	}

