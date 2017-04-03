#ifndef __StateLED_H
#define __StateLED_H

#include "../GlobalParameter.h"





// -------------------------------> macro
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--

#define DC_LED_Green1_B	0
#define DC_LED_Green2_B	1
#define DC_LED_Red1_B	2	

extern uint8 gLEDSumNum;


extern int32 gfInitDrv_StateLED(uint8 iRemain);
extern int32 gfUninitDrv_StateLED(uint8 iRemain);

extern int32 gfStaLED_GetLEDNumber(void);
extern int32 gfStaLED_SetLEDStatus(uint32 iPTime,uint32 iNTime,uint8 iSL_ID);


#endif

