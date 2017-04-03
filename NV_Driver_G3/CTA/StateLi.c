/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认:
        大部分产品都只用到一个状态灯
        但状态灯的控制IO有所不同

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
//#include "../HAL/HI3518/HI3518_HAL.h"

//=============================================================================
// DATA TYPE
//  the Every state light state buffer
typedef struct {
	mGPIOPinIfo *apPin;
	uint32 aPTime;		// positive time
	uint32 aNTime;		// negative time
	uint32 aCount;
	uint8 aState;
#define DC_SLCfg_On 	0x01
#define DC_SLCfg_Off 	0x00
} mSL_CfgParam;

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
#define DC_StaLi_LoopProcessingFrq	1	// 1ms

//==============================================================================
//extern
//local
static int32 sfdefStaLi_Init(void);
static int32 sfdefStaLi_Uninit(void);
static void sfdefStaLi_SetStatus(mStaLi_SetParam *iCmd);
static void sfStaLi_LoopProcessing(void);
#define DC_StaLi_OptCMD_Mask		0x03
#define DC_StaLi_OptCMD_On			0x01
#define DC_StaLi_OptCMD_Off 		0x02
#define DC_StaLi_OptCMD_Switch		0x03
#define DC_StaLi_OptCMD_GET			0x80

static uint32 sfdefStaLi_OptMode(uint32, mGPIOPinIfo *iPin);
#define DC_StaLi_OptRET_On			0x01
#define DC_StaLi_OptRET_Off			0x00
static uint32 sfdefStaLi_InOptMode(uint32, mGPIOPinIfo *iPin);
// static void sfdefStaLi_SetIOOuput(mGPIOPinIfo *ipPin);
//global

//==============================================================================
//extern
//local
static mSL_CfgParam *spStaLi_Info = NULL;
static uint8 sStaLi_Num = 0;
static mGPIOPinIfo sdefPin_StaLi[] = {
	{0, 3, 75, 0x00},
	{0, 4, 76, 0x00},
	{0, 2, 74, 0x00},
};

static mGPIOPinArr sdefPinArr_StaLi = {
	.apPin = sdefPin_StaLi,
	.aNum = 3
};

static uint32(*sfvStaLi_OptMode)(uint32, mGPIOPinIfo *);
//global
mClass_StateLi gClassStateLi = {
	.apPinArr = &sdefPinArr_StaLi,
	.afInit = sfdefStaLi_Init,
	.afUninit = sfdefStaLi_Uninit,
	.afSetStatus = sfdefStaLi_SetStatus,
	.OnStatus = 0x01,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefStaLi_Init(void)
@introduction:
    初始化状态灯

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefStaLi_Init(void)
{
	uint8 _i;
	uint8 tNum;
	sStaLi_Num = 0;
	tNum = gClassStateLi.apPinArr->aNum;
	// NVCPrint("%s: tNum = %d", __func__, tNum);
	spStaLi_Info =
	    (mSL_CfgParam *) kmalloc(sizeof(mSL_CfgParam) * tNum, GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) spStaLi_Info, 0x00,
	                   sizeof(mSL_CfgParam) * tNum);

	for (_i = 0; _i < tNum; _i++) {
		gClassHAL.Pin->
		prfSetDirection(&(gClassStateLi.apPinArr->apPin[_i]),
		                DC_HAL_PIN_SetOUT);
	}
	// sfdefStaLi_SetIOOuput( &(gClassStateLi.apPinArr->apPin[_i]) );

	if (gClassStateLi.OnStatus) {
		sfvStaLi_OptMode = sfdefStaLi_OptMode;
	} else {
		sfvStaLi_OptMode = sfdefStaLi_InOptMode;
	}

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_INT,
	                                  sfStaLi_LoopProcessing,
	                                  DC_StaLi_LoopProcessingFrq);

#if DEBUG_INIT
	NVCPrint("The StateLED module Start! OK");
#endif

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefStaLi_Uninit(void)
@introduction:
    释放状态灯初始化时候注册的资源

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefStaLi_Uninit(void)
{
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_INT, sfStaLi_LoopProcessing);
	kfree(spStaLi_Info);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void  sfdefStaLi_SetStatus(mStaLi_SetParam *iCmd)
@introduction:
    设置状态灯的状态

@parameter:
    iCmd
        指向状态灯设置状态信息对象的指针

@return:
    void

