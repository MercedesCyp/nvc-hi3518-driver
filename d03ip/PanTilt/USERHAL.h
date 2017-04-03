#ifndef __USERHAL_H
#define __USERHAL_H

#include <asm/system.h>
#include <asm/io.h>

#define HAL_MCU_FREQUENCY	440000000//Hz	

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

//SysCtrl
#define HAL_BASE_SYSCTRL	0x20050000
#define HAL_OFST_SCCTRL					0x00

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

//GPIO
#define HAL_BASE_GPIO0		0x20140000
#define HAL_BASE_GPIO1		0x20150000
#define HAL_BASE_GPIO2		0x20160000
#define HAL_BASE_GPIO3		0x20170000
#define HAL_BASE_GPIO4		0x20180000
#define HAL_BASE_GPIO5		0x20190000
#define HAL_BASE_GPIO6		0x201A0000
#define HAL_BASE_GPIO7		0x201B0000
#define HAL_BASE_GPIO8		0x201C0000
#define HAL_BASE_GPIO9		0x201D0000
#define HAL_BASE_GPIO10		0x201E0000
#define HAL_BASE_GPIO11		0x201F0000

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

//TIMER
#define HAL_TIMER_ADDR(_g,_r)			((((_g)==0)||((_g)==1))?(HAL_BASE_TIMER_01+(_r)):((((_g)==2)||((_g)==3))?(HAL_BASE_TIMER_23+(_r)):0x00))

#define HAL_TIMER_OFST(_g,_r)			((((_g)==0)||((_g)==2))?(HAL_OFST_TIME02+(_r)):((((_g)==1)||((_g)==3))?(HAL_OFST_TIME13+(_r)):0x00))

//sysctl
#define HAL_SCCTRL_ADDR		(HAL_BASE_SYSCTRL+HAL_OFST_SCCTRL)


#define HAL_readl(_addr)		readl(IO_ADDRESS(_addr))	
#define HAL_writel(_v,_addr)	writel(_v, IO_ADDRESS(_addr))

#endif