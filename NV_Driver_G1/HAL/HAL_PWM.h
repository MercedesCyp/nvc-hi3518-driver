#ifndef __HAL_PWM_H
#define __HAL_PWM_H

#include "../GlobalParameter.h"

extern int gfINIT_HAL_PWM(void);
extern int gfUNINIT_HAL_PWM(void);
extern int gfPWM_ModeSetWidth(uint8 iUnit,uint16 iWidth);

#endif