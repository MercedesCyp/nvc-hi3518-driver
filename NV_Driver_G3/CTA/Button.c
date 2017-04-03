/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
#include "CTA.h"
// remote
#include "../HAL/HAL.h"
#include "../Tool/String.h"
#include "../GlobalParameter.h"

//=============================================================================
// DATA TYPE
typedef struct {
	uint8 aNum;
	mGPIOPinIfo *aPin;
} mButPin;

typedef struct {
	uint8 aUnit;
	mButPin *aPinIfo;
} mButIntGrp_Info;

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
#define DC_Button_JitterDelayBase		10	//50ms
#define DC_Button_JitterDelay			5	//50ms
#define DC_Button_ReportDelay			1	//100ms

#define DF_SET_DetectIntFall(_Pin)	gClassHAL.Pin->prfPin_ConfigInt((_Pin),DC_HAL_PIN_IntFallEdge)
#define DF_SET_DetectIntRase(_Pin)	gClassHAL.Pin->prfPin_ConfigInt((_Pin),DC_HAL_PIN_IntRiseEdge)
#define DF_SET_ClrIntState(_Pin)	gClassHAL.Pin->prfPin_ConfigInt((_Pin),DC_HAL_PIN_IntClrState)
#define DF_JDG_IsIntExist(_Pin)		gClassHAL.Pin->prfIsIntStateExist(_Pin)
#define DF_GET_PinValue(_Pin)		gClassHAL.Pin->prfGetPinValue(_Pin)

#define DC_ButtonSTA_Update			0x80	//   0:                 1: update
#define DC_ButtonSTA_TarDetect			0x40	//   0: Low             1: High
#define DC_ButtonSTA_CurLevel			0x20	//   0: Low             1: High
#define DC_ButtonSTA_Report			0x08
#define DC_ButtonSTA_RpotSta			0x10	//   0: fall    1: rise
#define DF_ButtonSTA_CountClr(_x)	{ (_x) &= 0x00FFFFFF; }
#define DF_ButtonSTA_CountInc(_x)	do{	\
		if( ((_x)&0xFF000000) == 0xFF000000 )\
			(_x) &= 0x00FFFFFF;\
		else \
			(_x) += 0x01000000; \
	}while(0)

#define DF_ButtonSTA_CountJudge(_x,_y)	( (_x&0xFF000000) >= (_y<<24)?1:0 )

//==============================================================================
//extern
//local
static int32 sfdefButton_Init(void);
static int32 sfdefButton_Uninit(void);
static int32 sfdefBut_GetStatus(mButton_IntBackParam *);
static void sfdefBut_RegsiterCallBack(void (*Handle)(mButton_IntBackParam *));
static void sintf_GPIO_Grp1Function(void);
static void sintf_GPIO_Grp2Function(void);
static void sintf_GPIO_Grp3Function(void);
static void sfBut_Reporter(void);
static void sfBut_JittersElimination(void);
//global

//==============================================================================
//extern
//local
static mButton_DEFAULT sdefBut_INFO = {
	.adefLEVE = DC_BuMoDef_High
};

static mGPIOPinIfo sdefPin_Buttton[] = {
	{0, 1, 73, 0x00},
	{1, 7, 31, 0x00},
	{5, 4, 68, 0x00},
};

static mGPIOPinArr sdefPinArr_Button = {
	.apPin = sdefPin_Buttton,
	.aNum = 3,
};

static uint32 *sdefButton_Status;

