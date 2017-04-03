/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
#include "ProAbility.h"
#include "ProConfig.h"
// remote
#include "../GlobalParameter.h"
#include "../HAL/HAL.h"
#include "../Tool/String.h"
#include "../Tool/MsgProtocal.h"

//==============================================================================
//extern
//local
static int32 sfProBltyInit_DriverInfo(mProInfo *ProInfo);
static int32 sfProBltyInit_DriveCap(mClass_CTA *iCTA);
//global

//==============================================================================
//extern
extern uint8 gcNVCDrv_VersionInfo[];
extern uint8 gcNVCDrv_BuildInfo[];
//local
//global
mNVC_DRV_Ver_INFO NVCDriverInfo;
mNVC_DRV_CAP_INFO NVCDriverCap;

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int32 gfProBltyInit__(void)
@introduction:
    根据加载的机型，提取出基本信息，包括 版本，建立时间，能力集

@parameter:
    void

@return:
    0   初始化成功
    -1  初始化失败

*/
int32 gfProBltyInit__(void)
{

	if (gClassPro.apLogedPro->afInit()) {
		return -1;
	}
	sfProBltyInit_DriverInfo(gClassPro.apLogedPro);
	sfProBltyInit_DriveCap(gClassPro.apCTA);
	return 0;
}

//---------- ---------- ---------- ----------
/*  int32 gfProBltyUninit__(void)
@introduction:
    释放加载驱动时注册的资源

@parameter:
    void

@return:
    0   成功释放资源

*/
int32 gfProBltyUninit__(void)
{

	gClassPro.apLogedPro->afUninit();
	return 0;
}

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfProBltyInit_DriverInfo(mProInfo *iProInfo)
@introduction:
    初始化驱动的版本信息

@parameter:
    iProInfo
        指向欲加载机型的驱动配置对象（结构体）

@return:
    0       success

*/
static int32 sfProBltyInit_DriverInfo(mProInfo *iProInfo)
{

	NVCDriverInfo.aChipInfo = iProInfo->aChip.aID;
	NVCDriverInfo.aDeviceInfo = iProInfo->aPro.aID;
	gClassStr.afCopy((uint8 *) & NVCDriverInfo.aVerInfo,
	                 gcNVCDrv_VersionInfo, 16);
	gClassStr.afCopy((uint8 *) & NVCDriverInfo.aBuildData,
	                 gcNVCDrv_BuildInfo, 32);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfProBltyInit_DriveCap( mClass_CTA *iCTA )
@introduction:
    获取对象能力集

@parameter:
    iProInfo
        指向欲加载机型的驱动配置对象的方法类（结构体）

@return:
    0       success

*/
static int32 sfProBltyInit_DriveCap(mClass_CTA *iCTA)
{

	uint32 tDrvCapMask = 0;
	// ========================================
	uint32 tCpaMask = 0x00000001;
	uint8 _i;
	void **tTool = (void **)gSysCTA;
	for (_i = 0; _i < DC_CTAMethodNum; _i++, tTool++, tCpaMask <<= 1)
		if (*tTool != 0) {
			tDrvCapMask |= tCpaMask;
		}
	// printk("- - -- - - - - ->%x\r\n",(unsigned int)tCreateMask );
	/*
	   // ========================================
	   if( iCTA->apAudioPlug        != NULL )
	   tDrvCapMask |= CAP_SUPP_AudioPlug;
	   if( iCTA->apNightLi  != NULL )
	   tDrvCapMask |= CAP_SUPP_NightLight;
	   if( iCTA->apIfrLi            != NULL )
	   tDrvCapMask |= CAP_SUPP_IfrLamp;
	   if( iCTA->apStateLi  != NULL )
	   tDrvCapMask |= CAP_SUPP_StateLed;
	   if( iCTA->apDubLens  != NULL )
	   tDrvCapMask |= CAP_SUPP_DoubleLens;
	   if( iCTA->apIfrFilter        != NULL )
	   tDrvCapMask |= CAP_SUPP_Ircut;
	   if( iCTA->apButton           != NULL )
	   tDrvCapMask |= CAP_SUPP_ButtonMonitor;
	   if( iCTA->apPTZ              != NULL )
	   tDrvCapMask |= CAP_SUPP_PTZ;
	   if( iCTA->apLDR              != NULL )
	   tDrvCapMask |= CAP_SUPP_LdrMonitor;
	   if( iCTA->apTemp             != NULL )
	   tDrvCapMask |= CAP_SUPP_TempMonitor;
	   if( iCTA->apHumidity         != NULL )
	   tDrvCapMask |= CAP_SUPP_HumiMonitor;
	   if( iCTA->apRTC         != NULL )
	   tDrvCapMask |= CAP_SUPP_RTC;
	   if( iCTA->apPIR         != NULL )
	   tDrvCapMask |= CAP_SUPP_PIR;
	   if( iCTA->apDoorBell    != NULL )
	   tDrvCapMask |= CAP_SUPP_DoorBell;
	 */

	NVCDriverCap.aDrvCapMask = tDrvCapMask;
	NVCPrint("Capability:  0x%x", (unsigned int)tDrvCapMask);
	return 0;
}
