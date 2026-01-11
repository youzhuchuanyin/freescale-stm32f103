#include "dma.h"
#include "led.h"
#include "stdio.h"
#include "systick.h"

//定义串口1发送数据对应的DMA的内存和发送数据计数的变量
u32 UART1DMA_TxCounter = 0;
u8 UART1DMA_TxBuff[UART1DMA_TXBUFF_SIZE];

//定义DMA搬移完成中断到来的标志
u8 DMA_TcFlags = 0;

//定义串口1的DMA接收数据的内存和接收数据计数的变量
u8 UART1DMA_RxBuff[UART1DMA_RXBUFF_SIZE];
u32 UART1DMA_RxCounter = 0;

//定义串口1的DMA功能的初始化函数
void UART1DMA_Init(void) {
	DMA_InitTypeDef DMA_Config;
	
	//使能DMA1控制器时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//配置DMA1通道4作为串口1的发送数据功能(内存搬移到数据寄存器)
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR;	//指定DMA搬移对应的串口1数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART1DMA_TxBuff; //指定DMA搬移对应的内存的基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST;						//指定DMA搬移方向:内存->数据寄存器
	DMA_Config.DMA_BufferSize = UART1DMA_TXBUFF_SIZE;			//指定DMA缓冲区大小4KB
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//数据寄存器地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;			//内存地址要自增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//指定搬移的数据块为1字节
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//指定搬移的数据块大小1字节
	DMA_Config.DMA_Mode = DMA_Mode_Normal;									//普通模式
	DMA_Config.DMA_Priority = DMA_Priority_Medium;					//指定中等优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;										//禁止内存到内存的搬移
	DMA_Init(DMA1_Channel4, &DMA_Config);										//配置DMA1通道4工作参数
	
	//使能DMA1通道4的搬移完成中断功能
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	
	//初始化NVIC的DMA1通道4的中断功能
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel4_IRQn; 	//IRQ=interrupt request
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;	//抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0;					//子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;						//将来NVIC就可以发送DMA1通道4的中断了
	NVIC_Init(&NVIC_Config);
	
	//配置DMA1通道5作为串口1的接收数据功能(数据寄存器搬移到内存)
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR;	//指定DMA搬移对应的串口1数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART1DMA_RxBuff; //指定DMA搬移对应的内存的基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC;						//指定DMA搬移方向:数据寄存器->内存
	DMA_Config.DMA_BufferSize = UART1DMA_RXBUFF_SIZE;			//指定DMA缓冲区大小4KB
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//数据寄存器地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;			//内存地址要自增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//指定搬移的数据块为1字节
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//指定搬移的数据块大小1字节
	DMA_Config.DMA_Mode = DMA_Mode_Normal;									//普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High;					//指定高优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;										//禁止内存到内存的搬移
	DMA_Init(DMA1_Channel5, &DMA_Config);										//配置DMA1通道5工作参数
	DMA_Cmd(DMA1_Channel5, ENABLE);												//启动DMA1通道5作为串口1的接收功能
}

//定义串口1通过DMA1通道4来搬移数据将数据发送出去
void UART1DMA_Tx_Test(void) {
	//初始化构造DMA搬移的内存数据
	int i;
	for(i = 0; i < UART1DMA_TXBUFF_SIZE; i++)
		UART1DMA_TxBuff[i] = 'A';
	
	//先关闭DMA1通道4
	DMA_Cmd(DMA1_Channel4, DISABLE);
	//使能UART1的发送数据采用DMA方式
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	//指定要发送的数据长度
	DMA_SetCurrDataCounter(DMA1_Channel4, UART1DMA_TXBUFF_SIZE);
	//最后启动DMA1通道4开始传输UART1的数据,此时DMA1通道4硬件上自动将内存中的数据
	//偷偷摸摸的搬移到串口的数据寄存器DR中发送出去
	DMA_Cmd(DMA1_Channel4, ENABLE);
	
	//每次测试一开始标记DMA还没有开始搬移数据,但是即将开始搬移
	DMA_TcFlags = 0;
	
	//让CPU干其他的事情，一旦DMA搬移完成，测试函数执行完毕
	while(1) {
		if(DMA_TcFlags) {	//搬移完成产生中断，中断置1,此函数执行结束
			printf("UART1 DMA TX DONE!\n");
			break;
		}
		LED1 = !LED1;
		delay_ms(100);
	}
}


