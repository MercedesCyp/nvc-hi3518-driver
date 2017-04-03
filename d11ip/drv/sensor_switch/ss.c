/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *     using which sensor (day vision sensor or night vision sensor) base on
 *     poto or man control
 *
 *     day vision sensor power enable GPIO5_2 low
 *     night vision sensor power enable GPIO5_3 low
 *
 * 141025
 *      day sensor               GPIO3_1
 *      night sensor             GPIO3_0
 * 
 *
 *      poto use ADC channel one 
 * 
 *
 *  Support Hisilicon's chips, as HI3518
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 * File name : ss.c
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
#include "ss.h"
#include "type.h"



#define DRV_NAME                 "ss"
#define DRV_VERSION              "14.10.25 -- for: D11IPD"
#define DRV_MINOR                20
#define DRV_DESCRIPTION          "sensor switch and IR led switch for SOC:HI3518C\
                                  board: D11IPC"

//#define DEBUG
#ifdef  DEBUG
#define pk(fmt,args...)\
    printk(KERN_INFO "%s [%s ,%d]: " fmt "\n",DRV_NAME,__FUNCTION__,__LINE__,##args);
#else
#define pk(fmt,args...)
#endif



#define CDS_KEEP        (3)     // if CDS level had been changed and keep the
                                // state in specially second in the model AUTO  
                                // SS and IR-LED will chage to corresponding 
                                // state (the unit is second)

                              
#define CHECK_TIME      (50)    // Interval time(the unit is 10ms) that will 
                                // check PT's state from this time to next.
#define MD_TIME         (5)     // application will check if there IR-CUT or
                                // IR-LED switch
								
//if the value is greater than PT_DAY. It is now in day
#define PT_DAY          (0x3a)
//if the value is smaller than PT_NIGHT. it is now in night 
#define PT_NIGHT        (0x2a)


 /* The time that CDS's level change and keep before SS and IR LED state change.
 */

    static U32 u32_cds_keep =  CDS_KEEP;
    module_param(u32_cds_keep,  uint, 0);
    MODULE_PARM_DESC(u32_cds_keep, 
    "The time that CDS's level change and keep before SS and IR LED state change.(default= 3  second)");


/*
 * if SS in the state of DAY or NIGHT, the SS will hold the state.
 * if SS in the state of AUTO, the SS will change the state base on CDS.
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

struct ss_dev
{
    struct timer_list ss_timer;
 
    //switch_rec[0][0] -------- record IR-LED switch time(jiffies)
    //switch_rec[0][1] -------- IR-LED from on to off or reverse
    //switch_rec[1][0] -------- record SS switch time(jiffies)
    //switch_rec[1][1] -------- IR-CUT from infrared pass to end or reverse
    U32 switch_rec[2][2];
    struct ss_data ss_data;
    U32 u32_cms_type;               //The type of CMOS sensor
    //wait_queue_head wqh_adc;
    U16 u16_adc_result;             //story ADC result
	S32 s32_adc_need;               //indicate ss_timer need to start ADC
	                                //TRUE or FAULT
	U32 u32_sensor_type;            //which sensor is now connect to system
};

struct ss_dev *ss_devp;



static int ss_open(struct inode * inode, struct file *file)
{
    return 0;
}

static int ss_release(struct inode *inode, struct file *filp)
{
    return 0;
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
/*
	//open clock of video otherwise system will stop
	hi_writel(0x2A, PERI_CRG11);
	ul_data = hi_readl(PERI_CRG11);
    //pk("PERI_CRG11 = %lx", ul_data);
*/	
    //bypass color 
	/*
	//way one
	ul_data = hi_readl(BYPASS);
    if(u32_model == IMG_MONO)	
	{
	    set_bit(13, &ul_data);
		hi_writel(ul_data, BYPASS);
	}
	if(u32_model == IMG_COLOR)
	{
	    clear_bit(13, &ul_data);
		hi_writel(ul_data, BYPASS);
	}
	*/
	//way two
	ul_data = hi_readl(CCM_CTRL);
    if(u32_model == IMG_MONO)	
	{
	    clear_bit(0, &ul_data);
		hi_writel(ul_data, CCM_CTRL);
	}
	if(u32_model == IMG_COLOR)
	{
	    set_bit(0, &ul_data);
		hi_writel(ul_data, CCM_CTRL);
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

        ss_devp->ss_data.u32_led_state = ON;
        ss_devp->switch_rec[0][0] = jiffies;
        ss_devp->switch_rec[0][1] = OFF_2_ON;
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

            ss_devp->ss_data.u32_led_state = OFF;
            ss_devp->switch_rec[0][0] = jiffies;
            ss_devp->switch_rec[0][1] = ON_2_OFF;
            return SUCCESS;
        }
    }
    return FAULT;
}


