#include "PanTilt.h"

#include "../HAL/USERHAL.h"
#include "../HAL/HAL_Timer.h"
#include "../HAL/PeriodEvent.h"
#include "../MsgCenter.h"
#include "../ProInfo.h"
#include "../String.h"

#include <linux/interrupt.h>


// ================================================================================
// ---------------------------------------------------------------------->Product definition
// ------------------------------------------------------------>D01
struct{
	uint32 aHDgrPerSteps;
	uint32 aVDgrPerSteps;
	uint32 aHMinSteps;
	uint32 aVMinSteps;
	uint32 aHZeroAxis;
	uint32 aVZeroAxis;
}mPanTilt_BaseIfo;
// -------------------------------------------------->
#define DC_D03_3518C_HDgrPerSteps	378605		// 定轴 26 动轴 14 步距角 5.625/64 四相八拍 
#define DC_D03_3518C_VDgrPerSteps	703125		// 定轴 1  动轴 1  步距角 5.625/64 四相八拍
#define DC_D03_3518C_HMinStep		1
#define DC_D03_3518C_VMinStep		1
#define DC_D03_3518C_HRange			655
#define DC_D03_3518C_VRange			73
#define DC_D03_3518C_PTDF			(DF_PTDF_Left+DF_PTDF_Down)
// -------------------------------------------------->
#define DC_D03_3518C_PanTiltNum			12
static mGPIOPinMsg saD03_3518C_PanTiltPin[DC_D03_3518C_PanTiltNum]={
	
	{6,0,32,0x00}, // HA
	{6,1,33,0x00}, // HB
	{1,1,1 ,0x00}, // HC
	{6,3,35,0x00}, // HD
	{6,4,36,0x00}, // VA
	{6,5,37,0x02}, // VB
	{6,6,38,0x00}, // VC
	{6,7,39,0x00}, // VD
	
	{2,3,9 ,0x00}, // LHHead // Origin
	{2,5,11,0x00}, // LHTail // tail
	{5,2,47,0x00}, // LVHead // Origin
	{2,2,8 ,0x00}, // LVTail // tail
};


// ------------------------------------------------------------>D02
// -------------------------------------------------->
#define DC_D11_3518C_HDgrPerSteps	164794		// 定轴 64 动轴 15 步距角 5.625/64 四相八拍
#define DC_D11_3518C_VDgrPerSteps	703125		// 定轴 1  动轴 1  步距角 5.625/64 四相八拍
#define DC_D11_3518C_HMinStep		1
#define DC_D11_3518C_VMinStep		1
#define DC_D11_3518C_PTDF			(DF_PTDF_Right+DF_PTDF_Up)
// -------------------------------------------------->
#define DC_D11_3518C_PanTiltNum			8
static mGPIOPinMsg saD11_3518C_PanTiltPin[DC_D11_3518C_PanTiltNum]={
	{4,4,21,0x00}, // HA
	{4,5,20,0x00}, // HB
	{4,6,19,0x00}, // HC
	{4,7,18,0x00}, // HD
	
	{4,0,17,0x00}, // VA
	{4,1,16,0x00}, // VB
	{4,2,15,0x00}, // VC
	{4,3,14,0x00}, // VD
};


// ------------------------------------------------------------>D02
// -------------------------------------------------->
#define DC_F05_3518E_HDgrPerSteps	527343		// 定轴 24 动轴 18 步距角 5.625/64 四相八拍
#define DC_F05_3518E_VDgrPerSteps	703125		// 定轴 1  动轴 1  步距角 5.625/64 四相八拍
#define DC_F05_3518E_HMinStep		1
#define DC_F05_3518E_VMinStep		1
#define DC_F05_3518E_PTDF			(DF_PTDF_Left + DF_PTDF_Down)
// -------------------------------------------------->
#define DC_F05_3518E_PanTiltNum			12
static mGPIOPinMsg saF05_3518E_PanTiltPin[DC_F05_3518E_PanTiltNum]={

	{9,3,53,0x01}, // HD
	{9,2,52,0x01}, // HC
	{9,1,51,0x01}, // HB
	{9,0,50,0x01}, // HA
	
	{9,7,57,0x01}, // VD
	{9,6,56,0x01}, // VC
	{9,5,55,0x01}, // VB
	{9,4,54,0x01}, // VA

	{5,4,68,0x00}, // LHHead // Origin	
	{5,5,69,0x00}, // LHTail // tail

	{5,7,71,0x00}, // LV
	{5,6,70,0x00}, // LVHead // Origin
};


