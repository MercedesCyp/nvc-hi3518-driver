/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认云台方向
    	水平方向逆时针转时，起点<0点>为开始运动的点，终点为运动结束的点
    	垂直方向向上转动时, 起点<0点>为开始运动的点，终点为运动结束的点
*/
//==============================================================================
// C
// Linux
// local
#include "CTA.h"
// remote
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"
#include "../Tool/String.h"
#include "../HAL/HAL.h"

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
// --------------------------------------------------> driver pluse times per seconds
#define DC_PTGlobal_HDefMinSteps	1
#define DC_PTGlobal_VDefMinSteps	1
// --------------------------------------------------> driver pluse times per seconds
#define DC_PTGlobal_MinPluse	50
#define DC_PTGlobal_MaxPluse	450
#define DC_PTGlobal_IncUnit		4
#define DC_PTGlobal_DefPluse	250
// --------------------------------------------------> mutation loitor(unit: ms)
#define DC_PTGlobal_Loitor		100
// --------------------------------------------------> Status
// 0x00000008
// 0x00000400
// 0x00000800
// 0x00001000
// 0x00002000
// 0x00004000
// 0x00080000
// 0x04000000
// 0x08000000
// 0x01000000
// 0x02000000
// 0x04000000
// 0x08000000
#define DC_PTS_BuffFull		0x00008000
// Horizontal
#define DC_PTS_HRhythmArea	0x00000003
#define DC_PTS_HRhythmUnit	0x00000001
#define DC_PTS_HPeriodDone	0x00000004
#define DC_PTS_HIniting		0x00000010
#define DC_PTS_HRunning		0x00000020
#define DC_PTS_HAntiRoll	0x00000040
#define DC_PTS_HReducing	0x00000080
#define DC_PTS_HBanSum		0x00000100
#define DC_PTS_HRspEnding	0x00000200
// Vertical
#define DC_PTS_VRhythmArea	0x00030000
#define DC_PTS_VRhythmUnit	0x00010000
#define DC_PTS_VPeriodDone	0x00040000
#define DC_PTS_VIniting		0x00100000
#define DC_PTS_VRunning		0x00200000
#define DC_PTS_VAntiRoll	0x00400000
#define DC_PTS_VReducing	0x00800000
#define DC_PTS_VBanSum		0x01000000
#define DC_PTS_VRspEnding	0x02000000
//
#define DF_PTS_GetHRhythm			(sPTStatus&DC_PTS_HRhythmArea)
#define DF_PTS_GetVRhythm			((sPTStatus&DC_PTS_VRhythmArea)>>16)
#define DF_PTS_HaddRhythm			{sPTStatus+=DC_PTS_HRhythmUnit;}
#define DF_PTS_VaddRhythm			{sPTStatus+=DC_PTS_VRhythmUnit;}
//
#define DF_PTS_IsStatusExist(_x)	DF_STATUS_IsStatusExist(sPTStatus,(_x))
#define DF_PTS_SetStatus(_x)		DF_STATUS_SetStatus(sPTStatus,(_x))
#define DF_PTS_ClrStatus(_x)		DF_STATUS_ClrStatus(sPTStatus,(_x))
#define DF_PTS_InitStatus			{sPTStatus = 0;}
//
#define DF_PTS_GetHOriStatus		(gClassHAL.Pin->prfGetPinValue(&spPTLimPin[0]))
#define DF_PTS_GetHTilStatus		(gClassHAL.Pin->prfGetPinValue(&spPTLimPin[1]))
#define DF_PTS_GetVOriStatus		(gClassHAL.Pin->prfGetPinValue(&spPTLimPin[2]))
#define DF_PTS_GetVTilStatus		(gClassHAL.Pin->prfGetPinValue(&spPTLimPin[3]))

//=============================================================================
// DATA TYPE
typedef struct {
	uint16 aXRod;
	uint16 aYRod;
} mCood_xy;

typedef struct {
	mCood_xy aRange;	// 可扫描范围
	mCood_xy aCur;		// 当前位置
	mCood_xy aPool;		// 缓冲池
} mPT_Position;

#define DC_PTPRESET_POINT_NUM 16

typedef struct {
#define DC_PTS_PreSet_UNSet         0x00
#define DC_PTS_PreSet_ISSet         0x01
	union {
		uint8 aWork;
		uint8 aNum;
	};
	uint8 aSpeed;
	uint16 aStayTime;
} mPTZPreSetPoint;
//
typedef struct {
#define DC_PTS_CRUISE_READY         0x01
	uint8 aStatus;
	uint8 aMvPath[DC_PTPRESET_POINT_NUM];
} mPTZMvPath;
//预置位存储点
typedef struct {
	mPTZPreSetPoint aPoint[DC_PTPRESET_POINT_NUM];
	mCood_xy aPos[DC_PTPRESET_POINT_NUM];
} mPTZPSCof_f;

//==============================================================================
//extern
//local
static int32 sfdefPanTilt_Init(void);
static int32 sfdefPanTilt_Uninit(void);
static int32 sfdefPTZ_SetPresetPoint(void *iPresetArr);
static int32 sfdefPTZ_ClrPresetPoint(void *iPresetArr);
static int32 sfdefPTZ_InportPresetConfigInfo(void *iInfo);
static int32 sfdefPTZ_SetCruisePath(void *iPath);
static int32 sfdefPTZ_StartCruise(void);
static void sfdefPTZ_CruiseAutoRunning(void);
static void sfdefPTZ_UpdateInfo(void);
static void sfdefPTZ_RegInitCallBack(void (*iHandle)(void));
static void sfdefPTZ_ReportInitialDone(void);
static int32 sfdefPTZ_Start(mPTZAction iAct);
#define DC_PTF_FUAP_HClock  0x01
#define DC_PTF_FUAP_HAnti   0x02
#define DC_PTF_FUAP_VClock  0x04
#define DC_PTF_FUAP_VAnti   0x08
#define DC_PTF_FUAP_HLim	0x10
#define DC_PTF_FUAP_VLim	0x20
// 调用此函数之前应该判断水平和垂直方向的权重是否为0
static int32 sfdefPTZ_FillUpTheActPowder(uint8 iCmd, uint16 iHComp,
        uint16 iVComp);
static void sfdefPTZ_MoveUnitConversion(mPTZAction *iAct);
static int32 sfdefPTZ_Stop(void);
static void sfdefPTZ_PanTiltRuning(void);
static void sfdefPTZ_FindZeroPoint(void);
static void sfdefPTZ_FigureOutTheRange(void);
#define DC_PTF_SBH_Set			0x80
#define DC_PTF_SBH_Start		0x40
#define DC_PTF_SBH_Stop			0x20
static int32 sfdefPTZ_SetBeatHeart(uint8 iCmd,
                                   uint16 iPluse, void (*iHandler)(void));

static void sfOutputAPhasesToVSM(uint8 _cmd);
static void sfOutputAPhasesToHSM(uint8 _cmd);
//global

//==============================================================================
//extern
//local
static mGPIOPinArr sdefDrvPinArr_PTZ = {
	NULL, 0,
};

