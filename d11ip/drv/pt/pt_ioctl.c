/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *              Study to use ioctl to control step motor.
 *
 *  Support Hisilicon's chips, as HI3512
 *  2012.01.03 qiang.feng<qiang.feng@netviewtech.com>
 *
 * GPIO5_0   PHASE1
 * GPIO5_1   I01
 * GPIO5_2   I11
 * GPIO5_3   PHASE2
 * GPIO5_4   I12
 * GPIO5_5
 * GPIO5_6
 * GPIO5_7   I02
 * 
 * GPIO6_3   KEY
 *
 * File name : sm_ioctl.c
 * Author    : qiang.feng
 * Date      : 2012.01.03
 * Version   : 1.00
 */


#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/config.h>
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
#include <asm/arch/hardware.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

#include "hi_common.h"
#include "step_motor.h"
//#include "sm_ioctl.h"
//#include "sm_irq.h"

extern  struct step_motor_data sm_data;

/*
 * function       : sm_horizon_stop
 * data           : void
 * return         : void
 * description    : stop the step morot the control horizon.
 */

void sm_horizon_stop(void)
{

    sm_data.uistate = STOPED;
}


/*
 * function       : sm_horizon_start
 * data           : void
 * return         : void
 * description    : start the step morot the control horizon.
 */

void sm_horizon_start(void)
{
    sm_data.uistate = RUNNING;
}




/*
 * function       : sm_horizon_run_start_point
 * data           : void
 * return         : void
 * description    : start the step morot the control horizon, run to 
 *                  start point and stop.
 */

void sm_horizon_run_start_point(void)
{
    printk(KERN_INFO"starting\n");

    sm_data.uistep = sm_data.uirun_step;
    sm_data.uidir = ANTICLOCKWISE;
    sm_data.uistate = STOPED;

}

/*
 * function       : sm_horizon_run_end_point
 * data           : void
 * return         : void
 * description    : start the step morot the control horizon, run to 
 *                  end point and stop.
 */

void sm_horizon_run_end_point(void)
{

    printk(KERN_INFO"ending\n");

    sm_data.uistep = sm_data.uimax_step - sm_data.uirun_step;
    sm_data.uidir = DEASIL;
    sm_data.uistate = STOPED;

}



/*
 * function       : sm_horizon_write_step
 * data           : unsigned long arg
 * return         : void
 * description    : write the step of the horizon direction
 */

void sm_horizon_write_step(ulong arg)
{
    void __user* argp = (void __user*)arg;
    struct step_motor_data * val;

    sm_data.uistate = STOPED;
    val = (struct step_motor_data *) kmalloc(sizeof(struct step_motor_data),GFP_KERNEL);
    if(val == NULL)
    {
        printk(KERN_ERR"err! enable to kmalloc for val.\n");
        return -EFAULT;
    }
    if (copy_from_user(val, argp, sizeof(struct step_motor_data)))
    {
        printk(KERN_INFO"step_motor_ioctl copy from user fail!\n");
        return -EFAULT;	
    }

    sm_data.uistep = val->uistep;
    sm_data.uidir = val->uidir;;
    //sm_data.uistate = RUNNING;
}


/*
 * function       : sm_horizon_write_speed
 * data           : unsigned long arg
 * return         : void
 * description    : set the speed level of the horizon direction
 */

int  sm_horizon_write_speed(unsigned long arg)
{
    uint uiset_speed, uitmp = 13210 * 5;
    void __user* argp = (void __user*)arg;
    struct step_motor_data * val;

    sm_data.uistate = STOPED;
    val = (struct step_motor_data *) kmalloc(sizeof(struct step_motor_data),GFP_KERNEL);
    if(val == NULL)
    {
        printk(KERN_ERR"err! enable to kmalloc for val.\n");
        return -EFAULT;
    }
    if (copy_from_user(val, argp, sizeof(struct step_motor_data)))
    {
        printk(KERN_INFO"step_motor_ioctl copy from user fail!\n");
        return -EFAULT;	
    }


 
  //  printk(KERN_INFO"Now write speed. uispeed = %d\n", val->uispeed);
    sm_data.uispeed = val->uispeed;
    switch(val->uispeed)
    {
        case 0:      sm_data.uistate  = STOPED; break; 
        case 1:  
                     hi_writel(uitmp * 5, TIMER2_BGLOAD); 
                     sm_data.uistate = RUNNING;
                     break;
                                          
        case 2:   
                     hi_writel(uitmp * 4, TIMER2_BGLOAD);
                     sm_data.uistate = RUNNING;
                     break;
        case 3:    
                     hi_writel(uitmp * 3, TIMER2_BGLOAD);
                     sm_data.uistate = RUNNING;
                     break;
        case 4:     
                     hi_writel(uitmp * 2, TIMER2_BGLOAD);
                     sm_data.uistate = RUNNING;
                     break;
        case 5:      
                     hi_writel(uitmp , TIMER2_BGLOAD);
                     sm_data.uistate = RUNNING;
                     break;
        default:     break;
          
    }

    sm_data.uistate = STOPED;
    kfree(val);
    return 0;
}


