/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:

 *  Support Hisilicon's chips, as HI3518
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 *  140509
 *      change saturation of ISP to make monochroic at night model
 *
 *  140527
 *      change color model with notify application way. And application change 
 *      color saturation to reach result.
 *
 *  GPIO5_3 for IR-CUT switch
 *  voltage change from low to high optical filter will change to   mode
 *                      high to low                                 mode 
 *
 * File name : irc.c
 * Author    : qiang.feng
 */


#include <linux/string.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
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
#include <linux/timer.h>

#include "hi3518_com.h"
#include "irc.h"
#include "type.h"



#define DRV_NAME                 "irc"
#define DRV_VERSION              "14.01.15 - test for D01IPC D04IP"
#define DRV_MINOR                20
#define DRV_DESCRIPTION          "IR cut control and IR led for SOC:HI3518C\
                                  board: D11IPC"

#define DEBUG
#ifdef  DEBUG
#define pk(fmt,args...)\
    printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DRV_NAME,__FUNCTION__,__LINE__,##args);
#else
#define pk(fmt,args...)
#endif



#define CDS_KEEP        (3)     // if CDS level had been changed and keep the
                                // state in specially second in the model AUTO  
                                // IRC and IR-LED will chage to corresponding 
                                // state (the unit is second)

                              
#define CHECK_TIME      (50)    // Interval time(the unit is 10ms) that will 
                                // check PT's state from this time to next.
#define MD_TIME         (5)     // application will check if there IR-CUT or
                                // IR-LED switch
								
//if the value is greater than PT_DAY. It is now in day
#define PT_DAY          (0x30)
//if the value is smaller than PT_NIGHT. it is now in night 
#define PT_NIGHT        (0x20)


 /* The time that CDS's level change and keep before IRC and IR LED state change.
 */

    static U32 u32_cds_keep =  CDS_KEEP;
    module_param(u32_cds_keep,  uint, 0);
    MODULE_PARM_DESC(u32_cds_keep, 
    "The time that CDS's level change and keep before IRC and IR LED state change.(default= 3  second)");


/*
 * if IRC in the state of DAY or NIGHT, the IRC will hold the state.
 * if IRC in the state of AUTO, the IRC will change the state base on CDS.
 *
 */
    static U32 u32_model  = MOD_AUTO;
    module_param(u32_model, uint, 0);
    MODULE_PARM_DESC(u32_model, 
    "MOD_NIGHT(0x20),MOD_DAY(0x21), MOD_AUTO(0x22),(default= MOD_AUTO)");



    static U32 u32_md_time  = MD_TIME;
    module_param(u32_md_time, uint, 0);
    MODULE_PARM_DESC(u32_md_time,
	"if IR-CUT and IR-LED had been switch between this time, (default = 5)");

struct irc_dev
{
    struct timer_list irc_timer;
 
    //switch_rec[0][0] -------- record IR-LED switch time(jiffies)
    //switch_rec[0][1] -------- IR-LED from on to off or reverse
    //switch_rec[1][0] -------- record IRC switch time(jiffies)
    //switch_rec[1][1] -------- IR-CUT from infrared pass to end or reverse
    U32 switch_rec[2][2];
    struct irc_data irc_data;
    U32 u32_cms_type;               //The type of CMOS sensor
    //wait_queue_head wqh_adc;
    U16 u16_adc_result;             //story ADC result
	S32 s32_adc_need;               //indicate drv_timer need to start ADC
	                                //TRUE or FAULT
	U32 u32_sensor_type;            //which sensor is now connect to system
	U32 u32_sat_target;
	struct fasync_struct *async_queue;
};

struct irc_dev *irc_devp;



static int drv_open(struct inode * inode, struct file *file)
{
    return 0;
}

static int drv_release(struct inode *inode, struct file *filp)
{
    return 0;
}


static int drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
    if(size != 4)
	{
	    return FAULT;
	}
    
	copy_to_user(buf, &irc_devp->irc_data.u32_irc_state, 4);
    return 0;
}





static int drv_fasync(int fd, struct file *filp, int mode)
{
    return fasync_helper(fd, filp, mode, &irc_devp->async_queue);
}



/*
 * functon     : img_model
 * data        : U32 u32_model -------- IMG_MONO
 *                             -------- IMG_COLOR
 * return      : void
 * description : change color model of image
 */
