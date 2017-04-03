// /////////////////////////////////////////////////////////////////////////////
// LDR                                   OK
// INFRARED LIGHT
// INFRARED FILTER
// STATE LIGHT                           OK (GPIO03 GREEN)
// BUTTON                                Butto's voltage is always low
// AUDIO PLUG                            
// 
// 
// /////////////////////////////////////////////////////////////////////////////
#include "ProInclude.h"
#include "../HAL/HAL.h"
#include "../Tool/String.h"








// ================================================================================
// ---------------------------------------------------------------------->Local function definition
static int32		sfPro_3518E_F09_Init(void);
static int32		sfPro_3518E_F09_Uninit(void);
static mClass_CTA*	sfPro_3518E_F09_WR_CONF(void);









// ================================================================================
// ---------------------------------------------------------------------->Local Variable 
mProInfo pgs3518E_F09_ProInfo={
	.aPro					= {.aKey="F09",		.aID=0xF09,},
	.aChip					= {.aKey="3518E",	.aID=0x3518E,},
	.aSubCmd				= NULL,
	.afRewriteAndConfig 	= sfPro_3518E_F09_WR_CONF,
	.afInit					= sfPro_3518E_F09_Init,
	.afUninit				= sfPro_3518E_F09_Uninit,
};

// static mGPIOPinIfo sF09_StaLi_Pin[] = {
	// {0,3,75,0x00},
    // {0,4,76,0x00},
// };












// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------: 
extern mGPIOPinIfo sdefPin_IfrFilt_TypeB[];
static mClass_CTA*	sfPro_3518E_F09_WR_CONF(void){
	mClass_CTA	*tClassCTA;
	tClassCTA = (mClass_CTA*)kmalloc(sizeof(mClass_CTA),GFP_ATOMIC);
	gClassStr.afMemset( (uint8*)tClassCTA,  0x00, sizeof(mClass_CTA));

	// LDR
	tClassCTA->apLDR			= &gClassLDR;
	// Infrared light
	tClassCTA->apIfrLi			= &gClassIfrLi;
	// Infrared filter
	tClassCTA->apIfrFilter		= &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeB;
	tClassCTA->apIfrFilter->apPinArr->apPin = sdefPin_IfrFilt_TypeB;
	tClassCTA->apIfrFilter->apPinArr->aNum 	= 2;
	// Audio Plug
	tClassCTA->apAudioPlug 		= &gClassAudioPlug;
	// Button
	tClassCTA->apButton			= &gClassButton;
	// State Light
	tClassCTA->apStateLi		= &gClassStateLi;
	// tClassCTA->apStateLi->apPinArr->apPin = sF09_StaLi_Pin;
	// tClassCTA->apStateLi->apPinArr->aNum  = 1;
	
	return tClassCTA;
}


static int32		sfPro_3518E_F09_Init(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();
	
	tClassCTA->apLDR->afInit();
	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);
    tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);
    // tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);
	tClassCTA->apAudioPlug->afInit();
	tClassCTA->apAudioPlug->afSetStatus(DC_AuPl_Off);
	tClassCTA->apStateLi->afInit();
	// {
		// mStaLi_SetParam tTest;
		// tTest.aUnit = 0;
		// tTest.aPTime = 0xFF;
		// tTest.aNTime = 0xFF;
		// tClassCTA->apStateLi->afSetStatus(&tTest);
	// }
	
	tClassCTA->apButton->afInit();
	
	return 0;
}


static int32		sfPro_3518E_F09_Uninit(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	
	tClassCTA->apLDR->afUninit();
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apIfrFilter->afUninit();
	tClassCTA->apAudioPlug->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apButton->afUninit();
	
	kfree( tClassCTA );
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}
