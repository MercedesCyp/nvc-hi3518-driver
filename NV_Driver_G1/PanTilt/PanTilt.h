#ifndef __PanTilt_H
#define __PanTilt_H

#include "../GlobalParameter.h"
#include "../MsgCenter.h"

typedef struct{
	uint16	aXRod;
	uint16	aYRod;
}mCoodinate;

typedef struct{
	mCoodinate 	aRange;			// 可扫描范围
	mCoodinate 	aCurPosition;	// 当前位置
	mCoodinate 	aStepPool;		// 缓冲池
}mPaTiInfo;


typedef struct{
	uint16 aHComponent;
	uint16 aVComponent;
	uint16 aHSpeed;
	uint32 aHCommand;
}mPTZAction;


extern void (*gfPanTilt_ReportCurrentPosition)(uint16 iXRod,uint16 iYRod);
extern void (*gfPanTilt_ReportState)(void);


extern int32 gfInitDrv_PanTilt(uint8 iRemain);
extern int32 gfUninitDrv_PanTilt(uint8 iRemain);

extern int32 gfGetPanTiltCurPos(mNVC_PANTILT_Respons *iInfo);
extern int32 gfGetPanTiltInfo(mNVC_PANTILT_INFO *iInfo);

/*
Horizon
	default Right;
Vertical
	defalut Up;
*/
extern uint32 gfStartPTMove(uint16 iHSteps, uint16 iVSteps,uint16 iSpeed,uint32 iCmd);
#define DC_SSMM_Stop			0x00000001  // stop the Pan and tilt
#define DC_SSMM_HSet			0x00000002	// set the horizon component
#define DC_SSMM_VSet			0x00000004  // set the vertical component
#define DC_SSMM_HLeft			0x00000008  // set the horizon direction
#define DC_SSMM_VDown			0x00000010  // set the vertical direction
// #define DC_SSMM_HAntiClockWise	0x00000008  // set the horizon direction
// #define DC_SSMM_VAntiClockWise	0x00000010  // set the vertical direction
#define DC_SSMM_Dgr				0x00000020  // input unit type is Angel
#define DC_SSMM_Crd				0x00000040  // input unit type is absolute coordinate
#define DC_SSMM_Steps			0x00000080	// input unit type is steps


#define DC_SPMS_Success				0
#define DC_SPMS_Busy				0x00000001
#define DC_SPMS_InputEmpty			0x00000002
#define DC_SPMS_HOverLim			0x00000004
#define DC_SPMS_VOverLim			0x00000008
#define DC_SPMS_HMoveWorning		0x00000010
#define DC_SPMS_VMoveWorning		0x00000020
#define DC_SPMS_ProErr				0x00000040
#define DC_SPMS_ChipErr				0x00000080
#define DC_SPMS_OverSpeed			0x00000100
#define DC_SPMS_BufIdle				0x00000200
// #define DC_SPMS_HMoveLim			0x00000020
// #define DC_SPMS_VMoveLim			0x00000080

#endif