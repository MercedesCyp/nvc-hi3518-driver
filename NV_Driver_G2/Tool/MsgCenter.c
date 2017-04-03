#include "MsgCenter.h"
#include "MsgProtocal.h"
#include "String.h"
#include "Mqueue.h"
#include "../CTA/CTA.h"









// ================================================================================
// ----------------------------------------------------------------------> quote
// ------------------------------------------------------------> Orientation Outside
static int32 sfMsgCent_Init(uint32 iCmd);
static int32 sfMsgCent_Unint(uint32 iCmd);
static int32 sfMsgCent_Process(uint32 iUUID,void *iMsg,uint16 iLen);

extern int32 sfMsgConf_StartConfig(uint32 iCapMask);
extern void sfMsgConf_ClrConfig(void);

// ------------------------------------------------------------> Orientation inside
// --------------------------------------------------> Tools
static int32 sfMsgCent_GetHeader	( void *iMsg, mRcvParam *RcvInfo );
static int32 sfMsgCent_Union		(\
									void 	*iTar,\
									uint16 	 iTarLen,\
									void	*iMsg,\
									uint16	 iMsgLen,\
									uint16	 iCmd,\
									uint8	 iUnit,\
									uint8	 iErr);
					
static int32 sfMsgCent_Send			(uint32,void*,uint16);
static int32 sfMsgCent_Report		(void*,uint16);


// --------------------------------------------------> Server
// uint8 icmd mRcvParam*
// iCmd
#define DC_MsgCentCmd_Set				0x80
#define DC_MsgCentCmd_Get				0x40
//-=-=-=-=-=-=
// static void sfMsgCent_Report_Temp();
// static void sfMsgCent_Report_Humidity();
// static void sfMsgCent_Report_PTZ();
//----------



















// ================================================================================
// ----------------------------------------------------------------------> Variable
extern 	mNVC_DRV_Ver_INFO 	NVCDriverInfo;
extern 	mNVC_DRV_CAP_INFO 	NVCDriverCap;
mClass_MsgCenter gClassMsgCent = {
	.afInit 	= sfMsgCent_Init,
	.afUninit 	= sfMsgCent_Unint,
	.afProcess 	= sfMsgCent_Process
};
















 




// ================================================================================
// ----------------------------------------------------------------------> Local Function
// ------------------------------------------------------------> Orientation outside
static int32 sfMsgCent_Init(uint32 iCmd){
	sfMsgConf_StartConfig( NVCDriverCap.aDrvCapMask );
	
#if DEBUG_INIT
	NVCPrint("The Msg_Cent Start! OK");
#endif
	return 0;
}
static int32 sfMsgCent_Unint(uint32 iCmd){
	sfMsgConf_ClrConfig();
	return 0;
}

extern mMsgModule 	*sMsgModuleList;
extern uint8		sMsgModuleNum;
static int32 sfMsgCent_Process(uint32 iUUID,void *iMsg,uint16 iLen){
	uint8 			tBuf[DC_Protocal_MaxMesgLen];
	int32			tLen = DC_Protocal_MaxMesgLen;
	mRcvParam 		tMsgInfo;
	uint8			_i;
	
	tLen				= DC_Protocal_MaxMesgLen;
	tMsgInfo.aUUID 		= iUUID;
	tMsgInfo.apTarMsg 	= tBuf;
	tMsgInfo.apTarLen 	= &tLen;
	
	if( sfMsgCent_GetHeader(iMsg,&tMsgInfo) )
		return -1;
	for( _i = 0; _i < sMsgModuleNum; _i++ )
		if( DF_GET_MSGTYPE(tMsgInfo.aCmd) == sMsgModuleList[ _i ].aID ){
			if( sMsgModuleList[ _i ].aProcessing( &tMsgInfo ) ){
				return -1;
			}else{
				break;
			}
		}
	
	if( _i == sMsgModuleNum )
		return -1;

	if( (tLen > 0) && ( tLen < DC_Protocal_MaxMesgLen ) ){
#if DEBUG_RWMSG
	DbgPrinStr("/*  RESP   */\r\n",tBuf,tLen  );
#endif
		sfMsgCent_Send( iUUID, tBuf, (uint16)tLen);
		return 0;
	}else{
		return -1;
	}
	
	return 0;
}




















