#ifndef __CTA_H
#define __CTA_H

//#include "../HAL/USERHAL.h"
#include "../HAL/HAL.h"

// ================================================================================
// ------------------------------------------------------------Audio Plug
#define DC_AuPl_Speaker_On				0x00010000
#define DC_AuPl_Speaker_Off				0x00020000
#define DC_AuPl_Microph_On				0x00040000
#define DC_AuPl_Microph_Off				0x00080000
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afSetStatus)(uint32);
#define DC_AuPl_Speaker     0x01
#define DC_AuPl_Microph     0x02
	int32(*afGetStatus)(uint8);
	uint8 OnStatus;
} mClass_AudioPlug;

// ------------------------------------------------------------Night Light
typedef struct {
	uint8 aStatus;
	uint8 aLevel;
	uint8 aRemain[2];
} mClass_NiLiIfo;

#define DC_NiLi_SuppLevel		0x04
#define DC_NiLi_StaMask			0x03
#define DC_NiLi_On				0x01
#define DC_NiLi_Off				0x02
typedef struct {
	mGPIOPinArr *apPinArr;
	mClass_NiLiIfo *aIfo;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetStatus)(mClass_NiLiIfo *);
	void (*afGetStatus)(void);
	uint8 OnStatus;
} mClass_NightLi;

// ------------------------------------------------------------Infrared Light
#define DC_IfLi_On				0x00010000
#define DC_IfLi_Off				0x00020000
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetStatus)(uint32);
	int32(*afGetStatus)(void);
	uint8 OnStatus;
} mClass_IfrLi;

// ------------------------------------------------------------Status Light
#define DC_StaLi_Mod_DEFAULT              0
#define DC_StaLi_Mod_RED                  1
#define DC_StaLi_Mod_GREEN                2
#define DC_StaLi_Mod_BLUE                 3
#define DC_StaLi_Mod_BREATH               4
#define DC_StaLi_Mod_RACING               5
typedef struct {
	uint32 aPTime;		// positive time
	uint32 aNTime;		// negative time
	uint8 aUnit;
	uint32 aMode;
} mStaLi_SetParam;

typedef struct {
	uint16 *apArr;
	uint16 aSize;
	uint8 aNum;
} mStaLiBrth_Ifo;

typedef struct {
	uint16 aOnDgr;
	uint16 aOffDgr;
	uint16 aBrthNum;
	uint16 *apBrthDgr;
	uint8 aUnit;
} mStaLi_InitParrm;

typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	void (*afInitParam)(mStaLi_InitParrm *);
	int32(*afUninit)(void);
	void (*afSetStatus)(mStaLi_SetParam *);

	// default set :1 ( on-->high off-->low )
	uint8 OnStatus;
} mClass_StateLi;

// ------------------------------------------------------------Double Lens
#define DC_DuLens_DayLens		0x00010000
#define DC_DuLens_NightLens		0x00020000
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetStatus)(uint32);
	int32(*afGetStatus)(void);
} mClass_DubLens;

// ------------------------------------------------------------Infrared Filter
#define DC_IfrFlt_TypeA			0x00000001
#define DC_IfrFlt_TypeB			0x00000002
#define DC_IfrFlt_TypeC			0x00000003
#define DC_IfrFlt_TypeD			0x00000004
#define DC_IfrFlt_PassLi		0x00001000
#define DC_IfrFlt_BlockLi		0x00002000
#define DC_IfrFlt_NoSuppGet		0x10000000
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afRegCallBack)(void (*Handler)(uint32));
	void (*afSetStatus)(uint32);
	int32(*afGetStatus)(void);
	uint8 aType;
#define DC_IfrFltOpt_InTypeA		0x01
#define DC_IfrFltOpt_TypeB		0x02
#define DC_IfrFltOpt_TypeC		0x03
#define DC_IfrFltOpt_TypeD		0x04
	uint8 aStatusChgType;
#define	DC_NVIfrFilt_Auto               0x00
#define	DC_NVIfrFilt_Manual             0x01
} mClass_IfrFilter;

