#include "HAL_ADC.h"
#include "USERHAL.h"

#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* aSem */
#include <linux/interrupt.h>
#include <asm/delay.h>

#define HAL_ADC_REGAddr(_ofst)		((_ofst)+HAL_BASE_SRCADC)

#define ADC_TIME_CFG	HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC)
#define ADC_STATUS		HAL_ADC_REGAddr(HAL_OFST_ADC_STATUS)
#define ADC_CTRL		HAL_ADC_REGAddr(HAL_OFST_ADC_CTRL)
#define ADC_POWERDOWN	HAL_ADC_REGAddr(HAL_OFST_ADC_POWERDOWN)
#define ADC_INT_STATUS	HAL_ADC_REGAddr(HAL_OFST_ADC_INT_STATUS)
#define ADC_INT_MASK	HAL_ADC_REGAddr(HAL_OFST_ADC_INT_MASK)
#define ADC_INT_CLR		HAL_ADC_REGAddr(HAL_OFST_ADC_INT_CLR)
#define ADC_INT_RAW		HAL_ADC_REGAddr(HAL_OFST_ADC_RAW)
#define ADC_RESULT		HAL_ADC_REGAddr(HAL_OFST_ADC_RESULT)




//cfg
#define ADC_TIME_CFG_RESET	0x01
#define ADC_TIME_CFG_TIME	0x02


//STATUS
#define ADC_B_CONVERT_STATUS	0x00000001

//CTRL
#define ADC_B_CHANNEL_0		0x00000000
#define ADC_B_CHANNEL_1		0x00010000
#define ADC_B_START_CONVERT	0x00000001

//POWER
#define ADC_B_POWERDOWN		0x00000001
#define ADC_B_POWERUP		0x00000000

//Int Status
#define ADC_B_INT_STATUS		0x00000001

//Int Mask	
#define ADC_B_INT_ENABLE		0x00000000
#define ADC_B_INT_DISABLE		0x00000001

//Int Clear
#define ADC_B_INT_CLEAR		0x00000001

#define ADC_B_INT_RAW			0x00000001

// #define HAL_readl(_addr)		readl(IO_ADDRESS(_addr))	
// #define HAL_writel(_v,_addr)	writel(_v, IO_ADDRESS(_addr))

#define SET_EN_ADC_MODULE		HAL_writel(HAL_readl(ADC_TIME_CFG)|ADC_TIME_CFG_TIME,ADC_TIME_CFG)
#define SET_DIS_ADC_MODULE		HAL_writel(HAL_readl(ADC_TIME_CFG)&(~ADC_TIME_CFG_TIME),ADC_TIME_CFG)

#define SET_RESET_ADC_MODULE		HAL_writel(HAL_readl(ADC_TIME_CFG)|ADC_TIME_CFG_RESET,ADC_TIME_CFG)
#define SET_DISRESET_ADC_MODULE		HAL_writel(HAL_readl(ADC_TIME_CFG)&(~ADC_TIME_CFG_RESET),ADC_TIME_CFG)


#define V_ADC_CONVERT_STATUS   	HAL_readl(ADC_STATUS)&ADC_B_CONVERT_STATUS)
#define CLR_ADC_CONVER_STATUS	HAL_writel(ADC_B_CONVERT_STATUS,ADC_STATUS)

#define SET_ADC_CHANNEL_0		HAL_writel(ADC_B_CHANNEL_0,ADC_CTRL)
#define SET_ADC_CHANNEL_1		HAL_writel(ADC_B_CHANNEL_1,ADC_CTRL)
#define SET_ADC_START			HAL_writel((HAL_readl(ADC_CTRL)|ADC_B_START_CONVERT),ADC_CTRL)

#define SET_ADC_POWER_DOWN		HAL_writel(ADC_B_POWERDOWN,ADC_POWERDOWN)
#define SET_ADC_POWER_UP		HAL_writel(ADC_B_POWERUP,ADC_POWERDOWN)
//CVT -->convert
#define V_ADC_CVT_INT_STATUS 	HAL_readl(ADC_INT_STATUS)&ADC_B_INT_STATUS)

