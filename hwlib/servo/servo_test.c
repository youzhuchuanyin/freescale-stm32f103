#include"servo.h"
int main (void){
    servo_init();
    printf("servo_init success\n");
    
        servo_set_position(1,500,200);
        printf("setposition success\n");
        sleep(1);
        servo_get_position(1);
        printf("getposition success\n");
        sleep(1);

        servo_set_position(1,500,400);
        sleep(1);
        servo_get_position(1);
    
    return 0;
}
