/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    0301

    LDR              | (GPIO9_2)               OK
    INFRARED LIGHT   | (POWER:G9_3)            OK
    INFRARED FILTER  | (CTR:G9_0)              OK
    STATE LIGHT      | (G:G1_6  R:G1_5 )       OK
    BUTTON           | (G5_1)                  OK


*/
//==============================================================================
// C
// Linux
// local
#include "ProInclude.h"
// remote
#include "../HAL/HAL.h"

//==============================================================================
//extern
//local
//->LDR
static int32 sf0301_LDR_Init(void);
static int32 sf0301_LDR_Uninit(void);

static int32 sfPro_3518E_0301_Init(void);
static int32 sfPro_3518E_0301_Uninit(void);
static mClass_CTA *sfPro_3518E_0301_WR_CONF(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo s0301_LDR_Pin[] = {
	{9, 2, 52, 0x01},
};

static mGPIOPinIfo s0301_IfrLi_Pin[] = {
	{9, 3, 53, 0x01},
};

static mGPIOPinIfo s0301_IfrFilt_Pin[] = {
	{9, 0, 50, 0x01},
};

static mGPIOPinIfo s0301_StaLi_Pin[] = {
	// {1,5,4,0x00},
	{1, 6, 5, 0x00},
};

static mGPIOPinIfo s0301_Button_Pin[] = {
	{5, 1, 46, 0x00},
};

//global
mProInfo pgs3518E_0301_ProInfo = {
	.aPro = {.aKey = "F11", .aID = 0x0301,},
	.aChip = {.aKey = "3518E", .aID = 0x3518E,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518E_0301_WR_CONF,
	.afInit = sfPro_3518E_0301_Init,
	.afUninit = sfPro_3518E_0301_Uninit,
};

// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------:

static mClass_CTA *sfPro_3518E_0301_WR_CONF(void)
{
	mClass_CTA *tClassCTA;
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	// LDR
	tClassCTA->apLDR = &gClassLDR;
	tClassCTA->apLDR->apPinArr->apPin = s0301_LDR_Pin;
	tClassCTA->apLDR->apPinArr->aNum = 1;
	tClassCTA->apLDR->afInit = sf0301_LDR_Init;
	tClassCTA->apLDR->afUninit = sf0301_LDR_Uninit;

	// Infrared light
	tClassCTA->apIfrLi = &gClassIfrLi;
	tClassCTA->apIfrLi->apPinArr->apPin = s0301_IfrLi_Pin;
	tClassCTA->apIfrLi->apPinArr->aNum = 1;

	// Infrared filter
	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->apPinArr->apPin = s0301_IfrFilt_Pin;
	tClassCTA->apIfrFilter->apPinArr->aNum = 1;
	// tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeA;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_InTypeA;

	// Button
	tClassCTA->apButton = &gClassButton;
	tClassCTA->apButton->apPinArr->apPin = s0301_Button_Pin;
	tClassCTA->apButton->apPinArr->aNum = 1;

	// State Light
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = s0301_StaLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 1;
	tClassCTA->apStateLi->OnStatus = 0x00;

	return tClassCTA;
}

static int32 sfPro_3518E_0301_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	tClassCTA->apLDR->afInit();
	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);
	// tClassCTA->apIfrLi->afSetStatus(DC_IfLi_On);

	tClassCTA->apIfrFilter->afInit();
	// tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);

	tClassCTA->apStateLi->afInit();
	// {
	// mStaLi_SetParam tTest;
	// tTest.aUnit = 0;
	// tTest.aPTime = 1000;
	// tTest.aNTime = 1000;
	// tClassCTA->apStateLi->afSetStatus(&tTest);
	// }

	tClassCTA->apButton->afInit();

	return 0;
}

static int32 sfPro_3518E_0301_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	tClassCTA->apLDR->afUninit();
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apIfrFilter->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apButton->afUninit();

	kfree(tClassCTA);
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}

// ------------------------------------------------------------------>0301
// -------------------------------------------------------->LDR
#define DC_0301_LDR_DayValue    69
#define DC_0301_LDR_NightValue  20

#define DC_0301_LDR_CheckFreq	3	//300ms

extern void sfLDR_Judge(uint32 iVal);
extern void sfLDR_FilterBuf_Assimilate(uint32 iVal);
extern uint32 sfLDR_Filter(uint32 iVal);
//
static void sf0301_LDR_MotorCheck(void);

static int32 sf0301_LDR_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	// gClassHAL.Pin->prfSetDirection( tPin, DC_HAL_PIN_SetOUT);
	gClassHAL.Pin->prfSetDirection(tPin, DC_HAL_PIN_SetIN);

	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		sfLDR_FilterBuf_Assimilate(DC_0301_LDR_DayValue);
		sfLDR_Judge(DC_0301_LDR_DayValue);
	} else {
		sfLDR_FilterBuf_Assimilate(DC_0301_LDR_NightValue);
		sfLDR_Judge(DC_0301_LDR_NightValue);
	}

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
	                                  sf0301_LDR_MotorCheck,
	                                  DC_0301_LDR_CheckFreq);

#if DEBUG_INIT
	NVCPrint("The LDR module Start! OK");
#endif
	return 0;
}

static int32 sf0301_LDR_Uninit(void)
{

	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sf0301_LDR_MotorCheck);
	return 0;
}

static void sf0301_LDR_MotorCheck(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		sfLDR_Filter(DC_0301_LDR_DayValue);
	} else {
		sfLDR_Filter(DC_0301_LDR_NightValue);
	}
}