void img_model(U32 u32_model)
{
    SIZE_T ul_data = 0;
	hi_writel(0x2A, PERI_CRG11);
	ul_data = hi_readl(PERI_CRG11);
    //pk("PERI_CRG11 = %lx", ul_data);
	//ul_data = hi_readl(BYPASS);
    if(u32_model == IMG_MONO)	
	{
	    if(irc_devp->async_queue)
	    {
	         kill_fasync(&irc_devp->async_queue, SIGIO, POLL_IN);
	    }
		//printk(KERN_INFO"now monochrome");
		//printk(KERN_INFO"demosaic sta thresh = %ld\n", ul_data);
	}
	if(u32_model == IMG_COLOR)
	{
    	if(irc_devp->async_queue)
	    {
	         kill_fasync(&irc_devp->async_queue, SIGUSR1, POLL_IN);
	    }
		//printk(KERN_INFO"now color");
	}
}



/*
 * functon     : led_switch
 * data        : U32 u32_state -------- ON
 *                             -------- OFF
 * return      : SUCCESS -------- sucess
 *               FAULT   -------- fault
 * description : if u32_state = ON turn IR-LED on
 *               otherwise turn IR-LED  off
 *               if LED_CTL ping if high IR-LED will be turn on
 *               or it will turn off.
 */
S32 led_switch(U32 u32_state)
{
    SIZE_T ul_reg_val;

    if(u32_state == ON)
    { 
	   // pk("01");
        ul_reg_val = hi_readl(LED_DIR_REG);
        set_bit(LED_CTR_SN, &ul_reg_val);
        hi_writel(ul_reg_val, LED_DIR_REG); 
        hi_writel((1 << LED_CTR_SN), LED_CTR_REG);
        irc_devp->irc_data.u32_led_state = ON;
        irc_devp->switch_rec[0][0] = jiffies;
        irc_devp->switch_rec[0][1] = OFF_2_ON;
        return SUCCESS;
    }
    else 
    {
        if(u32_state == OFF)
        {
	       // pk("02");
            ul_reg_val = hi_readl(LED_DIR_REG);
            set_bit(LED_CTR_SN, &ul_reg_val);
            hi_writel(ul_reg_val, LED_DIR_REG); 
            hi_writel(0x00, LED_CTR_REG);
            irc_devp->irc_data.u32_led_state = OFF;
            irc_devp->switch_rec[0][0] = jiffies;
            irc_devp->switch_rec[0][1] = ON_2_OFF;
            return SUCCESS;
        }
    }
    return FAULT;
}


/*
 * function   : irc_switch
 * data       : U32 u32_ir
 *              IR_END  -------- set glass filter to day model
 *              IRC_PASS  -------- set glass filter to night model
 * return     : U32 SUCCESS -------- sucess
 *              U32 FAULT   -------- fault(the data tranfer is error)
 * description: glass filter set to infrared pass or prevent model
 */
static U32 irc_switch(U32 u32_ir)
{
    SIZE_T ul_reg_val;

    if(u32_ir == IR_END)
    {
        ul_reg_val = hi_readl(IRC_DIR_REG);
        set_bit(IRC_CTR_SN, &ul_reg_val);
        hi_writel(ul_reg_val, IRC_DIR_REG); 
        hi_writel(0x00, IRC_CTR_REG);
        irc_devp->irc_data.u32_irc_state = IR_PASS;
        irc_devp->switch_rec[1][0] = jiffies;
        irc_devp->switch_rec[1][1] = END_2_PASS;
        return SUCCESS;
    }
    else 
    {
        if(u32_ir == IR_PASS)
        {
            ul_reg_val = hi_readl(IRC_DIR_REG);
            set_bit(IRC_CTR_SN, &ul_reg_val);
            hi_writel(ul_reg_val, IRC_DIR_REG); 
            hi_writel((1 << IRC_CTR_SN), IRC_CTR_REG);
            irc_devp->irc_data.u32_irc_state = IR_END;
            irc_devp->switch_rec[1][0] = jiffies;
            irc_devp->switch_rec[1][1] = PASS_2_END;
            return SUCCESS;
        }
    }
    return FAULT;
}




/*
 * function    : irc_day
 * data        : void
 * return      : void
 * description : set IRC to the state of day.
 */
void irc_day(void)
{
    led_switch(OFF);    
    irc_switch(IR_END);
	img_model(IMG_COLOR);
}



