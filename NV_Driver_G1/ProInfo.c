#include "ProInfo.h"


//  -------------------------------> Internal Variable
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
static uint8 *pagNameSumProCmd[DC_ProCmdSum_Num]={
	DC_ProCmd1_AppName,
	DC_ProCmd2_AppName,
};

uint32 pagAppChipType[DC_ChipSum_Num]={
	DC_CHIP_3518C_Appname,
	DC_CHIP_3518E_Appname,
};


static uint8 *pagNameSumChip[DC_ChipSum_Num]={
	DC_CHIP_3518C_name,
	DC_CHIP_3518E_name,
};

static uint8 *pagNameSumPro[DC_ProSum_Num]={
	DC_Pro1_Name,
	DC_Pro2_Name,
	DC_Pro3_Name,
	DC_Pro4_Name,
	DC_Pro5_Name,
	DC_Pro6_Name,
	DC_Pro7_Name,
	DC_Pro8_Name,
};

uint32 pagAppProType[DC_ProSum_Num]={
	DC_Pro1_AppName,
	DC_Pro2_AppName,
	DC_Pro3_AppName,
	DC_Pro4_AppName,
	DC_Pro5_AppName,
	DC_Pro6_AppName,
	DC_Pro7_AppName,
	DC_Pro8_AppName,
};

//  -------------------------------> Global Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
uint32 gInsmodProType=0;
//--------------------------------------------------------------->
// 	Function NAME:
// 		gfIsTheInputNumExist
//	
// 	Param:
// 		iChip	Chip type
//		iPro	Product type
//
// 	Return:
// 		(8bit Chip)<<16 + (16bit Pro)
//		DC_DrivNumErrMask
//
//--------------------------------------------------------------->
int32 gfIsTheInputNumExist(uint8 *iChip,uint8 *iPro,uint8 *iPro_Cmd)
{
	int16 tRet;
	int32 tDrivNum;
	tDrivNum = 0;
	
	tRet = gfMatchStringFromArray(pagNameSumChip,iChip,DC_ChipSum_Num);
	if(tRet<0)
		tDrivNum|=DC_DrivNumErrMask;
	else
		// gGLOBAL_ChipType = tRet; 
		tDrivNum|=((tRet<<12)&0x000FF000);
	
	tRet = gfMatchStringFromArray(pagNameSumPro,iPro,DC_ProSum_Num);
	if(tRet<0)
		tDrivNum|=DC_DrivNumErrMask;
	else
		// gGLOBAL_DeviceNum = tRet;
		tDrivNum|=tRet&0x00000FFF;
		
	if(iPro_Cmd!=NULL){
		tRet = gfMatchStringFromArray(pagNameSumProCmd,iPro_Cmd,DC_ProCmdSum_Num);
		if(tRet<0)
			tDrivNum|=DC_DrivNumErrMask;
		else
			// gGLOBAL_DeviceNum = tRet;
			tDrivNum|=(tRet<<20)&0x0FF00000;		
	}
	
	return tDrivNum;
}


//  -------------------------------> include file
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
#include "MsgCenter.h"

#include "HAL/HAL.h"
#include "ButtonMonitor/Button.h"
#include "DNMonitor/DNMonitor.h"
#include "AudioPlug/AudioPlug.h"
#include "IfrFilter/IfrFilter.h"
#include "IfrLight/IfrLight.h"
#include "StateLED/StateLED.h"
#include "DoubleLens/DoubleLens.h"
#include "TempMonitor/TempMonitor.h"
#include "Humidity/Humidity.h"
#include "NightLight/NightLight.h"
#include "PanTilt/PanTilt.h"




//  -------------------------------> Global Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
//--------------------------------------------------------------->
// 	Function NAME:
// 		gfInitAppointPro
//	
// 	Param:
// 		void
//
// 	Return:
// 		0		Success
//		-1		Error
//
//--------------------------------------------------------------->
int32 gfInitAppointPro(void)
{
	//int32 t32Ret;
	
#if OPEN_DEBUG
	NVCPrint("The Pro is:%d 	The Chip is:%d\r\n",\
	(int)DF_GLOBAL_GetDivInfo,(int)DF_GLOBAL_GetChipInfo);
#endif
	
	if(gfInitDrv_HALLeyer(0)<0)
		return -1;
	if(gfInitDrv_MsgCenter(0)<0)
		return -1;
	
	if(gfInitDrv_ButtonMonitor(0)<0)
		return -1;
	if(gfInitDrv_DNMonitor(0)<0)
		return -1;
	if(gfInitDrv_IfrFilter(0)<0)
		return -1;
	if(gfInitDrv_IfrLight(0)<0)
		return -1;
	if(gfInitDrv_StateLED(0)<0)
		return -1;
	if(gfInitDrv_AudioPlug(0)<0)
		return -1;
	if(gfInitDrv_DoubleLens(0)<0)
		return -1;
	if(gfInitDrv_NightLight(0)<0)
		return -1;
	if(gfInitDrv_TempMonitor(0)<0)
		return -1;
	if(gfInitDrv_HumidityMonitor(0)<0)
		return -1;
	if(gfInitDrv_PanTilt(0)<0)
		return -1;
	
	return 0;
}

//--------------------------------------------------------------->
// 	Function NAME:
// 		gfUninitAppointPro
//	
// 	Param:
// 		void
//
// 	Return:
// 		0		Success
//		-1		Error
//
//--------------------------------------------------------------->
int32 gfUninitAppointPro(void)
{
	gfUninitDrv_ButtonMonitor(0);
	gfUninitDrv_DNMonitor(0);
	gfUninitDrv_IfrFilter(0);
	gfUninitDrv_IfrLight(0);
	gfUninitDrv_StateLED(0);
	gfUninitDrv_AudioPlug(0);
	gfUninitDrv_DoubleLens(0);
	gfUninitDrv_NightLight(0);
	gfUninitDrv_TempMonitor(0);
	gfUninitDrv_HumidityMonitor(0);
	gfUninitDrv_PanTilt(0);
	
	gfUninitDrv_MsgCenter(0);
	gfUninitDrv_HALLeyer(0);
	return 0;
}

