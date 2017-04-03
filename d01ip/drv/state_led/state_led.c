/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *     Control LED for APP.
 *     LED01    -------- GPIO0_2
 *     LED02    -------- GPIO0_3
 *     LED03    -------- GPIO0_4
 *
 *14.03.24
       change LED gpio
 *
 * 14.11.05
 *     unify output format when insmod or rmmod this module in prink function
 *
 *
 *  Support Hisilicon's chips, hi3518c
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 *
 * File name : state_led.c
 * Author    : qiang.feng
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
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>

#include "hi3518_com.h"
#include "state_led.h"
#include "type.h"


#define DRV_NAME                 "state_led"
#define DRV_VERSION              "14.11.05 - for D01IP D04IP"
#define DRV_MINOR                (0x84)

//#define DEBUG  1
#ifdef  DEBUG
#define pk(fmt,args...)\
    printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DRV_NAME,__FUNCTION__,__LINE__,##args);
#else
#define pk(fmt,args...)
#endif


struct state_led_dev
{
    struct timer_list state_led01_timer;
    struct timer_list state_led02_timer;
    struct timer_list state_led03_timer;

    //indecate if timer is on or off
    U32 u32_led01_timer_flags;    
    U32 u32_led02_timer_flags;
    U32 u32_led03_timer_flags;

    //record
    U32 u32_led01_state;
    U32 u32_led02_state;
    U32 u32_led03_state;

    struct  state_led_data sld;
};

struct state_led_dev *state_led_devp;



/*
 * functin     : led01_off
 * data        : void
 * return      : void
 * description : turn off the led01
 */
void led01_off(void)
{
    hi_writel(0xFF, GPIO0_2);
    state_led_devp->u32_led01_state = OFF;
}


/*
 * functin     : led01_on
 * data        : void
 * return      : void
 * description : turn on the led01
 */
void led01_on(void)
{
    hi_writel(0x00, GPIO0_2);
    state_led_devp->u32_led01_state = ON;
}

// led02
void led02_off(void)
{
    hi_writel(0xFF, GPIO0_3);
    state_led_devp->u32_led02_state = OFF;
}


void led02_on(void)
{
    hi_writel(0x00, GPIO0_3);
    state_led_devp->u32_led02_state = ON;
}


// led03
void led03_off(void)
{
    hi_writel(0xFF, GPIO0_4);
    state_led_devp->u32_led03_state = OFF;
}


void led03_on(void)
{
    hi_writel(0x00, GPIO0_4);
    state_led_devp->u32_led03_state = ON;
}



//all led
void led_all_off(void)
{
    hi_writel(0xFF, GPIO0_2);
    state_led_devp->u32_led01_state = OFF;

    hi_writel(0xFF, GPIO0_3);
    state_led_devp->u32_led02_state = OFF;

    hi_writel(0xFF, GPIO0_4);
    state_led_devp->u32_led03_state = OFF;
}


void led_all_on(void)
{
    hi_writel(0x00, GPIO0_2);
    state_led_devp->u32_led01_state = ON;

    hi_writel(0x00, GPIO0_3);
    state_led_devp->u32_led02_state = ON;

    hi_writel(0x00, GPIO0_4);
    state_led_devp->u32_led03_state = ON;
}




static int state_led_open(struct inode * inode, struct file * file)
{
    return 0;
}

static int state_led_close(struct inode * inode, struct file * file)
{
    return 0;
}




/*
 * function     : state_led01_timer_handle
 * data         : unsigned long arg
 * description  : twinkle led01 fit to the user set
 */
static void state_led01_timer_handle(unsigned long arg)
{
    //pk("STATE_LED01_TIMER_HANDLE\n");
    //printk("STATE_LED01_TIMER_HANDLE\n");
    if(state_led_devp->u32_led01_state == ON)
    {
        led01_off();
        mod_timer(&state_led_devp->state_led01_timer, 
                jiffies + state_led_devp->sld.u32_led01_off);
    }
    else
    {
        if(state_led_devp->u32_led01_state == OFF)
        {
            led01_on();
            mod_timer(&state_led_devp->state_led01_timer,
                    jiffies + state_led_devp->sld.u32_led01_on);
        }
    }
}


/*
 * function     : state_led02_timer_handle
 * data         : unsigned long arg
 * description  : twinkle led02 fit to the user set
 */
static void state_led02_timer_handle(unsigned long arg)
{
    if(state_led_devp->u32_led02_state == ON)
    {
        led02_off();
        mod_timer(&state_led_devp->state_led02_timer, 
                jiffies + state_led_devp->sld.u32_led02_off);
        return;
    }
    else
    {
        if(state_led_devp->u32_led02_state == OFF)
        {
            led02_on();
            mod_timer(&state_led_devp->state_led02_timer,
                    jiffies + state_led_devp->sld.u32_led02_on);
            return;
        }
    }
}
 



