#ifndef __TIMER4_H
#define __TIMER4_H

#include "stm32f10x.h"
#include "system.h"

//指定定时器4的工作参数：ARR,PSC,实现定时器中断间隔1秒
//产生定时器中断的周期=(PSC+1)*(ARR+1)/f	(f=72MHz)
#define ARR				4999
#define PSC				14399

//指定为500ms

//指定为2s

extern void TIMER4_Init(void);			//定时器4初始化函数
extern void TIMER4_Start(void);			//启动定时器4命令处理函数
extern void TIMER4_Stop(void);			//关闭定时器4命令处理函数

#endif

