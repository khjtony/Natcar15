#ifndef __KIT_CHAIN_H__
#define __KIT_CHAIN_H__

#include "PWM_Kit.h"
#include "ADC_Kit.h"
#include "uart.h"
#include "global_var.h"



unsigned int dutyCycle(char POT);
void put(char *ptr_str);
void DEBUG_print_track(char unsigned *buffer);
int SINGLE_TRACK_ANY(char unsigned *buffer);
int DEBUG_print_midpoint(char unsigned *buffer);
void translator_4(int keyIn);

#endif
