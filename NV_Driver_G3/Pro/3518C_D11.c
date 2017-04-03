/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    D11 AWox 球泡机 配置代码

    LDR                                   OK
    INFRARED LIGHT                        OK
    DOUBLE LENS                           OK
    STATE LIGHT                           OK
    BUTTON                                OK
    AUDIO PLUG                            OK
    NIGHT LIGHT                           OK
    PAN&TILT                              OK
    COOL FAN                              OK
*/
//==============================================================================
// C
// Linux
// local
#include "ProInclude.h"
// remote
#include "../HAL/HAL.h"

//=============================================================================
// MACRO
// CONSTANT
#define DC_D11_StaLiNum 1
#define DC_D11_PTZDrvPinNum			8

#define DC_D11_PTZHDgrPerStep		164794
#define DC_D11_PTZVDgrPerStep		703125

#define DC_D11_PTZHStepsPerFull		2184
#define DC_D11_PTZVStepsPerFull		512

#define DC_D11_PTZHLimSteps			2200
#define DC_D11_PTZVLimSteps			128

#define DC_D11_PreConf				(\
                                     DC_PTSP_Negative\
                                     |DC_PTSP_LeftIsA\
                                     |DC_PTSP_DownIsA\
                                     |DC_PTSP_Supp_HMove\
                                     |DC_PTSP_Supp_VMove\
                                     |DC_PTSP_Supp_HVMove)
// FUNCTION

//==============================================================================
//extern
//local
// state light
static int32 sfD11_NiLi_Init(void);
static int32 sfD11_NiLi_Uninit(void);
static void sfD11_NiLi_SetStatus(mClass_NiLiIfo *);
static void sfD11_NiLi_GetStatus(void);
static void sfD11_SubCmd_Implement(void);

static int32 sfPro_3518C_D11_Init(void);
static int32 sfPro_3518C_D11_Uninit(void);
static mClass_CTA *sfPro_3518C_D11_WR_CONF(void);
//global
extern void gfCOOLFAN_DriveTheFan(int32 tTemp);

//==============================================================================
//extern
//local
static mSubCmd tD11_SubCmd;
static mGPIOPinIfo sStaLi_Pin[DC_D11_StaLiNum] = {
	{0, 4, 76, 0x00},	// HA
};

static mGPIOPinIfo sD11_NiLi_Pin[] = {
	{5, 1, 46, 0x00},
};

static mGPIOPinIfo sPTZ_DrvPin[DC_D11_PTZDrvPinNum] = {
	{4, 4, 21, 0x00},	// HA
	{4, 5, 20, 0x00},	// HB
	{4, 6, 19, 0x00},	// HC
	{4, 7, 18, 0x00},	// HD

	{4, 0, 17, 0x00},	// VA
	{4, 1, 16, 0x00},	// VB
	{4, 2, 15, 0x00},	// VC
	{4, 3, 14, 0x00},	// VD
};

//global
mProInfo pgs3518C_D11_ProInfo = {
	.aPro = {.aKey = "D11", .aID = 0xD11,},
	.aChip = {.aKey = "3518C", .aID = 0x3518C,},
	.aSubCmd = &tD11_SubCmd,
	.afRewriteAndConfig = sfPro_3518C_D11_WR_CONF,
	.afInit = sfPro_3518C_D11_Init,
	.afUninit = sfPro_3518C_D11_Uninit,
};

// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------:
static mClass_CTA *sfPro_3518C_D11_WR_CONF(void)
{
	mClass_CTA *tClassCTA;

	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));
	// LDR
	tClassCTA->apLDR = &gClassLDR;
	// Double Lens
	tClassCTA->apDubLens = &gClassDouobLens;
	// Ifrared light
	tClassCTA->apIfrLi = &gClassIfrLi;
	// Audio Plug
	tClassCTA->apAudioPlug = &gClassAudioPlug;
	// Button
	tClassCTA->apButton = &gClassButton;
	// State Light
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = sStaLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = DC_D11_StaLiNum;
	tClassCTA->apStateLi->OnStatus = 0x01;
	// Night Light
	tClassCTA->apNightLi = &gClassNiLi;
	tClassCTA->apNightLi->apPinArr->apPin = sD11_NiLi_Pin;
	tClassCTA->apNightLi->apPinArr->aNum = 1;
	tClassCTA->apNightLi->afInit = sfD11_NiLi_Init;
	tClassCTA->apNightLi->afUninit = sfD11_NiLi_Uninit;
	tClassCTA->apNightLi->afSetStatus = sfD11_NiLi_SetStatus;
	tClassCTA->apNightLi->afGetStatus = sfD11_NiLi_GetStatus;
	// Pan Tilt
	tClassCTA->apPTZ = &gClassPTZ;
	tClassCTA->apPTZ->apDrvPinArr->apPin = sPTZ_DrvPin;
	tClassCTA->apPTZ->apDrvPinArr->aNum = DC_D11_PTZDrvPinNum;
	gClassStr.afMemset((uint8 *) tClassCTA->apPTZ->apDefInfo, 0x00,
	                   sizeof(mPanTiltInfo));
	tClassCTA->apPTZ->apDefInfo->aHDgrPerStep = DC_D11_PTZHDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aVDgrPerStep = DC_D11_PTZVDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aHStepsPerFull = DC_D11_PTZHStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aVStepsPerFull = DC_D11_PTZVStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aHLimitSteps = DC_D11_PTZHLimSteps;
	tClassCTA->apPTZ->apDefInfo->aVLimitSteps = DC_D11_PTZVLimSteps;
	tClassCTA->apPTZ->apDefInfo->aRegCf = DC_D11_PreConf;

	return tClassCTA;
}

