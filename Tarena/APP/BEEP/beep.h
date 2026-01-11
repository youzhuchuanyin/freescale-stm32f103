#ifndef __BEEP_H 
#define __BEEP_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

//声明操作函数
extern void BEEP_Init(void);		//初始化函数
extern void BEEP_On(void);			//打开蜂鸣器函数
extern void BEEP_Off(void);			//关闭蜂鸣器函数

//定义BEEP的位带操作
#define BEEP			PBout(8)

#endif
