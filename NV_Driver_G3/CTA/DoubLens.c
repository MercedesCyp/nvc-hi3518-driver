/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    注意:
    	暂且在此功能模块只适用 D11 3518C 的机型，默认的控制引脚数组里
    	0	代表控制日用镜头的引脚	1	代表控制晚上夜用镜头的引脚
    	所以万一镜头控制方式一样，但引脚有差别的话，只要替换
    	gClassIfrFilt.apPinArr->apPin 指向的引脚数组即可

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

//==============================================================================
//extern
//local
static int32 sfdefDoubLens_Init(void);
static int32 sfdefDoubLens_Uninit(void);
static void sfdefDoubLens_SetStatus(uint32 iCmd);
static int32 sfdefDoubLens_GetStatus(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sdefPin_DoubleLens[] = {
	{3, 1, 13, 0x00},
	{3, 0, 12, 0x00},
};

static mGPIOPinArr sdefPinArr_DoubLens = {
	.apPin = sdefPin_DoubleLens,
	.aNum = 2,
};

//global
mClass_DubLens gClassDouobLens = {
	.apPinArr = &sdefPinArr_DoubLens,
	.afInit = sfdefDoubLens_Init,
	.afUninit = sfdefDoubLens_Uninit,
	.afSetStatus = sfdefDoubLens_SetStatus,
	.afGetStatus = sfdefDoubLens_GetStatus,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static	int32	sfdefDoubLens_Init(void)
@introduction:
    初始化

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefDoubLens_Init(void)
{
	mGPIOPinIfo *tpDoubLensPin;
	tpDoubLensPin = gClassDouobLens.apPinArr->apPin;

	gClassHAL.Pin->prfSetDirection(&tpDoubLensPin[0], DC_HAL_PIN_SetOUT);
	gClassHAL.Pin->prfSetDirection(&tpDoubLensPin[1], DC_HAL_PIN_SetOUT);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefDoubLens_Uninit(void)
@introduction:

@parameter:

@return:


*/
static int32 sfdefDoubLens_Uninit(void)
{
	return 0;
}

//---------- ---------- ---------- ----------
/*  static	void	sfdefDoubLens_SetStatus(uint32 iCmd)
@introduction:
    设置双镜头状态

@parameter:

@return:


*/
static void sfdefDoubLens_SetStatus(uint32 iCmd)
{
	mGPIOPinIfo *tpDoubLensPin;
	tpDoubLensPin = gClassDouobLens.apPinArr->apPin;

	if (iCmd & DC_DuLens_DayLens) {
		gClassHAL.Pin->prfSetExport(&tpDoubLensPin[1],
		                            DC_HAL_PIN_SetLOW);
		gClassHAL.Pin->prfSetExport(&tpDoubLensPin[0],
		                            DC_HAL_PIN_SetHIGH);

	} else if (iCmd & DC_DuLens_NightLens) {
		gClassHAL.Pin->prfSetExport(&tpDoubLensPin[0],
		                            DC_HAL_PIN_SetLOW);
		gClassHAL.Pin->prfSetExport(&tpDoubLensPin[1],
		                            DC_HAL_PIN_SetHIGH);
	}
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefDoubLens_GetStatus(void)
@introduction:
    获取双镜头状态

@parameter:

@return:


*/
static int32 sfdefDoubLens_GetStatus(void)
{
	int32 tRet = 0;
	mGPIOPinIfo *tpDoubLensPin = gClassDouobLens.apPinArr->apPin;

	uint8 tDPin = gClassHAL.Pin->prfGetPinValue(&tpDoubLensPin[0]);
	uint8 tNPin = gClassHAL.Pin->prfGetPinValue(&tpDoubLensPin[1]);

	if ((tDPin == 1) && (tNPin == 0)) {
		tRet = DC_DuLens_DayLens;
	} else if ((tDPin == 0) && (tNPin == 1)) {
		tRet = DC_DuLens_NightLens;
	}

	return tRet;
}

//==============================================================================
//Others
