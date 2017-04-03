/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认：
    	红外灯只有一个控制引脚
    	引脚高电平为开灯
    	引脚低电平为关灯
    	默认引脚 GPIO0_0

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
static int32 sfdefIfrLi_Init(void);
static int32 sfdefIfrLi_Uninit(void);
static void sfdefIfrLi_SetStatus(uint32 iCmd);
static int32 sfdefIfrLi_GetStatus(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sdefPin_IfrLi = { 0, 0, 72, 0x00 };

static mGPIOPinArr sdefPinArr_IfrLi = {
	.apPin = &sdefPin_IfrLi,
	.aNum = 1
};

//global
mClass_IfrLi gClassIfrLi = {
	.apPinArr = &sdefPinArr_IfrLi,
	.afInit = sfdefIfrLi_Init,
	.afUninit = sfdefIfrLi_Uninit,
	.afSetStatus = sfdefIfrLi_SetStatus,
	.afGetStatus = sfdefIfrLi_GetStatus,
	.OnStatus = 0x01
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefIfrLi_Init(void)
@introduction:
    初始化红外灯

@parameter:

@return:


*/
static int32 sfdefIfrLi_Init(void)
{
	mGPIOPinIfo *tPin = gClassIfrLi.apPinArr->apPin;

	gClassHAL.Pin->prfSetDirection(tPin, DC_HAL_PIN_SetOUT);

	// DC_PIN_SET_IO_OUT( tPin->aGroup, tPin->aBit, tPin->aMuxVal, tPin->aMux );

#if DEBUG_INIT
	NVCPrint("The Infrared Light module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefIfrLi_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfdefIfrLi_Uninit(void)
{
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfdefIfrLi_SetStatus(uint32 iCmd)
@introduction:
    设置红外灯状态

@parameter:

@return:

*/
static void sfdefIfrLi_SetStatus(uint32 iCmd)
{

	mGPIOPinIfo *tPin = gClassIfrLi.apPinArr->apPin;
	if (iCmd & DC_IfLi_On) {
		//printk("------>ifr li on\r\n");
#if DEBUG_IFRLIGHT
		NVCPrint("IfrLight: Set ON");
#endif
		if (gClassIfrLi.OnStatus) {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetHIGH);
			// DF_Set_GPIO_Outx( tPin->aGroup, tPin->aBit );
		} else {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetLOW);
			// DF_Clr_GPIO_Outx( tPin->aGroup, tPin->aBit );
		}
	} else if (iCmd & DC_IfLi_Off) {
		//printk("------>ifr li off\r\n");
#if DEBUG_IFRLIGHT
		NVCPrint("IfrLight: Set OFF");
#endif
		if (gClassIfrLi.OnStatus) {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetLOW);
			// DF_Clr_GPIO_Outx( tPin->aGroup, tPin->aBit );
		} else {
			gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetHIGH);
			// DF_Set_GPIO_Outx( tPin->aGroup, tPin->aBit );
		}
	}
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefIfrLi_GetStatus(void)
@introduction:
    获取红外灯状态

@parameter:
    void

@return:
    DC_IfLi_On
        开
    DC_IfLi_Off
        关

*/
static int32 sfdefIfrLi_GetStatus(void)
{
	mGPIOPinIfo *tPin = gClassIfrLi.apPinArr->apPin;
	int32 tRet = 0;

	// if( DF_Get_GPIO_INx( tPin->aGroup, tPin->aBit ) ){
	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		if (gClassIfrLi.OnStatus) {
			tRet |= DC_IfLi_On;
		} else {
			tRet |= DC_IfLi_Off;
		}
	} else {
		if (gClassIfrLi.OnStatus) {
			tRet |= DC_IfLi_Off;
		} else {
			tRet |= DC_IfLi_On;
		}
	}
	return tRet;
}

//==============================================================================
//Others
