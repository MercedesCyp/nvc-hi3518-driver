#include "MsgCenter.h"
#include "HAL/PeriodEvent.h"
#include "HAL/SHT20.h"
#include "ButtonMonitor/Button.h"
#include "AudioPlug/AudioPlug.h"
#include "DNMonitor/DNMonitor.h"
#include "IfrFilter/IfrFilter.h"
#include "IfrLight/IfrLight.h"
#include "StateLED/StateLED.h"
#include "DoubleLens/DoubleLens.h"
#include "TempMonitor/TempMonitor.h"
#include "Humidity/Humidity.h"
#include "NightLight/NightLight.h"
#include "PanTilt/PanTilt.h"

// The Time are mainly using to Driver post message to application



// -------------------------------> Local Function
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--

static uint16 gfMsgCenter_UnionMsgBody(\
				uint8 *iTar,\
				uint16 iTarLen,\
				uint16 iCmdType,\
				uint8 iUnit,\
				uint8 iErrCode,\
				uint8 *iMsg,\
				uint16 iMsgLen);
static int32 sfMsgCenter_GetMsgHeader(void *iTar,mCmdHead *iCmdHeader);

// Send the user register success message 
static int32 sfSendDrvSubSuccessInfo(uint32 iUID);

static int32 sfSendDrvVerInfo(uint32 iUID);
static int32 sfSendDrvCapInfo(uint32 iUID);

static int32 sfSendDrvButtonInfo(uint32 iUID,mCmdHead *iCmdHeader);
static void  sfPE_ButtonEvent(void);
static int32 sfSendDrvAudioPlugInfo(uint32 iUID,void *MsgBody,uint8 icmd);
static int32 sfSendDrvDNMonitorInfo(uint32 iUID);
static void  sfReportDNMonitorInfo(uint8 iStatus);

#define DC_IfrFilt_GetType 	0x01
#define DC_IfrFilt_SetState 0x02
#define DC_IfrFilt_GetState 0x04
static int32 sfSendDrvIrFilterInfo(uint32 iUID,void *MsgBody,uint8 iCMD);

#define DC_IfrLight_SetState 0x01
#define DC_IfrLight_GetState 0x02
static int32 sfSendDrvIfrLightInfo(uint32 iUID,void *MsgBody,uint8 iCMD);

#define DC_StaLED_SetState 0x01
static int32 sfSendDrvStateLEDInfo(uint32 iUID,mCmdHead *MsgHead,void *MsgBody,uint8 iCMD);

#define DC_DubLens_SetState	0x01
#define DC_DubLens_GetState	0x02
static int32 sfSendDrvDubleLensInfo(uint32 iUID,void *MsgBody,uint8 iCMD);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define DC_TempMonitor_SetReport	0x01
#define DC_TempMonitor_GetData		0x02
static int32 sfSendDrvTempMonitorInfo(uint32 iUID,void *MsgBody,uint8 iCMD);
static void sfPE_TempReport(int32);

#define DC_HumiidityMonitor_SetReport	0x01
#define DC_HumidityMonitor_GetData		0x02
static int32 sfSendDrvHumidityMonitorInfo(uint32 iUID,void *MsgBody,uint8 iCMD);
static void sfPE_HumidityReport(int32);
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define DC_NightLight_SetState		0x01
#define DC_NightLight_GetState		0x02
static int32 sfSendDrvNightLightInfo(uint32 iUID,void *MsgBody,uint8 iCMD);

#define DC_PanTile_SetNormal	0x01
#define DC_PanTile_GetInfo		0x02
static int32 sfSendDrvPanTiltInfo(uint32 iUID,void *MsgBody,uint8 iCMD);
static void sfReportPanTiltInfo(uint16 ,uint16 );




// -------------------------------> Variable
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
int32 gfInitDrv_MsgCenter(uint8 iRemain)
{
	gfPE_RegisterEvent(sfPE_ButtonEvent,1,DC_PE_TYPE_UNINT);
	
	gfDNMonitor_ReportStateChange 		= sfReportDNMonitorInfo;
	
	gfPanTilt_ReportCurrentPosition 	= sfReportPanTiltInfo;
	

	pgfTempMonitor_PeriodEvent 			= sfPE_TempReport;
	
	pgfHumidityMonitor_PeriodEvent 		= sfPE_HumidityReport;
	
	return 0;
}

//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
int32 gfUninitDrv_MsgCenter(uint8 iRemain)
{
	gfPE_UnregisterEvent(sfPE_ButtonEvent,DC_PE_TYPE_UNINT);
	
	gfDNMonitor_ReportStateChange		= NULL;
	
	gfPanTilt_ReportCurrentPosition		= NULL;
	
	pgfTempMonitor_PeriodEvent			= NULL;
	
	pgfHumidityMonitor_PeriodEvent		= NULL;
	
	return 0;
}


