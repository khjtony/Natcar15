// Simple test program using PIT interrupts
// Toggle output on PTB0 at f = 1/(2*100 us) = 5 kHz

#include <MKL25Z4.H>
#include "timers.h"


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	SIM->SCGC5    |= (SIM_SCGC5_PORTB_MASK 
									| SIM_SCGC5_PORTD_MASK);      		// Enable Clock to Port B & D 
	PORTB->PCR[0] = (1UL << 8);												// Pin PTB0 is GPIO
	FPTB->PDOR |= 1;																	// initialize PTB0
	FPTB->PDDR |= 1;																	// configure PTB0 as output
	
	Init_PIT(100);																		// count-down period = 100 us
	
	Start_PIT();
	
	while (1) {
	}
}
