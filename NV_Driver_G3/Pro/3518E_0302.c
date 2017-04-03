/*
Coder:      aojie.meng
Date:       2015-8-27

Abstract:
    初步测试
    LDR             0
    IfrFilt         0
    StaLi           0
    Button          0
*/
//==============================================================================
// C
// Linux
// local
#include "ProInclude.h"
// remote
#include "../HAL/HAL.h"
#include "../Tool/String.h"

//=============================================================================
// MACRO
// CONSTANT
#define DC_0302_LDR_DayValue     DC_LDR_ShiftD_Value
#define DC_0302_LDR_NightValue   DC_LDR_ShiftN_Value
#define DC_0302_LDR_DAY          0x01
#define DC_0302_LDR_NIGHT        0x02
#define DC_0302_LDR_CheckFreq    10	// 1s  1000ms
// FUNCTION

//==============================================================================
//extern
extern void sfLDR_Judge(uint32 iVal);
extern void sfLDR_FilterBuf_Assimilate(uint32 iVal);
extern uint32 sfLDR_Filter(uint32 iVal);
// extern void    (*gfDNMonitor_StateChange)(uint8 iStatus);
//local
static mClass_CTA *sfPro_3518E_0302_WR_CONF(void);
static int32 sfPro_3518E_0302_Init(void);
static int32 sfPro_3518E_0302_Uninit(void);
//
static int32 sf0302_LDR_Init(void);
static int32 sf0302_LDR_Uninit(void);
static void sf0302_LDR_MotorCheck(void);
//global

//==============================================================================
//extern

//local
static mGPIOPinIfo sF0302_Button_Pin[] = {
	{0, 3, 75, 0x00},
};

static mGPIOPinIfo sF0302_StaLi_Pin[] = {
	{5, 3, 48, 0x00},
	// {5,2,47,0x00},
};

static mGPIOPinIfo sF0302_IfrFilt_Pin[] = {
	{0, 1, 73, 0x00},
	{0, 2, 74, 0x00},
};

static mGPIOPinIfo sF0302_LDR_Pin[] = {
	{0, 0, 72, 0x00},
};

//global
mProInfo pgs3518E_0302_ProInfo = {
	.aPro = {.aKey = "F14", .aID = 0x0302,},
	.aChip = {.aKey = "3518E", .aID = 0x3518E,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518E_0302_WR_CONF,
	.afInit = sfPro_3518E_0302_Init,
	.afUninit = sfPro_3518E_0302_Uninit,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static mClass_CTA*	sfPro_3518E_0302_WR_CONF(void)
@introduction:

@parameter:

@return:

*/
static mClass_CTA *sfPro_3518E_0302_WR_CONF(void)
{
	mClass_CTA *tClassCTA;
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	// LDR
	tClassCTA->apLDR = &gClassLDR;
	tClassCTA->apLDR->apPinArr->apPin = sF0302_LDR_Pin;
	tClassCTA->apLDR->apPinArr->aNum = 1;
	tClassCTA->apLDR->afInit = sf0302_LDR_Init;
	tClassCTA->apLDR->afUninit = sf0302_LDR_Uninit;
	// Buttons
	tClassCTA->apButton = &gClassButton;
	tClassCTA->apButton->apPinArr->apPin = sF0302_Button_Pin;
	tClassCTA->apButton->apPinArr->aNum = 1;
	// State Light
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = sF0302_StaLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 1;
	tClassCTA->apStateLi->OnStatus = 0x00;
	// Infrared filter
	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeD;
	tClassCTA->apIfrFilter->apPinArr->apPin = sF0302_IfrFilt_Pin;
	tClassCTA->apIfrFilter->apPinArr->aNum = 2;

	return tClassCTA;
}

//---------- ---------- ---------- ----------
/*  static int32		sfPro_3518E_0302_Init(void)
@introduction:

@parameter:

@return:

*/
static int32 sfPro_3518E_0302_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	// HAL
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	// LDR
	tClassCTA->apLDR->afInit();
	// Buuton
	tClassCTA->apButton->afInit();
	// State Light
	tClassCTA->apStateLi->afInit();
	// Infrared filter
	tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);
	// tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32		sfPro_3518E_0302_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfPro_3518E_0302_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	// LDR
	tClassCTA->apLDR->afUninit();
	// Button
	tClassCTA->apButton->afUninit();
	// State light
	tClassCTA->apStateLi->afUninit();
	// Infrared filter
	tClassCTA->apIfrFilter->afUninit();

	// HAL
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	kfree(tClassCTA);
	return 0;
}

//==============================================================================
//Others
//---------- ---------- ---------- ----------
/*  static int32        sfB14_LDR_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sf0302_LDR_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	gClassHAL.Pin->prfSetDirection(&tPin[0], DC_HAL_PIN_SetIN);

	if (gClassHAL.Pin->prfGetPinValue(&tPin[0])) {
		sfLDR_FilterBuf_Assimilate(DC_0302_LDR_DayValue);
		sfLDR_Judge(DC_0302_LDR_DayValue);
	} else {
		gClassHAL.Pin->prfSetExport(&tPin[1], DC_HAL_PIN_SetHIGH);
		sfLDR_FilterBuf_Assimilate(DC_0302_LDR_NightValue);
		sfLDR_Judge(DC_0302_LDR_NightValue);
	}

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
	                                  sf0302_LDR_MotorCheck,
	                                  DC_0302_LDR_CheckFreq);

#if DEBUG_INIT
	NVCPrint("The LDR module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32        sfB14_LDR_Init(void)
@introduction:

@parameter:

@return:

*/
static int32 sf0302_LDR_Uninit(void)
{
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sf0302_LDR_MotorCheck);
	return 0;

}

//---------- ---------- ---------- ----------
/*  static void sf0302_LDR_MotorCheck(void)
@introduction:

@parameter:

@return:

*/
static void sf0302_LDR_MotorCheck(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		sfLDR_Judge(DC_0302_LDR_DayValue);
		// sfLDR_Filter( DC_0302_LDR_DayValue );
	} else {
		sfLDR_Judge(DC_0302_LDR_NightValue);
		// sfLDR_Filter( DC_0302_LDR_NightValue );
	}
}
