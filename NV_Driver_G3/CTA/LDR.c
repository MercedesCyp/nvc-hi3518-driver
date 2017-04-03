/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认：
    	AD通道0采集光敏电阻的阻值
    	当值低于32时判定为晚上
    	值高于64时判定为白天

    Watch out:
        因为 Status 未初始状态是 Day | Night；所以在启用该模块的时候要特别注意
        先初始化好机器状态，否则状态可能是混乱的

*/
//==============================================================================
// C
// Linux
// local
#include "CTA.h"
// remote
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"
#include "../Pro/ProInclude.h"

//=============================================================================
// DATA TYPE
typedef struct {
	uint16 aDLimit;
	uint16 aNLimit;
	uint16 aLSValue;
	uint8 aStatus;
#define DC_DNMonitor_DAY 			0x01
#define DC_DNMonitor_NIGHT 			0x02
	uint8 aAdjUnit;
} mDNMonitor;

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
#define DC_LDR_CheckFreq		10	// 1s/times

#define DC_LDR_FilterBreakLimit		100

#define DC_LDR_FilterBreakTimes		5

#define DC_LDR_FilterOrder1Steps	5

#define DC_LDR_DLim			100
#define DC_LDR_NLim			32

#define DC_LDR_DEF_MAXSAM		1023

//==============================================================================
//extern
//local
static int32 sfdefLDR_Init(void);
static int32 sfdefLDR_Uninit(void);
static int32 sfdefLDR_GetStatus(void);
static int32 sfdefLDR_RegistCallBack(void (*iHandle)(uint8));
static int32 sfdefLDR_GetSensitive(uint8 *, uint8 *);
static int32 sfdefLDR_SetSensitive(uint8, uint8);
static int32 sfdefLDR_SetDNLimit(uint16, uint16);
static void sfLDR_MotorCheck(void);
//global
void sfLDR_Judge(uint32 iVal);
void sfLDR_FilterBuf_Assimilate(uint32 iVal);
uint32 sfLDR_Filter(uint32 iVal);

//==============================================================================
//extern

//local
static mGPIOPinArr sdefPinArr_LDR;
static mDNMonitor sDNMonitor = {
	.aDLimit = DC_LDR_DLim,
	.aNLimit = DC_LDR_NLim,
	.aStatus = DC_DNMonitor_DAY | DC_DNMonitor_NIGHT,
	.aAdjUnit = DC_LDR_DEF_MAXSAM / 100 + 1,
};

static uint32 u32LDRInitFlag = 0;

static uint32 saFltOrder1[DC_LDR_FilterOrder1Steps] = { 0 };

static void (*gfDNMonitor_ReportStateChange)(uint8 iStatus) = NULL;
void (*gfIRCMonitor_ReportStateChange)(uint32 iStatus) = NULL;

//global
void (*gfDNMonitor_StateChange)(uint8 iStatus) = NULL;
mClass_LDR gClassLDR = {
	.apPinArr = &sdefPinArr_LDR,
	.afInit = sfdefLDR_Init,
	.afUninit = sfdefLDR_Uninit,
	.afGetStatus = sfdefLDR_GetStatus,
	.afRegCallBack = sfdefLDR_RegistCallBack,
	.afSetSensitive = sfdefLDR_SetSensitive,
	.afGetSensitive = sfdefLDR_GetSensitive,
	.afSetDNLimit = sfdefLDR_SetDNLimit,
};

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  uint32 sfLDR_Filter(uint32 iVal)
@introduction:
    滤波函数，可能会被外部引用

@parameter:
    iVal
        源数据

@return:
    iVal
        加工数据

*/
// Filter weight 8 == 1 1 1 1 2 2
uint32 sfLDR_Filter(uint32 iVal)
{
	static uint32 tOri = 0;
	static uint8 tCount = 0;
	uint8 _i;
	uint32 tSumVal;

	{
		uint32 tDiff;
		if (iVal > tOri) {
			tDiff = iVal - tOri;
		} else {	//if( iVal <= tOri ){
			tDiff = tOri - iVal;
		}
		if (tDiff >= DC_LDR_FilterBreakLimit) {
			tCount++;
			if (tCount == DC_LDR_FilterBreakTimes) {
				sfLDR_FilterBuf_Assimilate(iVal);
				tCount = 0;
			}
		} else {
			tCount = 0;
		}
	}

	tSumVal = (iVal << 3)
	          + (saFltOrder1[0])
	          + (saFltOrder1[1])
	          + (saFltOrder1[2])
	          + (saFltOrder1[3])
	          + (saFltOrder1[4] << 2);
	tSumVal >>= 4;

	for (_i = 0; _i < DC_LDR_FilterOrder1Steps - 1; _i++) {
		saFltOrder1[_i] = saFltOrder1[_i + 1];
	}
	saFltOrder1[_i] = tSumVal;

	if (tCount == 0) {
		tOri = tSumVal;
	}

	sfLDR_Judge(tSumVal);
#if DEBUG_LDR
	// NVCPrint_h("The LDR Value:%d\r\n",(int)tSumVal);
	NVCPrint("The LDR Value Filt:%d:Ori:%d", (int)tSumVal, (int)iVal);
#endif

	return tSumVal;
}

