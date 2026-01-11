#include "uart2.h"
unsigned int UART2_RxCounter = 0; //记录串口2接收数据个数
unsigned char UART2_TXDATA[UART2_TXDATA_SIZE];    // 实际定义，分配内存
unsigned char UART2_RXDATA[UART2_RXDATA_SIZE];   // 实际定义，分配内存
struct modResData modresdata;
//定义CRC16校验
unsigned short Get_CRC16(unsigned char *ptr, int len) {
		uint8_t i;
    uint16_t crc = ~0x00;

    if((ptr == 0) || (len == 0xFFFF)) return crc;

    while(len--){
        crc ^= *ptr++;
        for(i = 0; i < 8; i++){
            if(crc & 0x01){
                crc >>= 1;
                crc ^= 0xA001;
            }
            else{
                crc >>= 1;
            }
        }
    }
    return crc;
}
//pa3-->rx;	pa2-->tx;	pd7-->DERE;
void UART2_INIT(void){
	GPIO_InitTypeDef	GPIO_Config;
	USART_InitTypeDef UART_Config;
	NVIC_InitTypeDef	NVIC_Config;
	
	//使能USART2时钟和GPIOA/D时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);
	
	//指定GPIOA2为UART2的发送TX功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_2;					//GPIOA2
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	//指定最大输出速度
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;		//指定为复用推挽输出，用于UART2的发送功能,GPIOA2->UART2TX
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//指定GPIOA3为UART2的接收RX功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_3;							//GPIOA3
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//指定为输入浮空，用于UART2的接收功能，GPIOA3->UART2RX
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//初始化485的DE/RE读写使能引脚
	GPIO_Config.GPIO_Pin = GPIO_Pin_7;				 //GPIOD7
 	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_Config);
	
	//配置UART2工作参数
	UART_Config.USART_BaudRate = 9600;						//指定UART2的波特率
	UART_Config.USART_WordLength = USART_WordLength_8b;	//数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;		//停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;				//无奇偶校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	UART_Config.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//指定可发可收模式
	USART_Init(USART2, &UART_Config);
	USART_Cmd(USART2, ENABLE);
	
	//启动UART2空闲中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);		//开启空闲中断
	
	//启动NVIC的UART2中断
	NVIC_Config.NVIC_IRQChannel = USART2_IRQn;			//启用UART2中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 3;	//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;			//指定响应式优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;		//使能UART2中断
	NVIC_Init(&NVIC_Config);
	
	//开启UART2的DMA接收
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	//开启UART2的DMA发送
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	
	//初始化UART2的DMA功能
	DMA_INIT();
}
//DMA1channl6-->uart2rx;	DMA1cchannel7-->uart2tx;
void DMA_INIT(void){
	DMA_InitTypeDef dma_init;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	dma_init.DMA_BufferSize=UART2_RXDATA_SIZE;
	dma_init.DMA_DIR=DMA_DIR_PeripheralSRC;
	dma_init.DMA_M2M=DMA_M2M_Disable;
	dma_init.DMA_MemoryBaseAddr=(unsigned int)UART2_RXDATA;
	dma_init.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	dma_init.DMA_MemoryInc=DMA_MemoryInc_Enable;
	dma_init.DMA_Mode=DMA_Mode_Normal;
	dma_init.DMA_PeripheralBaseAddr=(unsigned int)&USART2->DR;
	dma_init.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	dma_init.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	dma_init.DMA_Priority=DMA_Priority_High;
	DMA_Init(DMA1_Channel6,&dma_init);//rx

	dma_init.DMA_BufferSize=UART2_TXDATA_SIZE;
	dma_init.DMA_DIR=DMA_DIR_PeripheralDST;
	dma_init.DMA_M2M=DMA_M2M_Disable;
	dma_init.DMA_MemoryBaseAddr=(unsigned int)UART2_TXDATA;
	dma_init.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	dma_init.DMA_MemoryInc=DMA_MemoryInc_Enable;
	dma_init.DMA_Mode=DMA_Mode_Normal;
	dma_init.DMA_PeripheralBaseAddr=(unsigned int)&USART2->DR;
	dma_init.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	dma_init.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	dma_init.DMA_Priority=DMA_Priority_High;
	DMA_Init(DMA1_Channel7,&dma_init);//tx
	DMA_Cmd(DMA1_Channel6, ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	RS485_DERE=0;//接收
}
void UART2_TXFUNC(unsigned char *data,int len){
	printf("enter %s\n",__func__);
	RS485_DERE=1;
	// 1. 首先等待上一次DMA发送完成
	//while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);
	// 2. 还要等待USART发送器真正空闲（TC标志）
	//while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	DMA_Cmd(DMA1_Channel7,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7,len);
	DMA_Cmd(DMA1_Channel7,ENABLE);
	while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC7);
	u32 i;
	for(i=0;i<2;i++) delay_us(1000);
	
	RS485_DERE=0;//接收	
}
void Modbus_Handle_Master(void) {
	printf("enter%s\n",__func__);
	
	switch(UART2_RXDATA[1]) {
		case 0x03: 
		{
			if(UART2_RXDATA[0] == 1) {
				float temp = ((UART2_RXDATA[3] << 8)|UART2_RXDATA[4])/10.0;
				float humi = ((UART2_RXDATA[5] << 8)|UART2_RXDATA[6])/10.0;
				
				/*modresdata.temp=temp;
				modresdata.moisture=humi;*/
				printf("Slave %d: Temp = %.1f, Humi = %.1f\n", UART2_RXDATA[0], temp, humi);
			}
			break;
		}
		case 0x06:
		{
			int i;
			for(i = 0; i < 8; i++)
				printf("%#2x ", UART2_RXDATA[i]);
			printf("\n");
		
			break;
		}
	}

}
void Modbus_03_Master(void) {
	//printf("enter %s\n",__func__);
	while(1) {
		unsigned short CRC16 = 0;
		UART2_TXDATA[0] = 0x01;
		UART2_TXDATA[1] = 0x03;
		UART2_TXDATA[2] = 0x00;
		UART2_TXDATA[3] = 0x00;
		UART2_TXDATA[4] = 0x00;
		UART2_TXDATA[5] = 0x02;
		CRC16 = Get_CRC16(UART2_TXDATA, 6);
		UART2_TXDATA[6] = CRC16 & 0xFF;
		unsigned char temp= (CRC16 >> 8) & 0xFF;
		//printf("temp = 0x%02X\n", temp);     // 打印调试
		UART2_TXDATA[7] =temp;
		UART2_TXFUNC(UART2_TXDATA, 8);
		
		while(1) {
			if(UART2_RxCounter != 0) {//有接受到数据
				//Modbus_Handle_Master();
				UART2_RxCounter = 0;
				break;
			}
		}
		delay_ms(1000);
	}
}