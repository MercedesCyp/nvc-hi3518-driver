// ================================================================================
//| 默认：
//| 	默认云台方向
//|			水平方向逆时针转时，起点<0点>为开始运动的点，终点为运动结束的点
//| 		垂直方向向上转动时, 起点<0点>为开始运动的点，终点为运动结束的点
//| 	
//| 	
//| 
//| 适用：
//| 	
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "CTA.h"
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"
#include "../Tool/String.h"
#include "../HAL/HAL.h"










typedef struct{
	uint16	aXRod;
	uint16	aYRod;
}mCood_xy;

typedef struct{
	mCood_xy 	aRange;		// 可扫描范围
	mCood_xy 	aCur;		// 当前位置
	mCood_xy 	aPool;		// 缓冲池
}mPT_Position;

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










static int32 sfdefPanTilt_Init(void);
static int32 sfdefPanTilt_Uninit(void);
static void sfdefPTZ_UpdateInfo(void);
static void sfdefPTZ_RegInitCallBack( void (*iHandle)(void) );
static void sfdefPTZ_ReportInitialDone(void);
static int32 sfdefPTZ_Start(mPTZAction iAct);
#define DC_PTF_FUAP_HClock  0x01
#define DC_PTF_FUAP_HAnti   0x02
#define DC_PTF_FUAP_VClock  0x04
#define DC_PTF_FUAP_VAnti   0x08
#define DC_PTF_FUAP_HLim	0x10
#define DC_PTF_FUAP_VLim	0x20
// 调用此函数之前应该判断水平和垂直方向的权重是否为0
static int32 sfdefPTZ_FillUpTheActPowder(uint8 iCmd, uint16 iHComp,uint16 iVComp);
static void sfdefPTZ_MoveUnitConversion(mPTZAction *iAct);
static int32 sfdefPTZ_Stop(void);
static void sfdefPTZ_PanTiltRuning(void);
static void sfdefPTZ_FindZeroPoint(void);
static void sfdefPTZ_FigureOutTheRange(void);
#define DC_PTF_SBH_Set			0x80
#define DC_PTF_SBH_Start		0x40
#define DC_PTF_SBH_Stop			0x20
static int32 sfdefPTZ_SetBeatHeart(\
							uint8 iCmd,\
							uint16 iPluse,\
							void(*iHandler)(void));

static void sfOutputAPhasesToVSM(uint8 _cmd);
static void sfOutputAPhasesToHSM(uint8 _cmd);












static mGPIOPinArr sdefDrvPinArr_PTZ = {
	NULL,0,
};
static mGPIOPinArr sdefLimPinArr_PTZ = {
	NULL,0
};
static mPanTiltInfo sPTInfo;

static uint8 sRightPhases[5]		={0x0C,0x09,0x03,0x06,0x00};
static uint8 sAntiPhases[5]			={0x03,0x06,0x0C,0x09,0x0F};
static uint8 *sStepMotorPhases 		= sRightPhases;

static mGPIOPinIfo	*spPTDrvPin = NULL;
static mGPIOPinIfo	*spPTLimPin = NULL;
static mPT_Position	sPTPosition;
static uint32 		sPTStatus;
static uint32 		sPTLoitor;
// ------------------------------>Reduction ratio
// when the displacement and the Axes director vector is discrepancy,
// it could cause the speed is different in two component,
// this val will be used to record reducing ratio at one side. 
static uint32 		sRaducingRatio;
static uint32 		sRaducingCount;
static mPTZAction 	sPTZActBuf;
static void (*sdefPTZ_InitCallBack)(void) = NULL;


mClass_PTZ gClassPTZ = {
	.apDrvPinArr       = &sdefDrvPinArr_PTZ,
	.apLimPinArr       = &sdefLimPinArr_PTZ,
	.apDefInfo         = &sPTInfo,
	.afInit            = sfdefPanTilt_Init,
	.afUninit          = sfdefPanTilt_Uninit,
	.afUpdatePTZInfo   = sfdefPTZ_UpdateInfo,
	.gfPTZStart        = sfdefPTZ_Start,
	.gfPTZStop         = sfdefPTZ_Stop,
	.afRegInitCallback = sfdefPTZ_RegInitCallBack,
};










// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------->sfdefPanTilt_Init
static int32 sfdefPanTilt_Init(void){
	uint8			_i;
	
	gClassStr.afMemset( (uint8*)&sPTPosition, 0x00, sizeof(mPT_Position) );
	sPTStatus 		= 0;
	sPTLoitor 		= 0;
	sRaducingRatio 	= 0;
	sRaducingCount 	= 0;
	
	
	// initial the driver pin and the limit pin of PTZ.
	spPTDrvPin = gClassPTZ.apDrvPinArr->apPin;
	for( _i = 0 ; _i< gClassPTZ.apDrvPinArr->aNum ; _i++ ){
        
        gClassHAL.Pin->prfSetDirection( &spPTDrvPin[_i], DC_HAL_PIN_SetOUT );
        /* 
		DC_PIN_SET_IO_OUT(\
					spPTDrvPin[_i].aGroup,\
					spPTDrvPin[_i].aBit,\
					spPTDrvPin[_i].aMuxVal,\
					spPTDrvPin[_i].aMux);
                     */
#if DEBUG_PTZ
		NVCPrint("PIN  Drv_OUT-> G:%d\tB:%d\tMuxVal:%d\tMux:%d\t",\
		( int)spPTDrvPin[_i].aGroup,\
		( int)spPTDrvPin[_i].aBit,\
		( int)spPTDrvPin[_i].aMuxVal,\
		( int)spPTDrvPin[_i].aMux);
#endif
	}
	
	sfOutputAPhasesToHSM(4);
	sfOutputAPhasesToVSM(4);
	
	if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Locate ){
		
		spPTLimPin = gClassPTZ.apLimPinArr->apPin;
		for( _i=0; _i < gClassPTZ.apLimPinArr->aNum ; _i++ ){
            gClassHAL.Pin->prfSetDirection( &spPTDrvPin[_i], DC_HAL_PIN_SetIN );
        /* 
			DC_PIN_SET_IO_IN(\
						spPTLimPin[_i].aGroup,\
						spPTLimPin[_i].aBit,\
						spPTLimPin[_i].aMuxVal,\
						spPTLimPin[_i].aMux);
         */                
#if DEBUG_PTZ
			NVCPrint("PIN  Lim_IN-> G:%d\tB:%d\tMuxVal:%d\tMux:%d\t",\
			( int)spPTLimPin[_i].aGroup,\
			( int)spPTLimPin[_i].aBit,\
			( int)spPTLimPin[_i].aMuxVal,\
			( int)spPTLimPin[_i].aMux);
#endif
		}
	}
	
	if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Positive ){
#if DEBUG_INIT
	NVCPrint("Set Anti_Clock order!");
#endif
		sStepMotorPhases = sAntiPhases;
	}else if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Negative ){
#if DEBUG_INIT
	NVCPrint("Set Clock order!");
#endif
		sStepMotorPhases = sRightPhases;
	}else{
		return -1;
	}
	
	// 
	if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Locate ){
		//
		if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Loitor ){
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
		
		sfdefPTZ_SetBeatHeart(\
						DC_PTF_SBH_Set | DC_PTF_SBH_Start\
						,DC_PTGlobal_DefPluse\
						,sfdefPTZ_FindZeroPoint);
		
		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,sfdefPTZ_ReportInitialDone,10);
		
	}else{
		sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Set,DC_PTGlobal_DefPluse,sfdefPTZ_PanTiltRuning);
	}
#if DEBUG_INIT
	NVCPrint("The PTZ Module Start! OK");
#endif
	return 0;
}

// ------------------------------------------------->sfdefPanTilt_Uninit
static int32 sfdefPanTilt_Uninit(void){
	sfdefPTZ_SetBeatHeart( DC_PTF_SBH_Stop, 0, NULL );
	return 0;
}

