#include <MKL25Z4.H>
#include "PWM_Kit.h"

void Init_PWM_motor(void) {

// Set up the clock source for MCGPLLCLK/2. 
// See p. 124 and 195-196 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
// TPM clock will be 48.0 MHz if CLOCK_SETUP is 1 in system_MKL25Z4.c.
	
	SIM-> SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	
// See p. 207 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; // Turn on clock to TPM0


// See p. 163 and p. 183-184 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	PORTC->PCR[1] = PORT_PCR_MUX(4); // Configure PTC1 as TPM0_CH0
	PORTC->PCR[3] = PORT_PCR_MUX(4); // Configure PTC3 as TPM0_CH2

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
	NVIC_SetPriority(TPM0_IRQn, 192); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);
}	

void Init_PWM_servo(void) {

// Set up the clock source for MCGPLLCLK/2. 
// See p. 124 and 195-196 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
// TPM clock will be 48.0 MHz if CLOCK_SETUP is 1 in system_MKL25Z4.c.
	
	SIM-> SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	
// See p. 207 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; // Turn on clock to TPM1


// See p. 163 and p. 183-184 of the KL25 Sub-Family Reference Manual, Rev. 3, Sept 2012
	
	PORTB->PCR[0] = PORT_PCR_MUX(3); // Configure PTB1 as TPM1_CH1

// Set channel TPM1_CH1 to edge-aligned, high-true PWM
	
	TPM1->CONTROLS[0].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	
// Set period and pulse widths
	
	TPM1->MOD = 60000-1;		// Freq. = (48 MHz / 16) / 3000 = 1 kHz
	TPM1->CONTROLS[0].CnV = PW3; 	
	
// set TPM1 to up-counter, divide by 16 prescaler and clock mode
	
	TPM1->SC = (TPM_SC_CMOD(1) | TPM_SC_PS(4));
	
// clear the overflow mask by writing 1 to TOF
	
	if (TPM1->SC & TPM_CnSC_CHF_MASK) TPM1->SC |= TPM_CnSC_CHF_MASK;

// Enable Interrupts

	NVIC_SetPriority(TPM1_IRQn, 192); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM1_IRQn); 
	NVIC_EnableIRQ(TPM1_IRQn);	

}	
