/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *     base d03ip-pt_uln2003
 *
 *  24.10.24
 *     driver name in /dev/ change from pt2803 to pt
 *
 * File name : pt2803.c
 * Author    : qiang.feng
 * Date      : 2014.04.29
 * Version   : 1.00
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

#include "hi3518_com.h"
#include "pt2803.h"
#include "pt_run.h"
#include "type.h"


#define DRV_NAME                 "pt"
#define DRV_MINOR                 (107)
#define DRV_VERSION              "14.10.24 - for: D11IP"
#define DRV_DESCRIPTION          "pt control SOC:HI3518C"



U32 u32_tm2 = 107;


//#define DEBUG_LEVEL 1
#define DPRINTK(level,fmt,args...) do{ if(level < DEBUG_LEVEL)\
 printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DEV_NAME,__FUNCTION__,__LINE__,##args);\
}while(0)


#define DEBUG
#ifdef  DEBUG
#define pk(fmt,args...)\
    printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DRV_NAME,__FUNCTION__,__LINE__,##args);
#else
#define pk(fmt,args...)
#endif



 /* Vertical self check on or off when model is init
 */

static int si_v_self_check = OFF;
module_param(si_v_self_check,  int, 0);
MODULE_PARM_DESC(si_v_self_check, "Vertical self check ON or OFF(ON = 0, OFF = 1   default ON ");


/*
 * Horizontal self check on or off when model is init
 */

static int si_h_self_check = OFF;
module_param(si_h_self_check,  int, 0);
MODULE_PARM_DESC(si_h_self_check, "Horizontal self check ON or OFF(ON = 0, OFF = 1   default ON ");



static DEFINE_SPINLOCK(timer2);
struct pt_dev *pt_devp;


unsigned int track[10][2]  =        //Track for cruise
   { 
    {   0,  0},
    {1500, 50},
    {3000,150},
    {5000,300},
    {6000,400},
    {5000,350},
    {3000,300},
    {1500,250},
    {1000,120},
    { 500, 50},
   }; 



static int drv_open(struct inode * inode, struct file *file)
{
    return 0;
}

int drv_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*
static int drv_ioctl(struct inode *inode, struct file *filp, 
                            unsigned int cmd, unsigned long arg)
							*/
