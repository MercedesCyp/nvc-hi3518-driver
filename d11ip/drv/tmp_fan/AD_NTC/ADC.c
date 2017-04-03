#include <linux/string.h>
#include <linux/slab.h>  //kmalloc
#include <linux/module.h>
#include <linux/errno.h>
//#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>
//#include <asm/arch/hardware.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <mach/irqs.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#include <linux/wait.h>

#include "ADC.h"



#define CHECK_TIME 1000         //millisecond


void print_adc_regist(void)
{
	printk("ADC_STATUS:		%x\r\n",hi_readl(RD_ADC_STATUS));
	printk("ADC_CTRL:		%x\r\n",hi_readl(RD_ADC_CTRL));
	printk("ADC_POWERDOWN:	%x\r\n",hi_readl(RD_ADC_POWERDOWN));
	printk("ADC_INT_STATUS:	%x\r\n",hi_readl(RD_ADC_INT_STATUS));
	printk("ADC_INT_MASK:	%x\r\n",hi_readl(RD_ADC_INT_MASK));
	printk("ADC_INT_CLR:	%x\r\n",hi_readl(RD_ADC_INT_CLR));
	printk("ADC_INT_RAW:	%x\r\n",hi_readl(RD_ADC_INT_RAW));
	printk("ADC_RESULT:		%d\r\n",hi_readl(RD_ADC_RESULT));
}


typedef struct{
	wait_queue_head_t 	aWQH_Return;   
//	struct timer_list ADC_Time;    //Read ADC valut and output at regular time
	unsigned int 	irq_num;
	unsigned long 	irq_mode;
	const char  	*irq_name;
	uint16 			ADC_result;
	uint16 			ADC_Status;
}mADC_Struct;

static mADC_Struct *gADC_FUN_INT;

#define DC_AS_TempSampling		0xA5B3 	// 1010 0101 1011 0111   Temperature sampling
#define DC_AS_InfraredSampling	0x5A48 	// 0101 1010 0100 1000   Temperature sampling

#define DF_AS_IsTempSamp		( gADC_FUN_INT->ADC_Status&DC_AS_TempSampling )
#define DF_AS_IsInfrSamp		( gADC_FUN_INT->ADC_Status&DC_AS_InfraredSampling )
#define DF_AS_StartTempSamp		{ gADC_FUN_INT->ADC_Status|=DC_AS_TempSampling; }
#define DF_AS_StartInfrSamp		{ gADC_FUN_INT->ADC_Status|=DC_AS_InfraredSampling; }
#define DF_AS_ClrFlag			{ gADC_FUN_INT->ADC_Status=0; }


int ConvertChannelx(uint16 _Channel)
{
	if( !V_ADC_CONVERT_STATUS )
	{
		DECLARE_WAITQUEUE(DW_NTC, current);
		DF_AS_StartTempSamp;
		//print_adc_regist();
		switch(_Channel)
		{
			case Channel_0:
			
				SET_ADC_CHANNEL_0;
			
			break;
			case Channel_1:
			
				SET_ADC_CHANNEL_1;
			
			break;
			default:break;
		}
		
		/*
		add_wait_queue(&gADC_FUN_INT->aWQH_Return, &DW_NTC);
		SET_ADC_START;
		gADC_FUN_INT->ADC_Status|=DC_AS_TempSampling;
		sleep_on(&gADC_FUN_INT->aWQH_Return);
		remove_wait_queue(&gADC_FUN_INT->aWQH_Return, &DW_NTC);
		*/
		
		SET_ADC_START;
		
		add_wait_queue(&gADC_FUN_INT->aWQH_Return, &DW_NTC);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		remove_wait_queue(&gADC_FUN_INT->aWQH_Return, &DW_NTC);
		set_current_state(TASK_RUNNING);
		//print_adc_regist();
		//printk("-----2");
		//udelay(1000);
		
		return gADC_FUN_INT->ADC_result;
	}else
	{
		return -1;
	}
}



static irqreturn_t _interrupt(int irq, void *id)
{
	if(DF_AS_IsTempSamp)
	{
		DF_AS_ClrFlag;
		
		gADC_FUN_INT->ADC_result = V_ADC_VALUE;
		SET_ADC_CLR_INT;
		//wake_up(&gADC_FUN_INT->aWQH_Return);
		wake_up_interruptible(&gADC_FUN_INT->aWQH_Return);
		CLR_ADC_CONVER_STATUS;
	}
	return IRQ_HANDLED;
}
/*
static void F_ADC_Timer(unsigned long arg)
{
    static int i;
    uint32 ul_data;
    mod_timer(&gADC_FUN_INT->ADC_Time, jiffies + CHECK_TIME/10);
    //SET_ADC_START;
    ul_data = V_ADC_VALUE;
    printk(KERN_INFO"ADC times %4d = %4lu", i, ul_data);
    i++;
}
*/

int _InitADC(void)
{
	int theRct;
	printk("----------10\r\n");
	SET_ADC_POWER_UP;
	SET_EN_ADC_MODULE;
	SET_ADC_INT_ENABLE;
	
	SET_RESET_ADC_MODULE;
	udelay(1);
	SET_DISRESET_ADC_MODULE;
	
	printk("----------11\r\n");
    gADC_FUN_INT = kmalloc(sizeof( mADC_Struct), GFP_KERNEL);  
	memset(gADC_FUN_INT, 0, sizeof( mADC_Struct));
	
	gADC_FUN_INT->irq_num = 19;//IRQ_SAR_ADC;
	gADC_FUN_INT->irq_mode = IRQF_SHARED;//IRQF_DISABLED;//IRQF_SHARED;
	gADC_FUN_INT->irq_name = "SAR_ADC_IRQ";
	DF_AS_ClrFlag;
	printk("----------12\r\n");
	
	//request IRQ
	//theRct = request_irq(gADC_FUN_INT->irq_num, _interrupt, gADC_FUN_INT->irq_mode, gADC_FUN_INT->irq_name,&_interrupt);
	theRct = request_irq(gADC_FUN_INT->irq_num, _interrupt, gADC_FUN_INT->irq_mode, gADC_FUN_INT->irq_name,&request_irq);
	//theRct = request_irq(gADC_FUN_INT->irq_num, _interrupt, gADC_FUN_INT->irq_mode, gADC_FUN_INT->irq_name,&_interrupt);
	if(theRct)
    {printk("ni sha b a\r\n");
        return -1;
    }
	printk("----------13\r\n");
    //init wait queuq
    init_waitqueue_head(&(gADC_FUN_INT->aWQH_Return));
	
	printk("----------14\r\n");
	return 0;
}

void UninitADC(void)
{
	SET_ADC_POWER_DOWN;
	SET_ADC_CLR_INT;
	kfree(gADC_FUN_INT);
    free_irq(IRQ_SAR_ADC, &request_irq);
	SET_DIS_ADC_MODULE;
}


