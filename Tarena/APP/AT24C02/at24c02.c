#include "at24c02.h"
#include "iic.h"
#include "stdio.h"		//为了能够使用keil软件自带的printf库函数
#include "systick.h"

//定义AT24C02的初始化函数
void AT24C02_Init(void) {
	//初始化IIC总线使用的两个引脚：PB6,PB7，分别作为SCL和SDA
	IIC_Init();
}

//定义读1字节数据函数
//形参ReadAddr:指定要读取的片内存储空间的起始地址
//返回值：返回存储的数据
//时序：ST->0x50<<1|0->(ACK)->ReadAddr->(ACK)->SR->0x50<<1|1->(ACK)->(Data)->NACK->SP
//目标：就是利用封装好的时序函数将整个时序搭建起来，类似搭积木一样
u8 AT24C02_ReadByte(u16 ReadAddr) {
	u8 temp = 0;
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID<<1|0);
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID<<1|1);
	IIC_Wait_Ack();
	temp = IIC_Read_Byte(0);	//形参0:让CPU发送NACK,1:让CPU发送ACK
	IIC_Stop();
	return temp; 
}

//定义写1字节数据函数
//形参WriteAddr：指定要写入的片内存储空间的起始地址
//形参Data：指定要写入的数据
//时序：ST->0x50<<1|0->(ACK)->WriteAddr->(ACK)->Data->(ACK)->SP
void AT24C02_WriteByte(u16 WriteAddr, u8 Data) {
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(Data);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(10);
}

//定义连续多字节读取函数
//时序：ST->0x50<<1|0->(ACK)->ReadAddr起始地址->(ACK)->SR->0x50<<1|1->(ACK)
//->(DATA1)->ACK->(DATA2)->ACK->(DATA3)->ACK->(DATA4)->...->(DATAn)->NACK->SP
void AT24C02_ReadBlockData(u16 ReadAddr, u8 *pBuffer, u16 Len) {
	//方法2：
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID<<1|0);
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID<<1|1);
	IIC_Wait_Ack();
	while(Len) {
		if(Len == 1)
			*pBuffer = IIC_Read_Byte(0);	//读取最后1字节数据CPU发送NACK
		else
			*pBuffer = IIC_Read_Byte(1);	//前n-1字节数据读取完毕CPU发送ACK
		Len--;
		pBuffer++;
	}
	IIC_Stop();
}

//定义连续多字节写入函数
//AT24C02容量：2Kb=256B,分页:1页=8字节,共32页
//注意：AT24C02如果按页连续写入,只能一次连续写入一页,如果地址超过一页范围
//也就是跨页写入,会出现数据重新覆盖原先的数据,所以如果超过1页,跨页了,只需重新
//传输下一页的起始地址即可
//时序：ST->0x50<<1|0->(ACK)->WriteAddr->(ACK)->DATA1->(ACK)->DATA2->(ACK)->..->SP
void AT24C02_WriteBlockData(u16 WriteAddr, u8 *pBuffer, u16 Len) {
	//方法2
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID<<1|0);
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);
	IIC_Wait_Ack();
	while(Len--) {
		IIC_Send_Byte(*pBuffer);
		IIC_Wait_Ack();
		pBuffer++;
		WriteAddr++;		//由于硬件上自动地址加1,这里的软件加1的目的是为了判断是否跨页
		if(WriteAddr % 8 == 0) {
			IIC_Stop();
			delay_ms(5);
			//重新开始写入下一页数据
			IIC_Start();
			IIC_Send_Byte(AT24C02_ID<<1|0);
			IIC_Wait_Ack();
			IIC_Send_Byte(WriteAddr);
			IIC_Wait_Ack();
		}
	}
	IIC_Stop();
	delay_ms(5);
}

//定义读1字节命令处理函数
void AT24C02_ReadOne(void) {
	printf("Read Data is: %#x\n", AT24C02_ReadByte(0x00));//读取0x00地址空间存储的数据
}

//定义写1字节命令处理函数
void AT24C02_WriteOne(void) {
	AT24C02_WriteByte(0x00, 0xaa);	//向0x00地址空间写入数据0xaa
}

//定义连续多字节读取命令处理函数
void AT24C02_ReadMul(void) {
	u8 Data[5] = {0};
	//从0x00地址连续读取5字节数据保存到数组Data中
	AT24C02_ReadBlockData(0x00, Data, ARRAY_SIZE(Data));
	//打印输出
	int i;
	for(i = 0; i < ARRAY_SIZE(Data); i++)
		printf("Addr[%d]: Data[%#x]\n", i, Data[i]);
}

//定义连续多字节的写入处理函数
void AT24C02_WriteMul(void) {
	u8 Data[5] = {1, 2, 3, 4, 5};
	//从0x00地址连续写入5字节数据
	AT24C02_WriteBlockData(0x00, Data, ARRAY_SIZE(Data));
}





