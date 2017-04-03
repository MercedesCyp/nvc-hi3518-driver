/* 
 * Copyright (c) 2011 Netviewtech Co.,Ltd.  
 * Description: 
 *     GPIO6_1 for night light control 
 * 140811:
 *     add on and off control when initialise 
 *
 * 141023
 *     GPIO5_3 push high make night to work for test 
 *     befor GPIO5_3 for ir-cut for sensor switch
 *
 * 141024
 *     GPIO5_3 as PAM1 output and adjust night light bright
 *
 * File name       : nl.c
 * Author          : qiang.feng
 * Create Date     : 2014.05.01
 
 */ 


#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>



//#include "drv.h"
#include "hi3518_com.h"
#include "nl.h"
#include "type.h"

#define DRV_NAME                 "nl"
#define DRV_MINOR                 (108)
#define DRV_VERSION              "14.10.24 -- for: D11IPD"
#define DRV_DESCRIPTION          "night light control SOC:HI3518C"
 /* 
  * turn on or off LED when initialise
  */

static S32 s32_switch = 0x01;
module_param(s32_switch,  int, 0);
MODULE_PARM_DESC(s32_switch, "turn on or off LED 0x01: on, ox02 :off");




struct drv_dev
{
    struct timer_list ky_10ms_timer;
    U32 u32_drv_state;      //ON or OFF
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
 * function   : drv_off
 * data       : void
 * return     : U32 SUCCESS
 *                  FAULT
 * description: turn off the night light.
 */
U32 drv_off(void)
{
    hi_writel(0x00, NL_CTR_REG);
    drv_devp->u32_drv_state = OFF;
    return SUCCESS;
}


/*
 * function   : drv_on
 * data       : void
 * return     : U32 SUCCESS
 *                  FAULT
 * description: turn on the night light.
 */
U32 drv_on(void)
{
    hi_writel((1 << NL_CTR_SN), NL_CTR_REG);
    drv_devp->u32_drv_state = ON;
    return SUCCESS;
}




//int drv_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    unsigned int __user *argp = (unsigned int __user *)arg;
    //unsigned int chip_num;
    U32 u32_state = OFF;


    switch(cmd)
    {
        case NL_READ_STATE:
            {
                u32_state = drv_devp->u32_drv_state;
                copy_to_user(argp, (void *)(&u32_state), sizeof(unsigned int));
                break;
            }
        case NL_SET_OFF:
            {
                drv_off();
                break;
            }
        case NL_SET_ON:
            {
                drv_on();
                break;
            }
        default:
            {
                return FAULT;
                break;
            }
    }

    return SUCCESS;
}


/*
 * function   : pwm_init
 * data       : void
 * return     : void
 * description: initialise PWM for night light bright adjust
 */

static void pwm_init(void)
{
    SIZE_T ul_tmp;  

//    //turn on PWM clock
//	ul_tmp = hi_readl(PERI_CRG14);
//	set_bit(1, &ul_tmp);
//    clear_bit(0, &ul_tmp);
//	hi_writel(ul_tmp, PERI_CRG14);
//
//	//PWM output from GPIO5_3
//	ul_tmp = hi_readl(MUXCTRL_REG48);
//	set_bit(0, &ul_tmp);
//	hi_writel(ul_tmp, MUXCTRL_REG48);


	//set GPIO5_3 to hight to make LED can work,temporary
	ul_tmp = hi_readl(MUXCTRL_REG48);
	clear_bit(0, &ul_tmp);
	hi_writel(ul_tmp, MUXCTRL_REG48);
   
    
    //set power control pin as output
    ul_tmp = hi_readl(GPIO5_DIR);
	set_bit(3, &ul_tmp);
	hi_writel(ul_tmp, GPIO5_DIR);
    hi_writel((1<<3), GPIO5_3);

}





static int gpio_init(void)
{

    SIZE_T ul_val;

    ul_val = hi_readl(MUXCTRL_CRG46);
	clear_bit(0, &ul_val);
	hi_writel(ul_val, MUXCTRL_CRG46);

    ul_val = hi_readl(NL_DIR);
	set_bit(NL_CTR_SN, &ul_val);
	hi_writel(ul_val, NL_DIR);

	pwm_init();

    return 0;
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



static void drv_10ms_timer_handle(unsigned long arg)
{
    //static U32  u32_count = 0;
    mod_timer(&drv_devp->ky_10ms_timer, jiffies + 10);
}



static S32  __init drv_init(void)
{
    S32 s32_ret = 0;

    s32_ret = misc_register(&drv_dev);
    if(s32_ret)
    {
         printk(KERN_ERR"err: could not register drv devices. \n");
         return s32_ret;
    }

    if(gpio_init()<0)
    {
         misc_deregister(&drv_dev);
         printk(KERN_ERR"err: drv driver init fail for device init error!\n");
         return FAULT;
    }

    drv_devp = kmalloc(sizeof(struct drv_dev), GFP_KERNEL);
    if(!drv_devp)
    {
        misc_deregister(&drv_dev);
        return -ENOMEM;
    }
    
    memset(drv_devp, 0, sizeof(struct drv_dev));

    init_timer(&drv_devp->ky_10ms_timer);
    drv_devp->ky_10ms_timer.function = &drv_10ms_timer_handle;
    drv_devp->ky_10ms_timer.expires = jiffies + 1;
    
    add_timer(&drv_devp->ky_10ms_timer);

    drv_devp->u32_drv_state = OFF;

    if(s32_switch == 0x01)
	{
	    drv_on();
	}
	else
	{
	    if(s32_switch == 0x02)
		{
		    drv_off();
		}
	}


    //drv_off();
   //drv_on(); 
    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);

    printk(KERN_INFO"%s init succed\n", DRV_NAME);

    return 0;
}

static void __exit drv_exit(void)
{

    misc_deregister(&drv_dev);
 
    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);

   
    del_timer(&drv_devp->ky_10ms_timer);
    kfree(drv_devp);
    printk("drv driver exit OK.\n");
}
module_init(drv_init);
module_exit(drv_exit);


MODULE_AUTHOR("Netviewtech qiang.feng");
MODULE_DESCRIPTION("If the key has been keep pressed for 3 seconds and then reboot system.");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
