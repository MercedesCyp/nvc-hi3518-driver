#include <stdio.h>
#include <string.h>
#include "../GlobalParameter.h"
#include "app_def.h"
#include "../Tool/MsgProtocal.h"

static void sfShow_UnionCapString(uint8 *iTar,
                                  uint8 *iStr1, uint8 *iStr2, uint8 iNum1st);
static void sfShow_UnionCmdString(mCapModule *iMod[]);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
extern mNVC_DRV_CAP_INFO sDrvCap;
//
static uint8 const sShow_StdStr_L1[] =
    "***************************************************";
static uint8 const sShow_StdStr_L2[] =
    "| NUM | CMD              | NUM | CMD              |";
static uint8 const sShow_StdStr_L3[] =
    "---------------------------------------------------";
static uint8 const sShow_StdStr_L4[] = "Please select you want test event!";
static uint8 const sShow_StdStr_L5[] =
    "tips: Input `help` for more information";
static uint8 const sShow_StdStr_L6[] =
    "tips: Input `exit` for quit this operation.";
static uint8 const sShow_StdStr_L7[] =
    "tips: Input `show` for detect all modules.";
static uint8 const sShow_StdStr_Sta[] = "Status      :";
static uint8 const sShow_StdStr_ON[] = "ON";
static uint8 const sShow_StdStr_OFF[] = "OFF";
static uint8 const sShow_StdStr_FLASH[] = "FLASH";
static uint8 const sShow_StdStr_DAY[] = "Day";
static uint8 const sShow_StdStr_NIG[] = "Night";
static uint8 const sShow_StdStr_noSUPP[] = "NOT SUPPORTED TO THIS DIVICE";
static uint8 const sShow_StdStr_wAuTo[] =
    "Status:(AutoDetect 1 times/1s, Press `Enter` to stop!)";
static uint8 const sShow_StdStr_Input[] = "you input: ";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_Button_Head[] =
    "|                Button & Monitor                 |";
static uint8 const sShow_StdStr_Button[] = "Button";
static uint8 const sShow_StdStr_Button_Interval[] = "Interval    :";
static uint8 const sShow_StdStr_Button_Up[] = "Up";
static uint8 const sShow_StdStr_Button_Down[] = "Down";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_LDR_Head[] =
    "|                   LDR & Monitor                 |";
static uint8 const sShow_StdStr_LDR[] = "LDR & Monitor";
static uint8 const sShow_StdStr_LDR_Val[] = "Judge Value :";
static uint8 const sShow_StdStr_LDR_Spoint[] = "Snsi Point  :";
static uint8 const sShow_StdStr_LDR_SDomain[] = "Snsi Domain :";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_IfrFilt_Head[] =
    "|                Ifrared & filter                 |";
static uint8 const sShow_StdStr_IfrFilt[] = "Infrared Filter";
static uint8 const sShow_StdStr_IfrFilt_Pass[] = "Pass Li";
static uint8 const sShow_StdStr_IfrFilt_Block[] = "Block Li";
static uint8 const sShow_StdStr_IfrFilt_TYPE[] = "Type        :";
static uint8 const sShow_StdStr_IfrFilt_TYPEA[] = "TypeA";
static uint8 const sShow_StdStr_IfrFilt_TYPEB[] = "TypeB";
static uint8 const sShow_StdStr_IfrFilt_TYPEC[] = "TypeC";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_DubLens_Head[] =
    "|                 Double & Lens                   |";
static uint8 const sShow_StdStr_DubLens[] = "Double  Lens";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_IfrLi_Head[] =
    "|                Ifrared & Light                  |";
static uint8 const sShow_StdStr_IfrLi[] = "Infrared Light";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_StaLi_Head[] =
    "|                  State & Light                  |";
static uint8 const sShow_StdStr_StaLi[] = "State Light:\t";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_PTZ_Head[] =
    "|                    Pan & Tilt                   |";
static uint8 const sShow_StdStr_PTZ[] = "Pan & Tilt";
static uint8 const sShow_StdStr_PTZ_XRange[] = "h_Range     :";
static uint8 const sShow_StdStr_PTZ_YRange[] = "v_Range     :";
static uint8 const sShow_StdStr_PTZ_XMinDgr[] = "h_definition:";
static uint8 const sShow_StdStr_PTZ_YMinDgr[] = "v_definition:";
static uint8 const sShow_StdStr_PTZ_XPos[] = "h_Coo       :";
static uint8 const sShow_StdStr_PTZ_YPos[] = "v_Coo       :";
static uint8 const sShow_StdStr_PTZ_Limit[] = "Limit       :";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_NiLi_Head[] =
    "|                  Night & Light                  |";