#define SET_ADC_INT_ENABLE		HAL_writel(ADC_B_INT_ENABLE,ADC_INT_MASK)
#define SET_ADC_INT_DISABLE		HAL_writel(ADC_B_INT_DISABLE,ADC_INT_MASK)

#define SET_ADC_CLR_INT			HAL_writel(ADC_B_INT_CLEAR,ADC_INT_CLR)

#define V_ADC_INI_RAW			(HAL_readl(ADC_INT_RAW)&ADC_B_INT_RAW)

#define V_ADC_VALUE				HAL_readl(ADC_RESULT)




static uint8 tADCChannel = 0;
#define ADC_STATUS_IDLE		0x01
#define ADC_STATUS_WORKing 	0x02
//#define ADC_STATUS_DONE		0x04
typedef struct{
	uint16 aStatus;
	uint16 aResult;
}mADC_Struct;


static irqreturn_t ADC_int_Handler(int irq, void *id);

mADC_Struct *sHAL_ADC;


int32 gfINIT_HAL_ADC(void)
{
	sHAL_ADC = (mADC_Struct*)kmalloc(sizeof(mADC_Struct)*2,GFP_ATOMIC);
	
	if(sHAL_ADC==NULL)
		return -1;
	
	sHAL_ADC[0].aStatus = 0;
	sHAL_ADC[1].aStatus = 0;
	
	
	SET_EN_ADC_MODULE;
	SET_RESET_ADC_MODULE;
	udelay(1);
	SET_DISRESET_ADC_MODULE;
	
	SET_ADC_POWER_UP;
	SET_ADC_INT_ENABLE;
	 
	//request IRQ
	if(request_irq(IRQ_SAR_ADC, ADC_int_Handler, IRQF_SHARED, "HAl_ADC",&sHAL_ADC)){
		kfree(sHAL_ADC);
		return -1;
	}

	return 0;
}

int32 gfUNINIT_HAL_ADC(void)
{
	free_irq(IRQ_SAR_ADC,&sHAL_ADC);
	kfree(sHAL_ADC);
	return 0;
}



uint16 gfADC_GetAndStartConvert(uint8 iChannel)
{
	uint16 theADCResult = 0x8000;
	uint8 _i;
	
	
	_i = 0;
	while(_i<100){
		if( ((sHAL_ADC[0].aStatus&ADC_STATUS_WORKing)==0) && ((sHAL_ADC[1].aStatus&ADC_STATUS_WORKing)==0))
			break;
		udelay(2);
	}
	
	if(_i == 100)
		return theADCResult;

	tADCChannel = iChannel;	
	theADCResult = sHAL_ADC[iChannel].aResult;
	
	//sHAL_ADC[iChannel].aStatus = 0;
	sHAL_ADC[iChannel].aStatus = ADC_STATUS_WORKing;
	
	switch(iChannel)
	{
		case DC_ADC_Channel_0:
		
			SET_ADC_CHANNEL_0;
		
		break;
		case DC_ADC_Channel_1:
		
			SET_ADC_CHANNEL_1;
		
		break;
		default:break;
	}
	
	SET_ADC_START;
	return theADCResult;	
}




static irqreturn_t ADC_int_Handler(int irq, void *id)
{
	sHAL_ADC[tADCChannel].aResult = V_ADC_VALUE;
	
	SET_ADC_CLR_INT;
	//wake_up(&gADC_FUN_INT->aWQH_Return);
	//wake_up_interruptible(&gADC_FUN_INT->aWQH_Return);
	CLR_ADC_CONVER_STATUS;
	
	//sHAL_ADC.aStatus  |= ADC_STATUS_DONE;
	sHAL_ADC[tADCChannel].aStatus  &= ~ADC_STATUS_WORKing;
	return IRQ_HANDLED;
}
