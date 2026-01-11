#ifndef __UART_H 
#define __UART_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

#define BOUND				115200				//定义UART1串口的波特率

extern void UART_Init(void);								//声明串口1的初始化函数,1字节1中断玩法
extern void UART_Putc(u8 c);								//声明串口1发送字符函数
extern void UART_Puts(u8 *pstr);						//声明串口1发送字符串函数
extern u8 UART_Getc(void);									//声明串口1获取字符函数
extern void UART_Gets(u8 *buf, u32 len);		//声明串口1接收字符串函数

extern void UART_IDLEInit(void);						//声明串口1的初始化函数,1帧1中断玩法
#endif
