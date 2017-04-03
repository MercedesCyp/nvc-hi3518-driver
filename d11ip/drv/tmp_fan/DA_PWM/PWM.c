//#include <linux/string.h>
//#include <linux/slab.h>
//#include <linux/module.h>
//#include <linux/errno.h>
//#include <linux/miscdevice.h>
//#include <linux/fcntl.h>
#include <linux/delay.h>
//#include <linux/proc_fs.h>
// #include <linux/workqueue.h>
// #include <linux/cdev.h>
// #include <asm/uaccess.h>
// #include <asm/system.h>
#include <asm/io.h>
// #include <asm/arch/hardware.h>
// #include <linux/sched.h>
// #include <linux/kthread.h>
// #include <linux/err.h>
// #include <mach/irqs.h>

#include <linux/kernel.h>
#include <linux/init.h>
// #include <linux/interrupt.h>
// #include <linux/wait.h>

#include "PWM.h"




#include "GlobalParameter.h"

#define PWM_SYS_Frequency	3000000		//3MHz
#define PWM_Out_Freqiency   10000		//10KHz
#define PWM_Out_Priod		(PWM_SYS_Frequency/PWM_Out_Freqiency)
#define CHIP_PPVoltage		3240		//3.24v
#define PWM_DutyCycle_Default	(PWM_SYS_Frequency/PWM_Out_Freqiency)



void DisableFanforIO(void)
{
	uint32 rect;
	//Make the PWM IO change to output mode
	rect = hi_readl(GPIO0_5+GPIO0_DIR);
	rect|=0x00000004;
	hi_writel(rect,GPIO0_5+GPIO0_DIR);
	//Make the PWM IO change to output mode
	rect = hi_readl(GPIO0_5+CPIOO_DATA);
	rect|=(0x4<<2);
	hi_writel(rect,GPIO0_5+CPIOO_DATA);
}


int InitPWM(void)
{ 
	DisableFanforIO();
	
	//open the tiem of PWM modules
	MUX_PWM_TIME_ON;
	//Reset the PWM modules
	MUX_PWM_EnRESET;
	//unReset the PWM modules
	MUX_PWM_DisRESET;
	//Make the GPIO5.2 turn on the PWM mode. 
	MUTCTL_GPIO5_2_PWM0;
	
	PWMx_Disable(PWM0);
	if(PWMx_IsBusy(PWM0))
	{
		printk("Init faild	!1!	\r\n");
		return -1;//the PWM module is busy
	}
	//PWMx_CTRL(PWM0);
	//printk("the REGIST %x\r\n",);
	PWMx_SetPriod(PWM0,PWM_Out_Priod);
	PWMx_SetDutyCycle(PWM0,PWM_DutyCycle_Default);
	PWMx_SetCTRL(PWM0,(PWM_ENABLE|PWM_KEEP));
	
	return 0;
}

void UninitPWM(void)
{
	PWMx_Disable(PWM0);
	MUX_PWM_TIME_OFF;
	MUTCTL_GPIO5_2_IO;
	DisableFanforIO();
}

int PWM_ChangeTheDutyCycle(uint32 _num)
{
	PWMx_Disable(PWM0);
	if(PWMx_IsBusy(PWM0))
	{
		printk("Init faild !2! \r\n");
		return -1;//the PWM module is busy
	}
	if(_num>PWM_DUTYCYCLE_MASK)
	{
		return -2;//the input number is out of range,the range is from 0 to 0xFFFFF;
	}
	PWMx_SetDutyCycle(PWM0,_num);
	PWMx_Enable(PWM0);
	if(PWMx_IsBusy(PWM0))
	{
		return 0;
	}else
	{
		return -3;//the operation was Failed
	}
}

//the Output voltage range is between from 0v to the pin OutPutVoltage.
//the Max voltage is 3300mA in this Borad.

int PWM_ChangeTheOutPutVoltage(uint16 _num)
{
	//uint32 theRct;
	uint32 _DutyCycle;
	PWMx_Disable(PWM0);
	udelay(1);
	if(PWMx_IsBusy(PWM0))
	{
		printk("PWM module is busy !3! \r\n");
		return -1;//the PWM module is busy
	}
	if(_num>CHIP_PPVoltage)
	{
		printk("Input Data is too large !4! \r\n");
		return -2;//the input number is out of range,the range is from 0 to 3300mA;
	}
	
	_DutyCycle = _num*PWM_Out_Priod/CHIP_PPVoltage;
	_DutyCycle &= PWM_DUTYCYCLE_MASK;
	
	PWMx_SetDutyCycle(PWM0,_DutyCycle);
	PWMx_Enable(PWM0);
	udelay(1);
	//writel(PWM_ENABLE|PWM_KEEP,IO_ADDRESS(PWMx_CTRL(PWM0)));
	//udelay(1);
	//theRct  = readl(IO_ADDRESS(PWMx_CTRL(PWM0)));
	//printk("Im therer!-----------------0x%x\r\n",theRct);
	/*
	if(PWMx_IsBusy(PWM0))
	{
		printk("PWM module is busy !3! \r\n");
		return 0;
	}else
	{
		printk("PWM module is Idle !3! \r\n");
		return -3;//the operation was Failed
	}*/
	return 0;
}


