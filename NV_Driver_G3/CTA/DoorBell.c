/*
Coder:      aojie.meng
Date:       2015-8-25

Abstract:
    提供控制门铃的方法

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
// DATA TYPE

//=============================================================================
// MACRO
// CONSTANT
#define DC_DoorBell_DriverWidth         10	// excitation width, unit 100ms/1
// FUNCTION

//==============================================================================
//extern
//local
static int32 sfdefDoorBell_Init(void);
static int32 sfdefDoorBell_Uninit(void);
static void sfdefDoorBell_SetStatus(uint32);
static int32 sfdefDoorBell_GetStatus(void);
static void sf_defDoorBell_ReleaseTiming(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sdefPin_DoorBell[] = {
	{7, 5, 49, 0x01},
};

static mGPIOPinArr sdefPinArr_DoorBell = {
	.apPin = sdefPin_DoorBell,
	.aNum = 1,
};

//global
mClass_DoorBell gClassDoorBell = {
	.apPinArr = &sdefPinArr_DoorBell,
	.afInit = sfdefDoorBell_Init,
	.afUninit = sfdefDoorBell_Uninit,
	.afSetStatus = sfdefDoorBell_SetStatus,
	.afGetStatus = sfdefDoorBell_GetStatus,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static 	int32 	sfdefDoorBell_Init(void)
@introduction:
    初始化门铃控制，常态要保证控制 IO 的电平为高

@parameter:
    void

@return:
    0		Success
    -1  	Fail

*/
static int32 sfdefDoorBell_Init(void)
{
	mGPIOPinIfo *tPinArr = gClassDoorBell.apPinArr->apPin;

	gClassHAL.Pin->prfSetDirection(&tPinArr[0], DC_HAL_PIN_SetOUT);

	gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetHIGH);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefDoorBell_Uninit(void)
@introduction:
    释放资源

@parameter:
    void

@return:
    0		Success
    -1  	Fail

*/
static int32 sfdefDoorBell_Uninit(void)
{

	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfdefDoorBell_SetStatus( uint32 iStatus )
@introduction:
    门铃的控制函数，发送一个,针对 F10 机型的控制方式，产生一个低电平激励，在规定
    时间后将控制接口释放。

@parameter:
    void

@return:
    void

*/
static void sfdefDoorBell_SetStatus(uint32 iStatus)
{
	mGPIOPinIfo *tPinArr = gClassDoorBell.apPinArr->apPin;

	if (iStatus & DC_CTADoorBell_TapBell) {
		gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetLOW);
		// printk("DoorBell excitation\r\n");
		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
		                                  sf_defDoorBell_ReleaseTiming,
		                                  DC_DoorBell_DriverWidth);
	}
}

static int32 sfdefDoorBell_GetStatus(void)
{
	int32 tRet = 0;
	mGPIOPinIfo *tPinArr = gClassDoorBell.apPinArr->apPin;

	if (gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
		tRet = DC_DoorBell_High;
	} else if (!gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
		tRet = DC_DoorBell_Low;
	}
	return tRet;
}

//---------- ---------- ---------- ----------
/*  static void sf_defDoorBell_ReleaseTiming(void)
@introduction:
    初始化门铃控制，常态要保证控制 IO 的电平为高

@parameter:
    void

@return:
    0		Success
    -1  	Fail

*/
static void sf_defDoorBell_ReleaseTiming(void)
{
	mGPIOPinIfo *tPinArr = gClassDoorBell.apPinArr->apPin;
	gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetHIGH);
	// printk("DoorBell Release\r\n");
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
	                                sf_defDoorBell_ReleaseTiming);
}

//==============================================================================
//Others
