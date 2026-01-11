#include "timer4.h"
#include "led.h"
#include "msg.h"
#include "ds18b20.h"
#include "uart3.h"

//定义定时器4的初始化函数
void TIMER4_Init(void) {
	TIM_TimeBaseInitTypeDef TIMER4_Config;
	NVIC_InitTypeDef NVIC_Config;
	
	//启动定时器4的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	//初始化定时器4的工作参数
	TIMER4_Config.TIM_Period = ARR;											//初始化自动重装载值
	TIMER4_Config.TIM_Prescaler = PSC;									//初始化分频系数
	TIMER4_Config.TIM_ClockDivision = TIM_CKD_DIV1;			//1分频，72MHz
	TIMER4_Config.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM4, &TIMER4_Config);		
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	//使能定时器4控制器的溢出中断功能
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	//初始化NVIC支持定时器4的中断功能
	NVIC_Config.NVIC_IRQChannel = TIM4_IRQn;						//指定定时器4中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;	//抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;					//子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;						//打开定时器4的中断功能
	NVIC_Init(&NVIC_Config);
}

//定义定时器4的中断处理函数,每当计数到期，产生中断
void TIM4_IRQHandler(void) {
	//判断定时器4的中断类型是否是溢出中断
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)) {
		//清中断
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

//定义启动定时器4的处理函数
void TIMER4_Start(void) {
	TIM_Cmd(TIM4, ENABLE);
}

//定义关闭定时器的处理函数
void TIMER4_Stop(void) {
	TIM_Cmd(TIM4, DISABLE);
}







