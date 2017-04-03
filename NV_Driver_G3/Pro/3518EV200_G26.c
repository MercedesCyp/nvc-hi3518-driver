/*
Coder:		Chiyuan.Ma
Date:		2017-02-10
Platform:	Hi3518EV200
Device:		G26

Abstract:
    机型 G26 的配置文件，可视化门铃项目
    IfrFilt(IRCUT)
		IRC1( GPIO3_7,switch,low_pass,high_block )
		IRC2( GPIO4_0,enable,low_en,high_disen )
	IfrLED
		GPIO4_5 H-on L-off
	AudioSwitch
		GPIO8_1 H_off L_on
	button
		Visitor button( GPIO7_0, high_normal, low_press )
			after high to low to high, it should send a notify to app.
		Wifi button( GPIO7_7 high_normal, low_press  )
	LDR
		ADC 1st channel( ADC_CH0 ).
	PIR
		GPIO7_4
	DoorBell
		GPIO7_1
	DoorLock
		GPIO4_2 high_open low_close
	State LED
		(State LED) GPIO7_2 high_on low_off
		(State LED) GPIO7_3 high_on low_off
	FM1288
		IIC_SCL( GPIO3_5 )
		IIC_SDA( GPIO3_6 )
初步测试
IfrFilter				1
IfrLED					1
AudioPlug				1
Button(0 WIFI)				1
Button(1 Visitor)			1
Button(2 Remove det)			0
LDR					1
PIR					1
DoorBell				1
DoorLock				1
StateLED				1
FM1288					1
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
// DATA TYPE

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION

//==============================================================================
//extern
//local
static mClass_CTA *sfPro_3518EV200_G26_WR_CONF(void);
static int32 sfPro_3518EV200_G26_Init(void);
static int32 sfPro_3518EV200_G26_Uninit(void);

#define G26_LDR_DLim			50
#define G26_LDR_NLim			30

//global

//==============================================================================
//extern
//extern mGPIOPinIfo sdefPin_IfrFilt_TypeB[];
//local

static mGPIOPinIfo sG26_IfrFilter_Pin[] = {
	{3, 7, 20, 0x00},	// IRC switch
	{4, 0, 21, 0x00},	// IRC enable(N)
};

static mGPIOPinIfo sG26_IfrLED_Pin[] = {
	{4, 5, 26, 0x00},
};

static mGPIOPinIfo sG26_Button_Pin[] = {
	{7, 7, 63, 0x01},	// Wifi button
	{7, 0, 56, 0x00},	// Visitor button
	//{4, 6, 27, 0x00},     // Remove det
};

static mGPIOPinIfo sG26_StateLED_Pin[] = {
	{7, 3, 59, 0x01},	// State LED main
	{7, 2, 58, 0x01},	// State LED secend
};

static mGPIOPinIfo sG26_AudioPlug_Pin[] = {
	{8, 1, 65, 0x01},
};

static mGPIOPinIfo sG26_PIR_Pin[] = {
	{7, 4, 60, 0x01},	// PIR
};

static mGPIOPinIfo sG26_DoorBell_Pin[] = {
	{7, 1, 57, 0x00},	// Indoor bell
};

static mGPIOPinIfo sG26_DoorLock_Pin[] = {
	{4, 2, 23, 0x00},
};

static mGPIOPinIfo sG26_FM1288_Pin[] = {
	{3, 5, 18, 0x00},	// FM1288 IIC SCL
	{3, 6, 19, 0x00},	// FM1288 IIC SDA
};

/*
static uint8 u8G26_FM1288_InitData[870] = {
	// FM1288 Init Data
};
static uint32 u32G26_Fm1288_InitDataLen = sizeof(u8G26_FM1288_InitData);
*/

