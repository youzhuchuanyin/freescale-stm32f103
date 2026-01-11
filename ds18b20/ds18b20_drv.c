#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h> 
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/delay.h>
struct dev_ds18b20 {
    int gpio;
    char* name;
};
static struct dev_ds18b20* pdevDs18b20;
void ds18b20_reset(void){
    int ret;
unsigned long flags;
    local_irq_save(flags);
    gpio_direction_output(pdevDs18b20->gpio,0);
    udelay(500);
    gpio_direction_output(pdevDs18b20->gpio,1);
    udelay(30);
    gpio_direction_input(pdevDs18b20->gpio);
    ret=gpio_get_value(pdevDs18b20->gpio);
    udelay(500);
    if(ret!=0){
        printk("ds18b20 not fount\n ");
    }else{
        printk("ds18b20 found\n");
    }
     local_irq_restore(flags);
}
void cpu_write(unsigned char data){
    int i;
unsigned long flags;
        local_irq_save(flags);
    for(i=0;i<8;i++){
    if(data&1){
        gpio_direction_output(pdevDs18b20->gpio,0);
        udelay(3);
        gpio_direction_output(pdevDs18b20->gpio,1);
        udelay(70);
    }else{
        gpio_direction_output(pdevDs18b20->gpio,0);
        udelay(70);
        gpio_direction_output(pdevDs18b20->gpio,1); 
        udelay(3);
    }
    data>>=1;
    }
 local_irq_restore(flags);
}
unsigned char cpu_read(void){
    unsigned char data=0;
    unsigned char ret;
    int i;
unsigned long flags;
        local_irq_save(flags);
    for(i=0;i<8;i++){
    gpio_direction_output(pdevDs18b20->gpio,0);
    udelay(3);
    gpio_direction_input(pdevDs18b20->gpio);
    ret=gpio_get_value(pdevDs18b20->gpio);
    data|=(ret<<i);
    udelay(60);
    }
local_irq_restore(flags);
    return data;
}

static ssize_t read_ds18b20(struct file *file,
        char __user *buf,
        size_t count,
        loff_t *ppos) {
    unsigned char temp0;
    unsigned char temp1;
     int  temp;
    ds18b20_reset();
    cpu_write(0xcc);
    cpu_write(0x44);
    ds18b20_reset();
    cpu_write(0xcc);
    cpu_write(0xbe);
    temp0=cpu_read();
    temp1=cpu_read();
    temp=(temp1<<8|temp0);
    temp*=625;
    
    copy_to_user(buf,&temp,sizeof(temp));
    return count;
}
static struct file_operations fops_ds18b20={
    .read=read_ds18b20,
};
static struct miscdevice misc_ds18b20={
    .minor=MISC_DYNAMIC_MINOR,
    .name="ds18b20",
    .fops=&fops_ds18b20,
};

static int ds18b20_probe(struct platform_device * pplatDevDs18b20){
    pdevDs18b20=pplatDevDs18b20->dev.platform_data;
    gpio_request(pdevDs18b20->gpio,pdevDs18b20->name);
    gpio_direction_output(pdevDs18b20->gpio,1);
    misc_register(&misc_ds18b20);
    return 0;
}
static int ds18b20_remove(struct platform_device * pplatDevDs18b20){
    misc_deregister(&misc_ds18b20);
    gpio_free(pdevDs18b20->gpio);
    return 0;
}
static struct platform_driver platDrvDs18b20={
    .driver={.name="ItemDs18b20",},
    .probe=ds18b20_probe,
    .remove=ds18b20_remove,
};
static int ds18b20_drv_init(void){
    platform_driver_register(&platDrvDs18b20);
    return 0;
}
static void ds18b20_drv_exit(void){
    platform_driver_unregister(&platDrvDs18b20);
}
module_init(ds18b20_drv_init);
module_exit(ds18b20_drv_exit);
MODULE_LICENSE("GPL");