/*
 * function   : ss_switch
 * data       : U32 u32_ir
 *              IR_END  -------- use day vision sensor
 *              SS_PASS  -------- use night vision sensor
 * return     : U32 SUCCESS -------- sucess
 *              U32 FAULT   -------- fault(the data tranfer is error)
 * description: switch which sensor to use, day vison or night vision sensor
 *              on day vision sensor IR was end, but on night vison sensor
 *              IR was pass
 */
static U32 ss_switch(U32 u32_ir)
{
    SIZE_T ul_reg_val;

    if(u32_ir == IR_PASS)
    {
        ul_reg_val = hi_readl(SS_DIR);
        set_bit(SEN_DAY_SN, &ul_reg_val);
        set_bit(SEN_NIGHT_SN, &ul_reg_val);
        hi_writel(ul_reg_val, SS_DIR); 

        hi_writel((1 << SEN_DAY_SN), SEN_DAY_REG);
		mdelay(10);
        hi_writel(0x00, SEN_NIGHT_REG);

        ss_devp->ss_data.u32_ss_state = IR_PASS;
        ss_devp->switch_rec[1][0] = jiffies;
        ss_devp->switch_rec[1][1] = END_2_PASS;
        return SUCCESS;
    }
    else 
    {
        if(u32_ir == IR_END)
        {
    		ul_reg_val = hi_readl(SS_DIR);
            set_bit(SEN_DAY_SN, &ul_reg_val);
            set_bit(SEN_NIGHT_SN, &ul_reg_val);
            hi_writel(ul_reg_val, SS_DIR); 
    
            hi_writel((1 << SEN_NIGHT_SN), SEN_NIGHT_REG);
		    mdelay(10);
            hi_writel(0x00, SEN_DAY_REG);

            ss_devp->ss_data.u32_ss_state = IR_END;
            ss_devp->switch_rec[1][0] = jiffies;
            ss_devp->switch_rec[1][1] = PASS_2_END;
            return SUCCESS;
        }
    }
    return FAULT;
}




/*
 * function    : ss_day
 * data        : void
 * return      : void
 * description : set two sensor to the state of day.
 */
void ss_day(void)
{
    led_switch(OFF);    
    ss_switch(IR_END);
	img_model(IMG_COLOR);
}



/*
 * function    : ss_night
 * data        : void
 * return      : void
 * description : set two sensor to the state of night.
 */
void ss_night(void)
{
    ss_switch(IR_PASS);
    led_switch(ON);    
	img_model(IMG_MONO);
}




/*
 * function    : pt_read
 * data        : void 
 * return      : S32 SUCCESS
 *                   FAULT
 * description : read the value of A/D in channel 0
 *               ss_timer will start ADC. Because betwwen 
 *               hi_writel(ul_data, ADC_CTRL);   and
 *               sleep_on(&ss_devp->wqh_ss);
 *               if something unexpect was happen, then this program will not 
 *               be wake up
 *             
 */
S32 pt_read(void)
{
    SIZE_T ul_data;

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
    ss_devp->u16_adc_result = hi_readl(ADC_RESULT);
	pk("ADC result = %x",ss_devp->u16_adc_result);
    //hi_writel(0xFFFFFFFF, ADC_INT_CLR);

    return SUCCESS;
}