static uint8 const sShow_StdStr_NiLi[] = "Night & Light";
static uint8 const sShow_StdStr_NiLi_Level[] = "Level       :";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_AuPl_Head[] =
    "|                  Audio & Plug                   |";
static uint8 const sShow_StdStr_AuPl[] = "Audio Plug";
static uint8 const sShow_StdStr_AuPl_SPeaker[] = "Speaker     :";
static uint8 const sShow_StdStr_AuPl_Microphone[] = "Microphone  :";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_Env_Head[] =
    "|                   Environment                   |";
static uint8 const sShow_StdStr_Env[] = "Environment";
static uint8 const sShow_StdStr_Env_Temp[] = "Temperature :";
static uint8 const sShow_StdStr_Env_Humi[] = "Humidity    :";

// ------------------------------------------------------------->
static uint8 const sShow_StdStr_RTC_Head[] =
    "|                       RTC                       |";
static uint8 const sShow_StdStr_RTC[] = "RTC";

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void gShow_ClrShowArea(void)
{
	uint16 gClrLineNum = 0;
	gClrLineNum = 100;
	while (gClrLineNum != 0) {
		printf("\e[1A");	//先回到上一行
		printf("\e[K");	//清除该行
		gClrLineNum--;
	}
}

// =============================================================================
/* home page
113  111 16             1113  111 16             11
***************************************************
| NUM | CMD              | NUM | CMD              |
---------------------------------------------------
| 1   | BUTTON           | 2   | LDR              |
| 3   | IRCUT            | 4   | IfrLi            |
| 5   | StaLi            | 6   | AuPl             |
---------------------------------------------------
Please select you want test event!
LOOK: You can input `help` for more information, or
input `exit` for quit this operation.
*/
extern mCapModule *gLinkModule[14];
void gfShow_CapabilitySection(void)
{
	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_L2);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gLinkModule);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L5);
	APPPrint_b("%s", sShow_StdStr_L6);
	APPPrint_b("%s", sShow_StdStr_L7);
}

// =============================================================================
/*
113  111 16             1113  111 16             11
***************************************************
|                 Button  Monitor                 |
---------------------------------------------------
| 1   | Auto Detect      | 2   |                  |
---------------------------------------------------
Status:(AutoDetect 1 times/1s, Press `Enter` to stop!)
KEY 1: UP/DOWN
....
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_Button[2];
/*
iNum:   key number, Not allow more than 32
iSta:   the Key Status, per bit match correspond key
iTime:  press-time-delay date link
*/
void gfShow_ButtonSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_Button_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_Button);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_wAuTo);

	{
		uint8 _i = 0;
		uint8 const *tSta;
		for (; _i < sDrvCap.aNumOfButton; _i++) {
			if (gStaG.aBut[_i].aStatus) {
				tSta = sShow_StdStr_Button_Down;
			} else {
				tSta = sShow_StdStr_Button_Up;
			}
			APPPrint_b("%s%d: %s\t%d", sShow_StdStr_Button, (int)_i,
			           tSta, gStaG.aBut[_i].aTimes);
		}
	}
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);
}

