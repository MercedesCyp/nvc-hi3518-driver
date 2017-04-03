// ================================================================================
//| 默认:
//|		音频控制只有一个引脚
//|		引脚低电平为开
//|		引脚高电平为关
//|		默认引脚 GPIO 0——6
//| 
//|	目前适用：
//| 	D01 D03 D04 D11 F02 F05 F07 F08
//| 
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "CTA.h"
// #include "../HAL/USERHAL.h"
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"










static int32 sfdefAuPl_Init(void);
static int32 sfdefAuPl_Uninit(void);
static void sfdefAuPl_SetStatus(uint32 iCmd);
static int32 sfdefAuPl_GetStatus(void);










static mGPIOPinIfo sdefPin_AuPl = {	0,6,78,0  };

static mGPIOPinArr sdefPinArr_AuPl = {
	.apPin			=	&sdefPin_AuPl,
	.aNum			=	1
};

mClass_AudioPlug gClassAudioPlug = {
	.apPinArr 		=	&sdefPinArr_AuPl,
	.afInit 		=	sfdefAuPl_Init,
	.afUninit		=	sfdefAuPl_Uninit,
	.afSetStatus 	=	sfdefAuPl_SetStatus,
	.afGetStatus 	=	sfdefAuPl_GetStatus,
	.OnStatus		=	0x00
};











static int32 sfdefAuPl_Init(void){
	
	// The default Control Pin Num of Audio Plug is just one.
	// uint8			tPinNum = gClassAudioPlug->apPinArr->aNum;
	mGPIOPinIfo 	*tPin 	= gClassAudioPlug.apPinArr->apPin;
	uint8 			_i 		= 0;
	// for( _i = 0; _i < tPinNum: _i++ ){
		
		// Initial multi-Register 
		// Setting IO dirction OUTPUT 
        gClassHAL.Pin->prfSetDirection( &tPin[_i], DC_HAL_PIN_SetOUT);
		// DF_Set_GPIO_MUXx( tPin[_i].aMuxVal, tPin[_i].aMux );
		// DF_Set_GPIO_DIRx( tPin[_i].aGroup, tPin[_i].aBit );	
		
		// Set the pin to hight, cut off the Audio access
		if( gClassAudioPlug.OnStatus ){
            gClassHAL.Pin->prfSetExport( &tPin[_i], DC_HAL_PIN_SetLOW);
			// DF_Clr_GPIO_Outx( tPin[_i].aGroup, tPin[_i].aBit );
		}else{
            gClassHAL.Pin->prfSetExport( &tPin[_i], DC_HAL_PIN_SetHIGH);
			// DF_Set_GPIO_Outx( tPin[_i].aGroup, tPin[_i].aBit );
		}
	// }
#if DEBUG_INIT
	NVCPrint("The Audio Plug module Start! OK");
#endif
	return 0;
}

static int32 sfdefAuPl_Uninit(void){
	return 0;
}

static void sfdefAuPl_SetStatus(uint32 iCmd){
	
	mGPIOPinIfo 	*tPin 	= gClassAudioPlug.apPinArr->apPin;
	
	if( iCmd&DC_AuPl_On ){
		if( gClassAudioPlug.OnStatus ){
            gClassHAL.Pin->prfSetExport( &tPin[0], DC_HAL_PIN_SetHIGH);
			// DF_Set_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );
		}else{
            gClassHAL.Pin->prfSetExport( &tPin[0], DC_HAL_PIN_SetLOW);
			// DF_Clr_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );	
		}
	}else if( iCmd&DC_AuPl_Off ){
		if( gClassAudioPlug.OnStatus ){
            gClassHAL.Pin->prfSetExport( &tPin[0], DC_HAL_PIN_SetLOW);
			// DF_Clr_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );	
		}else{
            gClassHAL.Pin->prfSetExport( &tPin[0], DC_HAL_PIN_SetHIGH);
			// DF_Set_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );
		}
	}
}

static int32 sfdefAuPl_GetStatus(void){
	mGPIOPinIfo 	*tPin 	= gClassAudioPlug.apPinArr->apPin;
	int32 			tRet 	= 0;
		
	// if( DF_Get_GPIO_INx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit ) ){
	if( gClassHAL.Pin->prfGetPinValue( &tPin[ 0 ] ) ){
		if( gClassAudioPlug.OnStatus ){
			tRet |= DC_AuPl_On;
		}else{
			tRet |= DC_AuPl_Off;
		}
	}else{
		if( gClassAudioPlug.OnStatus ){
			tRet |= DC_AuPl_Off;
		}else{
			tRet |= DC_AuPl_On;
		}
	}
	return tRet;
}