// ------------------------------------------------------------> Orientation Inside
// --------------------------------------------------> Tools
static int32 sfMsgCent_GetHeader( void *iMsg, mRcvParam *iRcvInfo ){
	
	void *tMsgBuf = iMsg;
	
	if( DC_Protocal_MagicWord 	== 	*(uint16*)tMsgBuf ){
		tMsgBuf+=2;
		
		iRcvInfo->aCmd 			= 	*(uint16*)tMsgBuf;
		tMsgBuf+=2;
		
		iRcvInfo->aLen 			= 	*(uint16*)tMsgBuf;
		tMsgBuf+=2;
		
		iRcvInfo->aUnitNum 		= 	*(uint8*)tMsgBuf;
		
		iRcvInfo->apMsg			=	iMsg + DC_Protocal_MsgHeaderSize;
	}else{
		printk("The Magic word is wrong!\r\n");
		return -1;
	}	
	return 0;
}



static int32 sfMsgCent_Union(\
					void 	*iTar,\
					uint16 	 iTarLen,\
					void	*iMsg,\
					uint16	 iMsgLen,\
					uint16	 iCmd,\
					uint8	 iUnit,\
					uint8	 iErr){
	
	uint8 *tTar = iTar;
	uint8 *tSou = iMsg;
	uint16 tMagicWord = DC_Protocal_MagicWord;
	
	if( iTarLen<(DC_Protocal_MsgHeaderSize+iMsgLen) ){
		return -1;
	}else{
		tTar = gClassStr.afCopy( tTar, 	(uint8*)&tMagicWord,	2);
		tTar = gClassStr.afCopy( tTar, 	(uint8*)&iCmd,			2);
		tTar = gClassStr.afCopy( tTar, 	(uint8*)&iMsgLen,		2);
		tTar = gClassStr.afCopy( tTar, 	(uint8*)&iUnit,			1);
		tTar = gClassStr.afCopy( tTar, 	(uint8*)&iErr,			1);
		tTar = gClassStr.afMemset( tTar,         0x00,			4);
		
			if( iMsg != NULL ){
		tTar = gClassStr.afCopy( tTar,	tSou,					iMsgLen);
			}
		
	}
	return (int32)((void*)tTar-iTar);
}



static int32 sfMsgCent_Send(uint32 aUUID, void *iMsg, uint16 iMsgLen){
	int tRet;
	tRet = goMQUEUE_Class.cpProtect->prfACT_AddRmdMsg(aUUID,iMsg,iMsgLen);
	return (int32)tRet;
}

static int32 sfMsgCent_Report(void *iMsg, uint16 iMsgLen){
	int tRet;
#if DEBUG_RWMSG
	DbgPrinStr("/*  REPORT  */\r\n",iMsg,iMsgLen);
#endif
	tRet = goMQUEUE_Class.cpProtect->prfACT_AddSubMsg(iMsg,iMsgLen);
	return (int32)tRet;
}















// ================================================================================
// ------------------------------------------------------------> Module Device
static int32 sfMsgCent_SendDrvInfo( mRcvParam *iParam );
static int32 sfMsgCent_SendDrvAbility( mRcvParam *iParam );
static int32 sfMsgCent_RegisterReport( mRcvParam *iParam );

int32 sfMsgCent_Device_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_DEVICE_GET_INFO   :
			sfMsgCent_SendDrvInfo( iParam );
		break;
		case NVC_MSG_DEVICE_GET_CAP    :
			sfMsgCent_SendDrvAbility( iParam );
		break;
		case NVC_MSG_DEVICE_SUB_REPORT :
			sfMsgCent_RegisterReport( iParam );
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_SendDrvInfo( mRcvParam *iParam ){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	*tLen = (int32)sfMsgCent_Union(\
					tBuf,\
					*tLen,\
					(uint8*)&NVCDriverInfo,\
					sizeof(mNVC_DRV_Ver_INFO),\
					NVC_MSG_TYPE_DEVICE|NVC_MSG_DEVICE_GET_INFO_RESP,\
					0,0);
	return 0;
}

static int32 sfMsgCent_SendDrvAbility( mRcvParam *iParam ){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	*tLen = (int32)sfMsgCent_Union(\
					tBuf,\
					*tLen,\
					(uint8*)&NVCDriverCap,\
					sizeof(mNVC_DRV_CAP_INFO),\
					NVC_MSG_TYPE_DEVICE|NVC_MSG_DEVICE_GET_CAP_RESP,\
					0,0);
	return 0;
}

static int32 sfMsgCent_RegisterReport( mRcvParam *iParam ){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	// ========================================> User Code
	mNVC_ATTACHED_Msg	*tSetIfo = iParam->apMsg;
	uint8	tErrCode = 0;
	
	if( tSetIfo->aAttched&DC_NVCMD_ATTACHED_SubMsg ){
		int tRet = goMQUEUE_Class.cpProtect->prfACT_AddSubUser(iParam->aUUID);
		if( tRet ){
			tErrCode = NVC_DRIVER_ERR;
#if DEBUG_DEVELOP
		NVCPrint_h("MsgCent-AddSubUser-ErrCode:%d\r\n",tRet);
#endif
		}
	}else{
		int tRet = goMQUEUE_Class.cpProtect->prfACT_DelSubUser(iParam->aUUID);
		if( tRet ){
#if DEBUG_DEVELOP
		NVCPrint_h("MsgCent-DelSubUser-ErrCode:%d\r\n",tRet);
#endif
		}
	}
	/**************************************************/
	
	*tLen = (int32)sfMsgCent_Union(\
					tBuf,\
					*tLen,\
					NULL,0,\
					NVC_MSG_TYPE_DEVICE|NVC_MSG_DEVICE_SUB_REPORT_RESP,\
					0,tErrCode);
	
	return 0;
}










