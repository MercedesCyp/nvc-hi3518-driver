#ifndef __HUMIDITY_H
#define __HUMIDITY_H

#include "../GlobalParameter.h"

extern int32 gfInitDrv_HumidityMonitor(uint8 iRemain);
extern int32 gfUninitDrv_HumidityMonitor(uint8 iRemain);


extern int32 gfHumidity_GetNowValue(void);

#define DC_HUMIDITY_REPORT_Start 	0x01
#define DC_HUMIDITY_REPORT_Stop 	0x02
extern void gfHumidityMonitor_SetReport(uint8 iCmd , uint32 iFreq);
extern void (*pgfHumidityMonitor_PeriodEvent)(int32 iHumidity);




#endif