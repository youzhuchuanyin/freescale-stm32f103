#ifndef __UART2_H__
#define __UART2_H__
#include "stm32f10x.h"
#include "system.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "systick.h"
#include "misc.h"

#define UART2_TXDATA_SIZE 512
#define UART2_RXDATA_SIZE 512
#define RS485_DERE		PDout(7)	//485的驱动/读取 驱动 =1:读取 =0:
struct modResData {
	float temp;
	float moisture;
};
extern unsigned int UART2_RxCounter; //记录串口2接收数据个数
extern unsigned char UART2_TXDATA[UART2_TXDATA_SIZE];
extern unsigned char UART2_RXDATA[UART2_RXDATA_SIZE];

extern unsigned short Get_CRC16(unsigned char *ptr, int len);
extern void UART2_INIT(void);
extern void DMA_INIT(void);
extern void UART2_TXFUNC(unsigned char *data,int len);
extern void Modbus_Handle_Master(void);
extern void Modbus_03_Master(void);

#endif