static mGPIOPinArr sdefLimPinArr_PTZ = {
	NULL, 0
};

static mPanTiltInfo sPTInfo;

static uint8 sRightPhases[5] = { 0x0C, 0x09, 0x03, 0x06, 0x00 };	// 8 4 2 1    D C B A
static uint8 sAntiPhases[5] = { 0x03, 0x06, 0x0C, 0x09, 0x0F };	// 8 4 2 1    D C B A

static uint8 *sStepMotorPhases = sRightPhases;
static mPTZMvPath *sPTMvPath = NULL;
static mPTZPSCof_f *sPTPSCof_mem = NULL;

static mGPIOPinIfo *spPTDrvPin = NULL;
static mGPIOPinIfo *spPTLimPin = NULL;
static mPT_Position sPTPosition;
static uint32 sPTStatus;
static uint32 sPTLoitor;
// ------------------------------>Reduction ratio
// when the displacement and the Axes director vector is discrepancy,
// it could cause the speed is different in two component,
// this val will be used to record reducing ratio at one side.
static uint32 sRaducingRatio;
static uint32 sRaducingCount;
static mPTZAction sPTZActBuf;
static void (*sdefPTZ_InitCallBack)(void) = NULL;
//global
mClass_PTZ gClassPTZ = {
	.apDrvPinArr = &sdefDrvPinArr_PTZ,
	.apLimPinArr = &sdefLimPinArr_PTZ,
	.apDefInfo = &sPTInfo,
	.afInit = sfdefPanTilt_Init,
	.afUninit = sfdefPanTilt_Uninit,
	.afUpdatePTZInfo = sfdefPTZ_UpdateInfo,
	.gfPTZStart = sfdefPTZ_Start,
	.gfPTZStop = sfdefPTZ_Stop,
	.gfSetPreSet = sfdefPTZ_SetPresetPoint,
	.gfClrPreSet = sfdefPTZ_ClrPresetPoint,
	.gfInPSList = sfdefPTZ_InportPresetConfigInfo,
	.gfInPSlcusList = sfdefPTZ_SetCruisePath,
	.gfStartCuris = sfdefPTZ_StartCruise,
	.afRegInitCallback = sfdefPTZ_RegInitCallBack,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfdefPanTilt_Init(void)
@introduction:
    云台初始化程序

@parameter:
    void

@return:
    0   初始化成功
    -1  初始化失败
*/
static int32 sfdefPanTilt_Init(void)
{
	uint8 _i;

	gClassStr.afMemset((uint8 *) & sPTPosition, 0x00, sizeof(mPT_Position));
	sPTStatus = 0;
	sPTLoitor = 0;
	sRaducingRatio = 0;
	sRaducingCount = 0;

	// initial the driver pin and the limit pin of PTZ.
	spPTDrvPin = gClassPTZ.apDrvPinArr->apPin;
	for (_i = 0; _i < gClassPTZ.apDrvPinArr->aNum; _i++) {

		gClassHAL.Pin->prfSetDirection(&spPTDrvPin[_i],
		                               DC_HAL_PIN_SetOUT);
#if DEBUG_PTZ
		NVCPrint("PIN  Drv_OUT-> G:%d\tB:%d\tMuxVal:%d\tMux:%d\t",
		         (int)spPTDrvPin[_i].aGroup,
		         (int)spPTDrvPin[_i].aBit,
		         (int)spPTDrvPin[_i].aMuxVal, (int)spPTDrvPin[_i].aMux);
#endif
	}

	sfOutputAPhasesToHSM(4);
	sfOutputAPhasesToVSM(4);

	if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Locate) {

		spPTLimPin = gClassPTZ.apLimPinArr->apPin;
		for (_i = 0; _i < gClassPTZ.apLimPinArr->aNum; _i++) {
			gClassHAL.Pin->prfSetDirection(&spPTLimPin[_i],
			                               DC_HAL_PIN_SetIN);
#if DEBUG_PTZ
			NVCPrint
			("PIN  Lim_IN-> G:%d\tB:%d\tMuxVal:%d\tMux:%d\t",
			 (int)spPTLimPin[_i].aGroup,
			 (int)spPTLimPin[_i].aBit,
			 (int)spPTLimPin[_i].aMuxVal,
			 (int)spPTLimPin[_i].aMux);
#endif
		}
	}

	if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Supp_CRUISE) {
		sPTMvPath =
		    (mPTZMvPath *) kmalloc(sizeof(mPTZMvPath), GFP_ATOMIC);
		sPTPSCof_mem =
		    (mPTZPSCof_f *) kmalloc(sizeof(mPTZPSCof_f), GFP_ATOMIC);
		gClassStr.afMemset((uint8 *) sPTMvPath, 0x00,
		                   sizeof(mPTZMvPath));
		gClassStr.afMemset((uint8 *) sPTPSCof_mem, 0x00,
		                   sizeof(mPTZPSCof_f));
	}

	if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Positive) {
#if DEBUG_INIT
		NVCPrint("Set Anti_Clock order!");
#endif
		sStepMotorPhases = sAntiPhases;
	} else if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Negative) {
#if DEBUG_INIT
		NVCPrint("Set Clock order!");
#endif
		sStepMotorPhases = sRightPhases;
	} else {
		return -1;
	}

	//
	if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Locate) {
		//
		if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Loitor) {
			sPTLoitor = DC_PTGlobal_Loitor;
		}
		// Set initialize status
		DF_PTS_SetStatus(DC_PTS_HIniting);
		DF_PTS_SetStatus(DC_PTS_VIniting);
		// Running
		DF_PTS_SetStatus(DC_PTS_HRunning);
		DF_PTS_SetStatus(DC_PTS_VRunning);
		// period done
		DF_PTS_SetStatus(DC_PTS_HPeriodDone);
		DF_PTS_SetStatus(DC_PTS_VPeriodDone);

		sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Set | DC_PTF_SBH_Start,
		                      DC_PTGlobal_DefPluse,
		                      sfdefPTZ_FindZeroPoint);

		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
		                                  sfdefPTZ_ReportInitialDone,
		                                  10);

	} else {
		sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Set, DC_PTGlobal_DefPluse,
		                      sfdefPTZ_PanTiltRuning);
	}
#if DEBUG_INIT
	NVCPrint("The PTZ Module Start! OK");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPanTilt_Uninit(void)