/*
 * function    : irc_night
 * data        : void
 * return      : void
 * description : set IRC to the state of night.
 */
void irc_night(void)
{
    irc_switch(IR_PASS);
    led_switch(ON);    
	img_model(IMG_MONO);
}




/*
 * function     : irc_alarmirq
 * data         : int irq, void *id
 * return       : irqreturn_t i
 * description  : SAR_ADC irq server. Read the ADC result and store it 
 *                in irc_devp->u16_adc_result
*/
/*
static irqreturn_t irc_alarmirq(int irq, void *id)
{
    SIZE_T ul_data;

    irc_devp->u16_adc_result = hi_readl(ADC_RESULT);
    //clear ADC interrupt of ADC
    ul_data = hi_readl(ADC_INT_CLR);
    set_bit(0, &ul_data);
    hi_writel(ul_data, ADC_INT_CLR);
    wake_up(&irc_devp->wqh_irc);
    return IRQ_HANDLED;
}
*/



/*
 * function    : pt_read
 * data        : void  -------- the result had read from A/D
 *                              will be store in 
 *                              irc_devp->u16_adc_result
 * return      : S32 SUCCESS
 *                   FAULT
 * description : read the value of A/D in channel 0
 *               irc_timer will start ADC. Because betwwen 
 *               hi_writel(ul_data, ADC_CTRL);   and
 *               sleep_on(&irc_devp->wqh_irc);
 *               if something unexpect was happen, then this program will not 
 *               be wake up
 *             
 */
S32 pt_read(void)
{
    SIZE_T ul_data;

    //wait queue of tranfor
    //DECLARE_WAITQUEUQ(wq_tran, current);

    //add_wait_queue(&irc_devp->wqh_irc, &wq_tran)
    //irc_devp->s32_adc_need = TRUE;
    //sleep_on(&irc_devp->wqh_irc);
    //sleeping.....

    //remove_wait_queue(&irc_devp->wqh_irc, &wq_tran);
	/*
		ul_data = hi_readl(ADC_STATUS);
        printk(KERN_INFO"ADC_STATUS = %lx\n", ul_data);
		ul_data = hi_readl(ADC_CTRL);
        printk(KERN_INFO"ADC_CTRL = %lx\n", ul_data);
		ul_data = hi_readl(ADC_POWERDOWN);
        printk(KERN_INFO"ADC_POWERDOWN = %lx\n", ul_data);
		ul_data = hi_readl(ADC_INT_STATUS);
        printk(KERN_INFO"ADC_INT_STATUS = %lx\n", ul_data);
		ul_data = hi_readl(ADC_INT_MASK);
        printk(KERN_INFO"ADC_INT_MASK = %lx\n", ul_data);
		ul_data = hi_readl(ADC_INT_CLR);
        printk(KERN_INFO"ADC_INT_CLR = %lx\n", ul_data);
		ul_data = hi_readl(ADC_INT_RAW);
        printk(KERN_INFO"ADC_INT_RAW = %lx\n\n\n", ul_data);
    */
    hi_writel(0x00, ADC_POWERDOWN);    //Turn on ADC
	hi_writel(0x02, PERI_CRG32);       //turn on clock of ADC and cancal reset              
    hi_writel(0x00, ADC_CTRL);         //chose channel 0 
    hi_writel(0x01, ADC_INT_MASK);     //disable A/D interrupt in channel 0
    ul_data = hi_readl(ADC_CTRL);      //start A/D
    set_bit(0, &ul_data);
    hi_writel(ul_data, ADC_CTRL);

    do
	{
	    ul_data = hi_readl(ADC_CTRL);
    }
	while((ul_data & 0x01) != 0);
    irc_devp->u16_adc_result = hi_readl(ADC_RESULT);
	pk("ADC result = %x",irc_devp->u16_adc_result);
 //   hi_writel(0xFFFFFFFF, ADC_INT_CLR);

    return SUCCESS;
 
}





/*
 * function    : model_auto
 * data        : void
 * return      : void
 * description : switch IRC and IR-LED depend on the CDS
 */
void model_auto(void)
{
    pt_read();
    if(irc_devp->u16_adc_result > ((PT_DAY - PT_NIGHT)/2 + PT_NIGHT))
    {
	    irc_devp->irc_data.u32_state = STATE_DAY;
        irc_day();
    }
    else
    {
	    irc_devp->irc_data.u32_state = STATE_NIGHT;
        irc_night();
    }
}


