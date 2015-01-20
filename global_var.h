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

extern const uint32_t led_mask[];
extern const uint32_t camera_mask[];   //a new mask for camera
extern char unsigned buffer[2][2][buffer_ceil];
extern int unsigned buffer_index;
extern int buffer_sel;
extern int buffer_cam;
extern int CLK;
extern int DONE;
extern int ADC_sel;
extern unsigned long FB1;
extern unsigned long FB2;
extern unsigned int original_CFG2;
extern unsigned short ADC_FLG;
extern char hex[];
extern volatile unsigned int PW1;					//set initial value for PW1 and PW2
extern volatile unsigned int PW2;
extern volatile unsigned int PW3;
extern char* PW_chars;
extern int POT_COUNT_DOWN;
extern int dummy_time;
extern long int FB1_sum;
extern long int FB2_sum;
extern long int state_machine_timer;
extern unsigned char POT1;
extern unsigned char POT2;

#endif
