#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef _cplusplus
extern "C" {
#endif
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>
#include<errno.h>
extern int servo_init(void);
extern int servo_set_position(int id,int time,int position);
extern int servo_get_position(int id);


#ifdef _cplusplus
}
#endif
#endif
