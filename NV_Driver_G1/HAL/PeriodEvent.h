#ifndef __PeriodEvent_H
#define __PeriodEvent_H

#include "../GlobalParameter.h"
#include "../String.h"

extern int32 gfInitDrv_PeroidEvent(void);
extern int32 gfUnInitDrv_PeroidEvent(void);


#define DC_PE_TYPE_INT		0x01
#define DC_PE_TYPE_UNINT	0x02

extern int32 gfPE_RegisterEvent( void (*iHandler)(void), uint16 iTime ,uint8 iMode);

extern int32 gfPE_UnregisterEvent( void (*iHandler)(void) ,uint8 iMode);

#endif