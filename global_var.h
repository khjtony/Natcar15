#ifndef __GLOBAL_VAR_H__
#define __GLOBAL_VAR_H__
#include "MKL25Z4.h"
#define buffer_ceil 128

#define TRACK_ANY_GRP 8
#define TRACK_ANY_ELE 16



typedef struct car_control_t{
	int left_speed;
	int right_speed;
	int direction;
} car_control;

extern volatile char unsigned buffer[2][2][buffer_ceil];
extern volatile int unsigned buffer_index;
extern volatile int buffer_sel;
extern volatile int buffer_cam;
extern int CLK;
extern volatile int Camera_DONE;
extern int ADC_sel;
extern volatile unsigned long left_fb;
extern volatile unsigned long right_fb;

extern volatile unsigned short ADC_FLG;
extern char hex[];
extern volatile unsigned int left_PW;					//set initial value for PW1 and PW2
extern volatile unsigned int right_PW;
extern volatile unsigned int servo_PW;
extern char* PW_chars;
extern int POT_COUNT_DOWN;
extern int dummy_time;
extern long int FB1_sum;
extern long int FB2_sum;
extern long int state_machine_timer;
extern unsigned char POT1;
extern unsigned char POT2;
extern volatile int8_t current_roll;
extern volatile uint8_t right_FB;
extern volatile uint8_t left_FB;
extern volatile uint8_t _right_FB;
extern volatile uint8_t _left_FB;

#endif
