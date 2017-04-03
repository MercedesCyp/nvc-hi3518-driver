/*
Coder:      yan.xu
Date:       2015-12-25

Abstract:
    默认:
    	门锁控制只有一个引脚
    	引脚低电平为开
    	引脚高电平为关
    	默认引脚 GPIO 5——7

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
//local
static int32 sfdefDoorLock_Init(void);
static int32 sfdefDoorLock_Uninit(void);
static void sfdefDoorLock_SetStatus(uint32 iCmd);
static int32 sfdefDoorLock_GetStatus(void);

static mGPIOPinIfo sdefPin_DoorLock = { 5, 7, 71, 0 };

static mGPIOPinArr sdefPinArr_DoorLock = {
	.apPin = &sdefPin_DoorLock,
	.aNum = 1
};

//global
mClass_DoorLock gClassDoorLock = {
	.apPinArr = &sdefPinArr_DoorLock,
	.afInit = sfdefDoorLock_Init,
	.afUninit = sfdefDoorLock_Uninit,
	.afSetStatus = sfdefDoorLock_SetStatus,
	.afGetStatus = sfdefDoorLock_GetStatus,
};

//==============================================================================
//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefDoorLock_Init(void)
@introduction:
    初始化门锁对象

@parameter:
    void

@return:
    void
        初始化成功

*/
static int32 sfdefDoorLock_Init(void)
{

	// The default Control Pin Num of Door Lock is just one.
	// uint8                        tPinNum = gClassDoorLock->apPinArr->aNum;
	mGPIOPinIfo *tPin = gClassDoorLock.apPinArr->apPin;

	// Initial multi-Register
	// Setting IO direction OUTPUT
	gClassHAL.Pin->prfSetDirection(&tPin[0], DC_HAL_PIN_SetOUT);

#if DEBUG_INIT
	NVCPrint("The Door Lock module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefDoorLock_Uninit(void)
@introduction:

@parameter:

@return:


*/
static int32 sfdefDoorLock_Uninit(void)
{
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfdefDoorLock_SetStatus(uint32 iCmd)
@introduction:
    音频配置

@parameter:
    iCmd
        DC_DoorLock_Open
            门锁开
        DC_DoorLock_Close
            门锁关

@return:
    -2  命令不支持
    0   成功

*/
static void sfdefDoorLock_SetStatus(uint32 iCmd)
{

	mGPIOPinIfo *tPin = gClassDoorLock.apPinArr->apPin;

	if (iCmd & DC_DoorLock_Open) {
		gClassHAL.Pin->prfSetExport(&tPin[0], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&tPin[0], DC_HAL_PIN_SetLOW);
	}
}

static int32 sfdefDoorLock_GetStatus(void)
{
	int32 tRet = 0;
	mGPIOPinIfo *tPinArr = gClassDoorLock.apPinArr->apPin;

	if (gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
		tRet = DC_DoorLock_Open;
	} else if (!gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
		tRet = DC_DoorLock_Close;
	}
	return tRet;
}