//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
//_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--_-_--
/**
parameter:
	iCmdBuf	command buffer start 
	iBufLen	Command Buffer size
return:
	-1		Magic word is fault
	-2 		command not support or doesn't exist
*/
int32 gfMsgCenter_Processing(uint32 iUID,void *iCmdBuf,uint16 iBufLen){
	
	int32 tRet = 0 ;
	
	mCmdHead tCmdHeader;
	void *ptUMsgBody;
	
		
	if(sfMsgCenter_GetMsgHeader(iCmdBuf,&tCmdHeader))
		return -1;
	
	ptUMsgBody = iCmdBuf+DC_MsgCenter_MsgHeaderSize;
	
	switch(tCmdHeader.aMsgTypes){
		
// ------>PTZ	
	case NVC_QUERY_PTZ_INFO_REQ:{
		if(sfSendDrvPanTiltInfo(iUID,ptUMsgBody,DC_PanTile_GetInfo))
			tRet = -1;
	}break;
	
	case NVC_CONTROL_PTZ_COMMON_REQ:{
		if(sfSendDrvPanTiltInfo(iUID,ptUMsgBody,DC_PanTile_SetNormal))
			tRet = -1;
	}break;

	// case NVC_SET_PRESET_CRUISE_REQ{
	
	// }break;

	// case NVC_CLEAR_PRESET_CRUISE_REQ{
	
	// }break;
	
	
// ------>Night light
	case NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ:{
		sfSendDrvNightLightInfo(iUID,ptUMsgBody,DC_NightLight_SetState);
	}break;
	
	case NVC_QUERY_NIGHT_LIGHT_STATUS_REQ:{
		sfSendDrvNightLightInfo(iUID,NULL,DC_NightLight_GetState);
	}break;
	

// ------>Humidity	
	case NVC_SET_HUMIDITY_TIMER_REQ:{
	printk("---------->Humidity  NVC_SET_HUMIDITY_TIMER_REQ\r\n");
		sfSendDrvHumidityMonitorInfo(iUID,ptUMsgBody,DC_HumiidityMonitor_SetReport);
	}break;
	
	case NVC_QUERY_HUMIDIT_VLAUE_REQ:{
	printk("---------->Humidity  NVC_QUERY_HUMIDIT_VLAUE_REQ\r\n");
		sfSendDrvHumidityMonitorInfo(iUID,NULL,DC_HumidityMonitor_GetData);
	}break;

// ------>Temperature	
	case NVC_SET_TEMPERATURE_TIMER_REQ:{
		sfSendDrvTempMonitorInfo(iUID,ptUMsgBody,DC_TempMonitor_SetReport);
	}break;
	
	case NVC_QUERY_TEMPERATURE_VLAUE_REQ:{
		sfSendDrvTempMonitorInfo(iUID,NULL,DC_TempMonitor_GetData);
	}break;
	
// ----->Double lens
	case NVC_CONTROL_LENS_SWITCH_REQ:{
		sfSendDrvDubleLensInfo(iUID,ptUMsgBody,DC_DubLens_SetState);
	}break;
	
	case NVC_QUERY_LENS_STATUS_REQ:{
		sfSendDrvDubleLensInfo(iUID,NULL,DC_DubLens_GetState);
	}break;

// ------>Infrared Light
	case NVC_CONTROL_LAMP_SWITCH_REQ:{//25
		sfSendDrvIfrLightInfo(iUID,ptUMsgBody,DC_IfrLight_SetState);
	}break;
	
	case NVC_QUERY_LAMP_STATUS_REQ:{//27
		sfSendDrvIfrLightInfo(iUID,ptUMsgBody,DC_IfrLight_GetState);
	}break;
	
// ------>State Light
	case NVC_CONTROL_STATE_LED_REQ:{//29
		sfSendDrvStateLEDInfo(iUID,&tCmdHeader,ptUMsgBody,DC_StaLED_SetState);
	}break;
	
	
// ------>Infrared Filter
	case NVC_QUERY_IRC_TYPE_REQ:{//19
		sfSendDrvIrFilterInfo(iUID,ptUMsgBody,DC_IfrFilt_GetType);
		
	}break;
	
	case NVC_CONTROL_IRC_SWITCH_REQ:{//21
		sfSendDrvIrFilterInfo(iUID,ptUMsgBody,DC_IfrFilt_SetState);
	}break;
	
	case NVC_QUERY_IRC_STATUS_REQ:{//23
		sfSendDrvIrFilterInfo(iUID,ptUMsgBody,DC_IfrFilt_GetState);
	}break;
	
// ------>detective environment light flux
	case NVC_QUERY_LDR_STATUS_REQ:{//11
		sfSendDrvDNMonitorInfo(iUID);
	}break;
	/*暂不实现*/
	// case NVC_SET_LDR_SENSITIVITY_REQ{//15
		
	// }break;
	
	// case NVC_QUERY_LDR_SENSITIVITY_REQ:{//17
		
	// }break;

// ------>Audio Plug	
	case NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ:{//43
		sfSendDrvAudioPlugInfo(iUID,ptUMsgBody,DC_AuPl_CmdSet);
	}break;
	
	case NVC_QUERY_AUDIO_PLUG_STATUS_REQ:{//45
		sfSendDrvAudioPlugInfo(iUID,ptUMsgBody,DC_AuPl_CmdGet);
	}break;
	

// ------>Button Monitor
	case NVC_QUERY_BUTTON_STATUS_REQ:{//7
		if(sfSendDrvButtonInfo(iUID,&tCmdHeader))
			tRet = -1;
	
	}break;
	

// ------>Device Info
	case NVC_QUERY_DRIVER_CAPACITY_REQ:{//3
		sfSendDrvCapInfo(iUID);
	}break;
	
	case NVC_QUERY_DRIVER_INFO_REQ:{//1
		sfSendDrvVerInfo(iUID);
	}break;	
	
// ------>Subscribe
	case NVC_SET_ATTACHED_DRIVER_MSG_REQ:{//5
		mNVC_ATTACHED_Msg *tAtcdMsgBody;
		tAtcdMsgBody = (mNVC_ATTACHED_Msg*)(iCmdBuf+DC_MsgCenter_MsgHeaderSize);
		
		if(tAtcdMsgBody->aAttched == DC_NVCMD_ATTACHED_SubMsg){
			
			if(gfMsgQueue_addSubUser(iUID)<0)
				tRet = -1;
			sfSendDrvSubSuccessInfo(iUID);
			
		}else{ // if(tAtcdMsgBody->aAttched == DC_NVCMD_ATTACHED_UnsubMsg)
			if(gfMsgQueue_delSubUser(iUID)<0)
				tRet = -1;
		}
		
		
	}break;
	
	//--------  --------  --------  --------  --------  --------  
	
	default:
	return -2;
	}
	
	return tRet;
}