// ================================================================================
// ---------------------------------------------------------------------->Driver Info
// ------------------------------------------------------------>driver order
static uint8 sRightPhases[5]={0x0C,0x09,0x03,0x06,0x00};
static uint8 sAntiPhases[5]={0x03,0x06,0x0C,0x09,0x0F};
static uint8 *sStepMotorPhases = sAntiPhases;
// ------------------------------------------------------------>Step Motor Pin
static mGPIOPinMsg *spPanTiltPin;
static uint8 sPanTiltPinNum;
// ------------------------------------------------------------>Report 
// Report Call back function
void (*gfPanTilt_ReportCurrentPosition)(uint16 iXRod,uint16 iYRod) = NULL ;
// Check report frequent
#define DC_PanTilt_InitCheckFreq 	10   	// 1s
// ------------------------------------------------------------>Driver Clock
#define DC_PanTilt_BaseFreq			50
#define DC_PanTilt_DefaultFreq 		126
#define DC_PanTilt_DefaultDelay 	300
// ------------------------------------------------------------>Driver Status
static uint32	 	sPTStatus;
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
#define DC_PTS_BuffFull		0x00008000
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
#define DF_PTS_GetHOriStatus		(DF_Get_GPIO_INx(spPanTiltPin[8].aGroup,spPanTiltPin[8].aBit))
#define DF_PTS_GetHTilStatus		(DF_Get_GPIO_INx(spPanTiltPin[9].aGroup,spPanTiltPin[9].aBit))
#define DF_PTS_GetVOriStatus		(DF_Get_GPIO_INx(spPanTiltPin[10].aGroup,spPanTiltPin[10].aBit))
#define DF_PTS_GetVTilStatus		(DF_Get_GPIO_INx(spPanTiltPin[11].aGroup,spPanTiltPin[11].aBit))
// ------------------------------------------------------------>Director Flag
// The variable aims to identify the relationship of direction  
// between the PTZ and the stepper motor .
// like: 	Step motor anticlockwise running corresponding the The direction Right
// 			you need set the First bit in this variable
//          sDirectFlag	|= DF_PTDF_Right;
static uint8		sDirectFlag = 0;
#define DF_PTDF_Left	0x01
#define DF_PTDF_Right	0x02
#define DF_PTDF_Down	0x04
#define DF_PTDF_Up		0x08
// ------------------------------------------------------------>Command Buffer
static mPTZAction 	sPTZActBuf;
// ------------------------------------------------------------>Weapon cooling time
static uint32 		sMotorDelay;
// ------------------------------------------------------------>Pan tilt position info
// record the pan tilt run of activity, current position and displacement vector
static mPaTiInfo 	sPaTiInfo;
// ------------------------------------------------------------>Reduction ratio
// when the displacement and the Axes director vector is discrepancy,
// it could cause the speed is different in two component,
// this val will be used to record reducing ratio at one side. 
static uint32 		sRaducingRatio;
// 
static uint32 		sRaducingCount;
// ------------------------------------------------------------>Driver interface
static void (*spfTimer0ServerFunction)(void);



// ================================================================================
// ---------------------------------------------------------------------->Local function definition
static void sfDriveToFindOutCoordinate(void);
static void sfDriveToFindOutMaxRange(void);
static void sfStepMotorDriver(void);
static void sfPanTilt_ReportLocalPosition(void);
static void sfOutputAPhasesToHSM(uint8 _cmd);
static void sfOutputAPhasesToVSM(uint8 _cmd);
static irqreturn_t sfPTZHandle(int irq, void *id);










