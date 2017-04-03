#ifndef __HI3507_HAL
#define __HI3507_HAL

#include "../../GlobalParameter.h"
#include <asm/system.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#define HAL_MCU_FREQUENCY	440000000	//Hz

// UserFunction
// #define HAL_regRW(reg)          *((volatile unsigned int *)(reg))
#define HAL_readl(_addr)		readl(IO_ADDRESS(_addr))
#define HAL_writel(_v,_addr)	writel(_v, IO_ADDRESS(_addr))

//关于中断的定义
///////////////////////////////////////////////////////////////////////////////
#define IRQ_WDT				(0)	// WDT
#define IRQ_SOFT			(1)	// SOFT
#define IRQ_COMMRX          (2)	// COMMRX
#define IRQ_COMMTX          (3)	// COMMTX
#define IRQ_TM0             (4)	// Dual-Timer0
#define IRQ_TM1             (5)	// Dual-Timer1
#define IRQ_GPIO0           (6)	// GPIO0
#define IRQ_GPIO1           (7)	// GPIO1
#define IRQ_GPIOetc         (8)	// GPIO2,GPIO3,GPIO4,GPIO5,GPIO6,GPIO7,
#define IRQ_IR              (9)	// IR
#define IRQ_RTC             (10)	// RTC
#define IRQ_SSP             (11)	// SSP
#define IRQ_UART0           (12)	// UART0
#define IRQ_UART1           (13)	// UART1
#define IRQ_UART2           (14)	// UART2
#define IRQ_ETH             (15)	// ETH
#define IRQ_VOU             (16)	// VOU
#define IRQ_DMAC            (17)	// DMAC
#define IRQ_SIO1            (18)	// SIO1
#define IRQ_I2C             (19)	// I2C
#define IRQ_USB1_1          (20)	// USB 1.1 Host interrupt
#define IRQ_CIPHER          (21)	// CIPHER
#define IRQ_INTRN           (22)	// 外部管脚 INTRN （该管脚为复用管脚） 中断
#define IRQ_USB2_0          (23)	// USB 2.0 Host interrupt
#define IRQ_MMC             (24)	// MMC
#define IRQ_VIU             (25)	// VIU
#define IRQ_DSU             (26)	// DSU
#define IRQ_SIO0            (27)	// SIO0
#define IRQ_VEDU            (28)	// VEDU
#define IRQ_REMAIN          (29)	// ***********保留
#define IRQ_PCI             (30)	// PCI
#define IRQ_TDE             (31)	// TDE

typedef struct {
	uint32 aIRQNUM;
	irqreturn_t(*afHandler)(int, void *);
	uint32 aMode;
	uint8 *aName;
} mIRQ_Param;

// Sys contrl register
#define HAL_BASE_SYSCTRL	0x101E0000
#define HAL_OFST_CTRL       0x00
#define HAL_OFST_SYSSTAT    0x04
#define HAL_OFST_ITMCTRL    0x08
#define HAL_OFST_IMSTAT     0x0C
#define HAL_OFST_XTALCTRL   0x10
#define HAL_OFST_PLLCTRL    0x14
#define HAL_OFST_PLLFCTRL   0x18
#define HAL_OFST_PERCTRL0   0x1C
#define HAL_OFST_PERCTRL1   0x20
#define HAL_OFST_PEREN      0x24
#define HAL_OFST_PERDIS     0x28
#define HAL_OFST_PERCLKEN   0x2C
#define HAL_OFST_remain_0	//保留
#define HAL_OFST_PERCTRL2   0x34
#define HAL_OFST_PERCTRL3   0x38
#define HAL_OFST_PERCTRL4   0x3C
#define HAL_OFST_remain_1	//保留
#define HAL_OFST_PERLOCK    0x44
#define HAL_OFST_remain_2	// 0x06C 0xEDC
#define HAL_OFST_SYSID		// 0xEE0 0xEEC

