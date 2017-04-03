/*
Coder:		Chiyuan.Ma
Date:		2016-12-22

Abstract:
    机型 G09 的配置文件，第一个可视化门铃，添加了红外移动侦测和门铃模块，状态灯
    也会添加(呼吸灯模式)
初步测试
LDR             o
IfrLi           o
IfrFilt         o
AuPl            o
StaLi           o
Button          o
PIR             o
DoorBell        o
*/
//==============================================================================
// C
// Linux
// local
#include "ProInclude.h"
// remote
#include "../HAL/HAL.h"
#include "../Tool/String.h"

#include <linux/delay.h>

//=============================================================================
// DATA TYPE
typedef struct {
	uint16 aOnDgr;
	uint16 aOffDgr;
	uint16 aBrthNum;
	uint16 *apBrthDgr;
	uint32 aPTime;
	uint32 aNTime;
	uint32 aTimer;
	uint16 aOrder;
	uint8 aRunMod;
} mSL_CfgIfo;

//=============================================================================
// MACRO
// CONSTANT
#define DC_G09_StaLi_LoopFrq       1

#define DC_StaLi_PWM_Duty   3000
#define DC_StaLi_PWM_Mode   DC_PWM_CmdKeep

#define DC_G09_Static_aOnDgr           100
#define DC_G09_Static_aOffDgr          3000

#define DC_G09_DEF_Brth_Period          4

#define DC_StaLi_OptMode_ON        1
#define DC_StaLi_OptMode_OFF       2
#define DC_StaLi_OptMode_SWITCH    3
#define DC_StaLi_OptMode_BREATH    4

// FUNCTION
#define DF_StaLi_PWM_SetParam( _num,_width ) {\
		gClassHAL.PWM->pInfo->aDuty	= DC_StaLi_PWM_Duty;\
		gClassHAL.PWM->pInfo->aCmd 	= DC_StaLi_PWM_Mode;\
		gClassHAL.PWM->pInfo->aWidth = (_width);\
		gClassHAL.PWM->pInfo->aNum 	= (_num);\
	}

//==============================================================================
//extern
extern int32 gStaLiBrth_DgrArrSize(mStaLiBrth_Ifo *ipGetArr);
//local
static mClass_CTA *sfPro_3518E_G09_WR_CONF(void);
static int32 sfPro_3518E_G09_Init(void);
static int32 sfPro_3518E_G09_Uninit(void);
//
static int32 sfdefG09_StateLight_Init(void);
static int32 sfdefG09_StateLight_Uninit(void);
static void sfdefG09_StaLi_InitParam(mStaLi_InitParrm *ipStaIfo);
static void sfdefG09_StaLi_SetStatus(mStaLi_SetParam *ipSetIfo);
static void sfdefG09_StaLi_StartMode(uint8 iNum);
static void sfdefG09_StaLi_LoopProcessing(void);
//global

//==============================================================================
//extern
extern mGPIOPinIfo sdefPin_IfrFilt_TypeB[];
//local
static mGPIOPinIfo sG09_Button_Pin[] = {
	{0, 1, 73, 0x00},
	{1, 7, 31, 0x00},
};

// 在这里状态灯需要做成呼吸灯的样子，因此这里将 IO 口初始化成 PWM 的输出口。
static mGPIOPinIfo sG09_StateLight_Pin[] = {
	{5, 3, 48, 0x00},
	{5, 2, 47, 0x00},
};

// State Light
static mSL_CfgIfo *s0302_StaLi_CfgIfo;
static uint8 sG09_StaLi_LiNum = 0;
// option State Light shine upon Chip PWM port
static uint8 sStaLi_PWM_SU_List[2];

