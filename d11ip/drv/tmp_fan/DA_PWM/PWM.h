#ifndef __PWM_H
#define __PWM_H

#include "GlobalParameter.h"

///////////////////////////////////////////////////////////////////////////////
// 系统时钟寄存器
#define MUXTime_BASC_ADD	0x20030000
#define MUXTIME_PWM_CTRL	0x38

#define MT_RESAT			0x01
#define MT_TIME_ON			0x02


// 复用寄存器
#define MUXCTL_BASC_ADD		0x200F0000
#define MUXCTL_PWM0_OFFSET	0x0BC	
#define MUXCTL_PWM1_OFFSET	0x0C0	

///////////////////////////////////////////////////////////////////////////////


#define REGISTER_PWM_BASIC_ADDRESS 	(0x20130000)
#define PWMx_Offset(x) 		((x)<<5)
#define PWM_CFG0_offset		(0x00)
#define PWM_CFG1_offset		(0x04)
#define PWM_CFG2_offset		(0x08)
#define PWM_CTRL_offset		(0x0C)
#define PWM_STATE0_offset	(0x10)
#define PWM_STATE1_offset	(0x14)
#define PWM_STATE2_offset	(0x18)

#define PWM0	(0x0)
#define PWM1	(0x2)
#define PWM2	(0x4)

/*macro
	The regist address of PWM mode
	
*/
///////////////////////////////////////////////////////////////////////////////
// PWM define
#define PWM0_CFG0					(REGISTER_PWM_BASIC_ADDRESS+0x00)
#define PWM0_CFG1					(REGISTER_PWM_BASIC_ADDRESS+0x04)
#define PWM0_CFG2					(REGISTER_PWM_BASIC_ADDRESS+0x08)
#define PWM0_CTRL					(REGISTER_PWM_BASIC_ADDRESS+0x0C)
#define PWM0_STATE0					(REGISTER_PWM_BASIC_ADDRESS+0x10)
#define PWM0_STATE1					(REGISTER_PWM_BASIC_ADDRESS+0x14)
#define PWM0_STATE2					(REGISTER_PWM_BASIC_ADDRESS+0x18)

#define PWM1_CFG0					(REGISTER_PWM_BASIC_ADDRESS+0x20)
#define PWM1_CFG1					(REGISTER_PWM_BASIC_ADDRESS+0x24)
#define PWM1_CFG2					(REGISTER_PWM_BASIC_ADDRESS+0x28)
#define PWM1_CTRL					(REGISTER_PWM_BASIC_ADDRESS+0x2C)
#define PWM1_STATE0					(REGISTER_PWM_BASIC_ADDRESS+0x30)
#define PWM1_STATE1					(REGISTER_PWM_BASIC_ADDRESS+0x34)
#define PWM1_STATE2					(REGISTER_PWM_BASIC_ADDRESS+0x38)

#define PWM2_CFG0					(REGISTER_PWM_BASIC_ADDRESS+0x40)
#define PWM2_CFG1					(REGISTER_PWM_BASIC_ADDRESS+0x44)
#define PWM2_CFG2					(REGISTER_PWM_BASIC_ADDRESS+0x48)
#define PWM2_CTRL					(REGISTER_PWM_BASIC_ADDRESS+0x4C)
#define PWM2_STATE0					(REGISTER_PWM_BASIC_ADDRESS+0x50)
#define PWM2_STATE1					(REGISTER_PWM_BASIC_ADDRESS+0x54)
#define PWM2_STATE2					(REGISTER_PWM_BASIC_ADDRESS+0x58)



#define PWMx_CFG0(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_CFG0_offset)
#define PWMx_CFG1(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_CFG1_offset)
#define PWMx_CFG2(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_CFG2_offset)
#define PWMx_CTRL(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_CTRL_offset)
#define PWMx_STATE0(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_STATE0_offset)
#define PWMx_STATE1(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_STATE1_offset)
#define PWMx_STATE2(x)					(REGISTER_PWM_BASIC_ADDRESS+PWMx_Offset(x)+PWM_STATE2_offset)


// GPIO0 define
#define GPIO0_BASE       0x20140000
#define GPIO0_0          (GPIO0_BASE + (0x0<<16))
#define GPIO0_1          (GPIO0_BASE + (0x1<<16))
#define GPIO0_2          (GPIO0_BASE + (0x2<<16))
#define GPIO0_3          (GPIO0_BASE + (0x3<<16))
#define GPIO0_4          (GPIO0_BASE + (0x4<<16))
#define GPIO0_5          (GPIO0_BASE + (0x5<<16))
#define GPIO0_6          (GPIO0_BASE + (0x6<<16))
#define GPIO0_7          (GPIO0_BASE + (0x7<<16))
#define CPIOO_DATA		 (0x3FC)
#define GPIO0_DIR        (0x400)
#define GPIO0_IS         (0x404)
#define GPIO0_IBE        (0x408)
#define GPIO0_IEV        (0x40C)
#define GPIO0_IE         (0x410)
#define GPIO0_RIS        (0x414)
#define GPIO0_MIS        (0x418)
#define GPIO0_IC         (0x41C)
///////////////////////////////////////////////////////////////////////////////