///////////////////////////////////////////////////////////////////////
// ////////  ////////  ////////  ////////  ////////  ////////  ////////
/////////////////////////////////////////////////////////////////////// 
//						The specify Message Deal function
static int32 sfSendDrvSubSuccessInfo(uint32 iUID){

	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	// tMsgLength = DC_MsgCenter_MsgHeaderSize;
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
								DC_MsgCenter_MaxMesgLen,\
								NVC_SET_ATTACHED_DRIVER_MSG_RESP,\
								0x00 ,0x00 , NULL,\
								0);	

	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	return 0;
}




/**
parameter:
	iUID	The User ID of who wanna send 
return:
	0 		operating success
	-1		the Set Buffer not enough
	-2 		the Message queue have something wrong
*/			
static int32 sfSendDrvVerInfo(uint32 iUID){
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_DRV_Ver_INFO tDriveInfo;
	
	//tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_DRV_Ver_INFO);
	
	tDriveInfo.aChipInfo 	= 	pagAppChipType[DF_GLOBAL_GetChipInfo];
	tDriveInfo.aDeviceInfo 	= 	pagAppProType[DF_GLOBAL_GetDivInfo];
	gfStringCopy(tDriveInfo.aVerInfo,DC_GLOBAL_DriverVersion,gfStringLen(DC_GLOBAL_DriverVersion)+1);
	gfStringCopy(tDriveInfo.aBuildData,DC_GLOBAL_BuildData,gfStringLen(DC_GLOBAL_BuildData)+1);
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
								DC_MsgCenter_MaxMesgLen,\
								NVC_QUERY_DRIVER_INFO_RESP,\
								0x00 ,0x00 , (uint8*)&tDriveInfo,\
								sizeof(mNVC_DRV_Ver_INFO));	
	
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	return 0;
	
}


/**
parameter:
	iUID	The User ID of who wanna send 
return:
	0 		operating success
	-1		the Set Buffer not enough
	-2 		the Message queue have something wrong
*/
static int32 sfSendDrvCapInfo(uint32 iUID){
	
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_DRV_CAP_INFO tDriveInfo;
	
	//tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_DRV_CAP_INFO);
	
	switch(DF_GLOBAL_GetDivInfo){
		case 0:	tDriveInfo.aDrvCapMask = DC_CAP_D01;	break;
		case 1:	tDriveInfo.aDrvCapMask = DC_CAP_D03;	break;
		case 2:	tDriveInfo.aDrvCapMask = DC_CAP_D04;	break;
		case 3:	tDriveInfo.aDrvCapMask = DC_CAP_D11;	break;
		case 4:	tDriveInfo.aDrvCapMask = DC_CAP_F07;	break;
		case 5:	tDriveInfo.aDrvCapMask = DC_CAP_F02;	break;
		case 6:	tDriveInfo.aDrvCapMask = DC_CAP_F05;	break;
		case 7:	tDriveInfo.aDrvCapMask = DC_CAP_F08;	break;
		default:
		return -1;
	}
	tDriveInfo.aNumOfButton = gfButton_GetButtonNumber();
	tDriveInfo.aNumOfStaLED = gfStaLED_GetLEDNumber();
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
								DC_MsgCenter_MaxMesgLen,\
								NVC_QUERY_DRIVER_CAPACITY_RESP,\
								0x00 ,0x00 , (uint8*)&tDriveInfo,\
								sizeof(mNVC_DRV_CAP_INFO));	
	
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}	
	
	return 0;
}


