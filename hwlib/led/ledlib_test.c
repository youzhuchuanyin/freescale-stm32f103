#include"led.h"
int main(int argc,char* argv[]){
    int cmd;
    if(argc!=3){
        printf("usage : %s <on / off>  <0/1/2/3>\n",argv[0]);
    }
    led_open();
    if(strcmp(argv[1],"on")!=0){
        cmd=0;
    }else{
        cmd=1;
    }
    int index=strtoul(argv[2],NULL,0);
    led_config(index,cmd);
    led_close();
    return 0;
}
