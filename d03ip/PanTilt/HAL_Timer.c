#include "HAL_Timer.h"
#include "USERHAL.h"
#include <linux/slab.h> //printk


#define DC_Timer_Frequency	99000000
void fgConfigTimer0(uint16 iFrq,uint8 iCmd)
{
	uint32 theCmdValue = 0;
	uint32 theLoadVal;
	uint8 theDivide = 0;
	if(iCmd&DC_Timer_OneShot)
		theCmdValue |= 0x01;
	else if(iCmd&DC_Timer_PeriodMode)
		theCmdValue |= 0x40;
	
	if(iCmd&DC_Timer_Intenable)
		theCmdValue |= 0x20;
	
	if((iCmd&DC_Timer_Timerpre)==DC_Timer_Timerpre_16){
		theCmdValue |= 0x04;
		theDivide = 4;
	}else if((iCmd&DC_Timer_Timerpre)==DC_Timer_Timerpre_256){
		theCmdValue |= 0x08;
		theDivide = 8;
	}
	if(iCmd&DC_Timer_Count_32bit)
		theCmdValue |= 0x02;	
	
	//theLoadVal = (HAL_MCU_FREQUENCY>>theDivide)/iFrq;
	theLoadVal = (DC_Timer_Frequency>>theDivide)/iFrq;
	
	DF_Timer0_SetLoad(theLoadVal);
	DF_Timer0_SetBLoad(theLoadVal);
	/*{
		uint32 bb;
		printk("the load: %u\r\n",theLoadVal);
		bb = DF_TIMER_GetLoad(0);
		printk("the mcuload: %u\r\n",theLoadVal);
		bb = DF_TIMER_GetBLoad(0);
		printk("the mcubload: %u\r\n",theLoadVal);
	}*/
	DF_Timer0CLK_Set3MCLK;
	DF_Timer_Disable(0);
	

	
	DF_TIMER_SetControl(theCmdValue,0);
	//DF_Timer_Enable(0);
}