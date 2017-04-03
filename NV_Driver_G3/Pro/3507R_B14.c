/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    B14 即视通二 配置代码

    LDR                                   OK
    INFRARED LIGHT                        OK
    INFRARED FILTER                       OK
    STATE LIGHT                           OK
    BUTTON                                OK
    AUDIO PLUG                            OK
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
// -->LDR
static int32 sfB14_LDR_Uninit(void);
static int32 sfB14_LDR_Init(void);
// --> TLV320
extern int32 gfTLV320_Init(void);
static int32 sfB14_AuPl_GetStatus(uint8);
static int32 sfB14_AuPl_SetStatus(uint32);

static int32 sfPro_3507R_B14_Init(void);
static int32 sfPro_3507R_B14_Uninit(void);
static mClass_CTA *sfPro_3507R_B14_WR_CONF(void);
//global

//==============================================================================
//extern
//local
static mGPIOPinIfo sB14_StaLi_Pin[] = {
	{6, 0, 0, 0},
};

static mGPIOPinIfo sB14_IfrLi_Pin[] = {
	{7, 2, 0, 0},
};

static mGPIOPinIfo sB14_LDR_Pin[] = {
	{7, 3, 0, 0},		//  Detect Pin
	{7, 7, 0, 0},		//  Loop Control
};

static mGPIOPinIfo sB14_IfrFilt_Pin[] = {
	{7, 0, 0, 0},		//  DAY
	{7, 1, 0, 0},		//  NIGHT
	{7, 4, 0, 0},		//  ENABLE
};

static mGPIOPinIfo sB14_Button_Pin[] = {
	{6, 3, 0, 0},
};

//global
mProInfo pgs3507R_B14_ProInfo = {
	.aPro = {.aKey = "B14", .aID = 0xB14,},
	.aChip = {.aKey = "3507R", .aID = 0x3507,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3507R_B14_WR_CONF,
	.afInit = sfPro_3507R_B14_Init,
	.afUninit = sfPro_3507R_B14_Uninit,
};

// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------:

static mClass_CTA *sfPro_3507R_B14_WR_CONF(void)
{

	mClass_CTA *tClassCTA;
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->OnStatus = 0x01;
	tClassCTA->apStateLi->apPinArr->apPin = sB14_StaLi_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 1;

	tClassCTA->apIfrLi = &gClassIfrLi;
	tClassCTA->apIfrLi->apPinArr->apPin = sB14_IfrLi_Pin;
	tClassCTA->apIfrLi->apPinArr->aNum = 1;

	tClassCTA->apLDR = &gClassLDR;
	tClassCTA->apLDR->apPinArr->apPin = sB14_LDR_Pin;
	tClassCTA->apLDR->apPinArr->aNum = 2;
	tClassCTA->apLDR->afInit = sfB14_LDR_Init;
	tClassCTA->apLDR->afUninit = sfB14_LDR_Uninit;

	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->apPinArr->apPin = sB14_IfrFilt_Pin;
	tClassCTA->apIfrFilter->apPinArr->aNum = 3;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeC;

	tClassCTA->apButton = &gClassButton;
	tClassCTA->apButton->apPinArr->apPin = sB14_Button_Pin;
	tClassCTA->apButton->apPinArr->aNum = 1;

	tClassCTA->apRTC = &gClassRTC;

	tClassCTA->apAudioPlug = &gClassAudioPlug;
	tClassCTA->apAudioPlug->afInit = gfTLV320_Init;
	tClassCTA->apAudioPlug->afSetStatus = sfB14_AuPl_SetStatus;
	tClassCTA->apAudioPlug->afGetStatus = sfB14_AuPl_GetStatus;

	return tClassCTA;
}

static int32 sfPro_3507R_B14_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	tClassCTA->apStateLi->afInit();
	{
		mStaLi_SetParam tTest;
		tTest.aPTime = 0xFF;
		tTest.aNTime = 0x00;
		tTest.aUnit = 0;
		tClassCTA->apStateLi->afSetStatus(&tTest);
	}

	tClassCTA->apIfrLi->afInit();
	// tClassCTA->apIfrLi->afSetStatus(DC_IfLi_On);
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);

	tClassCTA->apLDR->afInit();

	tClassCTA->apIfrFilter->afInit();
	// tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);

	tClassCTA->apButton->afInit();

	tClassCTA->apRTC->afInit();

	tClassCTA->apAudioPlug->afInit();

	return 0;
}

static int32 sfPro_3507R_B14_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	tClassCTA->apStateLi->afUninit();
	tClassCTA->apIfrLi->afUninit();
	tClassCTA->apLDR->afUninit();
	tClassCTA->apIfrFilter->afUninit();
	tClassCTA->apButton->afUninit();
	tClassCTA->apRTC->afUninit();
	tClassCTA->apAudioPlug->afUninit();

	kfree(tClassCTA);
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	return 0;
}

// =============================================================================
// --------------------------------------------------> Day & Night Detected LDR
#define DC_B14_LDR_DayValue     DC_LDR_ShiftD_Value
#define DC_B14_LDR_NightValue   DC_LDR_ShiftN_Value
#define DC_B14_LDR_DAY          0x01
#define DC_B14_LDR_NIGHT        0x02

