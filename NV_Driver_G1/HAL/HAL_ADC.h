#ifndef __HAL_ADC_H
#define __HAL_ADC_H

#include "../GlobalParameter.h"

#define DC_ADC_Channel_0	0x00
#define DC_ADC_Channel_1	0x01


#define DC_ADC_UserID_For_LS 0x00 //light sensor
#define DC_ADC_UserID_For_TS 0x01 //Temperature sensor



extern int32 gfINIT_HAL_ADC(void);
extern int32 gfUNINIT_HAL_ADC(void);

extern uint16 gfADC_GetAndStartConvert(uint8 iChannel);




#endif