//global
mProInfo pgs3518E_G09_ProInfo = {
	.aPro = {.aKey = "G09", .aID = 0x1009,},
	.aChip = {.aKey = "3518E", .aID = 0x3518E,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518E_G09_WR_CONF,
	.afInit = sfPro_3518E_G09_Init,
	.afUninit = sfPro_3518E_G09_Uninit,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static mClass_CTA*	sfPro_3518E_G09_WR_CONF(void)
@introduction:

@parameter:

@return:

*/
static mClass_CTA *sfPro_3518E_G09_WR_CONF(void)
{
	mClass_CTA *tClassCTA;
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	// LDR
	tClassCTA->apLDR = &gClassLDR;
	// Infrared light
	tClassCTA->apIfrLi = &gClassIfrLi;
	// Infrared filter
	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeB;
	tClassCTA->apIfrFilter->apPinArr->apPin = sdefPin_IfrFilt_TypeB;
	tClassCTA->apIfrFilter->apPinArr->aNum = 2;
	// Audio Plug
	tClassCTA->apAudioPlug = &gClassAudioPlug;
	// Button
	tClassCTA->apButton = &gClassButton;
	tClassCTA->apButton->apPinArr->apPin = sG09_Button_Pin;
	tClassCTA->apButton->apPinArr->aNum = 2;

	// State Light
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = sG09_StateLight_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 2;
	tClassCTA->apStateLi->afInit = sfdefG09_StateLight_Init;
	tClassCTA->apStateLi->afInitParam = sfdefG09_StaLi_InitParam;
	tClassCTA->apStateLi->afUninit = sfdefG09_StateLight_Uninit;
	tClassCTA->apStateLi->afSetStatus = sfdefG09_StaLi_SetStatus;

	// PIR
	tClassCTA->apPIR = &gClassPIR;
	// Door Bell
	tClassCTA->apDoorBell = &gClassDoorBell;
	// Door Lock
	tClassCTA->apDoorLock = &gClassDoorLock;
	// Hardware AEC
	tClassCTA->apFM1288 = &gClassFM1288;

	return tClassCTA;
}

//---------- ---------- ---------- ----------
/*  static int32		sfPro_3518E_G09_Init(void)
@introduction:

@parameter:

@return:

*/
static int32 sfPro_3518E_G09_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	// HAL
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	// LDR
	tClassCTA->apLDR->afInit();

	// Infrared Light
	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);

	// Infrared filter
	tClassCTA->apIfrFilter->afInit();
	//ssleep(2);
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);
	//tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);

	// Audio Plug
	tClassCTA->apAudioPlug->afInit();
	tClassCTA->apAudioPlug->afSetStatus(DC_AuPl_Speaker_Off);

	// State Light
	tClassCTA->apStateLi->afInit();
	{
		mStaLi_InitParrm tStaLi_InitParam;
		mStaLiBrth_Ifo tBrth_Dgr;

		tBrth_Dgr.aNum = 1;
		gStaLiBrth_DgrArrSize(&tBrth_Dgr);

		tStaLi_InitParam.aOnDgr = DC_G09_Static_aOnDgr;
		tStaLi_InitParam.aOffDgr = DC_G09_Static_aOffDgr;
		tStaLi_InitParam.aBrthNum = tBrth_Dgr.aSize;
		tStaLi_InitParam.apBrthDgr = tBrth_Dgr.apArr;
		tStaLi_InitParam.aUnit = 0;
		tClassCTA->apStateLi->afInitParam(&tStaLi_InitParam);
		tStaLi_InitParam.aUnit = 1;
		tClassCTA->apStateLi->afInitParam(&tStaLi_InitParam);
	}
	// {
	// mStaLi_SetParam tTest;
	// tTest.aMode = DC_StaLi_Mod_DEFAULT;
	// tTest.aMode = DC_StaLi_Mod_BREATH;

	// tTest.aUnit = 1;
	// tTest.aPTime = 0;
	// tTest.aNTime = 0;

	// tClassCTA->apStateLi->afSetStatus(&tTest);
	// }

	// Buuton
	tClassCTA->apButton->afInit();

	// PIR
	tClassCTA->apPIR->afInit();

	// Door Bell
	tClassCTA->apDoorBell->afInit();
	// 加载驱动时会产生一个脉冲到433，测试是否会起作用
	// tClassCTA->apDoorBell->afSetStatus( DC_CTADoorBell_TapBell );

	// Door Lock
	tClassCTA->apDoorLock->afInit();

	// Door Lock
	tClassCTA->apFM1288->afInit();

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32		sfPro_3518E_G09_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfPro_3518E_G09_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	// LDR
	tClassCTA->apLDR->afUninit();

	// Infrared Light
	tClassCTA->apIfrLi->afUninit();

	// Infrared filter
	tClassCTA->apIfrFilter->afUninit();

	// Audio Plug
	tClassCTA->apAudioPlug->afUninit();

	// State light
	tClassCTA->apStateLi->afUninit();

	// Button
	tClassCTA->apButton->afUninit();

	// PIR
	tClassCTA->apPIR->afUninit();

	// Door Bell
	tClassCTA->apDoorBell->afUninit();

	// Door Lock
	tClassCTA->apDoorLock->afUninit();

	// HAL
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	kfree(tClassCTA);

	return 0;
}

