#include<linux/init.h>
#include<linux/module.h>
#include<linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include<linux/platform_device.h>
#include<mach/platform.h>
#define LED_ON 0x1000
#define LED_OFF 0x1001
struct user_led{
    //int cmd;
    int state;
    int index;
};
static struct user_led userLed;
struct dev_led {
    int gpio;
    char* name;
};
struct seal_dev_led{
    int size;
    struct dev_led* dev_led;
};
static struct seal_dev_led* psealDevLed;
long led_ioctl (struct file *file, unsigned int cmd, unsigned long
        buf){
    copy_from_user(&userLed,(struct user_led*)buf,sizeof(userLed));
    switch (cmd){
        case LED_ON:
            gpio_set_value(psealDevLed->dev_led[userLed.index].gpio,0);
            break;
        case LED_OFF:
            gpio_set_value(psealDevLed->dev_led[userLed.index].gpio,1);
            break;
    };
    userLed.state=gpio_get_value(psealDevLed->dev_led[userLed.index].gpio);
    copy_to_user((struct user_led*)buf,&userLed,sizeof(userLed));
    return 0;
}
static struct file_operations fops={
    .unlocked_ioctl=led_ioctl,
};
static struct miscdevice miscDev={
    .fops=&fops,
    .name="LED",
    .minor=MISC_DYNAMIC_MINOR,
};
static int plat_probe(struct platform_device* platDevLed){
    psealDevLed=platDevLed->dev.platform_data;
    int i;
    for(i=0;i<psealDevLed->size;i++){
        gpio_request(psealDevLed->dev_led[i].gpio,psealDevLed->dev_led[i].name);
        gpio_direction_output(psealDevLed->dev_led[i].gpio,1);
    };
    misc_register(&miscDev);
    return 0;
} 
static int plat_remove(struct platform_device* platDevLed){
    psealDevLed=platDevLed->dev.platform_data;
    int i;
    misc_deregister(&miscDev);
    for(i=0;i<psealDevLed->size;i++){
        gpio_free(psealDevLed->dev_led[i].gpio);
    }
    return 0;
}
static struct platform_driver platDrvLed={
    .driver={.name="itemLED"},
    .probe=plat_probe,
    .remove=plat_remove,
};
static int led_drv_init(void){
    platform_driver_register(&platDrvLed);
    return 0;
}
static void led_drv_exit(void){
    platform_driver_unregister(&platDrvLed);
}
module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");
