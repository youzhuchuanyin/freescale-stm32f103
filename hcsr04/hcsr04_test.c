#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    int fd;
    unsigned int time;

    fd=open("/dev/hcsr",O_RDWR);
    if(fd < 0){
        printf("open faild\n");
        return -1;
    }

    while(1){   
        ioctl(fd,0x2004,&time);
        float distance=time*34000/1000000/2;
        //printf("distance:%lf\n",distance);
        printf("Time: %u μs, Distance: %.2f cm\n", time, distance);
        sleep(1);
    }

    close(fd);
    return 0;
}

