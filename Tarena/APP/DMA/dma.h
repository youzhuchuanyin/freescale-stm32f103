#ifndef __DMA_H
#define __DMA_H

#include "stm32f10x.h"
#include "system.h"

#define UART1DMA_TXBUFF_SIZE		(1024*4)				//指定串口1发送缓冲区内存的大小
extern u8 UART1DMA_TxBuff[UART1DMA_TXBUFF_SIZE];//指定串口1发送缓冲区
extern u32 UART1DMA_TxCounter;									//指定串口1发送数据计数

extern u8 DMA_TcFlags;													//=1:表示中断到来DMA搬移完成,=0:DMA搬移中

#define UART1DMA_RXBUFF_SIZE		(1024)					//指定串口1接收缓冲区的内存大小
extern u8 UART1DMA_RxBuff[UART1DMA_RXBUFF_SIZE];//指定串口1接收缓冲区
extern u32 UART1DMA_RxCounter;									//指定串口1接收数据计数

extern void UART1DMA_Init(void);								//串口1DMA初始化函数
extern void UART1DMA_Tx_Test(void);							//串口1DMA功能测试命令处理函数
#endif
