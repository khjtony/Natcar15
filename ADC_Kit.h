#ifndef __ADC_KIT_H__
#define __ADC_KIT_H__

#include <MKL25Z4.H>
#include "global_var.h"
#include <stdio.h>
#include "adc0.h"


void Init_ADC(void);
unsigned int Read_ADC (int port);
void ADC_toggle(void);
void Init_ADC_lab2_part1(void);
void translator(char keyIn);
void DEBUG_print_camera(char unsigned *buffer);
void ADC_CHa(void);
void ADC_CHb(void);

#endif