// ------------------------------------------------------------Button monitor
#define DC_BuMoSta_HIGHG		0x00000100
#define DC_BuMoSta_LOW			0x00000200
#define DC_BuMoSta_RAISE		0x00000400
#define DC_BuMoSta_FALL			0x00000800
typedef struct {
	uint32 aStatus;
	uint8 aUnit;
	// uint8        aEventType;
} mButton_IntBackParam;
typedef struct {
#define DC_BuMoDef_High			0x01
#define DC_BuMoDef_Low			0x02
	uint8 adefLEVE;
} mButton_DEFAULT;

typedef struct {
	mButton_DEFAULT *apDefs;
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afGetStatus)(mButton_IntBackParam *);
	void (*afRegCallBack)(void (*Handler)(mButton_IntBackParam *));
} mClass_Button;

// ------------------------------------------------------------Cool Fan
typedef struct {
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afCoolFanProcessing)(uint32);
} mClass_CoolFan;

// ------------------------------------------------------------Pan Tilt
// aRegCf
#define DF_PTSP_RcfExport(_v)   	((_v)&0x0000FFFF)
// -------------- internal & external
// 是否支持水平方向运动
#define DC_PTSP_Supp_HMove		0x00000001
// 是否支持垂直方向运动
#define DC_PTSP_Supp_VMove		0x00000002
// 是否支持水平垂直层叠运动
#define DC_PTSP_Supp_HVMove		0x00000004
// 是否支持水平方向自动扫描
#define DC_PTSP_Supp_HScan		0x00000008
// 是否支持垂直方向自动扫描
#define DC_PTSP_Supp_VScan		0x00000010
// 是否支持水平方向限位
#define DC_PTSP_Supp_HLim		0x00000020
// 是否支持垂直方向限位
#define DC_PTSP_Supp_VLim		0x00000040
// 是否支持0位检测
#define DC_PTSP_Supp_OriPosition	0x00000080
// 是否支持获取云台当前位置
#define DC_PTSP_Supp_CurPos		0x00000100
// 是否支持预置位
#define DC_PTSP_Supp_PRESET		0x00000800
// 是否支持预置位巡航
#define DC_PTSP_Supp_CRUISE		0x00001000
// -------------- internal
// Pin status is equal to step motor status by the way of current amplify.
#define DC_PTSP_Positive		0x80000000
// Pin status is inequal to step motor status by the way of current amplify.
#define DC_PTSP_Negative		0x40000000
// 是否支持定位
#define DC_PTSP_Locate			0x20000000
// 是否要需要突变滞留
#define DC_PTSP_Loitor			0x10000000
// The variable aims to identify the relationship of direction
// between the PTZ and the stepper motor .
// like:        Step motor anticlockwise running corresponding the The direction Right
//                      you need set the First bit in this variable
//          sDirectFlag |= DF_PTDF_Right;
// 水平方向云台的逆时针方向和以正序驱动步进电机的方向相同
#define DC_PTSP_LeftIsA 		0x01000000
#define DC_PTSP_RightIsA		0x02000000
// 垂直向上的方向和正序驱动步进电机的方向相同
#define DC_PTSP_DownIsA			0x04000000
#define DC_PTSP_UpIsA			0x08000000
// 定位开关
#define DC_PTSP_LimMask			0x00F00000
// 碰撞归零，限位开关
#define DC_PTSP_HHeadLim		0x00800000
#define DC_PTSP_HTailLim		0x00400000
#define DC_PTSP_VHeadLim		0x00200000
#define DC_PTSP_VTailLim		0x00100000

// aStatus
#define DF_PTSP_StaExport(_v)		((_v)&0x0000000F)
//

#define DC_PTSP_InitDone		0x00000001
#define DC_PTSP_Busy			0x00000002
#define DC_PTSP_Full			0x00000004
typedef struct {
	uint32 aRegCf;
	uint32 aStatus;
	uint32 aHDgrPerStep;	//精度 0.000001
	uint32 aVDgrPerStep;
	uint16 aHStepsPerFull;
	uint16 aVStepsPerFull;
	uint16 aHLimitSteps;
	uint16 aVLimitSteps;
	uint16 aHCurPos;
	uint16 aYCurPos;
	uint16 aXRange;
	uint16 aYRange;
} mPanTiltInfo;