// #define HAL_ADDR_SYSCTRLx(_x)    ((_x)+HAL_BASE_SYSCTRL)

// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=->
// base gpio muxctrl
//#define HAL_ADDR_PERCTRL1   ( HAL_BASE_SYSCTRL+HAL_OFST_PERCTRL1 )
#define HAL_ADDR_IOMUXCTRL  ( HAL_BASE_SYSCTRL+HAL_OFST_PERCTRL1 )
//  VI0HS:GPIO_0 VI0VS:GPIO0_1
//  GPIO: 00
#define HAL_BIT_MUXCTRL_VI0_CLK         (BIT0+BIT1)
//  VI0DAT2:GPIO4_0 VI0DAT3:GPIO4_1 VI0DAT4:GPIO4_2 VI0DAT5:GPIO4_3
//  VI0DAT6:GPIO4_4 VI0DAT7:GPIO4_5 VI0DAT8:GPIO4_6 VI0DAT2:GPIO4_7
//  GPIO: 1
#define HAL_BIT_MUXCTRL_VI0_DATA        (BIT2)
//  VI1DAT0:GPIO6_4 VI1DAT1:GPIO6_5 VI1DAT2:GPIO6_6 VI1DAT3:GPIO6_7
//  VI1DAT4:GPIO7_0 VI1DAT5:GPIO7_1 VI1DAT6:GPIO7_2 VI1DAT7:GPIO7_3
//  GPIO: 1
#define HAL_BIT_MUXCTRL_VI1_DATA        (BIT23)
//  VI2HS:GPIO0_5 VI2VS:GPIO0_6
//  GPIO: 00
#define HAL_BIT_MUXCTRL_VI2_CLK         (BIT8+BIT7)
//  VI2DAT2:GPIO3_6 VI2DAT4:GPIO7_7 VI2DAT6:GPIO3_7
//  GPIO: 1
#define HAL_BIT_MUXCTRL_VI2_DATA        (BIT3)
//  VI3DAT0:GPIO5_0 VI3DAT1:GPIO5_1 VI3DAT2:GPIO5_2 VI3DAT3:GPIO5_3
//  VI3DAT4:GPIO5_4 VI3DAT5:GPIO5_5 VI3DAT6:GPIO5_6 VI3DAT7:GPIO5_7
//  GPIO: 1
#define HAL_BIT_MUXCTRL_VI3_DATA        (BIT6)
//  ERXERR:GPIO3_0  ECRS:GPIO3_1    ECOL:GPIO3_2
//  GPIO: 1
#define HAL_BIT_MUXCTRL_ETH             (BIT24)
//  DSIOCMD:GPIO1_3 SDIODAT0:GPIO1_4 SDIODAT1:GPIO1_5 SDIODAT2:GPIO1_6
//  SDIODAT3:GPIO2_1 SDIOCK:GPIO2_2
//  GPIO: 0
#define HAL_BIT_MUXCTRL_MMC             (BIT9)
//  IRRCV:GPIO2_3
//  GPIO: 1
#define HAL_BIT_MUXCTRL_IR              (BIT17)
//  SDA:GPIO3_3 SCL:GPIO3_4
//  GPIO: 1
#define HAL_BIT_MUXCTRL_I2C             (BIT14)
//  URXD1:GPIO2_4 UTXD1:GPIO2_5 URTSN1:GPIO2_6 UCTSN1:GPIO2_7
//  GPIO: 0
#define HAL_BIT_MUXCTRL_UART1           (BIT16)
//  SPIDI:GPIO6_0 SPIDO:GPIO7_6 SPICK:GPIO7_5 SPICSN0:GPIO6_1
//  GPIO: 0
#define HAL_BIT_MUXCTRL_SPI             (BIT10)
//  SPI_CS:en:1 CS:dis:0
#define HAL_BIT_MUXCTRL_SPI_CS          (BIT12)
//  SIO0XFS:GPIO6_2
//  GPIO: 0
#define HAL_BIT_MUXCTRL_SIO0_XFS        (BIT18)
//  SIO0XCK:GPIO6_3
//  GPIO: 0
#define HAL_BIT_MUXCTRL_SIO0_XCK        (BIT11)
//  ACKOUT:GPIO3_5
//  GPIO: 0
#define HAL_BIT_MUXCTRL_SIO0_ACKOUT     (BIT19)
//  SIO1DI:GPIO0_4 SIO1RFS:GPIO0_2 SIO1RCK:GPIO0_3
//  GPIO: 0
#define HAL_BIT_MUXCTRL_SIO1            (BIT5)
//  EBICS1N:GPIO1_1
//  GPIO: 0
#define HAL_BIT_MUXCTRL_SMI_EBICS1N     (BIT20)
//  EBIADR24:GPIO1_2
//  GPIO: 1
#define HAL_BIT_MUXCTRL_SMI_EBIADR24    (BIT21)
//  EBIRDYN:GPIO7_4
//  GPIO: 1
#define HAL_BIT_MUXCTRL_SMI_EBIRDYN     (BIT4)
//  DDRMRCVO:GPIO0_7 DDRMRCVI:GPIO1_0
//  GPIO: 0
#define HAL_BIT_MUXCTRL_DDR2            (BIT15)
//  PCIREQ4N:GPIO1_7 PCIGRANT4N:GPIO2_0
//  GPIO: 0
#define HAL_BIT_MUXCTRL_PCI             (BIT13)

// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=->
// the system control register
#define HAL_ADDR_SYSCTRL    ( HAL_BASE_SYSCTRL + HAL_OFST_CTRL )
// the bit of timer resource clock
// can set to 3M or main bus clock
#define DC_TimerCLK_ov(_x)	(0x00010000<<(_x * 2))

// GPIO
// 上电默认状态 GPIO全为输入
// 如果中断打开，IO的输出会影响中断
#define HAL_BASE_GPIO0			0x101E4000
#define HAL_BASE_GPIO1			0x101E5000
#define HAL_BASE_GPIO2			0x101E6000
#define HAL_BASE_GPIO3			0x101E7000
#define HAL_BASE_GPIO4			0x101F7000
#define HAL_BASE_GPIO5			0x101F8000
#define HAL_BASE_GPIO6			0x101F9000
#define HAL_BASE_GPIO7			0x101FA000
//
#define HAL_BASE_GPIOx(_g)  \
	(((_g)<4)?(0x101E4000+(_g)*0x00001000):(0x101F7000+((_g)-4)*0x00001000))
//
#define HAL_OFST_GPIO_DATA			0x000
#define HAL_OFST_GPIO_MDATA(_x)		((_x)<<2)	//Muilt
#define HAL_OFST_GPIO_SDATA(_x)		((0x01<<(_x))<<2)	//Sigal
#define HAL_OFST_GPIO_DIR			0x400
#define HAL_OFST_GPIO_IS			0x404
#define HAL_OFST_GPIO_IBE			0x408
#define HAL_OFST_GPIO_IEV			0x40C
#define HAL_OFST_GPIO_IE			0x410
#define HAL_OFST_GPIO_RIS			0x414
#define HAL_OFST_GPIO_MIS			0x418
#define HAL_OFST_GPIO_IC			0x41C
//
#define HAL_GPIO_Bit(_x)				(0x01<<(_x))
//
#define HAL_GPIOx_DATA( _g ) 		(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_DATA)	//|
#define HAL_GPIOx_MDATA( _g,_b) 	(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_MDATA(_b))	//|
#define HAL_GPIOx_SDATA( _g,_b) 	(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_SDATA(_b))	//|
#define HAL_GPIOx_DIR(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_DIR)
#define HAL_GPIOx_IS(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IS)
#define HAL_GPIOx_IBE(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IBE)
#define HAL_GPIOx_IEV(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IEV)
#define HAL_GPIOx_IE(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IE)
#define HAL_GPIOx_RIS(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_RIS)
#define HAL_GPIOx_MIS(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_MIS)
#define HAL_GPIOx_IC(_g) 			(HAL_BASE_GPIOx(_g)+HAL_OFST_GPIO_IC)

