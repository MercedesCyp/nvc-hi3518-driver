#include "REGShiftLens.h"

#include <linux/slab.h>  //kmalloc

#include <asm/io.h>



#define hi_readl(x)     readl(IO_ADDRESS(x))
#define hi_writel(v,x)	writel(v, IO_ADDRESS(x))

mIO_Stroage *gShiftLensIO;
void F_InitShiftLens(void)
{
	gShiftLensIO = kmalloc(sizeof(mIO_Stroage), GFP_KERNEL);
	gShiftLensIO->aIL_LightDIR = hi_readl(DCIL_REG_DIR);
	gShiftLensIO->aIL_LightDATA = hi_readl(DCIL_REG_DATA);
	gShiftLensIO->aIC_LensDIR = hi_readl(DCIC_REG_DIR);
	gShiftLensIO->aIC_LensDATA = hi_readl(DCIC_REG_DATA);
	//gShiftLensIO->aIP_REG = hi_readl(CCM_CTRL);
	//printk("------>Im here\r\n");
	DFMC_ACT_MUX_SIG1;
	DFMC_ACT_MUX_SIG2;
	DFMC_ACT_MUX_SIG3;
	DFIL_ACT_OUT;
	DFIC_ACT_OUT;
}

void F_UninitShiftLens(void)
{
	hi_writel(gShiftLensIO->aIL_LightDIR, DCIL_REG_DIR);
	hi_writel(gShiftLensIO->aIL_LightDATA, DCIL_REG_DATA);
	hi_writel(gShiftLensIO->aIC_LensDIR, DCIC_REG_DIR);
	hi_writel(gShiftLensIO->aIC_LensDATA, DCIC_REG_DATA);
	//hi_writel(gShiftLensIO->aIP_REG, CCM_CTRL);
	kfree(gShiftLensIO);
	gShiftLensIO=NULL;
}


void F_ShiftToDayMode(void)
{
	DFIL_ACT_LOW;
	DFIC_ACT_DAY;
	//DFIP_ACT_ShiftColorMode;
}


void F_ShiftToNightMode(void)
{
	DFIL_ACT_HIGH;
	DFIC_ACT_NIGHT;
	//DFIP_ACT_ShiftMoMode;
}