typedef struct {
	uint32 aCommand;
	union {
		uint16 aHComponent;
		uint16 aPresetNum;
	};
	uint16 aVComponent;
	uint16 aSpeed;
} mPTZAction;

typedef struct {
	mGPIOPinArr *apDrvPinArr;
	mGPIOPinArr *apLimPinArr;
	mPanTiltInfo *apDefInfo;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afUpdatePTZInfo)(void);
	// ------------------ Control Definition xWight yWight Speed   Cmd
#define DC_PTZCmd_Stop				0x00000001	// stop the Pan and tilt
#define DC_PTZCmd_Dgr				0x00000002	// input unit type is Angel
#define DC_PTZCmd_Crd				0x00000004	// input unit type is absolute coordinate
#define DC_PTZCmd_Steps				0x00000008	// input unit type is steps

#define DC_PTZCmd_DirMask			0x000000F0
#define DC_PTZCmd_Right				0x00000010	// set the horizon component
#define DC_PTZCmd_Left				0x00000000	// set the vertical component
	// #define DC_PTZCmd_Left                               0x00000020  // set the vertical component
#define DC_PTZCmd_Up				0x00000040	// set the horizon direction
#define DC_PTZCmd_Down				0x00000000	// set the vertical direction
	// #define DC_PTZCmd_Down                               0x00000080  // set the vertical direction
#define DC_PTZCmd_Preset			        0x00000200	// Start move to preset point
#define DC_PTZCmd_NeedLoitor			0x00000100
	// 命令执行状态
#define DC_PTZCmd_Ok				0x40000000
#define DC_PTZCmd_Err				0x80000000	// Error
	// ------------------ return Definition
#define DC_PTZRet_OK				0
#define DC_PTZRet_Busy				0x00000001
#define DC_PTZRet_InputEmpty			0x00000002
#define DC_PTZRet_InputErr				0x00000800
#define DC_PTZRet_HOverLim				0x00000004
#define DC_PTZRet_VOverLim			0x00000008
#define DC_PTZRet_HMoveWorning			0x00000010
#define DC_PTZRet_VMoveWorning			0x00000020
#define DC_PTZRet_ProErr			0x00000040
#define DC_PTZRet_ChipErr			0x00000080
#define DC_PTZRet_OverSpeed			0x00000100
#define DC_PTZRet_BufIdle			0x00000200
#define DC_PTZRet_BufFull			0x00000400
	int32(*gfPTZStart)(mPTZAction);
	int32(*gfPTZStop)(void);
	int32(*gfSetPreSet)(void *);
	int32(*gfClrPreSet)(void *);
	int32(*gfInPSList)(void *);	//Inport Preset List
	int32(*gfInPSlcusList)(void *);	//Inport Preset Move Locus
	int32(*gfStartCuris)(void);
	void (*afRegInitCallback)(void (*iHandle)(void));
} mClass_PTZ;

// ------------------------------------------------------------LDR
#define DC_LDR_DAY				0x01
#define DC_LDR_Night				0x00
#define DC_LDR_ShiftD_Value			120
#define DC_LDR_ShiftN_Value			20
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afGetStatus)(void);
	int32(*afRegCallBack)(void (*Handler)(uint8));
	// return sensitive
	int32(*afSetSensitive)(uint8, uint8);
	int32(*afGetSensitive)(uint8 *, uint8 *);
	int32(*afSetDNLimit)(uint16, uint16);
} mClass_LDR;

// ------------------------------------------------------------Temperature
typedef struct {
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetReportTime)(uint32);
	int32(*afGetTemperature)(void);
	int32(*afRegCallBack)(void (*CallbackHandle)(int32));
} mClass_Temp;

// ------------------------------------------------------------Humidity
typedef struct {
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetReportTime)(uint32);
	int32(*afGetHumidity)(void);
} mClass_Humidity;

// ------------------------------------------------------------Pan Tilt
typedef struct {
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afGetTime)(mTime_Struct *);
	void (*afSetTime)(mTime_Struct *);
} mClass_RTC;