#define DF_PIN_ClassifyGrp(_g)	((_g)==0)?0:(((_g)==1)?1:2)

#define DF_Set_GPIO_ADDx(_g,_b,_addr)	{\
		uint32 _v;\
		_v= HAL_readl(_addr);\
		_v|=HAL_GPIO_Bit(_b);\
		HAL_writel(_v,_addr);}

#define DF_Clr_GPIO_ADDx(_g,_b,_addr)	{\
		uint32 _v;\
		_v= HAL_readl(_addr);\
		_v&=~HAL_GPIO_Bit(_b);\
		HAL_writel(_v,_addr);}

#define DF_Switch_GPIO_ADDx(_g,_b,_addr)	{\
		uint32 _v;\
		_v= HAL_readl(_addr);\
		_v^=HAL_GPIO_Bit(_b);\
		HAL_writel(_v,_addr);}

#define DF_Get_GPIO_ADDx(_g,_b,_addr)	((HAL_readl(_addr)&HAL_GPIO_Bit(_b))?1:0)

// ================================================================================
// ---------------------------------------------------------------------->GPIO Macro
#define DF_Set_GPIO_DIRx(_g,_b)		DF_Set_GPIO_ADDx(_g,_b,HAL_GPIOx_DIR(_g))
#define DF_Clr_GPIO_DIRx(_g,_b)		DF_Clr_GPIO_ADDx(_g,_b,HAL_GPIOx_DIR(_g))
#define DF_Set_GPIO_Outx(_g,_b)		DF_Set_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
#define DF_Clr_GPIO_Outx(_g,_b)		DF_Clr_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
#define DF_Switch_GPIO_Outx(_g,_b)	DF_Switch_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
#define DF_Get_GPIO_INx(_g,_b)		DF_Get_GPIO_ADDx(_g,_b,HAL_GPIOx_SDATA(_g,_b))
//INT
#define DC_SET_GPIO_INT_RASE(_g,_b)	DF_Set_GPIO_ADDx((_g),(_b),HAL_GPIOx_IEV(_g))
#define DC_SET_GPIO_INT_FALL(_g,_b)	DF_Clr_GPIO_ADDx((_g),(_b),HAL_GPIOx_IEV(_g))
#define DC_Get_GPIO_RIS(_g,_b)		DF_Get_GPIO_ADDx((_g),(_b),HAL_GPIOx_RIS(_g))
#define DC_Clr_GPIO_INT_Sta(_g,_b)	DF_Set_GPIO_ADDx((_g),(_b),HAL_GPIOx_IC(_g))

// ---------------------------------------------------------------------->GPIO Macro

//Timer
#define HAL_BASE_TIMER_01	0x101E2000
#define HAL_BASE_TIMER_23	0x101E3000

#define HAL_OFST_TIME_LOAD		0x00
#define HAL_OFST_TIME_VALUE		0x04
#define HAL_OFST_TIME_CONTROL	0x08
#define HAL_OFST_TIME_INTCLR	0x0C
#define HAL_OFST_TIME_RIS		0x10
#define HAL_OFST_TIME_MIS		0x14
#define HAL_OFST_TIME_BGLOAD	0x18

#define HAL_OFST_TIME02			0x00
#define HAL_OFST_TIME13			0x20

//TIMER
#define HAL_TIMER_ADDR(_g,_r) \
	((((_g)==0)||((_g)==1))?(HAL_BASE_TIMER_01+(_r)):((((_g)==2)||((_g)==3))?(HAL_BASE_TIMER_23+(_r)):0x00))

