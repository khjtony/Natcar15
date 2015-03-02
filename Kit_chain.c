#include <MKL25Z4.H>
#include "Kit_chain.h"

void translator(char keyIn){
	
	int part1=keyIn&0xF0;
	int part2=keyIn&0x0F;
	part1=part1>>4;
	uart0_putchar(hex[part1]);
  uart0_putchar(hex[part2]);
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





void DEBUG_print_track(volatile char unsigned *buffer){
    int max=0x80;
	  int min=0x80;
		int i=0;
	  int threshold=0;
		//keyIn=0;   
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
			threshold = ((max-min)>>3) + min;
			for (i=0;i<buffer_ceil;i++){
				if (buffer[i]>=threshold){
					//buffer_gate[i]=1;
					uart0_putchar('*');
				}else{
					//buffer_gate[i]=0;
					uart0_putchar(' ');
				
			}
		}	
}

void DEBUG_print_double_camera(char unsigned *buffer1,char unsigned *buffer2)
{
	int i=0;    
	uart0_putchar(0xff);
	uart0_putchar(0x00);
	uart0_putchar(0xff);  //start sign
  for(i=0;i<buffer_ceil;i++){
		uart0_putchar(buffer1[i]);
	}
	i=0;
	for(i=0;i<buffer_ceil;i++){
		uart0_putchar(buffer2[i]);
	}
}

void DEBUG_print_camera(char unsigned *buffer){
  int i=0;    
	//uart0_putchar(0xff);
	//uart0_putchar(0x00);
	//uart0_putchar(0xff);  //start sign
  for(i=0;i<buffer_ceil;i++){
		if (buffer[i]==0x00){
			uart0_putchar(0x01);
			//translator(0x01);
		}else{
			uart0_putchar(buffer[i]);
	//		translator(buffer[i]);
	}
		
}
	}

int DEBUG_print_midpoint(char unsigned *buffer){
  int midpoint=SINGLE_TRACK_ANY(buffer);
	int i=0;
	for(i=0;i<midpoint;i++){
		uart0_putchar(' ');
	}
	
	i=0;
	
	uart0_putchar('X');
	for(i=0;i<(128-midpoint-1);i++){
		uart0_putchar(' ');
	}
	
	return midpoint;

}

int SINGLE_TRACK_ANY(char unsigned *buffer){
  int max=0;
	int min=0xff;
  int i=0;
	int threshold=0;   
	int left_bound;
	int right_bound;
  for (i=10;i<buffer_ceil-10;i++){
	if (buffer[i]>max){
	  max=buffer[i];
	}else if(buffer[i]<min){
		min=buffer[i];
	}
	}			
  
	threshold=((max-min)>>3)+min;
	i=9;
	while (buffer[i]>threshold && i<127){
		i=i+1;
   
	}
	left_bound=i;
	

	
	i=115;
	while (buffer[i]>threshold && i>0){
		i=i-1;
}
	right_bound=i;



	return ((right_bound+left_bound)>>1);
}





void translator_4(int keyIn){
	
	int part1=keyIn&0xF000;
	int part2=keyIn&0x0F00;
	int part3=keyIn&0x00F0;
	int part4=keyIn&0x000F;
	
	part1=part1>>12;
	part2=part1>>8;
	part3=part1>>4;
	uart0_putchar(hex[part1]);
  uart0_putchar(hex[part2]);
	uart0_putchar(hex[part3]);
  uart0_putchar(hex[part4]);
} 



int SINGLE_TRACK_SIDE(volatile char unsigned *buffer){
  int i=0;
	int threshold=0x40;
	int bound=0;
	int tempSum=0;
	
 	i=9;
	for(i=5;i<127-5;i++){
		//tempSum=buffer[i-1]+buffer[i]+buffer[i+1];
		//tempSum=tempSum/3;
		tempSum=buffer[i];
		if (tempSum<threshold){
			bound++;
		}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	}
	return bound;
}


//void LED_Initialize(void) {
//  PORTB->PCR[18] = (1UL <<  8);                      /* Pin PTB18 is GPIO */
//  PORTB->PCR[19] = (1UL <<  8);                      /* Pin PTB19 is GPIO */
//  PORTD->PCR[1]  = (1UL <<  8);                      /* Pin PTD1  is GPIO */ 
//	
//  FPTB->PDOR |= (led_mask[0] | led_mask[1] );          /* switch Red/Green LED off  */
//  FPTB->PDDR |= (led_mask[0] | led_mask[1]);          /* enable PTB18/19 as Output */
//
//  FPTD->PDOR |= led_mask[2];            /* switch Blue LED off  */
//  FPTD->PDDR |= led_mask[2];            /* enable PTD1 as Output */
//
//}

int _motor_limit(int input,int option){
	if (option==0){
		if (input<60000 && input >30000){
			return input;
		}else if(input>60000){
		return 60000;
		}
		else{
			return 10000;
		}
	}
	else{
		if (input<30000 && input >0){
			return input;
		}else if(input>30000){
		return 50000;
		}
		else{
			return 0;
		}
	}
}


int _servo_limit(int input){
		if (input<6000 && input >3000){
			return input;
		}else if(input>6000){
		return 6000;
		}
		else{
			return 3000;
		}
	}
