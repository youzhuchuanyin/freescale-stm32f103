#ifndef __MSG_H
#define __MSG_H

//指定消息类型id
#define O_X6818_STM32_LED_REQ		1
#define O_STM32_X6818_LED_RSP		2

#define O_STM32_X6818_TEMP_REQ		3
#define O_X6818_STM32_TEMP_RSP		4

#define O_X6818_STM32_BEEP_REQ      5
#define O_STM32_X6818_BEEP_RSP      6

#define O_X6818_STM32_MOTOR_REQ 	7
#define O_STM32_X6818_MOTOR_RSP 	8

#define O_STM32_X6818_MODBUS_REQ 	9
#define O_X6818_STM32_MODBUS_RSP 	10
//电机操作命令
#define MOTOR_FORWARD				1
#define MOTOR_BACK					2
#define MOTOR_LEFT					3
#define MOTOR_RIGHT					4
#define MOTOR_ACCELERATE		5
#define MOTOR_SLOWdOWN			6
#define MOTOR_STOP					7

//声明消息头结构体
typedef struct {
	unsigned int msgid;		//消息ID
	unsigned int msglen;	//消息长度
}msghead_t;

//声明LED灯开关命令信息(X6818->STM32)
typedef struct {
	msghead_t msgh;			//消息头
	unsigned int index;		//灯的编号:1/2
	unsigned int cmd;		//开关命令:1开/0关
}led_req_t;

//声明LED开关反馈信息(STM32->X6818)
typedef struct {
	msghead_t msgh;			//消息头
	unsigned int success;	//0:成功，1 失败
}led_rsp_t;
//声明BEEP开关命令信息(X6818->STM32)
typedef struct {
	msghead_t msgh;			//消息头
	unsigned int index;//没用但保持一致
	unsigned int cmd;		//开关命令:1开/0关
}beep_req_t;

//声明BEEP开关反馈信息(STM32->X6818)
typedef struct {
	msghead_t msgh;			//消息头
	unsigned int success;	//0:成功，1 失败
}beep_rsp_t;

//声明温度上报信息(STM32->X6818)
typedef struct {
	msghead_t msgh;
	float temp;
}temp_req_t;

//声明温度上报反馈信息(X6818->STM32)
typedef struct {
	msghead_t msgh;
	unsigned int success;
}temp_rsp_t;

typedef struct {
	msghead_t msgh;
	unsigned int cmd;
}motor_req_t;

//声明电机操作反馈信息(STM32->X6818)
typedef struct {
	msghead_t msgh;
	unsigned int success;
}motor_rsp_t;
//modbus
typedef struct{
	msghead_t msg;
	float temp;
	float moisture;
}modbus_req_t;
#endif