/*
 * function    : model_day
 * data        : void
 * return      : void
 * description : switch IRC and IR-LED to day model
 */
void model_day(void)
{
	//pk("01\n");
    irc_day();
}




/*
 * function    : model_night
 * data        : void
 * return      : void
 * description : switch IRC and IR-LED to night model
 */
void model_night(void)
{
    irc_night();
}


/*
 * function     : state_check
 * date         : U32 *u32_state STATE_DAY   -------- state of day
 *                              STATE_NIGHT -------- state of night
 * return       : S32 SUCCESS
 *                    FAULT
 * description  : check PT's value, and return the state now
*/
/*
S32 state_check(U32 *u32_state)
{
    pt_read(); 
    if(irc_devp->u16_adc_result > ((PT_DAY - PT_NIGHT)/2 + PT_NIGHT))
    {
        *u32_state = STATE_DAY;    
    }
    else
    {
        *u32_state = STATE_NIGHT;
    }
    
    return SUCCESS;
}
*/



/*
 * function     : state_check
 * date         : void
 *               
 * return       : void
 * description  : check PT's value, and story the result in irc_devp->irc_data.u32_state
*/
void state_check(void)
{
    pt_read(); 
    if(irc_devp->u16_adc_result > PT_DAY)
    {
         irc_devp->irc_data.u32_state = STATE_DAY;    
		 return;
    }
	if(irc_devp->u16_adc_result < PT_NIGHT)
    {
         irc_devp->irc_data.u32_state = STATE_NIGHT;    
    }
}




/*
 * function     : irc_timer_handle
 * data         : unsigned long arg
 * return       : void 
 * description  : every  ?? second IRC switch check one time
 */
static void irc_timer_handle(unsigned long arg)
{
//    static uint uicount = 0;
   // static uint uibefor_time_count = 
    static U32 u32_times_count = 0;
    //SIZE_T ul_data;
	//static U32 u32_state;
   // static U32 i;

    //static j = 0;
    mod_timer(&irc_devp->irc_timer, jiffies + CHECK_TIME);
   
    if(irc_devp->irc_data.u32_model != MOD_AUTO)
    {
        return ;
    }

/*
    //read ADC result, it was cover last.
    //irc_devp->u16_adc_result = hi_readl(ADC_RESULT);

    //Start ADC for next time to read.
    ul_data = hi_readl(ADC_CTRL);
    set_bit(0, &ul_data);
    hi_writel(ul_data, ADC_CTRL);
*/

   // state_check();
    pt_read();
   // pk("irc_data.u32_state = %x", irc_devp->irc_data.u32_state);
    //pk(" irc_data.u32_irc_state= %x", irc_devp->irc_data.u32_irc_state);
   // pk(" irc_data.j= %x", irc_devp->irc_data.u32_irc_state);
   // pk("irc_data.u32_state = %x", irc_devp->irc_data.u32_state);

    if((irc_devp->irc_data.u32_state == STATE_DAY)
	    && (irc_devp->u16_adc_result < PT_NIGHT))
    {
        if(++u32_times_count > irc_devp->irc_data.u32_befor_times)
        {
		    irc_devp->irc_data.u32_state = STATE_NIGHT;
		    irc_night();
            u32_times_count = 0;
		//	pk("01");
        }
		return;
    }
    else
    {
        if((irc_devp->irc_data.u32_state == STATE_NIGHT)
		   && (irc_devp->u16_adc_result > PT_DAY)) 
        {
            if(++u32_times_count > irc_devp->irc_data.u32_befor_times)
            {

		        irc_devp->irc_data.u32_state = STATE_DAY;
                irc_day();
			   // pk("02");
            }
			return;
        }
    }
    u32_times_count = 0;
}


