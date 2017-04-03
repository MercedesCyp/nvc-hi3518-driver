#ifndef __TempMonitor_H
#define __TempMonitor_H

#include "../GlobalParameter.h"

extern int32 gfInitDrv_TempMonitor(uint8 iRemain);
extern int32 gfUninitDrv_TempMonitor(uint8 iRemain);


/**
*	iTime 			Unit n*1s
*/
#define DC_TEMPMONITOR_REPORT_Start		0x01
#define DC_TEMPMONITOR_REPORT_Stop		0x02
extern void gfTempMonitor_SetReport(uint32 iCmd,uint32 iTime);
extern void (*pgfTempMonitor_PeriodEvent)(int32);

extern int32 gfTempMonitor_GetNowValue(void);

#endif