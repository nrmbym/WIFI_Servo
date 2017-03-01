#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern void TIM3_PWM_Init(u16 arr ,u16 psc );
extern void TIM7_Int_Init(u16 arr,u16 psc);
#endif