/**
parameter:
	iUID	The User ID of who wanna send 
return:
	0 		operating success
	-1		the Set Buffer not enough
	-2 		the Message queue have something wrong
*/
static int32 sfSendDrvButtonInfo(uint32 iUID,mCmdHead *iCmdHeader)
{
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_BUTTON_STATUS tButStatus;
	
	//tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_BUTTON_STATUS);
	
	
	switch(iCmdHeader->aUnit){
		case 0:
		case 1:{
			uint8 tButtStatus;
			
			tButtStatus = gfDrv_ButtonStatus(iCmdHeader->aUnit,1);
			
			if(tButtStatus&0x10){
				tButStatus.aStatus = DC_NVBUTT_ButtonUp;
			}else{
				tButStatus.aStatus = DC_NVBUTT_ButtonDown;
			}
			
		}break;
		
		default:
		return -1;
	}
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
								DC_MsgCenter_MaxMesgLen,\
								NVC_QUERY_BUTTON_STATUS_RESP,\
								0x00 ,0x00 , (uint8*)&tButStatus,\
								sizeof(mNVC_BUTTON_STATUS));
	// if this function return the error, that must error of driver,
	// so you have to guarantee your code are right.
	
	
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	return 0;
}


static int32 sfSendDrvAudioPlugInfo(uint32 iUID,void *iMsgBody,uint8 iCmd){
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	if(iCmd&DC_AuPl_CmdSet){
		mNVC_AUDIOPLUG_Msg *tInAudioMsg = (mNVC_AUDIOPLUG_Msg*)(iMsgBody);
		
		gfDrv_AudioPlugStatus(DC_AuPl_CmdSet|tInAudioMsg->aStatus);
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
									DC_MsgCenter_MaxMesgLen,\
									NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP,\
									0x00 ,0x00 , NULL,\
									0);	
		
	}else //if(iCmd&DC_AuPl_CmdGet)
	{
		mNVC_AUDIOPLUG_Msg tAudioStatus;
		uint8 tAuPlStatus;
		tAuPlStatus = gfDrv_AudioPlugStatus(DC_AuPl_CmdGet);
		if(tAuPlStatus&DC_AuPl_On){
			tAudioStatus.aStatus = DC_NVAuPl_On;
		}else //if(tAuPlStatus&DC_AuPl_Off)
		{
			tAudioStatus.aStatus = DC_NVAuPl_Off;
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_AUDIO_PLUG_STATUS_RESP,\
										0x00 ,0x00 , (uint8*)&tAudioStatus,\
										sizeof(mNVC_AUDIOPLUG_Msg));
		
		//tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_AUDIOPLUG_Msg);
	}
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	return 0;
}



/**
parameter:
	iUID 
	
return:
	0 success
*/
static int32 sfSendDrvDNMonitorInfo(uint32 iUID){
	
	
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_DNMONITOR_STATUS tDNMonitorStatus;
	
	tDNMonitorStatus.aStatus = gfDNMonitor_GetStatus();
	
	// if(iCmd&DC_DrvSend_Initiative){//initiative
		
	// }else{//passive
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
									DC_MsgCenter_MaxMesgLen,\
									NVC_QUERY_LDR_STATUS_RESP,\
									0x00 ,0x00 , (uint8*)&tDNMonitorStatus,\
									sizeof(mNVC_DNMONITOR_STATUS));	

	
	
	//tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_DNMONITOR_STATUS);
	
	
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	return 0;
}

static int32 sfSendDrvIrFilterInfo(uint32 iUID,void *MsgBody,uint8 iCMD)
{
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	if(iCMD&DC_IfrFilt_GetType){
		
		uint16 tIfrFiltState = gfDrv_InfraredFilterStatus(DC_IrFiltCMD_Get);
		mNVC_IRFILT_TYPE tIfrFilt;
		tIfrFilt.aType = (tIfrFiltState&0xff00)>>8;
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_IRC_TYPE_RESP,\
										0x00 ,0x00 , (uint8*)&tIfrFilt,\
										sizeof(mNVC_IRFILT_TYPE));
	//
	}else if(iCMD&DC_IfrFilt_SetState){
		
		mNVC_IRFILT_STATUS *tIfrFiltStatus = (mNVC_IRFILT_STATUS*)MsgBody;
		
		if( tIfrFiltStatus->aStatus&DC_NVIfrFilt_PassInfr ){
			gfDrv_InfraredFilterStatus(DC_IrFiltCMD_SetNight);
		}else{
			gfDrv_InfraredFilterStatus(DC_IrFiltCMD_SetDAY);
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_CONTROL_IRC_SWITCH_RESP,\
										0x00 ,0x00 , NULL,\
										0);
										
		//
	}else if(iCMD&DC_IfrFilt_GetState){
		mNVC_IRFILT_STATUS tIfrFiltStatus;
		uint16 tIfrFiltState = gfDrv_InfraredFilterStatus(DC_IrFiltCMD_Get);
		
		if(tIfrFiltState&DC_IrFiltRET_DAY){
			tIfrFiltStatus.aStatus = DC_NVIfrFilt_BlockInfr;
		}else if(tIfrFiltState&DC_IrFiltRET_NIGHT){
			tIfrFiltStatus.aStatus = DC_NVIfrFilt_PassInfr;
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_IRC_STATUS_RESP,\
										0x00 ,0x00 , (uint8*)&tIfrFiltStatus,\
										sizeof(mNVC_IRFILT_STATUS));
		//
	}else{
		return -1;
	}
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	
	return 0;
}