//---------- ---------- ---------- ----------
/*  void sfLDR_FilterBuf_Assimilate(uint32 iVal)
@introduction:
    全局修改滤波队列里的值

@parameter:
    iVal
        data

@return:
    void

*/
void sfLDR_FilterBuf_Assimilate(uint32 iVal)
{
	//homogenization or assimilate
	uint8 _i;
	for (_i = 0; _i < DC_LDR_FilterOrder1Steps; _i++) {
		saFltOrder1[_i] = iVal;
	}
}

//---------- ---------- ---------- ----------
/*  void sfLDR_Judge(uint32 iVal)
@introduction:
    根据最后滤波的值，判断是否改变状态，是否通报应用层

@parameter:
    iVal
        data

@return:
    void

*/
void sfLDR_Judge(uint32 iVal)
{
	if (iVal > sDNMonitor.aDLimit) {
		if (sDNMonitor.aStatus & DC_DNMonitor_NIGHT) {
			if (gfDNMonitor_ReportStateChange) {
				gfDNMonitor_ReportStateChange(DC_LDR_DAY);
			}
			sDNMonitor.aStatus &= ~DC_DNMonitor_NIGHT;
			sDNMonitor.aStatus |= DC_DNMonitor_DAY;

			if (gfDNMonitor_StateChange) {
				gfDNMonitor_StateChange(DC_DNMonitor_DAY);
			}

			if (gClassIfrFilt.aStatusChgType == DC_NVIfrFilt_Auto
			    && u32LDRInitFlag != 0) {
				// Auto
				gSysCTA->apIfrLi->afSetStatus(DC_IfLi_Off);	//Close the IfrLight
				gSysCTA->apIfrFilter->
				afSetStatus(DC_IfrFlt_BlockLi);
				if (gfIRCMonitor_ReportStateChange) {
					gfIRCMonitor_ReportStateChange
					(DC_IfrFlt_BlockLi);
				}
			}
		}
	} else if (iVal < sDNMonitor.aNLimit) {
		if (sDNMonitor.aStatus & DC_DNMonitor_DAY) {
			if (gfDNMonitor_ReportStateChange) {
				gfDNMonitor_ReportStateChange(DC_LDR_Night);
			}
			sDNMonitor.aStatus &= ~DC_DNMonitor_DAY;
			sDNMonitor.aStatus |= DC_DNMonitor_NIGHT;

			if (gfDNMonitor_StateChange) {
				gfDNMonitor_StateChange(DC_DNMonitor_NIGHT);
			}

			if (gClassIfrFilt.aStatusChgType == DC_NVIfrFilt_Auto
			    && u32LDRInitFlag != 0) {
				//Auto
				gSysCTA->apIfrLi->afSetStatus(DC_IfLi_On);	//Open the IfrLight
				gSysCTA->apIfrFilter->
				afSetStatus(DC_IfrFlt_PassLi);
				if (gfIRCMonitor_ReportStateChange) {
					gfIRCMonitor_ReportStateChange
					(DC_IfrFlt_PassLi);
				}
			}
		}
	}
	sDNMonitor.aLSValue = (uint16) iVal;
}

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefLDR_Init(void)
@introduction:
    初始化 LDR 包括打开 ADC0 ，注册周期任务已进行实时监控环境变化

@parameter:

@return:

*/
static int32 sfdefLDR_Init(void)
{

	uint32 tVal;
	uint8 _i;
	gClassHAL.ADC->prfSetOpt(DC_HAL_ADCOpt_StartCov | DC_HAL_ADChannel(0));
	_i = 0;
	do {
		udelay(2);
		tVal = gClassHAL.ADC->prfGetValue(DC_HAL_ADChannel(0));
		_i++;
	} while ((_i < 150) && (tVal & DC_HAL_ADCRet_Busy));

#if DEBUG_INIT
	if (_i == 150) {
		NVCPrint("LDR Init Failed");
		return -1;
	}
#endif

	sfLDR_FilterBuf_Assimilate(tVal & DC_HAL_ADCOpt_ValMask);
	sfLDR_Judge(tVal & DC_HAL_ADCOpt_ValMask);

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT, sfLDR_MotorCheck,
	                                  DC_LDR_CheckFreq);
	u32LDRInitFlag = 1;