*/
static void sfdefStaLi_SetStatus(mStaLi_SetParam *iCmd)
{
	uint8 tNum;
	tNum = gClassStateLi.apPinArr->aNum;

	if (iCmd->aUnit < tNum) {
		mGPIOPinIfo *tTools;
		uint8 _i = 0;
		tTools = &(gClassStateLi.apPinArr->apPin[iCmd->aUnit]);

		while ((spStaLi_Info[_i].apPin != tTools) && (_i < sStaLi_Num)) {
			_i++;
		}

		if (iCmd->aPTime || iCmd->aNTime) {
			// mount state light task

			if (_i != sStaLi_Num) {
				spStaLi_Info[_i].aPTime = iCmd->aPTime;
				spStaLi_Info[_i].aNTime = iCmd->aNTime;
			} else {
				spStaLi_Info[sStaLi_Num].apPin = tTools;
				spStaLi_Info[sStaLi_Num].aPTime = iCmd->aPTime;
				spStaLi_Info[sStaLi_Num].aNTime = iCmd->aNTime;
				spStaLi_Info[sStaLi_Num].aCount = 0;
				spStaLi_Info[sStaLi_Num].aState = DC_SLCfg_Off;
				sStaLi_Num++;
				// NVCPrint("%s: sStaLi_Num = %d", __func__, sStaLi_Num);
			}

		} else {
			// unmount state light task

			if (_i != sStaLi_Num) {
				gClassHAL.PeriodEvent->
				afControl(DC_HAL_PE_INT_PUSE);
				// 在这里防止中断出现，应当隔离禁用系统中断
				for (; _i < (sStaLi_Num - 1); _i++) {
					spStaLi_Info[_i].apPin =
					    spStaLi_Info[_i + 1].apPin;
					spStaLi_Info[_i].aPTime =
					    spStaLi_Info[_i + 1].aPTime;
					spStaLi_Info[_i].aNTime =
					    spStaLi_Info[_i + 1].aNTime;
					spStaLi_Info[_i].aCount =
					    spStaLi_Info[_i + 1].aCount;
					spStaLi_Info[_i].aState =
					    spStaLi_Info[_i + 1].aState;
				}
				spStaLi_Info[_i].apPin = NULL;
				spStaLi_Info[_i].aPTime = 0;
				spStaLi_Info[_i].aNTime = 0;
				spStaLi_Info[_i].aCount = 0;
				spStaLi_Info[_i].aState = 0;

				sStaLi_Num--;
				gClassHAL.PeriodEvent->afControl
				(DC_HAL_PE_INT_RESUME);
			}

			sfvStaLi_OptMode(DC_StaLi_OptCMD_Off, tTools);
		}
	}
	// NVCPrint("%s: After: reg71 *(int *)0x1001011C = %d", __func__, *(int *)0x1001011C);
	//NVCPrint("%s: After: reg71 *(int *)0x1001011C = %d", __func__, HAL_readl(0x1001011C));

}

