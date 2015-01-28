#ifndef __PWM_KIT_H__
#define __PWM_KIT_H__

#include <MKL25Z4.H>
#include "global_var.h"
#include <stdio.h>

void Init_PWM_motor(void);
void Init_PWM_servo(void);
void TPM0_IRQHandler(void);
void TPM1_IRQHandler(void);

#endif
