#include "init.h"
#include "led.h"
#include "beep.h"
#include "uart.h"
#include "key.h"
#include "systick.h"
#include "exti.h"
#include "at24c02.h"
#include "ds18b20.h"
#include "dma.h"
#include "timer4.h"
#include "esp8266.h"
#include "uart3.h"
#include "motor.h"
#include	"uart2.h"
#include "oled.h"
//定义初始化函数指针数组保存各个硬件的初始化函数首地址
//以后维护只需维护此数组即可,main函数一个字都不用动
static PINIT_T init_func[] = {
	SysTick_Init,			//滴答定时器初始化函数
	LED_Init,					//LED初始化函数
	BEEP_Init,				//蜂鸣器初始化函数
	//UART_Init,				//串口1初始化函数,采用1字节1中断方式
	UART_IDLEInit,			//串口1初始化函数,采用1帧1中断方式
	KEY_Init,					//按键初始化函数
	MY_EXTI_Init,			//按键中断初始化函数
	AT24C02_Init,			//初始化I2C
	DS18B20_Init,			//初始化DS18B20
	UART1DMA_Init,		//初始化UART1的DMA功能
	TIMER4_Init,			//初始化定时器4
	WIFI_ResetIO_Init, //初始化8266复位引脚
	UART3_Init,				 //初始化串口3
	MOTOR_Init,					//初始化PWM定时器
	UART2_INIT,
	OLED_Init,
	0									//为了退出下面的for循环
};

//定义硬件初始化总入口函数
void DEVICE_Init(void) {
	//定义初始化函数指针变量保存函数指针数组首地址
	PINIT_T *pfunc = init_func;
	
	//遍历函数指针数组中的每个元素，将每个元素对应的函数挨个调用
	for(; *pfunc; pfunc++) 
		(*pfunc)();	//注意：*运算符优先级低于后面的()圆括号,所以前面需要加()
								//即：(*pfunc),也就是先获取函数指针数组中每个初始化函数的地址
}
