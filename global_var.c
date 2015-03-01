#include "MKL25Z4.h"
#include "global_var.h"


const uint32_t led_mask[] = {1UL << 18, 1UL << 19, 1UL << 1};
const uint32_t camera_mask[]={1UL << 5, 1UL << 7, 1UL << 1};   //a new mask for camera
volatile char unsigned buffer[2][2][buffer_ceil];
volatile int unsigned buffer_index=0;
volatile int buffer_sel=0;
volatile int buffer_cam=0;
int CLK;
volatile int Camera_DONE=0;
int ADC_sel=0x6;
volatile unsigned long left_fb=0;
volatile unsigned long right_fb=0;
volatile unsigned short ADC_FLG=0;
char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};  // dictionary for hex transformation
volatile unsigned int left_PW = 0;					//set initial value for PW1 and PW2
volatile unsigned int right_PW = 0;
volatile unsigned int servo_PW = 4500;
long int FB1_sum=0;
long int FB2_sum=0;
char* PW_chars;
int POT_COUNT_DOWN=8192;
int dummy_time=0;

