#include "ds18b20.h"
#include "systick.h"
#include "stdio.h"

//定义DS18B20初始化函数
void DS18B20_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//使能GPIOG时钟
	RCC_APB2PeriphClockCmd(DS18B20_PORT_RCC, ENABLE);
	
	//默认初始化为推挽输出
	GPIO_Config.GPIO_Pin = DS18B20_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS18B20_PORT, &GPIO_Config);
}

//定义配置GPIOG11为输入功能的函数
void DS18B20_IO_IN(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = DS18B20_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(DS18B20_PORT, &GPIO_Config);
}

//定义配置GPIOG11为输出功能的函数
void DS18B20_IO_OUT(void) {
	GPIO_InitTypeDef GPIO_Config;

	GPIO_Config.GPIO_Pin = DS18B20_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS18B20_PORT, &GPIO_Config);
}

//定义初始化复位函数
void DS18B20_Reset(void) {
	DS18B20_IO_OUT();			//配置为输出
	DS18B20_DQ_OUT = 0;		//拉低
	delay_us(500);
	DS18B20_DQ_OUT = 1;		//帮助上拉电阻拉高
	delay_us(30);
	
	//检测应答或者握手信号
	u8 retry = 0;
	DS18B20_IO_IN();			//让CPU释放GPIOG11的控制权，将控制权交给外设
	while(DS18B20_DQ_IN && retry < 240) {
		retry++;
		delay_us(1);
	}
	
	if(retry >= 240)
		printf("Reset failed.\n");
	else {
		printf("Reset Successfully!\n");
		retry = 0;
	}
	
	delay_us(500);
}

//定义发送1字节数据函数
void DS18B20_Write_Byte(u8 data) {
	u8 i;
	DS18B20_IO_OUT();					//配置输出准备发送数据
	for(i = 0; i < 8; i++) {
		if(data & 0x01) { 			//写1操作
			DS18B20_DQ_OUT = 0;		//拉低
			delay_us(2);					
			DS18B20_DQ_OUT = 1;		//帮上拉电阻拉高
			delay_us(60);					//此时外设进行采样
		} else {
			DS18B20_DQ_OUT = 0;		//拉低
			delay_us(60);					//此时外设进行采样
			DS18B20_DQ_OUT = 1;		//帮上拉电阻拉高
			delay_us(2);					
		}
		data >>= 1;							//将高位往bit0这位挪动,开始进行下一次bit位传输
	}
}

//定义读取1字节数据函数
u8 DS18B20_Read_Byte(void) {
	u8 i;
	u8 data = 0;							//暂存读取到的1字节数据
	for(i = 0; i < 8; i++) {
		DS18B20_IO_OUT();				//先配输出
		DS18B20_DQ_OUT = 0;			//拉低
		delay_us(2);						//要小于15us
		DS18B20_IO_IN();				//配置为输入释放控制权交给外设或者上拉电阻
		delay_us(8);						//稍微再延时一下
		data |= DS18B20_DQ_IN << i;		//从低位开始一位一位的读取保存到data中
		delay_us(50);						//稍微等待一下继续下一个bit位的操作
	}
	return data;
}

//分配全局内存存储ROM值
static u8 rom[8] = {0};

//定义获取ROM值的命令处理函数
void DS18B20_ReadRom(void) {
	int i;
	//1.复位
	DS18B20_Reset();
	//2.发送读取ROM值的命令
	DS18B20_Write_Byte(0x33);
	//3.连续读取8字节的ROM值保存到数组中
	for(i = 0; i < 8; i++) {
		rom[i] = DS18B20_Read_Byte();
		printf("%#x ", rom[i]);
	}
	printf("\n");
}

//定义发送匹配ROM命令和ROM值的处理函数
void DS18B20_MatchRom(void) {
	int i;
	//1.发送Match ROM命令,进行ROM匹配
	DS18B20_Write_Byte(0x55);
	//2.发送要访问的外设对应的ROM值
	for(i = 0; i < 8; i++)
		DS18B20_Write_Byte(rom[i]);
}

//定义获取温度函数
float DS18B20_GetTemperture(void) {
	u16 temp = 0;			//保存温度值的数字量,只有CPU知道这个温度值,人不知道
	u16 temp_lsb = 0, temp_msb = 0;		//保存byte0和byte1的内存数据，温度的高和低字节
	float value = 0;	//保存温度值的模拟量,给人类看的,CPU不认识	
	#ifdef DS18B20_SKIP_ROM 
	//1.复位
	DS18B20_Reset();
	//2.发送SKIP ROM命令，跳过匹配
	DS18B20_Write_Byte(0xCC);
	//3.发送启动温度转换命令,硬件上自动将采集的温度保存到byte0和byte1内存中
	DS18B20_Write_Byte(0x44);
	//4.复位
	DS18B20_Reset();
	//5.发送SKIP ROM命令，跳过匹配
	DS18B20_Write_Byte(0xCC);
	//6.发送读取片内RAM内存数据的命令
	DS18B20_Write_Byte(0xBE);
	
	#else //否则通过Match rom的方式找到要访问的外设
	
	//1.复位
	DS18B20_Reset();
	//2.发送Match ROM命令和ROM值
	DS18B20_MatchRom();
	//3.发送启动温度转换命令,硬件上自动将采集的温度保存到byte0和byte1内存中
	DS18B20_Write_Byte(0x44);
	//4.复位
	DS18B20_Reset();
	//5.发送Match ROM命令和ROM值
	DS18B20_MatchRom();
	//6.发送读取片内RAM内存数据的命令
	DS18B20_Write_Byte(0xBE);
	#endif
	
	//7.读取温度的低字节和高字节数据
	temp_lsb = DS18B20_Read_Byte();
	temp_msb = DS18B20_Read_Byte();
	//8.将温度的高字节和低字节进行合并得到一个完整的温度值的数字量
	temp = (temp_msb << 8) | temp_lsb;
	//9.换算得到温度的模拟量
	if((temp & 0xF800) == 0xF800) { //负的温度值(0xF800=1111 1000 0000 0000)
		temp = (~temp) +1;	//已知负的二进制换算得到对应的10进制
		value = temp * -0.0625;
	} else {
		value = temp * 0.0625;
	}
	return value;
}

//定义温度传感器的测试函数
void DS18B20_Test(void){
	float temp = 0;
	temp = DS18B20_GetTemperture();
	printf("Current Temp: %.2f\r\n", temp);
}



