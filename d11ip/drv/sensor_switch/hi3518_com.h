#ifndef  __HI3518_COMMON_H_
#define  __HI3518_COMMON_H_

/*
 * define  register for hi3518
 */

/*
// interrupt register define
#define INT_BASE          0x10140000
#define INT_IRQSTATUS     INT_BASE 
#define INT_FIQSTATUS     (INT_BASE + 0x004)
#define INT_RAWINTR       (INT_BASE + 0x008)
#define INT_INTSELECT     (INT_BASE + 0x00C)
#define INT_INTENABLE     (INT_BASE + 0x010)
#define INT_INTENCLEAR    (INT_BASE + 0x014)
#define INT_SOFTINT       (INT_BASE + 0x018)
#define INT_SOFTINTCLEAR  (INT_BASE + 0x01C)
#define INT_PROTECTION    (INT_BASE + 0x020)


// SCTL register define
#define SCTL_BASE       0x101E0000
#define SC_CTRL         SCTL_BASE
#define SC_SYSSTAT      (SCTL_BASE + 0x004)
#define SC_PERCTRL1     (SCTL_BASE + 0x020)
#define SCPER_LOCK      (SCTL_BASE + 0x044)
*/


#define INT_SOFT         (0)
#define INT_WDT          (1)
#define INT_RTC          (2)
#define INT_TM0_TM1      (3)
#define INT_TM2_TM3      (4)
#define INT_UART0_UART1  (5)

#define INT_SIO          (9)
#define INT_TEMPER_CAP   (10)
#define INT_SFC          (11)
#define INT_ETH          (12)
#define INT_CIPHER       (13)
#define INT_DMAC         (14)
#define INT_USB_EHCI     (15)
#define INT_USB_OHCI     (16)
#define INT_VPSS         (17)
#define INT_SDIO         (18)
#define INT_SAR_ADC      (19)
#define INT_I2C          (20)
#define INT_IVE          (21)
#define INT_VICAP        (22)
#define INT_VOU          (23)
#define INT_VEDU         (24)

#define INT_JPGE         (26)
#define INT_TDE          (27)
#define INT_MDU_DDRT     (28)
#define INT_GPIO29       (29)    //GPIO0/ GPIO1/ GPIO2/ GPIO11
#define INT_GPIO30       (30)    //GPIO3/ GPIO4/ GPIO5/ GPIO10
#define INT_GPIO31       (31)    //GPIO6/ GPIO7/ GPIO8（Hi3518C NO/GPIO9（Hi3518C NO）






#define CRG_BASE         (0x20030000)
#define PERI_CRG11       (CRG_BASE + 0x002C)
#define PERI_CRG32       (CRG_BASE + 0x0080)

#define ISP_BASE         (0x205A0000)
#define BYPASS           (ISP_BASE + 0x0040)

#define CCM_CTRL         (ISP_BASE + 0x04A4)


#define ADC_BASE         (0x200B0000)
#define ADC_STATUS       (ADC_BASE + 0x0000)
#define ADC_CTRL         (ADC_BASE + 0x0004)
#define ADC_POWERDOWN    (ADC_BASE + 0x0008)
#define ADC_INT_STATUS   (ADC_BASE + 0x000C)
#define ADC_INT_MASK     (ADC_BASE + 0x0010)
#define ADC_INT_CLR      (ADC_BASE + 0x0014)
#define ADC_INT_RAW      (ADC_BASE + 0x0018)
#define ADC_RESULT       (ADC_BASE + 0x001C)




#define MUXCTRL_BASE     (0x200f0000)


#define MUXCTRL_REG12    (MUXCTRL_BASE + 0x030) 
#define MUXCTRL_REG13    (MUXCTRL_BASE + 0x034) 

#define MUXCTRL_REG47    (MUXCTRL_BASE + 0x0BC) 
#define MUXCTRL_REG48    (MUXCTRL_BASE + 0x0C0) 
#define MUXCTRL_REG72    (MUXCTRL_BASE + 0x120)
#define MUXCTRL_REG73    (MUXCTRL_BASE + 0x124)
#define MUXCTRL_REG74    (MUXCTRL_BASE + 0x128) 



