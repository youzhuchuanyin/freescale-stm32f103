#ifndef __UART3_H 
#define __UART3_H

#include "stm32f10x.h"
#include "system.h"

#define UART3_TXBUFF_SIZE				1024					//定义串口3发送缓冲区大小
#define UART3_RXBUFF_SIZE				1024					//定义串口3接收缓冲区大小

extern u32 UART3_RxCounter;										//指定串口3接收数据计数
extern char UART3_RxBuff[UART3_RXBUFF_SIZE];	//指定串口3接收缓冲区
extern char UART3_TxBuff[UART3_TXBUFF_SIZE];	//指定串口3发送缓冲区

extern void UART3_Init(void);									//串口3的初始化函数
extern void UART3_Puts(char *, ...);					//串口3的发送可变参函数
extern void UART3_TxData(unsigned char *data, unsigned int len);
extern void UART3DMA_Init(void);							//串口3的DMA初始化函数
#endif
