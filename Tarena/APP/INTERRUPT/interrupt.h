#ifndef __INTERRUPT_H
#define __INTERRUPT_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

#define UART_RXBUFF_SIZE			1024				//指定串口1接收缓冲区的内存大小
extern u32 UART_RxCounter;								//记录串口1接收数据的长度
extern u8 UART_RxBuff[UART_RXBUFF_SIZE];	//指定串口1接收缓冲区
extern u8 UART_Send_Flag;									//指定串口1接收上位机发送来的命令完成的标志

#endif
