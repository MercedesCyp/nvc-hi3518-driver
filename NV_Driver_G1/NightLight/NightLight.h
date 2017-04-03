#ifndef __NightLight_H
#define __NightLight_H

#include "../GlobalParameter.h"



extern int32 gfInitDrv_NightLight(uint8 iRemain);
extern int32 gfUninitDrv_NightLight(uint8 iRemain);

#define DC_NiLi_CmdSet				0x80
#define DC_NiLi_CmdGet				0x40
#define DC_NiLi_AdjustBrightness	0x02
#define DC_NiLi_On					0x01
#define DC_NiLi_Off					0x00
extern int32 gfNightLight_AdjustLight(uint8 iCmd,uint8 iDegree);

#endif