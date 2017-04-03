#include <string.h>
#include <stdio.h>
#include "app_def.h"
#include "../GlobalParameter.h"

int gfResolv_UserInput(mCapModule *iMod[])
{

	uint8 tUserInput[128];
	uint8 tInputbuf;
	uint8 tInputLen = 0;
	do {
		tInputbuf = getchar();
		if (tInputbuf == '\n') {
			tUserInput[tInputLen++] = '\0';
			break;
		} else {
			tUserInput[tInputLen++] = tInputbuf;
		}
		if (tInputLen == 128) {
			tInputLen = 0;
		}

	} while (tUserInput[tInputLen - 1]);

	if (0 == strcmp(tUserInput, "help")) {
		return -1;
	} else if (0 == strcmp(tUserInput, "exit")) {
		return -2;
	} else if (0 == strcmp(tUserInput, "show")) {
		return -3;
	} else {
		uint8 _i = 0;

		while (iMod[_i] != NULL) {
			if ((0 == strcmp(tUserInput, iMod[_i]->aCapName))
			    || (atoi(tUserInput) == iMod[_i]->aNum)) {
				return iMod[_i]->aNum;
			}
			_i++;
		}
	}
	return 0;
}

static mCapModule sMod__AutoDetect = { 1, "Auto Detect", };

mCapModule *gMod_Button[2] = {
	&sMod__AutoDetect,
	NULL,
};

static mCapModule sMod_LDR_SetSensitive = { 1, "Set Sensitive", };

mCapModule *gMod_LDR[3] = {
	&sMod_LDR_SetSensitive,
	&sMod__AutoDetect,
	NULL,
};

static mCapModule sMod_IfrFilt_BLOCK = { 1, "BLOCK ", };
static mCapModule sMod_IfrFilt_PASS = { 2, "PASS", };

mCapModule *gMod_IfrFilt[3] = {
	&sMod_IfrFilt_BLOCK,
	&sMod_IfrFilt_PASS,
	NULL,
};

static mCapModule sMod_DobLens_BLOCK = { 1, "DayLens", };
static mCapModule sMod_DobLens_PASS = { 2, "NightLens", };

mCapModule *gMod_DobLens[3] = {
	&sMod_DobLens_BLOCK,
	&sMod_DobLens_PASS,
	NULL,
};

static mCapModule sMod_IfrLi_On = { 1, "On", };
static mCapModule sMod_IfrLi_Off = { 2, "Off ", };

mCapModule *gMod_IfrLi[3] = {
	&sMod_IfrLi_On,
	&sMod_IfrLi_Off,
	NULL,
};

static mCapModule sMod_StaLi_On = { 1, "On", };
static mCapModule sMod_StaLi_Off = { 2, "Off", };
static mCapModule sMod_StaLi_Switch = { 3, "Switch", };

mCapModule *gMod_StaLi[4] = {
	&sMod_StaLi_On,
	&sMod_StaLi_Off,
	&sMod_StaLi_Switch,
	NULL,
};

static mCapModule sMod_PTZ_Left = { 1, "Left mv 15Dgr", };
static mCapModule sMod_PTZ_Right = { 2, "Right mv 15Dgr", };
static mCapModule sMod_PTZ_Up = { 3, "Down mv 15Dgr", };
static mCapModule sMod_PTZ_Down = { 4, "Up mv 15Dgr", };
static mCapModule sMod_PTZ_OO = { 5, "(0,0)" };
static mCapModule sMod_PTZ_OM = { 6, "(0,max)" };
static mCapModule sMod_PTZ_MO = { 7, "(max,0)" };
static mCapModule sMod_PTZ_MM = { 8, "(max,max)" };
static mCapModule sMod_PTZ_SP = { 9, "Set Pre" };
static mCapModule sMod_PTZ_CP = { 10, "Clr Pre" };
static mCapModule sMod_PTZ_LPC = { 11, "load Pre Conf" };
static mCapModule sMod_PTZ_SPP = { 12, "Set Pre Path" };
static mCapModule sMod_PTZ_STP = { 13, "Start Pre" };
static mCapModule sMod_PTZ_STC = { 14, "Start Curise" };

mCapModule *gMod_PTZ[] = {
	&sMod_PTZ_Left,
	&sMod_PTZ_Right,
	&sMod_PTZ_Up,
	&sMod_PTZ_Down,
	&sMod_PTZ_OO,
	&sMod_PTZ_OM,
	&sMod_PTZ_MO,
	&sMod_PTZ_MM,
	&sMod_PTZ_SP,
	&sMod_PTZ_CP,
	&sMod_PTZ_LPC,
	&sMod_PTZ_SPP,
	&sMod_PTZ_STP,
	&sMod_PTZ_STC,
	NULL,
};

static mCapModule sMod_NiLi_On = { 1, "On", };
static mCapModule sMod_NiLi_Off = { 2, "Off", };

// static mCapModule sMod_NiLi_AutoDetect      ={5, "SetLevel",};
mCapModule *gMod_NiLi[4] = {
	&sMod_NiLi_On,
	&sMod_NiLi_Off,
	// &sMod_NiLi_AutoDetect,
	NULL,
};

static mCapModule sMod_AuPl_SpeakerOn = { 1, "speaker ON", };
static mCapModule sMod_AuPl_SpeakerOff = { 2, "speaker OFF", };
static mCapModule sMod_AuPl_MicrophoneOn = { 3, "microphone ON", };
static mCapModule sMod_AuPl_MicrophoneOFF = { 4, "microphone OFF", };
static mCapModule sMod_AuPl_AutoDetect = { 5, "Auto Detect", };

mCapModule *gMod_AudioPlug[6] = {
	&sMod_AuPl_SpeakerOn,
	&sMod_AuPl_SpeakerOff,
	&sMod_AuPl_MicrophoneOn,
	&sMod_AuPl_MicrophoneOFF,
	&sMod_AuPl_AutoDetect,
	NULL,
};

// Temperature
// Humidity
// ResetIO
// RTC

static mCapModule sMod_RTC_SetCur = { 1, "Set Cur Time", };
static mCapModule sMod_RTC_SetErr = { 2, "Set Err Time", };
static mCapModule sMod_RTC_AutoDetect = { 3, "Auto Detect", };

mCapModule *gMod_RTC[4] = {
	&sMod_RTC_SetCur,
	&sMod_RTC_SetErr,
	&sMod_RTC_AutoDetect,
	NULL,
};
