/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认:
    	音频控制只有一个引脚
    	引脚低电平为开
    	引脚高电平为关
    	默认引脚 GPIO 0——6

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
static int32 sfdefAuPl_Init(void);
static int32 sfdefAuPl_Uninit(void);
static int32 sfdefAuPl_SetStatus(uint32 iCmd);
static int32 sfdefAuPl_GetStatus(uint8 iCmd);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sdefPin_AuPl = { 0, 6, 78, 0 };

static mGPIOPinArr sdefPinArr_AuPl = {
	.apPin = &sdefPin_AuPl,
	.aNum = 1
};

//global
mClass_AudioPlug gClassAudioPlug = {
	.apPinArr = &sdefPinArr_AuPl,
	.afInit = sfdefAuPl_Init,
	.afUninit = sfdefAuPl_Uninit,
	.afSetStatus = sfdefAuPl_SetStatus,
	.afGetStatus = sfdefAuPl_GetStatus,
	.OnStatus = 0x00
};

//==============================================================================
//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefAuPl_Init(void)
@introduction:
    初始化音频对象

@parameter:
    void

@return:
    void
        初始化成功

*/
static int32 sfdefAuPl_Init(void)
{

	// The default Control Pin Num of Audio Plug is just one.
	// uint8                        tPinNum = gClassAudioPlug->apPinArr->aNum;
	mGPIOPinIfo *tPin = gClassAudioPlug.apPinArr->apPin;
	uint8 _i = 0;
	// for( _i = 0; _i < tPinNum: _i++ ){

	// Initial multi-Register
	// Setting IO dirction OUTPUT
	gClassHAL.Pin->prfSetDirection(&tPin[_i], DC_HAL_PIN_SetOUT);
	// DF_Set_GPIO_MUXx( tPin[_i].aMuxVal, tPin[_i].aMux );
	// DF_Set_GPIO_DIRx( tPin[_i].aGroup, tPin[_i].aBit );

	// Set the pin to hight, cut off the Audio access
	if (gClassAudioPlug.OnStatus) {
		gClassHAL.Pin->prfSetExport(&tPin[_i], DC_HAL_PIN_SetLOW);
		// DF_Clr_GPIO_Outx( tPin[_i].aGroup, tPin[_i].aBit );
	} else {
		gClassHAL.Pin->prfSetExport(&tPin[_i], DC_HAL_PIN_SetHIGH);
		// DF_Set_GPIO_Outx( tPin[_i].aGroup, tPin[_i].aBit );
	}
	// }
#if DEBUG_INIT
	NVCPrint("The Audio Plug module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefAuPl_Uninit(void)
@introduction:

@parameter:

@return:


*/
static int32 sfdefAuPl_Uninit(void)
{
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefAuPl_SetStatus(uint32 iCmd)
@introduction:
    音频配置

@parameter:
    iCmd
        DC_AuPl_Speaker_On
            扬声器开
        DC_AuPl_Speaker_Off
            扬声器关

@return:
    -2  命令不支持
    0   成功

*/

static int32 sfdefAuPl_SetStatus(uint32 iCmd)
{

	mGPIOPinIfo *tPin = gClassAudioPlug.apPinArr->apPin;

	if (iCmd & DC_AuPl_Speaker_On) {
		if (gClassAudioPlug.OnStatus) {
			gClassHAL.Pin->prfSetExport(&tPin[0],
			                            DC_HAL_PIN_SetHIGH);
		} else {
			gClassHAL.Pin->prfSetExport(&tPin[0],
			                            DC_HAL_PIN_SetLOW);
		}
	} else if (iCmd & DC_AuPl_Speaker_Off) {
		if (gClassAudioPlug.OnStatus) {
			gClassHAL.Pin->prfSetExport(&tPin[0],
			                            DC_HAL_PIN_SetLOW);
		} else {
			gClassHAL.Pin->prfSetExport(&tPin[0],
			                            DC_HAL_PIN_SetHIGH);
		}
	} else {
		return -2;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefAuPl_GetStatus(uint8 iCmd)
@introduction:
    获取音频状态

@parameter:
    iCmd
        获取对象

@return:
        对象状态
    -2  对象不支持

*/
static int32 sfdefAuPl_GetStatus(uint8 iCmd)
{
	mGPIOPinIfo *tPin = gClassAudioPlug.apPinArr->apPin;
	int32 tRet = 0;

	if (iCmd == DC_AuPl_Microph) {
		return -2;
	}
	if (gClassHAL.Pin->prfGetPinValue(&tPin[0])) {
		if (gClassAudioPlug.OnStatus) {
			tRet |= DC_AuPl_Speaker_On;
		} else {
			tRet |= DC_AuPl_Speaker_Off;
		}
	} else {
		if (gClassAudioPlug.OnStatus) {
			tRet |= DC_AuPl_Speaker_Off;
		} else {
			tRet |= DC_AuPl_Speaker_On;
		}
	}
	return tRet;
}
