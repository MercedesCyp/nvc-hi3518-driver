/*
Coder:      aojie.meng
Date:       2015-8-15

Abstract:
    D04 圆 配置代码

    LDR                             OK
    INFRARED LIGHT                  OK
    INFRARED FILTER                 OK
    STATE LIGHT                     OK
    BUTTON                          OK
    AUDIO PLUG                      OK
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
static mClass_CTA *sfPro_3518C_D04_WR_CONF(void);
static int32 sfPro_3518C_D04_Init(void);
static int32 sfPro_3518C_D04_Uninit(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo s3518C_D04_StaLi_Pin[] = {
	{0, 4, 76, 0x00},
};

//global
mProInfo pgs3518C_D04_ProInfo = {
	.aPro = {.aKey = "D04", .aID = 0xD04,},
	.aChip = {.aKey = "3518C", .aID = 0x3518C,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518C_D04_WR_CONF,
	.afInit = sfPro_3518C_D04_Init,
	.afUninit = sfPro_3518C_D04_Uninit,
};

// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------:
static mClass_CTA *sfPro_3518C_D04_WR_CONF(void)
{
	mClass_CTA *tClassCTA = NULL;
	// 初始化产品自己的数组
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	tClassCTA->apAudioPlug = &gClassAudioPlug;

	tClassCTA->apIfrLi = &gClassIfrLi;

	tClassCTA->apLDR = &gClassLDR;

	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = s3518C_D04_StaLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 1;
	tClassCTA->apStateLi->OnStatus = 0x00;
	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_InTypeA;

	tClassCTA->apButton = &gClassButton;

	return tClassCTA;
}

static int32 sfPro_3518C_D04_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	tClassCTA->apAudioPlug->afInit();
	tClassCTA->apAudioPlug->afSetStatus(DC_AuPl_Speaker_Off);

	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);

	tClassCTA->apLDR->afInit();

	tClassCTA->apStateLi->afInit();
	// {
	// mStaLi_SetParam tTest;
	// tTest.aUnit = 0;
	// tTest.aPTime = 0xFF;
	// tTest.aNTime = 0xFF;
	// tClassCTA->apStateLi->afSetStatus(&tTest);
	// }

	tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);

	tClassCTA->apButton->afInit();

	return 0;
}

static int32 sfPro_3518C_D04_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	tClassCTA->apAudioPlug->afUninit();
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apLDR->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apIfrFilter->afUninit();
	tClassCTA->apButton->afUninit();
	kfree(tClassCTA);

	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}
