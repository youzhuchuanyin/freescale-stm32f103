#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include "system.h"

//GND  地线
//VCC  电源
//D0   时钟  				PD6 
//D1   数据  				PD7
//RES  复位  				PD4
//DC   命令数据控制	PD5
//CS   片选					PD3 

#define OLED_CS   	PDout(3)		
#define OLED_RST  	PDout(4)
#define OLED_DC   	PDout(5)
#define OLED_CLK  	PDout(6)
#define OLED_DATA  	PDout(7)

#define OLED_CMD_F  0	
#define OLED_DATA_F 1	    						  

extern void OLED_Init(void);
extern void OLED_WR_Byte(u8 data,u8 cmd);	  
extern void OLED_Draw_Point(u8 page, u8 col, u8 data);
extern void OLED_Draw_Char(u8 page, u8 col, u8 ch);
extern void OLED_Draw_String(u8 page, u8 col, u8 *pst);
extern void OLED_Draw_Chinese(u8 page, u8 col);
extern void OLED_Draw_Rect(u8 page, u8 col, u8 len);

extern void OLED_Clear(u8 color);
extern void OLED_Test(void);

extern int show_temp_flag;
#endif