/*
 * function    : model_auto
 * data        : void
 * return      : void
 * description : switch SS and IR-LED depend on the CDS
 */
void model_auto(void)
{
    pt_read();
    if(ss_devp->u16_adc_result > ((PT_DAY - PT_NIGHT)/2 + PT_NIGHT))
    {
	    ss_devp->ss_data.u32_state = STATE_DAY;
        ss_day();
    }
    else
    {
	    ss_devp->ss_data.u32_state = STATE_NIGHT;
        ss_night();
    }
}


/*
 * function    : model_day
 * data        : void
 * return      : void
 * description : switch double sensor and IR-LED to day model
 */
void model_day(void)
{
	//pk("01\n");
    ss_day();
}



/*
 * function    : model_night
 * data        : void
 * return      : void
 * description : switch double and IR-LED to night model
 */
void model_night(void)
{
    ss_night();
}



/*
 * function     : state_check
 * date         : void
 * return       : void
 * description  : check PT's value, and story the result in ss_devp->ss_data.u32_state
*/
void state_check(void)
{
    pt_read(); 
    if(ss_devp->u16_adc_result > PT_DAY)
    {
         ss_devp->ss_data.u32_state = STATE_DAY;    
		 return;
    }
	if(ss_devp->u16_adc_result < PT_NIGHT)
    {
         ss_devp->ss_data.u32_state = STATE_NIGHT;    
    }
}



/*
 * function     : ss_timer_handle
 * data         : unsigned long arg
 * return       : void 
 * description  : every  ?? second SS switch check one time
 */
static void ss_timer_handle(unsigned long arg)
{
   //static uint uicount = 0;
   // static uint uibefor_time_count = 
    static U32 u32_times_count = 0;
    //SIZE_T ul_data;
	//static U32 u32_state;
    //static U32 i;
   //static j = 0;
   //
   //return ;
	
    mod_timer(&ss_devp->ss_timer, jiffies + CHECK_TIME);
   
    if(ss_devp->ss_data.u32_model != MOD_AUTO)
    {
        return ;
    }

    pt_read();
   // pk("ss_data.u32_state = %x", ss_devp->ss_data.u32_state);
    //pk(" ss_data.u32_ss_state= %x", ss_devp->ss_data.u32_ss_state);
   // pk(" ss_data.j= %x", ss_devp->ss_data.u32_ss_state);
   // pk("ss_data.u32_state = %x", ss_devp->ss_data.u32_state);

    if((ss_devp->ss_data.u32_state == STATE_DAY)
	    && (ss_devp->u16_adc_result < PT_NIGHT))
    {
        if(++u32_times_count > ss_devp->ss_data.u32_befor_times)
        {
		    ss_devp->ss_data.u32_state = STATE_NIGHT;
		    ss_night();
            u32_times_count = 0;
		//	pk("01");
        }
		return;
    }
    else
    {
        if((ss_devp->ss_data.u32_state == STATE_NIGHT)
		   && (ss_devp->u16_adc_result > PT_DAY)) 
        {
            if(++u32_times_count > ss_devp->ss_data.u32_befor_times)
            {

		        ss_devp->ss_data.u32_state = STATE_DAY;
                ss_day();
			   // pk("02");
            }
			return;
        }
    }
    u32_times_count = 0;
}


