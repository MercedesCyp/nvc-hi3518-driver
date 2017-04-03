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
#include "../Tool/MsgProtocal.h"

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
#define DC_IfrFilt_TypeC_ShutdownPowerTime  5	//ms UNINT
#define DC_IfrFilt_TypeD_ShutdownPowerTime  5	//ms UNINT
#define DC_IfrFilt_TypeE_ShutdownPowerTime  5	//ms UNINT

//==============================================================================
//extern
//local
static void sfdefIfrFilt_TypeC_DisableModule(void);
static void sfdefIfrFilt_TypeD_DisableModule(void);

//
static int32 sfdefIfrFilt_Init(void);
static int32 sfdefIfrFilt_Uninit(void);
//
static void sfdefIfrFilt_SetStatus_InA(uint32);
static int32 sfdefIfrFilt_GetStatus_InA(void);
//
static void sfdefIfrFilt_SetStatus_C(uint32);
static int32 sfdefIfrFilt_GetStatus_C(void);
//
static void sfdefIfrFilt_SetStatus_D(uint32);
static int32 sfdefIfrFilt_GetStatus_D(void);

static int32 sfdefIRC_RegistCallBack(void (*iHandle)(uint32));
extern void (*gfIRCMonitor_ReportStateChange)(uint32 iStatus);

//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sdefPin_IfrFilt_TypeA[] = {
	{5, 3, 48, 0x00},
};

//global

mGPIOPinIfo sdefPin_IfrFilt_TypeB[] = {
	{7, 6, 66, 0x00},
	{7, 7, 67, 0x00},
};

mGPIOPinArr sdefPinArr_IfrFilt = {
	.apPin = sdefPin_IfrFilt_TypeA,
	.aNum = 1,
};