// ------------------------------------------------->sfdefPTZ_UpdateInfo
static void sfdefPTZ_UpdateInfo(void){
	
	mPanTiltInfo	*tpPTZInfo = gClassPTZ.apDefInfo;
	
	if( DF_PTS_IsStatusExist(DC_PTS_HIniting)||DF_PTS_IsStatusExist(DC_PTS_VIniting) )
		tpPTZInfo->aStatus &= ~DC_PTSP_InitDone;
	else
		tpPTZInfo->aStatus |= DC_PTSP_InitDone;
	
	
	if( DF_PTS_IsStatusExist(DC_PTS_HRunning)||DF_PTS_IsStatusExist(DC_PTS_HRunning) )
		tpPTZInfo->aStatus |= DC_PTSP_Busy;
	else
		tpPTZInfo->aStatus &= ~DC_PTSP_Busy;
	
	if( DF_PTS_IsStatusExist(DC_PTS_BuffFull) )
		tpPTZInfo->aStatus |= DC_PTSP_Full;
	else
		tpPTZInfo->aStatus &= ~DC_PTSP_Full;
	
	if( (tpPTZInfo->aRegCf&DC_PTSP_Locate)&&(tpPTZInfo->aRegCf&DC_PTSP_InitDone) ){
		tpPTZInfo->aXRange	= sPTPosition.aRange.aXRod;
		tpPTZInfo->aYRange	= sPTPosition.aRange.aYRod;
	}else{
		tpPTZInfo->aXRange	= 0;
		tpPTZInfo->aYRange	= 0;
	}
	
	tpPTZInfo->aHCurPos = sPTPosition.aCur.aXRod;
	tpPTZInfo->aYCurPos = sPTPosition.aCur.aYRod;
}


static void sfdefPTZ_RegInitCallBack( void (*iHandle)(void) ){
	sdefPTZ_InitCallBack = iHandle;
}

