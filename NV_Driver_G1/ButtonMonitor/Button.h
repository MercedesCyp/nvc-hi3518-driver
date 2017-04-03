#ifndef __BUTTON_H
#define __BUTTON_H

#include "./../HAL/USERHAL.h"
#include "./../GlobalParameter.h"

//static uint8 sNSB_Start;
#define DC_NSB_UnitMask		0x00FF0000
#define DC_NSB_ErrorMask	0xFF000000

#define DC_NSB_Int 			0x80
#define DC_NSB_OriLeve		0x40
#define DC_NSB_Slow1		0x20	
	
#define DC_NSB_RasingEdge	0x04
#define DC_NSB_FallingEdge	0x02

#define DC_NSB_ButtonUp		0x01
#define DC_NSB_ButtonDown	0x00


extern int32 gfButton_GetButtonNumber(void);
extern uint8 gfDrv_ButtonStatus(uint8 iUnit,uint8 icmd);
extern int32 gfInitDrv_ButtonMonitor(uint8 iRemain);
extern int32 gfUninitDrv_ButtonMonitor(uint8 iRemain);

#endif


