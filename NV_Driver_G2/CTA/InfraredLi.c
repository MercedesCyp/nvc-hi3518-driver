// ================================================================================
//| 默认：
//| 	小夜灯只有一个控制引脚
//| 	引脚高电平为开灯
//| 	引脚低电平为关灯
//| 	默认引脚 GPIO0_0
//| 
//| 适用：
//| 	D01 D03 D04 D11 F05 F08
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "CTA.h"
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"










static int32 sfdefIfrLi_Init(void);
static int32 sfdefIfrLi_Uninit(void);
static void  sfdefIfrLi_SetStatus(uint32 iCmd);
static int32 sfdefIfrLi_GetStatus(void);











static mGPIOPinIfo sdefPin_IfrLi = {	0,0,72,0x00 };

static mGPIOPinArr sdefPinArr_IfrLi = {
	.apPin			=	&sdefPin_IfrLi,
	.aNum 			=	1
};

mClass_IfrLi gClassIfrLi = {
	.apPinArr       =   &sdefPinArr_IfrLi,
	.afInit 		=	sfdefIfrLi_Init,
	.afUninit		=	sfdefIfrLi_Uninit,
	.afSetStatus 	=	sfdefIfrLi_SetStatus,
	.afGetStatus 	=	sfdefIfrLi_GetStatus,
	.OnStatus		=	0x01
};










static int32 sfdefIfrLi_Init(void){
	mGPIOPinIfo 	*tPin 	= gClassIfrLi.apPinArr->apPin;

    gClassHAL.Pin->prfSetDirection( tPin, DC_HAL_PIN_SetOUT );

	// DC_PIN_SET_IO_OUT( tPin->aGroup, tPin->aBit, tPin->aMuxVal, tPin->aMux );

	
#if DEBUG_INIT
	NVCPrint("The Infrared Light module Start! OK");
#endif
	return 0;
}

static int32 sfdefIfrLi_Uninit(void){
	
	
	return 0;
}

static void sfdefIfrLi_SetStatus(uint32 iCmd){

	mGPIOPinIfo 	*tPin 	= gClassIfrLi.apPinArr->apPin;
	if( iCmd&DC_IfLi_On ){
		// printk("------>ifr li on\r\n");
		if( gClassIfrLi.OnStatus ){
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetHIGH );
			// DF_Set_GPIO_Outx( tPin->aGroup, tPin->aBit );
		}else{
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetLOW );
			// DF_Clr_GPIO_Outx( tPin->aGroup, tPin->aBit );	
		}
	}else if( iCmd&DC_IfLi_Off ){
		// printk("------>ifr li off\r\n");
		if( gClassIfrLi.OnStatus ){
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetLOW );
			// DF_Clr_GPIO_Outx( tPin->aGroup, tPin->aBit );	
		}else{
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetHIGH );
			// DF_Set_GPIO_Outx( tPin->aGroup, tPin->aBit );
		}
	}
}
static int32 sfdefIfrLi_GetStatus(void){
	mGPIOPinIfo 	*tPin 	= gClassIfrLi.apPinArr->apPin;
	int32 			tRet 	= 0;
		
	// if( DF_Get_GPIO_INx( tPin->aGroup, tPin->aBit ) ){
    if( gClassHAL.Pin->prfGetPinValue(tPin) ){
		if( gClassIfrLi.OnStatus ){
			tRet |= DC_IfLi_On;
		}else{
			tRet |= DC_IfLi_Off;
		}
	}else{
		if( gClassIfrLi.OnStatus ){
			tRet |= DC_IfLi_Off;
		}else{
			tRet |= DC_IfLi_On;
		}
	}
	return tRet;
}


