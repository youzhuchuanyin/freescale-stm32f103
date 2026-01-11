#ifndef __IIC_H 
#define __IIC_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

//定义SCL引脚的信息
#define IIC_SCL_PORT						GPIOB
#define IIC_SCL_PIN							GPIO_Pin_6
#define IIC_SCL_PORT_RCC				RCC_APB2Periph_GPIOB

//定义SDA引脚的信息
#define IIC_SDA_PORT						GPIOB
#define IIC_SDA_PIN							GPIO_Pin_7
#define IIC_SDA_PORT_RCC				RCC_APB2Periph_GPIOB

//定义输入输出的位带信息
#define IIC_SCL									PBout(6)
#define IIC_SDA									PBout(7)
#define READ_SDA								PBin(7)

//声明GPIO模拟I2C时序的操作函数
extern void IIC_Init(void);						//IIC初始化
extern void IIC_Start(void);					//发送START信号
extern void IIC_Stop(void);						//发送STOP信号
extern void IIC_Send_Byte(u8 TxData);	//发送1字节数据
extern u8 IIC_Read_Byte(u8 Ack);			//读取1字节数据
extern u8 IIC_Wait_Ack(void);					//等待ACK
extern void IIC_Ack(void);						//CPU发送ACK信号
extern void IIC_NAck(void);						//CPU发送NACK信号
#endif

