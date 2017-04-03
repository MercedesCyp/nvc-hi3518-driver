#include "DNMonitor.h"
#include "../HAL/HAL_ADC.h"
#include "../HAL/PeriodEvent.h"

#include <linux/slab.h>

#define DC_DNMonitor_DefaultDayValue	0x40
#define DC_DNMonitor_DefaultnightValue	0x20

#define DC_DNMonitor_SumplingFreq		2 // 1  eq 100ms

#define DC_FLTOrder1_Steps 3
#define DC_FLTOrder2_Steps 6
#define DC_FLTOrder2_Period 4//DC_FLTOrder2_Period > DC_FLTOrder1_Steps





static void sfDNMonitor_MotorCheck(void);


typedef struct{
	uint16 aDLimit;
	uint16 aNLimit;
	uint16 aLSValue;
	uint8 tStatus;
#define DC_DNMonitor_DAY 	0x01
#define DC_DNMonitor_NIGHT 	0x00
}mDNMonitor;

mDNMonitor sDNMonitor;

static u16 saFltOrder1[DC_FLTOrder1_Steps]={0};
static u16 saFltOrder2[DC_FLTOrder2_Steps]={0};
static u16 sFltCount=0;

void (*gfDNMonitor_ReportStateChange)(uint8 iStatus);



int32 gfInitDrv_DNMonitor(uint8 iRemain)
{
	uint16 tLSValue;
	uint8 _i;
	
	gfADC_GetAndStartConvert(DC_ADC_UserID_For_LS);
	tLSValue = gfADC_GetAndStartConvert(DC_ADC_UserID_For_LS);
	
	for(_i = 0;_i<DC_FLTOrder1_Steps;_i++){
		saFltOrder1[_i] = tLSValue;
	}
	
	for(_i = 0;_i<DC_FLTOrder2_Steps;_i++){
		saFltOrder2[_i] = tLSValue;
	}
	sDNMonitor.aLSValue = tLSValue;
	
	sDNMonitor.aDLimit = DC_DNMonitor_DefaultDayValue;
	sDNMonitor.aNLimit = DC_DNMonitor_DefaultnightValue;
	
	gfPE_RegisterEvent(sfDNMonitor_MotorCheck,DC_DNMonitor_SumplingFreq,DC_PE_TYPE_UNINT);
	
#if OPEN_DEBUG
	NVCPrint("DNMonitor Started!\r\n");
#endif
	return 0;
}

int32 gfUninitDrv_DNMonitor(uint8 iRemain){
	gfPE_UnregisterEvent(sfDNMonitor_MotorCheck,DC_PE_TYPE_UNINT);
	return 0;
}

uint8 gfDNMonitor_GetStatus(void){
	return sDNMonitor.tStatus;
}

static void sfDNMonitor_MotorCheck(void){
	
	uint16 tADCSum;
	if(sFltCount<DC_FLTOrder2_Period-1){
		if(sFltCount<DC_FLTOrder1_Steps){
			saFltOrder1[0] = saFltOrder1[1];
			saFltOrder1[1] = saFltOrder1[2];
			saFltOrder1[2] = gfADC_GetAndStartConvert(DC_ADC_UserID_For_LS);
			if(sFltCount==2){
				tADCSum = \
				(saFltOrder1[0]<<2) + (saFltOrder1[1]<<1) + (saFltOrder1[2]<<1);
				tADCSum = tADCSum>>3;
				saFltOrder1[2] = tADCSum;
			}
		}
	}else{
		saFltOrder2[0] = saFltOrder2[1];
		saFltOrder2[1] = saFltOrder2[2];
		saFltOrder2[2] = saFltOrder2[3];
		saFltOrder2[3] = saFltOrder2[4];
		saFltOrder2[4] = saFltOrder2[5];
		saFltOrder2[5] = saFltOrder1[2];
		tADCSum = \
		(saFltOrder2[0]<<2) + (saFltOrder2[1]<<2) + (saFltOrder2[2]<<2) + \
		(saFltOrder2[3]<<1) + (saFltOrder2[4])    + (saFltOrder2[5]);
		tADCSum>>=4;
		sDNMonitor.aLSValue = tADCSum;
#if OPEN_DEBUG
	// NVCPrint_h("the NDMonitor value is:%d\r\n",sDNMonitor.aLSValue);
#endif
		if( (sDNMonitor.tStatus&DC_DNMonitor_DAY)&&(tADCSum<sDNMonitor.aNLimit)){
			
			sDNMonitor.tStatus &= ~DC_DNMonitor_DAY;
			gfDNMonitor_ReportStateChange(DC_DNMonitor_NIGHT);
			
		}else if((!(sDNMonitor.tStatus&DC_DNMonitor_DAY))&&(tADCSum>sDNMonitor.aDLimit)){
			
			sDNMonitor.tStatus |= DC_DNMonitor_DAY;
			gfDNMonitor_ReportStateChange(DC_DNMonitor_DAY);
		
		}
		sFltCount = 0;
	}
	/* 
	if(sFltCount%2)
		gfDNMonitor_ReportStateChange(DC_DNMonitor_NIGHT);
	else
		gfDNMonitor_ReportStateChange(DC_DNMonitor_DAY);
	*/
	
	sFltCount++;
}



