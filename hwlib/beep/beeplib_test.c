#include "beep.h"
int main(int argc ,char* argv[]){
    int ret;
    int cmd;
    if(argc!=2){
        printf("usage : %s <on|off>\n",argv[0]);
        return -1;
    }
    beep_open();
    if(strcmp(argv[1],"on")!=0){/*off*/
        cmd=0;
    }
    else{
        cmd=1;
    }
    beep_ctrl(cmd);
    beep_close();
    return 0;
}