#if DEBUG_INIT
	NVCPrint("The LDR module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefLDR_Uninit(void)
@introduction:
    取消 LDR 初始化时注册的资源

@parameter:

@return:


*/
// ---------------------------------------->sfdefLDR_Uninit
static int32 sfdefLDR_Uninit(void)
{
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sfLDR_MotorCheck);
	u32LDRInitFlag = 0;

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefLDR_GetStatus(void)
@introduction:
    获取当前 环境状态 （白天，还是黑夜）

@parameter:
    void

@return:
    DC_LDR_DAY
    DC_LDR_Night

*/
static int32 sfdefLDR_GetStatus(void)
{
	int32 tRet = DC_LDR_DAY;
	if (sDNMonitor.aStatus & DC_DNMonitor_DAY) {
		tRet = DC_LDR_DAY;
	} else if (sDNMonitor.aStatus & DC_DNMonitor_NIGHT) {
		tRet = DC_LDR_Night;
	}
	tRet |= (sDNMonitor.aLSValue & 0xFFFF) << 8;
	return tRet;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefLDR_RegistCallBack(void (*iHandle)(uint8))
@introduction:
    注册上报回调函数

@parameter:
    iHandle
        指向回调函数指针

@return:
    0   SUCCESS

*/
static int32 sfdefLDR_RegistCallBack(void (*iHandle)(uint8))
{
	gfDNMonitor_ReportStateChange = iHandle;
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32	sfdefLDR_SetSensitive(uint8 iSPoint,uint8 iDomain )
@introduction:
    提供给应用层用以修改检测灵敏度

@parameter:
    iSPoint
        检测照度的中心位置
    iDomain
        以中心位置的照度允许误差范围

@return:
    0   SUCCESS

*/
static int32 sfdefLDR_SetSensitive(uint8 iSPoint, uint8 iDomain)
{
	int tSetVal;

	tSetVal = (int)sDNMonitor.aAdjUnit * ((int)iSPoint + (int)iDomain);
	if (tSetVal > DC_LDR_DEF_MAXSAM) {
		tSetVal = DC_LDR_DEF_MAXSAM;
	}
	sDNMonitor.aDLimit = (uint16) tSetVal;
	tSetVal = (int)sDNMonitor.aAdjUnit * ((int)iSPoint - (int)iDomain);
	if (tSetVal < 0) {
		tSetVal = 0;
	}
	sDNMonitor.aNLimit = (uint16) tSetVal;

#if DEBUG_LDR
	NVCPrint("LDR Set Sensitive is not supported!\r\n");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32	sfdefLDR_GetSensitive(uint8 *iSPoint,uint8 *iDomain )
@introduction:
    获取当前 LDR 的检测灵敏度

@parameter:
    iSPoint
        返回检测照度的中心位置
    iDomain
        返回以中心位置的照度允许误差范围

@return:
    0   SUCCESS

*/
static int32 sfdefLDR_GetSensitive(uint8 *iSPoint, uint8 *iDomain)
{
	*iSPoint =
	    (sDNMonitor.aDLimit + sDNMonitor.aNLimit) / sDNMonitor.aAdjUnit;
	*iDomain =
	    (sDNMonitor.aDLimit - sDNMonitor.aNLimit) / sDNMonitor.aAdjUnit;
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void	sfLDR_MotorCheck(void)
@introduction:
    被初始化时注册到周期任务队列中的事件，目的在周期性的触发采集环境照度值（通过
    AD）

@parameter:
    void

@return:
    void

*/
static void sfLDR_MotorCheck(void)
{
	uint32 tVal;
	tVal = gClassHAL.ADC->prfGetValue(DC_HAL_ADChannel(0));
	gClassHAL.ADC->prfSetOpt(DC_HAL_ADCOpt_StartCov | DC_HAL_ADChannel(0));

	if (!(tVal & DC_HAL_ADCRet_Busy)) {
		tVal &= DC_HAL_ADCOpt_ValMask;
		sfLDR_Filter(tVal);
	}
}

static int32 sfdefLDR_SetDNLimit(uint16 iDLimit, uint16 iNLimit)
{
	sDNMonitor.aDLimit = iDLimit;
	sDNMonitor.aNLimit = iNLimit;

	return 0;
}

//==============================================================================
//Others