//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
int32 gfInitDrv_PanTilt(uint8 iRemain){
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	int tStatus = 0;
	uint8 _i;
	
	sMotorDelay = DC_PanTilt_DefaultDelay;
	sPTStatus = 0;
	gfMemset((uint8*)&sPaTiInfo,0,sizeof(mPaTiInfo));
	sRaducingRatio = 0;
	sRaducingCount = 0;
	spfTimer0ServerFunction = NULL;
	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D03:{
			sDirectFlag 	= DC_D03_3518C_PTDF;
			sPanTiltPinNum 	= DC_D03_3518C_PanTiltNum;
			spPanTiltPin 	= saD03_3518C_PanTiltPin;
			spfTimer0ServerFunction	= sfDriveToFindOutCoordinate;
			
			mPanTilt_BaseIfo.aHDgrPerSteps = 	DC_D03_3518C_HDgrPerSteps;
			mPanTilt_BaseIfo.aVDgrPerSteps = 	DC_D03_3518C_VDgrPerSteps;
			mPanTilt_BaseIfo.aHMinSteps = 		DC_D03_3518C_HMinStep;
			mPanTilt_BaseIfo.aVMinSteps = 		DC_D03_3518C_VMinStep;
			mPanTilt_BaseIfo.aHZeroAxis = 		0;
			mPanTilt_BaseIfo.aVZeroAxis =  		0;
			
			// Set initialize status to
			DF_PTS_SetStatus(DC_PTS_HIniting);
			DF_PTS_SetStatus(DC_PTS_VIniting);
			// start to find the origin
			DF_PTS_SetStatus(DC_PTS_VRunning);
			DF_PTS_SetStatus(DC_PTS_VPeriodDone);
			
			DF_PTS_SetStatus(DC_PTS_HRunning);
			DF_PTS_SetStatus(DC_PTS_HPeriodDone);
			
			tStatus |= 0x01;
		}
		break;
		
		case DC_Pro_D11:{
			sDirectFlag 			= DC_D11_3518C_PTDF;
			sPanTiltPinNum 			= DC_D11_3518C_PanTiltNum;
			spPanTiltPin 			= saD11_3518C_PanTiltPin;
			spfTimer0ServerFunction = sfStepMotorDriver;
			
			mPanTilt_BaseIfo.aHDgrPerSteps = 	DC_D11_3518C_HDgrPerSteps;
			mPanTilt_BaseIfo.aVDgrPerSteps = 	DC_D11_3518C_VDgrPerSteps;
			mPanTilt_BaseIfo.aHMinSteps = 		DC_D11_3518C_HMinStep;
			mPanTilt_BaseIfo.aVMinSteps = 		DC_D11_3518C_VMinStep;
			mPanTilt_BaseIfo.aHZeroAxis = 		0;
			mPanTilt_BaseIfo.aVZeroAxis =  		0;
		}break;
		case DC_Pro_D01:
		case DC_Pro_D04:
		return 1;
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F05:{
			sDirectFlag = DC_F05_3518E_PTDF;
			sStepMotorPhases = sRightPhases;
			
			sPanTiltPinNum = DC_F05_3518E_PanTiltNum;
			spPanTiltPin = saF05_3518E_PanTiltPin;
			spfTimer0ServerFunction = sfDriveToFindOutCoordinate;
			
			mPanTilt_BaseIfo.aHDgrPerSteps = 	DC_F05_3518E_HDgrPerSteps;
			mPanTilt_BaseIfo.aVDgrPerSteps = 	DC_F05_3518E_VDgrPerSteps;
			mPanTilt_BaseIfo.aHMinSteps = 		DC_F05_3518E_HMinStep;
			mPanTilt_BaseIfo.aVMinSteps = 		DC_F05_3518E_VMinStep;
			mPanTilt_BaseIfo.aHZeroAxis = 		0;
			mPanTilt_BaseIfo.aVZeroAxis =  		0;
			
			// Set initialize status to
			DF_PTS_SetStatus(DC_PTS_HIniting);
			DF_PTS_SetStatus(DC_PTS_VIniting);
			// start to find the origin
			DF_PTS_SetStatus(DC_PTS_VRunning);
			DF_PTS_SetStatus(DC_PTS_VPeriodDone);
			
			DF_PTS_SetStatus(DC_PTS_HRunning);
			DF_PTS_SetStatus(DC_PTS_HPeriodDone);
			
			tStatus |= 0x01;
		}break;
		case DC_Pro_F07:
		case DC_Pro_F08:
		return 1;
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	for( _i=0; _i< sPanTiltPinNum;_i++ ){
		if(_i<8){
			DF_Set_GPIO_DIRx(spPanTiltPin[_i].aGroup,spPanTiltPin[_i].aBit);
		}else{
			DF_Clr_GPIO_DIRx(spPanTiltPin[_i].aGroup,spPanTiltPin[_i].aBit);
			// DF_Set_GPIO_Outx(spPanTiltPin[_i].aGroup,spPanTiltPin[_i].aBit);
		}
		DF_Set_GPIO_MUXx(spPanTiltPin[_i].aMuxVal,spPanTiltPin[_i].aMux);
	}
	sfOutputAPhasesToHSM(8);
	sfOutputAPhasesToVSM(8);
	
	
	if(request_irq( IRQ_TM2_TM3, sfPTZHandle,IRQF_SHARED,"NVC_PTZ",&sPaTiInfo)){
#if OPEN_DEBUG
	NVCPrint("The Service Timer of PTZ requests fail!\r\n");
#endif
		return -1;
	}
	
	
	if(tStatus&0x01){
		fgConfigTimer3(sMotorDelay,DC_Timer_PeriodMode|DC_Timer_Intenable|DC_Timer_Timerpre_256|DC_Timer_Count_32bit);
		DF_Timer3_Enable;
	}
	
	
#if OPEN_DEBUG
	NVCPrint("Pan Tilt *** Started!\r\n");
#endif
	return 0;
}


//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
int32 gfUninitDrv_PanTilt(uint8 iRemain){
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D03:
		case DC_Pro_D11:{
			DF_Timer3_Disable;
			free_irq(IRQ_TM2_TM3,&sPaTiInfo);
		}break;	
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F05:{
			DF_Timer3_Disable;
			free_irq(IRQ_TM2_TM3,&sPaTiInfo);
		}
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	return 0;
}

//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
int32 gfGetPanTiltInfo(mNVC_PANTILT_INFO *iInfo){
	
	iInfo->aStatus = 0;
	
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F05:
		case DC_Pro_D03:{
			
			iInfo->aCmdMask = \
			NVC_PTZ_SUPP_HMOVE|\
			NVC_PTZ_SUPP_VMOVE|\
			NVC_PTZ_SUPP_HVMOVE|\
			NVC_PTZ_SUPP_HSCAN|\
			NVC_PTZ_SUPP_VSCAN|\
			NVC_PTZ_SUPP_HLIMIT|\
			NVC_PTZ_SUPP_VLIMIT|\
			NVC_PTZ_SUPP_ZERO;
			
			if(DF_PTS_IsStatusExist(DC_PTS_HRunning)||DF_PTS_IsStatusExist(DC_PTS_VRunning))
				iInfo->aStatus |= NVC_PTZ_STATUS_Busy;
			
			
			if((!DF_PTS_IsStatusExist(DC_PTS_HIniting))&&(!DF_PTS_IsStatusExist(DC_PTS_VIniting)))
				iInfo->aStatus |= NVC_PTZ_STATUS_Initing;
			
			if(DF_PTS_IsStatusExist(DC_PTS_BuffFull))
				iInfo->aStatus |= NVC_PTZ_STATUS_CmdFull;
			
		}break;
		case DC_Pro_D11:{
			iInfo->aCmdMask = \
			NVC_PTZ_SUPP_HMOVE|\
			NVC_PTZ_SUPP_VMOVE|\
			NVC_PTZ_SUPP_HVMOVE;
			
			iInfo->aStatus |= NVC_PTZ_STATUS_Initing;
			
		}break;
		default: return -1;
	}
	iInfo->aHRange 			= sPaTiInfo.aRange.aXRod;
	iInfo->aVRange 			= sPaTiInfo.aRange.aYRod;
	
	
	iInfo->aDgrPerHSteps 	= mPanTilt_BaseIfo.aHDgrPerSteps;
	iInfo->aDgrPerVSteps 	= mPanTilt_BaseIfo.aVDgrPerSteps;
	
	iInfo->aUnitHSteps 		= mPanTilt_BaseIfo.aHMinSteps;
	iInfo->aUnitVSteps 		= mPanTilt_BaseIfo.aVMinSteps;
	
	iInfo->aZeroXPos 		= mPanTilt_BaseIfo.aHZeroAxis;
	iInfo->aZeroYPos 		= mPanTilt_BaseIfo.aVZeroAxis;
	
	
	iInfo->aXPos 			= sPaTiInfo.aCurPosition.aXRod;
	iInfo->aYPos 			= sPaTiInfo.aCurPosition.aYRod;
	
	
	return 0;
}

