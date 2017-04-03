#ifndef __SHT20_H
#define __SHT20_H

#include "../GlobalParameter.h"

extern void gfSHT20_INIT(void);
extern void gfSHT20_UNINIT(void);

#define DC_SHT20OptCMD_Get_Temp			0x01
#define DC_SHT20OptCMD_Get_Humidity		0x02
extern int32 gfSHT20Opt_GetMeasure(uint8 iCmd);




#endif