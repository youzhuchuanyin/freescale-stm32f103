#ifndef __SYSTEM_H
#define __SYSTEM_H

//包含总头文件
#include "stm32f10x.h"

//定义位带区对应的位带别名区的地址换算公式
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//定义GPIO控制器内部的输出数据寄存器的基地址
#define GPIOA_ODR_Addr    (GPIOA_BASE+0xC) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+0xC) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0xC) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+0xC) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0xC) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+0xC) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+0xC) //0x40011E0C    

//定义GPIO控制器内部的输入寄存器的基地址
#define GPIOA_IDR_Addr    (GPIOA_BASE+0x8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x8) //0x40011E08 
 
//位带取别名的映射
//结论：PAout(n) = *(unsigned long *)位带别名区的地址 = GPIOA输出数据寄存器的第n个bit位的值嘛;
//结论：PAin(n)  = *(unsigned long *)位带别名区的地址 = GPIOA输入寄出去你的第n个bit位的值嘛

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出,例如：PAout(3) = 1;表示让GPIOA3输出高电平 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入,例如：char value = PAin(3);表示获取GPIOA3引脚的电平1或者0赋值给value保存起来 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define ARRAY_SIZE(x)			(sizeof(x) / sizeof(x[0]))
#endif