static int32 sfSendDrvIfrLightInfo(uint32 iUID,void *MsgBody,uint8 iCMD)
{
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	if(iCMD&DC_IfrLight_SetState){
		
		mNVC_IRLIGHT_STATUS *tIfrLightState;
		tIfrLightState  = (mNVC_IRLIGHT_STATUS*)MsgBody;
		
		if(tIfrLightState->aStatus&DC_NVIrLight_ON){
			gfDrv_InfraredLightStatus(DC_IrLight_CMDSet|DC_IrLight_StaOn);
		}else{ //if(tIfrLightState->aStatus&DC_NVIrLight_OFF)
			gfDrv_InfraredLightStatus(DC_IrLight_CMDSet|DC_IrLight_StaOff);
		}
		
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_CONTROL_LAMP_SWITCH_RESP,\
										0x00 ,0x00 , NULL,\
										0);
	
		//tMsgLength = DC_MsgCenter_MsgHeaderSize;
		
	}else if(iCMD&DC_IfrLight_GetState){
		mNVC_IRLIGHT_STATUS tsIfrLightState; //t the s struct
		uint8 tIfrLightState;
		
		tIfrLightState = gfDrv_InfraredLightStatus(DC_IrLight_CMDGet);
		
		if(tIfrLightState&DC_IrLight_StaOn){
			tsIfrLightState.aStatus = DC_NVIrLight_ON;
		}else{
			tsIfrLightState.aStatus = DC_NVIrLight_OFF;
		}
		
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_CONTROL_LAMP_SWITCH_RESP,\
										0x00 ,0x00 , (uint8*)&tsIfrLightState,\
										sizeof(mNVC_IRLIGHT_STATUS));
	
		// tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_IRLIGHT_STATUS);
		
	}else{
		return -1;
	}
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	
	return 0;
}

static int32 sfSendDrvStateLEDInfo(uint32 iUID,mCmdHead *MsgHead,void *MsgBody,uint8 iCMD)
{
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	mNVC_STALIGHT_SETINFO *tStaLight_Info;
	
	tStaLight_Info = (mNVC_STALIGHT_SETINFO*)MsgBody;
	
	gfStaLED_SetLEDStatus(tStaLight_Info->aOnMes,tStaLight_Info->aOffMes,MsgHead->aUnit);
	
	
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
									DC_MsgCenter_MaxMesgLen,\
									NVC_CONTROL_STATE_LED_RESP,\
									0x00 ,0x00 , NULL,\
									0);

	// tMsgLength = DC_MsgCenter_MsgHeaderSize;
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	
	
	return 0;
}


static int32 sfSendDrvDubleLensInfo(uint32 iUID,void *MsgBody,uint8 iCMD){
	
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	if(iCMD&DC_DubLens_SetState){
		mNVC_DUBLENS_SETINFO *tSetInfo = (mNVC_DUBLENS_SETINFO*)MsgBody;
		
		if( MsgBody == NULL ){
			return -1;
		}
		
		if(tSetInfo->aState == DC_DUBLENS_NIGHTLENS)
			gfDoubLens_ASG_TheLensState(DC_DubLens_CmdSet|DC_DubLens_NightLens);
		else if(tSetInfo->aState == DC_DUBLENS_DAYLENS)
			gfDoubLens_ASG_TheLensState(DC_DubLens_CmdSet|DC_DubLens_DayLens);
			
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_CONTROL_LENS_SWITCH_RESP,\
										0x00 ,0x00 , NULL,\
										0);

		
		
	}else if(iCMD&DC_DubLens_GetState){
		mNVC_DUBLENS_SETINFO tSetInfo;
		uint8 tLensInfo = gfDoubLens_ASG_TheLensState(DC_DubLens_CmdGet);
		if(tLensInfo == DC_DubLens_DayLens)
			tSetInfo.aState = DC_DUBLENS_DAYLENS;
		else if(tLensInfo == DC_DubLens_NightLens)
			tSetInfo.aState = DC_DUBLENS_NIGHTLENS;
		else {
			return -1;
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_LENS_STATUS_RESP,\
										0x00 ,0x00 , (uint8 *)&tSetInfo,\
										sizeof(mNVC_DUBLENS_SETINFO));
		
	}else{
		return -1;
	}
	
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	
	return 0;
}




static int32 sfSendDrvTempMonitorInfo(uint32 iUID,void *MsgBody,uint8 iCMD){
	
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	if(iCMD&DC_TempMonitor_SetReport){
		mNVC_TEMPMONITOR_Timer *tSetInfo;
		tSetInfo = (mNVC_TEMPMONITOR_Timer*)MsgBody;
		
		
		if(tSetInfo->aTimes){
			gfTempMonitor_SetReport(DC_TEMPMONITOR_REPORT_Start,tSetInfo->aTimes);
		}else{
			gfTempMonitor_SetReport(DC_TEMPMONITOR_REPORT_Stop,tSetInfo->aTimes);
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_SET_TEMPERATURE_TIMER_RESP,\
										0x00 ,0x00 , NULL,\
										0);
		
		gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength);
	}else if(iCMD&DC_TempMonitor_GetData){ //maj
		
		mNVC_TEMPMONITOR_Value tGetInfo;
		tGetInfo.aValue = gfTempMonitor_GetNowValue();
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_TEMPERATURE_VLAUE_RESP,\
										0x00 ,0x00 , (uint8*)&tGetInfo,\
										sizeof(mNVC_TEMPMONITOR_Value));
		gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength);
		
	}else{
		return -1;
	}
	
	return 0;
}




