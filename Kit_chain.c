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



/*
int SINGLE_TRACK_ANY(char unsigned *buffer){
	int i=0;
	int j=0;
	long int max=0;
	long int min=16*0xff;
	long int diff;
	int index_min;
	int left_bound=0;
	int right_bound=128;
	int position;
	long int bright_avg[TRACK_ANY_ELE];
	
	//store all 128 elements in 8 blocks
	for (i=0;i<TRACK_ANY_GRP;i++){
		bright_avg[i]=0;
		for (j=0;j<TRACK_ANY_ELE;j++){
		  bright_avg[i]=buffer[i*8+j]+bright_avg[i];
		}
		j=0;
	}
	
	//find largest value
	i=0;
	for (i=0;i<TRACK_ANY_GRP;i++){
	  if (bright_avg[i]>max){
			max=bright_avg[i];
		}
		if (bright_avg[i]<min){
			min=bright_avg[i];
			index_min=i;
		}
		diff=max-min;
		diff=diff>>1;
		
		if (index_min>0 || index_min <6){
			//find left bound
			if (bright_avg[i-1]<(diff+min)){
				left_bound=index_min*TRACK_ANY_ELE-(TRACK_ANY_ELE>>1)-(TRACK_ANY_ELE>>2);
		}
			else{
			  left_bound=index_min*TRACK_ANY_ELE-(TRACK_ANY_ELE>>1)+(TRACK_ANY_ELE>>2);
			}
			
			//find right bound
			if (bright_avg[i+1]<(diff+bright_avg[i])){
				if(bright_avg[i+2]<(diff+bright_avg[i])){
				  left_bound=(index_min+1)*TRACK_ANY_ELE+(TRACK_ANY_ELE>>1)+(TRACK_ANY_ELE>>2);
		    }
				else{
				left_bound=(index_min+1)*TRACK_ANY_ELE+(TRACK_ANY_ELE>>1)-(TRACK_ANY_ELE>>2);
		    }
			left_bound=(index_min)*TRACK_ANY_ELE+(TRACK_ANY_ELE>>1)+(TRACK_ANY_ELE>>2);
		  }
			else{
			  left_bound=index_min*TRACK_ANY_ELE+(TRACK_ANY_ELE>>1)-(TRACK_ANY_ELE>>2);
			}
			
		  position=(right_bound+left_bound)>>1;
			
		}
    else{
		if (index_min==0){
		  position=TRACK_ANY_ELE>>1;
		}
		else{
		  position=index_min*TRACK_ANY_ELE+(TRACK_ANY_ELE>>1);
		}
		}
		}
	return position;
}
*/		

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




