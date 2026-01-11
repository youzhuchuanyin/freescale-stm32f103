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

static struct hcsr04_resource hcsr04_info={
        .name = "hcsr",
        .trig = PAD_GPIO_B + 6,
        .echo = PAD_GPIO_B + 10
};

static void hcsr_release(struct device *dev){}

static struct platform_device hcsr_dev={
    .name="hcsr",
    .id=-1,
    .dev={
        .platform_data=&hcsr04_info,
        .release=hcsr_release
    }

};
static int hcsr04_dev_init(void){
    platform_device_register(&hcsr_dev);
    return 0;
}

static void hcsr04_dev_exit(void){
    platform_device_unregister(&hcsr_dev);
}

module_init(hcsr04_dev_init);
module_exit(hcsr04_dev_exit);
MODULE_LICENSE("GPL");