static long ss_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    void __user* argp = (void __user*)arg;
    U32  u32_data = 0;
    U32 u32_ret;
    U32 s32_err = 0;

    if(_IOC_TYPE(cmd) != SS_IOC_MAGIC)
    {
        return -ENOTTY;
    }

    if(_IOC_NR(cmd) > SS_IOC_MAXNR)
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
        case  SET_SS_DAY:
        {
		    pk("01");
            ss_switch(IR_END);
            //ss_devp->ss_data.u32_state = STATE_DAY;
            ss_devp->ss_data.u32_model = MOD_USER;
            break;
         }
        case  SET_SS_NIGHT:
        {
		    pk("02");
            ss_switch(IR_PASS);
            //ss_devp->ss_data.u32_state = STATE_NIGHT;
            ss_devp->ss_data.u32_model = MOD_USER;
            break;
         }
        case  READ_SS_STATE:
        {
            u32_data = ss_devp->ss_data.u32_ss_state;
            if (copy_to_user(argp, (void*)(&u32_data),
                              sizeof(unsigned int)))
            {
                 panic("SS, SS_READ_DELAY, copy to user fail!\n");
                 u32_ret = -EFAULT;
            }
            break; 
        }
        case  SET_IRLED_ON:
        {
            led_switch(ON);
            ss_devp->ss_data.u32_model = MOD_USER;
            break;
        }
    
        case  SET_IRLED_OFF:
        {
            led_switch(OFF);
            ss_devp->ss_data.u32_model = MOD_USER;
            break;
         }
        case READ_IRLED_STATE:
        {
            if(ss_devp->ss_data.u32_led_state == ON)
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
            copy_to_user(argp,(void*)(&ss_devp->u16_adc_result), 
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
            ss_devp->ss_data.u32_cds_keep = u32_data;
            ss_devp->ss_data.u32_befor_times = u32_data*100/CHECK_TIME;
            break;
        }
        case  READ_CDS_KEEP:
        {
            u32_data = ss_devp->ss_data.u32_cds_keep;
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
 
            ss_devp->ss_data.u32_md_time = u32_data; 
            break;
        }
        case  READ_MD_TIME:
        {
            u32_data = ss_devp->ss_data.u32_md_time; 
            copy_to_user(argp, &u32_data, sizeof(int));
            break;
        }
        case  READ_MD:
        {
		  
		    if((jiffies - ss_devp->switch_rec[0][0])
		           < ss_devp->ss_data.u32_md_time*100) 
		    {
                u32_data = TRUE;
			}
			else
			{
    			if((jiffies - ss_devp->switch_rec[1][0])
    		           < ss_devp->ss_data.u32_md_time*100) 
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
 
        case  SET_SS_MODE_AUTO:
        {
            ss_devp->ss_data.u32_model = MOD_AUTO;
			model_auto();
            break;
        }
        case  SET_SS_MODE_DAY:
        {
            ss_devp->ss_data.u32_model = MOD_DAY;
            model_day();
            break;
        }
 
        case  SET_SS_MODE_NIGHT:
        {
            ss_devp->ss_data.u32_model = MOD_NIGHT;
            model_night();
            break;
        }

        case  SET_SS_MODE_USER:
        {
            ss_devp->ss_data.u32_model = MOD_USER;
            break;
        }
 
        case  SET_SS_MODE_RESET:
        {
        	ss_devp->ss_data.u32_befor_times = u32_cds_keep * 100 / CHECK_TIME;
            ss_devp->ss_data.u32_model = u32_model;
        	ss_devp->ss_data.u32_cds_keep = u32_cds_keep;
        	ss_devp->ss_data.u32_md_time = u32_md_time;
        }
        case READ_SS_MODE:
        {
            u32_data = ss_devp->ss_data.u32_model;
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



static struct file_operations ss_fops = {
  .owner           = THIS_MODULE,
  .open	           = ss_open,
  .unlocked_ioctl  = ss_ioctl,
  .release         = ss_release,
}; 

static struct miscdevice ss_dev = {
  .minor           = DRV_MINOR,
  .name            = DRV_NAME,
  .fops            = &ss_fops,
};



/*
 * function     : adc_init
 * data         : void
 * return       : void
 * description  : init A/D for photodiode's level tranform to digital
 */
void adc_init(void)
{
    hi_writel(0, ADC_POWERDOWN);       //enable A/D
    hi_writel(0x02, PERI_CRG32);      
    hi_writel(0x00, ADC_CTRL);         //chose channel 0 
    hi_writel(0x01, ADC_INT_MASK);     //disable A/D interrupt in channel 0
}



/*
 * function     : gpio_ss_init
 * data         : void
 * return       : void
 * description  : set the GPIO's function as we need.
 */
void  gpio_ss_init(void)
{
     SIZE_T ul_reg_val;

    //multiplexing control register for GPIO3_0 GPIO3_1
    ul_reg_val = hi_readl(MUXCTRL_REG12);
    clear_bit(0, &ul_reg_val);
    clear_bit(1, &ul_reg_val);
    hi_writel(ul_reg_val,MUXCTRL_REG12);

    ul_reg_val = hi_readl(MUXCTRL_REG13);
    clear_bit(0, &ul_reg_val);
    clear_bit(1, &ul_reg_val);
    hi_writel(ul_reg_val,MUXCTRL_REG13);

    //pin as output
    ul_reg_val = hi_readl(SS_DIR);
	set_bit(SEN_DAY_SN, &ul_reg_val);
	set_bit(SEN_NIGHT_SN, &ul_reg_val);
	hi_writel(ul_reg_val, SS_DIR);

    //for GPIO0_0
    ul_reg_val = hi_readl(MUXCTRL_REG72);
    clear_bit(0, &ul_reg_val);
    clear_bit(1, &ul_reg_val);
    hi_writel(ul_reg_val,MUXCTRL_REG72);

    //ping as output
    ul_reg_val = hi_readl(LED_DIR_REG);
	set_bit(LED_CTR_SN, &ul_reg_val);
	hi_writel(ul_reg_val, LED_DIR_REG);
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
	pk("SENSOR = UNKNOW");
    return SUCCESS;
}


/*
 * function     : ss_init
 * data         : (void)
 * return       : int
 * descritpion  : register device and init GPIO that to be use.
 */
static int __init ss_init(void)
{
    S32 s32_ret = 0;
    S32 i;

    s32_ret = misc_register(&ss_dev);
    if(s32_ret)
    {
        printk(KERN_ERR"err: Can not register ss devices. \n");
        return s32_ret;
    }

	//pk("01\n");

    ss_devp = kmalloc(sizeof(struct ss_dev), GFP_KERNEL);
    if(!ss_devp)
    {
        misc_deregister(&ss_dev);
        printk(KERN_ERR"err: kmalloc for ss_devp err.\n");
        return FAULT;
    }
    memset(ss_devp, 0, sizeof(struct ss_dev));

    ss_devp->ss_data.u32_befor_times = u32_cds_keep * 100 / CHECK_TIME;
    ss_devp->ss_data.u32_model = u32_model;
	ss_devp->ss_data.u32_cds_keep = u32_cds_keep;
	ss_devp->ss_data.u32_md_time = u32_md_time;

    sensor_rec();
    gpio_ss_init();
    adc_init();

    //pk("02\n");

    switch(ss_devp->ss_data.u32_model)
    {
        case MOD_DAY:
        {
	        //pk("02.1\n");
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
 
	//pk("03\n");

    init_timer(&ss_devp->ss_timer);
    ss_devp->ss_timer.function = &ss_timer_handle;
    ss_devp->ss_timer.expires = jiffies + 1;
    add_timer(&ss_devp->ss_timer);
	//mdelay(20);

    for(i = 0; i < 2; i++)
    {
        ss_devp->switch_rec[i][0] = 0;
        ss_devp->switch_rec[i][1] = 0;
    }

    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s init succed.\n", DRV_NAME);
    return SUCCESS;
}

static void __exit ss_exit(void)
{
    misc_deregister(&ss_dev);
    del_timer(&ss_devp->ss_timer);
    kfree(ss_devp);
    printk(KERN_INFO"%s Verson = %s \nCompile Time = %s %s\n",
           DRV_NAME, DRV_VERSION, __DATE__, __TIME__);
    printk(KERN_INFO"%s rmmoded\n", DRV_NAME);
}
module_init(ss_init);
module_exit(ss_exit);


MODULE_AUTHOR("Netviewtech qiang.feng <qiang.feng@netviewtech.com>");
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
