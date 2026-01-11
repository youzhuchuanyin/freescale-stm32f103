#include "iic.h"
#include "systick.h"

//定义IIC初始化函数
void IIC_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//使能SCL和SDA引脚的GPIO时钟
	RCC_APB2PeriphClockCmd(IIC_SCL_PORT_RCC|IIC_SDA_PORT_RCC, ENABLE);
	
	//初始化SCL引脚的工作参数
	GPIO_Config.GPIO_Pin = IIC_SCL_PIN;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;			//GPIO模拟I2C时序,采用推挽输出
	GPIO_Init(IIC_SCL_PORT, &GPIO_Config);
	
	//初始化SDA引脚的工作参数
	GPIO_Config.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;			//GPIO模拟I2C时序,默认采用推挽输出
	GPIO_Init(IIC_SDA_PORT, &GPIO_Config);
	
	//默认拉高SCL和SDA,协议规定
	IIC_SCL = 1;
	IIC_SDA = 1;
}

//定义配置SDA为输出的函数,CPU发送数据的时候需要配置为输出
void SDA_OUT(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;			//GPIO模拟I2C时序,默认采用推挽输出
	GPIO_Init(IIC_SDA_PORT, &GPIO_Config);
}

//定义配置SDA为输入的函数,CPU从外设读取数据的时候需要配置为输入
void SDA_IN(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU;	//输入上拉
	GPIO_Init(IIC_SDA_PORT, &GPIO_Config);
}

//定义发送START信号函数
/*
SCL:
1	----------------------------------------

0
SDA:   >4.7us
1 -------------------
										|			>4us
0										----------------------
*/
void IIC_Start(void) {
	SDA_OUT();			//配置SDA为输出
	IIC_SCL = 1;		//时钟线拉高
	IIC_SDA = 1;		//数据线拉高
	delay_us(5);
	IIC_SDA = 0;		//数据线拉低
	delay_us(6);		//此时SCL还是高电平
	IIC_SCL = 0;		//准备开始发送设备地址信息了
}

//定义发送STOP信号函数
/*
SCL:
1	----------------------------------------

0
SDA:   									>4us
1 									----------------------
				>4.7us			|			
0	-------------------
*/
void IIC_Stop(void) {
	SDA_OUT();		//配置为输出
	IIC_SDA = 0;	//拉低数据线
	IIC_SCL = 1;	//拉高时钟线
	delay_us(6);		
	IIC_SDA = 1;	//拉高时钟线
	delay_us(6);	
}

//定义等待ACK信号函数
u8 IIC_Wait_Ack(void) {
	u8 tempTime = 0;		//判断接收外设发送的ACK信号是否超时
	
	IIC_SCL = 0;				//先把时钟线拉低的目的是让外设能够将ACK信号放到SDA上
	delay_us(6);				//在此期间外设将ACK信号放到数据线SDA上
	SDA_IN();						//如果CPU要想获取外设发送的ACK信号,首先将SDA配置为输入
	IIC_SCL = 1;				//本着低放高取的原则，将SCL拉高
	delay_us(6);
	while(READ_SDA) {
		tempTime++;
		if(tempTime > 250) {
			IIC_Stop();			//没有获取ACK信号超时,说明数据传输有问题停止后续的操作
			return 1;
		}
	}
	IIC_SCL = 0;				//为下一次数据传输做准备
	return 0;						//正确接收到了ACK信号,接下来可以继续传输数据了
}

//定义CPU发送ACK信号函数
void IIC_Ack(void) {
	IIC_SCL = 0;				//低放高取的原则
	SDA_OUT();					//SDA配置为输出
	IIC_SDA = 0;				//将低电平的ACK信号放到SDA上
	delay_us(6);
	IIC_SCL = 1;				//让外设在SCL为高电平的时候获取ACK信号
	delay_us(6);
	IIC_SCL = 0;				//开始准备下一个数据传输
}

//定义CPU发送NACK信号函数
void IIC_NAck(void) {
	IIC_SCL = 0;				//低放高取的原则
	SDA_OUT();					//SDA配置为输出
	IIC_SDA = 1;				//将高电平的NACK信号放到SDA上
	delay_us(6);
	IIC_SCL = 1;				//让外设在SCL为高电平的时候获取ACK信号
	delay_us(6);
	IIC_SCL = 0;				//开始准备下一个数据传输
}

//定义发送1字节数据函数
void IIC_Send_Byte(u8 TxData) {
	u8 i;
	SDA_OUT();					//配置为输出准备开始发送数据了
	IIC_SCL = 0;				//为了将数据放到数据线SDA上
	for(i = 0; i < 8; i++) {
		if(TxData & 0x80) 
			IIC_SDA = 1;		//发送1高电平
		else
			IIC_SDA = 0;		//发送0低电平
		TxData <<= 1;			//将低位数据往高位挪动
		delay_us(6);
		IIC_SCL = 1;			//为了让外设获取数据
		delay_us(6);
		IIC_SCL = 0;			//开始传输下一个bit位
	}
}

//定义读取1字节数据的函数
u8 IIC_Read_Byte(u8 ack) {
	u8 i, data = 0;
	SDA_IN();						//配置SDA为输入，开始获取数据
	for(i = 0; i < 8; i++) {
		IIC_SCL = 0;			//为了让外设将数据放到SDA上
		delay_us(6);
		IIC_SCL = 1;			//在SCL为高电平的时候获取数据
		data |= READ_SDA << (7 - i);	//从高位开始获取数据一位一位的保存起来
		delay_us(6);
	}
	//读取1字节数据，判断是否需要CPU发送ACK或者NACK
	if(!ack) 
		IIC_NAck();			//表示停止数据的读取操作
	else
		IIC_Ack();			//表示CPU还想继续读取数据
	return data;			//返回读取到的数据
}




