static long drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    void __user* argp = (void __user*)arg;
    U32  u32_data = 0;
    U32 u32_ret;
    U32 s32_err = 0;

    if(_IOC_TYPE(cmd) != IRC_IOC_MAGIC)
    {
        return -ENOTTY;
    }

    if(_IOC_NR(cmd) > IRC_IOC_MAXNR)
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
        case  SET_IRC_DAY:
        {
		    pk("01");
            irc_switch(IR_END);
            //irc_devp->irc_data.u32_state = STATE_DAY;
            irc_devp->irc_data.u32_model = MOD_USER;
            break;
         }
        case  SET_IRC_NIGHT:
        {
		    pk("02");
            irc_switch(IR_PASS);
            //irc_devp->irc_data.u32_state = STATE_NIGHT;
            irc_devp->irc_data.u32_model = MOD_USER;
            break;
         }
        case  READ_IRC_STATE:
        {
            u32_data = irc_devp->irc_data.u32_irc_state;
            if (copy_to_user(argp, (void*)(&u32_data),
                              sizeof(unsigned int)))
            {
                 panic("IRC, IRC_READ_DELAY, copy to user fail!\n");
                 u32_ret = -EFAULT;
            }
            break; 
        }
        case  SET_IRLED_ON:
        {
            led_switch(ON);
            irc_devp->irc_data.u32_model = MOD_USER;
            break;
        }
    
        case  SET_IRLED_OFF:
        {
            led_switch(OFF);
            irc_devp->irc_data.u32_model = MOD_USER;
            break;
         }
        case READ_IRLED_STATE:
        {
            if(irc_devp->irc_data.u32_led_state == ON)
            {
                u32_data = ON;
                copy_to_user(argp, (void*)(&u32_data), 
                              sizeof(unsigned int));
            }
            else
            {
                u32_data = OFF;
                copy_to_user(argp, (void*)(&u32_data),
                              sizeof(unsigned int));
            }
            break;
         }
        case  READ_CDS_VALUE:
        {
            pt_read();
            copy_to_user(argp,(void*)(&irc_devp->u16_adc_result), 
						sizeof(U16));
            break;
        }
        case  SET_CDS_KEEP:
        {
            copy_from_user(&u32_data, argp, sizeof(unsigned int));
            if((u32_data > 300) || (u32_data < 0))
            {
                printk(KERN_ERR"err:Delay time should between 0~300.\n");
                return FAULT;
            }
            irc_devp->irc_data.u32_cds_keep = u32_data;
            irc_devp->irc_data.u32_befor_times = u32_data*100/CHECK_TIME;
            break;
        }
        case  READ_CDS_KEEP:
        {
            u32_data = irc_devp->irc_data.u32_cds_keep;
            copy_to_user(argp, (void*)(&u32_data), sizeof(unsigned int));
            break;
        }
        
        case  SET_MD_TIME:
        {
            copy_from_user(&u32_data, argp, sizeof(int));
            if((u32_data > 20) || (u32_data < 0))
            {
                printk(KERN_ERR"err: Delay time should between 0~20.\n");
                return FAULT;
            }
 
            irc_devp->irc_data.u32_md_time = u32_data; 
            break;
        }
        case  READ_MD_TIME:
        {
            u32_data = irc_devp->irc_data.u32_md_time; 
            copy_to_user(argp, &u32_data, sizeof(int));
            break;
        }
        case  READ_MD:
        {
		  
		    if((jiffies - irc_devp->switch_rec[0][0])
		           < irc_devp->irc_data.u32_md_time*100) 
		    {
                u32_data = TRUE;
			}
			else
			{
    			if((jiffies - irc_devp->switch_rec[1][0])
    		           < irc_devp->irc_data.u32_md_time*100) 
    		    {
                    u32_data = TRUE;
    			}
				else
				{
			        u32_data = FAULT;
				}
			}
			copy_to_user(argp, &u32_data, sizeof(int));
            break;
        }
 
        case  SET_IRC_MODE_AUTO:
        {
            irc_devp->irc_data.u32_model = MOD_AUTO;
			model_auto();
            break;
        }
        case  SET_IRC_MODE_DAY:
        {
            irc_devp->irc_data.u32_model = MOD_DAY;
            model_day();
            break;
        }
 
        case  SET_IRC_MODE_NIGHT:
        {
            irc_devp->irc_data.u32_model = MOD_NIGHT;
            model_night();
            break;
        }

        case  SET_IRC_MODE_USER:
        {
            irc_devp->irc_data.u32_model = MOD_USER;
            break;
        }
 
        case  SET_IRC_MODE_RESET:
        {
        	irc_devp->irc_data.u32_befor_times = u32_cds_keep * 100 / CHECK_TIME;
            irc_devp->irc_data.u32_model = u32_model;
        	irc_devp->irc_data.u32_cds_keep = u32_cds_keep;
        	irc_devp->irc_data.u32_md_time = u32_md_time;
        }
        case READ_IRC_MODE:
        {
            u32_data = irc_devp->irc_data.u32_model;
            copy_to_user(argp, (void*)(&u32_data),
                          sizeof(unsigned int));
            break;
        }
        default :  
		{
	        return -ENOTTY;
	    }
    }
   
    return SUCCESS;
}



