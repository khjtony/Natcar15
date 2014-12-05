#include "MKL25Z4.h"
#include "timers.h"
#include "stdio.h"
#include "global_var.h"
#include "Kit_chain.h"

volatile unsigned PIT_interrupt_counter = 0;
unsigned char val;



#define LED_RED    0
#define LED_GREEN  1
#define LED_BLUE	 2




void buffer_sel_toggle(){
	buffer_sel=1-buffer_sel;
}

void buffer_cam_toggle(){
	buffer_cam=1-buffer_cam;
}


char unsigned get_buffer(int i){
	if (i>=buffer_index){
		return NULL;
	}
	return buffer[buffer_cam][1-buffer_sel][i];
		
}



void LED_Initialize(void) {
  PORTB->PCR[18] = (1UL <<  8);                      /* Pin PTB18 is GPIO */
  PORTB->PCR[19] = (1UL <<  8);                      /* Pin PTB19 is GPIO */
  PORTD->PCR[1]  = (1UL <<  8);                      /* Pin PTD1  is GPIO */ 
	
  FPTB->PDOR |= (led_mask[0] | led_mask[1] );          /* switch Red/Green LED off  */
  FPTB->PDDR |= (led_mask[0] | led_mask[1]);          /* enable PTB18/19 as Output */

  FPTD->PDOR |= led_mask[2];            /* switch Blue LED off  */
  FPTD->PDDR |= led_mask[2];            /* enable PTD1 as Output */

}




void ADC0_IRQHandler(void){
	if (ADC_FLG==0){
	//ADC_1.ASSERT CLK
	if(buffer_cam){
		FPTE->PSOR=camera_mask[2];  //assert CLK signal (i.e. set PTE1 high)
	}
	
	//ADC_2.rad value/clear flag
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	val = ADC0->R[0];				// read result register
		
	

	
	//ADC_3.store the value from ADC

	if (buffer_index > buffer_ceil-1) {
	  buffer_index = 0;
		buffer_sel_toggle();
		DONE=1;
	}
	else{
	  buffer[buffer_cam][buffer_sel][buffer_index] = val;
	  buffer_index=buffer_index+buffer_cam;
		
		ADC_toggle();
		
	//  ADC0->SC1[0] |= AIEN_ON| DIFF_SINGLE | ADC_SC1_ADCH(ADC_sel);  //start conversion on channel SE7b(PTD7)
		buffer_cam_toggle();
		
	  ADC_FLG=0;
		ADC0->SC1[0] = ADC_sel |1UL<<6; 
	}
	//ADC_4.increment CLK counter
	CLK=CLK+1;
	
	
	
	
	
	//ADC_5.if and else
	if(buffer_index<buffer_ceil+1){
		//Delay(120) //ns
		//startover ADC
		//start conversion
		
		
	}else{
	//	DONE=1;
			
		//Lower GPIO
		FPTB->PCOR = 1;
		CLK=1;
	}
	
	//ADC_6.Deasser CLK
	FPTE->PCOR=camera_mask[2];  //deassert CLK signal (i.e. set PTE1 low)
	
	//toggle ADC port
	if (DONE){
	ADC_FLG=3;
	ADC_CHa();
	ADC0->SC1[0] = 0x3 |1UL<<6;
	}
	
	
	
	
}
	else if(ADC_FLG==3){
		NVIC_ClearPendingIRQ(ADC0_IRQn);
	  FB1 = ADC0->R[0];				// read result register
	  ADC_FLG=7;
    
	  ADC0->SC1[0] = 0x7 |1UL<<6;
		
	}
	else if(ADC_FLG==7){
		NVIC_ClearPendingIRQ(ADC0_IRQn);
	  FB2 = ADC0->R[0];				// read result register
		ADC_FLG=0;
		ADC_CHb();
	}
		
	
	
	
}



void Init_PIT(unsigned period_us) {
	
	
	
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period_us*24); // 24 MHz clock frequency

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	//	unsigned char test;
 	LED_Initialize(); 
	
	Init_ADC_lab2_part1();
	
	//lower GPIO b0
	FPTB->PCOR=1; 
	
	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}


void Start_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
}

void Stop_PIT(void) {
// Disable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


void PIT_IRQHandler() {
	
	
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		FPTD->PSOR=camera_mask[1];  //Assert SI signal (i.e. set PTD7 high)
	
  	//start measuring indicating pin
	  FPTB->PSOR=1;

	  //clear pending IRQ
	  NVIC_ClearPendingIRQ(PIT_IRQn);
	
	  //wait 20ns
	
  	FPTE->PSOR=camera_mask[2];  //Assert CLK signal (i.e. set PTE1 high)
		
		
		// Do ISR work - move next sample from buffer to DAC
		FPTB->PSOR = 1;							// toggle PTB0
		
		
		
		//delay 120ns;
				
		
		FPTD->PCOR=camera_mask[1];  //deassert SI signal (i.e. set PTD7 low)
		
		//start conversion
		ADC0->SC1[0] = 0x6 |1UL<<6; 
		//ADC0->SC1[0] = 0x7 |1UL<<6; 
		
		FPTE->PCOR=camera_mask[2];  //deassert CLK signal (i.e. set PTE1 low)
		
		
		//read feedback
	
		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	} 
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