mClass_IfrFilter gClassIfrFilt = {
	.apPinArr = &sdefPinArr_IfrFilt,
	.afInit = sfdefIfrFilt_Init,
	.afUninit = sfdefIfrFilt_Uninit,
	.afRegCallBack = sfdefIRC_RegistCallBack,
	.aType = DC_IfrFltOpt_InTypeA,
	.aStatusChgType = DC_NVIfrFilt_Auto,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 	sfdefIfrFilt_Init(void)
@introduction:
    初始化红外滤波器设置

@parameter:

@return:


*/
static int32 sfdefIfrFilt_Init(void)
{
	uint8 _i;
	uint8 tPinNum = gClassIfrFilt.apPinArr->aNum;
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	for (_i = 0; _i < tPinNum; _i++) {
		gClassHAL.Pin->prfSetDirection(&tPinArr[_i], DC_HAL_PIN_SetOUT);
	}

	switch (gClassIfrFilt.aType) {
	case DC_IfrFltOpt_InTypeA:
		gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_InA;
		gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_InA;
		break;
	case DC_IfrFltOpt_TypeB:
		gClassHAL.Pin->prfSetExport(&tPinArr[1], DC_HAL_PIN_SetLOW);
		gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_InA;
		gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_InA;
		break;
	case DC_IfrFltOpt_TypeC:
		gClassHAL.Pin->prfSetExport(&tPinArr[2], DC_HAL_PIN_SetHIGH);
		gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_C;
		gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_C;
		break;
	case DC_IfrFltOpt_TypeD:
		gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetLOW);
		gClassHAL.Pin->prfSetExport(&tPinArr[1], DC_HAL_PIN_SetLOW);
		gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_D;
		gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_D;
		break;
	default:
		return -1;
	}

#if DEBUG_INIT
	NVCPrint("The Infrared Filter module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 	sfdefIfrFilt_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfdefIfrFilt_Uninit(void)
{
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void 	sfdefIfrFilt_SetStatus_InA(uint32 iCmd)
@introduction:
    TYPE A 设置 IRCUT 状态

@parameter:
    iCmd
        DC_IfrFlt_PassLi
            设置 IRCUT 通红外光
        DC_IfrFlt_BlockLi
            设置 IRCUT 阻挡红外光

@return:
    void

*/
static void sfdefIfrFilt_SetStatus_InA(uint32 iCmd)
{
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;
	if (iCmd & DC_IfrFlt_PassLi) {	//night
		if (!gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
			gClassHAL.Pin->prfSetExport(&tPinArr[1],
			                            DC_HAL_PIN_SetLOW);
			mdelay(200);
			gClassHAL.Pin->prfSetExport(&tPinArr[0],
			                            DC_HAL_PIN_SetHIGH);
			mdelay(700);
			gClassHAL.Pin->prfSetExport(&tPinArr[1],
			                            DC_HAL_PIN_SetHIGH);
		}
	} else if (iCmd & DC_IfrFlt_BlockLi) {	//day
		if (gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
			gClassHAL.Pin->prfSetExport(&tPinArr[1],
			                            DC_HAL_PIN_SetLOW);
			mdelay(200);
			gClassHAL.Pin->prfSetExport(&tPinArr[0],
			                            DC_HAL_PIN_SetLOW);
			mdelay(700);
			gClassHAL.Pin->prfSetExport(&tPinArr[1],
			                            DC_HAL_PIN_SetHIGH);
		}
	}
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefIfrFilt_GetStatus_InA(void)
@introduction:
    TYPE A 获取 IRCUT 状态

@parameter:
    void

@return:
    DC_IfrFlt_PassLi
    DC_IfrFlt_BlockLi

*/
static int32 sfdefIfrFilt_GetStatus_InA(void)
{
	int32 tRet = 0;
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	if (gClassHAL.Pin->prfGetPinValue(&tPinArr[0])) {
		tRet = DC_IfrFlt_PassLi;
	} else {
		tRet = DC_IfrFlt_BlockLi;
	}
	return tRet;
}

//---------- ---------- ---------- ----------
/*  static	void 	sfdefIfrFilt_SetStatus_C(uint32 iCmd )
@introduction:
    TYPE C 设置 IRCUT 状态
    这种切换 IRCUT 的方式没有硬件方面防止 IRcut 线圈切换完成后断电的机制，因此在
    等待工作完成后要手动的将 IRCUT 两条线的电平持平

@parameter:

@return:

*/
static void sfdefIfrFilt_SetStatus_C(uint32 iCmd)
{
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	gClassHAL.Pin->prfSetExport(&tPinArr[2], DC_HAL_PIN_SetLOW);

	if (iCmd & DC_IfrFlt_PassLi) {	//night
		gClassHAL.Pin->prfSetExport(&tPinArr[1], DC_HAL_PIN_SetLOW);
		gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetHIGH);

	} else if (iCmd & DC_IfrFlt_BlockLi) {	//day
		gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetLOW);
		gClassHAL.Pin->prfSetExport(&tPinArr[1], DC_HAL_PIN_SetHIGH);

	}
	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
	                                  sfdefIfrFilt_TypeC_DisableModule,
	                                  DC_IfrFilt_TypeC_ShutdownPowerTime);
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefIfrFilt_GetStatus_C(void)
@introduction:
    获取 IRCUT 的状态

@parameter:

@return:

*/
static int32 sfdefIfrFilt_GetStatus_C(void)
{
	int32 tRet = 0;
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	if ((gClassHAL.Pin->prfGetPinValue(&tPinArr[0]))
	    && (!gClassHAL.Pin->prfGetPinValue(&tPinArr[1]))) {

		tRet = DC_IfrFlt_PassLi;
	} else if ((!gClassHAL.Pin->prfGetPinValue(&tPinArr[0]))
	           && (gClassHAL.Pin->prfGetPinValue(&tPinArr[1]))) {

		tRet = DC_IfrFlt_BlockLi;
	}
	return tRet;
}

//---------- ---------- ---------- ----------
/*  static void sfdefIfrFilt_TypeC_DisableModule(void)
@introduction:
    在设置 IRCUT 的过程中，等到时间到来， 该函数会自动的切换至 IRCUT 不工作状态，
    并且从事件列表中将自己取消

@parameter:

@return:

*/
static void sfdefIfrFilt_TypeC_DisableModule(void)
{
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	gClassHAL.Pin->prfSetExport(&tPinArr[2], DC_HAL_PIN_SetHIGH);
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
	                                sfdefIfrFilt_TypeC_DisableModule);
}

//---------- ---------- ---------- ----------
/*  static	void 	sfdefIfrFilt_SetStatus_D(uint32 iCmd )
@introduction:
    同样同上，控制方式类似

@parameter:

@return:

*/
static void sfdefIfrFilt_SetStatus_D(uint32 iCmd)
{
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	if (iCmd & DC_IfrFlt_PassLi) {	//night
		gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetHIGH);
		// gClassHAL.Pin->prfSetExport( &tPinArr[1], DC_HAL_PIN_SetLOW );
	} else if (iCmd & DC_IfrFlt_BlockLi) {	//day
		// gClassHAL.Pin->prfSetExport( &tPinArr[0], DC_HAL_PIN_SetLOW );
		gClassHAL.Pin->prfSetExport(&tPinArr[1], DC_HAL_PIN_SetHIGH);
	}
	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
	                                  sfdefIfrFilt_TypeD_DisableModule,
	                                  DC_IfrFilt_TypeD_ShutdownPowerTime);
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefIfrFilt_GetStatus_D(void)
@introduction:

@parameter:

@return:

*/
static int32 sfdefIfrFilt_GetStatus_D(void)
{
	return DC_IfrFlt_NoSuppGet;
}

//---------- ---------- ---------- ----------
/*  static void sfdefIfrFilt_TypeD_DisableModule(void)
@introduction:

@parameter:

@return:

*/
static void sfdefIfrFilt_TypeD_DisableModule(void)
{
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;

	gClassHAL.Pin->prfSetExport(&tPinArr[0], DC_HAL_PIN_SetLOW);
	gClassHAL.Pin->prfSetExport(&tPinArr[1], DC_HAL_PIN_SetLOW);
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
	                                sfdefIfrFilt_TypeD_DisableModule);
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefIRC_RegistCallBack(void (*iHandle)(uint32))
@introduction:
    注册上报回调函数

@parameter:
    iHandle
        指向回调函数指针

@return:
    0   SUCCESS

*/
static int32 sfdefIRC_RegistCallBack(void (*iHandle)(uint32))
{
	gfIRCMonitor_ReportStateChange = iHandle;
	return 0;
}

//==============================================================================
//Others
