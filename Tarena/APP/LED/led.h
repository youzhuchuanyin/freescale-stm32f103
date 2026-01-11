//led.h:LED灯操作的各种声明
#ifndef __LED_H
#define __LED_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

//声明操作函数
extern void LED_Init(void);		//初始化函数
extern void LED_On(void);			//开灯函数
extern void LED_Off(void);		//关灯函数
extern void Delay(int n);			//延时函数

//定义LED0,LED1位带操作
#define LED1			PBout(5)
#define LED2			PEout(5)

#endif