// =============================================================================
/*
113  111 16             1113  111 16             11
***************************************************
|                   Audio Plug                    |
---------------------------------------------------
| 1   | speaker ON       | 2   | speaker OFF      |
| 3   | microphone ON    | 4   | microphone OFF   |
| 5   | Auto Detect      | 6   |                  |
---------------------------------------------------
Status:(AutoDetect 1 times/1s, Press any keys to stop!)
Speaker: ON/OFF/NOT SUPPORTED TO THIS DIVICE
Microphone: ON/OFF/NOT SUPPORTED TO THIS DIVICE
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_AudioPlug[5];
/*
iCmd
bit0: (0:off 1:on)  for Speaker
bit1: (0:off 1:on)  for Microphone
*/
void gfShow_AudioPlugSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_AuPl_Head);
	APPPrint_b("%s", sShow_StdStr_L3);

	sfShow_UnionCmdString(gMod_AudioPlug);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_wAuTo);
	if (gStaG.aAuP.aStatus & 0x01) {
		APPPrint_b("%s%s", sShow_StdStr_AuPl_SPeaker, sShow_StdStr_ON);
	} else if (gStaG.aAuP.aStatus & 0x04) {
		APPPrint_b("%s%s", sShow_StdStr_AuPl_SPeaker,
		           sShow_StdStr_noSUPP);
	} else {
		APPPrint_b("%s%s", sShow_StdStr_AuPl_SPeaker, sShow_StdStr_OFF);
	}

	if (gStaG.aAuP.aStatus & 0x02) {
		APPPrint_b("%s%s", sShow_StdStr_AuPl_Microphone,
		           sShow_StdStr_ON);
	} else if (gStaG.aAuP.aStatus & 0x08) {
		APPPrint_b("%s%s", sShow_StdStr_AuPl_Microphone,
		           sShow_StdStr_noSUPP);
	} else {
		APPPrint_b("%s%s", sShow_StdStr_AuPl_Microphone,
		           sShow_StdStr_OFF);
	}

	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);
}

// =============================================================================
/*
113  111 16             1113  111 16             11
***************************************************
|                    LDR  Monitor                 |
---------------------------------------------------
| 1   | Set Sensitive    | 2   | Auto Detect      |
---------------------------------------------------
Status:(AutoDetect 1 times/1s, Press `Enter` to stop!)
Enviroment:     Day/Night
Judge Value:    (num)
Sensitive:      (num)
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
/*
Sta:
    0x00        Night
    0x01        Day
*/
extern mCapModule *gMod_LDR[];
void gfShow_LDRSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_LDR_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_LDR);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_wAuTo);

	if (gStaG.aLDR.aStatus & 0x01) {
		APPPrint_b("%s%s", sShow_StdStr_Sta, sShow_StdStr_DAY);
	} else {
		APPPrint_b("%s%s", sShow_StdStr_Sta, sShow_StdStr_NIG);
	}
	APPPrint_b("%s%d", sShow_StdStr_LDR_Val, (int)gStaG.aLDR.aJudgeValue);
	// APPPrint_b("%s%d",sShow_StdStr_LDR_Sensi,(int)gStaG.aLDR.aSPoint);
	APPPrint_b("%s%d", sShow_StdStr_LDR_Spoint, (int)gStaG.aLDR.aSPoint);
	APPPrint_b("%s%d", sShow_StdStr_LDR_SDomain, (int)gStaG.aLDR.aSDomain);

	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);
}

