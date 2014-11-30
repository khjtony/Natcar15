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
#define buffer_ceil 128

void init_ADC0(void);
volatile unsigned int tick = 0;
volatile unsigned short PW1 = 30000;					//set initial value for PW1 and PW2
volatile unsigned short PW2 = 30000;
void translator(char keyIn);  //a translator to convert input value to char
char keyOut[2];  //output buffer
int current_read;
int max=0;  //set up minimum max value
int min=0xff;  //set up maximum min value



void Init_ADC(void) {
	
	init_ADC0();			// initialize and calibrate ADC0
	ADC0->CFG1 = (ADLPC_LOW | ADIV_1 | ADLSMP_LONG | MODE_8 | ADICLK_BUS_2);	// 8 bit, Bus clock/2 = 12 MHz
	ADC0->SC2 = 0;		// ADTRG=0 (software trigger mode)
}


// calculate the duty cycle
unsigned int dutyCycle(char POT) {
	int PW;
	PW = 60000 * POT / 0xFF;
	
	//check overflow
	if (PW>=60000){
	PW = 59999;
	}
	return PW;
}

void put(char *ptr_str)  //copied put function
{
	while(*ptr_str)
		uart0_putchar(*ptr_str++);
}

unsigned int Read_ADC (int port) {
	volatile unsigned int res=0;
	
	ADC0->SC1[0] = port; 			// start conversion (software trigger) on AD12 i.e. ADC0_SE12 (PTB2)
	
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {	; }		// wait for conversion to complete (polling)

	res = ADC0->R[0];				// read result register
	return res;
}