static int32 sfSendDrvHumidityMonitorInfo(uint32 iUID,void *MsgBody,uint8 iCMD){
		uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
		uint16 tMsgLength;
	
	if(iCMD&DC_HumiidityMonitor_SetReport){
		mNVC_HUMIDITY_Timer *tSetInfo;
		tSetInfo = (mNVC_HUMIDITY_Timer*)MsgBody;
		

		if(tSetInfo->aTimes){
			gfHumidityMonitor_SetReport(DC_HUMIDITY_REPORT_Start,tSetInfo->aTimes);
		}else{
			gfHumidityMonitor_SetReport(DC_HUMIDITY_REPORT_Stop,tSetInfo->aTimes);
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_SET_HUMIDIT_TIMER_RESP,\
										0x00 ,0x00 , NULL,\
										0);
		if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		
			return -1;
		}
	}else if(iCMD&DC_HumidityMonitor_GetData){ //maj
			
		mNVC_HUMIDITY_Value tGetInfo;
		
		tGetInfo.aValue = (uint32)gfHumidity_GetNowValue();
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_TEMPERATURE_VLAUE_RESP,\
										0x00 ,0x00 , (uint8*)&tGetInfo,\
										sizeof(mNVC_HUMIDITY_Value));
		gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength);
		
	}else{
		return -1;
	}
	return 0;
}




static int32 sfSendDrvNightLightInfo(uint32 iUID,void *MsgBody,uint8 iCMD){
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	if(iCMD&DC_NightLight_SetState){
		mNVC_NIGHTLIGHT_STATUS *tSetInfo;
		tSetInfo = (mNVC_NIGHTLIGHT_STATUS*)MsgBody;
		
		if((tSetInfo->aStatus) == 0x00){
			gfNightLight_AdjustLight(DC_NiLi_CmdSet|DC_NiLi_Off|DC_NiLi_AdjustBrightness,0x00);
		}else{
			gfNightLight_AdjustLight(DC_NiLi_CmdSet|DC_NiLi_On|DC_NiLi_AdjustBrightness,tSetInfo->aLemLevel);
		}
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP,\
										0x00 ,0x00 , NULL,\
										0);
		//tMsgLength = DC_MsgCenter_MsgHeaderSize;
		
	}else if(iCMD&DC_NightLight_GetState){
		
		int32 tNiLiRet;
		
		mNVC_NIGHTLIGHT_STATUS tGetInfo;
		tNiLiRet = gfNightLight_AdjustLight(DC_NiLi_CmdGet,0x00);
		if(tNiLiRet<0){
			return -1;
		}
		if(tNiLiRet&DC_NiLi_On)
			tGetInfo.aStatus 	= 1;
		else
			tGetInfo.aStatus 	= 0;
		tGetInfo.aLemLevel		= (tNiLiRet>>16)&0xFFF;
		
		tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_NIGHT_LIGHT_STATUS_RESP,\
										0x00 ,0x00 , (uint8*)&tGetInfo,\
										sizeof(mNVC_NIGHTLIGHT_STATUS));
		
	}else{
		return -1;
	}
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	
	return 0;
	
}