// ================================================================================
// ------------------------------------------------------------> Module Button
static int32 sfMsgCent_OptButton(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_Button_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_BUTTON_GET_STATUS   :
			sfMsgCent_OptButton( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}


static int32 sfMsgCent_OptButton(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;

	if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_BUTTON_STATUS 		tGetIfo;
		mButton_IntBackParam	tStaIfo;
		tStaIfo.aUnit = iParam->aUnitNum;
		gSysCTA->apButton->afGetStatus(&tStaIfo);
		if( tStaIfo.aStatus&DC_BuMoSta_HIGHG ){
			tGetIfo.aStatus = DC_NVBUTT_ButtonHight;
		}else if( tStaIfo.aStatus&DC_BuMoSta_LOW ){
			tGetIfo.aStatus = DC_NVBUTT_ButtonLow;
		}
		/**************************************************/ 
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_BUTTON_STATUS),\
						NVC_MSG_TYPE_BUTTON|NVC_MSG_BUTTON_GET_STATUS_RESP,\
						tStaIfo.aUnit,\
						0);
	
	}
	return 0;
}

void sfMsgCent_Report_Button(mButton_IntBackParam* iStatus){
	uint8	tRepotBuf[ DC_Protocal_MaxMesgLen ];
	uint16 	tLenth;
	mNVC_BUTTON_STATUS tReportInfo;
	
	printk("------->\r\n");
	
	if( iStatus->aStatus&DC_BuMoSta_RAISE ){
		tReportInfo.aStatus = DC_NVBUTT_ButtonRaise;
	}else if( iStatus->aStatus&DC_BuMoSta_FALL ){
		tReportInfo.aStatus = DC_NVBUTT_ButtonFall;
	}
	
	tLenth = (int32)sfMsgCent_Union(\
						tRepotBuf,\
						DC_Protocal_MaxMesgLen,\
						&tReportInfo,\
						sizeof(mNVC_BUTTON_STATUS),\
						NVC_MSG_TYPE_BUTTON|NVC_MSG_BUTTON_REPORT_STTAUS,\
						iStatus->aUnit,0);
						
	sfMsgCent_Report(tRepotBuf,	tLenth);
}










// ================================================================================
// ------------------------------------------------------------> Module LDR
#define DC_MsgCentCmd_LDRSensitive			0x01
#define DC_MsgCentCmd_LDRStatus				0x02
static int32 sfMsgCent_OptLDR(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_LDR_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_LDR_GET_STATE   :
			sfMsgCent_OptLDR( DC_MsgCentCmd_Get|DC_MsgCentCmd_LDRStatus, iParam );
		break;
		case NVC_MSG_LDR_SET_SENSITIVE   :
			sfMsgCent_OptLDR( DC_MsgCentCmd_Set|DC_MsgCentCmd_LDRSensitive, iParam );
		break;
		case NVC_MSG_LDR_GET_SENSITIVE   :
			sfMsgCent_OptLDR( DC_MsgCentCmd_Get|DC_MsgCentCmd_LDRSensitive, iParam );
		break;
		default:return -1;
	}
	return 0;
}


