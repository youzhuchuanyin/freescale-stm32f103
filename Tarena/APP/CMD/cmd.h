#ifndef __CMD_H
#define __CMD_H

#include "stm32f10x.h"
#include "system.h"

//声明函数指针类型并且取别名叫cb_t,将来保存命令对应的函数首地址
typedef void (*cb_t)(void);

//声明描述命令属性的结构数据类型并且取别名叫cmd_t
typedef struct _cmd {
	char *name;			//描述命令的名称，例如："led on"
	cb_t callback;	//描述命令的处理函数,例如：LED_On();
}cmd_t;

//声明根据上位机发送来的命令名称找到对应的命令结构体变量，并且返回这个变量的首地址
extern cmd_t *find_cmd(char *name);
#endif
