#ifndef __IfrFilter_H
#define __IfrFilter_H

#include "../GlobalParameter.h"



#define DC_IrFiltCMD_SetDAY			0x80
#define DC_IrFiltCMD_SetNight		0x40
#define DC_IrFiltCMD_Get			0x01

#define DC_IrFiltRET_Err			0x8000
#define DC_IrFiltRET_DAY			0x0001
#define DC_IrFiltRET_NIGHT			0x0002
#define DC_IrFiltRET_TypeA			0x0100
#define DC_IrFiltRET_TypeB			0x0200
extern uint16 gfDrv_InfraredFilterStatus(uint8 icmd);

extern int32 gfInitDrv_IfrFilter(uint8 iRemain);
extern int32 gfUninitDrv_IfrFilter(uint8 iRemain);



#endif