/*
 * function     : state_led03_timer_handle
 * data         : unsigned long arg
 * description  : twinkle led03 fit to the user set
 */
static void state_led03_timer_handle(unsigned long arg)
{
    if(state_led_devp->u32_led03_state == ON)
    {
        led03_off();
        mod_timer(&state_led_devp->state_led03_timer, 
                jiffies + state_led_devp->sld.u32_led03_off);
        return;
    }
    else
    {
        if(state_led_devp->u32_led03_state == OFF)
        {
            led03_on();
            mod_timer(&state_led_devp->state_led03_timer,
                    jiffies + state_led_devp->sld.u32_led03_on);
            return;
        }
    }
}
 



static long state_led_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    void __user *argp = (void __user *)arg;

    flash_time ft;

	switch(cmd)
	{
		case SET_LED01_OFF:
        {
            if(state_led_devp->u32_led01_timer_flags == ON)
            {
                del_timer(&state_led_devp->state_led01_timer);
                state_led_devp->u32_led01_timer_flags = OFF;
            }
            led01_off();
            break;
        }
		case SET_LED01_ON:
        {
            if(state_led_devp->u32_led01_timer_flags == ON)
            {
                del_timer(&state_led_devp->state_led01_timer);
                state_led_devp->u32_led01_timer_flags = OFF;
            }
            led01_on();
            break;
        }
        case SET_LED01_FLASH:
        {
            copy_from_user(&ft, argp, sizeof(flash_time));
            if((ft.u32_on == 0) || (ft.u32_off == 0))
            {
			    return FAULT;
			}

			state_led_devp->sld.u32_led01_on = ft.u32_on;
			state_led_devp->sld.u32_led01_off = ft.u32_off;

			led01_on();
            if(state_led_devp->u32_led01_timer_flags == ON)
            {
				del_timer(&state_led_devp->state_led01_timer);
            }
           
		    state_led_devp->state_led01_timer.expires = 
                jiffies + state_led_devp->sld.u32_led01_on; 
            add_timer(&state_led_devp->state_led01_timer);
            state_led_devp->u32_led01_timer_flags = ON;
			return SUCCESS;
            break;
        }

		case SET_LED02_OFF:
        {
            if(state_led_devp->u32_led02_timer_flags == ON)
            {
                del_timer(&state_led_devp->state_led02_timer);
                state_led_devp->u32_led02_timer_flags = OFF;
            }
            led02_off();
            break;
        }
		case SET_LED02_ON:
        {
            if(state_led_devp->u32_led02_timer_flags == ON)
            {
                del_timer(&state_led_devp->state_led02_timer);
                state_led_devp->u32_led02_timer_flags = OFF;
            }
            led02_on();
            break;
        }
        case SET_LED02_FLASH:
        {
            copy_from_user(&ft, argp, sizeof(flash_time));
            if((ft.u32_on == 0) || (ft.u32_off == 0))
            {
			    return FAULT;
			}

			state_led_devp->sld.u32_led02_on = ft.u32_on;
			state_led_devp->sld.u32_led02_off = ft.u32_off;

			led02_on();
            if(state_led_devp->u32_led02_timer_flags == ON)
            {
				del_timer(&state_led_devp->state_led02_timer);
            }
           
		    state_led_devp->state_led02_timer.expires = 
                jiffies + state_led_devp->sld.u32_led02_on; 
            add_timer(&state_led_devp->state_led02_timer);
            state_led_devp->u32_led02_timer_flags = ON;
			return SUCCESS;
            break;
        }

		case SET_LED03_OFF:
        {
            if(state_led_devp->u32_led03_timer_flags == ON)
            {
                del_timer(&state_led_devp->state_led03_timer);
                state_led_devp->u32_led03_timer_flags = OFF;
            }
            led03_off();
            break;
        }
		case SET_LED03_ON:
        {
            if(state_led_devp->u32_led03_timer_flags == ON)
            {
                del_timer(&state_led_devp->state_led03_timer);
                state_led_devp->u32_led03_timer_flags = OFF;
            }
            led03_on();
            break;
        }
        case SET_LED03_FLASH:
        {
            copy_from_user(&ft, argp, sizeof(flash_time));
            if((ft.u32_on == 0) || (ft.u32_off == 0))
            {
			    return FAULT;
			}

			state_led_devp->sld.u32_led03_on = ft.u32_on;
			state_led_devp->sld.u32_led03_off = ft.u32_off;

			led03_on();
            if(state_led_devp->u32_led03_timer_flags == ON)
            {
				del_timer(&state_led_devp->state_led03_timer);
            }
           
		    state_led_devp->state_led03_timer.expires = 
                jiffies + state_led_devp->sld.u32_led03_on; 
            add_timer(&state_led_devp->state_led03_timer);
            state_led_devp->u32_led03_timer_flags = ON;
			return SUCCESS;
            break;
        }

		default:
		{
		    return FAULT;
		}
	}
    return SUCCESS;
}

