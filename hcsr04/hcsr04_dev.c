#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/platform_device.h>
struct hcsr04_resource{
    char *name;
    int trig;
    int echo;
};
static struct hcsr04_resource hcsr04_dev={
    .trig=PAD_GPIO_B+6,
    .echo=PAD_GPIO_B+10,
    .name="hcsr",
};
static struct platform_device hcsr04_plat_dev={
    .name="hcsr04",
    .id=-1,
    .dev={.platform_data=&hcsr04_dev}
};

static int hcsr04_init(void){
    platform_device_register(&hcsr04_plat_dev);
    return 0;
}
static void hcsr04_exit(void){
    platform_device_unregister(&hcsr04_plat_dev);
}
module_init(hcsr04_init);
module_exit(hcsr04_exit);
MODULE_LICENSE("GPL");
