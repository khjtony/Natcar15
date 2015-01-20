#include <MKL25Z4.H>
#include "Kit_chain.h"
int left=0;
int right = 0;


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

void DEBUG_print_camera(char unsigned *buffer){
  int i=0;    
	for(i=0;i<buffer_ceil;i++){
		translator(buffer[i]);
		uart0_putchar(' ');
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



int SINGLE_TRACK_SIDE(char unsigned *buffer){
  int i=0;
	int threshold=0x40;   
	int bound;
	int tempSum=0;
	
 	i=9;
	for(i=5;i<127-5;i++){
		tempSum=buffer[i-2]+buffer[i-1]+buffer[i]+buffer[i+1]+buffer[i+2];
		tempSum=tempSum>>2;
		if (tempSum>threshold){
		return i;
		}
}
	return 127;
}



