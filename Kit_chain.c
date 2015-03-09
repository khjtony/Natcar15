#include <MKL25Z4.H>
#include "Kit_chain.h"
#include <stdlib.h>

volatile uint8_t _camera_buffer[128];
void _Mfilter_Camera(volatile uint8_t* buffer);

volatile int8_t current_roll;

volatile int8_t accel_queue[5];
volatile int8_t accel_queue_index=0;
uint8_t left_camera_last=0;
uint8_t right_camera_last=0;

int _compare (const void * a, const void * b)
{
  return ( *(int8_t*)a - *(int8_t*)b );
}


void accel_queue_init(void){
	int i=0;
	for (i=0;i<5;i++){
		accel_queue[i]=0;
	}
}

int8_t get_roll(void){
	int i=0;
	//create temp queue
	int8_t temp_Q[5];
	//copy and pass to Median filter
	for (i=0;i<5;i++){
		temp_Q[i]=accel_queue[i];
	}
	qsort(temp_Q, 5, sizeof(int8_t), _compare); 
	current_roll=temp_Q[2];
	return current_roll;
}



void accel_Q(){
	read_xyz();
	convert_xyz_to_roll();
	//add new value into volatile queue
	accel_queue[accel_queue_index]=roll;
	accel_queue_index++;
	//loop back
	if (accel_queue_index>=5){
		accel_queue_index=0;
	}
	return;
}


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

void DEBUG_print_camera(volatile char unsigned *buffer){
  int i=0;    
	//uart0_putchar(0xff);
	//uart0_putchar(0x00);
	//uart0_putchar(0xff);  //start sign
  for(i=0;i<buffer_ceil;i++){
		if (buffer[i]==0x00 | buffer[i]==0x01){
			uart0_putchar(0x02);
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
	// use _camera_buffer!!
  int i=0;
	int threshold=0x45;
	int bound=0;
	int tempSum=0;
	_Mfilter_Camera(buffer);
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




	


int _motor_limit(int input){
		if (input<= 3000 && input >0){
			return input;
		}else if(input>3000){
		return 2999;
		}
		else{
			return 0;
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

	
void _Mfilter_Camera(volatile uint8_t* buffer){
	int i=0;
	uint8_t temp1;
	uint8_t temp2;
	uint8_t temp3;
	for (i=1;i<127;i++){
		temp1=_camera_buffer[i-1];
		temp2=_camera_buffer[i];
		temp3=_camera_buffer[i+1];
		if (temp1>=temp2 && temp1<=temp3){		//temp1 is mid
			_camera_buffer[i]=temp1;
		}else if(temp1<=temp2 && temp1>=temp3){ 		//temp1 is mid
			_camera_buffer[i]=temp1;
		}else if(temp2>=temp1 && temp2<=temp3){
			_camera_buffer[i]=temp2;
		}else if(temp2<=temp1 && temp2>=temp3){
			_camera_buffer[i]=temp2;
		}else if(temp3>=temp1 && temp3<=temp2){
			_camera_buffer[i]=temp3;
		}else if(temp3<=temp1 && temp3>=temp2){
			_camera_buffer[i]=temp3;
		}
	}
}


int camera_edge_detect(volatile char unsigned *buffer,uint8_t option){
		//drop 7 pixels on the both ends
	int i=0;
	uint8_t max_pos=0;
	uint8_t max_the=0;
	int diff=0;
	unsigned char thereshold=0x3a;
	uint8_t vol_the=SINGLE_TRACK_SIDE(buffer);
	for (i=15;i<128-15;i++){
		diff = buffer[i]>buffer[i+4] ? buffer[i]-buffer[i+4] :  buffer[i+4]-buffer[i] ;
		if (diff > thereshold & diff > max_the){
			max_the=diff;
			max_pos=i+1;
			max_pos=127-max_pos;
		}
	}
	switch (option){
		case 0:		//left
			if (max_pos>=100 || max_pos<=20){
				return vol_the;
			}
			left_camera_last=max_pos;
			break;
		case 1:		//right
			if (max_pos>=100 || max_pos<=20){
				return 127-vol_the;
			}
			right_camera_last=max_pos;
			break;
	}
	return max_pos;
}

void DEBUG_camera_edge_detect(volatile char unsigned *buffer,uint8_t option){
	uint8_t pos=camera_edge_detect(buffer,option);
	if (pos==0x00 || pos==0x01){
			uart0_putchar(0x02);
			//translator(0x01);
		}else{
			uart0_putchar(pos);
	}
		
}

void Battery_ind(uint8_t count){
	switch(count){
		case 0:
			FPTB->PCOR |= 1UL<<8;
			FPTB->PCOR |= 1UL<<9;
			FPTB->PCOR |= 1UL<<10;
			FPTB->PCOR |= 1UL<<11;
			break;
		case 1:
			FPTB->PCOR |= 1UL<<8;
			FPTB->PCOR |= 1UL<<9;
			FPTB->PCOR |= 1UL<<10;
			FPTB->PSOR |= 1UL<<11;
			break;
		case 2:
			FPTB->PCOR |= 1UL<<8;
			FPTB->PCOR |= 1UL<<9;
			FPTB->PSOR |= 1UL<<10;
			FPTB->PSOR |= 1UL<<11;
			break;
		case 3:
			FPTB->PCOR |= 1UL<<8;
			FPTB->PSOR |= 1UL<<9;
			FPTB->PSOR |= 1UL<<10;
			FPTB->PSOR |= 1UL<<11;
			break;
		case 4:
			FPTB->PSOR |= 1UL<<8;
			FPTB->PSOR |= 1UL<<9;
			FPTB->PSOR |= 1UL<<10;
			FPTB->PSOR |= 1UL<<11;
			break;
	}
}