//global
mProInfo pgs3518EV200_G26_ProInfo = {
	.aPro = {.aKey = "G26", .aID = 0x1026,},
	.aChip = {.aKey = "3518EV200", .aID = 0x3518E200,},
	.aSubCmd = NULL,
	.afRewriteAndConfig = sfPro_3518EV200_G26_WR_CONF,
	.afInit = sfPro_3518EV200_G26_Init,
	.afUninit = sfPro_3518EV200_G26_Uninit,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static mClass_CTA*	sfPro_3518EV200_G26_WR_CONF(void)
@introduction:

@parameter:

@return:

*/
static mClass_CTA *sfPro_3518EV200_G26_WR_CONF(void)
{
	mClass_CTA *tClassCTA;
	tClassCTA = (mClass_CTA *) kmalloc(sizeof(mClass_CTA), GFP_ATOMIC);
	gClassStr.afMemset((uint8 *) tClassCTA, 0x00, sizeof(mClass_CTA));

	// LDR
	tClassCTA->apLDR = &gClassLDR;
	tClassCTA->apLDR->afSetDNLimit(G26_LDR_DLim, G26_LDR_NLim);

	// Infrared LED
	tClassCTA->apIfrLi = &gClassIfrLi;
	tClassCTA->apIfrLi->apPinArr->apPin = sG26_IfrLED_Pin;
	tClassCTA->apIfrLi->apPinArr->aNum = 1;

	// Infrared filter
	tClassCTA->apIfrFilter = &gClassIfrFilt;
	tClassCTA->apIfrFilter->aType = DC_IfrFltOpt_TypeB;
	tClassCTA->apIfrFilter->apPinArr->apPin = sG26_IfrFilter_Pin;
	tClassCTA->apIfrFilter->apPinArr->aNum = 2;

	// Audio Plug
	tClassCTA->apAudioPlug = &gClassAudioPlug;
	tClassCTA->apAudioPlug->apPinArr->apPin = sG26_AudioPlug_Pin;
	tClassCTA->apAudioPlug->apPinArr->aNum = 1;
	tClassCTA->apAudioPlug->OnStatus = 0;

	// Button
	tClassCTA->apButton = &gClassButton;
	tClassCTA->apButton->apPinArr->apPin = sG26_Button_Pin;
	tClassCTA->apButton->apPinArr->aNum = 2;

	// State LED
	tClassCTA->apStateLi = &gClassStateLi;
	tClassCTA->apStateLi->apPinArr->apPin = sG26_StateLED_Pin;
	tClassCTA->apStateLi->apPinArr->aNum = 2;
	tClassCTA->apStateLi->OnStatus = 1;

	// PIR
	tClassCTA->apPIR = &gClassPIR;
	tClassCTA->apPIR->apPinArr->apPin = sG26_PIR_Pin;
	tClassCTA->apPIR->apPinArr->aNum = 1;

	// Door Bell
	tClassCTA->apDoorBell = &gClassDoorBell;
	tClassCTA->apDoorBell->apPinArr->apPin = sG26_DoorBell_Pin;
	tClassCTA->apDoorBell->apPinArr->aNum = 1;

	// Door Lock
	tClassCTA->apDoorLock = &gClassDoorLock;
	tClassCTA->apDoorLock->apPinArr->apPin = sG26_DoorLock_Pin;
	tClassCTA->apDoorLock->apPinArr->aNum = 1;

	// Hardware AEC FM1288
	tClassCTA->apFM1288 = &gClassFM1288;
	tClassCTA->apFM1288->apPinArr->apPin = sG26_FM1288_Pin;
	tClassCTA->apFM1288->apPinArr->aNum = 2;

	return tClassCTA;
}

//---------- ---------- ---------- ----------
/*  static int32		sfPro_3518EV200_G26_Init(void)
@introduction:

@parameter:

@return:

*/
static int32 sfPro_3518EV200_G26_Init(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	// HAL
	gClassHAL.ADC->prfInit();
	gClassHAL.Timer->prfInit();
	gClassHAL.PeriodEvent->afInit();

	// LDR
	tClassCTA->apLDR->afInit();

	// Infrared LED
	tClassCTA->apIfrLi->afInit();
	tClassCTA->apIfrLi->afSetStatus(DC_IfLi_Off);

	// Infrared filter
	tClassCTA->apIfrFilter->afInit();
	tClassCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);

	// Audio Plug
	tClassCTA->apAudioPlug->afInit();
	tClassCTA->apAudioPlug->afSetStatus(DC_AuPl_Speaker_Off);

	// Button
	tClassCTA->apButton->afInit();

	// State LED
	tClassCTA->apStateLi->afInit();

	// PIR
	tClassCTA->apPIR->afInit();

	// Door Bell
	tClassCTA->apDoorBell->afInit();

	// Door Lock
	tClassCTA->apDoorLock->afInit();

	// FM1288
	tClassCTA->apFM1288->afInit();

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32		sfPro_3518EV200_G26_Uninit(void)
@introduction:

@parameter:

@return:

*/
static int32 sfPro_3518EV200_G26_Uninit(void)
{
	mClass_CTA *tClassCTA = gClassPro.apCTA;

	// LDR
	tClassCTA->apLDR->afUninit();
	// Infrared LED
	tClassCTA->apIfrLi->afUninit();
	// Infrared filter
	tClassCTA->apIfrFilter->afUninit();
	// Audio Plug
	tClassCTA->apAudioPlug->afUninit();
	// Button
	tClassCTA->apButton->afUninit();
	// State LED
	tClassCTA->apStateLi->afUninit();
	// PIR
	tClassCTA->apPIR->afUninit();
	// Door Bell
	tClassCTA->apDoorBell->afUninit();
	// Door Lock
	tClassCTA->apDoorLock->afUninit();
	// FM1288
	tClassCTA->apFM1288->afUninit();

	// HAL
	gClassHAL.PeriodEvent->afUninit();
	gClassHAL.Timer->prfUninit();
	gClassHAL.ADC->prfUninit();
	kfree(tClassCTA);
	return 0;
}
