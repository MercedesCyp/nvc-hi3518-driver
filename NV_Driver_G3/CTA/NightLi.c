/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认：
    	小夜灯只有一个控制引脚
    	引脚高电平为开灯
    	引脚低电平为关灯
    	默认引脚 GPIO5_3

*/
//==============================================================================
// C
// Linux
// local
#include "CTA.h"
// remote
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"
#include "../Tool/String.h"

//==============================================================================
//extern
//local
static int32 sfdefNiLi_Init(void);
static int32 sfdefNiLi_Uninit(void);
static void sfdefNiLi_SetStatus(mClass_NiLiIfo *SetIfo);
static void sfdefNiLi_GetStatus(void);
//global

//==============================================================================
//extern
//local
static mClass_NiLiIfo sdefIfo_NiLi;
static mGPIOPinIfo sdefPin_NiLi = { 5, 1, 46, 0x00 };

static mGPIOPinArr sdefPinArr_NiLi = {
	.apPin = &sdefPin_NiLi,
	.aNum = 1
};

//global
mClass_NightLi gClassNiLi = {
	.apPinArr = &sdefPinArr_NiLi,
	.aIfo = &sdefIfo_NiLi,
	.afInit = sfdefNiLi_Init,
	.afUninit = sfdefNiLi_Uninit,
	.afSetStatus = sfdefNiLi_SetStatus,
	.afGetStatus = sfdefNiLi_GetStatus,
	.OnStatus = 0x01
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefNiLi_Init(void)
@introduction:
    初始化小夜灯

@parameter:
    void

@return:
    0   SUCCESS

*/

static int32 sfdefNiLi_Init(void)
{

	mGPIOPinIfo *tPin = gClassNiLi.apPinArr->apPin;

	gClassStr.afMemset((uint8 *) gClassNiLi.aIfo, 0x00,
	                   sizeof(mClass_NiLiIfo));

	// Setting IO dirction OUTPUT
	gClassHAL.Pin->prfSetDirection(tPin, DC_HAL_PIN_SetOUT);
	// DF_Set_GPIO_DIRx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );

#if DEBUG_INIT
	NVCPrint("The Night Light module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefNiLi_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfdefNiLi_Uninit(void)
{
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfdefNiLi_SetStatus(mClass_NiLiIfo *SetIfo)
@introduction:
    设置小夜灯的状态

@parameter:
    SetIfo
        控制状态
        亮度

@return:
    void

*/
static void sfdefNiLi_SetStatus(mClass_NiLiIfo *SetIfo)
{

	mGPIOPinIfo *tPin = gClassNiLi.apPinArr->apPin;
	uint8 tCmd = SetIfo->aStatus;

	if (tCmd & DC_NiLi_On) {
		if (gClassNiLi.OnStatus) {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetHIGH);
		} else {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetLOW);
		}
	} else if (tCmd & DC_NiLi_Off) {
		if (gClassNiLi.OnStatus) {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetLOW);
		} else {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetHIGH);
		}
	}
}

//---------- ---------- ---------- ----------
/*  static void sfdefNiLi_GetStatus(void)
@introduction:
    获取小夜灯状态

@parameter:
    void

@return:
    void

*/
static void sfdefNiLi_GetStatus(void)
{
	mGPIOPinIfo *tPin = gClassNiLi.apPinArr->apPin;

	gClassNiLi.aIfo->aStatus &= ~DC_NiLi_StaMask;
	// if( DF_Get_GPIO_INx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit ) ){
	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		if (gClassNiLi.OnStatus) {
			gClassNiLi.aIfo->aStatus |= DC_NiLi_On;
		} else {
			gClassNiLi.aIfo->aStatus |= DC_NiLi_Off;
		}
	} else {
		if (gClassNiLi.OnStatus) {
			gClassNiLi.aIfo->aStatus |= DC_NiLi_Off;
		} else {
			gClassNiLi.aIfo->aStatus |= DC_NiLi_On;
		}
	}

}
