#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <mach/platform.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h> //kmalloc
struct hcsr04_resource{
    char *name;
    int trig;
    int echo;
};
struct hcsr04_private {
    int read_ready;//准备好的信号
    unsigned int distance_time;//时间差
    wait_queue_head_t hcsr_wait;//等待队列头
    struct timeval time_rising;//发送时间
    struct timeval time_falling;//到达时间
    struct hcsr04_resource *hcsr04_dev;//硬件
    spinlock_t hcsr_lock;
    struct tasklet_struct hcsr_tasklet;//中断之底半部

};
struct hcsr04_private* hcsr_combine;
static long hcsr_ioctl(struct file *file,
                        unsigned int cmd,
                          unsigned long buf){
    unsigned long flags;
    switch (cmd){
        case 0x2004:
            spin_lock_irqsave(&hcsr_combine->hcsr_lock, flags);
            gpio_set_value(hcsr_combine->hcsr04_dev->trig,1);
            udelay(12);
            gpio_set_value(hcsr_combine->hcsr04_dev->trig,0);
            spin_unlock_irqrestore(&hcsr_combine->hcsr_lock, flags);
            break;
    }
    wait_queue_t wait_kid;
    init_waitqueue_entry(&wait_kid,current);
    add_wait_queue(&hcsr_combine->hcsr_wait,&wait_kid);
    set_current_state(TASK_INTERRUPTIBLE);
    schedule();
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&hcsr_combine->hcsr_wait,&wait_kid);
    hcsr_combine->read_ready=0;
    spin_lock_irqsave(&hcsr_combine->hcsr_lock, flags);
    printk("time_falling: %d , time_rising:%d",hcsr_combine->time_falling,hcsr_combine->time_rising);
    hcsr_combine->distance_time=1000000*(hcsr_combine->time_falling.tv_sec)+hcsr_combine->time_falling.tv_usec-1000000*(hcsr_combine->time_rising.tv_sec)-hcsr_combine->time_rising.tv_usec;
    /*hcsr_combine->distance_time = 59;test*/
    spin_unlock_irqrestore(&hcsr_combine->hcsr_lock, flags);
    copy_to_user((unsigned int *)buf,&hcsr_combine->distance_time,sizeof(int));
    return 0;
}
static struct file_operations hcsr_fops={
    .unlocked_ioctl=hcsr_ioctl,
};
static struct miscdevice hcsr_misc={
    .name="hcsr",
    .minor=MISC_DYNAMIC_MINOR,
    .fops=&hcsr_fops,
};

static irqreturn_t hcsr_irq_handler(int irq,void * dev){
    struct hcsr04_private * pdev=(struct hcsr04_private*)dev;
    tasklet_schedule(&pdev->hcsr_tasklet);
    return IRQ_HANDLED;
}
static void tasklet_function(unsigned long data){
    unsigned int flags;
    struct tasklet_struct* tasklet=(struct tasklet_struct*)data;
    //struct tasklet_struct *tasklet = (struct tasklet_struct *)data;
    struct hcsr04_private* pdev=container_of(tasklet,struct hcsr04_private,hcsr_tasklet);
    spin_lock_irqsave(&pdev->hcsr_lock, flags);
    if(gpio_get_value(pdev->hcsr04_dev->echo)==1){
        do_gettimeofday(&pdev->time_rising);
    }else{
        do_gettimeofday(&pdev->time_falling);
        pdev->read_ready=1;
        wake_up(&pdev->hcsr_wait);
    }
    spin_unlock_irqrestore(&pdev->hcsr_lock, flags);
}
static int hcsr04_probe(struct platform_device* pdev){
    //1.kmalloc
    hcsr_combine=kmalloc(sizeof(struct hcsr04_private),GFP_KERNEL);
    memset(hcsr_combine,0,sizeof(struct hcsr04_private));
    //2.gpio,把形参的转换成此文件的
    hcsr_combine->hcsr04_dev=pdev->dev.platform_data;
    gpio_request(hcsr_combine->hcsr04_dev->echo,hcsr_combine->hcsr04_dev->name);
    gpio_request(hcsr_combine->hcsr04_dev->trig,hcsr_combine->hcsr04_dev->name);
    //3.irq
    request_irq(gpio_to_irq(hcsr_combine->hcsr04_dev->echo),hcsr_irq_handler,IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,hcsr_combine->hcsr04_dev->name,hcsr_combine);
    //4.tasklet
    tasklet_init(&hcsr_combine->hcsr_tasklet,tasklet_function,&hcsr_combine->hcsr_tasklet);
    //5.wait
    init_waitqueue_head(&hcsr_combine->hcsr_wait);
    //6.spinlock
    spin_lock_init(&hcsr_combine->hcsr_lock);
    //7.misc
    misc_register(&hcsr_misc);
    return 0;
}
static int hcsr04_remove(struct platform_device* pdev){
    misc_deregister(&hcsr_misc);
    free_irq(gpio_to_irq(hcsr_combine->hcsr04_dev->echo),hcsr_combine);
    gpio_free(hcsr_combine->hcsr04_dev->trig);
    gpio_free(hcsr_combine->hcsr04_dev->echo);
    kfree(hcsr_combine);
    return 0;
}
static struct platform_driver hcsr04_plat_drv={
    .driver={.name="hcsr04"},
    .probe=hcsr04_probe,
    .remove=hcsr04_remove,
};

static int hcsr04_init(void){
    platform_driver_register(&hcsr04_plat_drv);        
    return 0;
}
static void hcsr04_eixt(void){
    platform_driver_unregister(&hcsr04_plat_drv);
}

module_init(hcsr04_init);
module_exit(hcsr04_eixt);
MODULE_LICENSE("GPL");
