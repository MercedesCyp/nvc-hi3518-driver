#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>		// gettimeofday();
// #include <sys/stat.h>
#include <fcntl.h>
#include <fcntl.h>

#include "app_def.h"
#include "../GlobalParameter.h"
#include "../Tool/MsgProtocal.h"

#define DC_PTZ_PRESET_Single_Num 9

//==============================================================================
//----------------------------------------------------------->Function definition
// external
// extern void gfShow_CapabilitySection(void);
extern void gfShow_ButtonSection(void);
extern void gfShow_AudioPlugSection(void);
extern void gfShow_LDRSection(void);
extern void gfShow_IfrFiltSection(void);
extern void gfShow_IfrLiSection(void);
extern void gfShow_StaLiSection(void);
extern void gfShow_NiLiSection(void);
extern void gfShow_DubLensSection(void);
extern void gfShow_PTZSection(void);

extern uint8 *gfStr_Ist(uint8 *iDest, uint8 *iSrc);
extern uint8 *gfStr_Cpy(uint8 *iDest, void *iSrc, uint16 ilen);
// extern uint8 *gfStr_itoa(uint32 iVal, uint8 *tTar,uint8 tTLen);
// internal
static int32 sfMsgCent_Union(void *iMsg,
                             uint16 iMsgLen,
                             uint16 iCmd, uint8 iUnit, uint8 iErr);

static int sfMsgCent_write(uint16 iLen);
static int sfMsgCent_read(void);
static void sfMsgCent_ShowCap(mNVC_DRV_CAP_INFO *iCap);

static int sfMsgCent_TestButton(void);
static int sfMsgCent_TestLDR(void);
static int sfMsgCent_TestIfrLi(void);
static int sfMsgCent_TestIRCut(void);
static int sfMsgCent_TestDoubleLens(void);
static int sfMsgCent_TestStaLi(void);
static int sfMsgCent_TestNiLi(void);
static int sfMsgCent_TestAudioPlug(void);
static int sfMsgCent_PTZLens(void);

#define DCA_MsgCent_Update_ALL      (0xFFFFFFFF)
#define DCA_MsgCent_Update_Device   (0x01)
#define DCA_MsgCent_Update_Button   (0x02)
#define DCA_MsgCent_Update_Ldr      (0x03)
#define DCA_MsgCent_Update_Irc      (0x04)
#define DCA_MsgCent_Update_IfrLi    (0x05)
#define DCA_MsgCent_Update_PTZ      (0x07)
#define DCA_MsgCent_Update_NiLi     (0x08)
#define DCA_MsgCent_Update_AuPl     (0x09)
#define DCA_MsgCent_Update_Temp     (0x0A)
#define DCA_MsgCent_Update_Humi     (0x0B)
#define DCA_MsgCent_Update_DubLens  (0x0C)
#define DCA_MsgCent_Update_RTC      (0x0D)
void gfMsgCent_UpdateModStatus(uint32 iCmd);

//--------------------------------------------------------------------->Variable
mNVC_DRV_Ver_INFO sDrvInfo;
mNVC_DRV_CAP_INFO sDrvCap;
mCapModule *gLinkModule[14];
mSta_G gStaG = {
	.aStatus = 0,
};

void (*gfMsgCent_Thread1Show)(void) = NULL;
uint32 gShowFrameControler = 0;
//
static int sDrvHandler;
static int sMaxHd;
static uint8 sDrvSend[512];
static uint8 sDrvRcv[512];

static mCapModule sButtonModule = {
	.aCapName = DCA_CMD_BUTTON,
	.afModFun = sfMsgCent_TestButton,
};

static mCapModule sLDRModule = {
	.aCapName = DCA_CMD_LDR,
	.afModFun = sfMsgCent_TestLDR,
};

static mCapModule sIRCUTModule = {
	.aCapName = DCA_CMD_IRCUT,
	.afModFun = sfMsgCent_TestIRCut,
};

static mCapModule sDoubLensModule = {
	.aCapName = DCA_CMD_DoubLens,
	.afModFun = sfMsgCent_TestDoubleLens,
};

static mCapModule sIfrLiModule = {
	.aCapName = DCA_CMD_IfrLi,
	.afModFun = sfMsgCent_TestIfrLi,
};

static mCapModule sStaLiModule = {
	.aCapName = DCA_CMD_StaLi,
	.afModFun = sfMsgCent_TestStaLi,
};

static mCapModule sPTZModule = {
	.aCapName = DCA_CMD_PTZ,
	.afModFun = sfMsgCent_PTZLens,
};

static mCapModule sNiLiModule = {
	.aCapName = DCA_CMD_NiLi,
	.afModFun = sfMsgCent_TestNiLi,
};

static mCapModule sAuPlModule = {
	.aCapName = DCA_CMD_AuPl,
	.afModFun = sfMsgCent_TestAudioPlug,
};

static mCapModule sTempModule = {
	.aCapName = DCA_CMD_Temp,
};

static mCapModule sHumiModule = {
	.aCapName = DCA_CMD_Humi,
};

static mCapModule sGPIOResetModule = {
	.aCapName = DCA_CMD_GpioRst,
};

static mCapModule sRTCModule = {
	.aCapName = DCA_CMD_RTC,
};

//----------------------------------------------------------->Global Function
int gfMsgCenta_DetectEnv(void)
{
	if (system("test -f /mnt/mtd/modules/NV_Driver/NV_Driver.ko")) {
		APPPrint_r("NV_Driver.ko file not exist!");
		return -1;
	}

	if (system("test -d /tmp/NVDrive")) {
		system("mkdir /tmp/NVDrive");
	}

	if (system("test -f /tmp/NVDrive/1.log")) {
		system("touch /tmp/NVDrive/1.log");
	} else {
		int size;
		FILE *tF = fopen(DCA_DriverLog1_Path, "a");
		fseek(tF, 0, SEEK_END);
		size = ftell(tF);
		fclose(tF);
		if (size > 20480) {
			system("mv /tmp/NVDrive/1.log /tmp/NVDrive/2.log");
			system("touch /tmp/NVDrive/1.log");
		}
		tF = fopen(DCA_DriverLog1_Path, "a");
		fwrite("========================================\n", 41, 1, tF);
		fclose(tF);

	}
	return 0;
}

