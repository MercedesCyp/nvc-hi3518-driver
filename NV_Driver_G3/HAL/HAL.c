/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    进行不同平台的内核抽象层接口关联

*/
//==============================================================================
// C
// Linux
// local
#include "HAL.h"
// remote

//==============================================================================
//extern
extern mClass_PriodEvent const gcoClass_PeriodEvent;
// ------------------------------------------------     Hi3518
#if defined(PLATFORM_3518)
extern mClass_AD const gcoClass_AD;
extern mClass_Pin const gcoClass_Pin;
extern mClass_PWM const gcoClass_PWM;
extern mClass_Timer const gcoClass_Timer;
#endif
// ------------------------------------------------     Hi3518EV200
#if defined(PLATFORM_3518EV200)
extern mClass_AD const gcoClass_AD;
extern mClass_Pin const gcoClass_Pin;
extern mClass_PWM const gcoClass_PWM;
extern mClass_Timer const gcoClass_Timer;
#endif
// ------------------------------------------------     Hi3507
#ifdef PLATFORM_3507

extern mClass_Pin const gcoClass_Pin;
extern mClass_Timer const gcoClass_Timer;
#endif

//local
//global
// ------------------------------------------------     Hi3518
#if defined(PLATFORM_3518)
mClass_Hal const gClassHAL = {
	.Pin = &gcoClass_Pin,
	.PWM = &gcoClass_PWM,
	.ADC = &gcoClass_AD,
	.Timer = &gcoClass_Timer,
	.PeriodEvent = &gcoClass_PeriodEvent,
};
#endif
// ------------------------------------------------     Hi3518EV200
#if defined(PLATFORM_3518EV200)
mClass_Hal const gClassHAL = {
	.Pin = &gcoClass_Pin,
	.PWM = &gcoClass_PWM,
	.ADC = &gcoClass_AD,
	.Timer = &gcoClass_Timer,
	.PeriodEvent = &gcoClass_PeriodEvent,
};
#endif
// ------------------------------------------------     Hi3507
#ifdef PLATFORM_3507
mClass_Hal const gClassHAL = {
	.Pin = &gcoClass_Pin,
	// .PWM                 =       &gcoClass_PWM,
	// .ADC                 =       &gcoClass_AD,
	.Timer = &gcoClass_Timer,
	.PeriodEvent = &gcoClass_PeriodEvent,
};
#endif