#define HAL_TIMER_OFST(_g,_r) \
	((((_g)==0)||((_g)==2))?(HAL_OFST_TIME02+(_r)):((((_g)==1)||((_g)==3))?(HAL_OFST_TIME13+(_r)):0x00))

#define DF_TIMER_SetLoad(_v,_g)	\
	HAL_writel((_v),HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_LOAD)))

#define DF_TIMER_SetBLoad(_v,_g)	\
	HAL_writel((_v),HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_BGLOAD)))

#define DF_TIMER_GetLoad(_g)	\
	HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_LOAD)))

#define DF_TIMER_GetBLoad(_g)	\
	HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_BGLOAD)))

#define DF_TIMER_SetControl(_v,_g)	\
	HAL_writel((_v),HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_CONTROL)))

#define DF_TIMER_GetControl(_g)	\
	HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_CONTROL)))

#define DF_TIMER_SetINTCLR(_v,_g)	\
	HAL_writel((_v),HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_INTCLR)))
/*
//#define DF_TIMER_GetINTCLR(_g)	\
		{HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_INTCLR)));}
//#define DF_TIMER_SetRIS(_v,_g)	\
		HAL_writel((_v),HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_RIS)))
*/
#define DF_TIMER_GetRIS(_g)	\
	HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_RIS)))
/*
//#define DF_TIMER_SetMIS(_v,_g)	\
		HAL_writel((_v),HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(HAL_OFST_TIME_MIS)))
*/
#define DF_TIMER_GetMIS(_g)	\
	HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_MIS)))

// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=->
// ---------------------------------------------------------------------->IIC
#define HAL_BASE_IIC        0x101F6000
#define HAL_OFST_IIC_CON                0x000
#define HAL_OFST_IIC_TAR                0x004
#define HAL_OFST_IIC_SAR                0x008
#define HAL_OFST_IIC_DATA               0x010
#define HAL_OFST_IIC_ssSCLH             0x014
#define HAL_OFST_IIC_ssSCLL             0x018
#define HAL_OFST_IIC_fsSCLH             0x01C
#define HAL_OFST_IIC_fsSCLL             0x020
#define HAL_OFST_IIC_intSTA             0x02C
#define HAL_OFST_IIC_intMASK            0x030
#define HAL_OFST_IIC_rawINTSTA          0x034
#define HAL_OFST_IIC_rxTL               0x038
#define HAL_OFST_IIC_txTL               0x03C
#define HAL_OFST_IIC_clrINT             0x040
#define HAL_OFST_IIC_clrRXUNDER         0x044
#define HAL_OFST_IIC_clrRXOVER          0x048
#define HAL_OFST_IIC_clrTXOVER          0x04C
#define HAL_OFST_IIC_clrTXABRT          0x054
#define HAL_OFST_IIC_clrACTIVITY        0x05C
#define HAL_OFST_IIC_clrSTOPDET         0x060
#define HAL_OFST_IIC_clrSTARTDET        0x064
#define HAL_OFST_IIC_clrGENCALL         0x068
#define HAL_OFST_IIC_ENABLE             0x06C
#define HAL_OFST_IIC_STATUS             0x070
#define HAL_OFST_IIC_TXFLR              0x074
#define HAL_OFST_IIC_RXFLR              0x078
#define HAL_OFST_IIC_txABRTS            0x080
#define HAL_OFST_IIC_DMACR              0x088
#define HAL_OFST_IIC_DMATDLR            0x08C
#define HAL_OFST_IIC_DMARDLR            0x090

#define DF_IIC_SET_MUXCTRL  {\
		uint32 _v = HAL_readl( HAL_ADDR_IOMUXCTRL )\
		            _v &= ~HAL_BIT_MUXCTRL_I2C;\
		HAL_readl((_v),HAL_ADDR_IOMUXCTRL);}

#endif