//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
int32 gfGetPanTiltCurPos(mNVC_PANTILT_Respons *iInfo){
	iInfo->aStatus = 0x00;
	iInfo->aXPos = 	sPaTiInfo.aCurPosition.aXRod;
	iInfo->aYPos = 	sPaTiInfo.aCurPosition.aYRod;
	return 0;
}

//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
// left down is Anti 
// step one: Check the motor system is or not busy
// step two: translate of the angle or coordinates to the count steps
// step three: set the diraction
// step four: set the capacity
// step five: enable the step motor
uint32 gfStartPTMove(uint16 iHSteps, uint16 iVSteps,uint16 iSpeed,uint32 iCmd)
{
	uint32 tForReturn = 0;
	
	uint8 tStatus = 0;
// 0x02 Horizon empty
// 0x01 Horizon has data
// 0x08 Vertical empty
// 0x04 Vertical has data
// 0x80	Start Limit ability
	
	if(iCmd&DC_SSMM_Stop){// stop the motor
		sPaTiInfo.aStepPool.aXRod = 0;			
		sPaTiInfo.aStepPool.aYRod = 0;
		goto GT_Endone_fgfStartPTMove;
	}
	
	// step one
	if(DF_PTS_IsStatusExist(DC_PTS_BuffFull)){
		tForReturn |= DC_SPMS_Busy;
		goto GT_Endone_fgfStartPTMove;
	}else{			
		if( (DF_PTS_IsStatusExist(DC_PTS_HRunning))\
		|| (DF_PTS_IsStatusExist(DC_PTS_VRunning)) ){
			
			DF_PTS_SetStatus(DC_PTS_BuffFull);
			sPTZActBuf.aHComponent = iHSteps;
			sPTZActBuf.aVComponent = iVSteps;
			sPTZActBuf.aHSpeed = iSpeed;
			sPTZActBuf.aHCommand = iCmd;
			goto GT_Endone_fgfStartPTMove;
		}else{
			tForReturn |= DC_SPMS_BufIdle;
		}
	}
	
	if(iCmd&DC_SSMM_Dgr){
	// Angel
	// iHSteps = iHSteps*1024/360 == iHSteps*128/45 = (iHSteps<<7)/45
	// 
		iHSteps = (iHSteps<<7)/45;
		iVSteps = (iVSteps<<7)/45;
		
	}else if(iCmd&DC_SSMM_Crd){
	// Coordinate
		int16 theCount_1;
		
		if(iHSteps > sPaTiInfo.aRange.aXRod){
		
			tForReturn |= DC_SPMS_HOverLim;
			goto GT_Endone_fgfStartPTMove;
		}
		
		if(iVSteps > sPaTiInfo.aRange.aYRod){
			
			tForReturn |= DC_SPMS_VOverLim;
			goto GT_Endone_fgfStartPTMove;
		}
		
		theCount_1 = iHSteps - sPaTiInfo.aCurPosition.aXRod;
		if( theCount_1 < 0 ){
			iCmd |= DC_SSMM_HLeft;
			theCount_1 = -theCount_1;
		}
		if(theCount_1>0)
			iCmd |= DC_SSMM_HSet;
		iHSteps = theCount_1;
	
		theCount_1 = iVSteps - sPaTiInfo.aCurPosition.aYRod;
		if( theCount_1 < 0 ){
			iCmd |= DC_SSMM_VDown;
			theCount_1 = -theCount_1;
		}
		if(theCount_1>0)
			iCmd |= DC_SSMM_VSet;
		iVSteps = theCount_1;
	}
	
	
	
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_D03:
		case DC_Pro_F05:
			tStatus |= 0x80;
			goto GT_SetHorizon_Start;
			
		case DC_Pro_D11:
			goto GT_SetHorizon_Start;
			
		default:{ 
			tForReturn |= DC_SPMS_ProErr;
			goto GT_Endone_fgfStartPTMove;
		}
	}
GT_SetHorizon_Start:
	// printk("-------->1\r\n");
	if((iCmd&DC_SSMM_HSet)&&(iHSteps>0)){
		if(iCmd&DC_SSMM_HLeft){ // left
			if( sDirectFlag&DF_PTDF_Left ){
				goto GT_SetHorizon_Anti;
			}else{
				goto GT_SetHorizon_Nomal;
			}
		}else{					// Right
			if( sDirectFlag&DF_PTDF_Right ){
				goto GT_SetHorizon_Anti;
			}else{
				goto GT_SetHorizon_Nomal;
			}
		}
	}else{
		goto GT_SetVertical_Start;
	}
	
