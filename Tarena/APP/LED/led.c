#include "led.h"

//定义初始化函数
void LED_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//1.打开GPIOB/GPIOE控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	//2.配置GPIOB为推挽输出，50MHz
	GPIO_Config.GPIO_Pin = GPIO_Pin_5;					//指定GPIOB5
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 	//指定推挽输出
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	//指定最大输出速度50MHZ
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	//3.配置GPIOE为推挽输出，50MHz
	GPIO_Config.GPIO_Pin = GPIO_Pin_5;					//指定GPIOE5
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 	//指定推挽输出
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	//指定最大输出速度50MHZ
	GPIO_Init(GPIOE, &GPIO_Config);	
	
	//3.默认关灯，省电，输出高电平
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

//定义开灯函数
void LED_On(void) {
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}

//定义关灯函数
void LED_Off(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

//定义延时函数
void Delay(int n) {
	while(n--);
}