//---------- ---------- ---------- ----------
/*  static void	 sfStaLi_LoopProcessing(void)
@introduction:
    状态灯调度过程

@parameter:
    void

@return:
    void

*/
static void sfStaLi_LoopProcessing(void)
{
	uint8 _i;
	for (_i = 0; _i < sStaLi_Num; _i++) {
		if (spStaLi_Info[_i].aCount == 0) {
			if (spStaLi_Info[_i].aState & DC_SLCfg_On) {
				spStaLi_Info[_i].aState &= ~DC_SLCfg_On;
				if (spStaLi_Info[_i].aNTime > 0) {
					spStaLi_Info[_i].aCount =
					    spStaLi_Info[_i].aNTime;
					//set off
					sfvStaLi_OptMode(DC_StaLi_OptCMD_Off,
					                 spStaLi_Info
					                 [_i].apPin);
					// NVCPrint("%s: DC_StaLi_OptCMD_Off: HAL_readl(HAL_GPIOx_SDATA(%d, %d)) = 0X%x", __func__, spStaLi_Info[_i].apPin->aGroup, spStaLi_Info[_i].apPin->aBit, HAL_readl(HAL_GPIOx_SDATA(spStaLi_Info[_i].apPin->aGroup, spStaLi_Info[_i].apPin->aBit)));
					// NVCPrint("%s: DC_StaLi_OptCMD_Off: HAL_readl(HAL_GPIOx_DIR(%d)) = 0X%x", __func__, spStaLi_Info[_i].apPin->aGroup, HAL_readl(HAL_GPIOx_DIR(spStaLi_Info[_i].apPin->aGroup)));
				}
			} else {
				spStaLi_Info[_i].aState |= DC_SLCfg_On;
				if (spStaLi_Info[_i].aPTime > 0) {
					spStaLi_Info[_i].aCount =
					    spStaLi_Info[_i].aPTime;
					// set on
					sfvStaLi_OptMode(DC_StaLi_OptCMD_On,
					                 spStaLi_Info
					                 [_i].apPin);
					// NVCPrint("%s: DC_StaLi_OptCMD_On: HAL_readl(HAL_GPIOx_SDATA(%d, %d)) = 0X%x", __func__, spStaLi_Info[_i].apPin->aGroup, spStaLi_Info[_i].apPin->aBit, HAL_readl(HAL_GPIOx_SDATA(spStaLi_Info[_i].apPin->aGroup, spStaLi_Info[_i].apPin->aBit)));
					// NVCPrint("%s: DC_StaLi_OptCMD_On: HAL_readl(HAL_GPIOx_DIR(%d)) = 0X%x", __func__, spStaLi_Info[_i].apPin->aGroup, HAL_readl(HAL_GPIOx_DIR(spStaLi_Info[_i].apPin->aGroup)));
				}
			}
		}
		if (spStaLi_Info[_i].aCount > 0) {
			spStaLi_Info[_i].aCount--;
		}
	}

}

//---------- ---------- ---------- ----------
/*  static uint32 sfdefStaLi_OptMode(uint32 iCmd, mGPIOPinIfo *iPin )
@introduction:
    状态灯控制方式 1

@parameter:
    iCmd
        控制命令
    iPin
        控制引脚

@return:
    返回当前灯的状态

*/
static uint32 sfdefStaLi_OptMode(uint32 iCmd, mGPIOPinIfo *iPin)
{
	uint32 tRet = 0;
	switch (iCmd & DC_StaLi_OptCMD_Mask) {
	case DC_StaLi_OptCMD_On: {
		gClassHAL.Pin->prfSetExport(iPin, DC_HAL_PIN_SetHIGH);
	}
	break;
	case DC_StaLi_OptCMD_Off: {
		gClassHAL.Pin->prfSetExport(iPin, DC_HAL_PIN_SetLOW);
	}
	break;
	case DC_StaLi_OptCMD_Switch: {
		gClassHAL.Pin->prfSetExport(iPin, DC_HAL_PIN_SetSWITCH);
	}
	break;
	}

	if (iCmd & DC_StaLi_OptCMD_GET) {
		if (gClassHAL.Pin->prfGetPinValue(iPin)) {
			tRet |= DC_StaLi_OptRET_On;
		} else {
			tRet |= DC_StaLi_OptRET_Off;
		}
	}
	return tRet;
}

//---------- ---------- ---------- ----------
/*  static uint32 sfdefStaLi_InOptMode(uint32 iCmd, mGPIOPinIfo *iPin )
@introduction:
    状态灯控制方式 2

@parameter:
    iCmd
        控制命令
    iPin
        控制引脚

@return:
    返回当前灯的状态

*/
static uint32 sfdefStaLi_InOptMode(uint32 iCmd, mGPIOPinIfo *iPin)
{
	//inverse
	uint32 tRet = 0;
	switch (iCmd & DC_StaLi_OptCMD_Mask) {
	case DC_StaLi_OptCMD_On: {
		gClassHAL.Pin->prfSetExport(iPin, DC_HAL_PIN_SetLOW);
	}
	break;
	case DC_StaLi_OptCMD_Off: {
		gClassHAL.Pin->prfSetExport(iPin, DC_HAL_PIN_SetHIGH);
	}
	break;
	case DC_StaLi_OptCMD_Switch: {
		gClassHAL.Pin->prfSetExport(iPin, DC_HAL_PIN_SetSWITCH);
	}
	break;
	}

	if (iCmd & DC_StaLi_OptCMD_GET) {
		if (gClassHAL.Pin->prfGetPinValue(iPin)) {
			tRet |= DC_StaLi_OptRET_Off;
		} else {
			tRet |= DC_StaLi_OptRET_On;
		}
	}
	return tRet;
}