void TPM1_IRQHandler(void) {
//clear pending IRQ
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	
// clear the overflow mask by writing 1 to TOF
	
		TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
		TPM1->CONTROLS[0].CnV = PW1;
	tick++;
	if (tick % 50 == 0) {
		if (PW1 == 3000 || PW1 == 6000)  {
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

void Init_PWM(void) {

// Set up the clock source for MCGPLLCLK/2. 
// See p. 124 and 195-196 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
// TPM clock will be 48.0 MHz if CLOCK_SETUP is 1 in system_MKL25Z4.c.
	
	SIM-> SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	
// See p. 207 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; // Turn on clock to TPM0


// See p. 163 and p. 183-184 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	PORTC->PCR[1] = PORT_PCR_MUX(4); // Configure PTC1 as TPM0_CH0
	PORTC->PCR[3] = PORT_PCR_MUX(4); // Configure PTC1 as TPM0_CH2

// Set channel TPM0_CH0 to edge-aligned, high-true PWM
	
	TPM0->CONTROLS[0].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	TPM0->CONTROLS[2].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	
// Set period and pulse widths
	
	TPM0->MOD = 60000-1;		// Freq. = (48 MHz / 16) / 3000 = 1 kHz
	TPM0->CONTROLS[0].CnV = PW1; 
	TPM0->CONTROLS[2].CnV = PW2;
	
	
// set TPM0 to up-counter, divide by 16 prescaler and clock mode
	
	TPM0->SC = (TPM_SC_CMOD(1) | TPM_SC_PS(4));
	
// clear the overflow mask by writing 1 to TOF
	
	if (TPM0->SC & TPM_CnSC_CHF_MASK) TPM1->SC |= TPM_CnSC_CHF_MASK;

// Enable Interrupts

	



// See p. 207 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; // Turn on clock to TPM1


// See p. 163 and p. 183-184 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	PORTB->PCR[0] = PORT_PCR_MUX(3); // Configure PTB1 as TPM1_CH1

// Set channel TPM1_CH1 to edge-aligned, high-true PWM
	
	TPM1->CONTROLS[0].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	
// Set period and pulse widths
	
	TPM1->MOD = 60000-1;		// Freq. = (48 MHz / 16) / 3000 = 1 kHz
	TPM1->CONTROLS[0].CnV = PW1; 	
	
// set TPM1 to up-counter, divide by 16 prescaler and clock mode
	
	TPM1->SC = (TPM_SC_CMOD(1) | TPM_SC_PS(4));
	
// clear the overflow mask by writing 1 to TOF
	
	if (TPM1->SC & TPM_CnSC_CHF_MASK) TPM1->SC |= TPM_CnSC_CHF_MASK;

// Enable Interrupts

	NVIC_SetPriority(TPM1_IRQn, 192); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM1_IRQn); 
	NVIC_EnableIRQ(TPM1_IRQn);	
	NVIC_SetPriority(TPM0_IRQn, 192); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);
}	

	
/*------------------ ----------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	unsigned char POT1; 
	unsigned char POT2;
//	char unsigned buffer_gate[buffer_ceil];
	int uart0_clk_khz;
	
  char keyIn;
	int i=0;
	int threshold;
	int average=0;
	char welcome[]="Lab 2a\r\nEnter 'p' to print buffer\r\n\0";
//	int slope;
//	int limit=2;
	
	SIM->SCGC5    |= (SIM_SCGC5_PORTB_MASK
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
	FPTC->PDOR |= 1UL<<4;																	// initialize PTC2
	FPTC->PDDR |= 1UL<<4;
	
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

	uart0_clk_khz = (48000000 / 1000); // UART0 clock frequency will equal half the PLL frequency	
	uart0_init (uart0_clk_khz, TERMINAL_BAUD);// configure PTE1 as output
	PORTC->PCR[17] |= PORT_PCR_MUX(1);
	PORTC->PCR[13] |= PORT_PCR_MUX(1);
	FPTC->PDDR &= ~(1UL<<17);
	FPTC->PDDR &= ~(1UL<<13);
	
	put("\r\nPlease press SW2 to start program.\r\n");
	while(!(FPTC->PDIR & (1<<17))) {}			// wait for user pressing SW2
	Init_ADC();
	Init_PWM();
	original_CFG2=ADC0 -> CFG2;
	FPTE->PSOR = 1UL<<21;
	put("\r\nPlease set motor speed \r\n");


	//ADC conversion and read value
	while(!(FPTC->PDIR & (1<<13))) {			// if users press SW1, this loop will be ended.
	POT1 = Read_ADC(0xD);
	POT2 = Read_ADC(0xC);
	  translator(POT1);
	uart0_putchar(keyOut[0]);
  uart0_putchar(keyOut[1]);
	put("  POT1  ");
	translator(POT2);
	uart0_putchar(keyOut[0]);
  uart0_putchar(keyOut[1]);
	put("  POT2  ");
	//calculate duty cycle
	PW1 = dutyCycle(POT1);
	PW2 = dutyCycle(POT2);
	}
	uart0_putchars(welcome);
	
	Init_PIT(10000);																		// count-down period = 100HZ
	
	Start_PIT();
	
	while (1){   //Big while looping
  while (!uart0_getchar_present()) {  //if no input detected, print of data analysis
	  max=0;
	  min=0xff;
		i=0;
		average=0;
		//keyIn=0;
		if(DONE==1){   
      for (i=10;i<buffer_ceil-10;i++){
				if (buffer[0][1-buffer_sel][i]>max){
					max=buffer[0][1-buffer_sel][i];
				}else if(buffer[0][1-buffer_sel][i]<min){
					min=buffer[0][1-buffer_sel][i];
				}
			}
			//if done, begin analyze data
		  //Method one, voltage method			
      i=0;
			threshold=(int)((max-min)/2.5);
			for (i=0;i<buffer_ceil;i++){
				if (buffer[0][1-buffer_sel][i]>=threshold){
					//buffer_gate[i]=1;
					uart0_putchar('X');
				}else{
					//buffer_gate[i]=0;
					uart0_putchar(' ');
				}
			}
			//put(" \r\r");
			
			
			max=0;
	    min=0xff;
      i=0;
			for (i=10;i<buffer_ceil-10;i++){
				if (buffer[1][1-buffer_sel][i]>max){
					max=buffer[1][1-buffer_sel][i];
				}else if(buffer[1][1-buffer_sel][i]<min){
					min=buffer[1][1-buffer_sel][i];
				}
			}
			threshold=(int)((max-min)/3);
			i=0;
			for (i=0;i<buffer_ceil;i++){
				if (buffer[1][1-buffer_sel][i]>=threshold){
					//buffer_gate[i]=1;
					uart0_putchar('X');
				}else{
					//buffer_gate[i]=0;
					uart0_putchar(' ');
				}
			}
			
			put("\r\n\r");
			DONE=0;		
			}
		}
		if (UART0->D == 'p')  //if user input p, enter menu
		{
		  Stop_PIT();
			i=buffer_index;
			
			if (buffer_sel){
				put("\r\nPing buffer\r\n");
			}else{
				put("\r\nPong buffer\r\n");
			}
			
			i=0;
			for(i=0;i<buffer_ceil;i++){
				translator(buffer[0][1-buffer_sel][i]);
			  uart0_putchar(keyOut[0]);
				uart0_putchar(keyOut[1]);
				uart0_putchar(' ');
			}
			put("\r\n This is data from Camera2 \r\n");
			i=0;
			for(i=0;i<buffer_ceil;i++){
				translator(buffer[1][1-buffer_sel][i]);
			  uart0_putchar(keyOut[0]);
				uart0_putchar(keyOut[1]);
				uart0_putchar(' ');
			}
			
			put("\r\n");
			translator(FB1);
	    uart0_putchar(keyOut[0]);
      uart0_putchar(keyOut[1]);
	    put("  FB1  ");
	    translator(FB2);
	    uart0_putchar(keyOut[0]);
      uart0_putchar(keyOut[1]);
			put("/");
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
	
}

/*----------------------------------------------------------------------------
  translater function
 *----------------------------------------------------------------------------*/

void translator(char keyIn){
	
	int part1=keyIn&0xF0;
	int part2=keyIn&0x0F;
	part1=part1>>4;
	keyOut[0]=hex[part1];
	keyOut[1]=hex[part2];
}