static int32 sfPro_3518C_D11_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	tClassCTA->apLDR->afInit();

	tClassCTA->apDubLens->afInit();
	tClassCTA->apDubLens->afSetStatus(DC_DuLens_DayLens);

	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);
	// tClassCTA->apIfrLi->afSetStatus(DC_IfLi_On);

	tClassCTA->apAudioPlug->afInit();
	tClassCTA->apAudioPlug->afSetStatus(DC_AuPl_Speaker_Off);

	tClassCTA->apStateLi->afInit();
	// {
	// mStaLi_SetParam tTest;
	// tTest.aUnit = 0;
	// tTest.aPTime = 0xFF;
	// tTest.aNTime = 0xFF;
	// tClassCTA->apStateLi->afSetStatus(&tTest);
	// }

	tClassCTA->apButton->afInit();
	tClassCTA->apNightLi->afInit();
	{
		mClass_NiLiIfo tSetParam;
		tSetParam.aStatus = DC_NiLi_On;
		tSetParam.aLevel = 100;
		tClassCTA->apNightLi->afSetStatus(&tSetParam);
	}

	tClassCTA->apPTZ->afInit();
	// {
	// mPTZAction tTest;
	// tTest.aHComponent   = 100;
	// tTest.aVComponent   = 100;
	// tTest.aSpeed        = 10;
	// tTest.aCommand      = DC_PTZCmd_Steps|DC_PTZCmd_Right|DC_PTZCmd_Up;
	// tClassCTA->apPTZ->gfPTZStart(tTest);
	// }

	gClassTempMonitor.afInit();
	gClassTempMonitor.afRegCallBack(gfCOOLFAN_DriveTheFan);
	gClassTempMonitor.afSetReportTime(5);
	// gfCOOLFAN_DriveTheFan(500);

	// printk("-------------------1\n");
	sfD11_SubCmd_Implement();
	// printk("-------------------2\n");
	return 0;
}

static int32 sfPro_3518C_D11_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	tClassCTA->apLDR->afUninit();
	tClassCTA->apDubLens->afUninit();
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apAudioPlug->afUninit();
	tClassCTA->apButton->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apNightLi->afUninit();
	tClassCTA->apPTZ->afUninit();

	kfree(tClassCTA);
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}

// ================================================================================
// ----------------------------------------> Night Light
#define DC_D11_StaLi_Duty		300
#define DC_D11_StaLi_IncUnit	3
#define DC_D11_StaLi_defWidth	0

static int32 sfD11_NiLi_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	mGPIOPinIfo *tPin = tClassCTA->apNightLi->apPinArr->apPin;

	gClassHAL.Pin->prfSetDirection(tPin, DC_HAL_PIN_SetOUT);

	return 0;
}

static int32 sfD11_NiLi_Uninit(void)
{
	return 0;
}

static void sfD11_NiLi_SetStatus(mClass_NiLiIfo *iSetIfo)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	mGPIOPinIfo *tPin = tClassCTA->apNightLi->apPinArr->apPin;

	*(gClassPro.apCTA->apNightLi->aIfo) = *iSetIfo;
	if (iSetIfo->aStatus & DC_NiLi_On) {
		gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(tPin, DC_HAL_PIN_SetLOW);
	}

	gClassHAL.PWM->pInfo->aDuty = DC_D11_StaLi_Duty;
	if (iSetIfo->aLevel <= 100) {
		gClassHAL.PWM->pInfo->aWidth =
		    (100 - iSetIfo->aLevel) * DC_D11_StaLi_IncUnit;
	} else {
		gClassHAL.PWM->pInfo->aWidth = DC_D11_StaLi_defWidth;
	}
	gClassHAL.PWM->pInfo->aNum = 1;
	gClassHAL.PWM->pInfo->aCmd = DC_PWM_CmdKeep;
	gClassHAL.PWM->prfStart();
}

static void sfD11_NiLi_GetStatus(void)
{

}

// ================================================================================
// ----------------------------------------> 命令附加功能
static void sfD11_SubCmd_Record(uint32 iCmd);
static mArr tD11_SubCmdKeyArr[] = {
	{"NiLi_ON", 1},
	{"NiLi_OFF", 2},
};

static uint32 tSubCmdRecord = 1;
static mSubCmd tD11_SubCmd = {
	.aNum = 2,
	.apCmd = tD11_SubCmdKeyArr,
	.afCmdProcessing = sfD11_SubCmd_Record,
};

static void sfD11_SubCmd_Record(uint32 iCmd)
{
	tSubCmdRecord = iCmd;
	return;
}

static void sfD11_SubCmd_Implement(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	if (tSubCmdRecord == 1) {
		mClass_NiLiIfo tSetParam;
		tSetParam.aStatus = DC_NiLi_On;
		tSetParam.aLevel = 100;
		tClassCTA->apNightLi->afSetStatus(&tSetParam);
	} else if (tSubCmdRecord == 2) {
		mClass_NiLiIfo tSetParam;
		tSetParam.aStatus = DC_NiLi_Off;
		tSetParam.aLevel = 0;
		tClassCTA->apNightLi->afSetStatus(&tSetParam);
	}
	return;
}