#define DC_B14_LDR_CheckFreq    10	// 1s  1000ms
extern void sfLDR_Judge(uint32 iVal);
extern void sfLDR_FilterBuf_Assimilate(uint32 iVal);
extern uint32 sfLDR_Filter(uint32 iVal);
extern void (*gfDNMonitor_StateChange)(uint8 iStatus);
//
static void sfB14_LDR_MotorCheck(void);
static void sfB14_LDR_StateChangeCallBack(uint8 iStatus);

static int32 sfB14_LDR_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	gfDNMonitor_StateChange = sfB14_LDR_StateChangeCallBack;

	gClassHAL.Pin->prfSetDirection(&tPin[0], DC_HAL_PIN_SetIN);
	gClassHAL.Pin->prfSetDirection(&tPin[1], DC_HAL_PIN_SetOUT);

	gClassHAL.Pin->prfSetExport(&tPin[1], DC_HAL_PIN_SetLOW);

	if (gClassHAL.Pin->prfGetPinValue(&tPin[0])) {
		sfLDR_FilterBuf_Assimilate(DC_B14_LDR_DayValue);
		sfLDR_Judge(DC_B14_LDR_DayValue);
	} else {
		gClassHAL.Pin->prfSetExport(&tPin[1], DC_HAL_PIN_SetHIGH);
		sfLDR_FilterBuf_Assimilate(DC_B14_LDR_NightValue);
		sfLDR_Judge(DC_B14_LDR_NightValue);
	}

	gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT, sfB14_LDR_MotorCheck,
	                                  DC_B14_LDR_CheckFreq);

#if DEBUG_INIT
	NVCPrint("The LDR module Start! OK");
#endif
	return 0;
}

static int32 sfB14_LDR_Uninit(void)
{

	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sfB14_LDR_MotorCheck);
	return 0;
}

static void sfB14_LDR_MotorCheck(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	if (gClassHAL.Pin->prfGetPinValue(tPin)) {
		sfLDR_Filter(DC_B14_LDR_DayValue);
	} else {
		sfLDR_Filter(DC_B14_LDR_NightValue);
	}
}

static void sfB14_LDR_StateChangeCallBack(uint8 iStatus)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;
	mGPIOPinIfo *tPin = tClassCTA->apLDR->apPinArr->apPin;

	if (iStatus & DC_B14_LDR_DAY) {
		// printk("Change to Day\r\n");
		gClassHAL.Pin->prfSetExport(&tPin[1], DC_HAL_PIN_SetLOW);
	} else if (iStatus & DC_B14_LDR_NIGHT) {
		// printk("Change to Night\r\n");
		gClassHAL.Pin->prfSetExport(&tPin[1], DC_HAL_PIN_SetHIGH);
	}

}

// --------------------------------------------------> Day & Night Detected LDR
#define DC_TLV320_SpeakerON     0x01
#define DC_TLV320_SpeakerOFF    0x02
#define DC_TLV320_MiCroON       0x03
#define DC_TLV320_MiCroOFF      0x04
extern int32 gfTLV320_Set(uint8 iCmd);
//
#define DC_B14_AuPl_Sta_Speaker_On  0x01
#define DC_B14_AuPl_Sta_Microph_On  0x02
static uint8 sB14_AuPl_Status = (DC_B14_AuPl_Sta_Speaker_On +
                                 DC_B14_AuPl_Sta_Microph_On);
// static uint8 sB14_AuPl_Status = 0x03;
// ==-==-==
static int32 sfB14_AuPl_SetStatus(uint32 iCmd)
{

	if (iCmd & DC_AuPl_Speaker_On) {
		gfTLV320_Set(DC_TLV320_SpeakerON);

		sB14_AuPl_Status |= DC_B14_AuPl_Sta_Speaker_On;

	} else if (iCmd & DC_AuPl_Speaker_Off) {
		gfTLV320_Set(DC_TLV320_SpeakerOFF);

		sB14_AuPl_Status &= ~DC_B14_AuPl_Sta_Speaker_On;

	}

	if (iCmd & DC_AuPl_Microph_On) {
		gfTLV320_Set(DC_TLV320_MiCroON);

		sB14_AuPl_Status |= DC_B14_AuPl_Sta_Microph_On;

	} else if (iCmd & DC_AuPl_Microph_Off) {
		gfTLV320_Set(DC_TLV320_MiCroOFF);

		sB14_AuPl_Status &= ~DC_B14_AuPl_Sta_Microph_On;

	}
	return 0;
}

static int32 sfB14_AuPl_GetStatus(uint8 iCmd)
{
	int32 tRet = 0;

	if (iCmd & DC_AuPl_Speaker) {
		if (sB14_AuPl_Status & DC_B14_AuPl_Sta_Speaker_On) {
			tRet |= DC_AuPl_Speaker_On;
		} else {
			tRet |= DC_AuPl_Speaker_Off;
		}
	}
	if (iCmd & DC_AuPl_Microph) {
		if (sB14_AuPl_Status & DC_B14_AuPl_Sta_Speaker_On) {
			tRet |= DC_AuPl_Microph_On;
		} else {
			tRet |= DC_AuPl_Microph_Off;
		}
	}

	return tRet;
}
