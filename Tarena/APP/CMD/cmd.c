#include "cmd.h"
#include "led.h"
#include "beep.h"
#include "string.h"
#include "at24c02.h"
#include "ds18b20.h"
#include "dma.h"
#include "timer4.h"
#include "esp8266.h"

//定义初始化硬件操作的命令结构体数组,每个成员保存着对应的命令信息
//将来咱们只需维护此数组即可,main函数一个字也不用动
cmd_t cmd_tbl[] = {
	{"led on", LED_On},				//开灯命令
	{"led off", LED_Off},			//关灯命令
	{"beep on", BEEP_On},			//开蜂鸣器命令
	{"beep off", BEEP_Off},		//关闭蜂蜜器命令
	{"EEPROM R", AT24C02_ReadOne},	//读1字节命令
	{"EEPROM W", AT24C02_WriteOne},	//写1字节命令
	{"EEPROM RS", AT24C02_ReadMul},	//多字节读命令
	{"EEPROM WS", AT24C02_WriteMul}, //多字节写命令
	{"temp", DS18B20_Test},					//测试温度传感器命令
	{"rom", DS18B20_ReadRom},				//获取DS18B20内容ROM值的命令
	{"dma tx test", UART1DMA_Tx_Test},	//UART1的DMA发送功能测试命令
	{"timer4 start", TIMER4_Start},			//启动定时器4命令
	{"timer4 stop", TIMER4_Stop},				//关闭定时器命令
	{"esp connect", WIFI_Connect},			//连接服务器命令
	{"esp send", WIFI_Send_Data}				//给服务器发送数据命令
};

//记录数组元素个数
int cmd_num = sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);

//定义根据上位机发送来的命令名称在上面的命令结构体数组中找到对应的匹配的
//命令结构体成员，并且返回这个命令结构体成员的首地址给main函数
cmd_t *find_cmd(char *name) {
	int i;
	for(i = 0; i < cmd_num; i++) {
		if(!strcmp(name, cmd_tbl[i].name))
			return &cmd_tbl[i]; //匹配成功之后返回匹配的命令成员的首地址
	}
	return 0; //没有找到对应的命令
}
