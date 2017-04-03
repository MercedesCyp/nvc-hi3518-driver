// ================================================================================
// MsgConf.c
//
// 1. 在驱动初始化的时候，先会初始化好硬件，然后根据输入不同的机型计算出设备的能力集，
// 2. 初始化消息响应处理中心的任务（ 也就是这个文件的作用 ），应该在获取能力集后
// 3. 初始化方法先会通过能力集来计算出驱动要挂载的模块数量，
// 	  然后生成挂载这些消息响应处理模块的静态链表
//    然后根据能力集把模块一个个配置好
// 4. 模块的申明在 MsgCent.c 文件中，模块的入口函数类型统一为类型
//    int32 	(*aProcessing)(mRcvParam*);
//    return
//       0	   代表消息响应成功，并已经生成好回应的消息
//       -1    该模块不存在待处理的消息类型
//       -2    消息在处理的时候，与硬件相关的接口出现报错
//       -1    消息在处理的时候，组建消息出错
//
//	  
//    
//    
//    
//    
//    
// ================================================================================
#include "MsgCenter.h"
#include "MsgProtocal.h"
#include "../CTA/CTA.h"













mMsgModule 	*sMsgModuleList = NULL;
uint8		sMsgModuleNum = 0;










/*
	申明模块处理消息入口
	这些函数主要是实现 MsgCenter.c文件中
*/
extern int32 sfMsgCent_Device_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_Button_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_LDR_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_InfraredFilter_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_IfrLight_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_DoubleLens_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_StateLight_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_PTZ_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_NightLight_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_AudioPlug_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_TempMonitor_Processing( mRcvParam *iParam );
extern int32 sfMsgCent_HumidityMonitor_Processing( mRcvParam *iParam );

/*
	某些模块的回调函数
*/
extern void sfMsgCent_Report_LDR(uint8);
extern void sfMsgCent_Report_Button(mButton_IntBackParam*);










/**
	配置函数
	iCapMask = NVCDriverCap.aDrvCapMask;
*/
int32 sfMsgConf_StartConfig(uint32 iCapMask){

	uint8 		tCapNum = 0;
	uint32 		_i;
	
	for( _i = 0x01; _i ; _i <<= 1 )
		if( iCapMask&_i )
			tCapNum++;
	
	tCapNum += 1 ;// The Device module
	sMsgModuleList = (mMsgModule*)kmalloc(sizeof(mMsgModule)*tCapNum, GFP_ATOMIC  );
	if( sMsgModuleList == NULL){
		return -1;
	}
	sMsgModuleNum = tCapNum;
	
	
	tCapNum = 0;
	// ---------------------------------------->mount device module
	sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_DEVICE;
	//device module function enter
	sMsgModuleList[tCapNum].aProcessing = sfMsgCent_Device_Processing; 
	tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Device module");
#endif

	if( iCapMask&CAP_SUPP_ButtonMonitor ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_BUTTON;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_Button_Processing;
		
		gSysCTA->apButton->afRegCallBack(sfMsgCent_Report_Button);
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Button module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_LdrMonitor    ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_LDR;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_LDR_Processing;
		
		gSysCTA->apLDR->afRegCallBack(sfMsgCent_Report_LDR);
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load LDR module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_Ircut         ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_IRC;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_InfraredFilter_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load IRC module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_IfrLamp       ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_IfrLIGHT;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_IfrLight_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Ifr_light module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_DoubleLens    ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_DoubLENS;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_DoubleLens_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Doub_Lens module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_StateLed      ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_StaLIGHT;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_StateLight_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Sta_Light module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_PTZ           ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_PTZ;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_PTZ_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load PTZ module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_NightLight    ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_NitLIGHT;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_NightLight_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Night_Light module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_AudioPlug     ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_AudioPLGU;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_AudioPlug_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Audio_Plug module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_TempMonitor   ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_TempMONITOR;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_TempMonitor_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Temp_Monitor module");
#endif
	}
		
	if( iCapMask&CAP_SUPP_HumiMonitor   ){
		sMsgModuleList[tCapNum].aID = NVC_MSG_TYPE_HumiMONITOR;
		sMsgModuleList[tCapNum].aProcessing = \
			sfMsgCent_HumidityMonitor_Processing;
		
		tCapNum++;
#if DEBUG_INIT
	NVCPrint("MsgConf:   Load Humi_Monitor module");
#endif
	}
	
	return 0;
}

void sfMsgConf_ClrConfig(void){\
	
	if( sMsgModuleList != NULL ){
		kfree( sMsgModuleList );
		sMsgModuleList = NULL;
	}
	
}
