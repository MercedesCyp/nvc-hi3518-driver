#ifndef  __HI_COMMON_H_
#define  __HI_COMMON_H_

//Data type
#define uint unsigned int
#define uchar unsigned char
#define ulong unsigned long



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



// GPIO0 define
#define GPIO0_BASE       0x101E4000
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
#define GPIO3_BASE       0x101E7000
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
#define GPIO5_BASE       0x101F8000
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
#define GPIO6_BASE       0x101F9000
#define GPIO6_0          (GPIO6_BASE + (1 << (2 + 0)))
#define GPIO6_1          (GPIO6_BASE + (1 << (2 + 1)))
#define GPIO6_2          (GPIO6_BASE + (1 << (2 + 2)))
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

#define GPIO_LED        (GPIO6_BASE + (1 << (2 + 0)))
#define GPIO_KEY        (GPIO6_BASE + (1 << (2 + 3)))

/*
//GPIO5 define G
#define GPIO5_BASE       0x101F8000
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
*/


// GPIO7 define
#define GPIO7_BASE       0x101FA000
#define GPIO7_0          (GPIO7_BASE + (1 << (2 + 0)))
#define GPIO7_1          (GPIO7_BASE + (1 << (2 + 1)))
#define GPIO7_2          (GPIO7_BASE + (1 << (2 + 2)))
#define GPIO7_3          (GPIO7_BASE + (1 << (2 + 3)))
#define GPIO7_4          (GPIO7_BASE + (1 << (2 + 4)))
#define GPIO7_5          (GPIO7_BASE + (1 << (2 + 5)))
#define GPIO7_6          (GPIO7_BASE + (1 << (2 + 6)))
#define GPIO7_7          (GPIO7_BASE + (1 << (2 + 7)))
#define GPIO7_DIR        (GPIO7_BASE  + 0x400)
#define GPIO7_IS         (GPIO7_BASE  + 0x404)
#define GPIO7_IBE        (GPIO7_BASE  + 0x408)
#define GPIO7_IEV        (GPIO7_BASE  + 0x40C)
#define GPIO7_IE         (GPIO7_BASE  + 0x410)
#define GPIO7_RIS        (GPIO7_BASE  + 0x414)
#define GPIO7_MIS        (GPIO7_BASE  + 0x418)
#define GPIO7_IC         (GPIO7_BASE  + 0x41C)







/*
//Step motor define
//L6219
#define PHASE1           GPIO5_0     
#define I01              GPIO5_1    
#define I11              GPIO5_2 
#define PHASE2           GPIO5_3  
#define I12              GPIO5_4 
#define I02              GPIO5_7    
//ULN2003A
#define BP               GPIO5_0     
#define AP               GPIO5_1    
#define AN               GPIO5_2 
#define BN               GPIO5_3  
*/
//Watchdog define
#define HI_UNLOCK_VAL   0x1ACCE551

#define WDT_BASE        0x101E1000
#define WDT_LOAD        WDT_BASE
#define WDT_LOCK        (WDT_BASE + 0xC00)
#define WDT_VALUE       (WDT_BASE + 0x004)
#define WDT_CTRL        (WDT_BASE + 0x008)
#define WDT_INTCLR      (WDT_BASE + 0x00C)


//Timer define
#define DTIMER0_BASE      0x101E2000
//Register of timer0
#define TIMER0_LOAD     (DTIMER0_BASE + 0x000)
#define TIMER0_VALUE    (DTIMER0_BASE + 0x004)
#define TIMER0_CONTROL  (DTIMER0_BASE + 0x008)
#define TIMER0_INTCLR   (DTIMER0_BASE + 0x00C)
#define TIMER0_RIS      (DTIMER0_BASE + 0x010)
#define TIMER0_MIS      (DTIMER0_BASE + 0x014)
#define TIMER0_BGLOAD   (DTIMER0_BASE + 0x018)

// Register of timer1
#define TIMER1_LOAD     (DTIMER0_BASE + 0x020)
#define TIMER1_VALUE    (DTIMER0_BASE + 0x024)
#define TIMER1_CONTROL  (DTIMER0_BASE + 0x028)
#define TIMER1_INTCLR   (DTIMER0_BASE + 0x02C)
#define TIMER1_RIS      (DTIMER0_BASE + 0x030)
#define TIMER1_MIS      (DTIMER0_BASE + 0x034)
#define TIMER1_BGLOAD   (DTIMER0_BASE + 0x038)

