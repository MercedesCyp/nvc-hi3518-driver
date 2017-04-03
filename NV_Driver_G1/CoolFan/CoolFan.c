#include "CoolFan.h"

#include "../HAL/HAL_PWM.h"

#define FAN_MOV_MIN_VOL	500
#define FAN_MOV_MAX_VOL	2000


#define DC_COFAN_MIN_EXPORT		170 
#define DC_COFAN_MAX_EXPORT		47


// #define DC_COFAN_NORMAL_Export	3000
// #define DC_COFAN_OHMin_Export	2000
// #define DC_COFAN_OHMax_Export	500
// #define DC_COFAN_OOH_Export	100

#define DC_COFAN_OH_Temp		500
#define DC_COFAN_OOH_Temp		650

#define DF_COOFAN_ExportIncreasing	{sExportValue-=2;}
#define DF_COOFAN_ExportDecreasing	{sExportValue+=2;}



static uint16 sExportValue = DC_COFAN_MIN_EXPORT; 
int gfCOOLFAN_DriveTheFan(int32 tTemp){
	
	//DC_COOLFAN_INCREASING_POWER
	if(tTemp >= DC_COFAN_OH_Temp){
		if(tTemp>=DC_COFAN_OOH_Temp){
			sExportValue = 1;
		}else{
			if(sExportValue>DC_COFAN_MIN_EXPORT)
				sExportValue=DC_COFAN_MIN_EXPORT;
			else if(sExportValue>DC_COFAN_MAX_EXPORT)
				DF_COOFAN_ExportIncreasing;
		}
	}else{ //DC_COOLFAN_DECREASING_POWER
		if(sExportValue>DC_COFAN_MIN_EXPORT)
			sExportValue = 250;
		else
			DF_COOFAN_ExportDecreasing;
	}
	
	gfPWM_ModeSetWidth(0,sExportValue);
	
// #if OPEN_DEBUG
	// NVC_Print("The Current Export:%d\r\n",(int)sExportValue);
// #endif
	return 0;

}