static void sfdefPTZ_ReportInitialDone(void){
	if( !(DF_PTS_IsStatusExist(DC_PTS_VIniting) || DF_PTS_IsStatusExist(DC_PTS_VIniting)) ){
		if( sdefPTZ_InitCallBack )
			sdefPTZ_InitCallBack();
		gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,sfdefPTZ_ReportInitialDone);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 这块区域里面主要关于启动操作云台的函数
// 检查弹道 缓存弹夹
// 弹头转换
// 调准射击方向
// 填充弹药
// 设立检测数据 调整发射距离
// 发射
static int32 sfdefPTZ_Start(mPTZAction iAct){
	int32 	tRet = 0;
	uint32 	tPTZIfoStatus = gClassPTZ.apDefInfo->aRegCf;
	
	
	if( DF_PTS_IsStatusExist( DC_PTS_BuffFull ) ){
		tRet |= DC_PTZRet_Busy|DC_PTZRet_BufFull;
		return tRet;
	}else{	
		if( (DF_PTS_IsStatusExist(DC_PTS_HRunning))\
			|| (DF_PTS_IsStatusExist(DC_PTS_VRunning)) ){
			sPTZActBuf = iAct;
			tRet	|= DC_PTZRet_BufFull;
			return tRet;
		}else{
			tRet 	|= DC_PTZRet_BufIdle;
		}
	}
	
	sfdefPTZ_MoveUnitConversion( &iAct );

	{
		uint8 tCmd = 0;
		if( tPTZIfoStatus&DC_PTSP_Supp_HLim )
			tCmd |= DC_PTF_FUAP_HLim;
		if( tPTZIfoStatus&DC_PTSP_Supp_VLim )
			tCmd |= DC_PTF_FUAP_VLim;
		
		if( iAct.aHComponent != 0 ){
			if( iAct.aCommand&DC_PTZCmd_Right ){
				if( tPTZIfoStatus&DC_PTSP_RightIsA ){
					tCmd |= DC_PTF_FUAP_HAnti;
				}else{
					tCmd |= DC_PTF_FUAP_HClock;
				}
			}else{
				if( tPTZIfoStatus&DC_PTSP_LeftIsA ){
					tCmd |= DC_PTF_FUAP_HAnti;
				}else{
					tCmd |= DC_PTF_FUAP_HClock;
				}
			}
		}
		if( iAct.aVComponent != 0 ){
			if( iAct.aCommand&DC_PTZCmd_Up ){
				if( tPTZIfoStatus&DC_PTSP_UpIsA ){
					tCmd |= DC_PTF_FUAP_VAnti;
				}else{
					tCmd |= DC_PTF_FUAP_VClock;
				}
			}else{
				if( tPTZIfoStatus&DC_PTSP_DownIsA ){
					tCmd |= DC_PTF_FUAP_VAnti;
				}else{
					tCmd |= DC_PTF_FUAP_VClock;
				}
			}
		}
#if DEBUG_PTZ
	NVCPrint("PTZ  SETSTART H: %d\t V: %d\t",\
			(int)iAct.aHComponent,\
			(int)iAct.aVComponent);
	NVCPrint("PTZ  SETSTART_POOL H: %d\t V: %d\t",\
			(int)sPTPosition.aPool.aXRod,\
			(int)sPTPosition.aPool.aYRod);
	
#endif
		sfdefPTZ_FillUpTheActPowder( tCmd, iAct.aHComponent, iAct.aVComponent );
	}
	
	if( sPTPosition.aPool.aXRod ){
		DF_PTS_SetStatus(DC_PTS_HRunning);
		DF_PTS_SetStatus(DC_PTS_HPeriodDone);
	}
	
	if( sPTPosition.aPool.aYRod ){
		DF_PTS_SetStatus(DC_PTS_VRunning);
		DF_PTS_SetStatus(DC_PTS_VPeriodDone);
	}
	
	if( DF_PTS_IsStatusExist(DC_PTS_HRunning)&&DF_PTS_IsStatusExist(DC_PTS_VRunning) ){
		sRaducingCount = 0;
		DF_PTS_ClrStatus(DC_PTS_HReducing)
		DF_PTS_ClrStatus(DC_PTS_VReducing)
		if(iAct.aHComponent>iAct.aVComponent){
			sRaducingRatio = (uint32)(iAct.aHComponent<<7)/iAct.aVComponent;
			DF_PTS_SetStatus(DC_PTS_VReducing);
		}else{
			sRaducingRatio = (uint32)(iAct.aVComponent<<7)/iAct.aHComponent;
			DF_PTS_SetStatus(DC_PTS_HReducing);
		}
	}
	
	if( iAct.aCommand&DC_PTZCmd_NeedLoitor ){
		sPTLoitor = DC_PTGlobal_Loitor;
	}
	
	if( iAct.aSpeed > 0 && iAct.aSpeed <=100  ){
		iAct.aSpeed = iAct.aSpeed*DC_PTGlobal_IncUnit + DC_PTGlobal_MinPluse;
	}else{
		iAct.aSpeed = DC_PTGlobal_DefPluse;
	}
	
	sfdefPTZ_SetBeatHeart( DC_PTF_SBH_Set|DC_PTF_SBH_Start, iAct.aSpeed, sfdefPTZ_PanTiltRuning );
	
	return tRet;
}

static int32 sfdefPTZ_FillUpTheActPowder(uint8 iCmd, uint16 iHComp,uint16 iVComp){
	
	int32 tStatus = 0;
	
	if( iCmd&DC_PTF_FUAP_HClock ){
		DF_PTS_ClrStatus(DC_PTS_HAntiRoll);
		if( iCmd&DC_PTF_FUAP_HLim ){
			if( (iHComp+sPTPosition.aCur.aXRod)>sPTPosition.aRange.aXRod ){
				iHComp = sPTPosition.aRange.aXRod - sPTPosition.aCur.aXRod;
				tStatus |= 0x01;
			}
		}
	}else if( iCmd&DC_PTF_FUAP_HAnti ){
		DF_PTS_SetStatus(DC_PTS_HAntiRoll);
		if( iCmd&DC_PTF_FUAP_HLim ){
			if( iHComp > sPTPosition.aCur.aXRod ){
				iHComp  = sPTPosition.aCur.aXRod;
				tStatus |= 0x01;
			}
		}
	}
	
	if( iCmd&DC_PTF_FUAP_VClock ){
		DF_PTS_ClrStatus(DC_PTS_VAntiRoll);
		if( iCmd&DC_PTF_FUAP_VLim ){
			if( (iVComp + sPTPosition.aCur.aYRod) > sPTPosition.aRange.aYRod ){
				iVComp = sPTPosition.aRange.aYRod - sPTPosition.aCur.aYRod;
				tStatus |= 0x02;
			}
		}
	}else if( iCmd&DC_PTF_FUAP_VAnti ){
		DF_PTS_SetStatus(DC_PTS_VAntiRoll);
		if( iCmd&DC_PTF_FUAP_VLim ){
			if( iVComp > sPTPosition.aCur.aYRod ){
				iVComp = sPTPosition.aCur.aYRod;
				tStatus |= 0x02;
			}
		}
	}
	
	sPTPosition.aPool.aXRod = iHComp;
	sPTPosition.aPool.aYRod = iVComp;
	return tStatus;
}

static void sfdefPTZ_MoveUnitConversion(mPTZAction *iAct){
	uint32	tCmd = iAct->aCommand;
	uint32	tHComp = iAct->aHComponent;
	uint32	tVComp = iAct->aVComponent;
	
	
	if( tCmd&DC_PTZCmd_Dgr ){
		tHComp = (tHComp * gClassPTZ.apDefInfo->aHStepsPerFull)/360;
		tVComp = (tVComp * gClassPTZ.apDefInfo->aVStepsPerFull)/360;
	}else if( tCmd&DC_PTZCmd_Crd ){
		if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Locate ){
			if( tHComp > sPTPosition.aRange.aXRod ) tHComp = sPTPosition.aRange.aXRod;
			if( tVComp > sPTPosition.aRange.aYRod ) tVComp = sPTPosition.aRange.aYRod;
			
			tCmd &= ~DC_PTZCmd_DirMask;
			if( tHComp >= sPTPosition.aCur.aXRod ){
				tHComp = tHComp - sPTPosition.aCur.aXRod;
				tCmd |= DC_PTZCmd_Right;
			}else{ //if( tHComp<sPTPosition.aCur.aXRod ){
				tHComp = sPTPosition.aCur.aXRod - tHComp;
				tCmd |= DC_PTZCmd_Left;
			}
			
			if( tVComp >= sPTPosition.aCur.aYRod ){
				tVComp = tVComp - sPTPosition.aCur.aYRod;
				tCmd |= DC_PTZCmd_Up;
			}else{ //if( tVComp<sPTPosition.aCur.aYRod ){
				tVComp = sPTPosition.aCur.aYRod - tVComp;
				tCmd |= DC_PTZCmd_Down;
			}
			
		}else{
			tCmd |= DC_PTZCmd_Err;
		}
	}else{
		return;
	}
	tCmd |= DC_PTZCmd_Ok;
	
	iAct->aCommand   = tCmd   ; 
	iAct->aHComponent = tHComp ;
	iAct->aVComponent = tVComp ;
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$&&&& //
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$&&&& //




static int32 sfdefPTZ_Stop(void){
	sPTPosition.aPool.aXRod = 0;
	sPTPosition.aPool.aYRod = 0;
	DF_PTS_ClrStatus(DC_PTS_BuffFull);
	return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void sfdefPTZ_PanTiltRuning(void){
	
	uint8 theCurPhases;
	if(sPTLoitor != 0){
		sPTLoitor--;
		return ;
	}
	
	if( DF_PTS_IsStatusExist(DC_PTS_HRunning) ){
		if(DF_PTS_IsStatusExist(DC_PTS_HReducing)){
			sRaducingCount+=1<<7;
			if(sRaducingCount>=sRaducingRatio){
				sRaducingCount = sRaducingCount-sRaducingRatio;
			}else{
				goto GT_PanTiltRuning_HEnd;
			}
		}
		
		if( DF_PTS_IsStatusExist(DC_PTS_HPeriodDone) ){
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			
			if( sPTPosition.aPool.aXRod > 0 ){
				sPTPosition.aPool.aXRod--;
				if( DF_PTS_IsStatusExist(DC_PTS_HAntiRoll) ){
					sPTPosition.aCur.aXRod--;
				}else{
					sPTPosition.aCur.aXRod++;
				}
			}else{
				sfOutputAPhasesToHSM(4);
                
				DF_PTS_ClrStatus(DC_PTS_HIniting);
				DF_PTS_ClrStatus(DC_PTS_HRunning);
				if(!DF_PTS_IsStatusExist(DC_PTS_VRunning)){
					goto GT_PanTiltRuning_AllEnd;//JumpOutTheLoop
				}
				goto GT_PanTiltRuning_HEnd;
			}
			
		}
		
		if(DF_PTS_IsStatusExist(DC_PTS_HAntiRoll)){
			theCurPhases = 3 - DF_PTS_GetHRhythm;
			if( !DF_PTS_GetHOriStatus ){
				sPTPosition.aPool.aXRod = 0;
				sPTPosition.aCur.aXRod = 0;
			}
		}else{
			theCurPhases = DF_PTS_GetHRhythm;
			if( !DF_PTS_GetHTilStatus ){
				sPTPosition.aPool.aXRod = 0;
				sPTPosition.aCur.aXRod = sPTPosition.aRange.aXRod;
			}
		}
		DF_PTS_HaddRhythm;
		sfOutputAPhasesToHSM(theCurPhases);
	}
GT_PanTiltRuning_HEnd:
	if( DF_PTS_IsStatusExist(DC_PTS_VRunning) ){
		if(DF_PTS_IsStatusExist(DC_PTS_VReducing)){
			sRaducingCount+=1<<7;
			if(sRaducingCount>=sRaducingRatio){
				sRaducingCount = sRaducingCount-sRaducingRatio;
			}else{
				goto GT_PanTiltRuning_VEnd;
			}
		}
		
		if( DF_PTS_IsStatusExist(DC_PTS_VPeriodDone) ){
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			
			if( sPTPosition.aPool.aYRod > 0 ){
				sPTPosition.aPool.aYRod--;
				if( DF_PTS_IsStatusExist(DC_PTS_VAntiRoll) ){
					sPTPosition.aCur.aYRod--;
				}else{
					sPTPosition.aCur.aYRod++;
				}
			}else{
				sfOutputAPhasesToVSM(4);
				DF_PTS_ClrStatus(DC_PTS_VIniting);
				DF_PTS_ClrStatus(DC_PTS_VRunning);
				if(!DF_PTS_IsStatusExist(DC_PTS_HRunning)){
					goto GT_PanTiltRuning_AllEnd;//JumpOutTheLoop
				}
				goto GT_PanTiltRuning_VEnd;
			}
		}
		
		if(DF_PTS_IsStatusExist(DC_PTS_VAntiRoll)){
			theCurPhases = 3 - DF_PTS_GetVRhythm;
			if( !DF_PTS_GetVOriStatus ){
				sPTPosition.aPool.aYRod = 0;
				sPTPosition.aCur.aYRod = 0;
			}
		}else{
			theCurPhases = DF_PTS_GetVRhythm;
			if( !DF_PTS_GetVTilStatus ){
				sPTPosition.aPool.aYRod = 0;
				sPTPosition.aCur.aYRod = sPTPosition.aRange.aYRod;
			}
		}
		DF_PTS_VaddRhythm;
		sfOutputAPhasesToVSM(theCurPhases);
	}
	
GT_PanTiltRuning_VEnd:
	return;
GT_PanTiltRuning_AllEnd:
	if( DF_PTS_IsStatusExist(DC_PTS_BuffFull) ){
		DF_PTS_ClrStatus(DC_PTS_BuffFull);
		
		sfdefPTZ_Start(sPTZActBuf);
	}else{
		sfdefPTZ_SetBeatHeart(DC_PTF_SBH_Stop,0,NULL);
	}
	return ;
}

static void sfdefPTZ_FindZeroPoint(void){
	uint8 theCurrentPhases;
	
	if( DF_PTS_IsStatusExist(DC_PTS_HRunning) ){
		if( DF_PTS_IsStatusExist(DC_PTS_HPeriodDone) ){
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			sPTPosition.aPool.aXRod++;
				
			if( (!DF_PTS_GetHOriStatus)\
			|| ( sPTPosition.aPool.aXRod > gClassPTZ.apDefInfo->aHLimitSteps ) ){
				
				DF_PTS_ClrStatus( DC_PTS_HRunning );
				if( !DF_PTS_IsStatusExist(DC_PTS_VRunning) )
					goto GT_FindZeroPoint_ALLEnd;
				goto GT_FindZeroPoint_HEnd;
			}
		}
		theCurrentPhases = 3 - DF_PTS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
GT_FindZeroPoint_HEnd:	
	if( DF_PTS_IsStatusExist(DC_PTS_VRunning) ){
		if( DF_PTS_IsStatusExist(DC_PTS_VPeriodDone) ){
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			sPTPosition.aPool.aYRod++;
			if( (!DF_PTS_GetVOriStatus)\
			||(( sPTPosition.aPool.aYRod > gClassPTZ.apDefInfo->aVLimitSteps )) ){
				
				DF_PTS_ClrStatus( DC_PTS_VRunning );
				if( !DF_PTS_IsStatusExist(DC_PTS_HRunning) )
					goto GT_FindZeroPoint_ALLEnd;
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
	if( gClassPTZ.apDefInfo->aRegCf&DC_PTSP_Loitor ){
		sPTLoitor = DC_PTGlobal_Loitor;
	}
	
#if DEBUG_PTZ
	NVCPrint("PTZ  OriPosition H:%d\t V:%d\t",\
			(int)sPTPosition.aPool.aXRod,\
			(int)sPTPosition.aPool.aYRod);
#endif
	// Running
	DF_PTS_SetStatus(DC_PTS_HRunning);
	DF_PTS_SetStatus(DC_PTS_VRunning);
	// period done
	DF_PTS_SetStatus(DC_PTS_HPeriodDone);
	DF_PTS_SetStatus(DC_PTS_VPeriodDone);
	
	sfdefPTZ_SetBeatHeart(\
						DC_PTF_SBH_Set | DC_PTF_SBH_Start \
						,DC_PTGlobal_DefPluse \
						,sfdefPTZ_FigureOutTheRange);
	
	return ;
}


static void sfdefPTZ_FigureOutTheRange(void){
	uint8 theCurrentPhases;
	
	if( DF_PTS_IsStatusExist(DC_PTS_HRunning) ){
		if( DF_PTS_IsStatusExist(DC_PTS_HPeriodDone) ){
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			sPTPosition.aRange.aXRod++;
			if( (!DF_PTS_GetHTilStatus)\
			||( sPTPosition.aRange.aXRod > gClassPTZ.apDefInfo->aHLimitSteps) ){
				DF_PTS_ClrStatus( DC_PTS_HRunning );
                
                sPTPosition.aPool.aXRod = gClassPTZ.apDefInfo->aHStepsPerFull;
                sPTPosition.aRange.aXRod = gClassPTZ.apDefInfo->aHStepsPerFull;
                sPTPosition.aCur.aXRod = gClassPTZ.apDefInfo->aHStepsPerFull;
                sfOutputAPhasesToHSM(4);
                
				if( !DF_PTS_IsStatusExist(DC_PTS_VRunning) )
					goto GT_FigureOutTheRange_ALLEnd;
				goto GT_FigureOutTheRange_HEnd;
			}
		}
		theCurrentPhases = DF_PTS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
GT_FigureOutTheRange_HEnd:

	if( DF_PTS_IsStatusExist(DC_PTS_VRunning) ){
		if( DF_PTS_IsStatusExist(DC_PTS_VPeriodDone) ){
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			sPTPosition.aRange.aYRod++;
			if( (!DF_PTS_GetVTilStatus)\
			||( sPTPosition.aRange.aXRod > gClassPTZ.apDefInfo->aHLimitSteps)){
				DF_PTS_ClrStatus( DC_PTS_VRunning );
                
                sPTPosition.aPool.aYRod = gClassPTZ.apDefInfo->aVStepsPerFull;
                sPTPosition.aRange.aYRod = gClassPTZ.apDefInfo->aVStepsPerFull;
                sPTPosition.aCur.aYRod = gClassPTZ.apDefInfo->aVStepsPerFull;
                sfOutputAPhasesToVSM(4);
                
				if( !DF_PTS_IsStatusExist(DC_PTS_HRunning) )
					goto GT_FigureOutTheRange_ALLEnd;
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
	
	if( sPTPosition.aRange.aXRod > sPTPosition.aPool.aXRod ){
		sPTPosition.aPool.aXRod = sPTPosition.aRange.aXRod - sPTPosition.aPool.aXRod;
	}else{
		sPTPosition.aPool.aXRod = 0;
	}
	if( sPTPosition.aRange.aYRod > sPTPosition.aPool.aYRod ){
		sPTPosition.aPool.aYRod = sPTPosition.aRange.aYRod - sPTPosition.aPool.aYRod;
	}else{
		sPTPosition.aPool.aYRod = 0;
	}
	sPTPosition.aCur.aXRod = sPTPosition.aRange.aXRod;
	sPTPosition.aCur.aYRod = sPTPosition.aRange.aYRod;
	
#if DEBUG_PTZ
	NVCPrint("PTZ  RANGE H:%d\t V:%d\t",\
			(int)sPTPosition.aRange.aXRod,\
			(int)sPTPosition.aRange.aYRod);
	NVCPrint("PTZ  SPOOL H:%d\t V:%d\t",\
			(int)sPTPosition.aPool.aXRod,\
			(int)sPTPosition.aPool.aYRod);
#endif
	
	{
		mPTZAction tPTZAction;
		
		tPTZAction.aCommand 	= \
				DC_PTZCmd_Steps | DC_PTZCmd_Down | DC_PTZCmd_Left | DC_PTZCmd_NeedLoitor;
		
		tPTZAction.aHComponent 	= sPTPosition.aPool.aXRod ;
		tPTZAction.aVComponent 	= sPTPosition.aPool.aYRod ;
		tPTZAction.aSpeed 		= 50;

		sfdefPTZ_Start( tPTZAction );
	}
	return ;
}






static int32 sfdefPTZ_SetBeatHeart(\
							uint8 iCmd,\
							uint16 iPluse,\
							void(*iHandler)(void)){
	int32 tRet;
	if( iCmd&DC_PTF_SBH_Set ){
		gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(1));
		if( (iPluse != 0)&&(iHandler != NULL) ){
			mTIMER_param tSet_Param={
				1000000/iPluse,
				iHandler,
				DC_HAL_TIMER_cmdPTMode
			};
			tRet = gClassHAL.Timer->prfConfig( DC_HAL_TIMERChannel(1), &tSet_Param );
			
			if( tRet )
				return -1;
		}
	}
	if( iCmd&DC_PTF_SBH_Stop ){
		gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(1));
	}else if( iCmd&DC_PTF_SBH_Start ){
		gClassHAL.Timer->prfStart(DC_HAL_TIMERChannel(1));
	}
	return 0;
}

// -------------------------------------------------> Pin operate
static void sfOutputAPhasesToHSM(uint8 _cmd)
{
	printk("HSM:%x\r\n",(unsigned int)_cmd);
	if(0x01&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[0], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[0], DC_HAL_PIN_SetLOW );
	}
	if(0x02&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[1], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[1], DC_HAL_PIN_SetLOW );
	}
	if(0x04&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[2], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[2], DC_HAL_PIN_SetLOW );
	}
	if(0x08&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[3], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[3], DC_HAL_PIN_SetLOW );
	}
}
static void sfOutputAPhasesToVSM(uint8 _cmd)
{
	printk("VSM:%x\r\n",(unsigned int)_cmd);  
    if(0x01&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[4], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[4], DC_HAL_PIN_SetLOW );
	}
	if(0x02&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[5], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[5], DC_HAL_PIN_SetLOW );
	}
	if(0x04&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[6], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[6], DC_HAL_PIN_SetLOW );
	}
	if(0x08&sStepMotorPhases[_cmd]){
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[7], DC_HAL_PIN_SetHIGH );
	}else{
        gClassHAL.Pin->prfSetExport( &spPTDrvPin[7], DC_HAL_PIN_SetLOW );
	}
}