GT_SetHorizon_Nomal:
	// printk("-------->2\r\n");
	if( tStatus&0x80 ){
		if(sPaTiInfo.aCurPosition.aXRod >= sPaTiInfo.aRange.aXRod){
			tForReturn |= DC_SPMS_HOverLim;
			goto GT_SetVertical_Start;
		}else if((iHSteps+sPaTiInfo.aCurPosition.aXRod) > sPaTiInfo.aRange.aXRod){
			tForReturn |= DC_SPMS_HMoveWorning;
			sPaTiInfo.aStepPool.aXRod = iHSteps = sPaTiInfo.aRange.aXRod - sPaTiInfo.aCurPosition.aXRod;
		}else{
			sPaTiInfo.aStepPool.aXRod = iHSteps;
		}
	}else{
		sPaTiInfo.aStepPool.aXRod = iHSteps;
	}
	DF_PTS_ClrStatus(DC_PTS_HAntiRoll);
	goto GT_SetHorizon_End;
	
GT_SetHorizon_Anti:
	// printk("-------->3\r\n");
	if( tStatus&0x80 ){
		if(sPaTiInfo.aCurPosition.aXRod == 0){
			tForReturn |= DC_SPMS_HOverLim;
			goto GT_SetVertical_Start;
		}else if(iHSteps > sPaTiInfo.aCurPosition.aXRod){
			tForReturn |= DC_SPMS_HMoveWorning;
			sPaTiInfo.aStepPool.aXRod = iHSteps = sPaTiInfo.aCurPosition.aXRod;
		}else{
			sPaTiInfo.aStepPool.aXRod = iHSteps;
		}
	}else{
		sPaTiInfo.aStepPool.aXRod = iHSteps;
	}
	DF_PTS_SetStatus(DC_PTS_HAntiRoll);
	goto GT_SetHorizon_End;
	
GT_SetHorizon_End:
	// printk("-------->4\r\n");
	DF_PTS_SetStatus(DC_PTS_HRunning);
	DF_PTS_SetStatus(DC_PTS_HPeriodDone);
	tStatus |= 0x01;



GT_SetVertical_Start:
	// printk("-------->5\r\n");
	if((iCmd&DC_SSMM_VSet)&&(iVSteps>0)){
		if(iCmd&DC_SSMM_VDown){
			if( sDirectFlag&DF_PTDF_Down ){
				goto GT_SetVertical_Anti;
			}else{
				goto GT_SetVertical_Nomal;
			}
		}else{
			if( sDirectFlag&DF_PTDF_Up ){
				goto GT_SetVertical_Anti;
			}else{
				goto GT_SetVertical_Nomal;
			}
		}
	}else{
		goto GT_ExeCMD_fgfStartPTMove;
	}
	
GT_SetVertical_Nomal:
	// printk("-------->6\r\n");
	if( tStatus&0x80 ){
		if(sPaTiInfo.aCurPosition.aYRod == sPaTiInfo.aRange.aYRod){
			tForReturn |= DC_SPMS_VOverLim;
			goto GT_ExeCMD_fgfStartPTMove;
		}else if((iVSteps+sPaTiInfo.aCurPosition.aYRod)>sPaTiInfo.aRange.aYRod){
			tForReturn |= DC_SPMS_VMoveWorning;
			sPaTiInfo.aStepPool.aYRod = iVSteps = sPaTiInfo.aRange.aYRod - sPaTiInfo.aCurPosition.aYRod;
		}else{
			sPaTiInfo.aStepPool.aYRod = iVSteps;
		}
	}else{
		sPaTiInfo.aStepPool.aYRod = iVSteps;
	}
	DF_PTS_ClrStatus(DC_PTS_VAntiRoll);
	goto GT_SetVertical_End;
	
GT_SetVertical_Anti:
	// printk("-------->7\r\n");
	if( tStatus&0x80 ){
		if(sPaTiInfo.aCurPosition.aYRod == 0){
			tForReturn |= DC_SPMS_VOverLim;
			goto GT_ExeCMD_fgfStartPTMove;
		}else if(iVSteps > sPaTiInfo.aCurPosition.aYRod){
			tForReturn |= DC_SPMS_VMoveWorning;
			sPaTiInfo.aStepPool.aYRod = iVSteps = sPaTiInfo.aCurPosition.aYRod;
		}else{
			sPaTiInfo.aStepPool.aYRod = iVSteps;
		}
	}else{
		sPaTiInfo.aStepPool.aYRod = iVSteps;
	}
	DF_PTS_SetStatus(DC_PTS_VAntiRoll);
	goto GT_SetVertical_End;
	
GT_SetVertical_End:
	// printk("-------->8\r\n");
	DF_PTS_SetStatus(DC_PTS_VRunning);
	DF_PTS_SetStatus(DC_PTS_VPeriodDone);
	tStatus |= 0x04;
	
	
