#include "esp8266.h"
#include "systick.h"
#include "stdio.h"
#include "string.h"

//定义WIFI复位引脚GPIOA15的初始化函数
void WIFI_ResetIO_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//使能GPIOA控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//由于GPIOA15上电默认作为JTAG下载器的下载功能,不是作为普通的GPIO功能,所以只需禁止JTAG功能即可
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //此时GPIOA15就可以作为普通的GPIO功能了
	
	//配置GPIOA15为推挽输出
	GPIO_Config.GPIO_Pin = GPIO_Pin_15;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//默认拉高不复位,拉低GPIOA15才复位
	WIFI_RESET = 1;
}

//定义WIFI复位函数:将GPIOA15拉低延时500ms即可实现硬件复位
/*strstr库函数的作用：
char *pstring = "hello,world";
strstr(pstring, "world");
如果找到了,strstr返回world字符串在pstring的首地址
如果没有找到,返回NULL
*/
char WIFI_Reset(int timeout) {
	WIFI_RESET = 0;			//拉低启动复位
	delay_ms(500);
	WIFI_RESET = 1;			//拉高禁止复位

	while(timeout--) {
		delay_ms(100);
		if(strstr(WIFI_RxBuff, "ready"))		//此时WIFI_RxBuff就是串口3接收缓冲区
			break;
		printf("%d ", timeout);
	}
	printf("\r\n");
	if(timeout <= 0)
		return 1;				//说明等待ready超时,复位失败返回1
	return 0;					//复位成功
}

//定义STM32通过串口3给8266WIFI模块发送AT指令的处理函数
//例如：WIFI_SendCmd("AT+RST", 50);
char WIFI_SendCmd(char *cmd, int timeout) {
	WIFI_RxCounter = 0;													//先将串口3接收数据个数清0
	memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);		//然后将串口3接收换缓冲区清0
	WIFI_Printf("%s\r\n", cmd);									//然后通过串口3将AT指令发送给WIFI

	//一旦AT指令发送给8266模块之后,8266就会通过串口3给STM32发送反馈信息,例如"OK"
	//此时STM32会将反馈信息保存到串口3的接收缓冲区中,接下来只需判断接收到的
	//反馈信息中是否有OK字符串即可
	while(timeout--) {
		delay_ms(100);
		if(strstr(WIFI_RxBuff, "OK"))
			break;
		printf("%d ", timeout);
	}
	printf("\r\n");
	if(timeout <= 0)
		return 1;																//给WIFI发送的AT指令,对方无反馈失败
	return 0;																	//AT指令发送成功
}

//定义连接路由器函数
char WIFI_JoinAP(int timeout) {
	WIFI_RxCounter = 0;												//先将串口3接收缓冲区接收到的反馈信息计数清0
	memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);	//然后将串口3接收缓冲区清0等待下一次反馈信息
	WIFI_Printf("AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD); //通过串口3给WIFI发送连接路由器的AT指令
	//如果连接成功,8266通过串口3会给STM32发送反馈信息,然后将反馈信息保存到
	//串口3的接收缓冲区中，接下来只需判断接收缓冲区中是否有WIFI GOT IP接口
	while(timeout--) {
		delay_ms(1000);
		if(strstr(WIFI_RxBuff, "WIFI GOT IP"))
			break;
		printf("%d ", timeout);	
	}
	printf("\r\n");
	if(timeout <= 0)
		return 1;					//连接失败
	return 0;						//连接成功
}

//定义连接服务器函数
char WIFI_Connect_Server(int timeout) {
	WIFI_RxCounter = 0;												//先将串口3接收缓冲区接收到的反馈信息计数清0
	memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);	//然后将串口3接收缓冲区清0等待下一次反馈信息
	WIFI_Printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n3333333333", ServerIP, ServerPort); //通过串口3发送连接服务器AT指令
	printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n1111111111", ServerIP, ServerPort); 
	while(timeout--) {
		delay_ms(100);
		if(strstr(WIFI_RxBuff, "CONNECT"))
			break;
		printf("%d ", timeout);	
	}
	printf("\r\n");
	if(timeout <= 0)
		return 1;					//连接失败
	else { //连接成功,然后发送AT+CIPSEND指令跟服务器可以进行数据传输了
		printf("Connect Server Successfully, Start send data\r\n");
		WIFI_RxCounter = 0;												
		memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);
		WIFI_Printf("AT+CIPSEND\r\n");	//发送数据传输的AT指令
		while(timeout--) {
			delay_ms(100);
			printf("%s\r\n", WIFI_RxBuff);
			if(strstr(WIFI_RxBuff, "\r\nOK\r\n\r\n>"))
				break;
			printf("%d ", timeout);
		}
		printf("\r\n");
		if(timeout <= 0)
			return 2;
	}
	return 0;
}

//定义配置WIFI,连接路由器,连接服务器总处理函数
char WIFI_Connect_IOTServer(void) {
	//1.复位WIFI模块
	if(WIFI_Reset(50)) {
		printf("WIFI Reset Failed, Please Try Again\r\n");
		return 1;
	}
	printf("WIFI Reset Successfully!\r\n");
	
	//2.设置STA模块:WIFI连接路由器
	if(WIFI_SendCmd("AT+CWMODE=1", 50)) {
		printf("Set WIFI STA Mode Failed\r\n");
		return 2;
	}
	printf("Set STA Mode Successfully!\r\n");
	
	//3.取消自动连接
	if(WIFI_SendCmd("AT+CWAUTOCONN=0", 50)) {
		printf("Cancel Auto Connect Failed\r\n");
		return 3;
	}
	printf("Cancel Auto Connect Successfully!\r\n");
	
	//4.连接路由器
	if(WIFI_JoinAP(30)) {
		printf("Connect router Faild\r\n");
		return 4;
	}
	printf("Connect Router Successfully!\r\n");
	delay_ms(1000);
	delay_ms(1000);
	
	//5.设置透传模式
	if(WIFI_SendCmd("AT+CIPMODE=1", 50)) {
		printf("Set Direct Mode Failed\r\n");
		return 5;
	}
	printf("Set Direct Mode Successfully!\r\n");
	
	//6.设置单连接模式
	if(WIFI_SendCmd("AT+CIPMUX=0", 50)) {
		printf("Set Single Mode Failed.\r\n");
		return 6;
	}
	printf("Set Single Mode Susccessfully!\r\n");
	
	//7.最终连接服务器
	if(WIFI_Connect_Server(10)) {
		printf("Connect Server Failed\r\n");
		return 7;
	}
	printf("Connect Server Successfully!\r\n");
	return 0; //成功
}

//定义WIFI连接命令处理函数
void WIFI_Connect(void) {
	if(WIFI_Connect_IOTServer())
		WIFI_Connect();		//如果连接失败,重新连接
}

//定义WIFI发送数据函数
void WIFI_Send_Data(void) {
	char *msg = "hello,world";
	WIFI_Printf("%s", msg);		//通过串口3将发送的数据给WIFI传递过去
}













