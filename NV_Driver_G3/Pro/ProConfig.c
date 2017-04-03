/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
#include "ProConfig.h"
// remote

//==============================================================================
//extern
#if defined(PLATFORM_3507)
extern mProInfo pgs3507R_B14_ProInfo;
#endif
#if defined(PLATFORM_3518)
// extern mProInfo pgs3518C_D01_ProInfo;
// extern mProInfo pgs3518C_D03_ProInfo;
extern mProInfo pgs3518C_D04_ProInfo;
extern mProInfo pgs3518C_D11_ProInfo;
extern mProInfo pgs3518E_F05_ProInfo;
extern mProInfo pgs3518E_F08_ProInfo;
extern mProInfo pgs3518E_F09_ProInfo;
extern mProInfo pgs3518E_0301_ProInfo;
extern mProInfo pgs3518E_F10_ProInfo;
extern mProInfo pgs3518E_0302_ProInfo;
extern mProInfo pgs3518E_F16_ProInfo;
extern mProInfo pgs3518E_F17_ProInfo;
extern mProInfo pgs3518E_G02_ProInfo;
extern mProInfo pgs3518E_G03_ProInfo;
extern mProInfo pgs3518E_G05_ProInfo;
extern mProInfo pgs3518E_G06_ProInfo;
extern mProInfo pgs3518E_G07_ProInfo;
extern mProInfo pgs3518E_G09_ProInfo;
extern mProInfo pgs3518E_G11_ProInfo;

#endif

#if defined(PLATFORM_3518EV200)
extern mProInfo pgs3518EV200_G16_ProInfo;
extern mProInfo pgs3518EV200_G26_ProInfo;
extern mProInfo pgs3518EV200_H04_ProInfo;
#endif

//local
uint8 gcNVCDrv_VersionInfo[16] = DC_NVDRIV_VERSION;
uint8 gcNVCDrv_BuildInfo[32] = __DATE__;
//global
mProInfo *poaProInfo[] = {
#if defined(PLATFORM_3518)
	// &pgs3518C_D01_ProInfo,
	// &pgs3518C_D03_ProInfo,
	&pgs3518C_D04_ProInfo,
	&pgs3518C_D11_ProInfo,
	&pgs3518E_F05_ProInfo,
	&pgs3518E_F08_ProInfo,
	&pgs3518E_F09_ProInfo,
	&pgs3518E_0301_ProInfo,
	&pgs3518E_F10_ProInfo,
	&pgs3518E_0302_ProInfo,
	&pgs3518E_F16_ProInfo,
	&pgs3518E_F17_ProInfo,
	&pgs3518E_G02_ProInfo,
	&pgs3518E_G03_ProInfo,
	&pgs3518E_G05_ProInfo,
	&pgs3518E_G06_ProInfo,
	&pgs3518E_G07_ProInfo,
	&pgs3518E_G09_ProInfo,
	&pgs3518E_G11_ProInfo,
#endif
#if defined(PLATFORM_3507)
	&pgs3507R_B14_ProInfo,
#endif
#if defined(PLATFORM_3518EV200)
	&pgs3518EV200_G16_ProInfo,
	&pgs3518EV200_G26_ProInfo,
	&pgs3518EV200_H04_ProInfo,
#endif
	NULL
};

mClass_Pro gClassPro = {
	NULL,
	NULL
};

mClass_CTA *gSysCTA;

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int32 gfIsTheInputNumExist(uint8 *iChip,uint8 *iPro,uint8 *iSubCmd )
@introduction:
    匹配输入机型，如匹配成功，则加载相应机型驱动

@parameter:
    iChip
        加载驱动时输入的芯片类型
    iPro
        加载驱动时输入的产品类型
    iSubCmd
        对该机型进行预初始化命令

@return:
    DC_DrivNumErrMask	this product not exist
    DC_DrivNumErrMem	Create CTA fail
    DC_DrivChipErr		Parameter Chip not exist
    DC_DrivProErr			Parameter Pro not exist
    0x0000				success

*/
int32 gfIsTheInputNumExist(uint8 *iChip, uint8 *iPro, uint8 *iSubCmd)
{
	uint16 _i = 0;
	uint16 _searchChipFlag = 0;
	uint16 _searchProFlag = 0;

	while (poaProInfo[_i] != NULL) {
		if (0 == gClassStr.afMatch(poaProInfo[_i]->aChip.aKey, iChip)) {
			_searchChipFlag++;
			if (0 ==
			    gClassStr.afMatch(poaProInfo[_i]->aPro.aKey,
			                      iPro)) {
				_searchProFlag++;
				NVCPrint("The Get Pro ID is:%d", _i);
				break;
			}
		}
		_i++;
	}

	if (NULL == poaProInfo[_i]) {
		if (_searchChipFlag == 0) {
			return DC_DrivChipErr;
		}
		if (_searchChipFlag != 0 && _searchProFlag == 0) {
			return DC_DrivProErr;
		}
		return DC_DrivNumErrMask;
	} else {
		if ((iSubCmd != NULL) && (poaProInfo[_i]->aSubCmd != NULL)) {
			uint8 tCmdNum = poaProInfo[_i]->aSubCmd->aNum;
			mArr *tProSubCmd = poaProInfo[_i]->aSubCmd->apCmd;
			uint8 _j;
			for (_j = 0; _j < tCmdNum; _j++) {
				if (0 ==
				    gClassStr.afMatch(tProSubCmd[_j].aKey,
				                      iSubCmd)) {
					printk("abc");
					poaProInfo[_i]->aSubCmd->
					afCmdProcessing(tProSubCmd[_j].aID);
					break;
				}
			}
		}

		gClassPro.apLogedPro = poaProInfo[_i];
		gClassPro.apCTA = gClassPro.apLogedPro->afRewriteAndConfig();
		if (gClassPro.apCTA == NULL) {
			NVCPrint_err
			("The Pro's CTA structural is not definition! -->ProConfig.c(gfIsTheInputNumExist)");
			return DC_DrivNumErrMem;
		}
		gSysCTA = gClassPro.apCTA;

		return 0;
	}
}