GT_ExeCMD_fgfStartPTMove:
	// printk("-------->9\r\n");
	if((tStatus&0x01)&&(tStatus&0x04)){
			sRaducingCount = 0;
			DF_PTS_ClrStatus(DC_PTS_HReducing)
			DF_PTS_ClrStatus(DC_PTS_VReducing)
			if(iHSteps>iVSteps){
				sRaducingRatio = (uint32)(iHSteps<<7)/iVSteps;
				DF_PTS_SetStatus(DC_PTS_VReducing);
			}else{
				sRaducingRatio = (uint32)(iVSteps<<7)/iHSteps;
				DF_PTS_SetStatus(DC_PTS_HReducing);
			}
	}else if( (!(tStatus&0x01))&&(!(tStatus&0x04)) ){
			tForReturn |= DC_SPMS_InputEmpty;
				goto GT_Endone_fgfStartPTMove;
	}
	{
		uint16 tMotoSpeed;
		if(iSpeed == 0){
			tMotoSpeed = DC_PanTilt_DefaultFreq;
		}else if(iSpeed < 101){
			tMotoSpeed = iSpeed*2+DC_PanTilt_BaseFreq;
		}else{
			tForReturn |= DC_SPMS_OverSpeed;
			DF_PTS_ClrStatus(DC_PTS_HRunning);
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			DF_PTS_ClrStatus(DC_PTS_VRunning);
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			goto GT_Endone_fgfStartPTMove;
		}
		
		
		fgConfigTimer3(tMotoSpeed,DC_Timer_PeriodMode|\
		DC_Timer_Intenable|DC_Timer_Timerpre_256|DC_Timer_Count_32bit);
		//
		DF_Timer3_Enable;
	}

GT_Endone_fgfStartPTMove:
	// printk("-------->10\r\n");
	return tForReturn;
}



static void sfPanTilt_ReportLocalPosition(void){
	
	if(DF_PTS_IsStatusExist(DC_PTS_HIniting)||DF_PTS_IsStatusExist(DC_PTS_VIniting)){
		return;
	}else{
		if(gfPanTilt_ReportCurrentPosition != NULL)
			gfPanTilt_ReportCurrentPosition(sPaTiInfo.aCurPosition.aXRod,sPaTiInfo.aCurPosition.aYRod);
		
		gfPE_UnregisterEvent(sfPanTilt_ReportLocalPosition,DC_PE_TYPE_UNINT);
	}
}


