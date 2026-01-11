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
    int read_ready;
    unsigned int distance_time;
    wait_queue_head_t hcsr_wqh;
    struct timeval time_rising;
    struct timeval time_falling;
    struct hcsr04_resource *hcsr;
    spinlock_t lock;
    struct work_struct hcsr_work;
};

static struct hcsr04_private *hcsr_priv;

#define US_PER_SECOND       1000000
#define GET_DISTANCE      (0x10020)
//回传距离给上位机
static long hcsr_ioctl(struct file *file,
                unsigned int cmd,
                unsigned long buf)
{
    unsigned long flags;

    switch(cmd){
        case GET_DISTANCE:
            spin_lock_irqsave(&hcsr_priv->lock, flags);
            gpio_set_value(hcsr_priv->hcsr->trig,1);
            udelay(12);//维持一段时间高电平
            gpio_set_value(hcsr_priv->hcsr->trig,0);
            spin_unlock_irqrestore(&hcsr_priv->lock, flags);
            break;
    }
    
    wait_event_interruptible(hcsr_priv->hcsr_wqh,hcsr_priv->read_ready);
    hcsr_priv->read_ready=0;
    
    spin_lock_irqsave(&hcsr_priv->lock, flags);
	//开始时间：{tv_sec=1000, tv_usec=500000}（1000.5秒）
	//结束时间:{tv_sec=1000, tv_usec=800000}（1000.8秒）
	//(1000-1000)×1000000 + (800000-500000) = 300000 微秒
    hcsr_priv->distance_time = (hcsr_priv->time_falling.tv_sec - hcsr_priv->time_rising.tv_sec) * US_PER_SECOND
                                + hcsr_priv->time_falling.tv_usec - hcsr_priv->time_rising.tv_usec;
    spin_unlock_irqrestore(&hcsr_priv->lock, flags);

    if(copy_to_user((unsigned int *)buf,&hcsr_priv->distance_time,sizeof(int)))
        return -EFAULT;
    return 0;
}

static struct file_operations misc_fops={
    .unlocked_ioctl = hcsr_ioctl,
};

static struct miscdevice hcsr_misc={
    .name="hcsr",
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &misc_fops
};
//底半部之工作队列处理函数
static void hcsr_work_function(struct work_struct *work) {
    unsigned long flags;
    
    struct hcsr04_private *p = container_of(work, struct hcsr04_private, hcsr_work);

    spin_lock_irqsave(&p->lock, flags);
    if(gpio_get_value(p->hcsr->echo)){//if 1
        do_gettimeofday(&p->time_rising); 
    }else{//if 0
        do_gettimeofday(&p->time_falling);
        p->read_ready = 1;
        wake_up_interruptible(&p->hcsr_wqh);
    }
    spin_unlock_irqrestore(&p->lock, flags);
}

static irqreturn_t hcsr_irq(int irq,void *dev)
{
	//顶半部只获取当前私有结构体变量
    struct hcsr04_private *pdata = (struct hcsr04_private *)dev;
	//使用的是工作队列，它可以休眠，且用于不考虑效率问题时
		//底半部
    schedule_work(&pdata->hcsr_work);
    return IRQ_HANDLED;
}

static int hcsr_probe(struct platform_device *pdev)
{
	//1.申请内存
    hcsr_priv = kmalloc(sizeof(struct hcsr04_private), GFP_KERNEL);//分配一个结构体大小的内存
    if(hcsr_priv == NULL) 
        goto err1;
    memset(hcsr_priv, 0, sizeof(struct hcsr04_private));//清洗该内存
	//2.以前直接把pdev解包得到硬件结构体，现在把得到的结构体直接赋值给私有结构体的成员变量
    hcsr_priv->hcsr =pdev->dev.platform_data;
    //初始化gpio
    gpio_request(hcsr_priv->hcsr->trig,hcsr_priv->hcsr->name);
    gpio_request(hcsr_priv->hcsr->echo,hcsr_priv->hcsr->name);
    gpio_direction_output(hcsr_priv->hcsr->trig,0);
	//3.等待队列头 鸡妈妈
    init_waitqueue_head(&hcsr_priv->hcsr_wqh);
	//4.和用户域的接口
    misc_register(&hcsr_misc);
    //5.衍生自旋锁
    spin_lock_init(&hcsr_priv->lock);
	//6.底半部
    INIT_WORK(&hcsr_priv->hcsr_work, hcsr_work_function);
	//7.根据收发gpio的上升及下降沿触发的中断
    if(request_irq(gpio_to_irq(hcsr_priv->hcsr->echo),
                hcsr_irq,
                IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,
                hcsr_priv->hcsr->name, hcsr_priv)){
        goto err2;
    }
    return 0;

err2:
    kfree(hcsr_priv);
err1:
    return -1;
}

static int hcsr_remove(struct platform_device *pdev)
{
    misc_deregister(&hcsr_misc);//卸载接口
    free_irq(gpio_to_irq(hcsr_priv->hcsr->echo), hcsr_priv);//卸载中断号echo
    gpio_free(hcsr_priv->hcsr->trig);//卸载触发gpio
    gpio_free(hcsr_priv->hcsr->echo);//卸载收发gpio
    kfree(hcsr_priv);//释放内存
    return 0;
}

static struct platform_driver hcsr_drv={
    .driver={
        .name="hcsr"
    },
    .probe = hcsr_probe,
    .remove = hcsr_remove
};

static int hcsr_drv_init(void){
    platform_driver_register(&hcsr_drv);
    return 0;
}
static void hcsr_drv_exit(void){
    platform_driver_unregister(&hcsr_drv);
}

module_init(hcsr_drv_init);
module_exit(hcsr_drv_exit);
MODULE_LICENSE("GPL");
