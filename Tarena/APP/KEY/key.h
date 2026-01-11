#ifndef __KEY_H
#define __KEY_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

#define KEY0_PIN					GPIO_Pin_4			//指定KEY0按键的引脚编号
#define KEY1_PIN					GPIO_Pin_3			//指定KEY1按键的引脚编号
#define KEY_UP_PIN				GPIO_Pin_0			//指定KEY_UP按键的引脚编号

#define KEY_PORT					GPIOE						//指定KEY0,KEY1,KEY2所在的端口E
#define KEY_UP_PORT				GPIOA						//指定KEY_UP所在的端口A

#define KEY0							PEin(4)					//指定KEY0按键引脚的位带信息
#define KEY1							PEin(3)					//指定KEY1按键引脚的位带信息
#define KEY_UP						PAin(0)					//指定KEY_UP按键引脚的位带信息

#define KEY0_PRESS				1								//给KEY0按键指定一个键值
#define KEY1_PRESS				2								//给KEY1按键指定一个键值
#define KEY_UP_PRESS			3								//给KEY_UP按键指定一个键值

extern void KEY_Init(void); 							//按键的初始化函数
extern u8 KEY_Scan(void);									//按键状态的扫描函数
#endif
