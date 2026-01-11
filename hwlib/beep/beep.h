#ifndef __BEEP_H__
#define __BEEP_H__
#ifdef __cplusplus
extern "C"{
#endif

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#define BEEP_FILE_NAME "/dev/beep"
#define BEEP_ON 0X1002
#define BEEP_OFF 0X1003

#define DEBUG
#ifdef DEBUG
#define pr_debug(fmt,...) printf(fmt,##__VA_ARGS__)
#else 
#define pr_debug(fmt,...)
#endif
struct user_beep{
    int state;
};
extern int beep_open(void);
extern void beep_close(void);
extern long beep_ctrl(int cmd);

#ifdef __cplusplus
}
#endif
#endif 
