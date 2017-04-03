/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    F05 中本外壳 摇头机 配置代码

    LDR                                   OK
    INFRARED LIGHT                        OK
    DOUBLE LENS                           OK
    STATE LIGHT                           OK
    BUTTON                                OK
    AUDIO PLUG                            OK
    PAN&TILT                              OK
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
//LDR
#define DC_F16_LDR_DayValue     DC_LDR_ShiftD_Value
#define DC_F16_LDR_NightValue   DC_LDR_ShiftN_Value
#define DC_F16_LDR_DAY          0x01
#define DC_F16_LDR_NIGHT        0x02
#define DC_F16_LDR_CheckFreq    10	// 1s  1000ms

//PTZ DC_PTSP_Positive DC_PTSP_Negative
#define DC_F16_PTZHDgrPerStep		527343
#define DC_F16_PTZVDgrPerStep		703125
#define DC_F16_PTZHStepsPerFull		2048
#define DC_F16_PTZVStepsPerFull		157
#define DC_F16_PTZHLimSteps			2048
#define DC_F16_PTZVLimSteps			170
#define DC_F16_PreConf				(\
                                     DC_PTSP_Negative\
                                     |DC_PTSP_LeftIsA\
                                     |DC_PTSP_UpIsA\
                                     |DC_PTSP_Locate\
                                     |DC_PTSP_HHeadLim\
                                     |DC_PTSP_HTailLim\
                                     |DC_PTSP_VHeadLim\
                                     |DC_PTSP_VTailLim\
                                     |DC_PTSP_Supp_HMove\
                                     |DC_PTSP_Supp_VMove\
                                     |DC_PTSP_Supp_HVMove\
                                     |DC_PTSP_Supp_HScan\
                                     |DC_PTSP_Supp_VScan\
                                     |DC_PTSP_Supp_OriPosition\
                                     |DC_PTSP_Supp_CurPos\
                                     |DC_PTSP_Supp_HLim\
                                     |DC_PTSP_Supp_VLim\
                                     |DC_PTSP_Supp_PRESET\
                                     |DC_PTSP_Supp_CRUISE)

#define DC_F16_PTZDrvPinNum			8
#define DC_F16_PTZLimPinNum			4
// FUNCTION