@introduction:
    释放云台资源

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefPanTilt_Uninit(void)
{
	sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Stop, 0, NULL);
	if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Supp_CRUISE) {
		kfree(sPTMvPath);
		kfree(sPTPSCof_mem);
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_UpdateInfo(void)
@introduction:
    更新云台信息缓冲区中的数据

@parameter:
    void

@return:
    void

*/
static void sfdefPTZ_UpdateInfo(void)
{

	mPanTiltInfo *tpPTZInfo = gClassPTZ.apDefInfo;

	if (DF_PTS_IsStatusExist(DC_PTS_HIniting)
	    || DF_PTS_IsStatusExist(DC_PTS_VIniting)) {
		tpPTZInfo->aStatus &= ~DC_PTSP_InitDone;
	} else {
		tpPTZInfo->aStatus |= DC_PTSP_InitDone;
	}

	if (DF_PTS_IsStatusExist(DC_PTS_HRunning)
	    || DF_PTS_IsStatusExist(DC_PTS_HRunning)) {
		tpPTZInfo->aStatus |= DC_PTSP_Busy;
	} else {
		tpPTZInfo->aStatus &= ~DC_PTSP_Busy;
	}

	if (DF_PTS_IsStatusExist(DC_PTS_BuffFull)) {
		tpPTZInfo->aStatus |= DC_PTSP_Full;
	} else {
		tpPTZInfo->aStatus &= ~DC_PTSP_Full;
	}

	if ((tpPTZInfo->aRegCf & DC_PTSP_Locate)
	    && (tpPTZInfo->aRegCf & DC_PTSP_InitDone)) {
		tpPTZInfo->aXRange = sPTPosition.aRange.aXRod;
		tpPTZInfo->aYRange = sPTPosition.aRange.aYRod;
	} else {
		tpPTZInfo->aXRange = 0;
		tpPTZInfo->aYRange = 0;
	}

	tpPTZInfo->aHCurPos = sPTPosition.aCur.aXRod;
	tpPTZInfo->aYCurPos = sPTPosition.aCur.aYRod;

}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_SetPresetPoint( void* iPresetArr )
@introduction:
    该函数运行于支持云台限位的机型上，用户端在客户端确定当前云台转停的位置为当前
    预置号的具体位置时，可以让 linux 应用层发送设置预置位的信息下来设置当前位置。
    而内核驱动会调用这个函数来记录。

@parameter:
    iPreset
        该指针指向该预置位的控制信息

@return:
    0   设置成功
    1   之前该位置已存在设置值，但设置成功
    -1  此机型不支持此功能
*/
static int32 sfdefPTZ_SetPresetPoint(void *iPresetArr)
{
	mPTZPreSetPoint *tNode = (mPTZPreSetPoint *) iPresetArr;
	uint8 tPresetNum = tNode->aNum - 1;
	uint8 tStatus = 0;

	if (sPTPSCof_mem == NULL) {
		return -1;
	}

	if (sPTPSCof_mem->aPoint[tPresetNum].aWork == DC_PTS_PreSet_ISSet) {
		tStatus |= 0x01;
	}

	sPTPSCof_mem->aPoint[tPresetNum].aWork = DC_PTS_PreSet_ISSet;
	sPTPSCof_mem->aPoint[tPresetNum].aSpeed = tNode->aSpeed;
	sPTPSCof_mem->aPoint[tPresetNum].aStayTime = tNode->aStayTime;

	sPTPSCof_mem->aPos[tPresetNum].aXRod = sPTPosition.aCur.aXRod;
	sPTPSCof_mem->aPos[tPresetNum].aYRod = sPTPosition.aCur.aYRod;

#if DEBUG_PTZ
	NVCPrint("- - - - - - - - - -");
	NVCPrint("- Set Pre");
	NVCPrint("PreNo     :%d", (int)tNode->aNum);
	NVCPrint("PreSpeed  :%d",
	         (int)sPTPSCof_mem->aPoint[tNode->aNum - 1].aSpeed);
	NVCPrint("PreStayT  :%d",
	         (int)sPTPSCof_mem->aPoint[tNode->aNum - 1].aStayTime);
	NVCPrint("xCor      :%d",
	         (int)sPTPSCof_mem->aPos[tNode->aNum - 1].aXRod);
	NVCPrint("yCor      :%d",
	         (int)sPTPSCof_mem->aPos[tNode->aNum - 1].aYRod);
	NVCPrint("- - - - - - - - - -");
#endif
	if (tStatus & 0x01) {
		return 1;
	} else {
		return 0;
	}
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_ClrPresetPoint( void* iPresetArr )
@introduction:
    清除当前预置位

@parameter:
    iPreset
        该指针指向该预置位的控制信息

@return:
    1   原本不存在该预制信息，清除成功
    0   清除成功
    -1  该机型不支持此功能

*/
static int32 sfdefPTZ_ClrPresetPoint(void *iPresetArr)
{
	mPTZPreSetPoint *tNode = (mPTZPreSetPoint *) iPresetArr;
	uint8 tPresetNum = tNode->aNum - 1;
	uint8 tStatus = 0;

	if (sPTPSCof_mem == NULL) {
		return -1;
	}
	if (sPTPSCof_mem->aPoint[tPresetNum].aWork == DC_PTS_PreSet_UNSet) {
		tStatus |= 0x01;
	}

	sPTPSCof_mem->aPoint[tPresetNum].aWork = DC_PTS_PreSet_UNSet;
#if DEBUG_PTZ
	NVCPrint("- - - - - - - - - -");
	NVCPrint("- Clr Pre");
	NVCPrint("PreNo     :%d", (int)tNode->aNum);
	NVCPrint("PreSpeed  :%d",
	         (int)sPTPSCof_mem->aPoint[tNode->aNum - 1].aSpeed);
	NVCPrint("PreStayT  :%d",
	         (int)sPTPSCof_mem->aPoint[tNode->aNum - 1].aStayTime);
	NVCPrint("xCor      :%d",
	         (int)sPTPSCof_mem->aPos[tNode->aNum - 1].aXRod);
	NVCPrint("yCor      :%d",
	         (int)sPTPSCof_mem->aPos[tNode->aNum - 1].aYRod);
	NVCPrint("- - - - - - - - - -");
#endif
	if (tStatus & 0x01) {
		return 1;
	} else {
		return 0;
	}
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_InportPresetConfigInfo( void *iInfo )
@introduction:
    这个函数一般只会用一次，也就是应用层初始化的时候，把用户原先设置好的预置位重
    新导入驱动中

@parameter:
    iInfo
        指向停用层传进的云台预置位的配置信息

@return:
    0   导入预置位信息成功
    -1  此机型不支持此功能

*/
static int32 sfdefPTZ_InportPresetConfigInfo(void *iInfo)
{
	if (sPTPSCof_mem != NULL) {
		uint8 tCount_i;
		gClassStr.afCopy((uint8 *) sPTPSCof_mem, (uint8 *) iInfo,
		                 sizeof(mPTZPSCof_f));
		for (tCount_i = 0; tCount_i < DC_PTPRESET_POINT_NUM; tCount_i++) {
			if ((sPTPSCof_mem->aPoint[tCount_i].aNum > 0)
			    || (sPTPSCof_mem->aPoint[tCount_i].aNum <=
			        DC_PTPRESET_POINT_NUM)) {
				sPTPSCof_mem->aPoint[tCount_i].aWork =
				    DC_PTS_PreSet_ISSet;
			} else {
				sPTPSCof_mem->aPoint[tCount_i].aWork =
				    DC_PTS_PreSet_UNSet;
			}
#if DEBUG_PTZ
			NVCPrint("- - - - - - - - - -");
			NVCPrint("PreNo     :%d", (int)tCount_i + 1);
			if (sPTPSCof_mem->aPoint[tCount_i].aWork ==
			    DC_PTS_PreSet_ISSet) {
				NVCPrint("PreSpeed  :%d",
				         (int)sPTPSCof_mem->aPoint[tCount_i].
				         aSpeed);
				NVCPrint("PreStayT  :%d",
				         (int)sPTPSCof_mem->aPoint[tCount_i].
				         aStayTime);
				NVCPrint("xCor      :%d",
				         (int)sPTPSCof_mem->aPos[tCount_i].
				         aXRod);
				NVCPrint("yCor      :%d",
				         (int)sPTPSCof_mem->aPos[tCount_i].
				         aYRod);
			} else {
				NVCPrint("Not Set");
			}
#endif
		}
	} else {
		return -1;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_SetCruisePath( void *iPath )
@introduction:
    这个函数用于设置用户需要巡航的路径，但在设置的节点中如果出现为被用户实现设置
    好的预置位点，那么，这个函数将会放弃此次操作

@parameter:
    iPath
        指向巡航路径设置数据的指针

@return:
    0   SUCCESS
    -1  设置值包含无效预置位
    -2  设置值为空（没有设置任何东西）
    -3  此机型不支持此操作
*/
static int32 sfdefPTZ_SetCruisePath(void *iPath)
{

	uint8 _j = 0;
	uint8 *tSch = iPath;

	if (sPTPSCof_mem == NULL) {
		return -3;
	}

	if (iPath == NULL) {
		return -2;
	}
	for (; _j < DC_PTPRESET_POINT_NUM; _j++) {

		if ((tSch[_j] == 0) || (tSch[_j] > DC_PTPRESET_POINT_NUM)) {
			sPTMvPath->aMvPath[_j] = 0;
			break;
		}

		if (sPTPSCof_mem->aPoint[tSch[_j] - 1].aWork ==
		    DC_PTS_PreSet_ISSet) {
			sPTMvPath->aMvPath[_j] = tSch[_j];
		} else {
#if DEBUG_PTZ
			NVCPrint("ERR:  Target Pre not set!");
#endif
			return -1;
		}
	}

#if DEBUG_PTZ
	{
		uint8 _i;
		NVCPrint("- - - - - - - - - -");
		for (_i = 0; _i < DC_PTPRESET_POINT_NUM; _i++) {
			if (sPTMvPath->aMvPath[_i] != 0) {
				NVCPrint("PreNo     :%d",
				         (int)sPTMvPath->aMvPath[_i]);
			} else {
				break;
			}
		}
		NVCPrint("- - - - - - - - - -");
	}
#endif

	if (_j != 0) {
		sPTMvPath->aStatus |= DC_PTS_CRUISE_READY;
	} else {
		return -2;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_StartCruise( void  )
@introduction:

@parameter:
    iPath
        指向巡航路径设置数据的指针

@return:
    0   SUCCESS
    -1  NOT READY
    -2  Set curise path, but path is empty
    -3  Path Error Set Path
    -4  Inclued unwork preset

*/
#define DC_PTT_CuriseCheckTime  10	// 1s == 10 * 100ms/1
static uint8 sPTZCuriseOrder = 0;
static uint32 sPTZCuriseTimer = 0;
static int32 sfdefPTZ_StartCruise(void)
{
#if DEBUG_PTZ
	NVCPrint("- - - - - - - - - -");
	NVCPrint("Start Curise!");
#endif
	if (sPTMvPath->aStatus & DC_PTS_CRUISE_READY) {
		uint8 tCount_i;
		uint8 tStatus = 0;
		for (tCount_i = 0; tCount_i < DC_PTPRESET_POINT_NUM; tCount_i++) {
			if ((sPTMvPath->aMvPath[tCount_i] == 0)) {
				tStatus |= 0x01;
				break;
			} else if (sPTMvPath->aMvPath[tCount_i] >
			           DC_PTPRESET_POINT_NUM) {
				tStatus |= 0x02;
				break;
			} else if (sPTPSCof_mem->aPoint
			           [sPTMvPath->aMvPath[tCount_i] - 1].aWork ==
			           DC_PTS_PreSet_UNSet) {
				tStatus |= 0x04;
				break;
			}
		}

		if ((tStatus & 0x01) && (tCount_i == 0)) {
#if DEBUG_PTZ
			NVCPrint("Err! Empty Parth!");
#endif
			return -2;
		} else if (tStatus & 0x02) {
#if DEBUG_PTZ
			NVCPrint("Err! Illegal Preset Num!");
#endif
			return -3;
		} else if (tStatus & 0x04) {
#if DEBUG_PTZ
			NVCPrint("Err! Illegal Preset!");
#endif
			return -4;
		}

		sPTZCuriseOrder = 0;
		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
		                                  sfdefPTZ_CruiseAutoRunning,
		                                  DC_PTT_CuriseCheckTime);

	} else {
		return -1;
	}
	return 0;
}

static void sfdefPTZ_CruiseAutoRunning(void)
{

	if ((sPTZCuriseOrder % 2) == 0) {

		if ((!DF_PTS_IsStatusExist(DC_PTS_BuffFull))
		    && (!DF_PTS_IsStatusExist(DC_PTS_HRunning))
		    && (!DF_PTS_IsStatusExist(DC_PTS_VRunning))) {

			mPTZAction tAct;

			gClassStr.afMemset((uint8 *) & tAct, 0x00,
			                   sizeof(mPTZAction));
			tAct.aCommand = DC_PTZCmd_Preset;
			tAct.aPresetNum =
			    sPTMvPath->aMvPath[sPTZCuriseOrder / 2];

			sfdefPTZ_Start(tAct);
			sPTZCuriseTimer = 0;
			sPTZCuriseOrder++;
#if DEBUG_PTZ
			NVCPrint("A C PreSet is: %d", (int)tAct.aPresetNum);
#endif
		}
	} else {
		uint16 tPresetNum = sPTMvPath->aMvPath[sPTZCuriseOrder / 2];
		if (sPTZCuriseTimer ==
		    sPTPSCof_mem->aPoint[tPresetNum].aStayTime) {

			sPTZCuriseOrder++;
			if (sPTMvPath->aMvPath[sPTZCuriseOrder / 2] == 0) {
				gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
				                                sfdefPTZ_CruiseAutoRunning);
				return;
			}
		} else {
			sPTZCuriseTimer++;
#if DEBUG_PTZ
			NVCPrint("A C StayTime: %d", (int)sPTZCuriseTimer);
#endif
		}
	}
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_RegInitCallBack( void (*iHandle)(void) )
@introduction:
    注册上报回调函数

@parameter:
    iHandle
        回调函数

@return:
    void

*/
static void sfdefPTZ_RegInitCallBack(void (*iHandle)(void))
{
	sdefPTZ_InitCallBack = iHandle;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_ReportInitialDone(void)
@introduction:
    云台运行完成后，将会将目前云台的状态信息上报，上报之后会注销此事件本身

@parameter:
    void

@return:
    void

*/
static void sfdefPTZ_ReportInitialDone(void)
{
	if (!(DF_PTS_IsStatusExist(DC_PTS_VIniting)
	      || DF_PTS_IsStatusExist(DC_PTS_VIniting))) {
		if (sdefPTZ_InitCallBack) {
			sdefPTZ_InitCallBack();
		}
		gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
		                                sfdefPTZ_ReportInitialDone);
	}
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_Start(mPTZAction iAct)
@introduction:
    这块区域里面主要关于启动操作云台的函数
    检查弹道 缓存弹夹
    弹头转换
    调准射击方向
    填充弹药
    设立检测数据 调整发射距离
    发射
    主要是接收到应用层的指令后，对云台进行配置，然后注册运行

@parameter:
    iAct
        用户配置数据

@return:
    详见 CTA.h 文件


*/
static int32 sfdefPTZ_Start(mPTZAction iAct)
{
	int32 tRet = 0;
	uint32 tPTZIfoStatus = gClassPTZ.apDefInfo->aRegCf;

	if (DF_PTS_IsStatusExist(DC_PTS_BuffFull)) {
		tRet |= DC_PTZRet_Busy | DC_PTZRet_BufFull;
#if DEBUG_PTZ
		NVCPrint("Start: Buff Full! Jumb out!");
#endif
		return tRet;
	} else {
		if ((DF_PTS_IsStatusExist(DC_PTS_HRunning))
		    || (DF_PTS_IsStatusExist(DC_PTS_VRunning))) {
			// 缓存区目前为空，可将预置位值设置进缓存区
#if DEBUG_PTZ
			NVCPrint("Start: Storage in buf!");
#endif
			sPTZActBuf = iAct;
			tRet |= DC_PTZRet_BufFull;

			return tRet;
		} else {
			tRet |= DC_PTZRet_BufIdle;
		}
	}
#if DEBUG_PTZ
	NVCPrint("Start: Start Conv!");
#endif
	sfdefPTZ_MoveUnitConversion(&iAct);
	if (iAct.aCommand & DC_PTZCmd_Err) {
#if DEBUG_PTZ
		NVCPrint("Start: Conv fail!");
#endif
		return DC_PTZRet_InputErr;
	}
#if DEBUG_PTZ
	NVCPrint("Start: Finish Conv!");
	NVCPrint("Start: Start Feed Powder!");
#endif
	{
		uint8 tCmd = 0;
		if (tPTZIfoStatus & DC_PTSP_Supp_HLim) {
			tCmd |= DC_PTF_FUAP_HLim;
		}
		if (tPTZIfoStatus & DC_PTSP_Supp_VLim) {
			tCmd |= DC_PTF_FUAP_VLim;
		}

		if (iAct.aHComponent != 0) {
			if (iAct.aCommand & DC_PTZCmd_Right) {
				if (tPTZIfoStatus & DC_PTSP_RightIsA) {
					tCmd |= DC_PTF_FUAP_HAnti;
				} else {
					tCmd |= DC_PTF_FUAP_HClock;
				}
			} else {
				if (tPTZIfoStatus & DC_PTSP_LeftIsA) {
					tCmd |= DC_PTF_FUAP_HAnti;
				} else {
					tCmd |= DC_PTF_FUAP_HClock;
				}
			}
		}
		if (iAct.aVComponent != 0) {
			if (iAct.aCommand & DC_PTZCmd_Up) {
				if (tPTZIfoStatus & DC_PTSP_UpIsA) {
					tCmd |= DC_PTF_FUAP_VAnti;
				} else {
					tCmd |= DC_PTF_FUAP_VClock;
				}
			} else {
				if (tPTZIfoStatus & DC_PTSP_DownIsA) {
					tCmd |= DC_PTF_FUAP_VAnti;
				} else {
					tCmd |= DC_PTF_FUAP_VClock;
				}
			}
		}
#if DEBUG_PTZ
		NVCPrint("- - - - - - - - - - - - - - - - - -");
		NVCPrint("PTZ  SETSTART H: %d\t V: %d\t",
		         (int)iAct.aHComponent, (int)iAct.aVComponent);
		NVCPrint("PTZ  SETSTART_POOL H: %d\t V: %d\t",
		         (int)sPTPosition.aPool.aXRod,
		         (int)sPTPosition.aPool.aYRod);

#endif
		sfdefPTZ_FillUpTheActPowder(tCmd, iAct.aHComponent,
		                            iAct.aVComponent);
	}
#if DEBUG_PTZ
	NVCPrint("Start: Stop Feed Powder!");
#endif

	if (sPTPosition.aPool.aXRod) {
		DF_PTS_SetStatus(DC_PTS_HRunning);
		DF_PTS_SetStatus(DC_PTS_HPeriodDone);
	}

	if (sPTPosition.aPool.aYRod) {
		DF_PTS_SetStatus(DC_PTS_VRunning);
		DF_PTS_SetStatus(DC_PTS_VPeriodDone);
	}
#if DEBUG_PTZ
	NVCPrint("Start: Set V&H raducing rate!");
#endif
	if (DF_PTS_IsStatusExist(DC_PTS_HRunning)
	    && DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
		sRaducingCount = 0;
		DF_PTS_ClrStatus(DC_PTS_HReducing);
		DF_PTS_ClrStatus(DC_PTS_VReducing);
		if (iAct.aHComponent > iAct.aVComponent) {
			sRaducingRatio =
			    (uint32)(iAct.aHComponent << 7) / iAct.aVComponent;
			DF_PTS_SetStatus(DC_PTS_VReducing);
		} else {
			sRaducingRatio =
			    (uint32)(iAct.aVComponent << 7) / iAct.aHComponent;
			DF_PTS_SetStatus(DC_PTS_HReducing);
		}
	}

	if (iAct.aCommand & DC_PTZCmd_NeedLoitor) {
		sPTLoitor = DC_PTGlobal_Loitor;
	}
#if DEBUG_PTZ
	NVCPrint("Start: Set Speed!");
#endif
	if (iAct.aSpeed > 0 && iAct.aSpeed <= 100) {
		iAct.aSpeed =
		    iAct.aSpeed * DC_PTGlobal_IncUnit + DC_PTGlobal_MinPluse;
	} else {
		iAct.aSpeed = DC_PTGlobal_DefPluse;
	}

#if DEBUG_PTZ
	NVCPrint("Start: Start Move!");
#endif
	sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Set | DC_PTF_SBH_Start, iAct.aSpeed,
	                      sfdefPTZ_PanTiltRuning);

	return tRet;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_FillUpTheActPowder(uint8 iCmd, uint16 iHComp,uint16 iVComp)
@introduction:
    这个函数主要是根据不同云台机的装置不同来配置转动方向与实际方向的对应

@parameter:
    iCmd
        操作指令
    iHComp
        水平分量
    iVComp
        垂直分量

@return:
    水平垂直方向的状态

*/
static int32 sfdefPTZ_FillUpTheActPowder(uint8 iCmd, uint16 iHComp,
        uint16 iVComp)
{

	int32 tStatus = 0;

	if (iCmd & DC_PTF_FUAP_HClock) {
		DF_PTS_ClrStatus(DC_PTS_HAntiRoll);
		if (iCmd & DC_PTF_FUAP_HLim) {
			if ((iHComp + sPTPosition.aCur.aXRod) >
			    sPTPosition.aRange.aXRod) {
				iHComp =
				    sPTPosition.aRange.aXRod -
				    sPTPosition.aCur.aXRod;
				tStatus |= 0x01;
			}
		}
	} else if (iCmd & DC_PTF_FUAP_HAnti) {
		DF_PTS_SetStatus(DC_PTS_HAntiRoll);
		if (iCmd & DC_PTF_FUAP_HLim) {
			if (iHComp > sPTPosition.aCur.aXRod) {
				iHComp = sPTPosition.aCur.aXRod;
				tStatus |= 0x01;
			}
		}
	}

	if (iCmd & DC_PTF_FUAP_VClock) {
		DF_PTS_ClrStatus(DC_PTS_VAntiRoll);
		if (iCmd & DC_PTF_FUAP_VLim) {
			if ((iVComp + sPTPosition.aCur.aYRod) >
			    sPTPosition.aRange.aYRod) {
				iVComp =
				    sPTPosition.aRange.aYRod -
				    sPTPosition.aCur.aYRod;
				tStatus |= 0x02;
			}
		}
	} else if (iCmd & DC_PTF_FUAP_VAnti) {
		DF_PTS_SetStatus(DC_PTS_VAntiRoll);
		if (iCmd & DC_PTF_FUAP_VLim) {
			if (iVComp > sPTPosition.aCur.aYRod) {
				iVComp = sPTPosition.aCur.aYRod;
				tStatus |= 0x02;
			}
		}
	}

	sPTPosition.aPool.aXRod = iHComp;
	sPTPosition.aPool.aYRod = iVComp;
	return tStatus;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_MoveUnitConversion(mPTZAction *iAct)
@introduction:
    数据类型转换
    云台接受三种类型的操作，步数，角度，坐标
    角度和坐标最终在配置阶段都会调用此函数转化为步数

@parameter:
    iAct
        指向包含水平垂直分量对象的指针

@return:
    void

*/
static void sfdefPTZ_MoveUnitConversion(mPTZAction *iAct)
{
	uint32 tCmd = iAct->aCommand;
	uint32 tHComp = iAct->aHComponent;
	uint32 tVComp = iAct->aVComponent;

	if (tCmd & DC_PTZCmd_Preset) {
		uint16 tPreNum = iAct->aPresetNum - 1;
		if (sPTPSCof_mem->aPoint[tPreNum].aWork == DC_PTS_PreSet_UNSet) {
			tCmd |= DC_PTZCmd_Err;
			return;
		}
		tCmd &= ~DC_PTZCmd_Preset;
		tCmd |= DC_PTZCmd_Crd;
		tHComp = sPTPSCof_mem->aPos[tPreNum].aXRod;
		tVComp = sPTPSCof_mem->aPos[tPreNum].aYRod;
		iAct->aSpeed = sPTPSCof_mem->aPoint[tPreNum].aSpeed;
	}

	if (tCmd & DC_PTZCmd_Dgr) {
		tHComp = (tHComp * gClassPTZ.apDefInfo->aHStepsPerFull) / 360;
		tVComp = (tVComp * gClassPTZ.apDefInfo->aVStepsPerFull) / 360;
	} else if (tCmd & DC_PTZCmd_Crd) {
		if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Locate) {
			if (tHComp > sPTPosition.aRange.aXRod) {
				tHComp = sPTPosition.aRange.aXRod;
			}
			if (tVComp > sPTPosition.aRange.aYRod) {
				tVComp = sPTPosition.aRange.aYRod;
			}

			tCmd &= ~DC_PTZCmd_DirMask;
			if (tHComp >= sPTPosition.aCur.aXRod) {
				tHComp = tHComp - sPTPosition.aCur.aXRod;
				tCmd |= DC_PTZCmd_Right;
			} else {	//if( tHComp<sPTPosition.aCur.aXRod ){
				tHComp = sPTPosition.aCur.aXRod - tHComp;
				tCmd |= DC_PTZCmd_Left;
			}

			if (tVComp >= sPTPosition.aCur.aYRod) {
				tVComp = tVComp - sPTPosition.aCur.aYRod;
				tCmd |= DC_PTZCmd_Up;
			} else {	//if( tVComp<sPTPosition.aCur.aYRod ){
				tVComp = sPTPosition.aCur.aYRod - tVComp;
				tCmd |= DC_PTZCmd_Down;
			}

		} else {
			tCmd |= DC_PTZCmd_Err;
		}
	} else {
		return;
	}
	tCmd |= DC_PTZCmd_Ok;

	iAct->aCommand = tCmd;
	iAct->aHComponent = tHComp;
	iAct->aVComponent = tVComp;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$&&&& //
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$&&&& //

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_Stop(void)
@introduction:
    停止云台转动

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefPTZ_Stop(void)
{
	sPTPosition.aPool.aXRod = 0;
	sPTPosition.aPool.aYRod = 0;
	DF_PTS_ClrStatus(DC_PTS_BuffFull);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_PanTiltRuning(void)
@introduction:
    云台接受控制时都会将此函数注册到时钟驱动上，但调用此函数前，需要将云台要运行
    终点以及云台在垂直方向和水平方向运动的步数计算出来，并放到缓冲池当中，缓冲池
    中的数据会随着云台的移动，一点一点减少

@parameter:
    void

@return:
    void

*/
static void sfdefPTZ_PanTiltRuning(void)
{

	uint8 theCurPhases;
	if (sPTLoitor != 0) {
		sPTLoitor--;
		return;
	}

	if (DF_PTS_IsStatusExist(DC_PTS_HRunning)) {
		if (DF_PTS_IsStatusExist(DC_PTS_HReducing)) {
			sRaducingCount += 1 << 7;
			if (sRaducingCount >= sRaducingRatio) {
				sRaducingCount =
				    sRaducingCount - sRaducingRatio;
			} else {
				goto GT_PanTiltRuning_HEnd;
			}
		}

		if (DF_PTS_IsStatusExist(DC_PTS_HPeriodDone)) {

			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);

			if (sPTPosition.aPool.aXRod > 0) {
				sPTPosition.aPool.aXRod--;
				if (DF_PTS_IsStatusExist(DC_PTS_HAntiRoll)) {
					sPTPosition.aCur.aXRod--;
				} else {
					sPTPosition.aCur.aXRod++;
				}
			} else {
				sfOutputAPhasesToHSM(4);

				DF_PTS_ClrStatus(DC_PTS_HIniting);
				DF_PTS_ClrStatus(DC_PTS_HRunning);
				if (!DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
					goto GT_PanTiltRuning_AllEnd;	//JumpOutTheLoop
				}
				goto GT_PanTiltRuning_HEnd;
			}

		}

		if (DF_PTS_IsStatusExist(DC_PTS_HAntiRoll)) {
			theCurPhases = 3 - DF_PTS_GetHRhythm;
			if ((gClassPTZ.apDefInfo->aRegCf & DC_PTSP_HHeadLim)
			    && (!DF_PTS_GetHOriStatus)) {
				sPTPosition.aPool.aXRod = 0;
				sPTPosition.aCur.aXRod = 0;
			}
		} else {
			theCurPhases = DF_PTS_GetHRhythm;
			if ((gClassPTZ.apDefInfo->aRegCf & DC_PTSP_HTailLim)
			    && (!DF_PTS_GetHTilStatus)) {
				sPTPosition.aPool.aXRod = 0;
				sPTPosition.aCur.aXRod =
				    sPTPosition.aRange.aXRod;
			}
		}
		sfOutputAPhasesToHSM(theCurPhases);
		DF_PTS_HaddRhythm;
	}

GT_PanTiltRuning_HEnd:
	if (DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
		if (DF_PTS_IsStatusExist(DC_PTS_VReducing)) {
			sRaducingCount += 1 << 7;
			if (sRaducingCount >= sRaducingRatio) {
				sRaducingCount =
				    sRaducingCount - sRaducingRatio;
			} else {
				goto GT_PanTiltRuning_VEnd;
			}
		}

		if (DF_PTS_IsStatusExist(DC_PTS_VPeriodDone)) {
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);

			if (sPTPosition.aPool.aYRod > 0) {
				sPTPosition.aPool.aYRod--;
				if (DF_PTS_IsStatusExist(DC_PTS_VAntiRoll)) {
					sPTPosition.aCur.aYRod--;
				} else {
					sPTPosition.aCur.aYRod++;
				}
			} else {
				sfOutputAPhasesToVSM(4);
				DF_PTS_ClrStatus(DC_PTS_VIniting);
				DF_PTS_ClrStatus(DC_PTS_VRunning);
				if (!DF_PTS_IsStatusExist(DC_PTS_HRunning)) {
					goto GT_PanTiltRuning_AllEnd;	//JumpOutTheLoop
				}
				goto GT_PanTiltRuning_VEnd;
			}
		}

		if (DF_PTS_IsStatusExist(DC_PTS_VAntiRoll)) {
			theCurPhases = 3 - DF_PTS_GetVRhythm;
			if ((gClassPTZ.apDefInfo->aRegCf & DC_PTSP_VHeadLim)
			    && (!DF_PTS_GetVOriStatus)) {
				sPTPosition.aPool.aYRod = 0;
				sPTPosition.aCur.aYRod = 0;
			}
		} else {
			theCurPhases = DF_PTS_GetVRhythm;
			if ((gClassPTZ.apDefInfo->aRegCf & DC_PTSP_VTailLim)
			    && (!DF_PTS_GetVTilStatus)) {
				sPTPosition.aPool.aYRod = 0;
				sPTPosition.aCur.aYRod =
				    sPTPosition.aRange.aYRod;
			}
		}
		sfOutputAPhasesToVSM(theCurPhases);
		DF_PTS_VaddRhythm;
	}

GT_PanTiltRuning_VEnd:
	return;
GT_PanTiltRuning_AllEnd:
	if (DF_PTS_IsStatusExist(DC_PTS_BuffFull)) {
		sfdefPTZ_Start(sPTZActBuf);
		DF_PTS_ClrStatus(DC_PTS_BuffFull);
	} else {
		sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Stop, 0, NULL);
	}
	return;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_FindZeroPoint(void)
@introduction:
    驱动云台返回原点位置，主要有两个作用，第一个计算出当前位置，为计算云台移动范
    围做准备

@parameter:
    void

@return:
    void

*/
static void sfdefPTZ_FindZeroPoint(void)
{
	uint8 theCurrentPhases;

	if (DF_PTS_IsStatusExist(DC_PTS_HRunning)) {

		if (DF_PTS_IsStatusExist(DC_PTS_HPeriodDone)) {
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			sPTPosition.aPool.aXRod++;

			if ((!DF_PTS_GetHOriStatus)
			    || (sPTPosition.aPool.aXRod >
			        gClassPTZ.apDefInfo->aHLimitSteps)) {
				DF_PTS_ClrStatus(DC_PTS_HRunning);
				if (!DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
					goto GT_FindZeroPoint_ALLEnd;
				}
				goto GT_FindZeroPoint_HEnd;
			}
		}
		theCurrentPhases = 3 - DF_PTS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
GT_FindZeroPoint_HEnd:
	if (DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
		if (DF_PTS_IsStatusExist(DC_PTS_VPeriodDone)) {
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			sPTPosition.aPool.aYRod++;
			if ((!DF_PTS_GetVOriStatus)
			    ||
			    ((sPTPosition.aPool.aYRod >
			      gClassPTZ.apDefInfo->aVLimitSteps))) {

				DF_PTS_ClrStatus(DC_PTS_VRunning);
				if (!DF_PTS_IsStatusExist(DC_PTS_HRunning)) {
					goto GT_FindZeroPoint_ALLEnd;
				}
				goto GT_FindZeroPoint_VEnd;
			}
		}
		theCurrentPhases = 3 - DF_PTS_GetVRhythm;
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_PTS_VaddRhythm;
	}
GT_FindZeroPoint_VEnd:
	return;

GT_FindZeroPoint_ALLEnd:
	if (gClassPTZ.apDefInfo->aRegCf & DC_PTSP_Loitor) {
		sPTLoitor = DC_PTGlobal_Loitor;
	}
#if DEBUG_PTZ
	NVCPrint("PTZ  OriPosition H:%d\t V:%d\t",
	         (int)sPTPosition.aPool.aXRod, (int)sPTPosition.aPool.aYRod);
#endif
	// Running
	DF_PTS_SetStatus(DC_PTS_HRunning);
	DF_PTS_SetStatus(DC_PTS_VRunning);
	// period done
	DF_PTS_SetStatus(DC_PTS_HPeriodDone);
	DF_PTS_SetStatus(DC_PTS_VPeriodDone);

	sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Set | DC_PTF_SBH_Start,
	                      DC_PTGlobal_DefPluse, sfdefPTZ_FigureOutTheRange);

	return;
}

//---------- ---------- ---------- ----------
/*  static void sfdefPTZ_FigureOutTheRange(void)
@introduction:
    这是针对有限位的云台机，完成原点的寻找之后，进行一次巡航，检查云台的垂直与水
    平方向的范围，扫描完云台的范围之后，将会计算出回到初始位置的坐标，并驱动云台
    返回至原有坐标位置

@parameter:
    void

@return:
    void

*/
static void sfdefPTZ_FigureOutTheRange(void)
{
	uint8 theCurrentPhases;

	if (DF_PTS_IsStatusExist(DC_PTS_HRunning)) {
		if (DF_PTS_IsStatusExist(DC_PTS_HPeriodDone)) {
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			sPTPosition.aRange.aXRod++;

			if ((!DF_PTS_GetHTilStatus)
			    || (sPTPosition.aRange.aXRod >
			        gClassPTZ.apDefInfo->aHLimitSteps)) {

				if (sPTPosition.aRange.aXRod >
				    gClassPTZ.apDefInfo->aHLimitSteps) {
					sPTPosition.aPool.aXRod =
					    gClassPTZ.apDefInfo->aHStepsPerFull;
					sPTPosition.aRange.aXRod =
					    gClassPTZ.apDefInfo->aHStepsPerFull;
					sPTPosition.aCur.aXRod =
					    gClassPTZ.apDefInfo->aHStepsPerFull;
				}

				DF_PTS_ClrStatus(DC_PTS_HRunning);
				sfOutputAPhasesToHSM(4);
				if (!DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
					goto GT_FigureOutTheRange_ALLEnd;
				}
				goto GT_FigureOutTheRange_HEnd;
			}
		}
		theCurrentPhases = DF_PTS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
GT_FigureOutTheRange_HEnd:

	if (DF_PTS_IsStatusExist(DC_PTS_VRunning)) {
		if (DF_PTS_IsStatusExist(DC_PTS_VPeriodDone)) {
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			sPTPosition.aRange.aYRod++;
			if ((!DF_PTS_GetVTilStatus)
			    || (sPTPosition.aRange.aYRod >
			        gClassPTZ.apDefInfo->aVLimitSteps)) {

				if (sPTPosition.aRange.aYRod >
				    gClassPTZ.apDefInfo->aVLimitSteps) {
					sPTPosition.aPool.aYRod =
					    gClassPTZ.apDefInfo->aVStepsPerFull;
					sPTPosition.aRange.aYRod =
					    gClassPTZ.apDefInfo->aVStepsPerFull;
					sPTPosition.aCur.aYRod =
					    gClassPTZ.apDefInfo->aVStepsPerFull;
				}

				DF_PTS_ClrStatus(DC_PTS_VRunning);
				sfOutputAPhasesToVSM(4);
				if (!DF_PTS_IsStatusExist(DC_PTS_HRunning)) {
					goto GT_FigureOutTheRange_ALLEnd;
				}
				goto GT_FigureOutTheRange_VEnd;
			}
		}
		theCurrentPhases = DF_PTS_GetVRhythm;
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_PTS_VaddRhythm;
	}
GT_FigureOutTheRange_VEnd:
	return;
GT_FigureOutTheRange_ALLEnd:

	if (sPTPosition.aRange.aXRod >= sPTPosition.aPool.aXRod) {
		sPTPosition.aPool.aXRod =
		    sPTPosition.aRange.aXRod - sPTPosition.aPool.aXRod;
	} else {
		sPTPosition.aPool.aXRod = 0;
	}
	if (sPTPosition.aRange.aYRod >= sPTPosition.aPool.aYRod) {
		sPTPosition.aPool.aYRod =
		    sPTPosition.aRange.aYRod - sPTPosition.aPool.aYRod;
	} else {
		sPTPosition.aPool.aYRod = 0;
	}
	sPTPosition.aCur.aXRod = sPTPosition.aRange.aXRod;
	sPTPosition.aCur.aYRod = sPTPosition.aRange.aYRod;

#if DEBUG_PTZ
	NVCPrint("PTZ  RANGE H:%d\t V:%d\t",
	         (int)sPTPosition.aRange.aXRod, (int)sPTPosition.aRange.aYRod);
	NVCPrint("PTZ  SPOOL H:%d\t V:%d\t",
	         (int)sPTPosition.aPool.aXRod, (int)sPTPosition.aPool.aYRod);
#endif

	{
		mPTZAction tPTZAction;

		tPTZAction.aCommand =
		    DC_PTZCmd_Steps | DC_PTZCmd_Down | DC_PTZCmd_Left |
		    DC_PTZCmd_NeedLoitor;

		tPTZAction.aHComponent = sPTPosition.aPool.aXRod;
		tPTZAction.aVComponent = sPTPosition.aPool.aYRod;
		tPTZAction.aSpeed = 50;

		sfdefPTZ_Start(tPTZAction);
	}
	return;
}

//---------- ---------- ---------- ----------
/*  static int32 sfdefPTZ_SetBeatHeart(\
                                uint8 iCmd,\
                                uint16 iPluse,\
                                void(*iHandler)(void))
@introduction:
    这个函数功能主要是将云台驱动函数注册到 Timer3 上

@parameter:
    iCmd
        是开启还是停止
    iPluse
        云台的步速，单位（HZ）
    iHandler
        云台驱动函数

@return:
    0   SUCCESS
    -1  FAIL
*/
static int32 sfdefPTZ_SetBeatHeart(uint8 iCmd,
                                   uint16 iPluse, void (*iHandler)(void))
{
	int32 tRet;
	if (iCmd & DC_PTF_SBH_Set) {
		gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(1));
		if ((iPluse != 0) && (iHandler != NULL)) {
			mTIMER_param tSet_Param = {
				1000000 / iPluse,
				iHandler,
				DC_HAL_TIMER_cmdPTMode
			};
			tRet =
			    gClassHAL.Timer->prfConfig(DC_HAL_TIMERChannel(1),
			                               &tSet_Param);

			if (tRet) {
				return -1;
			}
		}
	}
	if (iCmd & DC_PTF_SBH_Stop) {
		gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(1));
	} else if (iCmd & DC_PTF_SBH_Start) {
		gClassHAL.Timer->prfStart(DC_HAL_TIMERChannel(1));
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfOutputAPhasesToHSM(uint8 _cmd)
@introduction:
    水平方向相位相位驱动

@parameter:
    _cmd
        相序编码

@return:
    void

*/
static void sfOutputAPhasesToHSM(uint8 _cmd)
{
#if DEBUG_PTZ
	if (_cmd == 4)
		// {
		// int _i;
		// _i = sStepMotorPhases[4];
	{
		NVCPrint("The PTZ RunEnd Set:%x", sStepMotorPhases[4]);
	}
	// }
#endif
	// printk("HSM:%x\r\n",(unsigned int)_cmd);
	if (0x01 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[0], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[0], DC_HAL_PIN_SetLOW);
	}
	if (0x02 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[1], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[1], DC_HAL_PIN_SetLOW);
	}
	if (0x04 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[2], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[2], DC_HAL_PIN_SetLOW);
	}
	if (0x08 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[3], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[3], DC_HAL_PIN_SetLOW);
	}
}

//---------- ---------- ---------- ----------
/*  static void sfOutputAPhasesToVSM(uint8 _cmd)
@introduction:
    垂直方向相位相位驱动

@parameter:
    _cmd
        相序编码

@return:
    void

*/
static void sfOutputAPhasesToVSM(uint8 _cmd)
{
#if DEBUG_PTZ
	if (_cmd == 4)
		// {
		// int _i;
		// _i = sStepMotorPhases[4];
	{
		NVCPrint("The PTZ RunEnd Set:%x", sStepMotorPhases[4]);
	}
	// }
#endif
	// printk("VSM:%x\r\n",(unsigned int)_cmd);
	if (0x01 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[4], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[4], DC_HAL_PIN_SetLOW);
	}
	if (0x02 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[5], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[5], DC_HAL_PIN_SetLOW);
	}
	if (0x04 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[6], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[6], DC_HAL_PIN_SetLOW);
	}
	if (0x08 & sStepMotorPhases[_cmd]) {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[7], DC_HAL_PIN_SetHIGH);
	} else {
		gClassHAL.Pin->prfSetExport(&spPTDrvPin[7], DC_HAL_PIN_SetLOW);
	}
}