static mButIntGrp_Info sdefButGrp_[3];
static void (*sfBut_ReportCallBack)(mButton_IntBackParam *);
static void (*tJitters)(void) = NULL;
//global
mClass_Button gClassButton = {
	.apDefs = &sdefBut_INFO,
	.apPinArr = &sdefPinArr_Button,
	.afInit = sfdefButton_Init,
	.afUninit = sfdefButton_Uninit,
	.afGetStatus = sfdefBut_GetStatus,
	.afRegCallBack = sfdefBut_RegsiterCallBack,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static 	int32	sfdefButton_Init(void)
@introduction:
    初始化按键

@parameter:
    void

@return:
    0   SUCCESS
    -1  FAIL
*/
static int32 sfdefButton_Init(void)
{

	uint8 _i;
	uint8 tGrp_n[3];
	uint8 tbutNum = gClassButton.apPinArr->aNum;
	uint8 tdefStatus = gClassButton.apDefs->adefLEVE;
	mGPIOPinIfo *tbutPin = gClassButton.apPinArr->apPin;
	uint8 tGrp_nNum;

	gClassStr.afMemset((uint8 *) & sdefButGrp_, 0x00,
	                   sizeof(mButIntGrp_Info) * 3);

	// Request and initial the register of memory to record the button state;
	{
		sdefButton_Status =
		    (uint32 *) kmalloc(sizeof(uint32) * tbutNum, GFP_ATOMIC);
		gClassStr.afMemset((uint8 *) sdefButton_Status, 0x00,
		                   tbutNum * sizeof(uint32));
		for (_i = 0; _i < tbutNum; _i++) {
			if (tdefStatus == DC_BuMoDef_High) {
				sdefButton_Status[_i] = DC_ButtonSTA_CurLevel;
			}
		}
	}

	// Figure out the pin sum number of each GPIO INTERRUPTER GROUP
	gClassStr.afMemset(tGrp_n, 0x00, 3);
	for (_i = 0; _i < tbutNum; _i++) {
		uint8 tGrp_nNum;
		// Assignment the Pin to corresponding group.
		tGrp_nNum = gClassHAL.Pin->prfWhichGrp(&tbutPin[_i]);
		tGrp_n[tGrp_nNum]++;
	}
#if DEBUG_BUTTON
	NVCPrint("tGrp_n[0]: %d", (int)tGrp_n[0]);
	NVCPrint("tGrp_n[1]: %d", (int)tGrp_n[1]);
	NVCPrint("tGrp_n[2]: %d", (int)tGrp_n[2]);
#endif

	// Analysis each GROUP sum number of button pin and distribution the same amount of memory.
	for (_i = 0; _i < 3; _i++) {
		if (tGrp_n[_i] != 0) {
			sdefButGrp_[_i].aUnit = tGrp_n[_i];
			sdefButGrp_[_i].aPinIfo =
			    (mButPin *) kmalloc(sizeof(mButPin) * tGrp_n[_i],
			                        GFP_ATOMIC);
		}
	}

	// Build the button pin to corresponding GROUP's pin node
	gClassStr.afMemset(tGrp_n, 0x00, 3);
	for (_i = 0; _i < tbutNum; _i++) {
		tGrp_nNum = gClassHAL.Pin->prfWhichGrp(&tbutPin[_i]);
		sdefButGrp_[tGrp_nNum].aPinIfo[tGrp_n[tGrp_nNum]].aPin =
		    &(tbutPin[_i]);
		sdefButGrp_[tGrp_nNum].aPinIfo[tGrp_n[tGrp_nNum]].aNum = _i;
		tGrp_n[tGrp_nNum]++;
	}
#if DEBUG_BUTTON
	NVCPrint("sdefButGrp_[ 0 ].aUnit = %d", sdefButGrp_[0].aUnit);
	NVCPrint("sdefButGrp_[ 1 ].aUnit = %d", sdefButGrp_[1].aUnit);
	NVCPrint("sdefButGrp_[ 2 ].aUnit = %d", sdefButGrp_[2].aUnit);
#endif
	// Initial The Interrupter
	if (sdefButGrp_[0].aUnit != 0) {
		if (gClassHAL.Pin->prfPin_RegistInt(DC_HAL_PIN_Grp0,
		                                    sintf_GPIO_Grp1Function)) {
			return -1;
		}
	}
	if (sdefButGrp_[1].aUnit != 0) {
		if (gClassHAL.Pin->prfPin_RegistInt(DC_HAL_PIN_Grp1,
		                                    sintf_GPIO_Grp2Function)) {
			return -1;
		}
	}
	if (sdefButGrp_[2].aUnit != 0) {
		if (gClassHAL.Pin->prfPin_RegistInt(DC_HAL_PIN_Grp2,
		                                    sintf_GPIO_Grp3Function)) {
			return -1;
		}
	}
	// Initial the pin to used in as button input.
	for (_i = 0; _i < tbutNum; _i++) {	// Initial The pin
		gClassHAL.Pin->prfPin_ConfigInt(&tbutPin[_i]
		                                , DC_HAL_PIN_IntFallEdge
		                                | DC_HAL_PIN_IntIN
		                                | DC_HAL_PIN_IntClrState
		                                | DC_HAL_PIN_IntEnable);
	}

#if DEBUG_INIT
	NVCPrint("The Button module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static 	int32	sfdefButton_Uninit(void)
@introduction:
    释放 button 初始化时注册的资源

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefButton_Uninit(void)
{
	// uint8 _i;

	//gClassHAL.Pin->prfClrConfig();
	if (sdefButGrp_[0].aUnit != 0) {
		gClassHAL.Pin->prfPin_CancelRegistInt(DC_HAL_PIN_Grp0,
		                                      sintf_GPIO_Grp1Function);
		kfree(sdefButGrp_[0].aPinIfo);
	}

	if (sdefButGrp_[1].aUnit != 0) {
		gClassHAL.Pin->prfPin_CancelRegistInt(DC_HAL_PIN_Grp1,
		                                      sintf_GPIO_Grp2Function);
		kfree(sdefButGrp_[1].aPinIfo);
	}

	if (sdefButGrp_[2].aUnit != 0) {
		gClassHAL.Pin->prfPin_CancelRegistInt(DC_HAL_PIN_Grp2,
		                                      sintf_GPIO_Grp3Function);
		kfree(sdefButGrp_[2].aPinIfo);
	}

	kfree(sdefButton_Status);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefBut_GetStatus(mButton_IntBackParam* iButInfo)
@introduction:
    主动获取按键当前状态，当前按键只可能处于两个状态，按下和未按下，这里通过状态
    位获取

@parameter:
    iButInfo
        获取到得状态会返回带此指针指向的数组中

@return:
    0   SUCCESS

*/
static int32 sfdefBut_GetStatus(mButton_IntBackParam *iButInfo)
{

	if (sdefButton_Status[iButInfo->aUnit] & DC_ButtonSTA_CurLevel) {
		iButInfo->aStatus = DC_BuMoSta_HIGHG;
	} else {
		iButInfo->aStatus = DC_BuMoSta_LOW;
	}

	return 0;
}

//---------- ---------- ---------- ----------
/*  asjkdaskdjasdk
@introduction:
    注册上报回调函数

@parameter:
    Handle
        回调函数指针

@return:
    void

*/
static void sfdefBut_RegsiterCallBack(void (*Handle)(mButton_IntBackParam *))
{
	sfBut_ReportCallBack = Handle;
}

//---------- ---------- ---------- ----------
/*  static void sintf_GPIO_Grp1Function(void)
@introduction:
    按键中断 Group1 服务函数

@parameter:
    void

@return:
    void

*/
static void sintf_GPIO_Grp1Function(void)
{
	uint8 _i;
	mButIntGrp_Info *tGrpPin;
	mGPIOPinIfo *tPin;
	uint8 tStateNum;
	uint8 tTarDetect;

	tGrpPin = &sdefButGrp_[0];

	for (_i = 0; _i < tGrpPin->aUnit; _i++) {
		tPin = tGrpPin->aPinIfo[_i].aPin;
		tStateNum = tGrpPin->aPinIfo[_i].aNum;
		if (DF_JDG_IsIntExist(tPin)) {
			if (DF_GET_PinValue(tPin)) {
				// 防抖检测电平   1
				// 检测类型                 下降沿
				tTarDetect = 1;

				DF_SET_DetectIntFall(tPin);
			} else {
				// 防抖检测电平   0
				// 检测类型                 上升沿
				tTarDetect = 0;
				DF_SET_DetectIntRase(tPin);
			}

			DF_ButtonSTA_CountClr(sdefButton_Status[tStateNum]);
			if (!
			    (sdefButton_Status[tStateNum] &
			     DC_ButtonSTA_Update)) {

				sdefButton_Status[tStateNum] |=
				    DC_ButtonSTA_Update;
				if (tTarDetect) {

					sdefButton_Status[tStateNum] |=
					    DC_ButtonSTA_CurLevel;
					sdefButton_Status[tStateNum] |=
					    DC_ButtonSTA_TarDetect;
				} else {

					sdefButton_Status[tStateNum] &=
					    ~DC_ButtonSTA_CurLevel;
					sdefButton_Status[tStateNum] &=
					    ~DC_ButtonSTA_TarDetect;
				}
				if (tJitters == NULL) {

					tJitters = sfBut_JittersElimination;

					gClassHAL.PeriodEvent->afRegister
					(DC_HAL_PE_INT, tJitters,
					 DC_Button_JitterDelayBase);
				}
			}
			DF_SET_ClrIntState(tPin);
		}
	}

}

//---------- ---------- ---------- ----------
/*  static void sintf_GPIO_Grp2Function(void)
@introduction:
    按键中断 Group2 服务函数

@parameter:
    void

@return:
    void

*/
static void sintf_GPIO_Grp2Function(void)
{

	uint8 _i;
	mButIntGrp_Info *tGrpPin;
	mGPIOPinIfo *tPin;
	uint8 tStateNum;
	uint8 tTarDetect;

	tGrpPin = &sdefButGrp_[1];

	for (_i = 0; _i < tGrpPin->aUnit; _i++) {

		tPin = tGrpPin->aPinIfo[_i].aPin;
		tStateNum = tGrpPin->aPinIfo[_i].aNum;
		if (DF_JDG_IsIntExist(tPin)) {

			if (DF_GET_PinValue(tPin)) {

				// 防抖检测电平   1
				// 检测类型                 下降沿
				tTarDetect = 1;
				DF_SET_DetectIntFall(tPin);
			} else {

				// 防抖检测电平   0
				// 检测类型                 上升沿
				tTarDetect = 0;
				DF_SET_DetectIntRase(tPin);
			}

			DF_ButtonSTA_CountClr(sdefButton_Status[tStateNum]);
			if (!
			    (sdefButton_Status[tStateNum] &
			     DC_ButtonSTA_Update)) {

				sdefButton_Status[tStateNum] |=
				    DC_ButtonSTA_Update;
				if (tTarDetect) {

					sdefButton_Status[tStateNum] |=
					    DC_ButtonSTA_CurLevel;
					sdefButton_Status[tStateNum] |=
					    DC_ButtonSTA_TarDetect;
				} else {

					sdefButton_Status[tStateNum] &=
					    ~DC_ButtonSTA_CurLevel;
					sdefButton_Status[tStateNum] &=
					    ~DC_ButtonSTA_TarDetect;
				}
				if (tJitters == NULL) {

					tJitters = sfBut_JittersElimination;

					gClassHAL.PeriodEvent->afRegister
					(DC_HAL_PE_INT, tJitters,
					 DC_Button_JitterDelayBase);
				}
			}
			DF_SET_ClrIntState(tPin);
		}
	}

}

//---------- ---------- ---------- ----------
/*  static void sintf_GPIO_Grp3Function(void)
@introduction:
    按键中断 Group3 服务函数

@parameter:
    void

@return:
    void

*/
static void sintf_GPIO_Grp3Function(void)
{

	uint8 _i;
	mButIntGrp_Info *tGrpPin;
	mGPIOPinIfo *tPin;
	uint8 tStateNum;
	uint8 tTarDetect;

	tGrpPin = &sdefButGrp_[2];

	for (_i = 0; _i < tGrpPin->aUnit; _i++) {

		tPin = tGrpPin->aPinIfo[_i].aPin;
		tStateNum = tGrpPin->aPinIfo[_i].aNum;
		if (DF_JDG_IsIntExist(tPin)) {

			if (DF_GET_PinValue(tPin)) {

				// 防抖检测电平   1
				// 检测类型                 下降沿
				tTarDetect = 1;
				DF_SET_DetectIntFall(tPin);
			} else {

				// 防抖检测电平   0
				// 检测类型                 上升沿
				tTarDetect = 0;
				DF_SET_DetectIntRase(tPin);
			}

			DF_ButtonSTA_CountClr(sdefButton_Status[tStateNum]);

			if (!
			    (sdefButton_Status[tStateNum] &
			     DC_ButtonSTA_Update)) {

				sdefButton_Status[tStateNum] |=
				    DC_ButtonSTA_Update;
				if (tTarDetect) {

					sdefButton_Status[tStateNum] |=
					    DC_ButtonSTA_CurLevel;
					sdefButton_Status[tStateNum] |=
					    DC_ButtonSTA_TarDetect;
				} else {

					sdefButton_Status[tStateNum] &=
					    ~DC_ButtonSTA_CurLevel;
					sdefButton_Status[tStateNum] &=
					    ~DC_ButtonSTA_TarDetect;
				}
				if (tJitters == NULL) {

					tJitters = sfBut_JittersElimination;

					gClassHAL.PeriodEvent->afRegister
					(DC_HAL_PE_INT, tJitters,
					 DC_Button_JitterDelayBase);
				}
			}
			DF_SET_ClrIntState(tPin);
		}
	}

}

//---------- ---------- ---------- ----------
/*  static void sfBut_JittersElimination(void)
@introduction:
    根据用户配置的按键队列，检索是否有新的按键状态变化，这个函数会根据按键的变化
    如实的记录到每个按键对应的状态存储变量当中

@parameter:
    void

@return:
    void

*/
static void sfBut_JittersElimination(void)
{
	uint8 _i, _j;
	uint8 tChk;
	uint8 tbutNum = gClassButton.apPinArr->aNum;
	mGPIOPinIfo *tbutPin = gClassButton.apPinArr->apPin;

	for (_i = 0; _i < tbutNum; _i++) {
		if (sdefButton_Status[_i] & DC_ButtonSTA_Update) {
			//
			if (DF_ButtonSTA_CountJudge
			    (sdefButton_Status[_i], DC_Button_JitterDelay)) {
				sdefButton_Status[_i] &= ~DC_ButtonSTA_Update;
				if (sdefButton_Status[_i] &
				    DC_ButtonSTA_TarDetect) {
					tChk = 1;
				} else {
					tChk = 0;
				}
				if (tChk == DF_GET_PinValue(&tbutPin[_i])) {
					// report
					sdefButton_Status[_i] |=
					    DC_ButtonSTA_Report;
					if (tChk) {
						sdefButton_Status[_i] |=
						    DC_ButtonSTA_RpotSta;
					} else {
						sdefButton_Status[_i] &=
						    ~DC_ButtonSTA_RpotSta;
					}
					gClassHAL.PeriodEvent->afRegister
					(DC_HAL_PE_UNINT, sfBut_Reporter,
					 DC_Button_ReportDelay);
				}
				// 检查是不是还要执行任务
				// 如果没有任务要执行就启动自爆程序
				// (⊙?⊙)
				_j = 0;
				while ((!
				        (DC_ButtonSTA_Update &
				         sdefButton_Status[_j]))
				       && (_j < tbutNum)) {
					_j++;
				}
				if (_j == tbutNum) {
					gClassHAL.PeriodEvent->
					afCancel(DC_HAL_PE_INT, tJitters);
					tJitters = NULL;
				}

			} else {
				DF_ButtonSTA_CountInc(sdefButton_Status[_i]);
			}
		}
	}
}

//---------- ---------- ---------- ----------
/*  static void sfBut_Reporter(void)
@introduction:
    按键上报，在上报一次之后，自动取消自己所注册的事件

@parameter:
    void

@return:
    void

*/
static void sfBut_Reporter(void)
{
	uint8 _i = 0;
	uint8 tButNum = gClassButton.apPinArr->aNum;
	mButton_IntBackParam tBackParam;
#if DEBUG_BUTTON
	NVCPrint("Report Button!");
#endif

	while (_i < tButNum) {

		if (sdefButton_Status[_i] & DC_ButtonSTA_Report) {

			sdefButton_Status[_i] &= ~DC_ButtonSTA_Report;
			tBackParam.aUnit = _i;
			if (sdefButton_Status[_i] & DC_ButtonSTA_RpotSta) {
				tBackParam.aStatus = DC_BuMoSta_RAISE;
#if DEBUG_BUTTON
				NVCPrint("Button: %d raise", (int)_i);
#endif
			} else {
				tBackParam.aStatus = DC_BuMoSta_FALL;
#if DEBUG_BUTTON
				NVCPrint("Button: %d Falling", (int)_i);
#endif
			}
			if (sfBut_ReportCallBack != NULL) {
				sfBut_ReportCallBack(&tBackParam);
			}
		}
		_i++;
	}

	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT, sfBut_Reporter);
}
