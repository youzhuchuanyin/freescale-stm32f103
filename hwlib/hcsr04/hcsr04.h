#ifndef __HCSR04_H__
#define __HCSR04_H__

#ifdef __cplusplus
extern "C" {
#endif

    /*头文件*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

    /*调试宏*/
#define DEBUG

#ifdef DEBUG
#define pr_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...) 
#endif

    /*设备文件名*/
#define HCS04_DEVFILE_NAME  "/dev/hcsr"
#define GET_DISTANCE      0x2004
#define MAX_DISTANCE    30

    extern int distance_open(void);
    extern void distance_close(void);
    extern int get_distance(void);
#ifdef __cplusplus
}
#endif

#endif
