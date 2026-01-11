#include"led.h"
static int fd;
int led_open(void){
    fd=open(LED_DEVFILE_NAME,O_RDWR);
    if(fd<0){
        pr_debug("open file failed!!!\n");
        return -1;
    }
    else{
        pr_debug("open file success\n");
    }
    return 0;
}

void led_close(void){
    close(fd);
}
struct user_led buf;
long led_config(int index,int cmd){
    long ret;
    buf.index=index;
    pr_debug("index is %d\n",buf.index);
    if(cmd==1){
        ret=ioctl(fd,LED_ON,&buf);
    }
    else{
        ret=ioctl(fd,LED_OFF,&buf);
    }
    if(ret<0){
        pr_debug("ioctl led device failed.\n");
    }
    else{
        pr_debug("ioctl led device sueecss\n");
    }
    return 0;
}