// =============================================================================
/*
113  111 16             1113  111 16             11
***************************************************
|                      IRCut                      |
---------------------------------------------------
| 1   | IfrLi Pass       | 2   | IfrLi Block      |
---------------------------------------------------
State:          Pass/Block
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_IfrFilt[];
void gfShow_IfrFiltSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_IfrFilt_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_IfrFilt);
	APPPrint_b("%s", sShow_StdStr_L3);

	switch (gStaG.aIfF.aType) {
	case 1:
		APPPrint_b("%s%s", sShow_StdStr_IfrFilt_TYPE,
		           sShow_StdStr_IfrFilt_TYPEA);
		break;
	case 2:
		APPPrint_b("%s%s", sShow_StdStr_IfrFilt_TYPE,
		           sShow_StdStr_IfrFilt_TYPEB);
		break;
	case 3:
		APPPrint_b("%s%s", sShow_StdStr_IfrFilt_TYPE,
		           sShow_StdStr_IfrFilt_TYPEC);
		break;
	default:
		APPPrint_b("type unknow !!\t%d", gStaG.aIfF.aType);
		break;
	}

	if (gStaG.aIfF.aStatus & 0x01) {
		APPPrint_b("%s%s", sShow_StdStr_Sta, sShow_StdStr_IfrFilt_Pass);
	} else {
		APPPrint_b("%s%s", sShow_StdStr_Sta,
		           sShow_StdStr_IfrFilt_Block);
	}

	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);

}

/*
113  111 16             1113  111 16             11
***************************************************
|               Infrared  Light                   |
---------------------------------------------------
| 1   | On               | 2   | Off              |
---------------------------------------------------
State:          On/Off
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_IfrLi[];

void gfShow_IfrLiSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_IfrLi_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_IfrLi);
	APPPrint_b("%s", sShow_StdStr_L3);

	if (gStaG.aIfL.aStatus & 0x01) {
		APPPrint_b("%s%s", sShow_StdStr_Sta, sShow_StdStr_ON);
	} else {
		APPPrint_b("%s%s", sShow_StdStr_Sta, sShow_StdStr_OFF);
	}
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);

}

/*
113  111 16             1113  111 16             11
***************************************************
|                  State  Light                   |
---------------------------------------------------
| 1   | On               | 2   | Off              |
| 3   | flash            |     |                  |
---------------------------------------------------
State:          On/Off/Flash
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_StaLi[];
void gfShow_StaLiSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_StaLi_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_StaLi);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);

}

/*
113  111 16             1113  111 16             11
***************************************************
|                  Night  Light                   |
---------------------------------------------------
| 1   | On               | 2   | Off              |
| 3   | Set LEVEL        |     |                  |
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_NiLi[];
// 0x 00 00 00 00
//    1             显示附加行
//        2         显示附加1行
//        0         显示数字
//        1         显示字符串
//       2          显示附加2行
//       0          显示附加2行数字
//       1          显示附加2行字符串
//          ff      附加行第二行的状态（值）
//             ff   附加行第一行的状态（值）
uint32 gShowNiLiStatus = 0;
uint8 *gShowNiLiLin1Point;
uint8 *gShowNiLiLin2Point;

void gfShow_NiLiSection(void)
{

	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_NiLi_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_NiLi);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);

	if (gShowNiLiStatus & 0x80000000) {
		if (gShowNiLiStatus & 0x00020000) {
			if (gShowNiLiStatus & 0x00010000) {
				APPPrint_b("%s%s", sShow_StdStr_Input,
				           gShowNiLiLin1Point);
			} else {
				APPPrint_b("%s%d", sShow_StdStr_Input,
				           gShowNiLiStatus & 0xFF);
			}

			if (gShowNiLiStatus & 0x00200000) {
				if (gShowNiLiStatus & 0x00100000) {
					APPPrint_b("%s%s", sShow_StdStr_Input,
					           gShowNiLiLin2Point);
				} else {
					APPPrint_b("%s%d", sShow_StdStr_Input,
					           (gShowNiLiStatus & 0xFF00) >>
					           8);
				}
			}
		}
	}
}

/*
113  111 16             1113  111 16             11
***************************************************
|                  Double  Lens                   |
---------------------------------------------------
| 1   | Day              | 2   | Night            |
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/
extern mCapModule *gMod_DobLens[];
void gfShow_DubLensSection(void)
{
	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_DubLens_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_DobLens);
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);
}

/*
113  111 16             1113  111 16             11
***************************************************
|                       PTZ                       |
---------------------------------------------------
| 1   | Left mv 15Dgr    | 2   | Right mv 15Dgr   |
| 3   | Down mv 15Dgr    | 4   | Up mv 15Dgr      |
| 5   | (0,0)            | 6   | (0,max)          |
| 7   | (max,0)          | 8   | (max,max)        |
---------------------------------------------------
h_Range     :
v_Range     :
h_definition:
v_definition:
h_Coo       :
v_Coo       :
Limit       : NONE/HV
---------------------------------------------------
Please select you want test event!
tips: Input `exit` for quit this operation.
*/

// 00 00 00 00
//           0  NONE
//           1  HV
//
uint32 gShowPTZStatus = 0;
extern mCapModule *gMod_PTZ[];
extern void gfShow_PTZSection(void)
{
	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("%s", sShow_StdStr_PTZ_Head);
	APPPrint_b("%s", sShow_StdStr_L3);
	sfShow_UnionCmdString(gMod_PTZ);
	APPPrint_b("%s", sShow_StdStr_L3);

	APPPrint_b("%s%d", sShow_StdStr_PTZ_XRange, gStaG.aPTZ.aXMinDgr);
	APPPrint_b("%s%d", sShow_StdStr_PTZ_YRange, gStaG.aPTZ.aYMinDgr);
	APPPrint_b("%s%d", sShow_StdStr_PTZ_XMinDgr, gStaG.aPTZ.aXRange);
	APPPrint_b("%s%d", sShow_StdStr_PTZ_YMinDgr, gStaG.aPTZ.aYRange);
	APPPrint_b("%s%d", sShow_StdStr_PTZ_XPos, gStaG.aPTZ.aXPos);
	APPPrint_b("%s%d", sShow_StdStr_PTZ_YPos, gStaG.aPTZ.aYPos);
	if (gShowPTZStatus & 0x01) {
		APPPrint_b("%s%s", sShow_StdStr_PTZ_Limit, "HV");
	} else {
		APPPrint_b("%s%s", sShow_StdStr_PTZ_Limit, "NONE");
	}
	APPPrint_b("%s", sShow_StdStr_L3);
	APPPrint_b("%s", sShow_StdStr_L4);
	APPPrint_b("%s", sShow_StdStr_L6);

}