/*
 * function      : sm_horizon_write_half
 * data          : struct step_motor_data
 * return        : void 
 * description   : set step mode for half mode
 */
int  sm_horizon_write_half(ulong arg)
{   
    void __user* argp = (void __user*)arg;
    struct step_motor_data * val;

    sm_data.uistate = STOPED;
    val = (struct step_motor_data *) kmalloc(sizeof(struct step_motor_data),GFP_KERNEL);
    if(val == NULL)
    {
        printk(KERN_ERR"err! enable to kmalloc for val.\n");
        return -EFAULT;
    }
    if (copy_from_user(val, argp, sizeof(struct step_motor_data)))
    {
        printk(KERN_INFO"step_motor_ioctl copy from user fail!\n");
        return -EFAULT;	
    }

    sm_data.uistep_mode = HALF_STEP;
    sm_data.uicur = val->uicur;
 //   printk(KERN_INFO"write_half uistep_mode = %u\n", sm_data.uistep_mode);
    switch(sm_data.uicur)
    {
        case NO_CUR :    p1_crnt_zero();p2_crnt_zero();  break;
        case LOW_CUR:    p1_crnt_low(); p2_crnt_low();   break;
        case MED_CUR:    p1_crnt_med(); p2_crnt_med();   break;
        case MAX_CUR:    p1_crnt_max(); p2_crnt_max();   break;
        default     :        break;
    }
    kfree(val);    
    return 0;
}


/*
 * function      : sm_horizon_write_fall
 * data          : struct step_motor_data
 * return        : void 
 * description   : set step mode for full mode
 */
int  sm_horizon_write_full(ulong arg)
{   
    void __user* argp = (void __user*)arg;
    struct step_motor_data * val;

    sm_data.uistate = STOPED;
    val = (struct step_motor_data *) kmalloc(sizeof(struct step_motor_data),GFP_KERNEL);
    if(val == NULL)
    {
        printk(KERN_ERR"err! enable to kmalloc for val.\n");
        return -EFAULT;
    }
    if (copy_from_user(val, argp, sizeof(struct step_motor_data)))
    {
        printk(KERN_INFO"step_motor_ioctl copy from user fail!\n");
        return -EFAULT;	
    }

    sm_data.uistep_mode = FULL_STEP;
    sm_data.uicur = val->uicur;
//    printk(KERN_INFO"write_full uistep_mode = %u\n", sm_data.uistep_mode);
    switch(sm_data.uicur)
    {
        case NO_CUR :    p1_crnt_zero();p2_crnt_zero();  break;
        case LOW_CUR:    p1_crnt_low(); p2_crnt_low();   break;
        case MED_CUR:    p1_crnt_med(); p2_crnt_med();   break;
        case MAX_CUR:    p1_crnt_max(); p2_crnt_max();   break;
        default     :        break;
    }
    kfree(val);    
    return 0;
}





/*
 * function      : sm_horizon_write_direction
 * data          : struct step_motor_data
 * return        : void 
 * description   : set step mode for full mode
 */
int  sm_horizon_write_direction(ulong arg)
{   
    void __user* argp = (void __user*)arg;
    struct step_motor_data * val;
    printk(KERN_INFO"write_direction.\n");
    sm_data.uistate = STOPED;
    val = (struct step_motor_data *) kmalloc(sizeof(struct step_motor_data),GFP_KERNEL);
    if(val == NULL)
    {
        printk(KERN_ERR"err! enable to kmalloc for val.\n");
        return -EFAULT;
    }
    if (copy_from_user(val, argp, sizeof(struct step_motor_data)))
    {
        printk(KERN_INFO"step_motor_ioctl copy from user fail!\n");
        return -EFAULT;	
    }

    sm_data.uidir = val->uidir;;
    kfree(val);    
    return 0;
}




/*
 * function      : sm_horizon_read
 * data          : struct step_motor_data
 * return        : void 
 * description   : read all state of step motor
 */
int  sm_horizon_read(ulong arg)
{   
    void __user* argp = (void __user*)arg;
    struct step_motor_data * val;
    if (copy_to_user(argp, &sm_data, sizeof(struct step_motor_data)))
    {
        printk(KERN_INFO"step_motor_ioctl copy from user fail!\n");
        return -EFAULT;	
    }
    return 0;
}


