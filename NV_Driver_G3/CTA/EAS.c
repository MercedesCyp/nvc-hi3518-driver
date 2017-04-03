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
#include "../Tool/String.h"
#include "../GlobalParameter.h"

//=============================================================================
// MACRO
// CONSTANT
#define DC_EAS_CheckFreq            10	// 1s/times
// FUNCTION

//==============================================================================
//extern
//local
static int32 sfdefEAS_Init(void);
static int32 sfdefEAS_Uninit(void);
static void sfdefEAS_MotorCheck(void);
static void sfdefEAS_RegsiterCallBack(void (*Handle)(void));
static int32 sfdefEAS_GetStatus(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sdefPin_EAS[] = {
	{5, 4, 48, 0x00},
};

static mGPIOPinArr sdefPinArr_EAS = {
	.apPin = sdefPin_EAS,
	.aNum = 1,
};

static void (*sfEASMonitor_ReportStateChange)(void) = NULL;
//global
mClass_EAS gClassEAS = {
	.aDefSta = 0x00,
	.apPinArr = &sdefPinArr_EAS,
	.afInit = sfdefEAS_Init,
	.afUninit = sfdefEAS_Uninit,
	.afGetStatus = sfdefEAS_GetStatus,
	.afRegCallBack = sfdefEAS_RegsiterCallBack,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static 	int32	sfdefButton_Init(void)
@introduction:
    EAS initialization

@parameter:
    void

@return:
    0   SUCCESS
    -1  FAIL
*/
static int32 sfdefEAS_Init(void)
{
	mGPIOPinIfo *tPin = gClassEAS.apPinArr->apPin;
	gClassHAL.Pin->prfSetDirection(tPin, DC_HAL_PIN_SetIN);

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT, sfdefEAS_MotorCheck,
	                                  DC_EAS_CheckFreq);

#if DEBUG_INIT
	NVCPrint("The EAS module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/* static 	int32	sfdefEAS_Uninit(void)
@introduction:
    EAS initialization

@parameter:
    void

@return:
    0   SUCCESS
    -1  FAIL
*/
static int32 sfdefEAS_Uninit(void)
{
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sfdefEAS_MotorCheck);
	return 0;
}

//---------- ---------- ---------- ----------
/* static void sfdefEAS_MotorCheck(void)
@introduction:
    EAS motor check function

@parameter:
    void

@return:
    void
*/
static void sfdefEAS_MotorCheck(void)
{
	mGPIOPinIfo *tPin = gClassEAS.apPinArr->apPin;
	if (gClassHAL.Pin->prfGetPinValue(tPin) == gClassEAS.aDefSta) {
		return;
	} else {
		if (sfEASMonitor_ReportStateChange) {
			sfEASMonitor_ReportStateChange();
		}
	}
}

//---------- ---------- ---------- ----------
/* static int32 sfdefEAS_GetStatus(void)
@introduction:
    GET EAS status

@parameter:
    void

@return:
    DC_EASSta_Safty
    DC_EASSta_Emergency
*/
static int32 sfdefEAS_GetStatus(void)
{
	int32 status;
	mGPIOPinIfo *tPin = gClassEAS.apPinArr->apPin;
	if (gClassHAL.Pin->prfGetPinValue(tPin) == gClassEAS.aDefSta) {
		// normal
		status = DC_EASSta_Safty;
	} else {
		// emegency
		status = DC_EASSta_Emergency;
	}
	return status;
}

//---------- ---------- ---------- ----------
/* static void sfdefEAS_RegsiterCallBack( void(*Handle)(void) )
@introduction:
    Register EAS notify emergency message

@parameter:
    void

@return:
    0   SUCCESS
    -1  FAIL
*/
static void sfdefEAS_RegsiterCallBack(void (*Handle)(void))
{
	sfEASMonitor_ReportStateChange = Handle;
}
