#ifndef __ADC_H
#define __ADC_H

#include "GlobalParameter.h"
//关于中断的定义
///////////////////////////////////////////////////////////////////////////////
#define IRQ_SOFT         (0)
#define IRQ_WDT          (1)
#define IRQ_RTC          (2)
#define IRQ_TM0_TM1      (3)
#define IRQ_TM2_TM3      (4)
#define IRQ_UART0_UART1  (5)

#define IRQ_SIO          (9)
#define IRQ_TEMPER_CAP   (10)
#define IRQ_SFC          (11)
#define IRQ_ETH          (12)
#define IRQ_CIPHER       (13)
#define IRQ_DMAC         (14)
#define IRQ_USB_EHCI     (15)
#define IRQ_USB_OHCI     (16)
#define IRQ_VPSS         (17)
#define IRQ_SDIO         (18)
#define IRQ_SAR_ADC      (19)
#define IRQ_I2C          (20)
#define IRQ_IVE          (21)
#define IRQ_VICAP        (22)
#define IRQ_VOU          (23)
#define IRQ_VEDU         (24)

#define IRQ_JPGE         (26)
#define IRQ_TDE          (27)
#define IRQ_MDU_DDRT     (28)
#define IRQ_GPIO29       (29)
#define IRQ_GPIO30       (30)
#define IRQ_GPIO31       (31)




//寄存器相关定义
///////////////////////////////////////////////////////////////////////////////




// SAR_ADC_TIME
#define MUXTime_BASC_ADD			0x20030000
#define MUXTIME_ADC_CTRL			0x80
#define TIME_ADC_CTL				(MUXTime_BASC_ADD+MUXTIME_ADC_CTRL)

#define MUX_ADC_RESAT				0x01
#define MUX_ADC_TIME				0x02

//ADC
#define BASC_SAR_ADC_ADD			(0x200B0000)
#define OFFSET_ADC_STATUS			(0x0)
#define OFFSET_ADC_CTRL				(0x4)
#define OFFSET_ADC_POWERDOWN		(0x8)
#define OFFSET_ADC_INT_STATUS		(0xC)
#define OFFSET_ADC_INT_MASK			(0x10)
#define OFFSET_ADC_INT_CLR			(0x14)
#define OFFSET_ADC_RAW				(0x18)
#define OFFSET_ADC_RESULT			(0x1C)
//Regist address --> RD
#define RD_ADC_STATUS				(BASC_SAR_ADC_ADD+OFFSET_ADC_STATUS)
#define ADC_CONVERT_STATUS			(0x00000001)
//
#define RD_ADC_CTRL					(BASC_SAR_ADC_ADD+OFFSET_ADC_CTRL)
#define ADC_CHANNEL_0				(0x00000000)
#define ADC_CHANNEL_1				(0x00010000)
#define ADC_START_CONVERT			(0x00000001)
//
#define RD_ADC_POWERDOWN			(BASC_SAR_ADC_ADD+OFFSET_ADC_POWERDOWN)
#define ADC_POWERDOWN				(0x00000001)
#define ADC_POWEUP					(0x00000000)

#define RD_ADC_INT_STATUS			(BASC_SAR_ADC_ADD+OFFSET_ADC_INT_STATUS)
#define ADC_CONVERT_INT_STATUS		(0x00000001)

#define RD_ADC_INT_MASK				(BASC_SAR_ADC_ADD+OFFSET_ADC_INT_MASK)
#define ADC_INT_ENABLE				(0x00000000)
#define ADC_INT_DISABLE				(0x00000001)

#define RD_ADC_INT_CLR				(BASC_SAR_ADC_ADD+OFFSET_ADC_INT_CLR)
#define ADC_INT_CLR					(0x00000001)

#define RD_ADC_INT_RAW				(BASC_SAR_ADC_ADD+OFFSET_ADC_RAW)
#define ADC_INI_RAM					(0x00000001)

#define RD_ADC_RESULT				(BASC_SAR_ADC_ADD+OFFSET_ADC_RESULT)



//
#define hi_readl(x)     readl(IO_ADDRESS(x))
#define hi_writel(v,x)	writel(v, IO_ADDRESS(x))






//function
//V --> value  
//SET -->set
#define SET_EN_ADC_MODULE		\
{hi_writel((hi_readl(TIME_ADC_CTL)|MUX_ADC_TIME),TIME_ADC_CTL);}
#define SET_DIS_ADC_MODULE		\
{hi_writel((hi_readl(TIME_ADC_CTL)&(~MUX_ADC_TIME)),TIME_ADC_CTL);}

#define SET_RESET_ADC_MODULE	\
{hi_writel((hi_readl(TIME_ADC_CTL)|MUX_ADC_RESAT),TIME_ADC_CTL);}
#define SET_DISRESET_ADC_MODULE	\
{hi_writel((hi_readl(TIME_ADC_CTL)&(~MUX_ADC_RESAT)),TIME_ADC_CTL);}

#define V_ADC_CONVERT_STATUS   	(hi_readl(RD_ADC_STATUS)&ADC_CONVERT_STATUS)
#define CLR_ADC_CONVER_STATUS	{hi_writel(ADC_CONVERT_STATUS,RD_ADC_STATUS);}

#define SET_ADC_CHANNEL_0		{hi_writel(ADC_CHANNEL_0,RD_ADC_CTRL);}
#define SET_ADC_CHANNEL_1		{hi_writel(ADC_CHANNEL_1,RD_ADC_CTRL);}
#define SET_ADC_START			\
{hi_writel((hi_readl(RD_ADC_CTRL)|ADC_START_CONVERT),RD_ADC_CTRL);}

#define SET_ADC_POWER_DOWN		{hi_writel(ADC_POWERDOWN,RD_ADC_POWERDOWN);}
#define SET_ADC_POWER_UP		{hi_writel(ADC_POWEUP,RD_ADC_POWERDOWN);}
//CVT -->convert
#define V_ADC_CVT_INT_STATUS 	\
(hi_readl(RD_ADC_INT_STATUS)&ADC_CONVERT_INT_STATUS)

#define SET_ADC_INT_ENABLE		{hi_writel(ADC_INT_ENABLE,RD_ADC_INT_MASK);} 
#define SET_ADC_INT_DISABLE		{hi_writel(ADC_INT_DISABLE,RD_ADC_INT_MASK);} 

#define SET_ADC_CLR_INT			{hi_writel(ADC_INT_CLR,RD_ADC_INT_CLR);}

#define V_ADC_INI_RAW			(hi_readl(RD_ADC_INT_RAW)&ADC_INI_RAM)

#define V_ADC_VALUE				(hi_readl(RD_ADC_RESULT))




extern int _InitADC(void);
extern void UninitADC(void);
//extern uint16 ConvertChannelx(uint16 _Channel);
extern int ConvertChannelx(uint16 _Channel);
#endif