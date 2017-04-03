#ifndef __USERHAL_H
#define __USERHAL_H

#include "../GlobalParameter.h"
#include <asm/system.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#define HAL_MCU_FREQUENCY	440000000//Hz	

//关于中断的定义
///////////////////////////////////////////////////////////////////////////////
#define IRQ_SOFT			(0)		// SOFT
#define IRQ_WDT				(1)		// WDT
#define IRQ_RTC				(2)		// RTC
#define IRQ_TM0_TM1			(3)		// TIMER0 TIMER1
#define IRQ_TM2_TM3			(4)		// TIMER2 TIMER3
#define IRQ_UART0_UART1		(5)		// UART0
#define IRQ_SSP0			(6)		// SSP0		(Hi3518C not contain SSP0)
#define IRQ_SSP1			(7)		// SSP1		(Hi3518C not contain SSP1)
#define IRQ_NANDC			(8)		// NANDC	(Hi3518C not contain NANDC)
#define IRQ_SIO				(9)		// SIO
#define IRQ_TEMPER_CAP		(10)	// TEMPER_CAP
#define IRQ_SFC				(11)	// SFC
#define IRQ_ETH				(12)	// ETH
#define IRQ_CIPHER			(13)	// CIPHER
#define IRQ_DMAC			(14)	// DMAC
#define IRQ_USB_EHCI		(15)	// USB_EHCI
#define IRQ_USB_OHCI		(16)	// USB_OHCI
#define IRQ_VPSS			(17)	// VPSS
#define IRQ_SDIO			(18)	// SDIO
#define IRQ_SAR_ADC			(19)	// ADC
#define IRQ_I2C				(20)	// I2C
#define IRQ_IVE				(21)	// IVE
#define IRQ_VICAP			(22)	// VICAP
#define IRQ_VOU				(23)	// VOU
#define IRQ_VEDU			(24)	// VEDU
#define IRQ_UART2			(25)	// UART2 (Hi3518C not contain UART2)
#define IRQ_JPGE			(26)	// JPGE
#define IRQ_TDE				(27)	// TED
#define IRQ_MDU_DDRT		(28)	// MDU/DDRT
#define IRQ_GPIO29			(29)	// GPIO0 GPIO1 GPIO2 GPIO11 
#define IRQ_GPIO30			(30)	// GPIO3 GPIO4 GPIO5 GPIO10
#define IRQ_GPIO31			(31)	// GPIO6 GPIO7 GPIO8 GPIO9 	(Hi3518C not contain GPIO 8 9)




typedef struct{
	uint32 aIRQNUM;
	irqreturn_t (*afHandler)(int , void *);
	uint32 aMode;
	uint8 *aName;
}mIRQ_Param;


//SysCtrl
#define HAL_BASE_SYSCTRL	0x20050000
#define HAL_OFST_SCCTRL		0x00