static struct file_operations irc_fops = {
  .owner           = THIS_MODULE,
  .open	           = drv_open,
  .read            = drv_read,
  .unlocked_ioctl  = drv_ioctl,
  .release         = drv_release,
  .fasync          = drv_fasync,
}; 

static struct miscdevice irc_dev = {
  .minor           = DRV_MINOR,
  .name            = DRV_NAME,
  .fops            = &irc_fops,
};



/*
 * function     : adc_init
 * data         : void
 * return       : void
 * description  : init A/D for photodiode's level tranform to digital
 */
void adc_init(void)
{
//    SIZE_T ul_data;

    hi_writel(0, ADC_POWERDOWN);       //enable A/D
    hi_writel(0x02, PERI_CRG32);      
    hi_writel(0x00, ADC_CTRL);         //chose channel 0 
    hi_writel(0x01, ADC_INT_MASK);     //disable A/D interrupt in channel 0
	/*
	ul_data = hi_readl(ADC_CTL);       //start ADC
	set_bit(1, &ul_data);
	hi_writel(ul_data, ADC_CTL);
	*/
}


/*
 * function     : gpio_irc_init
 * data         : void
 * return       : void
 * description  : set the GPIO's function as we need.
 */
void  gpio_irc_init(void)
{
     SIZE_T ul_reg_val;

    //multiplexing control register for GPIO5_3
    ul_reg_val = hi_readl(MUXCTRL_REG48);
    clear_bit(0, &ul_reg_val);
    hi_writel(ul_reg_val,MUXCTRL_REG48);

    //for GPIO0_0
    ul_reg_val = hi_readl(MUXCTRL_REG72);
    clear_bit(0, &ul_reg_val);
    clear_bit(1, &ul_reg_val);
    hi_writel(ul_reg_val,MUXCTRL_REG72);

}


/*
 * function   : sensor_rec
 * data       : void
 * return     : U32 SUCCESS
 *                  FAULT
 * description: recognise which sensor is now beening connect to ARM;
 */
extern int HI_I2C_Read(unsigned char I2cDevAddr, unsigned int I2cRegAddr, unsigned int I2cRegAddrByteNum, unsigned int DataLen);
U32 sensor_rec(void)
{
    //U32 u32_pidh, u32_pidl;
/*   
    u32_pidh = HI_I2C_Read(OV9712_DEV_ADD, OV9712_PIDH_ADD, 1, 1); 
    u32_pidl = HI_I2C_Read(OV9712_DEV_ADD, OV9712_PIDL_ADD, 1, 1); 
	if((u32_pidh == OV9712_PIDH) && (u32_pidl == OV9712_PIDL))
	{
	    irc_devp->u32_sensor_type = OV9712;
	    pk("SENSOR = OV9712");
		return SUCCESS;
	}
	*/
/*
    u32_pidh = gpio_sccb_read(OV7725_ADDR, 0x0A);
    u32_pidl = gpio_sccb_read(OV7725_ADDR, 0x0B);

  //  printk(KERN_INFO"dh = %x dl = %x\n", u32_pidh, u32_pidl);

    if((u32_pidh == 0x77) && (u32_pidl == 0x21))
    {
        irc_devp->u32_cms_type = OV7725;
        pk(KERN_INFO"dh = %x dl = %x cms = %d\n", u32_pidh, u32_pidl, 
                irc_devp->u32_cms_type);
        return SUCCESS;        
    }

    u32_pidh = gpio_sccb_read(OV2643_ADDR, 0x0A);
    u32_pidl = gpio_sccb_read(OV2643_ADDR, 0x0B);

  //  printk(KERN_INFO"dh = %x dl = %x\n", u32_pidh, u32_pidl);

    if((u32_pidh == 0x26) && (u32_pidl == 0x43))
    {
        irc_devp->u32_cms_type = OV2643;
        return SUCCESS;        
    }

    irc_devp->u32_cms_type = SENSOR_UNKNOW;
	*/
	pk("SENSOR = UNKNOW");
    return SUCCESS;
}


/*
 * function     : irc_init
 * data         : (void)
 * return       : int
 * descritpion  : register device and init GPIO that to be use.
 */
