#ifndef __INIT_H
#define __INIT_H

#include "stm32f10x.h"
#include "system.h"

//声明硬件初始化总入口函数
extern void DEVICE_Init(void);

//声明函数指针数据类型并且取别名叫PINIT_T,将来用它定义的变量来保存各个硬件初始化函数的首地址
typedef void (*PINIT_T)(void);

#endif