static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    void __user* argp = (void __user*)arg;
    int i_steps;
    int i_degree;
    int ierr = 0;
    unsigned long ul_delay;

    DECLARE_WAITQUEUE(wait_ret, current);

    ul_delay = jiffies + 3;

    if(_IOC_TYPE(cmd) != PT_IOC_MAGIC)
    {
        return -ENOTTY;
    }

    if(_IOC_NR(cmd) > PT_IOC_MAXNR)
    {
        return -ENOTTY;
    }
 
    if(_IOC_DIR(cmd) & _IOC_READ)
    {
        ierr = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)); 
    }
    else
    {
        if(_IOC_DIR(cmd) & _IOC_WRITE)
        {
            ierr = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd)); 
        }
    }

    if(ierr)
    {
        return -EFAULT;
    }
 
    switch(cmd)
    {
        case HRGN_ONE_STEP:
            {
                break;
            }
        case HRGN_30_DEG:
            {
                //1370 / 240 * 30  = 170
                pt_devp->i_h_will_steps = 170;
                pt_devp->ui_h_will_dir = DIR_HRGN;
                pt_devp->ui_h_mode = STEPS_RUN;
                timer2_start();
                break;
            }
        case HTML_30_DEG:
            {
                pt_devp->i_h_will_steps = 170;
                pt_devp->ui_h_will_dir = DIR_HTML;
                pt_devp->ui_h_mode = STEPS_RUN;
                timer2_start();
                break;
            }
        case VTML_15_DEG: { 
                //540 / 120 * 15 = 38
                pt_devp->i_v_will_steps = 38;
                pt_devp->ui_v_will_dir = DIR_VTML;
                pt_devp->ui_v_mode = STEPS_RUN;
                timer2_start();
                break;
            }
        case VRGN_15_DEG:
            {
                pt_devp->i_v_will_steps = 38;
                pt_devp->ui_v_will_dir = DIR_VRGN;
                pt_devp->ui_v_mode = STEPS_RUN;
                timer2_start();
                break;
            }
        case TRACK_01:
            {
                /*
                //Run to origin
                pt_devp->i_v_will_steps = sm_devp->i_v_pos;
                pt_devp->ui_v_will_dir = DIR_DOWN;
                pt_devp->ui_v_state = STEPS_RUN;
                v_start();
                */
           //     pt_devp_ui_v_track_point = 0;
                pt_devp->ui_v_mode = TRACK01;
                break;
            }
        case TRACK_02:
            {
                pt_devp->ui_h_mode = TRACK02;
                break;
            }
        case TRACK_03:
            {
                pt_devp->ui_h_mode = TRACK03;
                break;
            }
        case RUN_HRGN_STEPS:
            {
                pt_devp->ui_h_will_dir = DIR_HRGN;
                copy_from_user(&i_steps, argp, sizeof(int));
                if(i_steps < 0)
                {
                    return -1;
                }
                pt_devp->i_h_will_steps = i_steps;
                pt_devp->ui_h_mode = STEPS_RUN;
                timer2_start();
                break;
            }
        case RUN_HTML_STEPS:
            {
                pt_devp->ui_h_will_dir = DIR_HTML;
                copy_from_user(&i_steps, argp, sizeof(int));
                if(i_steps < 0)
                {
                    return -1;
                }
                pt_devp->i_h_will_steps = i_steps;
                pt_devp->ui_h_mode = STEPS_RUN;
                timer2_start();
                break;
            }
        case RUN_VTML_STEPS:
            {
                copy_from_user(&i_steps, argp, sizeof(int));
                if(i_steps < 0)
                {
                    return -1;
                }
                pt_devp->i_v_will_steps = i_steps;
                pt_devp->ui_v_will_dir = DIR_VTML;
             //   printk(KERN_INFO"RUN_UP_STEPS i_steps = %d \n", i_steps);
                pt_devp->ui_v_mode = STEPS_RUN;
                timer2_start();
                break;
            }

        case RUN_VRGN_STEPS:
            {
                copy_from_user(&i_steps, argp, sizeof(int));
                if(i_steps < 0)
                {
                    return -1;
                }
                pt_devp->i_v_will_steps = i_steps;
                pt_devp->ui_v_will_dir = DIR_VRGN;
                pt_devp->ui_v_mode = STEPS_RUN;
                timer2_start();
                break;
            }


        case RUN_HRGN_DEGREE:
            {
                pt_devp->ui_h_will_dir = DIR_HRGN;
                copy_from_user(&i_degree, argp, sizeof(int));
                if((i_degree < 0) || (i_degree > H_TOTAL_DEGREE))
                {
                    return PT_DEGREE;
                }
                i_steps =  pt_devp->ui_h_total_steps/H_TOTAL_DEGREE*i_degree; 
                pt_devp->i_h_will_steps = i_steps;
                pt_devp->ui_h_mode = STEPS_RUN;
                timer2_start();
                break;
            }
 
        case RUN_HTML_DEGREE:
            {
                pt_devp->ui_h_will_dir = DIR_HTML;
                copy_from_user(&i_degree, argp, sizeof(int));
                if((i_degree < 0) || (i_degree > H_TOTAL_DEGREE))
                {
                    return PT_DEGREE;
                }
                i_steps =  pt_devp->ui_h_total_steps/H_TOTAL_DEGREE*i_degree; 
                pt_devp->i_h_will_steps = i_steps;
                pt_devp->ui_h_mode = STEPS_RUN;
                timer2_start();
                break;
            }

         case RUN_VRGN_DEGREE:
            {
                pt_devp->ui_v_will_dir = DIR_VRGN;
                copy_from_user(&i_degree, argp, sizeof(int));
                if((i_degree < 0) || (i_degree > V_TOTAL_DEGREE ))
                {
                    return PT_DEGREE;
                }
                i_steps =  pt_devp->ui_v_total_steps/V_TOTAL_DEGREE*i_degree; 
                pt_devp->i_v_will_steps = i_steps;
                pt_devp->ui_v_mode = STEPS_RUN;
                timer2_start();
                break;
            }
 
        case RUN_VTML_DEGREE:
            {
                pt_devp->ui_v_will_dir = DIR_VTML;
                copy_from_user(&i_degree, argp, sizeof(int));
                if((i_degree < 0) || (i_degree > V_TOTAL_DEGREE))
                {
                    return PT_DEGREE;
                }
                i_steps =  pt_devp->ui_v_total_steps/V_TOTAL_DEGREE*i_degree; 
                pt_devp->i_v_will_steps = i_steps;
                pt_devp->ui_v_mode = STEPS_RUN;
                timer2_start();
                break;
            }
 
     
        default :   return PT_CMD;
    }
    add_wait_queue(&pt_devp->wqh_return, &wait_ret);
    sleep_on(&pt_devp->wqh_return);
  
    remove_wait_queue(&pt_devp->wqh_return, &wait_ret); 
    if(pt_devp->ui_v_pst == VTML)
    {
        return PT_VTML; 
    }
    if(pt_devp->ui_v_pst == VRGN)
    {
        return PT_VRGN; 
    }
 
    if(pt_devp->ui_h_pst == HTML)
    {
        return PT_HTML; 
    }
 
     if(pt_devp->ui_h_pst == HRGN)
    {
        return PT_HRGN; 
    }
    
    return PT_SUCCESS;
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
static struct file_operations sm_fops = {
  .owner    = THIS_MODULE,
  .open	    = sm_open,
  .ioctl    = sm_ioctl,
  .release  = sm_release,
};

