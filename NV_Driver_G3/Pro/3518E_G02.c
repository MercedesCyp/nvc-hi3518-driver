/*
Coder:      aojie.meng
Date:       2016-4-1

Abstract:
    Night Light
        GPIO5_1, high_on, low_off
    PIR
        GPIO1_0, high_normal, low_abnormal
    State light
        red     GPIO5_3, high_off, low_on
        green   GPIO5_2, high_off, low_on
    Button
        wifi_button, GPIO0_1, high_normal, low_pressed

    BUTTON                  0
    PIR                     0
    STATELED                0
    NIGHT_LIGHT             0
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
static mClass_CTA *sfPro_3518E_G02_WR_CONF(void);
static int32 sfPro_3518E_G02_Init(void);
static int32 sfPro_3518E_G02_Uninit(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo s3518E_G02_StaLi_Pin[] = {
	{5, 2, 47, 0x00},
	{5, 3, 48, 0x00},
};

static mGPIOPinIfo s3518E_G02_NiLi_Pin[] = {
	{5, 1, 46, 0x00},
};

//global
mProInfo pgs3518E_G02_ProInfo = {
	.aPro = {.aKey = "G02", .aID = 0x1002,},
	.aChip = {.aKey = "3518E", .aID = 0x3518E,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518E_G02_WR_CONF,
	.afInit = sfPro_3518E_G02_Init,
	.afUninit = sfPro_3518E_G02_Uninit,
};

// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------:
static mClass_CTA *sfPro_3518E_G02_WR_CONF(void)
{
	mClass_CTA *tClassCTA = NULL;
	// 初始化产品自己的数组
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	// Button
	tClassCTA->apButton = &gClassButton;
	// State light
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = s3518E_G02_StaLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 2;
	tClassCTA->apStateLi->OnStatus = 0x00;
	// PIR
	tClassCTA->apPIR = &gClassPIR;

	// NightLight
	tClassCTA->apNightLi = &gClassNiLi;
	tClassCTA->apNightLi->apPinArr->apPin = s3518E_G02_NiLi_Pin;

	return tClassCTA;
}

static int32 sfPro_3518E_G02_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	// State Light
	tClassCTA->apStateLi->afInit();
	// {
	// mStaLi_SetParam tTest;
	// tTest.aUnit = 0;
	// tTest.aPTime = 0xFF;
	// tTest.aNTime = 0xFF;
	// tClassCTA->apStateLi->afSetStatus(&tTest);
	// }
	// Button
	tClassCTA->apButton->afInit();
	// Nigh Li
	tClassCTA->apNightLi->afInit();
	// PIR
	tClassCTA->apPIR->afInit();

	return 0;
}

static int32 sfPro_3518E_G02_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	// State Light
	tClassCTA->apStateLi->afUninit();
	// Button
	tClassCTA->apButton->afUninit();
	// Nigh Li
	tClassCTA->apNightLi->afUninit();
	// PIR
	tClassCTA->apPIR->afUninit();

	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}
