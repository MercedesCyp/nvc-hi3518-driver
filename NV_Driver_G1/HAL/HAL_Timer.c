#include "HAL_Timer.h"
#include "USERHAL.h"
#include <linux/slab.h> //printk

#define DC_Timer_Frequency	3000000

void fgConfigTimer3(uint16 iFrq,uint8 iCmd)
{
	uint32 theCmdValue = 0;
	uint32 theLoadVal;
	uint8 theDivide = 0;
	// printk("The cmd:%x\r\n",iCmd);
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
	
	theLoadVal = (DC_Timer_Frequency>>theDivide)/iFrq;
	
	DF_Timer3_SetLoad(theLoadVal);
	DF_Timer3_SetBLoad(theLoadVal);
	DF_Timer3_Disable;
	DF_TIMER_SetControl(theCmdValue,3);
	
	DF_Timer3CLK_Set3MCLK;
	// {
		// uint32 tShow;
		// printk("The Timer3 CurLoad:%x\r\n",theLoadVal);
		// tShow = DF_TIMER_GetBLoad(3);
		// printk("The Timer3 Load:%x\r\n",tShow);
		// tShow = DF_TIMER_GetLoad(3);
		// printk("The Timer3 BLoad:%x\r\n",tShow);
	// }
}


// Timer1 1000Hz 
void gfINIT_HAL_Timer2(void){
	uint32 tTimer2CTL = 0x40|0x20|0x08|0x02;
	uint32 theLoadVal = (DC_Timer_Frequency>>8)/1000;
	DF_Timer2_SetLoad(theLoadVal);
	DF_Timer2_SetBLoad(theLoadVal);
	DF_Timer2_Disable;
	DF_TIMER_SetControl(tTimer2CTL,2);
	DF_Timer2CLK_Set3MCLK;
}

void gfUNINIT_HAL_Timer2(void){
	
	DF_Timer2_Disable;
	return;
}