static struct miscdevice pt_dev = {
  .minor = PT_MINOR,
  .name  = "pt",
  .fops  = &sm_fops,
};
*/

/* function    : v_origin
 *
 * Description : Run to the origin in vertical
 */


/*
 * function    : dual_timer1_init
 * data        : (void)
 * return      : U32 SUCCESS 
 *                   FAULT
 * description : Init timer2. 
 */
//#define DEBUG_TIME2
int timer_2_3_init(void)
{
    SIZE_T ul_data;
    SIZE_T ul_flags;

    // Timer2 init.
#ifdef DEBUG_TIMER2
    printk(KERN_INFO"timer2 init\n");
#endif
  
    spin_lock_irqsave(&timer2, ul_flags); 
    //Set timer2 load value.
    hi_writel(TIMER2_BGLOAD_INIT, TIMER2_LOAD);
    hi_writel(TIMER2_BGLOAD_INIT, TIMER2_BGLOAD);

    // Set timer2 use main bus clock
    ul_data = hi_readl(SC_CTRL);
#ifdef DEBUG_TIMER2
    printk(KERN_INFO "Befor SC_CTRL = %lx.\n", ul_data);
#endif
    clear_bit(20, &ul_data);
    hi_writel(ul_data, SC_CTRL);

#ifdef DEBUG_TIMER2
    ul_data = hi_readl(SC_CTRL);
    printk(KERN_INFO "After SC_CTRL = %lx.\n", ul_data);
#endif

    // Set timer2 enable, run in cycle mode, 
    // interrupt is enable, no dispart frequency
    // 32 bit count mode
    // timer2 interrupt time =  
    ul_data = hi_readl(TIMER2_CONTROL);
#ifdef DEBUG_TIMER2
    printk(KERN_INFO "Befor TIMER2_CONTROL = %lx.\n", ul_data);
#endif

    //Timer2 run int periodic mode, and don't mask the interrupt, no running
    ul_data |= 0xE2;                //0x62
    hi_writel(ul_data, TIMER2_CONTROL);

#ifdef DEBUG_TIMER2
    ul_data = hi_readl(TIMER2_CONTROL);
    printk(KERN_INFO "After TIMER2_CONTROL = %lx.\n", ul_data);
#endif

    spin_unlock_irqrestore(&timer2, ul_flags); 


    //if( request_irq(INT_TM2_TM3, timer_2_3_alarm, IRQF_SHARED, // SA_INTERRUPT, 
    if( request_irq(INT_TM2_TM3, timer_2_3_alarm, IRQF_SHARED, "INT_TM2", &u32_tm2))
    {
       printk(KERN_ERR"err: IRQ%d allread in use.\n", INT_TM2_TM3); 
       return FAULT;
    }

    return SUCCESS;
}

/*
 * Function   : gpio_init
 * data       : void
 * return     : U32 SUCCESS
 *                  FAULT
 * Description: Set some ping with mix function as GPIO, and set it match we
 *              need.
 */
//#define DEBUG_GPIO_INIT 
U32 gpio_init(void)
{
    SIZE_T  ul_data;
#ifdef DEBUG_GPIO_INIT
    printk(KERN_INFO"gpio init is runing.\n");  
#endif
/*
    //GPIO6_0
    ul_data = hi_readl(MUXCTRL_CRG32);
    clear_bit(0, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG32);


    //GPIO6_1
    ul_data = hi_readl(MUXCTRL_CRG33);
    clear_bit(0, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG33);


    //GPIO6_3
    ul_data = hi_readl(MUXCTRL_CRG35);
    clear_bit(0, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG35);


    //GPIO6_4
    ul_data = hi_readl(MUXCTRL_CRG36);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
    clear_bit(2, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG36);

    //GPIO6_5
    ul_data = hi_readl(MUXCTRL_CRG37);
    clear_bit(0, &ul_data);
    set_bit(1, &ul_data);
    clear_bit(2, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG37);

    //GPIO6_6
    ul_data = hi_readl(MUXCTRL_CRG38);
    clear_bit(0, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG38);

    //GPIO6_7
    ul_data = hi_readl(MUXCTRL_CRG39);
    clear_bit(0, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG39);

    ul_data = hi_readl(GPIO6_DIR);
	ul_data |= 0xFB;
	hi_writel(ul_data, GPIO6_DIR);


    //GPIO1_1
    ul_data = hi_readl(MUXCTRL_CRG1);
    clear_bit(0, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG1);

	ul_data = hi_readl(GPIO1_DIR);
	set_bit(1, &ul_data);
	hi_writel(ul_data, GPIO1_DIR);
*/

    //GPIO4_0
    ul_data = hi_readl(MUXCTRL_CRG17);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG17);


    //GPIO4_1
    ul_data = hi_readl(MUXCTRL_CRG16);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG16);


    //GPIO4_2
    ul_data = hi_readl(MUXCTRL_CRG15);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG15);


    //GPIO4_3
    ul_data = hi_readl(MUXCTRL_CRG14);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG14);

    //GPIO4_4
    ul_data = hi_readl(MUXCTRL_CRG21);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG21);

    //GPIO4_5
    ul_data = hi_readl(MUXCTRL_CRG20);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG20);

    //GPIO4_6
    ul_data = hi_readl(MUXCTRL_CRG19);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG19);

    //GPIO4_7
    ul_data = hi_readl(MUXCTRL_CRG18);
    clear_bit(0, &ul_data);
    clear_bit(1, &ul_data);
	hi_writel(ul_data, MUXCTRL_CRG18);


	hi_writel(0xFF, GPIO4_DIR);



    return SUCCESS;
}