static int __init irc_init(void)
{
    S32 s32_ret = 0;
    S32 i;
	//SIZE_T ul_data;

    s32_ret = misc_register(&irc_dev);
    if(s32_ret)
    {
        printk(KERN_ERR"err: Can not register irc devices. \n");
        return s32_ret;
    }

	//pk("01\n");

    irc_devp = kmalloc(sizeof(struct irc_dev), GFP_KERNEL);
    if(!irc_devp)
    {
        misc_deregister(&irc_dev);
        printk(KERN_ERR"err: kmalloc for irc_devp err.\n");
        return FAULT;
    }
    memset(irc_devp, 0, sizeof(struct irc_dev));

    irc_devp->irc_data.u32_befor_times = u32_cds_keep * 100 / CHECK_TIME;
    irc_devp->irc_data.u32_model = u32_model;
	irc_devp->irc_data.u32_cds_keep = u32_cds_keep;
	irc_devp->irc_data.u32_md_time = u32_md_time;

    sensor_rec();
    gpio_irc_init();
    adc_init();

 //   pk("02\n");

    switch(irc_devp->irc_data.u32_model)
    {
        case MOD_DAY:
        {
//	        pk("02.1\n");
            model_day();
            break;
        }
        case MOD_NIGHT:
        {
            model_night();
            break;
        }
        case MOD_AUTO:
        {
            model_auto();
            break;
        }
        default:
        {
		    printk(KERN_ERR"err: enter model is not right(%d)\n", u32_model);
			return FAULT;
            break;
        }
    }
 
//	pk("03\n");

    init_timer(&irc_devp->irc_timer);
    irc_devp->irc_timer.function = &irc_timer_handle;
    irc_devp->irc_timer.expires = jiffies + 1;
    add_timer(&irc_devp->irc_timer);
//	mdelay(20);

    for(i = 0; i < 2; i++)
    {
        irc_devp->switch_rec[i][0] = 0;
        irc_devp->switch_rec[i][1] = 0;
    }
/*
	ul_data = hi_readl(DEMOSAIC_SAT_THRESH);
	irc_devp->u32_sat_target = (U32)ul_data;
    printk(KERN_INFO"DEMOSAIC_SAT_THRESH = %d\n", irc_devp->u32_sat_target);

	ul_data = hi_readl(DEMOSAIC_VH_OFFSET);
    printk(KERN_INFO"DEMOSAIC_VH_OFFSET = %ld\n", ul_data);
	hi_writel(0x00, DEMOSAIC_VH_OFFSET);

	ul_data = hi_readl(DEMOSAIC_SAT_OFFSET);
    printk(KERN_INFO"DEMOSAIC_SAT_OFFSET = %ld\n", ul_data);
//	hi_writel(0x00, DEMOSAIC_VH_OFFSET);
//
	ul_data = hi_readl(DEMOSAIC_SAT_SLOPE);
    printk(KERN_INFO"DEMOSAIC_SAT_SLOPE = %ld\n", ul_data);
    hi_writel(0x00, DEMOSAIC_SAT_SLOPE);
*/

	//pk("04\n");
    
   // irc_devp->irc_data.u32_defor_times = u32_defor_times * 100; 

    //init waitqueue head
    //init_waitqueue_head(&adc_devp->wqh_adc);
/*
    //request IRQ of ADC
    s32_ret = request_irq(IRQ_SAR_ADC, adc_alarmirq, IRQF_DISABLE,
                          "SAR_ADC_IRQ", &adc_alarmirq);
    if(s32_ret)
    {
        misc_deregister(&irc_dev);
        printk(KERN_ERR"err: request_irq, SAR_ADC_IRQ.\n");
        return FAULT;
    }
	*/
	//pk("05\n");
       
    //irc_switch(IR_PASS);
    //irc_switch(IR_END);

    //led_switch(ON);
    //led_switch(OFF);

    printk(KERN_INFO"%s Verson = %s Compile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s init succed.\n", DRV_NAME);
    return SUCCESS;
}

static void __exit irc_exit(void)
{
    misc_deregister(&irc_dev);
    del_timer(&irc_devp->irc_timer);
    kfree(irc_devp);
    printk(KERN_INFO"%s Verson = %s Compile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s rmmoded\n", DRV_NAME);
}
module_init(irc_init);
module_exit(irc_exit);


MODULE_AUTHOR("Netviewtech qiang.feng <qiang.feng@netviewtech.com>");
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
