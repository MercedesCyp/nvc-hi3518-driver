#ifndef __IfrLight_H
#define __IfrLight_H

#include "../GlobalParameter.h"

#define DC_IrLight_CMDSet		0x80
#define DC_IrLight_CMDGet		0x40


#define DC_IrLight_StaOn		0x01
#define DC_IrLight_StaOff		0x02



extern uint8 gfDrv_InfraredLightStatus(uint8 icmd);

extern int32 gfInitDrv_IfrLight(uint8 iRemain);
extern int32 gfUninitDrv_IfrLight(uint8 iRemain);


#endif
