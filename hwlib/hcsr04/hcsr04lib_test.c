#include "hcsr04.h"

int main(int argc, char *argv[])
{
    distance_open();
    while(1) {
        printf("%d\n", get_distance());
        sleep(1);
    }
    distance_close();
    return 0;
}

