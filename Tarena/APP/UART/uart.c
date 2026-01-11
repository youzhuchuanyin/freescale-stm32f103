#include "uart.h"
#include "stdio.h"	//为了声明FILE类型

//定义串口1的初始化函数
void UART_Init(void) {
	//用于配置串口1的GPIO引脚的工作参数
	GPIO_InitTypeDef GPIO_Config;
	
	//用于配置串口1的工作参数
	USART_InitTypeDef UART_Config;
	
	//打开GPIOA和UART1控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//配置GPIOA9作为串口1的发送引脚功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_9;
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//配置GPIOA10作为串口1的接收引脚功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_10;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//配置串口1的工作参数：115200 8N1
	UART_Config.USART_BaudRate = BOUND;										//指定波特率115200
	UART_Config.USART_WordLength = USART_WordLength_8b;		//指定数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;				//指定停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;						//不校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不采用硬件流控
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能发送和接收功能
	USART_Init(USART1, &UART_Config);
	
	//打开串口1，开始传输数据了
	USART_Cmd(USART1, ENABLE);
	
	//配置NVIC支持UART1中断功能
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = USART1_IRQn;					//指定串口1的中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;	//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;					//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;						//打开串口1的中断功能
	NVIC_Init(&NVIC_Config);
	
	//打开串口1的接收中断功能:当UART1控制器接收到1字节数据之后会给CPU核发送一个
	//接收中断信号,通知CPU核数据准备就绪了,来吧CPU宝贝，赶紧把数据读走,一旦CPU核
	//把数据读走CPU核再回到原先被打断的地方继续运行等待着下一次串口接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

//定义串口1发送字符函数
void UART_Putc(u8 c) {
	//首先判断发送数据寄存器是否还有数据,如果有数据目前采用轮询方式死等，知道数据发送完毕
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	
	//发送字符
	USART_SendData(USART1, c);
}

//定义串口1发送字符串函数
//pstr = "hello,world\n"
void UART_Puts(u8 *pstr) {
	while(*pstr) {
		UART_Putc(*pstr);
		pstr++;
	}
}

//定义接收字符函数
u8 UART_Getc(void) {
	//首先判断接收数据寄存器是否有数据，如果有则读取，否则轮询死等直到有数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	
	//直接返回从上位机接收到的1字节数据
	return (u8)USART_ReceiveData(USART1);
}

//定义接收字符串函数
//明确：上位机通过SSCOM串口发送的字符串数据格式："hello\r\n"
void UART_Gets(u8 buf[], u32 len) { 
	int i;
	for(i = 0; i < len - 1; i++) {
		buf[i] = UART_Getc();
		if(buf[i] == '\n') //追加退出机制
			break;
	}
	buf[i - 1] = '\0';	//追加结束符
}

//重定向printf,也就是将printf默认输出的设备由显示屏重新指定为串口1，将来通过串口1
//将打印输出的信息发送给上位机
int fputc(int c, FILE *fp) {
	//首先判断发送数据寄存器是否还有数据,如果有数据目前采用轮询方式死等，知道数据发送完毕
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	//发送字符
	USART_SendData(USART1, c);	
	return c;
}

//定义串口1的初始化函数,指定为1帧1中断
void UART_IDLEInit(void) {
	//用于配置串口1的GPIO引脚的工作参数
	GPIO_InitTypeDef GPIO_Config;
	
	//用于配置串口1的工作参数
	USART_InitTypeDef UART_Config;
	
	//打开GPIOA和UART1控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//配置GPIOA9作为串口1的发送引脚功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_9;
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//配置GPIOA10作为串口1的接收引脚功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_10;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//配置串口1的工作参数：115200 8N1
	UART_Config.USART_BaudRate = BOUND;										//指定波特率115200
	UART_Config.USART_WordLength = USART_WordLength_8b;		//指定数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;				//指定停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;						//不校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不采用硬件流控
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能发送和接收功能
	USART_Init(USART1, &UART_Config);
	
	//打开串口1，开始传输数据了
	USART_Cmd(USART1, ENABLE);
	
	//配置NVIC支持UART1中断功能
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = USART1_IRQn;					//指定串口1的中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;	//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;					//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;						//打开串口1的中断功能
	NVIC_Init(&NVIC_Config);
	
	//开启串口1的空闲中断功能
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	//开启串口1的DMA接收和发送功能
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}
