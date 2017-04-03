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


#define DUAL_TM1_BASE    (0x20010000)
#define TIMER2_LOAD      (DUAL_TM1_BASE + 0x0000)
#define TIMER2_VALUE     (DUAL_TM1_BASE + 0x0004)
#define TIMER2_CONTROL   (DUAL_TM1_BASE + 0x0008)
#define TIMER2_INTCLR    (DUAL_TM1_BASE + 0x000C)
#define TIMER2_RIS       (DUAL_TM1_BASE + 0x0010)
#define TIMER2_MIS       (DUAL_TM1_BASE + 0x0014)
#define TIMER2_BGLOAD    (DUAL_TM1_BASE + 0x0018)

#define TIMER3_LOAD      (DUAL_TM1_BASE + 0x0020)
#define TIMER3_VALUE     (DUAL_TM1_BASE + 0x0024)
#define TIMER3_CONTROL   (DUAL_TM1_BASE + 0x0028)
#define TIMER3_INTCLR    (DUAL_TM1_BASE + 0x002C)
#define TIMER3_RIS       (DUAL_TM1_BASE + 0x0030)
#define TIMER3_MIS       (DUAL_TM1_BASE + 0x0034)
#define TIMER3_BGLOAD    (DUAL_TM1_BASE + 0x0038)





#define CRG_BASE         (0x20030000)
#define PERI_CRG14       (CRG_BASE + 0x0038)

#define SC_BASE          (0x20050000)
#define SC_CTRL          (SC_BASE + 0x0000)


#define ADC_BASE         (0x200B0000)
#define ADC_STATUS       (ADC_BASE + 0x0000)
#define ADC_CTRL         (ADC_BASE + 0x0004)
#define ADC_POWERDOWN    (ADC_BASE + 0x0008)
#define ADC_INT_STATUS   (ADC_BASE + 0x000C)
#define ADC_INT_MASK     (ADC_BASE + 0x0010)
#define ADC_INT_CLR      (ADC_BASE + 0x0014)
#define ADC_INT_RAW      (ADC_BASE + 0x0018)
#define ADC_RESULT       (ADC_BASE + 0x001C)




#define MUXCTRL_BASE        (0x200f0000)

#define MUXCTRL_CRG1        (MUXCTRL_BASE + 0x0004)
#define MUXCTRL_CRG32       (MUXCTRL_BASE + 0x0080)
#define MUXCTRL_CRG33       (MUXCTRL_BASE + 0x0084)
#define MUXCTRL_CRG35       (MUXCTRL_BASE + 0x008C)
#define MUXCTRL_CRG36       (MUXCTRL_BASE + 0x0090)
#define MUXCTRL_CRG37       (MUXCTRL_BASE + 0x0094)
#define MUXCTRL_CRG38       (MUXCTRL_BASE + 0x0098)
#define MUXCTRL_CRG39       (MUXCTRL_BASE + 0x009C)

#define MUXCTRL_CRG45       (MUXCTRL_BASE + 0x00B4)
#define MUXCTRL_CRG46       (MUXCTRL_BASE + 0x00B8)

#define MUXCTRL_REG47       (MUXCTRL_BASE + 0x0BC) 
#define MUXCTRL_REG48       (MUXCTRL_BASE + 0x0C0) 
#define MUXCTRL_REG72       (MUXCTRL_BASE + 0x120)
#define MUXCTRL_REG73       (MUXCTRL_BASE + 0x124)
#define MUXCTRL_REG74       (MUXCTRL_BASE + 0x128) 
#define MUXCTRL_REG76       (MUXCTRL_BASE + 0x130) 



#define PWM_BASE 	        (0x20130000)
#define PWM0_CFG0			(PWM_BASE + 0x00)
#define PWM0_CFG1			(PWM_BASE + 0x04)
#define PWM0_CFG2			(PWM_BASE + 0x08)
#define PWM0_CTRL			(PWM_BASE + 0x0C)
#define PWM0_STATE0			(PWM_BASE + 0x10)
#define PWM0_STATE1			(PWM_BASE + 0x14)
#define PWM0_STATE2			(PWM_BASE + 0x18)