//==============================================================================
//Others
//----------------------------------------------------------- MODULE State Light
/*   static int32 sfdefG09_StateLight_Init(void)
@introduction:
    PWM 占空比越大，亮度越低，当占空比与周期的宽度相同时，灯的亮度达到0

@parameter:
    void

@return:
    0		Success
    -1      failed

*/
static int32 sfdefG09_StateLight_Init(void)
{
	uint8 _i;
	uint8 tPwmSumChannel;
	mGPIOPinIfo *tPwmPin;
	int32 tPWMChannel;

	// Init State Light
	tPwmPin = gClassStateLi.apPinArr->apPin;
	sG09_StaLi_LiNum = gClassStateLi.apPinArr->aNum;
	tPwmSumChannel = gClassHAL.PWM->pInfo->aSumChannel;
	if (sG09_StaLi_LiNum > tPwmSumChannel) {
		return -1;
	}

	s0302_StaLi_CfgIfo =
	    (mSL_CfgIfo *) kmalloc(sizeof(mSL_CfgIfo) * tPwmSumChannel,
	                           GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) s0302_StaLi_CfgIfo, 0x00,
	                   sizeof(mSL_CfgIfo) * tPwmSumChannel);

	for (_i = 0; _i < sG09_StaLi_LiNum; _i++) {
		tPWMChannel = gClassHAL.PWM->prfGCFPin(&tPwmPin[_i]);
		if (tPWMChannel < 0) {
			return -1;
		} else {
			sStaLi_PWM_SU_List[_i] = tPWMChannel;
#if DEBUG_INIT
			NVCPrint("The State Light Init PWM Channel %d. OK!",
			         (int)tPWMChannel);
#endif
		}
	}

#if DEBUG_INIT
	NVCPrint("The State Light Init module Start. OK!");
#endif
	return 0;
}

static int32 sfdefG09_StateLight_Uninit(void)
{
	uint8 _i;
	uint8 tPwmSumChannel;
	mGPIOPinIfo *tPwmPin;
	int32 tPWMChannel;

	tPwmPin = gClassStateLi.apPinArr->apPin;
	tPwmSumChannel = gClassHAL.PWM->pInfo->aSumChannel;

	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_INT,
	                                sfdefG09_StaLi_LoopProcessing);

	for (_i = 0; _i < sG09_StaLi_LiNum; _i++) {
		tPWMChannel = gClassHAL.PWM->prfGCFPin(&tPwmPin[_i]);
		DF_StaLi_PWM_SetParam(tPWMChannel,
		                      s0302_StaLi_CfgIfo[tPWMChannel].aOffDgr);
		gClassHAL.PWM->prfStart();
	}

	kfree(s0302_StaLi_CfgIfo);
	return 0;
}

