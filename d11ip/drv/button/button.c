/*
 * Copyright (c) 2014 Netviewtech Co.,Ltd.
 *
 * Description:
 *    If button had been pressed, send signal to user application.
 *
 *
 * GPIO0_1  Connect to button
 * 
 * 140803
 *   button need to be pushed more than one second then trigger singnal
 * 140806
 *   buttom long press 8 second go into pairing mode
 *          short press more than 1 second and less than 8 second swtich on/off switch
 *
 * 141025
 *  button trigger change to fall down endge 
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
#define DRV_VERSION              "14.10.25 -- for: D11IPD"
#define DRV_MINOR                (101)
#define DRV_CHECK_TIME           (10)      //base unit is 10ms
#define DRV_HOLD_TIME            (8000)    //base unit is 1ms


spinlock_t   drv_lock;
struct drv_dev
{
    int dev_id;
	struct timer_list bt_timer;
	U32 u32_timer_flag;
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

//	pk("button input\n");
    //judge if the interrupt had been triggered by button
    if((hi_readl(BT_RIS) & (1 << BT_SN)) != 0)
    {
        mdelay(10);
        hi_writel((1 << BT_SN), BT_IC); 

        drv_devp->bt_timer.expires = jiffies + DRV_CHECK_TIME;
		if(drv_devp->u32_timer_flag == OFF)
		{
		    add_timer(&drv_devp->bt_timer);
	    	drv_devp->u32_timer_flag = ON;
	    }
/*     
        if(drv_devp->async_queue)
        {
            kill_fasync(&drv_devp->async_queue, SIGIO, POLL_IN);
			pk("button had pressed!\n");
        }
		*/
    }
    hi_writel((1 << BT_SN), BT_IC); 
    spin_unlock_irqrestore(&drv_lock, ul_flags);
    return IRQ_HANDLED;
}





/*
 * function     : bt_timer_handle
 * data         : unsigned long arg
 * return       : void
 * description  : timer for button press and hold for more than one second
 */
static void bt_timer_handle(unsigned long arg)
{
    static U32 u32_count = 0;


	mod_timer(&drv_devp->bt_timer, jiffies + DRV_CHECK_TIME);

	if((hi_readl(BT_REG) & (1 << BT_SN)) == 0)
	{
	    //pk("th");
		pk("%d", u32_count);
	    u32_count++;
		if(u32_count >= (DRV_HOLD_TIME / DRV_CHECK_TIME / 10))
		{
		    del_timer(&drv_devp->bt_timer);
		    drv_devp->u32_timer_flag = OFF;
		    u32_count = 0;
			pk("en");
            if(drv_devp->async_queue)
            {
                kill_fasync(&drv_devp->async_queue, SIGIO, POLL_IN);
            }
		}
	}
	else
	{
	    del_timer(&drv_devp->bt_timer);
		drv_devp->u32_timer_flag = OFF;
		u32_count = 0;
	}
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

	//falling or low level
	ul_val = hi_readl(BT_IEV);
	clear_bit(BT_SN, &ul_val);
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

	init_timer(&drv_devp->bt_timer);
    drv_devp->bt_timer.function = bt_timer_handle;
	drv_devp->u32_timer_flag = OFF;

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
	
	if(drv_devp->u32_timer_flag == ON)
	{
	    del_timer(&drv_devp->bt_timer);
	}

    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
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