static int32 sfMsgCent_OptLDR(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
			// ========================================> User Code
			mNVC_DNMONITOR_SENSI	*tSetIfo = iParam->apMsg;
			
			gSysCTA->apLDR->afSetSensitive( tSetIfo->aSensitive );
			
			/**************************************************/ 
			*tLen = (int32)sfMsgCent_Union(\
							tBuf,\
							*tLen,\
							NULL,0,\
							NVC_MSG_TYPE_LDR|NVC_MSG_LDR_SET_SENSITIVE_RESP,\
							0,0);
		
	}else if( iCmd&DC_MsgCentCmd_Get ){
		if( iCmd&DC_MsgCentCmd_LDRStatus ){
			// ========================================> User Code
			mNVC_DNMONITOR_STATUS	tGetIfo;
			int32 					tRet;
			tRet = gSysCTA->apLDR->afGetStatus();
			if( tRet == DC_LDR_DAY ){
				tGetIfo.aStatus = DC_NVANMonitor_DAY;
			}else{
				tGetIfo.aStatus = DC_NVANMonitor_NIGHT;
			}
			/**************************************************/ 
			*tLen = (int32)sfMsgCent_Union(\
							tBuf,\
							*tLen,\
							&tGetIfo,\
							sizeof(mNVC_DNMONITOR_STATUS),\
							NVC_MSG_TYPE_LDR|NVC_MSG_LDR_GET_STATE_RESP,\
							0,0);
			
		}else if( iCmd&DC_MsgCentCmd_LDRSensitive ){
			// ========================================> User Code
			mNVC_DNMONITOR_SENSI	tGetIfo;
			
			tGetIfo.aSensitive = gSysCTA->apLDR->afSetSensitive(0x00);
			
			/**************************************************/ 
			*tLen = (int32)sfMsgCent_Union(\
							tBuf,\
							*tLen,\
							&tGetIfo,\
							sizeof(mNVC_DNMONITOR_SENSI),\
							NVC_MSG_TYPE_LDR|NVC_MSG_LDR_GET_SENSITIVE_RESP,\
							0,0);
			
			
		}
		
	}
	
	return 0;
}

void sfMsgCent_Report_LDR(uint8 iState){
	uint8	tRepotBuf[ DC_Protocal_MaxMesgLen ];
	uint16 	tLenth;
	mNVC_DNMONITOR_STATUS tReportInfo;
	
	if( iState == DC_LDR_DAY ){
		tReportInfo.aStatus = DC_NVANMonitor_DAY;
	}else{
		tReportInfo.aStatus = DC_NVANMonitor_NIGHT;
    }
	tLenth = (int32)sfMsgCent_Union(\
						tRepotBuf,\
						DC_Protocal_MaxMesgLen,\
						&tReportInfo,\
						sizeof(mNVC_DNMONITOR_STATUS),\
						NVC_MSG_TYPE_LDR|NVC_MSG_LDR_REPORT_STATE,\
						0,0);
						
	sfMsgCent_Report( tRepotBuf, tLenth);
	
}










// ================================================================================
// ------------------------------------------------------------> Module Infrared filter
#define DC_MsgCentCmd_DvcStatus			0x00
#define DC_MsgCentCmd_DvcType			0x01
static int32 sfMsgCent_OptIfrFilter(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_InfraredFilter_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_IRC_GET_TYPE   :
			sfMsgCent_OptIfrFilter( DC_MsgCentCmd_Get|DC_MsgCentCmd_DvcType,\
									iParam);
		break;
		case NVC_MSG_IRC_SET_STATUS   :
			sfMsgCent_OptIfrFilter( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_IRC_GET_STATUS   :
			sfMsgCent_OptIfrFilter( DC_MsgCentCmd_Get|DC_MsgCentCmd_DvcStatus,\
									iParam);
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_OptIfrFilter(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mNVC_IRFILT_STATUS *tSetIfo = iParam->apMsg;
		if( tSetIfo->aStatus == DC_NVIfrFilt_PassInfr ){
			gSysCTA->apIfrFilter->afSetStatus(DC_IfrFlt_PassLi);
		}else{
			gSysCTA->apIfrFilter->afSetStatus(DC_IfrFlt_BlockLi);
		}
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_IRC|NVC_MSG_IRC_SET_STATUS_RESP,\
						0,0);
		
	}else if( iCmd&DC_MsgCentCmd_Get ){
		if( iCmd&DC_MsgCentCmd_DvcType ){
			// ========================================> User Code
			mNVC_IRFILT_TYPE 	tGetIfo;
			int32 				tRet;
			tRet = gSysCTA->apIfrFilter->afGetStatus();
			tGetIfo.aType	= tRet&0xFF;
			/**************************************************/ 
			
			*tLen = (int32)sfMsgCent_Union(\
							tBuf,\
							*tLen,\
							&tGetIfo,
							sizeof(mNVC_IRFILT_TYPE),\
							NVC_MSG_TYPE_IRC|NVC_MSG_IRC_GET_TYPE_RESP,\
							0,0);
						
		}else{
			// ========================================> User Code
			mNVC_IRFILT_STATUS 	tGetIfo;
			int32				tRet;
			tRet = gSysCTA->apIfrFilter->afGetStatus();
			if( tRet&DC_IfrFlt_PassLi ){
				tGetIfo.aStatus = DC_NVIfrFilt_PassInfr;
			}else if( tRet&DC_IfrFlt_BlockLi ){
				tGetIfo.aStatus = DC_NVIfrFilt_BlockInfr;
			}
			/**************************************************/ 
			
			*tLen = (int32)sfMsgCent_Union(\
							tBuf,\
							*tLen,\
							&tGetIfo,
							sizeof(mNVC_IRFILT_STATUS),\
							NVC_MSG_TYPE_IRC|NVC_MSG_IRC_GET_STATUS_RESP,\
							0,0);
		}
	}
	
	return 0;
}










