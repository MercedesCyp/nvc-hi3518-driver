#include "HAL.h"

#if defined(PLATFORM_3518)
extern mClass_AD 			const gcoClass_AD;
extern mClass_Pin 			const gcoClass_Pin;
extern mClass_PWM 			const gcoClass_PWM;
extern mClass_Timer 		const gcoClass_Timer;
extern mClass_PriodEvent 	const gcoClass_PeriodEvent;

mClass_Hal const gClassHAL={
	.Pin 			=	&gcoClass_Pin,
	.PWM			=	&gcoClass_PWM,
	.ADC			= 	&gcoClass_AD,
	.Timer			=	&gcoClass_Timer,
	.PeriodEvent	= 	&gcoClass_PeriodEvent,
};
#endif

#ifdef PLATFORM_3507
mClass_Hal const gClassHAL;
#endif
