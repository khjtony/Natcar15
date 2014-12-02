#include <MKL25Z4.H>
#include "IRQHandler.h"


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
	ADC0->SC1[0] = 0x3 |1UL<<6;
	}
	
	
	
	
}
	else if(ADC_FLG==3){
		NVIC_ClearPendingIRQ(ADC0_IRQn);
	  FB1 = ADC0->R[0];				// read result register
	  ADC_FLG=7;
    ADC0 -> CFG2 |= ADC_CFG2_MUXSEL_MASK;
	  ADC0->SC1[0] = 0x7 |1UL<<6;
		
	}
	else if(ADC_FLG==7){
		NVIC_ClearPendingIRQ(ADC0_IRQn);
	  FB2 = ADC0->R[0];				// read result register
		ADC_FLG=0;
		ADC0 -> CFG2=original_CFG2;
	}
}