// ================================================================================
// ------------------------------------------------------------> Module Infrared light
static int32 sfMsgCent_OptIfrLight(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_IfrLight_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_IfrLIGHT_SET_STATUS   :
			sfMsgCent_OptIfrLight( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_IfrLIGHT_GET_STATUS   :
			sfMsgCent_OptIfrLight( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}


static int32 sfMsgCent_OptIfrLight(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mNVC_IRLIGHT_STATUS *tSetIfo = iParam->apMsg;
		if( tSetIfo->aStatus == DC_NVIrLight_ON ){
			gSysCTA->apIfrLi->afSetStatus( DC_IfLi_On );
		}else if( tSetIfo->aStatus == DC_NVIrLight_OFF ){
			gSysCTA->apIfrLi->afSetStatus( DC_IfLi_Off );
		}
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_IfrLIGHT|NVC_MSG_IfrLIGHT_SET_STATUS_RESP,\
						0,0);
	
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_IRLIGHT_STATUS 	tGetIfo;
		int32 					tRet;
		tRet	= gSysCTA->apIfrLi->afGetStatus();
		if( tRet&DC_IfLi_On ){
			tGetIfo.aStatus = DC_NVIrLight_ON;
		}else if( tRet&DC_IfLi_Off ){
			tGetIfo.aStatus = DC_NVIrLight_OFF;
		}
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_IRLIGHT_STATUS),\
						NVC_MSG_TYPE_IfrLIGHT|NVC_MSG_IfrLIGHT_GET_STATUS_RESP,\
						0,0);
		
	}
	return 0;
}










// ================================================================================
// ------------------------------------------------------------> Module State Light
static int32 sfMsgCent_OptStatelight(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_StateLight_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_StaLIGHT_SET_STATUS   :
			sfMsgCent_OptStatelight( DC_MsgCentCmd_Set, iParam );
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_OptStatelight(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mStaLi_SetParam			 tSetParam;
		mNVC_STALIGHT_SETINFO 	*tSetIfo = iParam->apMsg;
		tSetParam.aPTime	=	tSetIfo->aOnMes;
		tSetParam.aNTime	=	tSetIfo->aOffMes;
		tSetParam.aUnit		=	tSetIfo->aLightID - 1;
		// printk(": %d\r\n: %d\r\n: %d\r\n",(int)tSetParam.aPTime,(int)tSetParam.aNTime,(int)tSetParam.aUnit);
		gSysCTA->apStateLi->afSetStatus( &tSetParam );
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_StaLIGHT|NVC_MSG_StaLIGHT_SET_STATUS_RESP,\
						0,0);
		
	}
	
	return 0;
}









// ================================================================================
// ------------------------------------------------------------> Module Pan Tilt
static int32 sfMsgCent_OptPTZ(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_PTZ_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_PTZ_GET_INFO   :
			sfMsgCent_OptPTZ( DC_MsgCentCmd_Get, iParam );
		break;
		case NVC_MSG_PTZ_SET_ACTION   :
			sfMsgCent_OptPTZ( DC_MsgCentCmd_Set, iParam );
		break;
		// case NVC_MSG_PTZ_SET_PRESET   :
		// break;
		// case NVC_MSG_PTZ_CLR_PRESET   :
		// break;
		default:return -1;
	}
	return 0;
}


