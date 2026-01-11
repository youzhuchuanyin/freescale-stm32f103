#ifndef _PWM_H
#define _PWM_H

#include "stm32f10x.h"
#include "system.h"

//20KHz
#define PWM_ARR 99
#define PWM_PSC 35

extern int ccr;

extern void MOTOR_Init(void);
extern void SERVO_Init(void);
extern void SERVO_Test(void);
#endif