/*
 * Copyright (c) 2014 Netviewtech Co.,Ltd.
 *
 * Description:
 *    If button had been pressed, send signal to user application.
 *
 *
 * GPIO0_1  Connect to button
 *
 * File name : button.c
 * Author    : qiang.feng  qiang.feng<qiang.feng@netviewtech.com>
 * create date   :   2014.02.17
 */


#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>

#include <asm/signal.h>
#include <asm/siginfo.h>
#include <linux/irq.h>
#include <linux/interrupt.h>



#include "type.h"
#include "hi3518_com.h"
#include "button.h"

//#define DEBUG
#ifdef  DEBUG
#define pk(fmt,args...)\
    printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DRV_NAME,__FUNCTION__,__LINE__,##args);
#else
#define pk(fmt,args...)
#endif


#define DRV_NAME                 "button"
#define DRV_VERSION              "14.10.15 - for D01IP, D04IP"
#define DRV_MINOR                (101)


spinlock_t   drv_lock;
struct drv_dev
{
    int dev_id;
    struct fasync_struct *async_queue;
};

struct drv_dev* drv_devp;


static int drv_fasync(int fd, struct file *filp, int mode)
{
    return fasync_helper(fd, filp, mode, &drv_devp->async_queue);
}

static int drv_open(struct inode * inode, struct file * file)
{
    return 0;
}

static int drv_release(struct inode *inode, struct file *filp)
{
    drv_fasync(-1, filp, 0);
    return 0;
}

//static irqreturn_t intnr_gpio0_1(int irq, void *dev_id, struct pt_regs *regs)
static irqreturn_t intnr_gpio0_1(int irq, void *dev_id)
{
    SIZE_T ul_flags;
    
    spin_lock_irqsave(&drv_lock, ul_flags);

	pk("button input\n");
    //judge if the interrupt had been triggered by button
    if((hi_readl(BT_RIS) & (1 << BT_SN)) != 0)
    {
	    //increase delay time to avoid double trigger
        hi_writel((1 << BT_SN), BT_IC); 
     
        if(drv_devp->async_queue)
        {
            kill_fasync(&drv_devp->async_queue, SIGIO, POLL_IN);
			pk("button had pressed!\n");
        }
		while(!hi_readl(BT_REG)); 
        mdelay(300);
    }
    hi_writel((1 << BT_SN), BT_IC); 
    spin_unlock_irqrestore(&drv_lock, ul_flags);
    return IRQ_HANDLED;
}

static struct file_operations drv_fops = {
    .owner          = THIS_MODULE,
    .open	        = drv_open,
    .release        = drv_release,
    .fasync         = drv_fasync,
};

static struct miscdevice drv_dev = {
    .minor          = DRV_MINOR,
    .name           = DRV_NAME,
    .fops           = &drv_fops,
};


/*
 * function     : gpio_init
 * data         : void
 * return       : void
 * description  : set the GPIO's function as we need.
 */
void  gpio_init(void)
{
     SIZE_T ul_val;

    //for GPIO0_1
    ul_val = hi_readl(MUXCTRL_REG73);
    clear_bit(0, &ul_val);
    clear_bit(1, &ul_val);
    hi_writel(ul_val,MUXCTRL_REG73);

    //set button pin as input
    ul_val = hi_readl(BT_DIR);
	clear_bit(BT_SN, &ul_val);
	hi_writel(ul_val, BT_DIR);

	//edge trigger
    ul_val = hi_readl(BT_IS);	
	clear_bit(BT_SN, &ul_val);
	hi_writel(ul_val, BT_IS);

	//single edge trigger
	ul_val = hi_readl(BT_IBE);
	clear_bit(BT_SN, &ul_val);
	hi_writel(ul_val, BT_IBE);

	//rising  or ligh level
	ul_val = hi_readl(BT_IEV);
	set_bit(BT_SN, &ul_val);
	hi_writel(ul_val, BT_IEV);

	//unmask interrupt
	ul_val = hi_readl(BT_IE);
	set_bit(BT_SN, &ul_val);
	hi_writel(ul_val, BT_IE);
    pk("");
}


static int __init drv_init(void)
{
    S32 s32_ret = 0;

    spin_lock_init(&drv_lock);


	gpio_init();

    drv_devp = kmalloc(sizeof(struct drv_dev), GFP_ATOMIC);
    if(!drv_devp)
    {
        misc_deregister(&drv_dev);
        printk(KERN_ERR"err: kmalloc for %s_devp!\n", DRV_NAME);
        return FAULT;
    }
    memset(drv_devp, 0, sizeof(struct drv_dev));

    s32_ret = misc_register(&drv_dev);
    if(s32_ret)
    {
         printk(KERN_ERR"err: could not register %s devices.\n", DRV_NAME);
         return s32_ret;
    }

    if(request_irq(INT_GPIO29, intnr_gpio0_1, IRQF_SHARED, 
                     "GPIO0_1 interrupt", &drv_devp->dev_id))
    {
       printk(KERN_ERR"err: request_irq INT_GPIO29.\n"); 
       return FAULT;
    }

    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s init succed.\n", DRV_NAME);
    return SUCCESS;
}

static void  __exit drv_exit(void)
{
    misc_deregister(&drv_dev);
    kfree(drv_devp);
    
    free_irq(INT_GPIO29, &drv_devp->dev_id);

    printk(KERN_INFO"%s Verson = %s Compile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s rmmoded\n", DRV_NAME);
}
module_init(drv_init);
module_exit(drv_exit);

MODULE_INFO(build, __DATE__);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("If button had been pressed, then send signal SIGIO to user application.");
MODULE_AUTHOR("Stone.Feng <qiang.feng@netviewtech.com>");
MODULE_LICENSE("GPL");