// ------------------------------------------------------------PIR
#define DC_DefPIR_OriHIGH			0x01	// 正常状态下原始的电平为高
#define DC_DefPIR_OriLOW			0x00	// 正常状态下原始的电平为低
typedef struct {
	uint32 aCount;
	uint32 aDeadTime;
	uint8 aConf;
} mPIR_Info;

#define DC_PIR_Discover_Normal			0x00
#define DC_PIR_Discover_Unnormal		0x01
typedef struct {
	mPIR_Info *apIfoArr;
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afGetStatus)(void);
	void (*afSetDeadTime)(uint32);
	void (*afRegCallBack)(void (*Handle)(uint32));
} mClass_PIR;

// ------------------------------------------------------------Door Bell
#define DC_CTADoorBell_TapBell			0x01
#define DC_DoorBell_Low				0x00
#define DC_DoorBell_High			0x01
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetStatus)(uint32);
	int32(*afGetStatus)(void);
} mClass_DoorBell;

// ------------------------------------------------------------Door Lock
#define DC_DoorLock_Close    			0x00
#define DC_DoorLock_Open     			0x01
typedef struct {
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	void (*afSetStatus)(uint32);
	int32(*afGetStatus)(void);
} mClass_DoorLock;

// ------------------------------------------------------------Hardware AEC
typedef struct {
	uint8 *u8Data;		// the data which want to write
	uint32 u32DataLength;	// data length
} mFM1288_WriteData;

typedef struct {
	uint8 u8RegHigh;	// register address high byte
	uint8 u8RegLow;		// register address low byte
} mFM1288_ReadData;

typedef struct {
	uint8 *apData;
	uint32 apDataLen;
} mFM1288_InitData;

typedef struct {
	mGPIOPinArr *apPinArr;
	mFM1288_InitData *apInitData;
	int32(*afInit)(void);
	int32(*afUninit)(void);
	int32(*afWriteData)(mFM1288_WriteData *);
	int32(*afReadData)(mFM1288_ReadData *);
} mClass_FM1288;

// ------------------------------------------------------------EAS
typedef struct {
	uint32 aDefSta;
	mGPIOPinArr *apPinArr;
	int32(*afInit)(void);
	int32(*afUninit)(void);
#define DC_EASSta_Safty         	0x00000000
#define DC_EASSta_Emergency     	0x00000001
	int32(*afGetStatus)(void);
	void (*afRegCallBack)(void (*Handler)(void));
} mClass_EAS;

#define DC_CTAMethodNum			19
typedef struct {
	mClass_Button *apButton;
	mClass_LDR *apLDR;
	mClass_IfrFilter *apIfrFilter;
	mClass_IfrLi *apIfrLi;
	mClass_DubLens *apDubLens;
	mClass_StateLi *apStateLi;
	mClass_PTZ *apPTZ;
	mClass_NightLi *apNightLi;
	mClass_CoolFan *apCoolF;
	mClass_AudioPlug *apAudioPlug;
	mClass_Temp *apTemp;
	mClass_Humidity *apHumidity;
	void *apIOReset;
	mClass_RTC *apRTC;
	mClass_PIR *apPIR;
	mClass_DoorBell *apDoorBell;
	mClass_DoorLock *apDoorLock;
	mClass_FM1288 *apFM1288;
	mClass_EAS *apEAS;
} mClass_CTA;

extern mClass_AudioPlug gClassAudioPlug;
extern mClass_IfrLi gClassIfrLi;
extern mClass_StateLi gClassStateLi;
extern mClass_IfrFilter gClassIfrFilt;
extern mClass_Button gClassButton;
extern mClass_LDR gClassLDR;
extern mClass_NightLi gClassNiLi;
extern mClass_DubLens gClassDouobLens;
extern mClass_Temp gClassTempMonitor;
extern mClass_PTZ gClassPTZ;
extern mClass_RTC gClassRTC;
extern mClass_PIR gClassPIR;
extern mClass_DoorBell gClassDoorBell;
extern mClass_DoorLock gClassDoorLock;
extern mClass_FM1288 gClassFM1288;
extern mClass_EAS gClassEAS;

extern mClass_CTA *gSysCTA;

#endif
