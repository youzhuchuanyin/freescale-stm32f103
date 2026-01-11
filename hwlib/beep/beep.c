#include"beep.h"
static int fd;
int beep_open(void){
    fd=open(BEEP_FILE_NAME,O_RDWR);
    if(fd<1){
        pr_debug("open beep file error\n");
        return -1;
    }
    else{
        pr_debug("open beep file success\n");
    }
    return 0;
};
void beep_close(void){
    close(fd);
}
static struct user_beep userBeep;
long  beep_ctrl(int cmd){
    long ret;
    switch (cmd){
        case 0:
            ret=ioctl(fd,BEEP_OFF,&userBeep);
            break;
        case 1:
            ret=ioctl(fd,BEEP_ON,&userBeep);
            break;
    }
    if(ret<0){
        pr_debug("ioctl beep failed\n");
    }
    else{
        pr_debug("ioctl beep success\n");
    }
    return 0;

}