#define DTIMER1_BASE      0x101E3000
//Register of timer2
#define TIMER2_LOAD     (DTIMER1_BASE + 0x000)
#define TIMER2_VALUE    (DTIMER1_BASE + 0x004)
#define TIMER2_CONTROL  (DTIMER1_BASE + 0x008)
#define TIMER2_INTCLR   (DTIMER1_BASE + 0x00C)
#define TIMER2_RIS      (DTIMER1_BASE + 0x010)
#define TIMER2_MIS      (DTIMER1_BASE + 0x014)
#define TIMER2_BGLOAD   (DTIMER1_BASE + 0x018)

//Register of timer3
#define TIMER3_LOAD     (DTIMER1_BASE + 0x020)
#define TIMER3_VALUE    (DTIMER1_BASE + 0x024)
#define TIMER3_CONTROL  (DTIMER1_BASE + 0x028)
#define TIMER3_INTCLR   (DTIMER1_BASE + 0x02C)
#define TIMER3_RIS      (DTIMER1_BASE + 0x030)
#define TIMER3_MIS      (DTIMER1_BASE + 0x034)
#define TIMER3_BGLOAD   (DTIMER1_BASE + 0x038)


// interrupt number
#define  TIMER2_INT_NO           5
#define  GPIO0_INT_NO            6
#define  GPIO6_INT_NO            8


#define INTNR_IRQ_START	0
#define INTNR_IRQ_END	31

#define INTNR_WATCHDOG					0	/* Watchdog timer */
#define INTNR_SOFTINT					1	/* Software interrupt */
#define INTNR_COMMRx					2	/* Debug Comm Rx interrupt */
#define INTNR_COMMTx					3	/* Debug Comm Tx interrupt */
#define INTNR_TIMER_0_1					4	/* Timer 0 and 1 */
#define INTNR_TIMER_2_3                 5	/* Timer 2 and 3 */
#define INTNR_GPIO_0                    6	/* GPIO 0 */
#define INTNR_GPIO_1                    7	/* GPIO 1 */
#define INTNR_GPIO_2_7                  8	/* GPIO 2 */
#define INTNR_IR						9	/* GPIO 3 */
#define INTNR_RTC						10	/* Real Time Clock */
#define INTNR_SSP						11	/* Synchronous Serial Port */
#define INTNR_UART0						12
#define INTNR_UART1						13
#define INTNR_UART2						14
#define INTNR_ETH						15
#define INTNR_VOU						16
#define INTNR_DMAC						17
#define INTNR_SIO_1						18
#define INTNR_I2C						19
#define INTNR_USB11						20
#define INTNR_CIPHER					21
#define INTNR_EXTINT					22
#define INTNR_USBOTG					23
#define INTNR_MMC						24
#define INTNR_VIU						25
#define INTNR_DSU						26
#define INTNR_SIO_0						27
#define INTNR_VEDU						28
#define INTNR_RESERVED_0				29
#define INTNR_PCI						30
#define INTNR_TDE						31


#define hi_readl(x)     readl(IO_ADDRESS(x))
#define hi_writel(v,x)	writel(v, IO_ADDRESS(x))

#define HW_REG(reg) *((volatile unsigned int *)(reg))
#define IO_REG(reg) *((volatile unsigned int *)((IO_ADDRESS(reg))))

#define AI01 (1<<1)
#define AI01_HI  (HW_REG(IO_ADDRESS(REG_BASE_GPIO5 + (AI01 << 2))) =\
                   HW_REG(IO_ADDRESS(REG_BASE_GPIO5 + (AI01 << 2))) | AI01)
#define AI01_LO  (HW_REG(IO_ADDRESS(REG_BASE_GPIO5 + (AI01 << 2))) =\
                   HW_REG(IO_ADDRESS(REG_BASE_GPIO5 + (AI01 << 2))) & ~AI01)
#define gpio_bit_hi(base, offset)\
          (HW_REG(IO_ADDRESS((base) + ((1 << offset) << 2))) =\
           HW_REG(IO_ADDRESS((base) + ((1 << offset) << 2))) | (1 << offset))

#define gpio_bit_lo(base, offset)\
          (HW_REG(IO_ADDRESS((base) + ((1 << offset) << 2))) =\
           HW_REG(IO_ADDRESS((base) + ((1 << offset) << 2))) & ~(1 << offset))

#define gpio_bit_read(base, offset)\
          HW_REG(IO_ADDRESS((base) + ((1 << offset) << 2)))


#endif  /* __HI_COMMON_H_ */