/*
 * * * * * * * * * * * * * * * * * * * * *
*                                         *
*           NV_DRIVER TEST                *
*                                         *
 * * * * * * * * * * * * * * * * * * * * *
--------------- DRIVER INFO ---------------
*/
void *sfMsgCent_ReadDeal(void *args);
int gfMsgCenta_DetectDriver(void)
{
	int tRet;

	APPPrint_r(" * * * * * * * * * * * * * * * * * * * * * ");
	APPPrint_r("*                                         *");
	APPPrint_r("*           NV_DRIVER TEST                *");
	APPPrint_r("*                                         *");
	APPPrint_r(" * * * * * * * * * * * * * * * * * * * * * ");
	sleep(1);
	sDrvHandler = open(DCA_Driver_Path, O_RDWR);
	if (sDrvHandler < 0) {
		APPPrint_r
		("Your device didn't INSMOD driver! Please INSMOD first!!!");
		return -1;
	} else {
		APPPrint_b("open `/dev/NV_Driver` Success");

		{
			int tRet;
			pthread_t a_thread;
			// pthread_create 如果成功则返回 0，如果失败则返回出错编号
			tRet =
			    pthread_create(&a_thread, NULL, sfMsgCent_ReadDeal,
			                   NULL);
			if (tRet) {
				APPPrint_r
				("ERROR NUMBER:%d\tThe thread create fail!",
				 tRet);
				return -1;
			} else {
				APPPrint_b("Thread create success!");
			}
		}
		sMaxHd = sDrvHandler + 1;
	}

	sleep(1);

	{
		uint16 tSendLen;

		tSendLen = sfMsgCent_Union(NULL, 0,
		                           NVC_MSG_TYPE_DEVICE |
		                           NVC_MSG_DEVICE_GET_INFO, 0, 0);
		tRet = sfMsgCent_write(tSendLen);

		tSendLen = sfMsgCent_Union(NULL, 0,
		                           NVC_MSG_TYPE_DEVICE |
		                           NVC_MSG_DEVICE_GET_CAP, 0, 0);
		tRet = sfMsgCent_write(tSendLen);

		{
			// wait driver return initialized driver infomation
			uint8 iCount_wait = 10;
			while (iCount_wait) {
				if ((gStaG.aStatus & DCA_StaG_Cap_UpD)
				    && (gStaG.aStatus & DCA_StaG_Dvc_UpD)) {
					break;
				}
				sleep(1);
				iCount_wait--;
			}
			if (iCount_wait == 0) {
				APPPrint_b
				("Fail to recive the driver innitialized information!");
				return -1;
			}
		}

		APPPrint_b("--------------- DRIVER INFO ---------------");
		APPPrint_b("Driver path: /dev/NV_Driver");
		APPPrint_b("Chip: 0x%x", sDrvInfo.aChipInfo);
		APPPrint_b("Pro: 0x%x", sDrvInfo.aDeviceInfo);
		APPPrint_b("Driver Version: %s", sDrvInfo.aVerInfo);
		APPPrint_b("Compiler Date: %s", sDrvInfo.aBuildData);
		sfMsgCent_ShowCap(&sDrvCap);

		tSendLen = sfMsgCent_Union(NULL, 0,
		                           NVC_MSG_TYPE_DEVICE |
		                           NVC_MSG_DEVICE_SUB_REPORT, 0, 0);
		tRet = sfMsgCent_write(tSendLen);
		// tRet = sfMsgCent_read();
		APPPrint_b("Register Report Success!");
	}
	printf("\n");
	APPPrint_b("Press `Enter` continue.....");
	{
		int tInput;
		do {
			tInput = getchar();
		} while (tInput != '\n');
	}
	system("clear");
	gfMsgCent_UpdateModStatus(0xFFFFFFFF);
	return 0;
}

int gfMsgCenta_CloseDriver(void)
{
	close(sDrvHandler);
	return 0;
}

static uint32 sfMsgCent_Device_GetStatus(void)
{
	uint16 tSendLen;
	int tRet;

	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_DEVICE |
	                           NVC_MSG_DEVICE_GET_INFO, 0, 0);
	tRet = sfMsgCent_write(tSendLen);

	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_DEVICE | NVC_MSG_DEVICE_GET_CAP,
	                           0, 0);
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

//----------------------------------->Button monitor
extern mCapModule *gMod_Button[2];
static int sfMsgCent_TestButton(void)
{
	int tRet;
	uint8 tSta = 0;

	gfMsgCent_Thread1Show = gfShow_ButtonSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		gfMsgCent_UpdateModStatus(DCA_MsgCent_Update_Button);
		if (tSta) {
			uint8 tChar;
			sleep(1);
			tChar = getchar();
			if (tChar > 0) {
				while (tChar != '\n') {
					tChar = getchar();
				}
				tSta = 0x00;
				fcntl(0, F_SETFL, 0);
				DF_DCA_FrameUpdate_SetOnce;
			}
		} else {
			tRet = gfResolv_UserInput(gMod_Button);
			DF_DCA_FrameUpdate_IncOnce;
			if (tRet == -2) {
				break;
			} else if (tRet > 0) {
				switch (tRet) {
				case 1:
					tSta = 1;
					fcntl(0, F_SETFL, O_NONBLOCK);
					DF_DCA_FrameUpdate_RUN;
					break;
				}
			}
		}

	}
	return 0;
}

/*
return:
    but status
*/
static uint32 sfMsgCent_Button_GetStatus(void)
{
	uint16 tSendLen;
	uint8 _i = 0;
	int tRet;

	for (; _i < sDrvCap.aNumOfButton; _i++) {
		tSendLen = sfMsgCent_Union(NULL, 0,
		                           NVC_MSG_TYPE_BUTTON |
		                           NVC_MSG_BUTTON_GET_STATUS, _i, 0);
		tRet = sfMsgCent_write(tSendLen);
	}
	return 0;
}

//-------------------------------------------->LDR
extern mCapModule *gMod_LDR[];
static uint32 sfMsgCent_LDR_SetStatus(uint8 iSensi);
static int sfMsgCent_TestLDR(void)
{
	uint32 tRet;
	uint8 tSta = 0x00;

	gfMsgCent_Thread1Show = gfShow_LDRSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		gfMsgCent_UpdateModStatus(DCA_MsgCent_Update_Ldr);
		if (tSta) {
			sleep(1);
			if (getchar() > 0) {
				int tTypein;
				do {
					tTypein = getchar();
				} while ((tTypein > 0) && (tTypein != '\n'));
				tSta = 0x00;
				fcntl(0, F_SETFL, 0);
				DF_DCA_FrameUpdate_SetOnce;
			}
		} else {
			tRet = gfResolv_UserInput(gMod_LDR);
			DF_DCA_FrameUpdate_IncOnce;
			if (tRet == -2) {
				break;
			} else if (tRet > 0) {
				switch (tRet) {
				case 1: {
					uint8 tSensi;
					uint8 tStrBuf[10];
					uint8 _i = 0;
					do {
						tSensi = getchar();
						if (tSensi == '\n') {
							tStrBuf[_i] =
							    '\0';
						} else {
							tStrBuf[_i] =
							    tSensi;
						}

						_i++;
						if (_i == 10) {
							_i = 0;
						}
					} while (tSensi != '\n');
					do {
						APPPrint_r
						("Do you realy want set it to %s. (y/n)",
						 tStrBuf);
						tSensi = getchar();
						while (getchar() !=
						       '\n') ;
						printf("\e[1A");	//先回到上一行
						printf("\e[K");	//清除该行
						printf("\e[1A");	//先回到上一行
						printf("\e[K");	//清除该行
					} while ((tSensi != 'y')
					         && (tSensi != 'n'));
					if (tSensi == 'y') {
						tSensi =
						    (atoi(tStrBuf) &
						     0xFF);
						sfMsgCent_LDR_SetStatus
						(tSensi);
					}

					break;
				}
				case 2:
					tSta = 1;
					fcntl(0, F_SETFL, O_NONBLOCK);
					DF_DCA_FrameUpdate_RUN;
					break;
				}
			}
		}
	}
	return 0;
}

static uint32 sfMsgCent_LDR_GetStatus(void)
{
	int tRet;
	uint16 tSendLen;
	{
		tSendLen =
		    sfMsgCent_Union(NULL, 0,
		                    NVC_MSG_TYPE_LDR | NVC_MSG_LDR_GET_STATE, 0,
		                    0);
		tRet = sfMsgCent_write(tSendLen);
	}
	{
		tSendLen = sfMsgCent_Union(NULL, 0,
		                           NVC_MSG_TYPE_LDR |
		                           NVC_MSG_LDR_GET_SENSITIVE, 0, 0);
		tRet = sfMsgCent_write(tSendLen);
	}
	return 0;
}

