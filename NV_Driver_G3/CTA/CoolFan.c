/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    目前，只有D11中有风扇，而且，风扇的启动与否，基本上是驱动控制的
    D11
    在配置驱动的时候，不必要挂载温度模块，但要在初始化的时候要先初始化温度功能
    包括 Init and Config_Report
    然后再调用该文档的初始化函数，使之与温度之间建立联系

*/
//==============================================================================
// C
// Linux
// local
// remote
#include "../GlobalParameter.h"
#include "../HAL/HAL.h"
#include "../Tool/NTC.h"

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
#define DC_CoolFan_PWMDuty			300
#define DC_CoolFan_PWMMinWidth		0	// 0
#define DC_CoolFan_PWMMaxWidth		300	// 1

#define DC_CoolFan_DEF_EXPORT		0
#define DC_CoolFan_MIN_EXPORT		180
#define DC_CoolFan_MAX_EXPORT		300

#define DC_COFAN_OH_Temp		500
#define DC_COFAN_OOH_Temp		650

#define DC_COFAN_LBreak			-300
#define DC_COFAN_HBreak			1200

#define DF_COOFAN_ExportIncreasing	{sExportValue+=4;}
#define DF_COOFAN_ExportDecreasing	{sExportValue-=4;}

//==============================================================================
//extern
//local
static uint16 sExportValue = DC_CoolFan_DEF_EXPORT;
//global

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  void gfCOOLFAN_DriveTheFan(int32 tTemp)
@introduction:
    根据温度的上下限，配置风扇输出

@parameter:
    tTemp
        环境温度*10

@return:
    void

*/
void gfCOOLFAN_DriveTheFan(int32 tTemp)
{
	//DC_COOLFAN_INCREASING_POWER
	if (tTemp < DC_TEMPERATURE_iAbsoluteZero) {
		sExportValue = DC_CoolFan_MAX_EXPORT;

	} else {
		if (tTemp >= DC_COFAN_OH_Temp) {
			if (tTemp >= DC_COFAN_OOH_Temp) {
				sExportValue = DC_CoolFan_MAX_EXPORT;
			} else {
				if (sExportValue < DC_CoolFan_MIN_EXPORT) {
					sExportValue = DC_CoolFan_MIN_EXPORT;
				} else if (sExportValue < DC_CoolFan_MAX_EXPORT) {
					DF_COOFAN_ExportIncreasing;
				}
			}
		} else {	//DC_COOLFAN_DECREASING_POWER
			if (tTemp < DC_COFAN_LBreak) {
				sExportValue = DC_CoolFan_MAX_EXPORT;
			} else {
				if (sExportValue > DC_CoolFan_MIN_EXPORT) {
					sExportValue = DC_CoolFan_DEF_EXPORT;
				}
			}
		}
	}

#if DEBUG_FAN
	NVCPrint("Temperature: %d\r\nExport: %d\r\n", (int)tTemp,
	         (int)sExportValue);
#endif
	gClassHAL.PWM->pInfo->aDuty = DC_CoolFan_PWMDuty;
	gClassHAL.PWM->pInfo->aWidth = sExportValue;
	gClassHAL.PWM->pInfo->aNum = 0;
	gClassHAL.PWM->pInfo->aCmd = DC_PWM_CmdKeep;
	gClassHAL.PWM->prfStart();
}