/*

113  111 16             1113  111 16             11
***************************************************
Button
Button_(x)
Status      :
Interval    :

Day & Night Detect:
Status      :
Judge Value :
Sensitive   :

Infrared Filter
Type        :
Status      :

Double Lens
Status      :

Infrared Light
Status      :

PTZ
Min XDegree :
Min YDegree :
X Range     :
Y Range     :
XPos        :
YPos        :

Night Light
Status      :

Audio Plug
Speaker     :
Microphone  :

Environment
Temperature :
Humidity    :

RTC
yy-mm-dd-hh-mm-ss
***************************************************
*/

void gfShow_AllModeStatus(void)
{
	uint8 const *tStrSta = NULL;
	uint32 iCap = sDrvCap.aDrvCapMask;
	APPPrint_b("%s", sShow_StdStr_L1);
	APPPrint_b("Capability show:");

	if (iCap & CAP_SUPP_ButtonMonitor) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_Button);
		{
			uint8 _i;
			for (_i = 0; _i < sDrvCap.aNumOfButton; _i++) {
				if (gStaG.aBut[_i].aStatus) {
					tStrSta = sShow_StdStr_Button_Down;
				} else {
					tStrSta = sShow_StdStr_Button_Up;
				}
				APPPrint_b("\t%s_%d\t%s%s\t\t%s%d",
				           sShow_StdStr_Button,
				           (int)_i,
				           sShow_StdStr_Sta,
				           tStrSta,
				           sShow_StdStr_Button_Interval,
				           (int)gStaG.aBut[_i].aTimes);
			}
		}
	}
	if (iCap & CAP_SUPP_LdrMonitor) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_LDR);
		if (gStaG.aLDR.aStatus & 0x01) {
			tStrSta = sShow_StdStr_DAY;
		} else {
			tStrSta = sShow_StdStr_NIG;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_Sta, tStrSta);
		APPPrint_b("\t%s%d", sShow_StdStr_LDR_Val,
		           (int)gStaG.aLDR.aJudgeValue);
		// APPPrint_b("%s%d",sShow_StdStr_LDR_Sensi,(int)gStaG.aLDR.aSensitive);
		APPPrint_b("\t%s%d", sShow_StdStr_LDR_Spoint,
		           (int)gStaG.aLDR.aSPoint);
		APPPrint_b("\t%s%d", sShow_StdStr_LDR_SDomain,
		           (int)gStaG.aLDR.aSDomain);

	}
	if (iCap & CAP_SUPP_Ircut) {
		tStrSta = NULL;
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_IfrFilt);
		switch (gStaG.aIfF.aType) {
		case 1:
			tStrSta = sShow_StdStr_IfrFilt_TYPEA;
			break;
		case 2:
			tStrSta = sShow_StdStr_IfrFilt_TYPEB;
			break;
		case 3:
			tStrSta = sShow_StdStr_IfrFilt_TYPEC;
			break;
		default:
			APPPrint_b("type unknow !!\t%d", gStaG.aIfF.aType);
			break;
		}

		if (tStrSta != NULL) {
			APPPrint_b("\t%s%s", sShow_StdStr_IfrFilt_TYPE,
			           tStrSta);
		}

		if (gStaG.aIfF.aStatus & 0x01) {
			tStrSta = sShow_StdStr_IfrFilt_Pass;
		} else {
			tStrSta = sShow_StdStr_IfrFilt_Block;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_Sta, tStrSta);
	}
	if (iCap & CAP_SUPP_DoubleLens) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_DubLens);
		if (gStaG.aDbL.aStatus & 0x01) {
			tStrSta = sShow_StdStr_IfrFilt_Pass;
		} else {
			tStrSta = sShow_StdStr_IfrFilt_Block;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_Sta, tStrSta);
	}
	if (iCap & CAP_SUPP_IfrLamp) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_IfrLi);
		if (gStaG.aIfL.aStatus & 0x01) {
			tStrSta = sShow_StdStr_ON;
		} else {
			tStrSta = sShow_StdStr_OFF;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_Sta, tStrSta);
	}
	// if( iCap&CAP_SUPP_StateLed       )
	if (iCap & CAP_SUPP_PTZ) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_PTZ);

		APPPrint_b("\t%s%d", sShow_StdStr_PTZ_XMinDgr,
		           (int)gStaG.aPTZ.aXMinDgr);
		APPPrint_b("\t%s%d", sShow_StdStr_PTZ_YMinDgr,
		           (int)gStaG.aPTZ.aYMinDgr);
		APPPrint_b("\t%s%d", sShow_StdStr_PTZ_XRange,
		           (int)gStaG.aPTZ.aXRange);
		APPPrint_b("\t%s%d", sShow_StdStr_PTZ_YRange,
		           (int)gStaG.aPTZ.aYRange);
		APPPrint_b("\t%s%d", sShow_StdStr_PTZ_XPos,
		           (int)gStaG.aPTZ.aXPos);
		APPPrint_b("\t%s%d", sShow_StdStr_PTZ_YPos,
		           (int)gStaG.aPTZ.aYPos);

	}
	if (iCap & CAP_SUPP_NightLight) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_NiLi);
		if (gStaG.aNiL.aStatus & 0x01) {
			tStrSta = sShow_StdStr_ON;
		} else {
			tStrSta = sShow_StdStr_OFF;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_Sta, tStrSta);
		APPPrint_b("\t%s%s", sShow_StdStr_Sta, gStaG.aNiL.aLevel);

	}
	// if( iCap&NVC_SUPP_CoolFan        )
	if (iCap & CAP_SUPP_AudioPlug) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_AuPl);

		if (gStaG.aAuP.aStatus & 0x01) {
			tStrSta = sShow_StdStr_ON;
		} else if (gStaG.aAuP.aStatus & 0x04) {
			tStrSta = sShow_StdStr_noSUPP;
		} else {
			tStrSta = sShow_StdStr_OFF;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_AuPl_SPeaker, tStrSta);

		if (gStaG.aAuP.aStatus & 0x02) {
			tStrSta = sShow_StdStr_ON;
		} else if (gStaG.aAuP.aStatus & 0x08) {
			tStrSta = sShow_StdStr_noSUPP;
		} else {
			tStrSta = sShow_StdStr_OFF;
		}
		APPPrint_b("\t%s%s", sShow_StdStr_AuPl_Microphone, tStrSta);

	}
	if (iCap & (CAP_SUPP_TempMonitor | CAP_SUPP_HumiMonitor)) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_Env);
		APPPrint_b("\t%s%d", sShow_StdStr_Env_Temp,
		           (int)gStaG.aEnv.aHumi);
		APPPrint_b("\t%s%d", sShow_StdStr_Env_Humi,
		           (int)gStaG.aEnv.aTemp);
	}
	// if( iCap&CAP_SUPP_TempMonitor    )
	// if( iCap&CAP_SUPP_HumiMonitor    )
	// if( iCap&CAP_SUPP_GpioReset      )
	if (iCap & CAP_SUPP_RTC) {
		APPPrint_b("%s", sShow_StdStr_L3);
		APPPrint_b("%s", sShow_StdStr_RTC);
		APPPrint_b("\t%d-%d-%d %d:%d:%d",
		           (int)gStaG.aRTC.aScd,
		           (int)gStaG.aRTC.aMnu,
		           (int)gStaG.aRTC.aHur,
		           (int)gStaG.aRTC.aDay,
		           (int)gStaG.aRTC.aMth, (int)gStaG.aRTC.aYea);
	}

	APPPrint_b("%s", sShow_StdStr_L1);
}