/*
 * function     : sm_init
 * data         : (void)
 * return       : (none) 
 * descritpion  : register device and init GPIO that to be use.
 *                GPIO5 for control sm_2003
 *                GPIO7_0 GPIO7_1 GPIO7_2 GPIO7_3 for the end detect  
 */
static S32 __init drv_init(void)
{
    S32 s32_ret = 0;

    s32_ret = misc_register(&drv_dev);
    if(s32_ret)
    {
        printk(KERN_ERR"err: register pt2803.\n");
        return s32_ret;
    }

    if(gpio_init() != SUCCESS)
    {
        misc_deregister(&drv_dev);
        printk(KERN_ERR"err: pt2803 GPIO init.\n");
        return FAULT;
    }

    if(timer_2_3_init() != SUCCESS)
    {
        misc_deregister(&drv_dev);
        printk(KERN_ERR"err: sm_2003 GPIO init.\n");
        return FAULT;
    }

    pt_devp = kmalloc(sizeof(struct pt_dev), GFP_KERNEL);
    if(pt_devp == NULL)
    {
        misc_deregister(&drv_dev);
        printk(KERN_ERR"err: sm_2003 kmalloc.\n");
        return -1;
    }
    memset(pt_devp, 0, sizeof(struct pt_dev));
    pt_devp->ui_v_mode = STAND_BY;
    pt_devp->ui_h_mode = STAND_BY;
    pt_devp->ui_h_total_steps = H_TOTAL_STEPS;
    pt_devp->ui_v_total_steps = V_TOTAL_STEPS;
  //  pt_devp->ui_v_steps = 1000;
  //  pt_devp->ui_v_steps = 1000;

/*
    //When startup detect if it at the boundary of  directon 
    pt_devp->ui_v_start_pos = v_check_pos();
    pt_devp->ui_h_start_pos = h_check_pos();
*/
    pt_devp->ui_h_start_pos = OTHER_POS;
    pt_devp->ui_v_start_pos = OTHER_POS;

    init_waitqueue_head(&pt_devp->wqh_return);
   //vertical self check
    if(si_v_self_check == ON)
    {
        pt_devp->ui_v_mode = SELF_CHECK;
        v_timer_start();
    }

    //horizontal self check
    if(si_h_self_check == ON)
    {
        pt_devp->ui_h_mode = SELF_CHECK;
        v_timer_start();
    //    h_start();
    }

    //hi_writel(00, (SM_BASE + 0x3FC));
    V_RELEASE();
	H_RELEASE();
//	hi_writel(0xFF, GPIO1_1);
    
//	V_STEPA();
//	V_STEPB();
//	V_STEPC();
	//V_STEPD();
	
//	H_STEPA();
	//H_STEPB();
//	H_STEPC();
	//H_STEPD();
    
    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s init succed.\n", DRV_NAME);

    return 0;
}

static void __exit drv_exit(void)
{
    unsigned long int uldata;

	//pk("90");
    misc_deregister(&drv_dev);

	//pk("91");
    
    //Disable timer2
    uldata = hi_readl(TIMER2_CONTROL);
    clear_bit(7, &uldata);                             // 0x62 = 0101 0010 B disenable
    hi_writel(uldata, TIMER2_CONTROL);
    //pk("92");
    free_irq(INT_TM2_TM3, &u32_tm2);
    msleep(50);
    V_RELEASE();
    H_RELEASE();
    //pk("93");
    kfree(pt_devp);

    printk(KERN_INFO"%s Verson = %s Compile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s rmmoded\n", DRV_NAME);
}
module_init(drv_init);
module_exit(drv_exit);


MODULE_AUTHOR("Netviewtech qiang.feng << qiang.feng@netvuewtech >>");
MODULE_DESCRIPTION("Step motor driver, hardware driver ULN2003A, for board of C05IPA");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