static uint32 sfMsgCent_LDR_SetStatus(uint8 iSensi)
{

	int tRet;
	mNVC_DNMONITOR_SENSI tSetSta;
	uint16 tSendLen = 0;

	tSetSta.aSPoint = 4;
	tSetSta.aDomain = iSensi;

	tSendLen = sfMsgCent_Union(&tSetSta,
	                           sizeof(mNVC_DNMONITOR_SENSI),
	                           NVC_MSG_TYPE_LDR | NVC_MSG_LDR_SET_SENSITIVE,
	                           0, 0);

	//
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

//-------------------------------------------->IfrLi
extern mCapModule *gMod_IfrLi[];
static uint32 sfMsgCent_IfrLi_SetStatus(uint8 iCmd);
static int sfMsgCent_TestIfrLi(void)
{
	int tRet;
	gfMsgCent_Thread1Show = gfShow_IfrLiSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		gfMsgCent_UpdateModStatus(DCA_MsgCent_Update_IfrLi);
		tRet = gfResolv_UserInput(gMod_IfrLi);
		DF_DCA_FrameUpdate_IncOnce;
		if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			switch (tRet) {
			case 1:
				sfMsgCent_IfrLi_SetStatus(0x01);
				break;
			case 2:
				sfMsgCent_IfrLi_SetStatus(0x00);
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

static uint32 sfMsgCent_IfrLi_GetStatus(void)
{

	int tRet;
	uint16 tSendLen;
	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_IfrLIGHT |
	                           NVC_MSG_IfrLIGHT_GET_STATUS, 0, 0);
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

static uint32 sfMsgCent_IfrLi_SetStatus(uint8 iCmd)
{
	int tRet;
	mNVC_IRLIGHT_STATUS tSetSta;
	uint16 tSendLen = 0;

	if (iCmd & 0x01) {
		tSetSta.aStatus = 0x01;
	} else {
		tSetSta.aStatus = 0x00;
	}

	tSendLen = sfMsgCent_Union(&tSetSta,
	                           sizeof(mNVC_IRLIGHT_STATUS),
	                           NVC_MSG_TYPE_IfrLIGHT |
	                           NVC_MSG_IfrLIGHT_SET_STATUS, 0, 0);
	//
	tRet = sfMsgCent_write(tSendLen);

	return 0;
}

//-------------------------------------------->IRCUT
extern mCapModule *gMod_IfrFilt[];
static uint32 sfMsgCent_IRCut_SetStatus(uint8 iCmd);
static int sfMsgCent_TestIRCut(void)
{
	int tRet;
	gfMsgCent_Thread1Show = gfShow_IfrFiltSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		gfMsgCent_UpdateModStatus(DCA_MsgCent_Update_Irc);

		tRet = gfResolv_UserInput(gMod_IfrFilt);
		DF_DCA_FrameUpdate_IncOnce;
		if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			switch (tRet) {
			case 1:
				sfMsgCent_IRCut_SetStatus(0x00);
				break;
			case 2:
				sfMsgCent_IRCut_SetStatus(0x01);
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

static uint32 sfMsgCent_IRCut_GetStatus(void)
{

	int tRet;
	uint16 tSendLen;
	tSendLen =
	    sfMsgCent_Union(NULL, 0, NVC_MSG_TYPE_IRC | NVC_MSG_IRC_GET_TYPE, 0,
	                    0);
	tRet = sfMsgCent_write(tSendLen);
	tSendLen =
	    sfMsgCent_Union(NULL, 0, NVC_MSG_TYPE_IRC | NVC_MSG_IRC_GET_STATUS,
	                    0, 0);
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

static uint32 sfMsgCent_IRCut_SetStatus(uint8 iCmd)
{

	int tRet;
	mNVC_IRFILT_STATUS tSetSta;
	uint16 tSendLen = 0;

	if (iCmd & 0x01) {
		tSetSta.aStatus = 0x01;
	} else {
		tSetSta.aStatus = 0x00;
	}

	tSendLen = sfMsgCent_Union(&tSetSta,
	                           sizeof(mNVC_IRFILT_STATUS),
	                           NVC_MSG_TYPE_IRC | NVC_MSG_IRC_SET_STATUS,
	                           0, 0);
	//
	tRet = sfMsgCent_write(tSendLen);

	return 0;
}

//-------------------------------------------->DoubleLens
extern mCapModule *gMod_DobLens[];
static void sfMsgCent_DubLens_SetStatus(uint32 iCmd);

static int sfMsgCent_TestDoubleLens(void)
{
	int tRet;
	gfMsgCent_Thread1Show = gfShow_DubLensSection;
	DF_DCA_FrameUpdate_IncOnce;

	while (1) {
		tRet = gfResolv_UserInput(gMod_DobLens);
		DF_DCA_FrameUpdate_IncOnce;
		if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			switch (tRet) {
			case 1:
				sfMsgCent_DubLens_SetStatus(0x01);
				break;
			case 2:
				sfMsgCent_DubLens_SetStatus(0x02);
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

static uint32 sfMsgCent_DubLens_GetStatus(void)
{

	int tRet;
	uint16 tSendLen;
	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_DoubLENS |
	                           NVC_MSG_DoubLENS_GET_STATUS, 0, 0);
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

static void sfMsgCent_DubLens_SetStatus(uint32 iCmd)
{
	int tRet;
	uint16 tSendLen;
	mNVC_DUBLENS_SETINFO tSetIfo;

	if (iCmd & 0x01) {
		tSetIfo.aStatus = DC_DUBLENS_DAYLENS;
	}

	else if (iCmd & 0x02) {
		tSetIfo.aStatus = DC_DUBLENS_NIGHTLENS;
	}

	tSendLen = sfMsgCent_Union(&tSetIfo,
	                           sizeof(mNVC_DUBLENS_SETINFO),
	                           NVC_MSG_TYPE_DoubLENS |
	                           NVC_MSG_DoubLENS_SET_STATUS, 0, 0);
	//
	tRet = sfMsgCent_write(tSendLen);
}

//-------------------------------------------->PTZ
extern mCapModule *gMod_PTZ[];
static void sfMsgCent_PTZ_SetStatus(uint32 iCmd);
static int sfMsgCent_PTZLens(void)
{
	uint tRet;
	gfMsgCent_Thread1Show = gfShow_PTZSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		tRet = gfResolv_UserInput(gMod_PTZ);
		if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			// switch(tRet){
			// case 1: sfMsgCent_PTZ_SetStatus(0x01); break;
			// case 2: sfMsgCent_PTZ_SetStatus(0x02); break;
			// case 3: sfMsgCent_PTZ_SetStatus(0x03); break;
			// case 4: sfMsgCent_PTZ_SetStatus(0x04); break;
			// case 5: sfMsgCent_PTZ_SetStatus(0x05); break;
			// case 6: sfMsgCent_PTZ_SetStatus(0x06); break;
			// case 7: sfMsgCent_PTZ_SetStatus(0x07); break;
			// case 8: sfMsgCent_PTZ_SetStatus(0x08); break;
			// default:break;
			// }
			sfMsgCent_PTZ_SetStatus(tRet);
		}
		DF_DCA_FrameUpdate_IncOnce;
	}
	return 0;
}

static uint32 sfMsgCent_PTZ_GetStatus(void)
{

	int tRet;
	uint16 tSendLen;
	tSendLen =
	    sfMsgCent_Union(NULL, 0, NVC_MSG_TYPE_PTZ | NVC_MSG_PTZ_GET_INFO, 0,
	                    0);
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

//
//
//
#define DC_MsgCent_PZT_15Dgr_Steps      15*6
static void sfMsgCent_PTZ_SetStatus(uint32 iCmd)
{
	int tRet;
	uint16 tSendLen;

	if ((iCmd >= 1) && (iCmd <= 8) || (iCmd == 13) || (iCmd == 14)) {

		mNVC_PANTILT_SET tSetIfo;

		uint32 tH15 = 15000000;
		uint32 tV15 = 15000000;

		tSetIfo.aHParam = 0;
		tSetIfo.aVParam = 0;

		if ((iCmd >= 1) && (iCmd <= 4)) {
			tSetIfo.aParaType = 0;
			tSetIfo.aSpeed = 100;
		} else if ((iCmd >= 5) && (iCmd <= 8) || (iCmd == 13)
		           || (iCmd == 14)) {
			if ((gStaG.aPTZ.aInfo & NVC_PTZ_SUPP_HSCAN)
			    && (gStaG.aPTZ.aInfo & NVC_PTZ_SUPP_VSCAN)) {
				tSetIfo.aParaType = 2;
				tSetIfo.aSpeed = 100;
			} else {
				APPPrint_r
				("You test device dosent support this function!\r\n");

				DF_DCA_FrameUpdate_IncOnce;
			}
		}

		switch (iCmd) {
		case 1:
			tSetIfo.aCmd = NV_PTZ_LEFT;
			tSetIfo.aHParam = tH15 / gStaG.aPTZ.aXMinDgr;
			break;
		case 2:
			tSetIfo.aCmd = NV_PTZ_RIGHT;
			tSetIfo.aHParam = tH15 / gStaG.aPTZ.aXMinDgr;
			break;
		case 3:
			tSetIfo.aCmd = NV_PTZ_DOWN;
			tSetIfo.aVParam = tV15 / gStaG.aPTZ.aYMinDgr;
			break;
		case 4:
			tSetIfo.aCmd = NV_PTZ_UP;
			tSetIfo.aVParam = tV15 / gStaG.aPTZ.aYMinDgr;
			break;
		case 5:
			tSetIfo.aCmd = NV_PTZ_LEFT_DOWN;
			tSetIfo.aHParam = 0;
			tSetIfo.aVParam = 0;
			break;
		case 6:
			tSetIfo.aCmd = NV_PTZ_LEFT_UP;
			tSetIfo.aHParam = 0;
			tSetIfo.aVParam = gStaG.aPTZ.aYRange;
			break;
		case 7:
			tSetIfo.aCmd = NV_PTZ_RIGHT_DOWN;
			tSetIfo.aHParam = gStaG.aPTZ.aXRange;
			tSetIfo.aVParam = 0;
			break;
		case 8:
			tSetIfo.aCmd = NV_PTZ_RIGHT_UP;
			tSetIfo.aHParam = gStaG.aPTZ.aXRange;
			tSetIfo.aVParam = gStaG.aPTZ.aYRange;
			break;
		case 13: {
			uint8 tSetNum;
			printf("Please Input preset num!\n");
			{
				uint8 tSta = 0;
				uint8 tInC;
				uint8 tInS[10];
				uint8 _i = 0;
				do {
					tInC = getchar();
					if ((tInC >= '0')
					    && (tInC <= '9')) {
						tInS[_i] = tInC;
						_i++;
						if (_i == 10) {
							_i = 0;
						}
					} else if (tInC == '\n') {
						tInS[_i] = '\0';
						break;
					} else {
						tSta |= 0x01;
						break;
					}
				} while (1);
				if (tSta) {
					printf("Illegal input!\n");
					return;
				} else {
					tSetNum = atoi(tInS);
					if (!((tSetNum <= 16)
					      && (tSetNum >= 1))) {
						printf
						("Input num big than max limit!\n");
						return;
					}
				}
			}
			tSetIfo.aCmd = NV_PTZ_MVT_PRESET;
			tSetIfo.aNo = tSetNum;

		}
		break;
		case 14:
			tSetIfo.aCmd = NV_PTZ_STAR_CRUISE;
			break;

		}
		tSendLen = sfMsgCent_Union(&tSetIfo,
		                           sizeof(mNVC_PANTILT_SET),
		                           NVC_MSG_TYPE_PTZ |
		                           NVC_MSG_PTZ_SET_ACTION, 0, 0);
	} else if (iCmd == 9) {

		mNVC_PTCruiseUnit tSetIfo;
		uint8 tSetNum;
		printf("Please Input preset num!\n");
		{
			uint8 tSta = 0;
			uint8 tInC;
			uint8 tInS[10];
			uint8 _i = 0;
			do {
				tInC = getchar();
				if ((tInC >= '0') && (tInC <= '9')) {
					tInS[_i] = tInC;
					_i++;
					if (_i == 10) {
						_i = 0;
					}
				} else if (tInC == '\n') {
					tInS[_i] = '\0';
					break;
				} else {
					tSta |= 0x01;
					break;
				}
			} while (1);
			if (tSta) {
				printf("Illegal input!\n");
				return;
			} else {
				tSetNum = atoi(tInS);
				if (!((tSetNum <= 16) && (tSetNum >= 1))) {
					printf
					("Input num big than max limit!\n");
					return;
				}
			}
		}
		tSetIfo.aPreSetNo = tSetNum;
		tSetIfo.aSpeed = 100;
		tSetIfo.aStaySeconds = 5;
		tSendLen = sfMsgCent_Union(&tSetIfo,
		                           sizeof(mNVC_PTCruiseUnit),
		                           NVC_MSG_TYPE_PTZ |
		                           NVC_MSG_PTZ_SET_PRESET, 0, 0);
	} else if (iCmd == 10) {
		mNVC_PTCruiseUnit tSetIfo;
		uint8 tSetNum;
		printf("Please Input preset num!\n");
		{
			uint8 tSta = 0;
			uint8 tInC;
			uint8 tInS[10];
			uint8 _i = 0;
			do {
				tInC = getchar();
				if ((tInC >= '0') && (tInC <= '9')) {
					tInS[_i] = tInC;
					_i++;
					if (_i == 10) {
						_i = 0;
					}
				} else if (tInC == '\n') {
					tInS[_i] = '\0';
					break;
				} else {
					tSta |= 0x01;
					break;
				}
			} while (1);
			if (tSta) {
				printf("Illegal input!\n");
				return;
			} else {
				tSetNum = atoi(tInS);
				if (!((tSetNum <= 16) && (tSetNum >= 1))) {
					printf
					("Input num big than max limit!\n");
					return;
				}
			}
		}
		tSetIfo.aPreSetNo = tSetNum;
		tSendLen = sfMsgCent_Union(&tSetIfo,
		                           sizeof(mNVC_PTCruiseUnit),
		                           NVC_MSG_TYPE_PTZ |
		                           NVC_MSG_PTZ_CLR_PRESET, 0, 0);

	} else if (iCmd == 11) {

	} else if (iCmd == 12) {
		mNVC_PTZMvPath aPath[] = { 2, 3, 1, 4, 0 };
		tSendLen = sfMsgCent_Union(&aPath,
		                           sizeof(mNVC_PTZMvPath),
		                           NVC_MSG_TYPE_PTZ |
		                           NVC_MSG_PTZ_SET_CRUISE_PATH_REQ, 0,
		                           0);
	} else {
		return;
	}

	tRet = sfMsgCent_write(tSendLen);
	return;
}

//-------------------------------------------->StaLi
extern mCapModule *gMod_StaLi[];
static uint32 sfMsgCent_StaLi_SetStatus(uint8 iCmd);
static int sfMsgCent_TestStaLi(void)
{
	int tRet;
	gfMsgCent_Thread1Show = gfShow_StaLiSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		tRet = gfResolv_UserInput(gMod_StaLi);
		DF_DCA_FrameUpdate_IncOnce;
		if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			switch (tRet) {
			case 1:
				sfMsgCent_StaLi_SetStatus(0x01);
				break;
			case 2:
				sfMsgCent_StaLi_SetStatus(0x02);
				break;
			case 3:
				sfMsgCent_StaLi_SetStatus(0x03);
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

static uint32 sfMsgCent_StaLi_SetStatus(uint8 iCmd)
{
	uint16 tSendLen = 0;
	int tRet;
	mNVC_STALIGHT_SETINFO tSetSta;

	tSetSta.aLightMode = 1;
	switch (iCmd) {

	case 1:
		break;
	}
	switch (iCmd) {
	case 1: {
		tSetSta.aOnMes = 1000;
		tSetSta.aOffMes = 0;
		break;
	}
	case 2: {
		tSetSta.aOnMes = 0;
		tSetSta.aOffMes = 1000;
		break;
	}
	case 3: {
		tSetSta.aOnMes = 100;
		tSetSta.aOffMes = 100;
		break;
	}
	}
	tSendLen = sfMsgCent_Union(&tSetSta,
	                           sizeof(mNVC_STALIGHT_SETINFO),
	                           NVC_MSG_TYPE_StaLIGHT |
	                           NVC_MSG_StaLIGHT_SET_STATUS, 0, 0);
	//
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

//-------------------------------------------->NiLi
static uint8 sMsgCent_illeagalMsg[] = "You Input is illegal";

static void sfMsgCent_NiLi_SetStatus(uint32 iCmd);

extern mCapModule *gMod_NiLi[];
extern uint32 gShowNiLiStatus;
extern uint8 *gShowNiLiLin1Point;
extern uint8 *gShowNiLiLin2Point;
static int sfMsgCent_TestNiLi(void)
{
	int tRet;
	gShowNiLiStatus = 0;
	gfMsgCent_Thread1Show = gfShow_NiLiSection;
	gShowNiLiLin1Point = sMsgCent_illeagalMsg;
	gShowNiLiLin2Point = sMsgCent_illeagalMsg;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		//jin
		tRet = gfResolv_UserInput(gMod_NiLi);
		DF_DCA_FrameUpdate_IncOnce;
		if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			switch (tRet) {
			case 1:
				sfMsgCent_NiLi_SetStatus(0x01);
				break;
			case 2:
				sfMsgCent_NiLi_SetStatus(0x02);
				break;
			default:
				gShowNiLiStatus = 0x80030000;
				DF_DCA_FrameUpdate_IncOnce;
				break;
			}
		}
		sleep(1);
	}
	return 0;
}

static uint32 sfMsgCent_NiLi_GetStatus(void)
{

	int tRet;
	uint16 tSendLen;
	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_NitLIGHT |
	                           NVC_MSG_NitLIGHT_GET_STATUS, 0, 0);
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

/*
    iCmd
    该参数为该方法选择设置项
    0x01 代表将灯关闭
    0x02 代表将灯打开
    如果输入其他值，将不予理会

    输入第三项的时候(也就是1),会要用户设置一个设置值
    区间在 0-100 之间
    0           的时候相当于灯是关闭的状态
    1-100       之间时小夜灯将会呈现一个相应的亮度状态
*/
static void sfMsgCent_NiLi_SetStatus(uint32 iCmd)
{
	uint16 tSendLen;
	int tRet;
	mNVC_NIGHTLIGHT_STATUS tSetIfo;

	if (iCmd == 1) {
		uint8 tSetLevel;
		uint8 tLevelCharBuf[10];
		uint8 tLevelBufoint = 0;
		gShowNiLiStatus = 0x80020001;
		DF_DCA_FrameUpdate_IncOnce;
		do {
			tLevelCharBuf[tLevelBufoint] = getchar();
			if (tLevelCharBuf[tLevelBufoint] == '\n') {
				tLevelCharBuf[tLevelBufoint] = '\0';
				break;
			}
			tLevelBufoint++;
			if (tLevelBufoint >= 10) {
				tLevelBufoint = 0;
			}
		} while (1);

		tSetLevel = (atoi(tLevelCharBuf) & 0xFF);

		if (tSetLevel > 100) {
			gShowNiLiStatus = 0x80320001;
			DF_DCA_FrameUpdate_IncOnce;
			return;
		}
		tSetIfo.aLemLevel = tSetLevel;
		tSetIfo.aStatus = DC_NIGHTLIGHT_On;

		gShowNiLiStatus = 0x80220001 | (tSetLevel << 8);
		DF_DCA_FrameUpdate_IncOnce;

	} else if (iCmd == 2) {
		gShowNiLiStatus = 0x80020002;
		DF_DCA_FrameUpdate_IncOnce;
		tSetIfo.aStatus = DC_NIGHTLIGHT_Off;
	}

	tSendLen = sfMsgCent_Union(&tSetIfo,
	                           sizeof(mNVC_NIGHTLIGHT_STATUS),
	                           NVC_MSG_TYPE_NitLIGHT |
	                           NVC_MSG_NitLIGHT_SET_STATUS, 0, 0);
	//
	tRet = sfMsgCent_write(tSendLen);
}

//-------------------------------------------->AudioPlug
extern mCapModule *gMod_AudioPlug[];
static uint32 sfMsgCent_AuPl_SetStatus(uint32 iCmd);
//
static int sfMsgCent_TestAudioPlug(void)
{
	int tRet;
	uint8 tSta = 0x00;
	gfMsgCent_Thread1Show = gfShow_AudioPlugSection;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		gfMsgCent_UpdateModStatus(DCA_MsgCent_Update_AuPl);
		if (tSta) {
			uint8 tChar;
			sleep(1);
			tChar = getchar();
			if (tChar > 0) {
				while (tChar != '\n') {
					tChar = getchar();
				}
				tSta = 0x00;
				fcntl(0, F_SETFL, 0);
				DF_DCA_FrameUpdate_SetOnce;
			}
		} else {
			tRet = gfResolv_UserInput(gMod_AudioPlug);
			DF_DCA_FrameUpdate_IncOnce;
			if (tRet == -2) {
				break;
			} else if (tRet > 0) {
				switch (tRet) {
				case 1:
					sfMsgCent_AuPl_SetStatus(0x01 | 0x02);
					break;
				case 2:
					sfMsgCent_AuPl_SetStatus(0x00 | 0x02);
					break;
				case 3:
					sfMsgCent_AuPl_SetStatus(0x01 | 0x04);
					break;
				case 4:
					sfMsgCent_AuPl_SetStatus(0x00 | 0x04);
					break;
				case 5:
					tSta = 1;
					fcntl(0, F_SETFL, O_NONBLOCK);
					DF_DCA_FrameUpdate_RUN;
					break;
				default:
					break;
				}
			}
		}

	}
	return 0;
}

/*
return:
    bit0:   0:OFF   1:ON Speaker
    bit1:   0:OFF   1:ON Microphone
    bit2:   0:Support   1:NOT Support!  Speaker
    bit3:   0:Support   1:NOT Support!  Microphone
*/
static uint32 sfMsgCent_AuPl_GetStatus(void)
{
	uint16 tSendLen;
	int tRet;

	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_AudioPLGU |
	                           NVC_MSG_SPEAKER_GET_STATUS, 0, 0);
	tRet = sfMsgCent_write(tSendLen);

	tSendLen = sfMsgCent_Union(NULL, 0,
	                           NVC_MSG_TYPE_AudioPLGU |
	                           NVC_MSG_MICROPH_GET_STATUS, 0, 0);
	tRet = sfMsgCent_write(tSendLen);

	return 0;
}

/*
iCmd:
    Bit0:   0:OFF   1:ON
    Bit1:   1:Speaker
    Bit2:   1:Microphone
*/
static uint32 sfMsgCent_AuPl_SetStatus(uint32 iCmd)
{
	int tRet;
	mNVC_AUDIOPLUG_Msg tSta;
	uint16 tSendLen = 0;

	if (iCmd & 0x01) {
		tSta.aStatus = 0x01;
	} else {
		tSta.aStatus = 0;
	}

	if (iCmd & 0x02) {
		tSendLen = sfMsgCent_Union(&tSta,
		                           sizeof(mNVC_AUDIOPLUG_Msg),
		                           NVC_MSG_TYPE_AudioPLGU |
		                           NVC_MSG_SPEAKER_SET_STATUS, 0, 0);
	} else if (iCmd & 0x04) {
		tSendLen = sfMsgCent_Union(&tSta,
		                           sizeof(mNVC_AUDIOPLUG_Msg),
		                           NVC_MSG_TYPE_AudioPLGU |
		                           NVC_MSG_MICROPH_SET_STATUS, 0, 0);
	}
	tRet = sfMsgCent_write(tSendLen);
	return 0;
}

//----------------------------------------------------------->local function
static void sfMsgCent_ShowCap(mNVC_DRV_CAP_INFO *iCap)
{
	uint32 tCapMask = iCap->aDrvCapMask;
	uint8 _i = 0;

	APPPrint_b("--------------- DRIVER CAPB ---------------");
	if (tCapMask & CAP_SUPP_ButtonMonitor) {
		APPPrint_b("Support: Button(number:%d)",
		           (int)iCap->aNumOfButton);
		gLinkModule[_i++] = &sButtonModule;
		sButtonModule.aNum = _i;
	}

	if (tCapMask & CAP_SUPP_LdrMonitor) {
		APPPrint_b("Support: Day&Night-Detect");
		gLinkModule[_i++] = &sLDRModule;
		sLDRModule.aNum = _i;
	}

	if (tCapMask & CAP_SUPP_Ircut) {
		APPPrint_b("Support: Infrared-filter");
		gLinkModule[_i++] = &sIRCUTModule;
		sIRCUTModule.aNum = _i;
	}

	if (tCapMask & CAP_SUPP_IfrLamp) {
		APPPrint_b("Support: Infrared-Light");
		gLinkModule[_i++] = &sIfrLiModule;
		sIfrLiModule.aNum = _i;
	}

	if (tCapMask & CAP_SUPP_DoubleLens) {
		APPPrint_b("Support: Double-Lens");
		gLinkModule[_i++] = &sDoubLensModule;
		sDoubLensModule.aNum = _i;
	}

	if (tCapMask & CAP_SUPP_StateLed) {
		APPPrint_b("Support: State-LED(number:%d)",
		           (int)iCap->aNumOfStaLED);
		gLinkModule[_i++] = &sStaLiModule;
		sStaLiModule.aNum = _i;
	}
	if (tCapMask & CAP_SUPP_PTZ) {
		APPPrint_b("Support: Pan&Tilt");
		gLinkModule[_i++] = &sPTZModule;
		sPTZModule.aNum = _i;
	}
	if (tCapMask & CAP_SUPP_NightLight) {
		APPPrint_b("Support: Night-Light");
		gLinkModule[_i++] = &sNiLiModule;
		sNiLiModule.aNum = _i;
	}
	if (tCapMask & NVC_SUPP_CoolFan) {
		APPPrint_b("Support: Cool-Fan");
	}
	if (tCapMask & CAP_SUPP_AudioPlug) {
		APPPrint_b("Support: Audio-Plug");
		gLinkModule[_i++] = &sAuPlModule;
		sAuPlModule.aNum = _i;
	}
	if (tCapMask & CAP_SUPP_TempMonitor) {
		APPPrint_b("Support: Temperature-monitor");
		gLinkModule[_i++] = &sTempModule;
		sTempModule.aNum = _i;
	}
	if (tCapMask & CAP_SUPP_HumiMonitor) {
		APPPrint_b("Support: Humidity-monitor");
		gLinkModule[_i++] = &sHumiModule;
		sHumiModule.aNum = _i;
	}
	if (tCapMask & CAP_SUPP_GpioReset) {
		APPPrint_b("Support: GPIO-Reset");
		gLinkModule[_i++] = &sGPIOResetModule;
		sGPIOResetModule.aNum = _i;
	}
	if (tCapMask & CAP_SUPP_RTC) {
		APPPrint_b("Support: RTC");
		gLinkModule[_i++] = &sRTCModule;
		sRTCModule.aNum = _i;
	}
	gLinkModule[_i++] == NULL;

}

static int32 sfMsgCent_Union(void *iMsg,
                             uint16 iMsgLen,
                             uint16 iCmd, uint8 iUnit, uint8 iErr)
{

	uint8 *tTar = sDrvSend;
	uint8 *tSou = iMsg;
	uint16 tMagicWord = DC_Protocal_MagicWord;

	strncpy(tTar, (uint8 *) & tMagicWord, 2);
	tTar += 2;

	strncpy(tTar, (uint8 *) & iCmd, 2);
	tTar += 2;

	strncpy(tTar, (uint8 *) & iMsgLen, 2);
	tTar += 2;

	strncpy(tTar, (uint8 *) & iUnit, 1);
	tTar++;

	strncpy(tTar, (uint8 *) & iErr, 1);
	tTar++;

	memset(tTar, 0x00, 4);
	tTar += 4;

	if (iMsg != NULL) {
		gfStr_Cpy(tTar, tSou, iMsgLen);
		tTar += iMsgLen;
	}

	return (int32)((void *)tTar - (void *)sDrvSend);
}

static int sMsgCent_LogExpress(uint8 iCmd, void *iMsg, uint8 iLen);
static int sfMsgCent_write(uint16 iLen)
{
	uint32 tLen;
	tLen = sMsgCent_LogExpress(0x01, sDrvSend, iLen);
	return write(sDrvHandler, sDrvSend, iLen);
}

static int sfMsgCent_read(void)
{
	int tLen = read(sDrvHandler, sDrvRcv, DC_Protocal_MaxMesgLen);
	sMsgCent_LogExpress(0x00, sDrvRcv, tLen);
	return tLen;
}

/*
iCmd
    0x01       send
    0x00        reciv

    WARNING :
    LOG     :
    ERROR   : (ErrCode) (time) (MsgType) (Unit) (Data)

*/
// extern uint8 *gfStr_Cpy(uint8 *iDest,void *iSrc,uint16 ilen);
// extern uint8 *gfStr_Ist(uint8 *iDest,uint8 *iSrc);

extern unsigned char sgStandCharBlack[];
static int sMsgCent_LogExpress(uint8 iCmd, void *iMsg, uint8 iLen)
{
	uint8 iTar[1024];
	uint8 tBuf[10];
	uint8 *tpU8;
	uint16 *tpU16;
	uint32 *tpU32;
	uint8 *tTar = iTar;
	{
		// uint8   *tT;
		time_t timep;
		time(&timep);
		tTar = gfStr_Cpy(tTar, asctime(gmtime(&timep)), 24);
		*tTar++ = '\t';
	}
	if (iCmd & 0x01) {
		tTar = gfStr_Cpy(tTar, "SEND:   ", 8);
	} else {
		tTar = gfStr_Cpy(tTar, "RECIVE: ", 8);
	}
	tpU16 = iMsg;
	gfStr_itoa(*tpU16, tBuf, 16, 10);
	tTar = gfStr_Ist(tTar, tBuf);
	*tTar++ = '\t';

	tpU16 = iMsg + 2;
	gfStr_itoa(*tpU16, tBuf, 16, 10);
	tTar = gfStr_Ist(tTar, tBuf);
	*tTar++ = '\t';

	tpU16 = iMsg + 4;
	gfStr_itoa(*tpU16, tBuf, 10, 10);
	tTar = gfStr_Ist(tTar, tBuf);
	*tTar++ = '\t';

	tpU8 = iMsg + 6;
	gfStr_itoa(*tpU8, tBuf, 10, 10);
	tTar = gfStr_Ist(tTar, tBuf);
	*tTar++ = '\t';

	tpU8 = iMsg + 7;
	gfStr_itoa(*tpU8, tBuf, 10, 10);
	tTar = gfStr_Ist(tTar, tBuf);
	*tTar++ = '\n';

	tTar = gfStr_Cpy(tTar, "MsgBody:\n", 9);
	if (iLen == 12) {
		tTar = gfStr_Cpy(tTar, "none\n\n", 6);
	} else {
		uint8 _i;
		tpU8 = iMsg + 12;
		for (_i = 0; _i < (iLen - 12); _i++) {
			*tTar++ = sgStandCharBlack[(*tpU8 & 0xF0) >> 4];
			*tTar++ = sgStandCharBlack[*tpU8 & 0xF];
			tpU8++;
			if ((_i + 1) % 20 == 0) {
				*tTar++ = '\n';
			} else if ((_i + 1) % 10 == 0) {
				*tTar++ = '\t';
			} else {
				*tTar++ = ' ';
			}
		}
		*tTar++ = '\n';
		*tTar++ = '\n';
	}
	*tTar = '\0';

	{
		FILE *tF = fopen(DCA_DriverLog1_Path, "a+");
		fwrite(iTar, tTar - iTar, 1, tF);
		fclose(tF);
	}
	return (tTar - iTar);
}

// =============================================================================
// ----------------------------------------------------------->Gethring Statu
/*
    0xFFFFFFFF 获取全局状态信息
    1   更新 device 信息
    2   更新 button 信息
    3   更新 LDR 信息
    4   更新 IRC 信息
    5   更新 Ifrared Light 信息

    7   更新 PTZ 信息
    8   更新 NiLi 信息
    9   更新 Audio Plug 信息
    10  更新 Temp Monitor 信息
    11  更新 Humi Monitor 信息
    12  更新 Double Lens 信息
    13  更新 RTC 信息
*/
void gfMsgCent_UpdateModStatus(uint32 iCmd)
{

	if (iCmd == 0xFFFFFFFF) {
		sfMsgCent_Device_GetStatus();
		if (sDrvCap.aDrvCapMask & CAP_SUPP_ButtonMonitor) {
			sfMsgCent_Button_GetStatus();
		}
		if (sDrvCap.aDrvCapMask & CAP_SUPP_LdrMonitor) {
			sfMsgCent_LDR_GetStatus();
		}
		if (sDrvCap.aDrvCapMask & CAP_SUPP_Ircut) {
			sfMsgCent_IRCut_GetStatus();
		}
		if (sDrvCap.aDrvCapMask & CAP_SUPP_IfrLamp) {
			sfMsgCent_IfrLi_GetStatus();
		}
		if (sDrvCap.aDrvCapMask & CAP_SUPP_DoubleLens) {
			sfMsgCent_DubLens_GetStatus();
		}

		if (sDrvCap.aDrvCapMask & CAP_SUPP_PTZ) {
			sfMsgCent_PTZ_GetStatus();
		}
		if (sDrvCap.aDrvCapMask & CAP_SUPP_NightLight) {
			sfMsgCent_NiLi_GetStatus();
		}

		if (sDrvCap.aDrvCapMask & CAP_SUPP_AudioPlug) {
			sfMsgCent_AuPl_GetStatus();
		}

		if (sDrvCap.aDrvCapMask & CAP_SUPP_TempMonitor) ;
		if (sDrvCap.aDrvCapMask & CAP_SUPP_HumiMonitor) ;
		if (sDrvCap.aDrvCapMask & CAP_SUPP_RTC) ;

	} else {
		switch (iCmd) {
		case 1:
			sfMsgCent_Device_GetStatus();
			break;
		case 2:
			sfMsgCent_Button_GetStatus();
			break;
		case 3:
			sfMsgCent_LDR_GetStatus();
			break;
		case 4:
			sfMsgCent_IRCut_GetStatus();
			break;
		case 5:
			sfMsgCent_IfrLi_GetStatus();
			break;
		case 7:
			sfMsgCent_PTZ_GetStatus();
			break;
		case 8:
			sfMsgCent_NiLi_GetStatus();
			break;
		case 9:
			sfMsgCent_AuPl_GetStatus();
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			sfMsgCent_DubLens_GetStatus();
			break;
		case 13:
			break;
		default:
			break;
		}
	}

}

// =======================================================================Thread
#include <sys/types.h>
#include <sys/times.h>
#include <sys/select.h>
static void sfMsgCent_ReadExpress(void);

uint32 gMsgCentStatus = 0;

void *sfMsgCent_ReadDeal(void *args)
{

	int tRet;
	struct timeval tTOut;
	fd_set tR_FdSet;
	tTOut.tv_sec = 0;
	tTOut.tv_usec = 1000;
	while (1) {
		while (1) {
			FD_ZERO(&tR_FdSet);
			FD_SET(sDrvHandler, &tR_FdSet);
			tRet = select(sMaxHd, &tR_FdSet, NULL, NULL, &tTOut);
			if (tRet < 0) {
				break;
			} else if (tRet > 0) {
				if (FD_ISSET(sDrvHandler, &tR_FdSet)) {
					tRet = sfMsgCent_read();
					if (tRet < 0) {
						break;
					}
					sfMsgCent_ReadExpress();
				}
			} else {
				break;
			}
		}
		if (gShowFrameControler) {
			gShowFrameControler--;
			if (gfMsgCent_Thread1Show != NULL) {
				gShow_ClrShowArea();
				gfMsgCent_Thread1Show();
			}
		}
		if (gMsgCentStatus & DCA_MsgCent_MOD_SHOW) {
			gfMsgCent_UpdateModStatus(DCA_MsgCent_Update_ALL);
		}

		sleep(1);
	}
	return 0;
}

// =============================================================================
// -----------------------------------------------------------> local static function
static void sfMsgCent_GetMsgHead(mNVMsg_Head *iTar);
static void sfMsgCent_ReadExpress(void)
{
	uint16 tMsgType;
	mNVMsg_Head tMsgHead;
	sfMsgCent_GetMsgHead(&tMsgHead);
	if (tMsgHead.aMagicWord != DC_Protocal_MagicWord) {
		// WARNING 接收到非法数据 记录数据到 log
		return;
	}
	/*记录错误信息 数据如果出错，直接记录，不需要再解析 */
	if (tMsgHead.aErr) {
		goto sfMsgCent_ReadExpress_ErrEnd;
		//红色警告
		return;
	}
	/*记录 Unit */

	/*接收数据类型 */
	tMsgType = tMsgHead.aMType << 8;
	switch (tMsgType) {
	case NVC_MSG_TYPE_DEVICE: {
		if (tMsgHead.aSType == NVC_MSG_DEVICE_GET_INFO_RESP) {
			sDrvInfo =
			    *(mNVC_DRV_Ver_INFO *)(sDrvRcv +
			                           DC_Protocal_MsgHeaderSize);
			gStaG.aStatus |= DCA_StaG_Dvc_UpD;
		} else if (tMsgHead.aSType ==
		           NVC_MSG_DEVICE_GET_CAP_RESP) {
			sDrvCap =
			    *(mNVC_DRV_CAP_INFO *)(sDrvRcv +
			                           DC_Protocal_MsgHeaderSize);
			gStaG.aStatus |= DCA_StaG_Cap_UpD;
		} else if (tMsgHead.aSType ==
		           NVC_MSG_DEVICE_SUB_REPORT_RESP) {

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	case NVC_MSG_TYPE_BUTTON: {
		mNVC_BUTTON_STATUS *tSta;
		if ((tMsgHead.aSType == NVC_MSG_BUTTON_GET_STATUS_RESP)
		    || (tMsgHead.aSType ==
		        NVC_MSG_BUTTON_REPORT_STATUS)) {
			tSta =
			    (mNVC_BUTTON_STATUS *)(sDrvRcv +
			                           DC_Protocal_MsgHeaderSize);
			if (tSta->aStatus & DC_NVBUTT_ButtonLow) {
				gStaG.aBut[tMsgHead.aUint].aStatus =
				    DCA_But_Down;
			} else {
				gStaG.aBut[tMsgHead.aUint].aStatus =
				    DCA_But_Up;
			}

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	case NVC_MSG_TYPE_LDR: {
		if (tMsgHead.aSType == NVC_MSG_LDR_GET_STATE_RESP) {
			mNVC_DNMONITOR_STATUS *tGetSta;
			tGetSta =
			    (mNVC_DNMONITOR_STATUS *)(sDrvRcv +
			                              DC_Protocal_MsgHeaderSize);
			gStaG.aLDR.aStatus = tGetSta->aStatus;
			gStaG.aLDR.aJudgeValue = tGetSta->aVal;
		} else if (tMsgHead.aSType ==
		           NVC_MSG_LDR_SET_SENSITIVE_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_LDR_GET_SENSITIVE_RESP) {
			mNVC_DNMONITOR_SENSI *tGetSta;
			tGetSta =
			    (mNVC_DNMONITOR_SENSI *)(sDrvRcv +
			                             DC_Protocal_MsgHeaderSize);
			gStaG.aLDR.aSPoint = tGetSta->aSPoint;
			gStaG.aLDR.aSDomain = tGetSta->aDomain;
		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	case NVC_MSG_TYPE_IRC: {
		if (tMsgHead.aSType == NVC_MSG_IRC_GET_TYPE_RESP) {
			mNVC_IRFILT_TYPE *tGetSta;
			tGetSta =
			    (mNVC_IRFILT_TYPE *)(sDrvRcv +
			                         DC_Protocal_MsgHeaderSize);
			gStaG.aIfF.aType = tGetSta->aType;
		} else if (tMsgHead.aSType ==
		           NVC_MSG_IRC_SET_STATUS_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_IRC_GET_STATUS_RESP) {
			mNVC_IRFILT_STATUS *tGetSta;
			tGetSta =
			    (mNVC_IRFILT_STATUS *)(sDrvRcv +
			                           DC_Protocal_MsgHeaderSize);
			gStaG.aIfF.aStatus = tGetSta->aStatus;
		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	case NVC_MSG_TYPE_IfrLIGHT: {
		if (tMsgHead.aSType == NVC_MSG_IfrLIGHT_SET_STATUS_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_IfrLIGHT_GET_STATUS_RESP) {
			mNVC_IRLIGHT_STATUS *tGetSta;
			tGetSta =
			    (mNVC_IRLIGHT_STATUS *)(sDrvRcv +
			                            DC_Protocal_MsgHeaderSize);
			gStaG.aIfL.aStatus = tGetSta->aStatus;
		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	case NVC_MSG_TYPE_StaLIGHT: {
		if (tMsgHead.aSType == NVC_MSG_StaLIGHT_SET_STATUS_RESP) {

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}

	}
	break;
	case NVC_MSG_TYPE_PTZ: {
		if (tMsgHead.aSType == NVC_MSG_PTZ_GET_INFO_RESP) {
			mNVC_PANTILT_INFO *tGetInfo;
			tGetInfo =
			    (mNVC_PANTILT_INFO *)(sDrvRcv +
			                          DC_Protocal_MsgHeaderSize);
			gStaG.aPTZ.aInfo = tGetInfo->aCmdMask;

			gStaG.aPTZ.aXMinDgr = tGetInfo->aDgrPerHSteps;
			gStaG.aPTZ.aYMinDgr = tGetInfo->aDgrPerVSteps;
			gStaG.aPTZ.aXRange = tGetInfo->aHRange;
			gStaG.aPTZ.aYRange = tGetInfo->aVRange;
			gStaG.aPTZ.aXPos = tGetInfo->aXPos;
			gStaG.aPTZ.aYPos = tGetInfo->aYPos;

		} else if (tMsgHead.aSType ==
		           NVC_MSG_PTZ_SET_ACTION_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_PTZ_SET_PRESET_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_PTZ_CLR_PRESET_RESP) {

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	case NVC_MSG_TYPE_NitLIGHT: {
		if (tMsgHead.aSType == NVC_MSG_NitLIGHT_SET_STATUS_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_NitLIGHT_GET_STATUS_RESP) {
			mNVC_NIGHTLIGHT_STATUS *tGetInfo;
			tGetInfo =
			    (mNVC_NIGHTLIGHT_STATUS *)(sDrvRcv +
			                               DC_Protocal_MsgHeaderSize);
			gStaG.aNiL.aStatus = tGetInfo->aStatus;
			gStaG.aNiL.aLevel = tGetInfo->aLemLevel;

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}

	}
	break;
	case NVC_MSG_TYPE_AudioPLGU: {
		mNVC_AUDIOPLUG_Msg *tSta;
		// APPPrint_r("NVC_MSG_SPEAKER_GET_STATUS_RESP\n");
		if (tMsgHead.aSType == NVC_MSG_SPEAKER_SET_STATUS_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_SPEAKER_GET_STATUS_RESP) {
			// ==========

			// APPPrint_r("NVC_MSG_SPEAKER_GET_STATUS_RESP\n");

			if (tSta->aStatus == DC_NVAuPl_On) {
				gStaG.aAuP.aStatus |=
				    DCA_AuP_Speaker_On;
			} else if (tSta->aStatus == DC_NVAuPl_NOSUPP) {
				gStaG.aAuP.aStatus |=
				    DCA_AuP_Speaker_NoSupp;
			} else {
				gStaG.aAuP.aStatus &=
				    ~DCA_AuP_Speaker_On;
			}

		} else if (tMsgHead.aSType ==
		           NVC_MSG_MICROPH_SET_STATUS_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_MICROPH_GET_STATUS_RESP) {
			// ==========
			// APPPrint_r("NVC_MSG_MICROPH_GET_STATUS_RESP\n");
			if (tSta->aStatus == DC_NVAuPl_On) {
				gStaG.aAuP.aStatus |=
				    DCA_AuP_Microphone_On;
			} else if (tSta->aStatus == DC_NVAuPl_NOSUPP) {
				gStaG.aAuP.aStatus |=
				    DCA_AuP_Microphone_NoSupp;
			} else {
				gStaG.aAuP.aStatus &=
				    ~DCA_AuP_Microphone_On;
			}
		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}

	}
	break;
	case NVC_MSG_TYPE_TempMONITOR: {
		if (tMsgHead.aSType ==
		    NVC_MSG_TempMONITOR_SUB_REPORT_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_TempMONITOR_GET_VALUE_RESP) {
			mNVC_TEMPMONITOR_Value *tGetInfo;
			tGetInfo =
			    (mNVC_TEMPMONITOR_Value *)(sDrvRcv +
			                               DC_Protocal_MsgHeaderSize);
			gStaG.aEnv.aTemp = tGetInfo->aValue;

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}

	}
	break;
	case NVC_MSG_TYPE_HumiMONITOR: {
		if (tMsgHead.aSType ==
		    NVC_MSG_HumiMONITOR_SUB_REPORT_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_HumiMONITOR_GET_VALUE_RESP) {
			mNVC_HUMIDITY_Value *tGetInfo;
			tGetInfo =
			    (mNVC_HUMIDITY_Value *)(sDrvRcv +
			                            DC_Protocal_MsgHeaderSize);
			gStaG.aEnv.aHumi = tGetInfo->aValue;

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}

	}
	break;
	case NVC_MSG_TYPE_DoubLENS: {
		if (tMsgHead.aSType == NVC_MSG_DoubLENS_SET_STATUS_RESP) {

		} else if (tMsgHead.aSType ==
		           NVC_MSG_DoubLENS_GET_STATUS_RESP) {
			mNVC_DUBLENS_SETINFO *tGetInfo;
			tGetInfo =
			    (mNVC_DUBLENS_SETINFO *)(sDrvRcv +
			                             DC_Protocal_MsgHeaderSize);
			gStaG.aDbL.aStatus = tGetInfo->aStatus;

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}

	}
	break;
	case NVC_MSG_TYPE_RTC: {
		if (tMsgHead.aSType == NVC_MSG_RTC_SET_TIME_RESP) {

		} else if (tMsgHead.aSType == NVC_MSG_RTC_GET_TIME_RESP) {
			mNVC_RTC_DATA *tGetInfo;
			tGetInfo =
			    (mNVC_RTC_DATA *)(sDrvRcv +
			                      DC_Protocal_MsgHeaderSize);
			gStaG.aRTC.aScd = tGetInfo->aSecond;
			gStaG.aRTC.aMnu = tGetInfo->aMinute;
			gStaG.aRTC.aHur = tGetInfo->aHour;
			gStaG.aRTC.aDay = tGetInfo->aDay;
			gStaG.aRTC.aWkD = tGetInfo->aWeekday;
			gStaG.aRTC.aMth = tGetInfo->aMonth;
			gStaG.aRTC.aYea = tGetInfo->aYear;

		} else {
			goto sfMsgCent_ReadExpress_ErrEnd;
		}
	}
	break;
	default:
		goto sfMsgCent_ReadExpress_ErrEnd;
		break;
	}

	return;
sfMsgCent_ReadExpress_ErrEnd:

	return;
}

static void sfMsgCent_GetMsgHead(mNVMsg_Head *iTar)
{
	iTar->aMagicWord = *((uint16 *)(sDrvRcv + 0));
	iTar->aSType = *((uint8 *)(sDrvRcv + 2));
	iTar->aMType = *((uint8 *)(sDrvRcv + 3));
	iTar->aLen = *((uint16 *)(sDrvRcv + 4));
	iTar->aUint = *((uint8 *)(sDrvRcv + 6));
	iTar->aErr = *((uint8 *)(sDrvRcv + 7));
}
