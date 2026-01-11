#include "hcsr04.h"
static int fd;
int distance_open(void)
{
    fd = open(HCS04_DEVFILE_NAME, O_RDWR);
    if (fd < 0) {
        pr_debug("open hc device failed.\n");
        return -1;  
    }
    pr_debug("open hc device successfully!\n");
    return 0;
}

void distance_close(void) 
{
    pr_debug("close hc device...\n");
    close(fd);
}
static int get_some_distance(int *buf,int size)
{
    int i;
    for(i = 0; i < size; i++){
        int time;
        int distance;
        ioctl(fd,GET_DISTANCE,&time);
        distance=time*34000/1000000/2;//cm
        buf[i]=distance;//数组里头就有数据了
        usleep(5000);
    }
    return 0;
}
static int smoothing(int *buf,int size)
{
    int distance;
    int sub[size];
    int i,j;
    for(i=0;i<size;i++){
        int sum=0;
        for(j=0;j<size;j++){
            sum=sum+(buf[i]>buf[j]?buf[i]-buf[j]:buf[j]-buf[i]);//求差的绝对值
        }
        sub[i]=sum;
    }
    int min=sub[0];
    distance=buf[0];
    for(i=0;i<size;i++){
        if(sub[i]<min){
            min=sub[i];
            distance=buf[i];
        }
    }
    return distance;
}
int get_distance(void){
    int distence;
    int b_buf[10]={0};
    int size = sizeof(b_buf)/sizeof(b_buf[0]);
    int distance;
    get_some_distance(b_buf,size);
    distance=smoothing(b_buf,size);//多选一最终确定的距离
    return distance;
}