#if DEBUG_MSGCENT
static void sfMsgCent_PTZ_ShowPTZGetInfo(mNVC_PANTILT_INFO *);
#endif
static int32 sfMsgCent_OptPTZ(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mPTZAction			tAct;
		uint8 				tErrCode = 0;
		uint32 				tCmdToPT = 0;
		mNVC_PANTILT_SET	*tSetIfo = iParam->apMsg;
		int32				tPtzRet;
		mNVC_PANTILT_Respons tPanTiltInfo;
		
		switch( tSetIfo->aCmd ){
			case NV_PTZ_STOP:		
			gSysCTA->apPTZ->gfPTZStop();
			break;
			case NV_PTZ_UP:			tCmdToPT 	|=  DC_PTZCmd_Up;
			break;
			case NV_PTZ_DOWN:		tCmdToPT 	|=  DC_PTZCmd_Down;
			break;
			case NV_PTZ_LEFT:		tCmdToPT 	|=  DC_PTZCmd_Left;
			break;
			case NV_PTZ_RIGHT:		tCmdToPT 	|=  DC_PTZCmd_Right;
			break;
			case NV_PTZ_LEFT_UP:	tCmdToPT 	|=  DC_PTZCmd_Left|DC_PTZCmd_Up;
			break;
			case NV_PTZ_LEFT_DOWN:	tCmdToPT 	|=  DC_PTZCmd_Left|DC_PTZCmd_Down;
			break;
			case NV_PTZ_RIGHT_UP:	tCmdToPT 	|=  DC_PTZCmd_Right|DC_PTZCmd_Up;
			break;
			case NV_PTZ_RIGHT_DOWN:	tCmdToPT 	|=  DC_PTZCmd_Right|DC_PTZCmd_Down;
			break;
			default: 				return -1;
		}
		
		switch( tSetIfo->aParaType ){
			case 0:						tCmdToPT |=	DC_PTZCmd_Steps; 	break;
			case 1:						tCmdToPT |=	DC_PTZCmd_Dgr; 		break;
			case 2:						tCmdToPT |=	DC_PTZCmd_Crd;		break;
			default: return -1;
		}
		tAct.aHComponent = tSetIfo->aHParam;
		tAct.aVComponent = tSetIfo->aVParam;
		tAct.aSpeed      = tSetIfo->aSpeed;
		tAct.aCommand    = tCmdToPT;
		
		
		tPtzRet = gSysCTA->apPTZ->gfPTZStart( tAct );
		
		// handle the return status msg
		if( 	(tPtzRet&DC_PTZRet_HOverLim)||\
				(tPtzRet&DC_PTZRet_VOverLim)){
				
			tErrCode 				= NVC_DRIVER_ERR_LIMITED; 
		
		}else if((tPtzRet&DC_PTZRet_HMoveWorning)||\
				(tPtzRet&DC_PTZRet_VMoveWorning)){
				
			tErrCode 				= NVC_DRIVER_ERR_UNFINISHED;
		
		}else if(tPtzRet&DC_PTZRet_Busy){
			
			tErrCode 				= NVC_DRIVER_ERR_BUSY;
		
		}else if((tPtzRet&DC_PTZRet_InputEmpty)||\
				(tPtzRet&DC_PTZRet_OverSpeed)){
		
			tErrCode 				= NVC_DRIVER_ERR_PARAM;
		
		}else if((tPtzRet&DC_PTZRet_BufIdle		)){
		
			tPanTiltInfo.aStatus 	|= NV_PTZ_STA_RemainSpace;
			tPanTiltInfo.aStatus 	|= NV_PTZ_STA_Success;
		}
		/**************************************************/ 
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tPanTiltInfo,\
						sizeof(mNVC_PANTILT_Respons),\
						NVC_MSG_TYPE_PTZ|NVC_MSG_PTZ_SET_ACTION_RESP,\
						0,tErrCode);
						
						
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_PANTILT_INFO	tGetIfo;
		gSysCTA->apPTZ->afUpdatePTZInfo();
		
		tGetIfo.aCmdMask 		= DF_PTSP_RcfExport(gSysCTA->apPTZ->apDefInfo->aRegCf);
		tGetIfo.aStatus 		= DF_PTSP_StaExport(gSysCTA->apPTZ->apDefInfo->aStatus);
		tGetIfo.aHRange 		= gSysCTA->apPTZ->apDefInfo->aXRange;
		tGetIfo.aDgrPerHSteps 	= gSysCTA->apPTZ->apDefInfo->aHDgrPerStep;
		tGetIfo.aUnitHSteps 	= 1;
		tGetIfo.aVRange 		= gSysCTA->apPTZ->apDefInfo->aYRange;
		tGetIfo.aDgrPerVSteps	= gSysCTA->apPTZ->apDefInfo->aVDgrPerStep;
		tGetIfo.aUnitVSteps		= 1;
		tGetIfo.aZeroXPos 		= 0;
		tGetIfo.aZeroYPos 		= 0;
		tGetIfo.aXPos			= gSysCTA->apPTZ->apDefInfo->aHCurPos;
		tGetIfo.aYPos			= gSysCTA->apPTZ->apDefInfo->aYCurPos;
#if DEBUG_MSGCENT
	sfMsgCent_PTZ_ShowPTZGetInfo(&tGetIfo);
