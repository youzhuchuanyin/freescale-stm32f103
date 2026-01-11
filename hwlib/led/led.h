#ifndef __LED_H__
#define __LED_H__
#ifdef __cplusplus
extern "C"{
#endif

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>




#define DEBUG
#ifdef DEBUG
#define pr_debug(fmt,...) printf(fmt,##__VA_ARGS__)
#else 
#define pr_debug(fmt,...)
#endif

#define LED_DEVFILE_NAME "/dev/LED"
#define LED_ON 0x1000
#define LED_OFF 0x1001
struct user_led{
  //int cmd;
    int state;
    int index;
};
extern int led_open(void);//open()
extern void led_close(void);//close()
extern long led_config(int index,int cmd);//inctl()




#ifdef __cplusplus
}
#endif
#endif
