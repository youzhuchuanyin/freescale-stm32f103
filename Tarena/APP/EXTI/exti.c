#include "exti.h"
#include "led.h"
#include "systick.h"
#include "key.h"
#include "beep.h"

//定义按键中断的初始化函数
void MY_EXTI_Init(void) {
	//用于配置EXTI外部中断控制器
	EXTI_InitTypeDef EXTI_Config;
	//用于配置NVIC中断控制器
	NVIC_InitTypeDef NVIC_Config;
	
	//使能AFIO复用功能控制器时钟,配置GPIO为中断功能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//将KEY0按键的引脚GPIOE4作为外部中断输入源：GPIOE4->EXTI4
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
	//将KEY1按键的引脚GPIOE3作为外部中断输入源：GPIOE3->EXTI3
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
	//将KEY_UP按键的引脚GPIOA0作为外部中断输入源：GPIOA0->EXTI0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	//配置EXTI3/4的EXTI外部中断控制器工作参数
	EXTI_Config.EXTI_Line = EXTI_Line3 | EXTI_Line4; 	//指定EXTI3/EXTI4作为中断源
	EXTI_Config.EXTI_Mode = EXTI_Mode_Interrupt; 			//指定中断模式,将来产生的中断直接给CPU核
	EXTI_Config.EXTI_Trigger = EXTI_Trigger_Falling; 	//指定下降沿触发中断
	EXTI_Config.EXTI_LineCmd = ENABLE; 								//使能中断功能
	EXTI_Init(&EXTI_Config);
	
	//配置EXTI0的EXTI外部中断控制器工作参数
	EXTI_Config.EXTI_Line = EXTI_Line0; 							//指定EXTI0作为中断源
	EXTI_Config.EXTI_Mode = EXTI_Mode_Interrupt; 			//指定中断模式,将来产生的中断直接给CPU核
	EXTI_Config.EXTI_Trigger = EXTI_Trigger_Rising; 	//指定下降沿触发中断
	EXTI_Config.EXTI_LineCmd = ENABLE; 								//使能中断功能
	EXTI_Init(&EXTI_Config);
	
	//配置NVIC控制器支持EXTI0,EXTI3,EXTI4
	NVIC_Config.NVIC_IRQChannel = EXTI4_IRQn;					//指定EXTI4中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0; 			//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;					//使能通道4
	NVIC_Init(&NVIC_Config);
	
	NVIC_Config.NVIC_IRQChannel = EXTI3_IRQn;					//指定EXTI3中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 1; 			//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;					//使能通道3
	NVIC_Init(&NVIC_Config);
	
	NVIC_Config.NVIC_IRQChannel = EXTI0_IRQn;					//指定EXTI0中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3; 			//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;					//使能通道0
	NVIC_Init(&NVIC_Config);
}

//定义EXTI4外部中断4的中断处理函数
void EXTI4_IRQHandler(void) {
	//首先判断是否是EXTI4中断源给的中断信号
	if(EXTI_GetITStatus(EXTI_Line4) == 1) {
		//去抖动
		delay_ms(10);
		//如果是KEY0按键产生的中断,根据用户的需求要求开关灯
		if(KEY0 == 0) {
			LED2 = !LED2;
			BEEP = !BEEP;
		}
	}
	//切记清除中断到来的标志位为0，否则一直产生中断
	EXTI_ClearITPendingBit(EXTI_Line4);
}

//定义EXTI3外部中断3的中断处理函数
void EXTI3_IRQHandler(void) {
	//首先判断是否是EXTI3中断源给的中断信号
	if(EXTI_GetITStatus(EXTI_Line3) == 1) {
		//去抖动
		delay_ms(10);
		//如果是KEY1按键产生的中断,根据用户的需求要求开灯
		if(KEY1 == 0) {
			LED2 = 0;
		}
	}
	//切记清除中断到来的标志位为0，否则一直产生中断
	EXTI_ClearITPendingBit(EXTI_Line3);
}

//定义EXTI0外部中断0的中断处理函数
void EXTI0_IRQHandler(void) {
	//首先判断是否是EXTI0中断源给的中断信号
	if(EXTI_GetITStatus(EXTI_Line0) == 1) {
		//去抖动
		delay_ms(10);
		//如果是KEY_UP按键产生的中断,根据用户的需求要求关灯
		if(KEY_UP == 1) {
			LED2 = 1;
		}
	}
	//切记清除中断到来的标志位为0，否则一直产生中断
	EXTI_ClearITPendingBit(EXTI_Line0);
}












