#include <MKL25Z4.H>
#include "ADC_Kit.h"


void Init_ADC(void) {
	
	init_ADC0();			// initialize and calibrate ADC0
	ADC0->CFG1 = (ADLPC_LOW | ADIV_1 | ADLSMP_LONG | MODE_8 | ADICLK_BUS_2);	// 8 bit, Bus clock/2 = 12 MHz
	ADC0->SC2 = 0;		// ADTRG=0 (software trigger mode)
}

unsigned int Read_ADC (int port) {
	volatile unsigned int res=0;
	
	ADC0->SC1[0] = port; 			// start conversion (software trigger) on AD12 i.e. ADC0_SE12 (PTB2)
	
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {	; }		// wait for conversion to complete (polling)

	res = ADC0->R[0];				// read result register
	return res;
}




void ADC_toggle(){
  ADC_sel=13-ADC_sel;
}


void Init_ADC_lab2_part1(void){
	init_ADC0();			// initialize and calibrate ADC0
	ADC0->CFG1 = (ADLPC_NORMAL |  DIFF_SINGLE  | ADIV_2 | ADLSMP_LONG | MODE_8 | ADICLK_BUS_2);	// 8 bit, Bus clock/2 = 12 MHz
	ADC0->CFG2|=ADC_CFG2_MUXSEL_MASK;  //select B channel
	ADC0->SC1[0] |= AIEN_ON| DIFF_SINGLE | ADC_SC1_ADCH(6); //start conversion on channel SE6b(PTD5)
	
	ADC0->SC2 = 0;		// ADTRG=0 (software trigger mode)
	
	
	NVIC_SetPriority(ADC0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(ADC0_IRQn); 
	NVIC_EnableIRQ(ADC0_IRQn);
	
	
}

void ADC_CHa(){
  ADC0 -> CFG2 &= 0x0<<4;
}

void ADC_CHb(){
  ADC0 -> CFG2 |= 0x1<<4;
}