// GPIO0 define
#define GPIO0_BASE       0x20140000
#define GPIO0_0          (GPIO0_BASE + (1 << (2 + 0)))
#define GPIO0_1          (GPIO0_BASE + (1 << (2 + 1)))
#define GPIO0_2          (GPIO0_BASE + (1 << (2 + 2)))
#define GPIO0_3          (GPIO0_BASE + (1 << (2 + 3)))
#define GPIO0_4          (GPIO0_BASE + (1 << (2 + 4)))
#define GPIO0_5          (GPIO0_BASE + (1 << (2 + 5)))
#define GPIO0_6          (GPIO0_BASE + (1 << (2 + 6)))
#define GPIO0_7          (GPIO0_BASE + (1 << (2 + 7)))
#define GPIO0_DIR        (GPIO0_BASE  + 0x400)
#define GPIO0_IS         (GPIO0_BASE  + 0x404)
#define GPIO0_IBE        (GPIO0_BASE  + 0x408)
#define GPIO0_IEV        (GPIO0_BASE  + 0x40C)
#define GPIO0_IE         (GPIO0_BASE  + 0x410)
#define GPIO0_RIS        (GPIO0_BASE  + 0x414)
#define GPIO0_MIS        (GPIO0_BASE  + 0x418)
#define GPIO0_IC         (GPIO0_BASE  + 0x41C)


// GPIO3 define
#define GPIO3_BASE       0x20170000
#define GPIO3_0          (GPIO3_BASE + (1 << (2 + 0)))
#define GPIO3_1          (GPIO3_BASE + (1 << (2 + 1)))
#define GPIO3_2          (GPIO3_BASE + (1 << (2 + 2)))
#define GPIO3_3          (GPIO3_BASE + (1 << (2 + 3)))
#define GPIO3_4          (GPIO3_BASE + (1 << (2 + 4)))
#define GPIO3_5          (GPIO3_BASE + (1 << (2 + 5)))
#define GPIO3_6          (GPIO3_BASE + (1 << (2 + 6)))
#define GPIO3_7          (GPIO3_BASE + (1 << (2 + 7)))
#define GPIO3_DIR        (GPIO3_BASE  + 0x400)
#define GPIO3_IS         (GPIO3_BASE  + 0x404)
#define GPIO3_IBE        (GPIO3_BASE  + 0x408)
#define GPIO3_IEV        (GPIO3_BASE  + 0x40C)
#define GPIO3_IE         (GPIO3_BASE  + 0x410)
#define GPIO3_RIS        (GPIO3_BASE  + 0x414)
#define GPIO3_MIS        (GPIO3_BASE  + 0x418)
#define GPIO3_IC         (GPIO3_BASE  + 0x41C)


// GPIO5 define
#define GPIO5_BASE       0x20190000
#define GPIO5_0          (GPIO5_BASE + (1 << (2 + 0)))
#define GPIO5_1          (GPIO5_BASE + (1 << (2 + 1)))
#define GPIO5_2          (GPIO5_BASE + (1 << (2 + 2)))
#define GPIO5_3          (GPIO5_BASE + (1 << (2 + 3)))
#define GPIO5_4          (GPIO5_BASE + (1 << (2 + 4)))
#define GPIO5_5          (GPIO5_BASE + (1 << (2 + 5)))
#define GPIO5_6          (GPIO5_BASE + (1 << (2 + 6)))
#define GPIO5_7          (GPIO5_BASE + (1 << (2 + 7)))
#define GPIO5_DIR        (GPIO5_BASE  + 0x400)
#define GPIO5_IS         (GPIO5_BASE  + 0x404)
#define GPIO5_IBE        (GPIO5_BASE  + 0x408)
#define GPIO5_IEV        (GPIO5_BASE  + 0x40C)
#define GPIO5_IE         (GPIO5_BASE  + 0x410)
#define GPIO5_RIS        (GPIO5_BASE  + 0x414)
#define GPIO5_MIS        (GPIO5_BASE  + 0x418)
#define GPIO5_IC         (GPIO5_BASE  + 0x41C)


#define hi_readl(x)     readl(IO_ADDRESS(x))
#define hi_writel(v,x)	writel(v, IO_ADDRESS(x))

#define HW_REG(reg) *((volatile unsigned int *)(reg))
#define IO_REG(reg) *((volatile unsigned int *)((IO_ADDRESS(reg))))
#endif  /* __HI_COMMON_H_ */
