#ifndef __APP_DEF_H
#define __APP_DEF_H

#include "../GlobalParameter.h"

// extern uint8 gfStr_itoa(int i,void *iDes);
extern uint8 gfStr_itoa(uint32 iVal, uint8 *tTar, uint8 iPro, uint8 tTLen);
extern uint16 gClrLineNum;

typedef struct {
	uint8 aNum;
	uint8 *aCapName;
	int (*afModFun)(void);
} mCapModule;

// *****************************************************************************
// LDR
typedef struct {
	uint8 aStatus;
	// uint8   aSensitive;
	uint8 aSPoint;
	uint8 aSDomain;
	uint16 aJudgeValue;
} mSta_LDR;
// IfrLi
typedef struct {
	uint8 aStatus;
} mSta_IfL;
// IfrFilter
typedef struct {
	uint8 aStatus;
	uint8 aType;
} mSta_IfF;
// Double Lens
typedef struct {
	uint8 aStatus;
#define DCA_DbL_DayLens     0x00
#define DCA_DbL_NightLens   0x01
} mSta_DbL;
// State Light
typedef struct {
	uint8 aStatus;
} mSta_StL;
// Night Light
typedef struct {
	uint8 aStatus;
	uint8 aLevel;
} mSta_NiL;
// Audio Plug
typedef struct {
	uint8 aStatus;
#define DCA_AuP_Speaker_On          0x01
#define DCA_AuP_Speaker_Off         0x00
#define DCA_AuP_Microphone_On       0x02
#define DCA_AuP_Microphone_Off      0x00
#define DCA_AuP_Speaker_NoSupp      0x04
#define DCA_AuP_Microphone_NoSupp   0x08
} mSta_AuP;
//  Button
typedef struct {
	uint8 aStatus;
	uint16 aTimes;
#define DCA_But_Up                  0x00
#define DCA_But_Down                0x01
} mSta_But;
//  Pan&Tilt
typedef struct {
	uint32 aInfo;
	uint32 aXMinDgr;
	uint32 aYMinDgr;
	uint16 aXRange;
	uint16 aYRange;
	uint16 aXPos;
	uint16 aYPos;
} mSta_PTZ;
// Temperature Humidity
typedef struct {
	int16 aHumi;
	int16 aTemp;
} mSta_Env;
// RTC
typedef struct {
	uint8 aScd;
	uint8 aMnu;
	uint8 aHur;
	uint8 aDay;
	uint8 aWkD;
	uint8 aMth;
	uint8 aYea;
} mSta_RTC;
//
typedef struct {
#define DCA_StaG_Cap_UpD    0x01
#define DCA_StaG_Dvc_UpD    0x02
	uint32 aStatus;

	mSta_But aBut[2];
	mSta_LDR aLDR;
	union {
		mSta_IfF aIfF;
		mSta_DbL aDbL;
	};
	mSta_IfL aIfL;
	// mSta_StL        aStL;
	mSta_PTZ aPTZ;
	mSta_NiL aNiL;
	mSta_AuP aAuP;
	mSta_Env aEnv;
	mSta_RTC aRTC;
} mSta_G;
extern mSta_G gStaG;

// *****************************************************************************
#define DCA_Driver_Path "/dev/NV_Driver"
#define DCA_DriverLog1_Path     "/tmp/NVDrive/1.log"
#define DCA_DriverLog2_Path     "/tmp/NVDrive/2.log"

/*
1. BUTTON
2. LDR
3. IRCUT
4. Infrared Light
5. Double Lens
6. State Light
7. PTZ
8. Night Light
9. Audio Plug
10.Temp Monitor
11.Humidity Monitor
13.GPIO Reset
14.RTC
*/
#define DCA_CMD_HELP        "help"
#define DCA_CMD_BUTTON      "BUTTON"
#define DCA_CMD_LDR         "LDR"
#define DCA_CMD_IRCUT       "IRCUT"
#define DCA_CMD_IfrLi       "IfrLi"
#define DCA_CMD_DoubLens    "DoubLens"
#define DCA_CMD_StaLi       "StaLi"
#define DCA_CMD_PTZ         "PTZ"
#define DCA_CMD_NiLi        "NiLi"
#define DCA_CMD_AuPl        "AuPl"
#define DCA_CMD_Temp        "Temp"
#define DCA_CMD_Humi        "Humi"
#define DCA_CMD_GpioRst     "GpioRst"
#define DCA_CMD_RTC         "RTC"

#define DCA_FILE_README     "vi ./README"

#define DCA_ERR_CMD         "Please input right command\r\n"

extern uint32 gShowFrameControler;
#define DF_DCA_FrameUpdate_IncOnce    gShowFrameControler++
#define DF_DCA_FrameUpdate_Stop       gShowFrameControler=0
#define DF_DCA_FrameUpdate_RUN        gShowFrameControler=0xFFFFFF00
#define DF_DCA_FrameUpdate_SetOnce    gShowFrameControler=1

extern uint32 gMsgCentStatus;
#define  DCA_MsgCent_MOD_SHOW 0x01

#endif