#define PWM1_CFG0   		(PWM_BASE + 0x20)
#define PWM1_CFG1			(PWM_BASE + 0x24)
#define PWM1_CFG2			(PWM_BASE + 0x28)
#define PWM1_CTRL			(PWM_BASE + 0x2C)
#define PWM1_STATE0			(PWM_BASE + 0x30)
#define PWM1_STATE1			(PWM_BASE + 0x34)
#define PWM1_STATE2			(PWM_BASE + 0x38)

#define PWM2_CFG0			(PWM_BASE + 0x40)
#define PWM2_CFG1			(PWM_BASE + 0x44)
#define PWM2_CFG2			(PWM_BASE + 0x48)
#define PWM2_CTRL			(PWM_BASE + 0x4C)
#define PWM2_STATE0			(PWM_BASE + 0x50)
#define PWM2_STATE1			(PWM_BASE + 0x54)
#define PWM2_STATE2			(PWM_BASE + 0x58)




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

// GPIO1 define #define GPIO1_BASE       0x20150000
#define GPIO1_0          (GPIO1_BASE + (1 << (2 + 0)))
#define GPIO1_1          (GPIO1_BASE + (1 << (2 + 1)))
#define GPIO1_2          (GPIO1_BASE + (1 << (2 + 2)))
#define GPIO1_3          (GPIO1_BASE + (1 << (2 + 3)))
#define GPIO1_4          (GPIO1_BASE + (1 << (2 + 4)))
#define GPIO1_5          (GPIO1_BASE + (1 << (2 + 5)))
#define GPIO1_6          (GPIO1_BASE + (1 << (2 + 6)))
#define GPIO1_7          (GPIO1_BASE + (1 << (2 + 7)))
#define GPIO1_DIR        (GPIO1_BASE  + 0x400)
#define GPIO1_IS         (GPIO1_BASE  + 0x404)
#define GPIO1_IBE        (GPIO1_BASE  + 0x408)
#define GPIO1_IEV        (GPIO1_BASE  + 0x40C)
#define GPIO1_IE         (GPIO1_BASE  + 0x410)
#define GPIO1_RIS        (GPIO1_BASE  + 0x414)
#define GPIO1_MIS        (GPIO1_BASE  + 0x418)
#define GPIO1_IC         (GPIO1_BASE  + 0x41C)



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


// GPIO6 define
#define GPIO6_BASE       0x201A0000
#define GPIO6_0          (GPIO6_BASE + (1 << (2 + 0)))
#define GPIO6_1          (GPIO6_BASE + (1 << (2 + 1)))
//#define GPIO6_2          (GPIO6_BASE + (1 << (2 + 2)))
#define GPIO6_3          (GPIO6_BASE + (1 << (2 + 3)))
#define GPIO6_4          (GPIO6_BASE + (1 << (2 + 4)))
#define GPIO6_5          (GPIO6_BASE + (1 << (2 + 5)))
#define GPIO6_6          (GPIO6_BASE + (1 << (2 + 6)))
#define GPIO6_7          (GPIO6_BASE + (1 << (2 + 7)))
#define GPIO6_DIR        (GPIO6_BASE  + 0x400)
#define GPIO6_IS         (GPIO6_BASE  + 0x404)
#define GPIO6_IBE        (GPIO6_BASE  + 0x408)
#define GPIO6_IEV        (GPIO6_BASE  + 0x40C)
#define GPIO6_IE         (GPIO6_BASE  + 0x410)
#define GPIO6_RIS        (GPIO6_BASE  + 0x414)
#define GPIO6_MIS        (GPIO6_BASE  + 0x418)
#define GPIO6_IC         (GPIO6_BASE  + 0x41C)




#define hi_readl(x)     readl(IO_ADDRESS(x))
#define hi_writel(v,x)	writel(v, IO_ADDRESS(x))

#define HW_REG(reg) *((volatile unsigned int *)(reg))
#define IO_REG(reg) *((volatile unsigned int *)((IO_ADDRESS(reg))))
#endif  /* __HI_COMMON_H_ */