//==============================================================================
//---------------------------------------------->INTERNAL FUNCTION
static void sfShow_UnionCapString(uint8 *iTar,
                                  uint8 *iStr1, uint8 *iStr2, uint8 iNum1st)
{
	//
	uint8 tNumStr[20];
	uint8 tNumNum;
	memcpy(iTar, "|     |                  |     |                  |", 52);

	iTar += 2;
	tNumNum = gfStr_itoa(iNum1st, tNumStr, 10, 20);

	while (tNumNum != 3) {
		tNumStr[tNumNum++] = ' ';
	};
	strncpy(iTar, tNumStr, 3);
	iTar += 3 + 3;

	// iTar += 3;
	if (iStr1 != NULL) {
		tNumNum = strlen(iStr1);
		strncpy(iTar, iStr1, tNumNum);
	}
	iTar += 16 + 3;

	if (iStr2 != NULL) {
		// iTar += 3;
		iNum1st++;
		tNumNum = gfStr_itoa(iNum1st, tNumStr, 10, 20);
		while (tNumNum != 3) {
			tNumStr[tNumNum++] = ' ';
		};
		strncpy(iTar, tNumStr, 3);
		iTar += 3 + 3;

		// iTar += 3;
		tNumNum = strlen(iStr2);
		strncpy(iTar, iStr2, tNumNum);
	}
}

