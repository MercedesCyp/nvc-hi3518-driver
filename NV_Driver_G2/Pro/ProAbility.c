#include "ProAbility.h"
#include "ProConfig.h"
#include "../GlobalParameter.h"
#include "../HAL/HAL.h"
#include "../Tool/String.h"
#include "../Tool/MsgProtocal.h"

// ================================================================================
static int32 sfProBltyInit_DriverInfo(mProInfo *ProInfo);
static int32 sfProBltyInit_DriveCap( mClass_CTA *iCTA );










// ================================================================================
mNVC_DRV_Ver_INFO NVCDriverInfo;
mNVC_DRV_CAP_INFO NVCDriverCap;











// ================================================================================
// ---------------------------------------------------------------------->Function
// ------------------------------------------------------------>External
// -------------------------------------------------: gfProBltyInit__
int32 gfProBltyInit__(void){
	
	if(gClassPro.apLogedPro->afInit())
		return -1;
	sfProBltyInit_DriverInfo( gClassPro.apLogedPro );
	sfProBltyInit_DriveCap( gClassPro.apCTA );
	return 0;
}

// -------------------------------------------------: gfProBltyUninit__
int32 gfProBltyUninit__(void){
	
	gClassPro.apLogedPro->afUninit();
	return 0;
}

// ------------------------------------------------------------>Internal
// -------------------------------------------------: sfProBltyInit_DriverInfo
extern uint8 gcNVCDrv_VersionInfo[16];
extern uint8 gcNVCDrv_BuildInfo[32];

static int32 sfProBltyInit_DriverInfo(mProInfo *iProInfo){
	
	NVCDriverInfo.aChipInfo = iProInfo->aChip.aID;
	NVCDriverInfo.aDeviceInfo = iProInfo->aPro.aID;
	gClassStr.afCopy((uint8*)&NVCDriverInfo.aVerInfo,gcNVCDrv_VersionInfo,16);
	gClassStr.afCopy((uint8*)&NVCDriverInfo.aBuildData,gcNVCDrv_BuildInfo,32);
	return 0;
}


// -------------------------------------------------: sfProBltyInit_DriveCap
static int32 sfProBltyInit_DriveCap( mClass_CTA *iCTA ){
	
	uint32 tDrvCapMask = 0;
	// ========================================
	if( iCTA->apAudioPlug 	!= NULL )
		tDrvCapMask |= CAP_SUPP_AudioPlug;
	
	
	// -------->
	if( iCTA->apNightLi 	!= NULL )
		tDrvCapMask |= CAP_SUPP_NightLight;
	
	// -------->
	if( iCTA->apIfrLi 		!= NULL )
		tDrvCapMask |= CAP_SUPP_IfrLamp;
	
 	// -------->
	if( iCTA->apStateLi 	!= NULL ){
		tDrvCapMask |= CAP_SUPP_StateLed;
		NVCDriverCap.aNumOfStaLED = iCTA->apStateLi->apPinArr->aNum;
	}
	
	// -------->
	if( iCTA->apDubLens 	!= NULL )
		tDrvCapMask |= CAP_SUPP_DoubleLens;
	
	// -------->
	if( iCTA->apIfrFilter 	!= NULL )
		tDrvCapMask |= CAP_SUPP_Ircut;
	
	// -------->
	if( iCTA->apButton 		!= NULL ){
		tDrvCapMask |= CAP_SUPP_ButtonMonitor;
		NVCDriverCap.aNumOfButton = iCTA->apButton->apPinArr->aNum;
	}
	
	// -------->
	if( iCTA->apPTZ 		!= NULL )
		tDrvCapMask |= CAP_SUPP_PTZ;
	
	
	// -------->
	if( iCTA->apLDR 		!= NULL )
		tDrvCapMask |= CAP_SUPP_LdrMonitor;
	
	// -------->
	if( iCTA->apTemp 		!= NULL )
		tDrvCapMask |= CAP_SUPP_TempMonitor;
	
	// -------->
	if( iCTA->apHumidity 	!= NULL )
		tDrvCapMask |= CAP_SUPP_HumiMonitor; 
	
	NVCDriverCap.aDrvCapMask = tDrvCapMask;
	NVCPrint("Capability:  %x",(unsigned int)tDrvCapMask);
	return 0;
}
