#include "ProConfig.h"


uint8 gcNVCDrv_VersionInfo[16]="V: 01.01";
uint8 gcNVCDrv_BuildInfo[32]="2015-5-19:00";



//  -------------------------------> Macro
// this driver can support product number
// 3518C D01 D03 D04 D11
// 3518E F02 F05 F07
// #define DC_PROINFO_ProductSumNum	2

// =================================================================================
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ==================== Definition Extern Pro Config Block =========================
#if defined(PLATFORM_3518)
extern mProInfo pgs3518C_D01_ProInfo;
extern mProInfo pgs3518C_D03_ProInfo;
extern mProInfo pgs3518C_D04_ProInfo;
extern mProInfo pgs3518C_D11_ProInfo;
extern mProInfo pgs3518E_F05_ProInfo;
extern mProInfo pgs3518E_F08_ProInfo;
extern mProInfo pgs3518E_F09_ProInfo;
extern mProInfo pgs3518E_0301_ProInfo;
#endif
#if defined(PLATFORM_3507)
extern mProInfo pgs3507R_B14_ProInfo;
#endif

mProInfo *poaProInfo[]={
#if defined(PLATFORM_3518)
	&pgs3518C_D01_ProInfo,
	&pgs3518C_D03_ProInfo,
	&pgs3518C_D04_ProInfo,
	&pgs3518C_D11_ProInfo,
	&pgs3518E_F05_ProInfo,
	&pgs3518E_F08_ProInfo,
    &pgs3518E_F09_ProInfo,
    &pgs3518E_0301_ProInfo,
#endif
#if defined(PLATFORM_3507)
    &pgs3507R_B14_ProInfo,
#endif
	NULL
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// =================================================================================

mClass_Pro gClassPro={
	NULL,
	NULL
};
mClass_CTA *gSysCTA;

//  -------------------------------> Global Function
//|================================================================================
//| 	Function NAME:
//| 		gfIsTheInputNumExist
//|	
//| 	Param:
//| 		iChip	Chip type
//|		iPro	Product type
//|
//| 	Return:
//|		DC_DrivNumErrMask	this product not exist
//|		DC_DrivNumErrMem	Create CTA fail
//|		0x0000				success
//|
//|================================================================================
int32 gfIsTheInputNumExist(uint8 *iChip,uint8 *iPro,uint8 *iSubCmd )
{
	uint16 _i = 0;
	while( poaProInfo[_i] != NULL ){
		if( 0 == gClassStr.afMatch( poaProInfo[_i]->aPro.aKey, iPro) ){
			if( 0 == gClassStr.afMatch( poaProInfo[_i]->aChip.aKey, iChip) ){
				NVCPrint("The Get Pro ID is:%d",_i);
				break;
			}
		}
		_i++;
	}
	
	if( NULL == poaProInfo[_i] ){
		return DC_DrivNumErrMask;
	}else{
		
		if( poaProInfo[_i]->aSubCmd != NULL ){
			uint8	tCmdNum			=	poaProInfo[_i]->aSubCmd->aNum;
			mArr	*tProSubCmd		=	poaProInfo[_i]->aSubCmd->apCmd;
			uint8	_j;
			for( _j=0; _j<tCmdNum; _j++ ){
				if( 0 == gClassStr.afMatch( tProSubCmd[_j].aKey, iSubCmd) ){
					poaProInfo[_i]->aSubCmd->afCmdProcessing( tProSubCmd[_j].aID );
					break;
				}
			}
		}
		
		gClassPro.apLogedPro	= poaProInfo[_i];
		gClassPro.apCTA			= gClassPro.apLogedPro->afRewriteAndConfig();
		if( gClassPro.apCTA == NULL ){
			NVCPrint_err("The Pro's CTA structural is not definition! -->ProConfig.c(gfIsTheInputNumExist)");
			return DC_DrivNumErrMem;
		}
		gSysCTA = gClassPro.apCTA;
		return 0;
	}
}
