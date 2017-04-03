#ifndef __AudioPlug_H
#define __AudioPlug_H
#include "../GlobalParameter.h"




#define DC_AuPl_CmdSet	0x10
#define DC_AuPl_CmdGet	0x20


#define DC_AuPl_Off		0x00
#define DC_AuPl_On		0x01

extern uint8 gfDrv_AudioPlugStatus(uint8 icmd);

extern int32 gfInitDrv_AudioPlug(uint8 iRemain);
extern int32 gfUninitDrv_AudioPlug(uint8 iRemain);



#endif