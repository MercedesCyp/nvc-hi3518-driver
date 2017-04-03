/*
Coder:      aojie.meng
Date:       2015-8-25

Abstract:
    热设红外，移动侦测

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
// The time of PIR stop work after deetcted a activity. (unit 100ms/1)
#define DC_defPIR_DeadTimeInterval  (30)
// PIR triger time, from triger on to triger off. (unit 10ms/1)
#define DC_defPIR_TriggerTimeVal		(15)
// PIR trigger need report status
#define DC_PIR_TriggerNeedReport	1
#define DC_PIR_TriggerNoNeedReport	0

// PIR triger status: On--Low level, Off--High level
#define DC_defPIR_TriggerStatusOn	0
#define DC_defPIR_TriggerStatusOff	1

// FUNCTION

//==============================================================================
//extern
//local
static int32 sfdefPIR_Init(void);
static int32 sfdefPIR_Uninit(void);
static int32 sfdefPIR_GetStatus(void);
static void sfdefPIR_SetDeadTime(uint32 iDeadTime);
static void sfdefPIR_RegReportCall(void (*iHandler)(uint32));
static void sfdefPIR_ImportInt(void);
static void sfdefPIR_ImportIntTrigger(void);
static void sfdefPIR_ResponTimerToReport(void);
static void sfdefPIR_ResponTimerToReleasePIRInt(void);
//global

//==============================================================================
//extern
//local
static mPIR_Info sdefPIR_Ifo[] = {
	{0, DC_defPIR_DeadTimeInterval, DC_DefPIR_OriHIGH},
};

static mGPIOPinIfo sdefPin_PIR[] = {
	{1, 0, 0, 0x00},
};

static mGPIOPinArr sdefPinArr_PIR = {
	.apPin = sdefPin_PIR,
	.aNum = 1
};

static void (*sfpdefPIR_ReportCall)(uint32) = NULL;
//global
mClass_PIR gClassPIR = {
	.apIfoArr = sdefPIR_Ifo,
	.apPinArr = &sdefPinArr_PIR,
	.afInit = sfdefPIR_Init,
	.afUninit = sfdefPIR_Uninit,
	.afGetStatus = sfdefPIR_GetStatus,
	.afSetDeadTime = sfdefPIR_SetDeadTime,
	.afRegCallBack = sfdefPIR_RegReportCall,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefPIR_Init(void)
@introduction:
    初始化 PIR 硬件配置

@parameter:
    void

@return:
    void

*/
static int32 sfdefPIR_Init(void)
{
	mGPIOPinIfo *tPin = gClassPIR.apPinArr->apPin;
	mPIR_Info *tIfo = gClassPIR.apIfoArr;

	gClassHAL.Pin->prfPin_RegistInt(DC_HAL_PIN_Grp0, sfdefPIR_ImportInt);

	if (tIfo->aConf & DC_DefPIR_OriLOW)
		gClassHAL.Pin->prfPin_ConfigInt(tPin, DC_HAL_PIN_IntFallEdge
		                                | DC_HAL_PIN_IntIN
		                                | DC_HAL_PIN_IntClrState
		                                | DC_HAL_PIN_IntEnable);
	else {
		gClassHAL.Pin->prfPin_ConfigInt(tPin, DC_HAL_PIN_IntRiseEdge
		                                | DC_HAL_PIN_IntIN
		                                | DC_HAL_PIN_IntClrState
		                                | DC_HAL_PIN_IntEnable);
	}
#if DEBUG_INIT
	NVCPrint("The PIR module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static 	int32 	sfdefPIR_Uninit(void)
@introduction:
    释放资源

@parameter:
    void

@return:
    void

*/
static int32 sfdefPIR_Uninit(void)
{
	gClassHAL.Pin->prfPin_CancelRegistInt(DC_HAL_PIN_Grp0,
	                                      sfdefPIR_ImportInt);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static	int32 	sfdefPIR_GetStatus(void)
@introduction:
    为 PIR 注册用于上报的函数

@parameter:
    iHandler
        指向上报处理函数的上报指针

@return:
    void

*/
static int32 sfdefPIR_GetStatus(void)
{
	mPIR_Info *tIfo = gClassPIR.apIfoArr;
#if DEBUG_PIR
	NVCPrint("PIR: You get sum times is:%d", (int)tIfo->aCount);
#endif
	return tIfo->aCount;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPIR_SetDeadTime( uint32 iDeadTime )
@introduction:
    设置死区时间

@parameter:
    iDeadTime
        死去时间，单位 0.1s

@return:
    void

*/
static void sfdefPIR_SetDeadTime(uint32 iDeadTime)
{
	mPIR_Info *tIfo = gClassPIR.apIfoArr;
	tIfo->aDeadTime = iDeadTime;
	return;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPIR_RegReportCall( void (*iHandler)(uint32) )
@introduction:
    为 PIR 注册用于上报的函数

@parameter:
    iHandler
        指向上报处理函数的上报指针

@return:
    void

*/
static void sfdefPIR_RegReportCall(void (*iHandler)(uint32))
{
	sfpdefPIR_ReportCall = iHandler;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPIR_RegReportCall( void (*iHandler)(uint32) )
@introduction:
    PIR 检测中断响应函数
	判断PIR触发时间是否符合要求，不符合要求的触发视为干扰
@parameter:
    void

@return:
    void

*/
static void sfdefPIR_ImportInt(void)
{
	mGPIOPinIfo *tPin = gClassPIR.apPinArr->apPin;
	mPIR_Info *tIfo = gClassPIR.apIfoArr;
	static uint32 stTriggerTimeout = 0;
	static uint32 stTriggerStartTime = 0;
	static uint32 stTriggerStopTime = 0;
	uint8 tTriggerStatus = DC_defPIR_TriggerStatusOff;
	uint8 tIsTriggerReport = DC_PIR_TriggerNoNeedReport;
#if DEBUG_PIR
	uint32 tTriggerTime = 0;
#endif

	if (gClassHAL.Pin->prfIsIntStateExist(tPin)) {
		tIfo->aCount++;

		if (gClassHAL.Pin->prfGetPinValue(tPin)) {	// PIR Trigger over, level Up
			gClassHAL.Pin->prfPin_ConfigInt((tPin),
			                                DC_HAL_PIN_IntFallEdge);
			stTriggerStopTime = jiffies;
			if (time_after(stTriggerStopTime, stTriggerTimeout)) {
				tIsTriggerReport = DC_PIR_TriggerNeedReport;
			}
			tTriggerStatus = DC_defPIR_TriggerStatusOff;
			stTriggerTimeout = 0;
		} else {	// PIR Trigger, level Down
			gClassHAL.Pin->prfPin_ConfigInt((tPin),
			                                DC_HAL_PIN_IntRiseEdge);
			stTriggerStartTime = jiffies;
			stTriggerTimeout = stTriggerStartTime + DC_defPIR_TriggerTimeVal;
			tIsTriggerReport = DC_PIR_TriggerNoNeedReport;
			tTriggerStatus = DC_defPIR_TriggerStatusOn;
		}

		if ((tTriggerStatus == DC_defPIR_TriggerStatusOff)
		    && (tIsTriggerReport == DC_PIR_TriggerNeedReport)) {
			tIsTriggerReport = DC_PIR_TriggerNoNeedReport;
			sfdefPIR_ImportIntTrigger();
#if DEBUG_PIR
			tTriggerTime = stTriggerStopTime - stTriggerStartTime;
			NVCPrint("%s: PIR Trigger time = %lu", __func__, tTriggerTime);
#endif
		}
	}

}

//---------- ---------- ---------- ----------
/*  static void sfdefPIR_ImportIntTrigger(void)
@introduction:
    PIR 触发后（已经排除干扰）开始上报PIR 触发事件

@parameter:
    void

@return:
    void

*/
static void sfdefPIR_ImportIntTrigger(void)
{
	mGPIOPinIfo *tPin = gClassPIR.apPinArr->apPin;

	gClassHAL.Pin->prfPin_ConfigInt(tPin,
	                                DC_HAL_PIN_IntClrState
	                                | DC_HAL_PIN_IntDisable);

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
	                                  sfdefPIR_ResponTimerToReport, 1);
}

//---------- ---------- ---------- ----------
/*  static void sfdefPIR_ResponTimerToReport(void)
@introduction:
    被中断注册的用于上报检测到移动侦测的方法

@parameter:

@return:

*/
static void sfdefPIR_ResponTimerToReport(void)
{
	mGPIOPinIfo *tPin = gClassPIR.apPinArr->apPin;
	mPIR_Info *tIfo = gClassPIR.apIfoArr;

#if DEBUG_PIR
	NVCPrint("PIR: Detect once activity!");
#endif

	if (sfpdefPIR_ReportCall != NULL) {
		sfpdefPIR_ReportCall(DC_PIR_Discover_Unnormal);
	}

	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
	                                sfdefPIR_ResponTimerToReport);

	if (tIfo->aDeadTime == 0)
		gClassHAL.Pin->prfPin_ConfigInt(tPin,
		                                DC_HAL_PIN_IntClrState
		                                | DC_HAL_PIN_IntEnable);
	else
		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
		                                  sfdefPIR_ResponTimerToReleasePIRInt,
		                                  tIfo->aDeadTime);

}

//---------- ---------- ---------- ----------
/*  static void sfdefPIR_ResponTimerToReleasePIRInt(void)
@introduction:
    被注册的定时事件，用于重新恢复 PIR 的移动检测功能。

@parameter:

@return:

*/
static void sfdefPIR_ResponTimerToReleasePIRInt(void)
{
	mGPIOPinIfo *tPin = gClassPIR.apPinArr->apPin;
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
	                                sfdefPIR_ResponTimerToReleasePIRInt);

	gClassHAL.Pin->prfPin_ConfigInt(tPin,
	                                DC_HAL_PIN_IntClrState
	                                | DC_HAL_PIN_IntEnable);
	return;
}

//==============================================================================
//Others