/*
return:
    0: 队列中还有数据
    -1：队列中已经没有数据了
*/
static void sfShow_UnionCmdString(mCapModule *iMod[])
{
	uint8 _i = 0;
	uint8 tShowBuf[60];
	while (iMod[_i] != NULL) {
		if (iMod[_i + 1] == NULL) {
			sfShow_UnionCapString(tShowBuf,
			                      iMod[_i]->aCapName, NULL, _i + 1);
			_i += 1;
		} else {
			sfShow_UnionCapString(tShowBuf,
			                      iMod[_i]->aCapName,
			                      iMod[_i + 1]->aCapName, _i + 1);
			_i += 2;
		}
		APPPrint_b("%s", tShowBuf);
	}
}

// =============================================================================
// show desk
#define MQ_PK printf
unsigned char sgStandCharBlack[] = "0123456789ABCDEF";
void DbgPrinStr(unsigned char *iStr, unsigned char *iPD, unsigned short iLen)
{
	unsigned short _i, _j;
	unsigned char tTppl[4];
	tTppl[2] = ' ';
	tTppl[3] = '\0';

	MQ_PK("%s", iStr);
	_i = 0;
	while (_i < iLen) {
		for (_j = 0; (_j < 20) && (_i < iLen); _j++) {
			if (_j == 10) {
				MQ_PK(" ");
			}
			tTppl[0] = sgStandCharBlack[(iPD[_i] & 0xF0) >> 4];
			tTppl[1] = sgStandCharBlack[iPD[_i] & 0x0F];
			MQ_PK("%s", tTppl);
			_i++;
		}
		MQ_PK("\r\n");
	}
}

//默认10进制

uint8 *gfStr_Cpy(uint8 *iDest, void *iSrc, uint16 ilen)
{
	uint8 *tSrc = (uint8 *) iSrc;
	uint8 *ptSrcEnd = tSrc + ilen;
	while (tSrc < ptSrcEnd) {
		*iDest++ = *tSrc++;
	}
	return iDest;
}

uint8 *gfStr_Ist(uint8 *iDest, uint8 *iSrc)
{
	uint8 *tSrc = (uint8 *) iSrc;
	while (*tSrc != '\0') {
		*iDest++ = *tSrc++;
	}
	return iDest;
}

uint8 gfStr_itoa(uint32 iVal, uint8 *iTar, uint8 iPro, uint8 tTLen)
{

	uint8 *tToo = iTar + tTLen - 1;
	uint8 *tToo1 = iTar;
	uint8 tNum = 0;
	if (iPro == 10) {
		*tToo-- = '\0';
		do {
			*tToo-- = sgStandCharBlack[iVal % 10];
			iVal /= 10;
		} while (iVal);
		tToo++;
		while (*tToo != '\0') {
			*tToo1++ = *tToo++;
			tNum++;
		}
		*tToo1 = '\0';
	} else if (iPro == 16) {
		*iTar++ = sgStandCharBlack[(iVal & 0xF0000000) >> 28];
		*iTar++ = sgStandCharBlack[(iVal & 0xF000000) >> 24];
		*iTar++ = sgStandCharBlack[(iVal & 0xF00000) >> 20];
		*iTar++ = sgStandCharBlack[(iVal & 0xF0000) >> 16];
		*iTar++ = sgStandCharBlack[(iVal & 0xF000) >> 12];
		*iTar++ = sgStandCharBlack[(iVal & 0xF00) >> 8];
		*iTar++ = sgStandCharBlack[(iVal & 0xF0) >> 4];
		*iTar++ = sgStandCharBlack[iVal & 0xF];
		*iTar = '\0';
	}
	return tNum;
}