//MUXCTRL
#define HAL_BASE_MUXCTRL	0x200F0000
#define HAL_OFST_MUXCTRL_reg0			(0<<2)
#define HAL_OFST_MUXCTRL_reg1			(1<<2)
#define HAL_OFST_MUXCTRL_reg2			(2<<2)
#define HAL_OFST_MUXCTRL_reg3			(3<<2)
#define HAL_OFST_MUXCTRL_reg4			(4<<2)
#define HAL_OFST_MUXCTRL_reg5			(5<<2)
#define HAL_OFST_MUXCTRL_reg6			(6<<2)
#define HAL_OFST_MUXCTRL_reg7			(7<<2)
#define HAL_OFST_MUXCTRL_reg8			(8<<2)
#define HAL_OFST_MUXCTRL_reg9			(9<<2)
#define HAL_OFST_MUXCTRL_reg10		(10<<2)
#define HAL_OFST_MUXCTRL_reg11		(11<<2)
#define HAL_OFST_MUXCTRL_reg12		(12<<2)
#define HAL_OFST_MUXCTRL_reg13		(13<<2)
#define HAL_OFST_MUXCTRL_reg14		(14<<2)
#define HAL_OFST_MUXCTRL_reg15		(15<<2)
#define HAL_OFST_MUXCTRL_reg16		(16<<2)
#define HAL_OFST_MUXCTRL_reg17		(17<<2)
#define HAL_OFST_MUXCTRL_reg18		(18<<2)
#define HAL_OFST_MUXCTRL_reg19		(19<<2)
#define HAL_OFST_MUXCTRL_reg20		(20<<2)
#define HAL_OFST_MUXCTRL_reg21		(21<<2)
#define HAL_OFST_MUXCTRL_reg22		(22<<2)
#define HAL_OFST_MUXCTRL_reg23		(23<<2)
#define HAL_OFST_MUXCTRL_reg24		(24<<2)
#define HAL_OFST_MUXCTRL_reg25		(25<<2)
#define HAL_OFST_MUXCTRL_reg26		(26<<2)
#define HAL_OFST_MUXCTRL_reg27		(27<<2)
#define HAL_OFST_MUXCTRL_reg28		(28<<2)
#define HAL_OFST_MUXCTRL_reg29		(29<<2)
#define HAL_OFST_MUXCTRL_reg30		(30<<2)
#define HAL_OFST_MUXCTRL_reg31		(31<<2)
#define HAL_OFST_MUXCTRL_reg32		(32<<2)
#define HAL_OFST_MUXCTRL_reg33		(33<<2)
#define HAL_OFST_MUXCTRL_reg34		(34<<2)
#define HAL_OFST_MUXCTRL_reg35		(35<<2)
#define HAL_OFST_MUXCTRL_reg36		(36<<2)
#define HAL_OFST_MUXCTRL_reg37		(37<<2)
#define HAL_OFST_MUXCTRL_reg38		(38<<2)
#define HAL_OFST_MUXCTRL_reg39		(39<<2)
#define HAL_OFST_MUXCTRL_reg40		(40<<2)
#define HAL_OFST_MUXCTRL_reg41		(41<<2)
#define HAL_OFST_MUXCTRL_reg42		(42<<2)
#define HAL_OFST_MUXCTRL_reg43		(43<<2)
#define HAL_OFST_MUXCTRL_reg44		(44<<2)
#define HAL_OFST_MUXCTRL_reg45		(45<<2)
#define HAL_OFST_MUXCTRL_reg46		(46<<2)
#define HAL_OFST_MUXCTRL_reg47		(47<<2)
#define HAL_OFST_MUXCTRL_reg48		(48<<2)
#define HAL_OFST_MUXCTRL_reg49		(49<<2)
#define HAL_OFST_MUXCTRL_reg50		(50<<2)
#define HAL_OFST_MUXCTRL_reg51		(51<<2)
#define HAL_OFST_MUXCTRL_reg52		(52<<2)
#define HAL_OFST_MUXCTRL_reg53		(53<<2)
#define HAL_OFST_MUXCTRL_reg54		(54<<2)
#define HAL_OFST_MUXCTRL_reg55		(55<<2)
#define HAL_OFST_MUXCTRL_reg56		(56<<2)
#define HAL_OFST_MUXCTRL_reg57		(57<<2)
#define HAL_OFST_MUXCTRL_reg58		(58<<2)
#define HAL_OFST_MUXCTRL_reg59		(59<<2)
#define HAL_OFST_MUXCTRL_reg60		(60<<2)
#define HAL_OFST_MUXCTRL_reg61		(61<<2)
#define HAL_OFST_MUXCTRL_reg62		(62<<2)
#define HAL_OFST_MUXCTRL_reg63		(63<<2)
#define HAL_OFST_MUXCTRL_reg64		(64<<2)
#define HAL_OFST_MUXCTRL_reg65		(65<<2)
#define HAL_OFST_MUXCTRL_reg66		(66<<2)
#define HAL_OFST_MUXCTRL_reg67		(67<<2)
#define HAL_OFST_MUXCTRL_reg68		(68<<2)
#define HAL_OFST_MUXCTRL_reg69		(69<<2)
#define HAL_OFST_MUXCTRL_reg70		(70<<2)
#define HAL_OFST_MUXCTRL_reg71		(71<<2)
#define HAL_OFST_MUXCTRL_reg72		(72<<2)
#define HAL_OFST_MUXCTRL_reg73		(73<<2)
#define HAL_OFST_MUXCTRL_reg74		(74<<2)
#define HAL_OFST_MUXCTRL_reg75		(75<<2)
#define HAL_OFST_MUXCTRL_reg76		(76<<2)
#define HAL_OFST_MUXCTRL_reg77		(77<<2)
#define HAL_OFST_MUXCTRL_reg78		(78<<2)
#define HAL_OFST_MUXCTRL_reg79		(79<<2)
#define HAL_OFST_MUXCTRL_reg80		(80<<2)
#define HAL_OFST_MUXCTRL_reg81		(81<<2)
#define HAL_OFST_MUXCTRL_reg82		(82<<2)
#define HAL_OFST_MUXCTRL_reg83		(83<<2)
#define HAL_OFST_MUXCTRL_reg84		(84<<2)
#define HAL_OFST_MUXCTRL_reg85		(85<<2)
#define HAL_OFST_MUXCTRL_reg86		(86<<2)
#define HAL_OFST_MUXCTRL_reg87		(87<<2)
#define HAL_OFST_MUXCTRL_reg88		(88<<2)
#define HAL_OFST_MUXCTRL_reg89		(89<<2)
#define HAL_OFST_MUXCTRL_reg91		(90<<2)
#define HAL_OFST_MUXCTRL_reg92		(91<<2)
#define HAL_OFST_MUXCTRL_reg93		(92<<2)
#define HAL_OFST_MUXCTRL_reg94		(93<<2)



