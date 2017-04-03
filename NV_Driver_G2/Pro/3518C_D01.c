#include "ProInclude.h"
#include "../HAL/HAL.h"









// ================================================================================
// ---------------------------------------------------------------------->Local function definition
int32			sfPro_3518C_D01_Init(void);
int32			sfPro_3518C_D01_Uninit(void);
mClass_CTA* 	sfPro_3518C_D01_WR_CONF(void);









// ================================================================================
// ---------------------------------------------------------------------->Local Variable
mProInfo pgs3518C_D01_ProInfo={
	.aPro					= {.aKey="D01",		.aID=0xD01,},
	.aChip					= {.aKey="3518C",	.aID=0x3518C,},
	.aSubCmd				= NULL,
	.afRewriteAndConfig		= sfPro_3518C_D01_WR_CONF,
	.afInit					= sfPro_3518C_D01_Init,
	.afUninit				= sfPro_3518C_D01_Uninit,
};










// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------: sfPro_3518C_D01_WR_CONF
mClass_CTA* sfPro_3518C_D01_WR_CONF(void){
	mClass_CTA	*tClassCTA = NULL;
	// 初始化产品自己的数组
	tClassCTA = (mClass_CTA*)kmalloc(sizeof(mClass_CTA),GFP_ATOMIC);
	gClassStr.afMemset( (uint8*)tClassCTA,  0x00, sizeof(mClass_CTA));
	
	tClassCTA->apAudioPlug 	= &gClassAudioPlug;
	tClassCTA->apIfrLi			= &gClassIfrLi;
	tClassCTA->apLDR			= &gClassLDR;
	tClassCTA->apStateLi		= &gClassStateLi;	
	tClassCTA->apIfrFilter		= &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType	=	DC_IfrFltOpt_InTypeA;
	tClassCTA->apButton		= &gClassButton;
	
	return tClassCTA;
}

// -------------------------------------------------: sfPro_3518C_D01_Init
int32 sfPro_3518C_D01_Init(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();
	
	tClassCTA->apAudioPlug->afInit();
	tClassCTA->apAudioPlug->afSetStatus(DC_AuPl_Off);
	
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

// -------------------------------------------------: sfPro_3518C_D01_Uninit
int32 sfPro_3518C_D01_Uninit(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	
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
