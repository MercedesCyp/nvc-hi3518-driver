/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
#include "CTA.h"
// remote
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"

//==============================================================================
//extern
extern int32 gfPCF8563_INIT(void);
extern int32 gfPCF8563_GetTime(mTime_Struct *iData);
extern void gfPCF8563_SetTime(mTime_Struct *iData);
//local
static int32 sfRTC_Uninit(void);
//global

//==============================================================================
//extern
//local
//global
mClass_RTC gClassRTC = {
	.afInit = gfPCF8563_INIT,
	.afUninit = sfRTC_Uninit,
	.afGetTime = gfPCF8563_GetTime,
	.afSetTime = gfPCF8563_SetTime,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfRTC_Uninit(void)
@introduction:

@parameter:

@return:


*/
static int32 sfRTC_Uninit(void)
{
	return 0;
}
