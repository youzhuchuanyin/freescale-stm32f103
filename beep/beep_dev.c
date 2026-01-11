#include <linux/init.h>
#include<linux/module.h>
#include<linux/gpio.h>
#include<linux/platform_device.h>
#include<mach/platform.h>
struct device_beep {
    int gpio;
    char* name;
};
static struct device_beep deviceBeep={
    .gpio=PAD_GPIO_C+14,
    .name="beep",
};
static struct platform_device platDevBeep={
    .name="ItemBeep",
    .id=-1,
    .dev={.platform_data=&deviceBeep},
};

static int beep_dev_init(void){
    platform_device_register(&platDevBeep);
    return 0;
} 
static void beep_dev_exit(void){
    platform_device_unregister(&platDevBeep);
}
module_init(beep_dev_init);
module_exit(beep_dev_exit);
MODULE_LICENSE("GPL");