//==============================================================================
//extern
extern void sfLDR_Judge(uint32 iVal);
extern void sfLDR_FilterBuf_Assimilate(uint32 iVal);
extern uint32 sfLDR_Filter(uint32 iVal);
// extern void    (*gfDNMonitor_StateChange)(uint8 iStatus);
//local
static mClass_CTA *sfPro_3518E_F16_WR_CONF(void);
static int32 sfPro_3518E_F16_Init(void);
static int32 sfPro_3518E_F16_Uninit(void);
//LDR
static void sfF16_LDR_MotorCheck(void);
static int32 sfF16_LDR_Uninit(void);
static int32 sfF16_LDR_Init(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo s3518E_F16_LDR_Pin[] = {
	{0, 0, 72, 0x00},
};

static mGPIOPinIfo sFf16_IfrFilt_Pin[] = {
	{0, 1, 73, 0x00},
	{0, 2, 74, 0x00},
};

static mGPIOPinIfo s3518E_F16_Button_Pin[] = {
	{5, 1, 46, 0x00},
};

static mGPIOPinIfo s3518E_F16_StatLi_Pin[] = {
	{7, 6, 66, 00},
	{7, 7, 67, 00},
};

static mGPIOPinIfo sPTZ_DrvPin[DC_F16_PTZDrvPinNum] = {
	{9, 7, 57, 0x01},	// VD B
	{9, 6, 56, 0x01},	// VC B
	{9, 5, 55, 0x01},	// VB B
	{9, 4, 54, 0x01},	// VA B

	{9, 3, 53, 0x01},	// HD A
	{9, 2, 52, 0x01},	// HC A
	{9, 1, 51, 0x01},	// HB A
	{9, 0, 50, 0x01},	// HA A
};

static mGPIOPinIfo sPTZ_LimPin[DC_F16_PTZLimPinNum] = {
	{5, 5, 69, 0x00},	// LVHead // Origin   B
	{5, 4, 68, 0x00},	// LVTail // tail     B
	{1, 0, 0, 0x00},	// LHTail // tail     A
	{1, 7, 31, 0x00},	// LHHead // Origin     A
};

//global
mProInfo pgs3518E_F16_ProInfo = {
	.aPro = {.aKey = "F16", .aID = 0xF16,},
	.aChip = {.aKey = "3518E", .aID = 0x3518E,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518E_F16_WR_CONF,
	.afInit = sfPro_3518E_F16_Init,
	.afUninit = sfPro_3518E_F16_Uninit,
};

// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------:
extern mGPIOPinIfo sdefPin_IfrFilt_TypeB[];
static mClass_CTA *sfPro_3518E_F16_WR_CONF(void)
{
	mClass_CTA *tClassCTA;
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	// LDR
	tClassCTA->apLDR = &gClassLDR;
	tClassCTA->apLDR->apPinArr->apPin = s3518E_F16_LDR_Pin;
	tClassCTA->apLDR->apPinArr->aNum = 1;
	tClassCTA->apLDR->afInit = sfF16_LDR_Init;
	tClassCTA->apLDR->afUninit = sfF16_LDR_Uninit;

	// Infrared filter
	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeD;
	tClassCTA->apIfrFilter->apPinArr->apPin = sFf16_IfrFilt_Pin;
	tClassCTA->apIfrFilter->apPinArr->aNum = 2;
	// Button
	tClassCTA->apButton = &gClassButton;
	tClassCTA->apButton->apPinArr->apPin = s3518E_F16_Button_Pin;
	tClassCTA->apButton->apPinArr->aNum = 1;

	// State Light
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = s3518E_F16_StatLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 2;
	tClassCTA->apStateLi->OnStatus = 0x00;

	//PTZ
	tClassCTA->apPTZ = &gClassPTZ;
	tClassCTA->apPTZ->apDrvPinArr->apPin = sPTZ_DrvPin;
	tClassCTA->apPTZ->apDrvPinArr->aNum = DC_F16_PTZDrvPinNum;
	tClassCTA->apPTZ->apLimPinArr->apPin = sPTZ_LimPin;
	tClassCTA->apPTZ->apLimPinArr->aNum = DC_F16_PTZLimPinNum;
	gClassStr.afMemset((uint8 *) tClassCTA->apPTZ->apDefInfo, 0x00,
	                   sizeof(mPanTiltInfo));
	tClassCTA->apPTZ->apDefInfo->aHDgrPerStep = DC_F16_PTZHDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aVDgrPerStep = DC_F16_PTZVDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aHStepsPerFull = DC_F16_PTZHStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aVStepsPerFull = DC_F16_PTZVStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aHLimitSteps = DC_F16_PTZHLimSteps;
	tClassCTA->apPTZ->apDefInfo->aVLimitSteps = DC_F16_PTZVLimSteps;
	tClassCTA->apPTZ->apDefInfo->aRegCf = DC_F16_PreConf;

	return tClassCTA;
}

static int32 sfPro_3518E_F16_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	tClassCTA->apLDR->afInit();
	tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);
	// tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);

	tClassCTA->apStateLi->afInit();
	{
		mStaLi_SetParam tTest;
		tTest.aUnit = 1;
		tTest.aPTime = 0xFF;
		tTest.aNTime = 0xFF;
		tTest.aMode = DC_StaLi_Mod_DEFAULT;
		tClassCTA->apStateLi->afSetStatus(&tTest);
	}
	tClassCTA->apButton->afInit();
	tClassCTA->apPTZ->afInit();

	return 0;
}

static int32 sfPro_3518E_F16_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	tClassCTA->apLDR->afUninit();
	tClassCTA->apIfrFilter->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apButton->afUninit();
	tClassCTA->apPTZ->afUninit();

	kfree(tClassCTA);
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}

//==============================================================================
//Others
//---------- ---------- ---------- ----------
/*  static int32 sfF16_LDR_Init(void)
@introduction:

@parameter:

@return:

*/
static int32 sfF16_LDR_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	gClassHAL.Pin->prfSetDirection(&tPin[0], DC_HAL_PIN_SetIN);

	if (gClassHAL.Pin->prfGetPinValue(&tPin[0])) {
		sfLDR_FilterBuf_Assimilate(DC_F16_LDR_DayValue);
		sfLDR_Judge(DC_F16_LDR_DayValue);
	} else {
		sfLDR_FilterBuf_Assimilate(DC_F16_LDR_NightValue);
		sfLDR_Judge(DC_F16_LDR_NightValue);
	}

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT, sfF16_LDR_MotorCheck,
	                                  DC_F16_LDR_CheckFreq);

#if DEBUG_INIT
	NVCPrint("The LDR module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfF16_LDR_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfF16_LDR_Uninit(void)
{
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sfF16_LDR_MotorCheck);
	return 0;

}

//---------- ---------- ---------- ----------
/*  static void sfF16_LDR_MotorCheck(void)
@introduction:

@parameter:

@return:

*/
static void sfF16_LDR_MotorCheck(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;
	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		sfLDR_Judge(DC_F16_LDR_DayValue);
	} else {
		sfLDR_Judge(DC_F16_LDR_NightValue);
	}
}