#endif
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_PANTILT_INFO),\
						NVC_MSG_TYPE_PTZ|NVC_MSG_PTZ_GET_INFO_RESP,\
						0,0);
	}
	return 0;
}
#if DEBUG_MSGCENT
static void sfMsgCent_PTZ_ShowPTZGetInfo(mNVC_PANTILT_INFO *tGetIfo){
	NVCPrint("************************************************************");
	NVCPrint("PTZIfo.aCmdMask      : 0x%x", (unsigned int)tGetIfo->aCmdMask);
	NVCPrint("PTZIfo.aStatus       : 0x%x", (unsigned int)tGetIfo->aStatus );
	NVCPrint("PTZIfo.aHRange       : %d",   (int)tGetIfo->aHRange          );
	NVCPrint("PTZIfo.aDgrPerHSteps : %d",   (int)tGetIfo->aDgrPerHSteps    );
	NVCPrint("PTZIfo.aUnitHSteps   : %d",   (int)tGetIfo->aUnitHSteps      );
	NVCPrint("PTZIfo.aVRange       : %d",   (int)tGetIfo->aVRange          );
	NVCPrint("PTZIfo.aDgrPerVSteps : %d",   (int)tGetIfo->aDgrPerVSteps    );
	NVCPrint("PTZIfo.aUnitVSteps   : %d",   (int)tGetIfo->aUnitVSteps      );
	NVCPrint("PTZIfo.aZeroXPos     : %d",   (int)tGetIfo->aZeroXPos        );
	NVCPrint("PTZIfo.aZeroYPos     : %d",   (int)tGetIfo->aZeroYPos        );
	NVCPrint("PTZIfo.aXPos         : %d",   (int)tGetIfo->aXPos            );
	NVCPrint("PTZIfo.aYPos         : %d",   (int)tGetIfo->aYPos            );
	NVCPrint("************************************************************");
}
#endif








// ================================================================================
// ------------------------------------------------------------> Module Night Light
static int32 sfMsgCent_OptNightLight(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_NightLight_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_NitLIGHT_SET_STATUS   :
			sfMsgCent_OptNightLight( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_NitLIGHT_GET_STATUS    :
			sfMsgCent_OptNightLight( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}


static int32 sfMsgCent_OptNightLight(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mNVC_NIGHTLIGHT_STATUS *tSetIfo = iParam->apMsg;
		mClass_NiLiIfo	tSetParam;
		
		gClassStr.afMemset( (uint8*)&tSetParam, 0x00, sizeof(mClass_NiLiIfo) );
		
		tSetParam.aLevel = tSetIfo->aLemLevel;
		if( tSetIfo->aStatus == DC_NIGHTLIGHT_On ){
			tSetParam.aStatus |= DC_NiLi_On;
		}else{
			tSetParam.aStatus |= DC_NiLi_Off;
		}
		gSysCTA->apNightLi->afSetStatus( &tSetParam );
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_NitLIGHT|NVC_MSG_NitLIGHT_SET_STATUS_RESP,\
						0,0);
	
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_NIGHTLIGHT_STATUS 	tGetIfo;
		
		gClassStr.afMemset( (uint8*)&tGetIfo, 0x00, sizeof(mNVC_NIGHTLIGHT_STATUS) );
		
		gSysCTA->apNightLi->afGetStatus();
		
		if( gSysCTA->apNightLi->aIfo->aStatus&DC_NiLi_On ){
			tGetIfo.aStatus = DC_NIGHTLIGHT_On;
		}
		tGetIfo.aLemLevel = gSysCTA->apNightLi->aIfo->aLevel;
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_NIGHTLIGHT_STATUS),\
						NVC_MSG_TYPE_NitLIGHT|NVC_MSG_NitLIGHT_GET_STATUS_RESP,\
						0,0);
	}
	return 0;
}










// ================================================================================
// ------------------------------------------------------------> Module Audio Plug
static int32 sfMsgCent_OptAudioPlug(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_AudioPlug_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_AudioPLUG_SET_STATUS   :
			sfMsgCent_OptAudioPlug( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_AudioPLUG_GET_STATUS    :
			sfMsgCent_OptAudioPlug( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_OptAudioPlug(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;

	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code 
		mNVC_AUDIOPLUG_Msg	*tSetIfo = iParam->apMsg;
		if( DC_NVAuPl_On&tSetIfo->aStatus ){
			gSysCTA->apAudioPlug->afSetStatus(DC_AuPl_On);
		}else{
			gSysCTA->apAudioPlug->afSetStatus(DC_AuPl_Off);
		}	
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_AudioPLGU|NVC_MSG_AudioPLUG_SET_STATUS_RESP,\
						0,0);
						
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_AUDIOPLUG_Msg	tGetIfo;
		int32 				tRet;
		tRet = gSysCTA->apAudioPlug->afGetStatus();
		if( tRet&DC_AuPl_On ){
			tGetIfo.aStatus = DC_NVAuPl_On;
		}else if( tRet&DC_AuPl_Off ){
			tGetIfo.aStatus = DC_NVAuPl_Off;
		}
		/**************************************************/
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_AUDIOPLUG_Msg),\
						NVC_MSG_TYPE_AudioPLGU|NVC_MSG_AudioPLUG_GET_STATUS_RESP,\
						0,0);
	}
	return 0;
}









