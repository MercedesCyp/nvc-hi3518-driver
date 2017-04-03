#ifndef __DoubleLens_H
#define __DoubleLens_H

#include "../GlobalParameter.h"


#define DC_DubLens_CmdErr		0x80

#define DC_DubLens_CmdSet		0x20
#define DC_DubLens_CmdGet		0x10

#define DC_DubLens_DayLens		0x01
#define DC_DubLens_NightLens	0x02


extern uint8 gfDoubLens_ASG_TheLensState(uint8 iCmd);

extern int32 gfInitDrv_DoubleLens(uint8 iRemain);
extern int32 gfUninitDrv_DoubleLens(uint8 iRemain);

#endif