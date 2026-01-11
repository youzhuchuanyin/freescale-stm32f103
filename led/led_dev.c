#include <linux/init.h>
#include<linux/module.h>
#include<linux/gpio.h>
#include<linux/platform_device.h>
#include<mach/platform.h>
struct dev_led {
    int gpio;
    char* name;
};
static struct dev_led devLed[]={
    {.gpio=PAD_GPIO_C+7,.name="led3"},
    {.gpio=PAD_GPIO_C+11,.name="led2"},
    {.gpio=PAD_GPIO_C+12,.name="led4"},
    {.gpio=PAD_GPIO_B+26,.name="led1"},
};
struct seal_dev_led{
    int size;
    struct dev_led* dev_led;
};
static struct seal_dev_led sealDevLed={
    .dev_led=devLed,
    .size=ARRAY_SIZE(devLed),
};
static struct platform_device platDevLed={
    .name="itemLED",
    .id=-1,
    .dev={.platform_data=&sealDevLed},
};

static int led_dev_init(void){
    platform_device_register(&platDevLed);
    return 0;
}
static void led_dev_exit(void){
    platform_device_unregister(&platDevLed);
}

module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");
