#include "beep.h"

//定义初始化函数
void BEEP_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//1.打开GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.指定GPIOB8的工作参数
	GPIO_Config.GPIO_Pin = GPIO_Pin_8;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_Config);
	
	//3.默认关闭蜂鸣器,输出低电平
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

//定义打开蜂鸣器函数
void BEEP_On(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_8); //输出高电平
}

//定义关闭蜂鸣器函数
void BEEP_Off(void) {
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