//CRG
#define HAL_CRG_BASE				0x20030000
#define HAL_OFST_CRG0_APLL0			0x00
#define HAL_OFST_CRG1_APLL1			0x04
#define HAL_OFST_CRG2_VPLL00		0x08
#define HAL_OFST_CRG3_VPLL01		0x0C
#define HAL_OFST_CRG4_BPLL0			0x10
#define HAL_OFST_CRG5_BPLL1			0x14
#define HAL_OFST_CRG8_EPLL0			0x20
#define HAL_OFST_CRG9_EPLL1			0x24
#define HAL_OFST_CRG10_RstARM		0x28
#define HAL_OFST_CRG11_RstVICAP		0x2C
#define HAL_OFST_CRG12_TSensor		0x30
#define HAL_OFST_CRG13_RsTVOU		0x34
#define HAL_OFST_CRG14_RsTPWM		0x38
#define HAL_OFST_CRG16_RsTVEDU		0x40
#define HAL_OFST_CRG18_RsTVPSS		0x48
#define HAL_OFST_CRG22_RsTTDE		0x58
#define HAL_OFST_CRG24_RsTJPGE		0x60
#define HAL_OFST_CRG26_RsTMDU		0x68
#define HAL_OFST_CRG27_RsTVAPU		0x6C
#define HAL_OFST_CRG31_RsCIPHER		0x7C
#define HAL_OFST_CRG32_RsTADC		0x80
#define HAL_OFST_CRG34_RsTMCLK		0x34
#define HAL_OFST_CRG35_RsTSIO		0x8C
#define HAL_OFST_CRG46_RsTUSB		0xB8
#define HAL_OFST_CRG48_RsTSFC		0xC0
#define HAL_OFST_CRG49_RsTSDIO		0xC4
#define HAL_OFST_CRG51_RsTETH		0xCC
#define HAL_OFST_CRG52_RsTNandC		0xD0
#define HAL_OFST_CRG54_RsTDDRTest	0xD8
#define HAL_OFST_CRG56_RsTDMA		0xE0
#define HAL_OFST_CRG57_RsTCRGOt		0xE4
#define HAL_OFST_CRG58_RsTCrgSta	0xE8
#define HAL_OFST_CRG61_RsTSMSta1	0xF4
#define HAL_OFST_CRG62_RsTSMSta2	0xF8

