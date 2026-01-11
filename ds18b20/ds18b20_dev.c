#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <mach/platform.h>

struct dev_ds18b20 {
    int gpio;
    char* name;
};
static struct dev_ds18b20 devDs18b20={
    .gpio=PAD_GPIO_B+8,
    .name="ds18b20",
};
static struct platform_device platDevDs18b20={
    .name="ItemDs18b20",
    .id=-1,
    .dev={.platform_data=&devDs18b20,},
};

static int ds18b20_dev_init(void){
    platform_device_register(&platDevDs18b20);
    return 0;
}
static void ds18b20_dev_exit(void){
    platform_device_unregister(&platDevDs18b20);
}
module_init(ds18b20_dev_init);
module_exit(ds18b20_dev_exit);
MODULE_LICENSE("GPL");
