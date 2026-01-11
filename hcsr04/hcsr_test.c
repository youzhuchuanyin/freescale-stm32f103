#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define GET_DISTANCE      0x10020

int main(void)
{
    int fd;
    int time;
    
    fd=open("/dev/hcsr",O_RDWR);
    if(fd < 0){
            printf("open faild\n");
            return -1;
    }

    while(1){   
        ioctl(fd, GET_DISTANCE,&time);
        float distance=time*34000/1000000/2;
        printf("distance:%lf\n",distance);
        sleep(1);
    }

    close(fd);
    return 0;
}