#define HAL_CRGx_Addr(_ofst)	(_ofst+HAL_CRG_BASE)


//GPIO
#define HAL_BASE_GPIO0			0x20140000
#define HAL_BASE_GPIO1			0x20150000
#define HAL_BASE_GPIO2			0x20160000
#define HAL_BASE_GPIO3			0x20170000
#define HAL_BASE_GPIO4			0x20180000
#define HAL_BASE_GPIO5			0x20190000
#define HAL_BASE_GPIO6			0x201A0000
#define HAL_BASE_GPIO7			0x201B0000
#define HAL_BASE_GPIO8			0x201C0000
#define HAL_BASE_GPIO9			0x201D0000
#define HAL_BASE_GPIO10			0x201E0000
#define HAL_BASE_GPIO11			0x201F0000

#define HAL_OFST_GPIO_DATA			0x00
#define HAL_OFST_GPIO_MDATA(_x)		((_x)<<2)			//Muilt
#define HAL_OFST_GPIO_SDATA(_x)		((0x01<<(_x))<<2)	//Sigal
#define HAL_OFST_GPIO_DIR			0x400
#define HAL_OFST_GPIO_IS			0x404
#define HAL_OFST_GPIO_IBE			0x408
#define HAL_OFST_GPIO_IEV			0x40C
#define HAL_OFST_GPIO_IE			0x410
#define HAL_OFST_GPIO_RIS			0x414
#define HAL_OFST_GPIO_MIS			0x418
#define HAL_OFST_GPIO_IC			0x41C

//Timer
#define HAL_BASE_TIMER_01	0x20000000
#define HAL_BASE_TIMER_23	0x20010000

#define HAL_OFST_TIME_LOAD		0x00
#define HAL_OFST_TIME_VALUE		0x04
#define HAL_OFST_TIME_CONTROL	0x08
#define HAL_OFST_TIME_INTCLR	0x0C
#define HAL_OFST_TIME_RIS		0x10
#define HAL_OFST_TIME_MIS		0x14
#define HAL_OFST_TIME_BGLOAD	0x18

#define HAL_OFST_TIME02			0x00
#define HAL_OFST_TIME13			0x20

//ADC
#define HAL_BASE_SRCADC				0x200B0000

#define HAL_OFST_ADC_STATUS			0x0
#define HAL_OFST_ADC_CTRL			0x4
#define HAL_OFST_ADC_POWERDOWN		0x8
#define HAL_OFST_ADC_INT_STATUS		0xC
#define HAL_OFST_ADC_INT_MASK		0x10
#define HAL_OFST_ADC_INT_CLR		0x14
#define HAL_OFST_ADC_RAW			0x18
#define HAL_OFST_ADC_RESULT			0x1C

#define HAL_ADC_REGAddr(_ofst)		((_ofst)+HAL_BASE_SRCADC)

//PWM
#define HAL_BASE_PWM				0x20130000
#define HAL_OFST_PWMx(_g)			((_g)*0x20)
#define HAL_OFST_PWMx_CFG0(_g)	 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x00)
#define HAL_OFST_PWMx_CFG1(_g)	 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x04)
#define HAL_OFST_PWMx_CFG2(_g)	 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x08)
#define HAL_OFST_PWMx_CTRL(_g)	 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x0C)
#define HAL_OFST_PWMx_STATE0(_g) 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x10)
#define HAL_OFST_PWMx_STATE1(_g) 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x14)
#define HAL_OFST_PWMx_STATE2(_g) 	(HAL_BASE_PWM+HAL_OFST_PWMx(_g)+0x18)



//MUTCTRL
#define HAL_OFST_MUXCTRL_regx(_x)		((_x)<<2)
#define HAL_MUXCTRL_regx(_x)			(HAL_OFST_MUXCTRL_regx(_x)+HAL_BASE_MUXCTRL)

