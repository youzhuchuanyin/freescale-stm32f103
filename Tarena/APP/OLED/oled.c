#include "oled.h"
#include "systick.h"
#include "oledfont.h"  
#include "ds18b20.h"
#include "stdio.h"
#include "key.h"
#include "led.h"
#include "beep.h"

//向SSD1106写入一个字节。
//data:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 data,u8 cmd)
{	
	u8 i;			  
	OLED_DC=cmd;  			  
	OLED_CS=0;
	for(i=0;i<8;i++)
	{			  
		OLED_CLK=0;
		if(data&0x80)
			OLED_DATA=1;
		else 
		  OLED_DATA=0;
		OLED_CLK=1;
		data<<=1;   
	}				 		  
	OLED_CS=1;  	  
} 

void OLED_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	 
 	GPIO_SetBits(GPIOD,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);	//拉高电平
	//复位
	OLED_RST=1;
	delay_ms(100);
	OLED_RST=0;
	delay_ms(100);
	OLED_RST=1; 
			
	OLED_WR_Byte(0xAE,OLED_CMD_F);//关闭显示
     
  OLED_WR_Byte(0x00,OLED_CMD_F);//X轴低位，起始X轴为0
  OLED_WR_Byte(0x10,OLED_CMD_F);//X轴高位
  OLED_WR_Byte(0x40,OLED_CMD_F);//Y轴，可设区间[0x40,0x7F]，设置为0了
     
 
  OLED_WR_Byte(0xA1,OLED_CMD_F);//设置X轴扫描方向，0xa0左右反置 ，0xa1正常（左边为0列）
  OLED_WR_Byte(0xC8,OLED_CMD_F);//设置Y轴扫描方向，0xc0上下反置 ，0xc8正常（上边为0行）
  OLED_WR_Byte(0xA6,OLED_CMD_F);//位值表示的意义，0xa6表示正常，1为点亮，0为关闭，0xa7显示效果相反
     
 
  OLED_WR_Byte(0x81,OLED_CMD_F);//命令头，调节亮度,对比度,变化很小，但是仔细可以观察出来
  OLED_WR_Byte(0xFF,OLED_CMD_F);//可设置区间[0x00,0xFF]
     
  OLED_WR_Byte(0xA8,OLED_CMD_F);//命令头，设置多路复用率(1 to 64)
  OLED_WR_Byte(0x3f,OLED_CMD_F);//--1/64 duty
     
  OLED_WR_Byte(0xD3,OLED_CMD_F);//命令头，设置显示偏移移位映射RAM计数器(0x00~0x3F)
  OLED_WR_Byte(0x00,OLED_CMD_F);//不偏移
     
  OLED_WR_Byte(0xd5,OLED_CMD_F);//命令头，设置显示时钟分频比/振荡器频率
  OLED_WR_Byte(0x80,OLED_CMD_F);//设置分割比率，设置时钟为100帧/秒
     
  OLED_WR_Byte(0xD9,OLED_CMD_F);//命令头，--set pre-charge period
  OLED_WR_Byte(0xF1,OLED_CMD_F);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
     
  OLED_WR_Byte(0xDA,OLED_CMD_F);//命令头，--set com pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD_F);
     
  OLED_WR_Byte(0xDB,OLED_CMD_F);//命令头，--set vcomh
  OLED_WR_Byte(0x40,OLED_CMD_F);//Set VCOM Deselect Level
     
  OLED_WR_Byte(0x20,OLED_CMD_F);//命令头，设置寻址模式
  OLED_WR_Byte(0x10,OLED_CMD_F);//页面寻址模式(重置) (0x00/0x01/0x02)
     
  OLED_WR_Byte(0x8D,OLED_CMD_F);//命令头，--set Charge Pump enable/disable
  OLED_WR_Byte(0x14,OLED_CMD_F);//--set(0x10) disable
     
  OLED_WR_Byte(0xA4,OLED_CMD_F);//恢复到RAM内容显示(重置)    						   
	OLED_WR_Byte(0xAF,OLED_CMD_F); //开启显示	  
	
	OLED_Clear(0x00);   
	//OLED_Draw_String(0, 0, "hello,world");
}
  		
void OLED_Draw_Point(u8 page, u8 col, u8 data) {
	OLED_WR_Byte (0xb0+page,OLED_CMD_F);
	OLED_WR_Byte (0x00+(col&0x0F),OLED_CMD_F); 
	OLED_WR_Byte (0x10+((col>>4)&0x0F),OLED_CMD_F);
	OLED_WR_Byte(data,OLED_DATA_F);
}

void OLED_Clear(u8 color)  {  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  {  
		OLED_WR_Byte (0xb0+i,OLED_CMD_F);    	//从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD_F);      	//列低地址
		OLED_WR_Byte (0x10,OLED_CMD_F);      	//列高地址  
		for(n=0;n<128;n++)
			OLED_WR_Byte(color,OLED_DATA_F); 				//清屏
	}
}

void OLED_Draw_Char(u8 page, u8 col, u8 data) {
	unsigned char n;
	unsigned char ch;
	unsigned int index = data - 32; //获取字符在字符集中的偏移量（因为空格的asc码是32）
	for(n = 0; n < 8; n++) {
			ch = ascii_1608[index][2*n];
			OLED_Draw_Point(page, col + n, ch);
			ch = ascii_1608[index][2*n+1];
			OLED_Draw_Point(page+1, col + n, ch);
	}
}

void OLED_Draw_String(u8 page, u8 col, u8 *pstr) {
	while(*pstr) {
		OLED_Draw_Char(page, col, *pstr);
		col += 8;
		pstr++;
	}
}

void OLED_Draw_Chinese(u8 page, u8 col) {
	unsigned char n;
	unsigned char ch;
	unsigned char *p = (unsigned char *)FontHzk;
	int size = sizeof(FontHzk) / sizeof(FontHzk[0][0]);
	for(n = 0; n < size/2; n++) {
			ch = p[2*n];
			OLED_Draw_Point(page, col + n, ch);
			ch = p[2*n+1];
			OLED_Draw_Point(page+1, col + n, ch);
	}
}