// ================================================================================
// ------------------------------------------------------------> Module Temperature Monitor
static int32 sfMsgCent_OptTemp(uint8 iCmd, mRcvParam *iParam);


int32 sfMsgCent_TempMonitor_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_TempMONITOR_SUB_REPORT   :
			sfMsgCent_OptTemp( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_TempMONITOR_GET_VALUE    :
			sfMsgCent_OptTemp( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_OptTemp(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mNVC_TEMPMONITOR_Timer *tSetIfo = iParam->apMsg;
		gSysCTA->apTemp->afSetReportTime( tSetIfo->aTimes );
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_TempMONITOR|NVC_MSG_TempMONITOR_SUB_REPORT_RESP,\
						0,0);
		
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_TEMPMONITOR_Value	tGetIfo;
		int32 					tRet;
		tRet = gSysCTA->apTemp->afGetTemperature();
		tGetIfo.aValue = tRet;
		/**************************************************/ 
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_TEMPMONITOR_Value),\
						NVC_MSG_TYPE_TempMONITOR|NVC_MSG_TempMONITOR_GET_VALUE_RESP,\
						0,0);
	}
	return 0;
}










// ================================================================================
// ------------------------------------------------------------> Module Humidity Monitor
static int32 sfMsgCent_OptHumidity(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_HumidityMonitor_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_HumiMONITOR_SUB_REPORT   :
			sfMsgCent_OptHumidity( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_HumiMONITOR_GET_VALUE    :
			sfMsgCent_OptHumidity( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_OptHumidity(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mNVC_HUMIDITY_Timer *tSetIfo = iParam->apMsg;
		gSysCTA->apHumidity->afSetReportTime( tSetIfo->aTimes );
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_HumiMONITOR|NVC_MSG_HumiMONITOR_SUB_REPORT_RESP,\
						0,0);
						
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_HUMIDITY_Value 	tGetIfo;
		int32 					tRet;
		tRet = gSysCTA->apHumidity->afGetHumidity();
		tGetIfo.aValue = tRet;
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_HUMIDITY_Value),\
						NVC_MSG_TYPE_HumiMONITOR|NVC_MSG_HumiMONITOR_GET_VALUE_RESP,\
						0,0);
						
	}
	return 0;
}










// ================================================================================
// ------------------------------------------------------------> Module Double Lens
static int32 sfMsgCent_OptDoubleLens(uint8 iCmd, mRcvParam *iParam);

int32 sfMsgCent_DoubleLens_Processing( mRcvParam *iParam ){
	
	uint8 tCmd = DF_GET_MSGCMD( iParam->aCmd );
	switch( tCmd ){
		case NVC_MSG_DoubLENS_SET_STATUS   :
			sfMsgCent_OptDoubleLens( DC_MsgCentCmd_Set, iParam );
		break;
		case NVC_MSG_DoubLENS_GET_STATUS    :
			sfMsgCent_OptDoubleLens( DC_MsgCentCmd_Get, iParam );
		break;
		default:return -1;
	}
	return 0;
}

static int32 sfMsgCent_OptDoubleLens(uint8 iCmd, mRcvParam *iParam){
	uint8 	*tBuf = iParam->apTarMsg;
	int32	*tLen = iParam->apTarLen;
	
	if( iCmd&DC_MsgCentCmd_Set ){
		// ========================================> User Code
		mNVC_DUBLENS_SETINFO *tSetIfo = iParam->apMsg;
		if( tSetIfo->aStatus == DC_DUBLENS_NIGHTLENS ){
			gSysCTA->apDubLens->afSetStatus(DC_DuLens_NightLens);
		}else {
			gSysCTA->apDubLens->afSetStatus(DC_DuLens_DayLens);
		}
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						NULL,0,\
						NVC_MSG_TYPE_DoubLENS|NVC_MSG_DoubLENS_SET_STATUS_RESP,\
						0,0);
						
	}else if( iCmd&DC_MsgCentCmd_Get ){
		// ========================================> User Code
		mNVC_DUBLENS_SETINFO	tGetIfo;
		int32 					tRet;
		tRet = gSysCTA->apDubLens->afGetStatus();
		if( tRet&DC_DuLens_DayLens ){
			tGetIfo.aStatus = DC_DUBLENS_DAYLENS;
		}else if( tRet&DC_DuLens_NightLens ){
			tGetIfo.aStatus = DC_DUBLENS_NIGHTLENS;
		}
		/**************************************************/ 
		
		*tLen = (int32)sfMsgCent_Union(\
						tBuf,\
						*tLen,\
						&tGetIfo,\
						sizeof(mNVC_DUBLENS_SETINFO),\
						NVC_MSG_TYPE_DoubLENS|NVC_MSG_DoubLENS_GET_STATUS_RESP,\
						0,0);
						
	}
	return 0;
}
