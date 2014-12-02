#include <MKL25Z4.H>
#include "Kit_chain.h"


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


void DEBUG_print_track(char unsigned *buffer){
    int max=0;
	  int min=0xff;
		int i=0;
	  int threshold=0;
		//keyIn=0;
		if(DONE==1){   
      for (i=10;i<buffer_ceil-10;i++){
				if (buffer[i]>max){
					max=buffer[i];
				}else if(buffer[i]<min){
					min=buffer[i];
				}
			}
			//if done, begin analyze data
		  //Method one, voltage method			
      i=0;
			threshold=(max-min)>>1;
			for (i=0;i<buffer_ceil;i++){
				if (buffer[i]>=threshold){
					//buffer_gate[i]=1;
					uart0_putchar('X');
				}else{
					//buffer_gate[i]=0;
					uart0_putchar(' ');
				}
			}
		}
		
}

