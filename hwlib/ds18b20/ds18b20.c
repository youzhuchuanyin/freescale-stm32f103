#include "ds18b20.h"
static int fd ;
int ds18b20_open(void){
    fd=open(DS18B20_DEVFILE_NAME,O_RDWR);
    if(fd<0){
        return -1;
    }
    return 0;
}

void ds18b20_close(void){
  close(fd);
}

float ds18b20_read(void){
    int buf;
    float n_buf;
    read(fd,&buf,sizeof(buf));
    n_buf=(float)buf/10000;
    return n_buf;
}