static void sfdefG09_StaLi_InitParam(mStaLi_InitParrm *ipStaIfo)
{
	uint8 _j;
	_j = sStaLi_PWM_SU_List[ipStaIfo->aUnit];
	s0302_StaLi_CfgIfo[_j].aOnDgr = ipStaIfo->aOnDgr;
	s0302_StaLi_CfgIfo[_j].aOffDgr = ipStaIfo->aOffDgr;
	s0302_StaLi_CfgIfo[_j].aBrthNum = ipStaIfo->aBrthNum;
	s0302_StaLi_CfgIfo[_j].apBrthDgr = ipStaIfo->apBrthDgr;

	if (ipStaIfo->aUnit == 0) {
		s0302_StaLi_CfgIfo[_j].aRunMod = DC_StaLi_OptMode_ON;
	} else {
		s0302_StaLi_CfgIfo[_j].aRunMod = DC_StaLi_OptMode_OFF;
	}
	DF_StaLi_PWM_SetParam(_j, s0302_StaLi_CfgIfo[_j].aOffDgr);
	sfdefG09_StaLi_StartMode(_j);
}

static void sfdefG09_StaLi_SetStatus(mStaLi_SetParam *ipSetIfo)
{
	uint8 _j;

	if (ipSetIfo->aUnit >= 2) {
		return;
	}

	_j = sStaLi_PWM_SU_List[ipSetIfo->aUnit];
	s0302_StaLi_CfgIfo[_j].aPTime = ipSetIfo->aPTime;
	s0302_StaLi_CfgIfo[_j].aNTime = ipSetIfo->aNTime;

	switch (ipSetIfo->aMode) {
	case DC_StaLi_Mod_DEFAULT:

		if ((s0302_StaLi_CfgIfo[_j].aPTime == 0)
		    || ((s0302_StaLi_CfgIfo[_j].aPTime == 0)
		        && (s0302_StaLi_CfgIfo[_j].aNTime == 0))) {

			s0302_StaLi_CfgIfo[_j].aRunMod = DC_StaLi_OptMode_OFF;

		} else if (s0302_StaLi_CfgIfo[_j].aNTime == 0) {
			s0302_StaLi_CfgIfo[_j].aRunMod = DC_StaLi_OptMode_ON;
		} else {
			s0302_StaLi_CfgIfo[_j].aRunMod =
			    DC_StaLi_OptMode_SWITCH;
		}
		break;
	case DC_StaLi_Mod_BREATH:
		if (s0302_StaLi_CfgIfo[_j].aPTime == 0) {
			s0302_StaLi_CfgIfo[_j].aPTime = DC_G09_DEF_Brth_Period;
		} else if (s0302_StaLi_CfgIfo[_j].aPTime > 10) {
			return;
		}
		s0302_StaLi_CfgIfo[_j].aRunMod = DC_StaLi_OptMode_BREATH;
		break;
	default:

		s0302_StaLi_CfgIfo[_j].aRunMod = DC_StaLi_OptMode_OFF;

		break;
	}
	sfdefG09_StaLi_StartMode(_j);
	return;
}