static void sfStepMotorDriver(void)
{
	uint8 theCurrentPhases;
	if(sMotorDelay != 0){
		sMotorDelay--;
		return ;
	}
	
	if(DF_PTS_IsStatusExist(DC_PTS_HRunning)){
		if(DF_PTS_IsStatusExist(DC_PTS_HReducing)){
			sRaducingCount+=1<<7;
			if(sRaducingCount>=sRaducingRatio){
				sRaducingCount = sRaducingCount-sRaducingRatio;
			}else{
				goto GT_DriveHStepMotorEnd;
			}
		}
		if(DF_PTS_IsStatusExist(DC_PTS_HPeriodDone)){
			
			//if((0<sPaTiInfo.aStepPool.aXRod)&&(sPaTiInfo.aStepPool.aXRod<=1024)){
			if( 0<sPaTiInfo.aStepPool.aXRod ){
				sPaTiInfo.aStepPool.aXRod--;
				
				if(DF_PTS_IsStatusExist(DC_PTS_HAntiRoll))
					sPaTiInfo.aCurPosition.aXRod--;
				else
					sPaTiInfo.aCurPosition.aXRod++;
				
			}else if(sPaTiInfo.aStepPool.aXRod==0){ // if(gHSMCap>=1024)//超出360度

				DF_PTS_ClrStatus(DC_PTS_HRunning);
				DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
				// It should have been checked yes or not seted up
				// but it dont effect the system normal running,
				// and for reducing unnecessary wasted.
				// i decide to ignore this step.
				DF_PTS_ClrStatus(DC_PTS_HIniting);
				
				if(DF_PTS_IsStatusExist(DC_PTS_HRspEnding)){
					// 反馈结束位
					//DF_PTS_SetStatus();
				}
				sfOutputAPhasesToHSM(8);
				
				if(!DF_PTS_IsStatusExist(DC_PTS_VRunning)){
					DF_Timer3_Disable;
					goto GT_DriveCk_sfStepMotorDriver;//JumpOutTheLoop
				}
				goto GT_DriveHStepMotorEnd;
			}
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
		}
		if(DF_PTS_IsStatusExist(DC_PTS_HAntiRoll)){
			if((sPanTiltPinNum>8)&&(sPaTiInfo.aCurPosition.aXRod<5)){
				if(!DF_PTS_GetHOriStatus){
					sPaTiInfo.aStepPool.aXRod = 0;
					sPaTiInfo.aCurPosition.aXRod = 0;
				}
			}
			theCurrentPhases = DF_PTS_GetHRhythm;
		}
		else{
			theCurrentPhases = 3-DF_PTS_GetHRhythm;
			if((sPanTiltPinNum>8)&&(sPaTiInfo.aCurPosition.aXRod>sPaTiInfo.aRange.aXRod-5)){
				if(!DF_PTS_GetHTilStatus){
					sPaTiInfo.aStepPool.aXRod = 0;
					sPaTiInfo.aCurPosition.aXRod = sPaTiInfo.aRange.aXRod;
				}
			}
		}
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
GT_DriveHStepMotorEnd:

	if(DF_PTS_IsStatusExist(DC_PTS_VRunning)){
		if(DF_PTS_IsStatusExist(DC_PTS_VReducing)){
			sRaducingCount += 1<<7 ;
			if(sRaducingCount>=sRaducingRatio){
				sRaducingCount = sRaducingCount-sRaducingRatio;
			}else{
				goto GT_END_sfStepMotorDriver;
			}
		}
		if(DF_PTS_IsStatusExist(DC_PTS_VPeriodDone)){
			
			if((0<sPaTiInfo.aStepPool.aYRod)&&(sPaTiInfo.aStepPool.aYRod<=1024)){
				sPaTiInfo.aStepPool.aYRod--;
				if(DF_PTS_IsStatusExist(DC_PTS_VAntiRoll))
					sPaTiInfo.aCurPosition.aYRod--;
				else
					sPaTiInfo.aCurPosition.aYRod++;
				
			}else if(sPaTiInfo.aStepPool.aYRod==0){
				DF_PTS_ClrStatus(DC_PTS_VRunning);
				DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
				// 
				// 
				// ditto!
				// 
				DF_PTS_ClrStatus(DC_PTS_VIniting);
				
				
				if(DF_PTS_IsStatusExist(DC_PTS_VRspEnding)){
					// 反馈结束位
					//DF_PTS_SetStatus();
				}
				sfOutputAPhasesToVSM(8);
				
				if(!DF_PTS_IsStatusExist(DC_PTS_HRunning)){
					DF_Timer3_Disable;
					goto GT_DriveCk_sfStepMotorDriver;//JumpOutTheLoop
				}
				goto GT_END_sfStepMotorDriver;
			}
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
		}
		if(DF_PTS_IsStatusExist(DC_PTS_VAntiRoll)){
			if((sPanTiltPinNum>8)&&(sPaTiInfo.aCurPosition.aYRod<5)){
				if(!DF_PTS_GetVTilStatus){
					sPaTiInfo.aStepPool.aYRod = 0;
					sPaTiInfo.aCurPosition.aYRod = 0;
				}
			}
			theCurrentPhases = 3 - DF_PTS_GetVRhythm;
		}else{
			if((sPanTiltPinNum>8)&&(sPaTiInfo.aCurPosition.aYRod>sPaTiInfo.aRange.aYRod-5)){
				if(!DF_PTS_GetVOriStatus){
					sPaTiInfo.aStepPool.aYRod = 0;
					sPaTiInfo.aCurPosition.aYRod = sPaTiInfo.aRange.aYRod;
				}
			}
			theCurrentPhases = DF_PTS_GetVRhythm;
		}
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_PTS_VaddRhythm;
	}
	
GT_END_sfStepMotorDriver:
return ;

GT_DriveCk_sfStepMotorDriver:
	if(DF_PTS_IsStatusExist(DC_PTS_BuffFull)){
		DF_PTS_ClrStatus(DC_PTS_BuffFull);
		gfStartPTMove(	sPTZActBuf.aHComponent,\
						sPTZActBuf.aVComponent,\
						sPTZActBuf.aHSpeed,\
						sPTZActBuf.aHCommand);
	}	

}


static void sfDriveToFindOutCoordinate(void)
{
	uint8  theCurrentPhases;
	/// Test the horizontal Rotation Origin
	if(DF_PTS_IsStatusExist(DC_PTS_HRunning)){
		if(DF_PTS_IsStatusExist(DC_PTS_HPeriodDone)){
			sPaTiInfo.aStepPool.aXRod++;
			
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			if(!DF_PTS_GetHOriStatus){
				DF_PTS_ClrStatus(DC_PTS_HRunning);
				if(!DF_PTS_IsStatusExist(DC_PTS_VRunning))
					goto GT_DriveToFindOutCoordinate;
			}
		}
		theCurrentPhases = DF_PTS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
	/// Test the vertical Rotation Origin
	if(DF_PTS_IsStatusExist(DC_PTS_VRunning)){
		if(DF_PTS_IsStatusExist(DC_PTS_VPeriodDone)){
			sPaTiInfo.aStepPool.aYRod++;
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			if(!DF_PTS_GetVTilStatus){
				DF_PTS_ClrStatus(DC_PTS_VRunning);
				if(!DF_PTS_IsStatusExist(DC_PTS_HRunning))
					goto GT_DriveToFindOutCoordinate;
			}
		}
		theCurrentPhases = 3 - DF_PTS_GetVRhythm;
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_PTS_VaddRhythm;
	}
	return;
	
GT_DriveToFindOutCoordinate:
	{
		spfTimer0ServerFunction = sfDriveToFindOutMaxRange;
		
		sPaTiInfo.aStepPool.aXRod--;
		sPaTiInfo.aStepPool.aYRod--;
		
		gfPE_RegisterEvent(sfPanTilt_ReportLocalPosition,DC_PanTilt_InitCheckFreq,DC_PE_TYPE_UNINT);
		
		sPaTiInfo.aRange.aXRod = 0;
		sPaTiInfo.aRange.aYRod = 0;
		DF_PTS_SetStatus(DC_PTS_VRunning);
		DF_PTS_SetStatus(DC_PTS_VPeriodDone);
		
		DF_PTS_SetStatus(DC_PTS_HRunning);
		DF_PTS_SetStatus(DC_PTS_HPeriodDone);

	}
	return;
}


static void sfDriveToFindOutMaxRange(void)
{
	uint8  theCurrentPhases;
	/// Test the horizontal Rotation range
	if(DF_PTS_IsStatusExist(DC_PTS_HRunning)){
		if(DF_PTS_IsStatusExist(DC_PTS_HPeriodDone)){
			sPaTiInfo.aRange.aXRod++;
			
			DF_PTS_ClrStatus(DC_PTS_HPeriodDone);
			if(!DF_PTS_GetHTilStatus){
				DF_PTS_ClrStatus(DC_PTS_HRunning);
				if(!DF_PTS_IsStatusExist(DC_PTS_VRunning))
					goto GT_DriveToFindOutCoordinate;
			}
		}
		theCurrentPhases = 3 - DF_PTS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_PTS_HaddRhythm;
	}
	/// Test the vertical Rotation range
	if(DF_PTS_IsStatusExist(DC_PTS_VRunning)){
		if(DF_PTS_IsStatusExist(DC_PTS_VPeriodDone)){
			sPaTiInfo.aRange.aYRod++;
			
			DF_PTS_ClrStatus(DC_PTS_VPeriodDone);
			if(!DF_PTS_GetVOriStatus){
				DF_PTS_ClrStatus(DC_PTS_VRunning);
				if(!DF_PTS_IsStatusExist(DC_PTS_HRunning))
					goto GT_DriveToFindOutCoordinate;
			}
		}
		theCurrentPhases = DF_PTS_GetVRhythm;
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_PTS_VaddRhythm;
	}
	return;
	
GT_DriveToFindOutCoordinate:
	{
		uint32 tret;
		uint8 theSetCmd = 0;
		
		DF_PTS_InitStatus;
		spfTimer0ServerFunction = sfStepMotorDriver;
		
		sPaTiInfo.aCurPosition.aXRod = sPaTiInfo.aRange.aXRod;
		sPaTiInfo.aCurPosition.aYRod = sPaTiInfo.aRange.aYRod;
		
		
		
		printk("sPaTiInfo.aRange.aXRod:%d\r\n",(int)sPaTiInfo.aRange.aXRod);
		printk("sPaTiInfo.aRange.aYRod:%d\r\n",(int)sPaTiInfo.aRange.aYRod);
		// printf("sPaTiInfo.aCurPosition.aXRod");
		// printf("sPaTiInfo.aCurPosition.aYRod");
		printk("sPaTiInfo.aStepPool.aXRod:%d\r\n",(int)sPaTiInfo.aStepPool.aXRod);
		printk("sPaTiInfo.aStepPool.aYRod:%d\r\n",(int)sPaTiInfo.aStepPool.aYRod);
		if( sPaTiInfo.aRange.aXRod > sPaTiInfo.aStepPool.aXRod ){
			
			sPaTiInfo.aStepPool.aXRod = sPaTiInfo.aRange.aXRod - sPaTiInfo.aStepPool.aXRod;
			theSetCmd |= DC_SSMM_HSet|DC_SSMM_HLeft;
			
			DF_PTS_SetStatus(DC_PTS_HIniting);
			
		}else{
			sPaTiInfo.aStepPool.aXRod = 0;
		}
		
		if( sPaTiInfo.aRange.aYRod > sPaTiInfo.aStepPool.aYRod ){
			sPaTiInfo.aStepPool.aYRod = sPaTiInfo.aRange.aYRod - sPaTiInfo.aStepPool.aYRod;
			theSetCmd |= DC_SSMM_VSet|DC_SSMM_VDown ;
			DF_PTS_SetStatus(DC_PTS_VIniting);
		}else{
			sPaTiInfo.aStepPool.aYRod = 0;
		}
		
		theSetCmd |= DC_SSMM_Steps;
		
		tret =  gfStartPTMove(sPaTiInfo.aStepPool.aXRod,sPaTiInfo.aStepPool.aYRod,75,theSetCmd);
		
	}
	return;
}





static void sfOutputAPhasesToHSM(uint8 _cmd)
{
	if(0x01&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[0].aGroup,spPanTiltPin[0].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[0].aGroup,spPanTiltPin[0].aBit);
	}
	if(0x02&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[1].aGroup,spPanTiltPin[1].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[1].aGroup,spPanTiltPin[1].aBit);
	}
	if(0x04&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[2].aGroup,spPanTiltPin[2].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[2].aGroup,spPanTiltPin[2].aBit);
	}
	if(0x08&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[3].aGroup,spPanTiltPin[3].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[3].aGroup,spPanTiltPin[3].aBit);
	}
}

static void sfOutputAPhasesToVSM(uint8 _cmd)
{
	
	if(0x01&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[4].aGroup,spPanTiltPin[4].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[4].aGroup,spPanTiltPin[4].aBit);
	}
	if(0x02&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[5].aGroup,spPanTiltPin[5].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[5].aGroup,spPanTiltPin[5].aBit);
	}
	if(0x04&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[6].aGroup,spPanTiltPin[6].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[6].aGroup,spPanTiltPin[6].aBit);
	}
	if(0x08&sStepMotorPhases[_cmd]){
		DF_Set_GPIO_Outx(spPanTiltPin[7].aGroup,spPanTiltPin[7].aBit);
	}else{
		DF_Clr_GPIO_Outx(spPanTiltPin[7].aGroup,spPanTiltPin[7].aBit);
	}
}


uint32 tCountHaha=0;
static irqreturn_t sfPTZHandle(int irq, void *id){
	
	tCountHaha++;
	
	if(DF_Timer3_IntFlag){
		if( spfTimer0ServerFunction != NULL )
			spfTimer0ServerFunction();
		DF_Timer3_ClrIntFlag;	
	}
	return IRQ_HANDLED;
}


