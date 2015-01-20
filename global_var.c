#include "MKL25Z4.h"
#include "global_var.h"


const uint32_t led_mask[] = {1UL << 18, 1UL << 19, 1UL << 1};
const uint32_t camera_mask[]={1UL << 5, 1UL << 7, 1UL << 1};   //a new mask for camera
char unsigned buffer[2][2][buffer_ceil];
int unsigned buffer_index=0;
int buffer_sel=0;
int buffer_cam=0;
int CLK;
int DONE=0;
int ADC_sel=0x6;
unsigned long FB1=0;
unsigned long FB2=0;
unsigned int original_CFG2;
unsigned short ADC_FLG=0;
char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};  // dictionary for hex transformation
volatile unsigned int PW1 = 0;					//set initial value for PW1 and PW2
volatile unsigned int PW2 = 0;
volatile unsigned int PW3 = 4500;
long int FB1_sum=0;
long int FB2_sum=0;
char* PW_chars;
int POT_COUNT_DOWN=8192;
int dummy_time=0;

