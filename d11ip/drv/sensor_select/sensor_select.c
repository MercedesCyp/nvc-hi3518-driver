/*
 * Copyright (c) 2014 Netviewtech Co.,Ltd.
 *
 * Description:
 *                 enable power of one sensor in two. 
 *
 * 140811
 *                 base on E01IP sensor switch
 *                 sensor 1 power enable GPIO5_2 low
 *                 sensor 2 power enable GPIO5_3 low
 *
 * 141023          night sensor              GPIO3_0  low
 *                 day sensor                GPIO3_1  low
 *
 * File name     : sensor_select.c
 * Author        : qiang.feng  qiang.feng<qiang.feng@netviewtech.com>
 * create date   : 2014.08.11
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
#include "sensor_select.h"

#define DEBUG
#ifdef  DEBUG
#define pk(fmt,args...)\
    printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DRV_NAME,__FUNCTION__,__LINE__,##args);
#else
#define pk(fmt,args...)
#endif


#define DRV_NAME                 "sensor_select"
#define DRV_VERSION              "14.08.11 -- for: D11IPD"
#define DRV_DESCRIPTION          "enable the power of sensor in two, any moment\
                                  just cant enable one."
#define DRV_MINOR                (111)



 /* 
  * enable which sensor can be select when insmod this model
  */

static S32 s32_sensor = 0x01;
module_param(s32_sensor,  int, 0);
MODULE_PARM_DESC(s32_sensor, "enable which sensor (01 for day, 02 for night, default night ");



struct drv_dev
{
    int dev_id;
};

struct drv_dev* drv_devp;


static int drv_open(struct inode * inode, struct file * file)
{
    return 0;
}

static int drv_release(struct inode *inode, struct file *filp)
{
    return 0;
}



/*
 * function     : sensor_day_enable
 * data         : void
 * return       : void
 * description  : GPIO3_0 = low so sensor for day power will turn on 
 */
void sensor_day_enable(void)
{
    hi_writel((1 << SEN_NIGHT_SN), SEN_NIGHT_REG);
    hi_writel(0x0, SEN_DAY_REG);
}




/*
 * function     : sensor_night_enable
 * data         : void
 * return       : void
 * description  : GPIO3_1 = LOW so sensor for night  power will turn on 
 */
void sensor_night_enable(void)
{
    hi_writel((1 << SEN_DAY_SN), SEN_DAY_REG);
    hi_writel(0x0, SEN_NIGHT_REG);
}




static long drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    S32 s32_err = 0;

    if(_IOC_TYPE(cmd) != DRV_IOC_MAGIC)
    {
        return -ENOTTY;
    }

    if(_IOC_NR(cmd) > DRV_IOC_MAXNR)
    {
        return -ENOTTY;
    }
 
    if(_IOC_DIR(cmd) & _IOC_READ)
    {
        s32_err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)); 
    }
    else
    {
        if(_IOC_DIR(cmd) & _IOC_WRITE)
        {
            s32_err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd)); 
        }
    }

    if(s32_err)
    {
        return FAULT;
    }


    switch(cmd)
    {
        case  SET_S0_EN:
		{
		    sensor_day_enable();
		    break;
		}
		case  SET_S1_EN:
		{
		    sensor_night_enable();
		    break;
		}
		case  SET_SEN_DAY_EN:
		{
		    sensor_day_enable();
		    break;
		}
		case  SET_SEN_NIGHT_EN:
		{
		    sensor_night_enable();
		    break;
		}

		default:
		{
		    break;
		}
	}
	return SUCCESS;
}

static struct file_operations drv_fops = {
    .owner          = THIS_MODULE,
    .open	        = drv_open,
    .release        = drv_release,
    .unlocked_ioctl = drv_ioctl,
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

    //for GPIO3_0
    ul_val = hi_readl(MUXCTRL_REG12);
    clear_bit(0, &ul_val);
    clear_bit(1, &ul_val);
    hi_writel(ul_val,MUXCTRL_REG12);
 
    //for GPIO3_1
    ul_val = hi_readl(MUXCTRL_REG13);
    clear_bit(0, &ul_val);
    clear_bit(1, &ul_val);
    hi_writel(ul_val,MUXCTRL_REG13);

    //set power control pin as output
    ul_val = hi_readl(SS_DIR);
	set_bit(SEN_DAY_SN, &ul_val);
	set_bit(SEN_NIGHT_SN, &ul_val);
	hi_writel(ul_val, SS_DIR);
}

static int __init drv_init(void)
{
    S32 s32_ret = 0;

    s32_ret = misc_register(&drv_dev);
    if(s32_ret)
    {
         printk(KERN_ERR"err: could not register %s devices.\n", DRV_NAME);
         return s32_ret;
    }

	gpio_init();

    drv_devp = kmalloc(sizeof(struct drv_dev), GFP_ATOMIC);
    if(!drv_devp)
    {
        misc_deregister(&drv_dev);
        printk(KERN_ERR"err: kmalloc for %s_devp!\n", DRV_NAME);
        return FAULT;
    }
    memset(drv_devp, 0, sizeof(struct drv_dev));



    if(s32_sensor == 0x01)
	{
	    sensor_day_enable();
	}
	else
	{
	    if(s32_sensor == 0x02)
		{
		    sensor_night_enable();
		}
	}

//    sensor0_enable();
//    sensor1_enable();

    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s init succed.\n", DRV_NAME);
    return SUCCESS;
}

static void  __exit drv_exit(void)
{
    misc_deregister(&drv_dev);
    kfree(drv_devp);
    
    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s rmmoded\n", DRV_NAME);
}
module_init(drv_init);
module_exit(drv_exit);

MODULE_INFO(build, __DATE__);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR("Stone.Feng <qiang.feng@netviewtech.com>");
MODULE_LICENSE("GPL");
