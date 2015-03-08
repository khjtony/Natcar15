#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"
#include "adc16.h"

void Init_PIT1(unsigned period_us);
void Start_PIT1(void);
void Stop_PIT1(void);
void Init_PIT(unsigned period_us);
void Start_PIT(void);
void Stop_PIT(void);

void _update_camera(void);
void _update_wheel(int);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
