#include "key.h"
#include "systick.h"

//定义按键的初始化函数
void KEY_Init(void){
	GPIO_InitTypeDef GPIO_Config;
	
	//1.使能GPIOE和GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE, ENABLE);
	
	//2.初始化GPIOE4,GPIOE3
	GPIO_Config.GPIO_Pin = KEY0_PIN|KEY1_PIN;	//指定GPIOE4,GPIO3
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU;		//指定输入上拉,默认电平为高电平
	GPIO_Init(KEY_PORT, &GPIO_Config);				//初始化GPIOE4,GPIOE3
	
	//3.初始化GPIOA0
	GPIO_Config.GPIO_Pin = KEY_UP_PIN;				//指定GPIOA0
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPD;		//指定下拉输入,默认电平为低电平
	GPIO_Init(KEY_UP_PORT, &GPIO_Config);			//初始化GPIOA0
}

//定义按键的扫描函数
u8	KEY_Scan(void) {
	//如果按键有操作ispress=1,如果按键无操作ispressed=0
	static u8 ispressed = 0;
	//判断到底是哪个按键的操作
	if((ispressed == 0) && (KEY0 == 0 || KEY1 == 0 || KEY_UP == 1))	{
		delay_ms(10);  //采用延时去抖动,优点是能够去除抖动,缺点：降低了CPU利用率,实时性差
		ispressed = 1; //标记按键有操作了
		if(KEY0 == 0)
			return KEY0_PRESS;	//返回KEY0按键的键值
		else if(KEY1 == 0)
			return KEY1_PRESS;	//返回KEY1按键的键值
		else if(KEY_UP == 1)
			return KEY_UP_PRESS;	//返回KEY_UP按键的键值
	} else if(KEY0 == 1 && KEY1 == 1 && KEY_UP == 0) //松开
		ispressed = 0; //标记按键无操作
	return 0;
}





