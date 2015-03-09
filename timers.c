//this timer file logged handlers

#include "MKL25Z4.h"
#include "timers.h"
#include "stdio.h"
#include "global_var.h"
#include "Kit_chain.h"
#include "./accel/mma8451.h"
#include "./accel/LEDs.h"



volatile unsigned PIT_interrupt_counter = 0;
volatile uint8_t _right_FB;
volatile uint8_t _left_FB;
volatile uint8_t right_FB;
volatile uint8_t left_FB;
volatile uint8_t batter_count;
volatile int8_t LED_flag=0;
void PORTA_IRQHandler(void) {  

	// clear pending interrupts
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	
	if (PORTA->ISFR & (1u<<1)){	//read speed sensor at PTA1
		_right_FB+=1;
		PORTA->ISFR |= 1UL << 1;
	}else if (PORTA->ISFR & (1u<<2)){	//read speed sensor at PTA2
		Control_RGB_LEDs(0,LED_flag,0);
		LED_flag=~LED_flag;
		_left_FB+=1;
		PORTA->ISFR |= 1UL << 2;
	}
	// clear status flags 
	
}


void Init_PIT1(unsigned period_us) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(period_us*24); // 24 MHz clock frequency

	// No chaining
	PIT->CHANNEL[1].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;

}


void Start_PIT1(void) {
// Enable counter
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
}

void Stop_PIT1(void) {
// Disable counter
	PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;
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
 	//LED_Initialize(); 
	
	Init_ADC();
	
	
	//lower GPIO b0
	//FPTB->PCOR=1; 
	
	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 64); // 0, 64, 128 or 192
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
	

	//************************************
	//	 Clear PIT FLAG
  //	 PIT
	//************************************
	  NVIC_ClearPendingIRQ(PIT_IRQn);
		
	//************************************
	//	 Read cameras
  //	 camera
	//************************************
	
	_update_camera();


		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
		//************************************
	//	 Clear PIT FLAG
  //	 PIT
	//************************************
	  NVIC_ClearPendingIRQ(PIT_IRQn);
		get_roll();
		left_FB=_left_FB;
		right_FB=_right_FB;
	//	Battery_ind(_left_FB);
		_left_FB=0;
		_right_FB=0;
		
	} 
}


//*****************************************************************
//ADC
//*****************************************************************


void _update_camera(){
	//************************************
	//	 Using channel b
  //	 Camera
	//************************************
	ADC0 -> CFG2 |= 1UL<<4;		//using ch_b
	
	//************************************
	//	 Assert SI
  //	 SI is PTD7 so FPTD->PSOR=7;
	//************************************
		FPTD->PSOR=1UL<<7;  //Assert SI signal (i.e. set PTD7 high)
		

	//************************************
	//	 Assert CLK signal 
  //	 CLK is PTE1 so FPTE->PSOR=1;
	//***********************************
  	FPTE->PSOR=1UL<<1;  //Assert CLK signal (i.e. set PTE1 high)
	
	
	
	//************************************
	//	 Camera reading indicator
  //	 Camera
	//***********************************
	  FPTB->PSOR=1;

	
	  //wait 20ns
	
	
		
		
	//************************************
	//	 toggle Camera reading Indicator
  //	 Camera. PTB1
	//***********************************
		FPTB->PTOR = 1UL<<1;						
		
		//delay 120ns;
				
	//************************************
	//	 deassert SI
  //	 SI is PTD7 so FPTD->PCOR=7;
	//************************************
		FPTD->PCOR=1UL<<7;  //deassert SI signal (i.e. set PTD7 low)
		
	//************************************
	//	 Start to read Camera 
  //	 Read Camera 0 first to enter ADC handler
	//************************************
		ADC_FLG=0;								//read Camera
		
		ADC0->SC1[0] = 0x6 |1UL<<6; //start to read Camera 0
		
	//************************************
	//	 Dessert CLK signal 
  //	 CLK is PTE1 so FPTE->PCOR=1;
	//***********************************
  	FPTE->PCOR=1UL<<1;  //Dessert CLK signal (i.e. set PTE1 Low)
		
}//update camera data

void _update_wheel(int side){
	if (side==0){	
	//************************************
	//	 Read left Wheel feedback of ADC0_SE7a
  //	 Wheel A
	//***********************************
		ADC_FLG=7;
		ADC0 -> CFG2 &= 0UL<<4;			//use ch.a
		ADC0->SC1[0] = 0x7 |1UL<<6;	//read ADC0_SE7a
		

	}else if(side==1){
	//************************************
	//	 Read right Wheel feedback 
  //	 Wheel
	//***********************************
		ADC_FLG=0;
		ADC0 -> CFG2 &= 0UL<<4;			//use ch.a
		ADC0->SC1[0] = 0x3 |1UL<<6;	//read ADC0_SE3
}
return;

}


void ADC0_IRQHandler(void){
	unsigned char _adc_val;
	//************************************
	//	 assert CLK signal after read Camera 1
  //	 CLK is PTE1 so FPTE->PSOR=1;
	//***********************************
	if(buffer_cam){
		FPTE->PSOR=1UL<<1;  //assert CLK signal (i.e. set PTE1 high)
	}
	
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	
	//************************************
	//	 Read value to val
  //	 Camera 0/1
	//***********************************
	_adc_val = ADC0->R[0];				// read result register
		

	//************************************
	//	 Store val to buffer
  //	 Camera 0/1
	//***********************************
	if (buffer_index > (buffer_ceil-1)) {

	//************************************
	//	 finish to fill both buffer, set Camera_DONE
  //	 Camera 0/1
	//***********************************
	  buffer_index = 0;
		buffer_sel=1-buffer_sel;
		Camera_DONE=1;
	}
	else{
		
	//************************************
	//	 Store _adc_val to specific buffer
  //	 Camera 0/1
	//***********************************
	  buffer[buffer_cam][buffer_sel][buffer_index] = _adc_val;
	  buffer_index=buffer_index+buffer_cam;
		
		
	//************************************
	//	 toggle to read another camera
  //	 Camera 0/1 Read 6/7 so their summation is 13
	//***********************************
		 ADC_sel=13-ADC_sel;
		 buffer_cam=1-buffer_cam;
		
	//************************************
	//	 start to read another camera
  //	 Camera 0/1 Read 6/7 so their summation is 13
	//***********************************
		ADC0->SC1[0] = ADC_sel |1UL<<6; 
	}
	//ADC_4.increment CLK counter
	CLK=CLK+1;

	//************************************
	//	 Dessert CLK signal 
  //	 CLK is PTE1 so FPTE->PCOR=1;
	//***********************************
	FPTE->PCOR=1UL<<1;  //deassert CLK signal (i.e. set PTE1 low)

}
	


// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