#define PWM_ENABLE				0x00000001
#define PWM_INV					0x00000002
#define PWM_KEEP				0x00000004

#define PWM_S_BUSY				0x00000400
#define PWM_S_KEEP				0x00000800


#define	PWM_S_PriodMask			0x000003FF
#define	PWM_S_CntMask			0x003FF000

#define PWM_PRIOD_MASK			0x000FFFFF
#define PWM_DUTYCYCLE_MASK		0x000FFFFF

///////////////////////////////////////////////////////////////////////////////





#define hi_readl(x)     readl(IO_ADDRESS(x))
#define hi_writel(v,x)	writel(v, IO_ADDRESS(x))
///////////////////////////////////////////////////////////////////////////////
#define MUXTime_BASC_ADD	0x20030000
#define MUXTIME_PWM_CTRL	0x38

#define MT_RESAT			0x01
#define MT_TIME_ON			0x02
///////////////////////////////////////////////////////////////////////////////

#define MUX_PWM_TIME_ON		\
{writel(readl(IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL))|MT_TIME_ON,\
IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL));}
#define MUX_PWM_TIME_OFF	\
{writel(readl(IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL))&(~MT_TIME_ON),\
IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL));}
	
#define MUX_PWM_EnRESET		\
{writel(readl(IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL))|MT_RESAT,\
IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL));}
#define MUX_PWM_DisRESET	\
{writel(readl(IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL))&(~MT_RESAT),\
IO_ADDRESS(MUXTime_BASC_ADD+MUXTIME_PWM_CTRL));}


///////////////////////////////////////////////////////////////////////////////
#define MUTCTL_GPIO5_2_PWM0			\
{writel(0x00000001,IO_ADDRESS(MUXCTL_BASC_ADD+MUXCTL_PWM0_OFFSET));}

#define MUTCTL_GPIO5_2_IO			\
{writel(0x00000000,IO_ADDRESS(MUXCTL_BASC_ADD+MUXCTL_PWM0_OFFSET));}

#define MUTCTL_GPIO5_3_PWM1			\
{writel(0x00000001,IO_ADDRESS(MUXCTL_BASC_ADD+MUXCTL_PWM1_OFFSET));}

#define MUTCTL_GPIO5_3_IO			\
{writel(0x00000000,IO_ADDRESS(MUXCTL_BASC_ADD+MUXCTL_PWM1_OFFSET));}
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#define PWMx_Enable(x) 			\
{writel((readl(IO_ADDRESS(PWMx_CTRL(x)))|(PWM_ENABLE)),IO_ADDRESS(PWMx_CTRL(x)));}

#define PWMx_Disable(x) 		\
{writel((readl(IO_ADDRESS(PWMx_CTRL(x)))&(~PWM_ENABLE)),IO_ADDRESS(PWMx_CTRL(x)));}

#define PWMx_ContinueSend(x)	\
{writel(readl(IO_ADDRESS(PWMx_CTRL(x)))|(PWM_KEEP),IO_ADDRESS(PWMx_CTRL(x));}

#define PWMx_LimitSend(x)		\
{writel(readl(IO_ADDRESS(PWMx_CTRL(x)))&(~PWM_KEEP),IO_ADDRESS(PWMx_CTRL(x));}

#define PWMx_NormalOutput(x)	\
{writel(readl(IO_ADDRESS(PWMx_CTRL(x))))|(PWM_INV),IO_ADDRESS(PWMx_CTRL(x)));}


#define PWMx_AbnormalOutput(x)	\
{writel(readl(IO_ADDRESS(PWMx_CTRL(x)))&(~PWM_INV),IO_ADDRESS(PWMx_CTRL(x));}
///////////////////////////////////////////////////////////////////////////////


#define PWMx_SetPriod(x,v)		\
{writel((v),IO_ADDRESS(PWMx_CFG0(x)));}

#define PWMx_SetDutyCycle(x,v)	\
{writel((v),IO_ADDRESS(PWMx_CFG1(x)));}

#define PWMx_SetSendNum(x,v)	\
{writel((v),IO_ADDRESS(PWMx_CFG2(x)));}	

#define PWMx_SetCTRL(x,v)		\
{writel((v),IO_ADDRESS(PWMx_CTRL(x)));}
///////////////////////////////////////////////////////////////////////////////
#define PWMx_EnState(x)		(hi_readl(PWMx_CTRL(x))&PWM_ENABLE)
#define PWMx_IsBusy(x)		(hi_readl(PWMx_STATE2(x))&PWM_S_BUSY)
///////////////////////////////////////////////////////////////////////////////

extern int InitPWM(void);
extern void UninitPWM(void);
int PWM_ChangeTheDutyCycle(uint32 _num);
int PWM_ChangeTheOutPutVoltage(uint16 _num);

#endif