//GPIO
#define HAL_BASE_GPIOx(_g)				(0x20140000+((_g)<<16))
#define HAL_GPIOx_DATA( _g ) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_DATA)			//|
#define HAL_GPIOx_MDATA( _g,_b) 		(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_MDATA(_b))	//|
#define HAL_GPIOx_SDATA( _g,_b) 		(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_SDATA(_b))	//|
#define HAL_GPIOx_DIR(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_DIR)
#define HAL_GPIOx_IS(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IS)
#define HAL_GPIOx_IBE(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IBE)
#define HAL_GPIOx_IEV(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IEV)
#define HAL_GPIOx_IE(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IE)
#define HAL_GPIOx_RIS(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_RIS)
#define HAL_GPIOx_MIS(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_MIS)
#define HAL_GPIOx_IC(_g) 				(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IC)


//#define HAL_GPIO_SBit(_x)				((0x01<<(_x))<<2)// Its wrong macro
#define HAL_GPIO_Bit(_x)				(0x01<<(_x))


typedef struct{
	uint8 	aGroup;
	uint8 	aBit;
	uint8 	aMux;
	uint32	aMuxVal;
}mGPIOPinMsg;


//TIMER
#define HAL_TIMER_ADDR(_g,_r)			((((_g)==0)||((_g)==1))?(HAL_BASE_TIMER_01+(_r)):((((_g)==2)||((_g)==3))?(HAL_BASE_TIMER_23+(_r)):0x00))

#define HAL_TIMER_OFST(_g,_r)			((((_g)==0)||((_g)==2))?(HAL_OFST_TIME02+(_r)):((((_g)==1)||((_g)==3))?(HAL_OFST_TIME13+(_r)):0x00))

//sysctl
#define HAL_SCCTRL_ADDR		(HAL_BASE_SYSCTRL+HAL_OFST_SCCTRL)
#define DC_SysTemMode_NORMAL 	0x00000100
#define DC_SysTemMode_SLOW 		0x00000010
#define DC_SysTemMode_DOZE 		0x00000001


#define DF_SetSysMode_NORMAL	{\
								uint32 _v;\
								_v = HAL_readl(HAL_SCCTRL_ADDR);\
								_v &= 0xFFFFFFF8;\
								_v |= DC_SysTemMode_NORMAL;\
								HAL_writel(_v,(HAL_SCCTRL_ADDR));}

// UserFunction
#define HAL_readl(_addr)		readl(IO_ADDRESS(_addr))	
#define HAL_writel(_v,_addr)	writel(_v, IO_ADDRESS(_addr))


#define DF_Set_GPIO_MUXx(_v,_Mux)	HAL_writel(_v,HAL_MUXCTRL_regx(_Mux))

#define DF_Set_GPIO_ADDx(_g,_b,_addr)	{\
										uint32 _v;\
										_v= HAL_readl(_addr);\
										_v|=HAL_GPIO_Bit(_b);\
										HAL_writel(_v,(_addr));}

#define DF_Clr_GPIO_ADDx(_g,_b,_addr)	{\
										uint32 _v;\
										_v= HAL_readl(_addr);\
										_v&=~HAL_GPIO_Bit(_b);\
										HAL_writel(_v,(_addr));}

#define DF_Switch_GPIO_ADDx(_g,_b,_addr)	{\
										uint32 _v;\
										_v= HAL_readl(_addr);\
										_v^=HAL_GPIO_Bit(_b);\
										HAL_writel(_v,(_addr));}

//
#define DF_Get_GPIO_ADDx(_g,_b,_addr)	((HAL_readl(_addr)&HAL_GPIO_Bit(_b))?1:0)

//
#define DF_Set_GPIO_DIRx(_g,_b)		DF_Set_GPIO_ADDx(_g,_b,HAL_GPIOx_DIR(_g))
#define DF_Clr_GPIO_DIRx(_g,_b)		DF_Clr_GPIO_ADDx(_g,_b,HAL_GPIOx_DIR(_g))
#define DF_Set_GPIO_Outx(_g,_b)		DF_Set_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
#define DF_Clr_GPIO_Outx(_g,_b)		DF_Clr_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
#define DF_Switch_GPIO_Outx(_g,_b)	DF_Switch_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
#define DF_Get_GPIO_INx(_g,_b)		DF_Get_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))

#endif