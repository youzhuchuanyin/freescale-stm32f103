#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h> 
#include <linux/gpio.h>
#include <mach/platform.h>
#define BEEP_ON 0X1002
#define BEEP_OFF 0X1003
struct device_beep {
    int gpio;
    char* name;
};
struct device_beep * pdeviceBeep;
struct user_beep {
    int state;
};
static struct user_beep userBeep;
long beep_ioctl (struct file *file, unsigned int cmd, unsigned long buf){
    copy_from_user(&userBeep,(struct user_beep*)buf,sizeof(userBeep));
    switch (cmd){
        case BEEP_ON:
            gpio_set_value(pdeviceBeep->gpio,1);
            break;
        case BEEP_OFF:
            gpio_set_value(pdeviceBeep->gpio,0);
            break;
    };
    userBeep.state=gpio_get_value(pdeviceBeep->gpio);
    copy_to_user((struct user_beep*)buf,&userBeep,sizeof(userBeep));
    return 0;
}
static struct file_operations fopsBeep={
    .unlocked_ioctl=beep_ioctl,
};
static struct miscdevice miscBeep={
    .fops=&fopsBeep,
    .minor=MISC_DYNAMIC_MINOR,
    .name="beep",
};
static int beep_probe(struct platform_device * pplatDevBeep){
    pdeviceBeep=pplatDevBeep->dev.platform_data;
    gpio_request(pdeviceBeep->gpio,pdeviceBeep->name);
    gpio_direction_output(pdeviceBeep->gpio,0);
    misc_register(&miscBeep);
    return 0;
}
static int beep_remove(struct platform_device * pplatDevBeep){
    misc_deregister(&miscBeep);
    gpio_free(pdeviceBeep->gpio);
    return 0;
}
static struct platform_driver platDrvBeep={
    .driver={.name="ItemBeep",},
    .probe=beep_probe,
    .remove=beep_remove,
};
static int beep_drv_init(void){
    platform_driver_register(&platDrvBeep);
    return 0;
}
static void beep_drv_exit(void){
    platform_driver_unregister(&platDrvBeep);
}
module_init(beep_drv_init);
module_exit(beep_drv_exit);
MODULE_LICENSE("GPL");