// 	return 
//	0		success
//	-1		product type error
//	-2		Transmit moto speed error
// 	-3		
// #define DC_PanTile_SetNormal	0x01
// #define DC_PanTile_GetInfo		0x02
static int32 sfSendDrvPanTiltInfo(uint32 iUID,void *MsgBody,uint8 iCMD){
	
	uint8	tErrorCode = 0 ;
	uint8 	tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 	tMsgLength;
	
	if(iCMD&DC_PanTile_GetInfo){
		
		mNVC_PANTILT_INFO tGetInfo;
		
		if(gfGetPanTiltInfo(&tGetInfo)){
			tErrorCode |= 	NVC_DRIVER_ERR_NOT_SUPPORT;
		}
		
		tMsgLength = 	gfMsgCenter_UnionMsgBody(\
										tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_QUERY_PTZ_INFO_RESP,\
										0x00 ,tErrorCode , (uint8*)&tGetInfo,\
										sizeof(mNVC_PANTILT_INFO));
		
	}else
	if(iCMD&DC_PanTile_SetNormal){
		
		mNVC_PANTILT_SET 	*tSetInfo;
		uint8 				tCmdToPT = 0;
							tSetInfo = (mNVC_PANTILT_SET*)MsgBody;
		
		
		switch(tSetInfo->aCmd){
			case NV_PTZ_STOP:			tCmdToPT |= DC_SSMM_Stop;
			break;
			case NV_PTZ_UP:				tCmdToPT |= DC_SSMM_VSet;
			break;
			case NV_PTZ_DOWN:			tCmdToPT |= DC_SSMM_VSet|DC_SSMM_VDown;
			break;
			case NV_PTZ_LEFT:			tCmdToPT |= DC_SSMM_HSet|DC_SSMM_HLeft;
			break;
			case NV_PTZ_RIGHT:			tCmdToPT |= DC_SSMM_HSet;
			break;
			case NV_PTZ_LEFT_UP:		tCmdToPT |= DC_SSMM_VSet|DC_SSMM_HSet|DC_SSMM_HLeft;
			break;
			case NV_PTZ_LEFT_DOWN:		tCmdToPT |= DC_SSMM_VSet|DC_SSMM_HSet|DC_SSMM_HLeft|DC_SSMM_VDown;
			break;
			case NV_PTZ_RIGHT_UP:		tCmdToPT |= DC_SSMM_VSet|DC_SSMM_HSet;
			break;
			case NV_PTZ_RIGHT_DOWN:		tCmdToPT |= DC_SSMM_VSet|DC_SSMM_HSet|DC_SSMM_VDown;
			break;
			case NV_PTZ_Cordin:			tCmdToPT |= DC_SSMM_Crd;
				break;
			default: 					return -1;
		}
		
		switch( tSetInfo->aParaType ){
			case 0:						tCmdToPT |=	DC_SSMM_Steps; 	break;
			case 1:						tCmdToPT |=	DC_SSMM_Dgr; 	break;
			case 2:						tCmdToPT |=	DC_SSMM_Crd;	break;
			default: return -1;
		}
		
		
		{
			uint32 					tMoToReturn;
			mNVC_PANTILT_Respons 	tPanTiltInfo;
			gfMemset((uint8*)&tPanTiltInfo,0,sizeof(mNVC_PANTILT_Respons));
			// Set the Cmd and see what will be happen
			tMoToReturn = 	gfStartPTMove(\
							tSetInfo->aHParam,\
							tSetInfo->aVParam,\
							tSetInfo->aSpeed,\
							tCmdToPT);
			
				
			// handle the return status msg
			if( 	(tMoToReturn&DC_SPMS_HOverLim)||\
					(tMoToReturn&DC_SPMS_VOverLim)){//////////////// 9
				tErrorCode 				= NVC_DRIVER_ERR_LIMITED; 
			}else if((tMoToReturn&DC_SPMS_HMoveWorning)||\
					(tMoToReturn&DC_SPMS_VMoveWorning)){/////////// 8
				tErrorCode 				= NVC_DRIVER_ERR_UNFINISHED;
			}else if(tMoToReturn&DC_SPMS_Busy){//////////////////// 4
				tErrorCode 				= NVC_DRIVER_ERR_BUSY;
			}else if((tMoToReturn&DC_SPMS_InputEmpty)||\
					(tMoToReturn&DC_SPMS_OverSpeed)){
				tErrorCode 				= NVC_DRIVER_ERR_PARAM;//// 3
			}else if((tMoToReturn&DC_SPMS_BufIdle		)){
				tPanTiltInfo.aStatus 	|= NV_PTZ_STA_RemainSpace;
				tPanTiltInfo.aStatus 	|= NV_PTZ_STA_Success;
			}
			
			
			tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
											DC_MsgCenter_MaxMesgLen,\
											NVC_CONTROL_PTZ_COMMON_RESP,\
											0x00 ,tErrorCode , (uint8*)&tPanTiltInfo,\
											sizeof(mNVC_PANTILT_Respons));
		}
		
	}else{return -2;}
	
	if(gfAddAUserMsgForRmd(iUID,tMsgBody,tMsgLength)){
		return -1;
	}
	
	return 0;
}




///////////////////////////////////////////////////////////////////////
// ////////  ////////  ////////  ////////  ////////  ////////  ////////
/////////////////////////////////////////////////////////////////////// 
//						Message Deal tools
/**
parameter:
	*iTar
	iTarLen
	iCmdType
	iUnit
	iErrCode
	*iMsg
	iMsgLe
return:
	NULL		space not enough
	pToTailAddr
*/
static uint16 gfMsgCenter_UnionMsgBody(\
				uint8 *iTar,\
				uint16 iTarLen,\
				uint16 iCmdType,\
				uint8 iUnit,\
				uint8 iErrCode,\
				uint8 *iMsg,\
				uint16 iMsgLen)
{
	uint8 *tTTools;
	uint16 tMagicWord = DC_MsgCenter_MagicWord; 
	
	if(iTarLen<(DC_MsgCenter_MsgHeaderSize+iMsgLen)){
		return 0;
	}
	tTTools = iTar;
	tTTools = gfStringCopy(tTTools,(uint8*)&tMagicWord,2);
	tTTools = gfStringCopy(tTTools,(uint8*)&iCmdType,2);
	tTTools = gfStringCopy(tTTools,(uint8*)&iMsgLen,2);
	tTTools = gfStringCopy(tTTools,(uint8*)&iUnit,1);
	tTTools = gfStringCopy(tTTools,(uint8*)&iErrCode,1);
	tTTools = gfMemset(tTTools,0,4);
	
	if(iMsg != NULL)
		tTTools = gfStringCopy(tTTools,iMsg,iMsgLen);
	
	return (uint16)(tTTools-iTar);
}



