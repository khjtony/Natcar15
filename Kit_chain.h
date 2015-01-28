#ifndef __KIT_CHAIN_H__
#define __KIT_CHAIN_H__

#include "PWM_Kit.h"
#include "ADC_Kit.h"
#include "uart.h"
#include "global_var.h"

#define LCD_UPDATE_PERIOD (100)


#define LED_RED    0
#define LED_GREEN  1
#define LED_BLUE	 2


int SINGLE_TRACK_SIDE(char unsigned *buffer);
unsigned int dutyCycle(char POT);
void DEBUG_print_track(char unsigned *buffer);
void DEBUG_print_camera(char unsigned *buffer);
int SINGLE_TRACK_ANY(char unsigned *buffer);
int DEBUG_print_midpoint(char unsigned *buffer);
void translator_4(int keyIn);
void _DEBUG_running(void);
int _servo_limit(int input);

#endif
