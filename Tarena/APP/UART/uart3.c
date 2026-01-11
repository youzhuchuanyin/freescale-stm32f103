#include "uart3.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"		//可变参函数实现所需的头文件,声明va_list数据类型

//定义串口3相关的变量和内存
u32 UART3_RxCounter = 0;
char UART3_RxBuff[UART3_RXBUFF_SIZE];
char UART3_TxBuff[UART3_TXBUFF_SIZE];

//定义串口3的初始化函数
void UART3_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	USART_InitTypeDef UART_Config;
	NVIC_InitTypeDef NVIC_Config;
	
	//指定UART3为复位默认值
	USART_DeInit(USART3);
	
	//使能UART3时钟和GPIOB控制器时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//初始化GPIOB10和GPIOB11作为串口3的发送和接收引脚功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_10;
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出:GPIOB10->UART3TX
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);
	
	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//输入浮空
	GPIO_Init(GPIOB, &GPIO_Config);
	
	//配置串口3的工作参数：115200 8N1
	UART_Config.USART_BaudRate = 115200;										//指定波特率115200
	UART_Config.USART_WordLength = USART_WordLength_8b;		//指定数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;				//指定停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;						//不校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不采用硬件流控
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能发送和接收功能
	USART_Init(USART3, &UART_Config);
	USART_Cmd(USART3, ENABLE);
	
	//启动使能UART3空闲中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	//配置NVIC支持串口3的中断
	NVIC_Config.NVIC_IRQChannel = USART3_IRQn;					//指定串口1的中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;	//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0;					//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;						//打开串口1的中断功能
	NVIC_Init(&NVIC_Config);
	
	//开启串口3的DMA接收和发送功能
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	//初始化UART3的DMA功能
	UART3DMA_Init();
}

//定义串口3的DMA功能函数
void UART3DMA_Init(void) {
	DMA_InitTypeDef DMA_Config;
	NVIC_InitTypeDef NVIC_Config;
	
	//使能DMA1控制器的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//配置DMA1控制器的通道2作为串口3的发送数据功能
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART3->DR;	//指定DMA搬移对应的串口3数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_TxBuff; //指定DMA搬移对应的内存的基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST;						//指定DMA搬移方向:内存->数据寄存器
	DMA_Config.DMA_BufferSize = UART3_TXBUFF_SIZE;			//指定DMA缓冲区大小1KB
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//数据寄存器地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;			//内存地址要自增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//指定搬移的数据块为1字节
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//指定搬移的数据块大小1字节
	DMA_Config.DMA_Mode = DMA_Mode_Normal;									//普通模式
	DMA_Config.DMA_Priority = DMA_Priority_Medium;					//指定中等优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;										//禁止内存到内存的搬移
	DMA_Init(DMA1_Channel2, &DMA_Config);										//配置DMA1通道2工作参数
	
	//配置DMA1控制器的通道3作为串口3的接收数据功能
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART3->DR;	//指定DMA搬移对应的串口3数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_RxBuff; //指定DMA搬移对应的内存的基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC;						//指定DMA搬移方向:数据寄存器->内存
	DMA_Config.DMA_BufferSize = UART3_RXBUFF_SIZE;			//指定DMA缓冲区大小4KB
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//数据寄存器地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;			//内存地址要自增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//指定搬移的数据块为1字节
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//指定搬移的数据块大小1字节
	DMA_Config.DMA_Mode = DMA_Mode_Normal;									//普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High;					//指定高优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;										//禁止内存到内存的搬移
	DMA_Init(DMA1_Channel3, &DMA_Config);										//配置DMA1通道3工作参数
	DMA_Cmd(DMA1_Channel3, ENABLE);												//启动DMA1通道3作为串口1的接收功能

	//使能DMA1通道2搬移数据完成中断
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	
	//初始化NVIC的DMA1通道2的中断功能
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel2_IRQn; 	//IRQ=interrupt request
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;	//抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0;					//子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;						//将来NVIC就可以发送DMA1通道4的中断了
	NVIC_Init(&NVIC_Config);
}

//定义UART3发送数据可变参函数
//WIFI_Printf("%s\r\n", "AT+CIPSEND");
//UART3_Puts("%s\r\n", "AT+CIPSEND");
void UART3_Puts(char *fmt, ...) {
	//目标将要发送的可变参数据信息构造成一个字符串保存到串口3发送缓冲区中
	//然后让DMA进行搬移到串口3的数据寄存器中进行数据的发送
	va_list ap;
	va_start(ap, fmt);
	vsprintf(UART3_TxBuff, fmt, ap);	//结果：UART3_TxBuff="AT+CIPSEND\r\n"
	va_end(ap);
	
	//启动DMA通道2将数据从UART3_TxBuff缓冲区搬移到数据寄存器发送出去
	DMA_Cmd(DMA1_Channel2, DISABLE);													//先关
	DMA_SetCurrDataCounter(DMA1_Channel2, UART3_TXBUFF_SIZE);	//指定发送的数据长度
	DMA_Cmd(DMA1_Channel2, ENABLE);														//启动DMA搬移
}

void UART3_TxData(unsigned char *data, unsigned int len) {
	DMA_Cmd(DMA1_Channel2, DISABLE);
	//利用DMA1通道2发送报文数据
	DMA1_Channel2->CMAR = (unsigned int)data;
	//设置DMA发送发布报文的长度
	DMA_SetCurrDataCounter(DMA1_Channel2, len);
	DMA_Cmd(DMA1_Channel2, ENABLE); //启动DMA开始将发布报文进行搬移传输
}