/**
parameter:
	iSour		the User Command buffer point
	iCmdHeader	the message header buffer point
return:
	0 	operationg success
	-1	Magic world was err
*/
static int32 sfMsgCenter_GetMsgHeader(void *iSour,mCmdHead *iCmdHeader){
//int32 gfMsgCenter_GetMsgHeader(void *iSour,mCmdHead *iCmdHeader){
	
	void *tMsgBuf = iSour;
	iCmdHeader->aMagicWord =  *(uint16*)tMsgBuf;
	tMsgBuf+=2;
	
	
	if(DC_MsgCenter_MagicWord != iCmdHeader->aMagicWord)
		return -1;
	
	iCmdHeader->aMsgTypes =  *(uint16*)tMsgBuf;
	tMsgBuf+=2;
	
	iCmdHeader->aMsgLen =  *(uint16*)tMsgBuf;
	tMsgBuf+=2;
	
	iCmdHeader->aUnit =  *(uint8*)tMsgBuf;
	tMsgBuf+=1;
	
	iCmdHeader->aErrCode =  *(uint8*)tMsgBuf;
	tMsgBuf+=1;
	
	iCmdHeader->aRemain =  *(uint32*)tMsgBuf;
	
	return 0;
}


///////////////////////////////////////////////////////////////////////
// ////////  ////////  ////////  ////////  ////////  ////////  ////////
/////////////////////////////////////////////////////////////////////// 
//						Period Event
static void  sfReportDNMonitorInfo(uint8 iStatus){
	
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_DNMONITOR_STATUS tDNMonitorStatus;
	
	tDNMonitorStatus.aStatus = iStatus;
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
									DC_MsgCenter_MaxMesgLen,\
									NVC_REPORT_LDR_STATUS_MSG,\
									0x00 ,0x00 , (uint8*)&tDNMonitorStatus,\
									sizeof(mNVC_DNMONITOR_STATUS));	
	
	//tMsgLength = DC_MsgCenter_MsgHeaderSize+sizeof(mNVC_DNMONITOR_STATUS);
	
	gfAddAUserMsgForSub(tMsgBody,tMsgLength);
}



static void sfPE_TempReport(int32 tTemp){
	//int tRet;
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_TEMPMONITOR_Value tGetInfo;
	
	tGetInfo.aValue = (uint32)tTemp;
	
	// printk( "---------->Temp: %d\r\n",(int)tGetInfo.aValue );

	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_REPORT_TEMPERATURE_VLAUE_MSG,\
										0x00 ,0x00 , (uint8*)&tGetInfo,\
										sizeof(mNVC_TEMPMONITOR_Value));
	
	gfAddAUserMsgForSub(tMsgBody,tMsgLength);
	
}



static void sfPE_HumidityReport(int32 iHumidity){
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	mNVC_HUMIDITY_Value tGetInfo;
	
	tGetInfo.aValue = (uint32)iHumidity;
	// printk( "---------->iHumidity: %d\r\n", (int)tGetInfo.aValue );
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_REPORT_HUMIDIT_VLAUE_MSG,\
										0x00 ,0x00 , (uint8*)&tGetInfo,\
										sizeof(mNVC_HUMIDITY_Value));
	
	gfAddAUserMsgForSub(tMsgBody,tMsgLength);
	
}

/**
parameter:
	
	void
	
return:
	void
*/
static void sfPE_ButtonEvent(void){

	//因为这个事件时发生在内核中断里面的，所以要做好错误事件的处理
	
	// 因为是突发事件，所以里会依据读取的案件返回值来平衡到底上传什么数据，
	// 如果案件有多个，那么可能要做两次发送
	
	uint8 _i;
	uint8 tButtonBum;
	uint8 tButtStatus;
	mNVC_BUTTON_STATUS tButStatus;
	
	tButtonBum = gfButton_GetButtonNumber();
	
	for(_i = 0; _i<tButtonBum; _i++){
		
		tButtStatus = gfDrv_ButtonStatus(_i,2);
		if((tButtStatus&0x0f)==DC_NSB_FallingEdge){
			tButStatus.aStatus = DC_NVBUTT_ButtonFall ;
		}else if((tButtStatus&0x0f)==DC_NSB_RasingEdge){
			tButStatus.aStatus = DC_NVBUTT_ButtonRaise;
		}else{
			continue;
		}
		{	
			uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
			uint16 tMsgLength;
			
			
			tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
										DC_MsgCenter_MaxMesgLen,\
										NVC_REPORT_BUTTON_STATUS_MSG,\
										_i ,0x00 , (uint8*)&tButStatus,\
										sizeof(mNVC_BUTTON_STATUS));
			
			gfAddAUserMsgForSub(tMsgBody,tMsgLength);
		}
	}
}

static void sfReportPanTiltInfo(uint16 iXRod,uint16 iYRod){
	
	uint8 tMsgBody[DC_MsgCenter_MaxMesgLen];
	uint16 tMsgLength;
	
	mNVC_PANTILT_INFO tGetInfo;
	if(gfGetPanTiltInfo(&tGetInfo)){
		return ;
	}
	
	tGetInfo.aXPos = iXRod;
	tGetInfo.aYPos = iYRod;
	
	tMsgLength = gfMsgCenter_UnionMsgBody(	tMsgBody,\
									DC_MsgCenter_MaxMesgLen,\
									NVC_REPORT_PTZ_INFO_MSG,\
									0x00 ,0x00 , (uint8*)&tGetInfo,\
									sizeof(mNVC_PANTILT_INFO));
	
	gfAddAUserMsgForSub(tMsgBody,tMsgLength);
	
}
