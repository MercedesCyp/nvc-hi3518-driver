#ifndef __REGShiftLens_H
#define __REGShiftLens_H



typedef struct{
	unsigned long aIL_LightDIR;
	unsigned long aIL_LightDATA;
	unsigned long aIC_LensDIR;
	unsigned long aIC_LensDATA;
	unsigned long aIP_REG;
}mIO_Stroage;



/*
IO Multi control
*/
#define MUXCTRL_BASE     (0x200f0000)


#define MUXCTRL_REG12    (MUXCTRL_BASE + 0x030) 
#define MUXCTRL_REG13    (MUXCTRL_BASE + 0x034) 
#define MUXCTRL_REG72    (MUXCTRL_BASE + 0x120)

#define DFMC_ACT_MUX_SIG1 \
{unsigned long _d; _d=0; hi_writel( _d, MUXCTRL_REG12 );}

#define DFMC_ACT_MUX_SIG2 \
{unsigned long _d; _d=0; hi_writel( _d, MUXCTRL_REG13 );}

#define DFMC_ACT_MUX_SIG3 \
{unsigned long _d; _d=0; hi_writel( _d, MUXCTRL_REG72 );}

// GPIO0 define
#define GPIO0_BASE       0x20140000
// GPIO3 define
#define GPIO3_BASE       0x20170000

#define GPIO_REG_DATA	0x3FC
#define GPIO_REG_DIR	0x400
#define GPIO_REG_IS		0x404
#define GPIO_REG_IBE	0x408
#define GPIO_REG_IEV	0x40C
#define GPIO_REG_IE		0x410
#define GPIO_REG_RIS	0x414
#define GPIO_REG_MIS	0x418
#define GPIO_REG_IC		0x41C

/*
Infrared Light
*/
#define DCIL_ONOFF_DBIT	(0x04<<0)
#define DCIL_DATA_MASK	(DCIL_ONOFF_DBIT)

#define DCIL_ONOFF_BIT	0x01

#define DCIL_REG_BASE	GPIO0_BASE
#define DCIL_REG_DATA	(DCIL_REG_BASE+DCIL_DATA_MASK)
#define DCIL_REG_DIR	(DCIL_REG_BASE+GPIO_REG_DIR)
#define DCIL_REG_IS		(DCIL_REG_BASE+GPIO_REG_IS)
#define DCIL_REG_IBE	(DCIL_REG_BASE+GPIO_REG_IBE)
#define DCIL_REG_IEV	(DCIL_REG_BASE+GPIO_REG_IEV)
#define DCIL_REG_IE		(DCIL_REG_BASE+GPIO_REG_IE)
#define DCIL_REG_RIS	(DCIL_REG_BASE+GPIO_REG_RIS)
#define DCIL_REG_MIS	(DCIL_REG_BASE+GPIO_REG_MIS)
#define DCIL_REG_IC		(DCIL_REG_BASE+GPIO_REG_IC)


#define DFIL_ACT_OUT	\
{unsigned long _d; _d=hi_readl(DCIL_REG_DIR); _d|=DCIL_ONOFF_BIT;  hi_writel( _d, DCIL_REG_DIR );}
#define DFIL_ACT_IN		\
{unsigned long _d; _d=hi_readl(DCIL_REG_DIR); _d&=~DCIL_ONOFF_BIT; hi_writel( _d, DCIL_REG_DIR );}

#define DFIL_ACT_HIGH	\
{unsigned long _d; _d=hi_readl(DCIL_REG_DATA); _d|=DCIL_ONOFF_BIT; hi_writel( _d, DCIL_REG_DATA );}

#define DFIL_ACT_LOW	\
{unsigned long _d; _d=hi_readl(DCIL_REG_DATA); _d&=~DCIL_ONOFF_BIT; hi_writel( _d, DCIL_REG_DATA );}


/*
Infrared Cammera (Lens)
*/
/*
#define DCIC_NIGHT_DBIT	(0x04<<0)
#define DCIC_DAY_DBIT	(0x04<<1)
#define DCIC_DATA_MASK	(DCIC_NIGHT_DBIT+DCIC_DAY_DBIT)

#define DCIC_NIGHT_BIT	(0x01)
#define DCIC_DAY_BIT	(0x02)
#define DCIC_BIT_MASK	(DCIC_NIGHT_BIT+DCIC_DAY_BIT)
*/
#define DCIC_DAY_DBIT	(0x04<<0)
#define DCIC_NIGHT_DBIT	(0x04<<1)
#define DCIC_DATA_MASK	(DCIC_NIGHT_DBIT+DCIC_DAY_DBIT)

#define DCIC_DAY_BIT	(0x01)
#define DCIC_NIGHT_BIT	(0x02)
#define DCIC_BIT_MASK	(DCIC_NIGHT_BIT+DCIC_DAY_BIT)


#define DCIC_REG_BASE	GPIO3_BASE
#define DCIC_REG_DATA	(DCIC_REG_BASE+DCIC_DATA_MASK)
#define DCIC_REG_DIR	(DCIC_REG_BASE+GPIO_REG_DIR)
#define DCIC_REG_IS		(DCIC_REG_BASE+GPIO_REG_IS)
#define DCIC_REG_IBE	(DCIC_REG_BASE+GPIO_REG_IBE)
#define DCIC_REG_IEV	(DCIC_REG_BASE+GPIO_REG_IEV)
#define DCIC_REG_IE		(DCIC_REG_BASE+GPIO_REG_IE)
#define DCIC_REG_RIS	(DCIC_REG_BASE+GPIO_REG_RIS)
#define DCIC_REG_MIS	(DCIC_REG_BASE+GPIO_REG_MIS)
#define DCIC_REG_IC		(DCIC_REG_BASE+GPIO_REG_IC)






#define DFIC_ACT_OUT	\
{unsigned long _d; _d=hi_readl(DCIC_REG_DIR); _d|=(DCIC_NIGHT_BIT+DCIC_DAY_BIT);\
hi_writel( _d, DCIC_REG_DIR );}

#define DFIC_ACT_IN		\
{unsigned long _d; _d=hi_readl(DCIC_REG_DIR); _d&=~(DCIC_NIGHT_BIT+DCIC_DAY_BIT);\
hi_writel( _d, DCIC_REG_DIR );}

#define DFIC_ACT_DAY	\
{unsigned long _d; _d=hi_readl(DCIC_REG_DATA); \
_d&=~DCIC_BIT_MASK; _d|=DCIC_NIGHT_BIT; \
hi_writel( _d, DCIC_REG_DATA );}

#define DFIC_ACT_NIGHT	\
{unsigned long _d; _d=hi_readl(DCIC_REG_DATA); \
_d&=~DCIC_BIT_MASK; _d|=DCIC_DAY_BIT; \
hi_writel( _d, DCIC_REG_DATA );}




/*
Image Processing
*/



#define ISP_BASE         (0x205A0000)
//#define BYPASS           (ISP_BASE + 0x0040)
#define CCM_CTRL         (ISP_BASE + 0x04A4)

#define DCIP_COLOR_BIT	0x01


#define DFIP_ACT_ShiftColorMode	{hi_writel(0x01,CCM_CTRL);}
#define DFIP_ACT_ShiftMoMode	{hi_writel(0x00,CCM_CTRL);}


extern void F_InitShiftLens(void);
extern void F_UninitShiftLens(void);
extern void F_ShiftToDayMode(void);
extern void F_ShiftToNightMode(void);

#endif
