#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"
#include "system.h"
#include "uart3.h"

#define WIFI_RESET				PAout(15)					//ESP8266复位引脚的位带操作,输出低电平复位

#define WIFI_Printf				UART3_Puts				//指定采用串口3给WIFI发送数据信息
#define WIFI_RxCounter		UART3_RxCounter		//记录通过串口3接收到的WIFI数据个数
#define WIFI_RxBuff				UART3_RxBuff			//指定串口3接收缓冲区的首地址
#define WIFI_RXBUFF_SIZE	UART3_RXBUFF_SIZE	//指定串口3接收缓冲区的大小

#define SSID							"2704"						//指定路由器的名称
#define PASSWD						"18086088095"				//指定路由器的密码
#define ServerIP					"192.168.10.8"		//指定上位机服务器的IP地址
#define ServerPort				8080							//指定服务器的端口号

extern void WIFI_ResetIO_Init(void);				//声明WIFI复位引脚的初始化
extern char WIFI_SendCmd(char *cmd, int timeout);	//声明通过串口3给WIFI发送AT指令
extern char WIFI_Reset(int timeout);							//声明WIFI复位函数,拉低GPIOA15
extern char WIFI_JoinAP(int timeout);							//声明WIFI连接路由器的函数
extern char WIFI_Connect_Server(int timeout);			//声明连接服务器的函数
extern char WIFI_Connect_IOTServer(void);					//声明配置WIFI,连接路由器,连接服务器的总处理函数

extern void WIFI_Connect(void);										//测试WIFI连接命令处理函数
extern void WIFI_Send_Data(void);									//测试WIFI发送数据命令处理函数
#endif
