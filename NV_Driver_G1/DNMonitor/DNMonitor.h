#ifndef __DNMonitor_H
#define __DNMonitor_H

#include "../GlobalParameter.h"


extern void (*gfDNMonitor_ReportStateChange)(uint8 iStatus);

extern int32 gfInitDrv_DNMonitor(uint8 iRemain);
extern int32 gfUninitDrv_DNMonitor(uint8 iRemain);
extern uint8 gfDNMonitor_GetStatus(void);

#endif