static void sfdefG09_StaLi_StartMode(uint8 iPwmChnl)
{
	uint8 _i;

	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_INT,
	                                sfdefG09_StaLi_LoopProcessing);

	if (s0302_StaLi_CfgIfo[iPwmChnl].aRunMod == DC_StaLi_OptMode_ON) {
		DF_StaLi_PWM_SetParam(iPwmChnl,
		                      s0302_StaLi_CfgIfo[iPwmChnl].aOnDgr);
		gClassHAL.PWM->prfStart();

	} else if (s0302_StaLi_CfgIfo[iPwmChnl].aRunMod == DC_StaLi_OptMode_OFF) {
		DF_StaLi_PWM_SetParam(iPwmChnl,
		                      s0302_StaLi_CfgIfo[iPwmChnl].aOffDgr);
		gClassHAL.PWM->prfStart();
	} else {
		s0302_StaLi_CfgIfo[iPwmChnl].aTimer = 0;
		s0302_StaLi_CfgIfo[iPwmChnl].aOrder = 0;
	}

	for (_i = 0; _i < gClassHAL.PWM->pInfo->aSumChannel; _i++) {
		if ((s0302_StaLi_CfgIfo[_i].aRunMod == DC_StaLi_OptMode_SWITCH)
		    || (s0302_StaLi_CfgIfo[_i].aRunMod ==
		        DC_StaLi_OptMode_BREATH)) {

			gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_INT,
			                                  sfdefG09_StaLi_LoopProcessing,
			                                  DC_G09_StaLi_LoopFrq);
			break;
		}
	}
}

static void sfdefG09_StaLi_LoopProcessing(void)
{
	uint8 _i, _j;
	for (_i = 0; _i < sG09_StaLi_LiNum; _i++) {

		_j = sStaLi_PWM_SU_List[_i];

		if (s0302_StaLi_CfgIfo[_j].aRunMod == DC_StaLi_OptMode_SWITCH) {

			if (s0302_StaLi_CfgIfo[_j].aTimer) {
				s0302_StaLi_CfgIfo[_j].aTimer--;
			} else {
				if (s0302_StaLi_CfgIfo[_j].aOrder % 2 == 0) {

					s0302_StaLi_CfgIfo[_j].aTimer =
					    s0302_StaLi_CfgIfo[_j].aPTime;
					DF_StaLi_PWM_SetParam(_j,
					                      s0302_StaLi_CfgIfo
					                      [_j].aOnDgr);

				} else {

					s0302_StaLi_CfgIfo[_j].aTimer =
					    s0302_StaLi_CfgIfo[_j].aNTime;
					DF_StaLi_PWM_SetParam(_j,
					                      s0302_StaLi_CfgIfo
					                      [_j].aOffDgr);

				}
				gClassHAL.PWM->prfStart();

				s0302_StaLi_CfgIfo[_j].aOrder++;
			}
		} else if (s0302_StaLi_CfgIfo[_j].aRunMod ==
		           DC_StaLi_OptMode_BREATH) {

			if (s0302_StaLi_CfgIfo[_j].aTimer) {
				s0302_StaLi_CfgIfo[_j].aTimer--;
			} else {

				s0302_StaLi_CfgIfo[_j].aTimer =
				    s0302_StaLi_CfgIfo[_j].aPTime;
				if ((s0302_StaLi_CfgIfo[_j].aOrder /
				     s0302_StaLi_CfgIfo[_j].aBrthNum) % 2) {

					DF_StaLi_PWM_SetParam(_j,
					                      s0302_StaLi_CfgIfo
					                      [_j].apBrthDgr
					                      [s0302_StaLi_CfgIfo
					                       [_j].aBrthNum -
					                       1 -
					                       s0302_StaLi_CfgIfo
					                       [_j].aOrder %
					                       s0302_StaLi_CfgIfo
					                       [_j].aBrthNum]);
				} else {
					DF_StaLi_PWM_SetParam(_j,
					                      s0302_StaLi_CfgIfo
					                      [_j].apBrthDgr
					                      [s0302_StaLi_CfgIfo
					                       [_j].aOrder %
					                       s0302_StaLi_CfgIfo
					                       [_j].aBrthNum]);
				}
				gClassHAL.PWM->prfStart();

				s0302_StaLi_CfgIfo[_j].aOrder++;
				if (s0302_StaLi_CfgIfo[_j].aOrder ==
				    s0302_StaLi_CfgIfo[_j].aBrthNum * 2) {
					s0302_StaLi_CfgIfo[_j].aOrder = 0;
				}
			}
		}
	}
}
