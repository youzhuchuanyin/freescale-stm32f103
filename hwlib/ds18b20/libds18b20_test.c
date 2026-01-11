#include "ds18b20.h"
int main(void){
    float temperature;
    int ret;
    ret=ds18b20_open();
    if(ret<0){
        pr_debug("open file error\n");
    }
    else{
        pr_debug("open file success\n");
    }
    while(1){
        
        temperature=ds18b20_read();
        printf("temperature is %.3f\n",temperature);
        sleep(1);
    }
    ds18b20_close();
    return 0;
}
