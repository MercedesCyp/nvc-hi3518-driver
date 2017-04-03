#include "ProInclude.h"
#include "../HAL/HAL.h"
#include "../Tool/String.h"








// ================================================================================
// ---------------------------------------------------------------------->Local function definition
static int32		sfPro_3518E_F05_Init(void);
static int32		sfPro_3518E_F05_Uninit(void);
static mClass_CTA*	sfPro_3518E_F05_WR_CONF(void);









// ================================================================================
// ---------------------------------------------------------------------->Local Variable 
mProInfo pgs3518E_F05_ProInfo={
	.aPro					= {.aKey="F05",		.aID=0xF05,},
	.aChip					= {.aKey="3518E",	.aID=0x3518E,},
	.aSubCmd				= NULL,
	.afRewriteAndConfig		= sfPro_3518E_F05_WR_CONF,
	.afInit 				= sfPro_3518E_F05_Init,
	.afUninit				= sfPro_3518E_F05_Uninit,
};

// static mGPIOPinIfo sF05_StaLi_Pin[] = {	
        // {0,3,75,0x00},
		// {0,4,76,0x00},
// };
//PTZ
#define DC_F05_PTZHDgrPerStep		527343
#define DC_F05_PTZVDgrPerStep		703125
#define DC_F05_PTZHStepsPerFull		680
#define DC_F05_PTZVStepsPerFull		170
#define DC_F05_PTZHLimSteps			700
#define DC_F05_PTZVLimSteps			200
#define DC_F05_PreConf				(\
							DC_PTSP_Positive\
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

									
#define DC_F05_PTZDrvPinNum			8
static mGPIOPinIfo sPTZ_DrvPin[DC_F05_PTZDrvPinNum] = {
	{9,0,50,0x01}, // HA
	{9,1,51,0x01}, // HB
	{9,2,52,0x01}, // HC
	{9,3,53,0x01}, // HD
	
	{9,7,57,0x01}, // VD
	{9,6,56,0x01}, // VC
	{9,5,55,0x01}, // VB
	{9,4,54,0x01}, // VA
};

#define DC_F05_PTZLimPinNum			4
static mGPIOPinIfo sPTZ_LimPin[DC_F05_PTZLimPinNum] = {
	{5,4,68,0x00}, // LHHead // Origin	
	{5,5,69,0x00}, // LHTail // tail
	{5,6,70,0x00}, // LVHead // Origin
	{5,7,71,0x00}, // LV
};










// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------: 
extern mGPIOPinIfo sdefPin_IfrFilt_TypeB[];
static mClass_CTA*	sfPro_3518E_F05_WR_CONF(void){
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
	// tClassCTA->apStateLi->apPinArr->apPin = sF05_StaLi_Pin;
	// tClassCTA->apStateLi->apPinArr->aNum  = 1;
	//PTZ
	tClassCTA->apPTZ			= &gClassPTZ;
	tClassCTA->apPTZ->apDrvPinArr->apPin = sPTZ_DrvPin;
	tClassCTA->apPTZ->apDrvPinArr->aNum  = DC_F05_PTZDrvPinNum;
	tClassCTA->apPTZ->apLimPinArr->apPin = sPTZ_LimPin;
	tClassCTA->apPTZ->apLimPinArr->aNum  = DC_F05_PTZLimPinNum;
	gClassStr.afMemset( (uint8*)tClassCTA->apPTZ->apDefInfo,0x00,sizeof(mPanTiltInfo) );
	tClassCTA->apPTZ->apDefInfo->aHDgrPerStep    = DC_F05_PTZHDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aVDgrPerStep    = DC_F05_PTZVDgrPerStep;
	tClassCTA->apPTZ->apDefInfo->aHStepsPerFull  = DC_F05_PTZHStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aVStepsPerFull  = DC_F05_PTZVStepsPerFull;
	tClassCTA->apPTZ->apDefInfo->aHLimitSteps    = DC_F05_PTZHLimSteps;
	tClassCTA->apPTZ->apDefInfo->aVLimitSteps    = DC_F05_PTZVLimSteps;
	tClassCTA->apPTZ->apDefInfo->aRegCf          = DC_F05_PreConf;
	
	return tClassCTA;
}


static int32		sfPro_3518E_F05_Init(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();
	
	tClassCTA->apLDR->afInit();
	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);
	tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);
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
	tClassCTA->apPTZ->afInit();
	
	return 0;
}


static int32		sfPro_3518E_F05_Uninit(void){
	mClass_CTA	*tClassCTA = gClassPro.apCTA;
	
	tClassCTA->apLDR->afUninit();
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apIfrFilter->afUninit();
	tClassCTA->apAudioPlug->afUninit();
	tClassCTA->apStateLi->afUninit();
	tClassCTA->apButton->afUninit();
	tClassCTA->apPTZ->afUninit();
	
	
	kfree( tClassCTA );
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	return 0;
}
