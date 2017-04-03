#include "ProInclude.h"
#include "../HAL/HAL.h"
#include "../Tool/String.h"








// ================================================================================
// ---------------------------------------------------------------------->Local function definition
static int32		sfPro_3518C_D03_Init(void);
static int32		sfPro_3518C_D03_Uninit(void);
static mClass_CTA*	sfPro_3518C_D03_WR_CONF(void);
// state light









// ================================================================================
// ---------------------------------------------------------------------->Local Variable 
mProInfo pgs3518C_D03_ProInfo={
	.aPro					= {.aKey="D03",		.aID=0xD03,},
	.aChip					= {.aKey="3518C",	.aID=0x3518C,},
	.aSubCmd				= NULL,
	.afRewriteAndConfig		= sfPro_3518C_D03_WR_CONF,
	.afInit					= sfPro_3518C_D03_Init,
	.afUninit				= sfPro_3518C_D03_Uninit,
};

#define DC_D03_PTZHDgrPerStep		378605
#define DC_D03_PTZVDgrPerStep		703125
#define DC_D03_PTZHStepsPerFull		2184
#define DC_D03_PTZVStepsPerFull		512
#define DC_D03_PTZHLimSteps			950
#define DC_D03_PTZVLimSteps			128
#define DC_D03_PreConf              (\
							DC_PTSP_Negative\
							|DC_PTSP_LeftIsA\
							|DC_PTSP_DownIsA\
									|DC_PTSP_Supp_HMove\
									|DC_PTSP_Supp_VMove\
									|DC_PTSP_Supp_HVMove\
									|DC_PTSP_Supp_HScan\
									|DC_PTSP_Supp_VScan\
									|DC_PTSP_Supp_OriPosition\
									|DC_PTSP_Supp_CurPos\
									|DC_PTSP_Locate\
									|DC_PTSP_Supp_HLim\
									|DC_PTSP_Supp_VLim\
									|DC_PTSP_HHeadLim\
									|DC_PTSP_HTailLim\
									|DC_PTSP_VHeadLim\
									|DC_PTSP_VTailLim)
//

	
#define DC_D03_PTZDrvPinNum			8
static mGPIOPinIfo sPTZ_DrvPin[DC_D03_PTZDrvPinNum] = {
	{6,0,32,0x00}, // HA
	{6,1,33,0x00}, // HB
	{1,1,1 ,0x00}, // HC
	{6,3,35,0x00}, // HD
	{6,4,36,0x00}, // VA
	{6,5,37,0x02}, // VB
	{6,6,38,0x00}, // VC
	{6,7,39,0x00}, // VD
};

#define DC_D03_PTZLimPinNum			8
static mGPIOPinIfo sPTZ_LimPin[DC_D03_PTZLimPinNum] = {
	{2,3,9 ,0x00}, // LHHead // Origin
	{2,5,11,0x00}, // LHTail // tail
	{5,2,47,0x00}, // LVHead // Origin
	{2,2,8 ,0x00}, // LVTail // tail
};









// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------: 
static mClass_CTA*	sfPro_3518C_D03_WR_CONF(void){
	mClass_CTA	*tClassCTA;
	
	tClassCTA = (mClass_CTA*)kmalloc(sizeof(mClass_CTA),GFP_ATOMIC);
	gClassStr.afMemset( (uint8*)tClassCTA,  0x00, sizeof(mClass_CTA));
	// LDR
	tClassCTA->apLDR			= &gClassLDR;
	// Infrared Filter
	tClassCTA->apIfrFilter		= &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType	=	DC_IfrFltOpt_InTypeA;
	// Infrared light
	tClassCTA->apIfrLi			= &gClassIfrLi;
	// Audio Plug
	tClassCTA->apAudioPlug 		= &gClassAudioPlug;
	// Button
	tClassCTA->apButton			= &gClassButton;
	// State Light
	tClassCTA->apStateLi		= &gClassStateLi;
	// Night Light
	tClassCTA->apNightLi 		= &gClassNiLi;
	// Pan Tilt
	tClassCTA->apPTZ			= &gClassPTZ;
	tClassCTA->apPTZ->apDrvPinArr->apPin = sPTZ_DrvPin;
	tClassCTA->apPTZ->apDrvPinArr->aNum  = DC_D03_PTZDrvPinNum;
	tClassCTA->apPTZ->apDrvPinArr->apPin = sPTZ_LimPin;
	tClassCTA->apPTZ->apDrvPinArr->aNum  = DC_D03_PTZLimPinNum;
	gClassStr.afMemset( (uint8*)tClassCTA->apPTZ->apDefInfo,0x00,sizeof(mPanTiltInfo) );
	tClassCTA->apPTZ->apDefInfo->aHDgrPerStep    = DC_D03_PTZHDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aVDgrPerStep    = DC_D03_PTZVDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aHStepsPerFull  = DC_D03_PTZHStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aVStepsPerFull  = DC_D03_PTZVStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aHLimitSteps    = DC_D03_PTZHLimSteps;
	tClassCTA->apPTZ->apDefInfo->aVLimitSteps    = DC_D03_PTZVLimSteps;
	tClassCTA->apPTZ->apDefInfo->aRegCf          = DC_D03_PreConf;
	
	return tClassCTA;
}

static int32		sfPro_3518C_D03_Init(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();
	
	tClassCTA->apLDR->afInit();
	
	tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);
	
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
	tClassCTA->apNightLi->afInit();
	{
		mClass_NiLiIfo tSetParam;
		tSetParam.aStatus = DC_NiLi_On;
		tSetParam.aLevel  = 1; 
		tClassCTA->apNightLi->afSetStatus(&tSetParam);
	}
	
	tClassCTA->apPTZ->afInit();
	
	return 0;
}

static int32		sfPro_3518C_D03_Uninit(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	
	tClassCTA->apLDR->afUninit();
	tClassCTA->apIfrFilter->afUninit();	
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apAudioPlug->afUninit();
	tClassCTA->apButton->afUninit();
	tClassCTA->apNightLi->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apPTZ->afUninit();
	
	kfree( tClassCTA );
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}