/*
 * function   : gpio_init
 * data       : void
 * return     : void
 * description: init gpio and set GPIO as output
 */

static void gpio_init(void)
{
    SIZE_T ul_val;


    //GPIO0_2
    ul_val = hi_readl(MUXCTRL_REG74);
    clear_bit(0, &ul_val);
    clear_bit(1, &ul_val);
    hi_writel(ul_val, MUXCTRL_REG74);


    //GPIO0_3
    ul_val = hi_readl(MUXCTRL_REG75);
    clear_bit(3, &ul_val);
    hi_writel(ul_val, MUXCTRL_REG75);

    //GPIO0_4
    ul_val = hi_readl(MUXCTRL_REG76);
    clear_bit(0, &ul_val);
    hi_writel(ul_val, MUXCTRL_REG76);

	//GPIO0_4 GPIO0_3 GPIO0_2 as output
	ul_val = hi_readl(GPIO0_DIR);
	set_bit(2, &ul_val);
	set_bit(3, &ul_val);
	set_bit(4, &ul_val);
	hi_writel(ul_val, GPIO0_DIR);


}

static struct file_operations state_led_fops = {
    .owner             = THIS_MODULE,
    .unlocked_ioctl    = state_led_ioctl,
    .open	           = state_led_open,
    .release           = state_led_close
};

static struct miscdevice state_led_dev = {
    .minor = DRV_MINOR,
    .name  = DRV_NAME,
    .fops  = &state_led_fops,
};

static int __init state_led_init(void)
{
    S32 s32_ret = 0;

    s32_ret = misc_register(&state_led_dev);
    if(s32_ret)
    {
        printk(KERN_ERR"err: register state of light.\n");
        return s32_ret;
    }

    gpio_init();    

    state_led_devp = kmalloc(sizeof(struct state_led_dev), GFP_KERNEL);
    if(!state_led_devp)
    {
        misc_deregister(&state_led_dev);
        printk(KERN_ERR"err: kmalloc for state_led_devp.\n");
        return -ENOMEM;
    }

    led_all_off(); 
    init_timer(&state_led_devp->state_led01_timer);
    state_led_devp->state_led01_timer.function = &state_led01_timer_handle;
    state_led_devp->u32_led01_timer_flags = OFF;

    init_timer(&state_led_devp->state_led02_timer);
    state_led_devp->state_led02_timer.function = &state_led02_timer_handle;
    state_led_devp->u32_led02_timer_flags = OFF;

    init_timer(&state_led_devp->state_led03_timer);
    state_led_devp->state_led03_timer.function = &state_led03_timer_handle;
    state_led_devp->u32_led03_timer_flags = OFF;
/*
    while(0) // for timer test
    {
        state_led_devp->sld.u32_led01_on = 50;
        state_led_devp->sld.u32_led02_off = 50;
        state_led_devp->state_led01_timer.expires 
		      = jiffies + state_led_devp->sld.u32_led01_on;

        add_timer(&state_led_devp->state_led01_timer);
        state_led_devp->u32_led01_timer_flags = ON;
    }
*/
    
    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);

    printk(KERN_INFO"%s init succed.\n", DRV_NAME);
         
    return 0;
}

static void __exit state_led_exit(void)
{
    if(state_led_devp->u32_led01_timer_flags == ON)
    {
        del_timer(&state_led_devp->state_led01_timer);
    }
	
	if(state_led_devp->u32_led02_timer_flags == ON)
    {
        del_timer(&state_led_devp->state_led02_timer);
    }

    if(state_led_devp->u32_led03_timer_flags == ON)
    {
        del_timer(&state_led_devp->state_led03_timer);
    }

    misc_deregister(&state_led_dev);

    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s rmmoded\n", DRV_NAME);
}

MODULE_AUTHOR("qiang.feng<qiang.feng@netviewtech.com>");
MODULE_DESCRIPTION("Change the state of led, ON; OFF or TWINKLE");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(state_led_init);
module_exit(state_led_exit);
