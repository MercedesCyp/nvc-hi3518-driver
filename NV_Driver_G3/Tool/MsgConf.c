/*
Coder:      aojie.meng
Date:       2015-8-15

Abstract:
    1. 在驱动初始化的时候，先会初始化好硬件，然后根据输入不同的机型计算出设备的能力集，
    2. 初始化消息响应处理中心的任务（ 也就是这个文件的作用 ），应该在获取能力集后
    3. 初始化方法先会通过能力集来计算出驱动要挂载的模块数量，
        然后生成挂载这些消息响应处理模块的静态链表
        然后根据能力集把模块一个个配置好
    4. 模块的申明在 MsgCent.c 文件中，模块的入口函数类型统一为类型
        int32 	(*aProcessing)(mRcvParam*);
        return
        0	   代表消息响应成功，并已经生成好回应的消息
        -1    该模块不存在待处理的消息类型
        -2    消息在处理的时候，与硬件相关的接口出现报错
        -1    消息在处理的时候，组建消息出错
*/
//==============================================================================
// C
// Linux
// local
#include "MsgCenter.h"
#include "MsgProtocal.h"
// remote
#include "../CTA/CTA.h"

//==============================================================================
//extern
/*
    申明模块处理消息入口
    这些函数主要是实现 MsgCenter.c文件中
*/
extern int32 sfMsgCent_Device_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_Button_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_LDR_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_InfraredFilter_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_IfrLight_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_DoubleLens_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_StateLight_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_PTZ_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_NightLight_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_AudioPlug_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_TempMonitor_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_HumidityMonitor_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_RTC_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_PIR_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_DoorBell_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_DoorLock_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_FM1288_Processing(mRcvParam *iParam);
extern int32 sfMsgCent_EAS_Processing(mRcvParam *iParam);
/*
    某些模块的回调函数
*/
extern void gfMsgCent_Report_LDR(uint8);
extern void gfMsgCent_Report_IRC(uint32);
extern void gfMsgCent_Report_Button(mButton_IntBackParam *);
extern void gfMsgCent_Report_PIR(uint32 iStatus);
extern void gfMsgCent_Report_EAS(void);
//local
//global

//=============================================================================
//extern
//local
//global
mMsgModule *sMsgModuleList = NULL;
uint8 sMsgModuleNum = 0;

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int32 sfMsgConf_StartConfig(uint32 iCapMask)
@introduction:
    挂载消息处理模块
    配置函数
    iCapMask = NVCDriverCap.aDrvCapMask;

@parameter:
    iCapMask
        每个方法模块的 Mask bit

@return:
    0

*/
int32 sfMsgConf_StartConfig(uint32 iCapMask)
{

	uint8 tCapNum = 0;
	uint32 _i;

	for (_i = 0x01; _i; _i <<= 1)
		if (iCapMask & _i) {
			tCapNum++;
		}

	tCapNum += 1;		// The Device module
	sMsgModuleList = (mMsgModule *) kmalloc(sizeof(mMsgModule) * tCapNum,
	                                        GFP_ATOMIC);
	// if( sMsgModuleList == NULL){
	// return -1;
	// }
	sMsgModuleNum = tCapNum;

	tCapNum = 0;
	// ---------------------------------------->mount device module
	sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_DEVICE;
	//device module function enter
	sMsgModuleList[tCapNum].aProcessing = sfMsgCent_Device_Processing;
	tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Device module. Ok!");
#endif

	if (iCapMask & CAP_SUPP_ButtonMonitor) {
		// 初始化功能模块的主消息类型
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_BUTTON;
		// 初始化模块收到应用层请求消息的处理函数
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_Button_Processing;
		// 初始化订阅回调函数（只有存在上报机制的某些机型才有此设置）
		gSysCTA->apButton->afRegCallBack(gfMsgCent_Report_Button);
		// 等等
		// 该机型的总模块数++
		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Button module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_LdrMonitor) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_LDR;
		sMsgModuleList[tCapNum].aProcessing = sfMsgCent_LDR_Processing;

		gSysCTA->apLDR->afRegCallBack(gfMsgCent_Report_LDR);
		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load LDR module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_Ircut) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_IRC;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_InfraredFilter_Processing;

		gSysCTA->apIfrFilter->afRegCallBack(gfMsgCent_Report_IRC);
		//printk("MsgCenter: ****()*********** count\n");
		tCapNum++;
		//printk("MsgCenter: ****()*********** tCapNum = %d\n", tCapNum);
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load IRC module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_IfrLamp) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_IfrLIGHT;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_IfrLight_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Ifr_light module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_DoubleLens) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_DoubLENS;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_DoubleLens_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Doub_Lens module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_StateLed) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_StaLIGHT;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_StateLight_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Sta_Light module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_PTZ) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_PTZ;
		sMsgModuleList[tCapNum].aProcessing = sfMsgCent_PTZ_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load PTZ module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_NightLight) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_NitLIGHT;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_NightLight_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Night_Light module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_AudioPlug) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_AudioPLGU;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_AudioPlug_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Audio_Plug module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_TempMonitor) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_TempMONITOR;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_TempMonitor_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Temp_Monitor module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_HumiMonitor) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_HumiMONITOR;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_HumidityMonitor_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load Humi_Monitor module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_RTC) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_RTC;
		sMsgModuleList[tCapNum].aProcessing = sfMsgCent_RTC_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load RTC module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_PIR) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_PIR;
		sMsgModuleList[tCapNum].aProcessing = sfMsgCent_PIR_Processing;

		gSysCTA->apPIR->afRegCallBack(gfMsgCent_Report_PIR);

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load PIR module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_DoorBell) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_DoorBell;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_DoorBell_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load DoorBell module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_DoorLock) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_DoorLock;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_DoorLock_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load DoorLock module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_FM1288) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_FM1288;
		sMsgModuleList[tCapNum].aProcessing =
		    sfMsgCent_FM1288_Processing;

		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load FM1288 module. Ok!");
#endif
	}

	if (iCapMask & CAP_SUPP_EAS) {
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_EAS;
		sMsgModuleList[tCapNum].aProcessing = sfMsgCent_EAS_Processing;

		gSysCTA->apEAS->afRegCallBack(gfMsgCent_Report_EAS);
		tCapNum++;
#if DEBUG_INIT
		NVCPrint("MsgConf:   Load EAS module. Ok!");
#endif
	}

	return 0;
}

//---------- ---------- ---------- ----------
/*  void sfMsgConf_ClrConfig(void)
@introduction:
    释放挂载消息处理模块时申请的资源

@parameter:
    void

@return:
    void

*/
void sfMsgConf_ClrConfig(void)
{
	if (sMsgModuleList != NULL) {
		kfree(sMsgModuleList);
		sMsgModuleList = NULL;
	}
}
