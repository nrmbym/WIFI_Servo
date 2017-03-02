#ifndef __SERVO_H
#define __SERVO_H
#include "sys.h"


extern  u16 compare1_value;
extern u16 compare2_value;
extern void Servo_Init(void);         //初始化舵机偏转角度
extern void Rotate(void);                       //改变舵机角度函数
#endif
