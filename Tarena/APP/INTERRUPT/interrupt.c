#include "interrupt.h"
#include "dma.h"
#include "uart3.h"
#include "uart2.h"
//定义串口1接收数据长度，缓冲区和接收完成标志
u32 UART_RxCounter = 0;
u8 UART_RxBuff[UART_RXBUFF_SIZE];
u8 UART_Send_Flag = 0; //如果把上位机发送的命令接收完毕置1否则置0

//定义串口1的中断处理函数
void USART1_IRQHandler(void) {
	u8 c;			//暂存接收到的1字节数据
	//判断是否是串口1的接收中断类型,只要串口1接收到1字节数据立刻产生中断,CPU核立刻执行此函数
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		//清中断
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		//读取接收到的数据
		c = USART_ReceiveData(USART1);
		//将接收到的1字节数据暂存到接收缓冲区中
		UART_RxBuff[UART_RxCounter++] = c;
		if(c == '\n') {
			//注意：上位机SSCOM发送来的字符串格式："led on\r\n"
			UART_RxBuff[UART_RxCounter - 2] = '\0';
			UART_RxCounter = 0;
			UART_Send_Flag = 1; //表示上位机发送来的命令字符串传输完毕,接下来可以解析命令了
		}
	}
	
	//只要有一帧数据接收完毕,后面再没有数据了,就会产生一次空闲中断
	//此分支用来判断是否是串口的空闲中断
	//心里要明明白白此时的串口接收到的数据由DMA1控制器自动搬移到接收缓冲区中
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		//清中断
		USART1->SR;	//先读状态寄存器
		USART1->DR;	//后读数据寄存器
		//首先要获取此次串口1利用DMA接收到的数据大小=DMA总的内存大小-剩余DMA缓冲区大小
		UART1DMA_RxCounter = UART1DMA_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
		//然后获取一个完整的上位机发送的字符串信息,追加结束符
		UART1DMA_RxBuff[UART1DMA_RxCounter - 2] = '\0'; //再次强调UART1DMA_RxBuff的数据已经由DMA帮你搞定
		//重新启动DMA的下一次数据接收功能
		//正常模式：先关闭DMA，然后设置搬移的数据长度，最后启动DMA开始等待数据搬移
		DMA_Cmd(DMA1_Channel5, DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel5, UART1DMA_RXBUFF_SIZE);
		DMA_Cmd(DMA1_Channel5, ENABLE);
	}
}

//定义DMA1通道4的中断处理函数
void DMA1_Channel4_IRQHandler(void) {
	//首先判断中断的类型是否是搬移完成中断
	if(DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
		//清中断
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		//关闭DMA1通道4
		DMA_Cmd(DMA1_Channel4, DISABLE);
		//标记DMA搬移完成,通知测试函数,通知CPU已经搬移完成
		DMA_TcFlags = 1;
	}
}

//定义串口3中断处理函数
void USART3_IRQHandler(void) {
	//判断是否是空闲中断
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) {
		//清中断
		USART3->SR;
		USART3->DR;
		//计算此次串口3接收到的数据个数
		//再次强调,此时此刻串口3接收缓冲区UART3_RxBuff中已经有了接收到的数据
		//此数据是由DMA硬件上自动进行搬移的
		UART3_RxCounter = UART3_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		//重新设置接收的数据长度,等待下一次DMA搬移接收数据
		DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA1_Channel3->CNDTR = UART3_RXBUFF_SIZE;//等价于DMA_SetCurrDataCounter(DMA1_Channel3, UART3_RXBUFF_SIZE);
		DMA_Cmd(DMA1_Channel3, ENABLE);
	}
}

//定义DMA1通道2数据搬移完成中断
void DMA1_Channel2_IRQHandler(void) {
	//判断是否是通道2的数据搬移完成中断
	if(DMA_GetITStatus(DMA1_IT_TC2) != RESET) {
		//清中断
		DMA_ClearITPendingBit(DMA1_IT_TC2);
		//关闭DMA1通道2,将来需要发送数据自己再打开,参见UART3_Puts
		DMA_Cmd(DMA1_Channel2, DISABLE);
	}
}
//串口2的接收中断
void USART2_IRQHandler(void){
	
	if((USART_GetITStatus(USART2, USART_IT_IDLE)) != RESET) {
	//当中断到来,清除标志位
	USART2->SR;
	USART2->DR;
	UART2_RxCounter = UART2_RXDATA_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);//获取接收到的字符长度
	DMA_Cmd(DMA1_Channel6,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel6,UART2_RXDATA_SIZE);
	DMA_Cmd(DMA1_Channel6,ENABLE);
	}
}




















