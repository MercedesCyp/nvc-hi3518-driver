// ================================================================================
//| Ĭ�ϣ�
//| 	Сҹ��ֻ��һ����������
//| 	���Ÿߵ�ƽΪ����
//| 	���ŵ͵�ƽΪ�ص�
//| 	Ĭ������ GPIO5_3
//| 
//| ���ã�
//| 	D03
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "CTA.h"
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"
#include "../Tool/String.h"










static int32 sfdefNiLi_Init(void);
static int32 sfdefNiLi_Uninit(void);
static void sfdefNiLi_SetStatus(mClass_NiLiIfo *SetIfo);
static void sfdefNiLi_GetStatus(void);










static mClass_NiLiIfo   sdefIfo_NiLi;
static mGPIOPinIfo      sdefPin_NiLi = {	5,3,48,0x00 };

static mGPIOPinArr sdefPinArr_NiLi = {
	.apPin			=	&sdefPin_NiLi,
	.aNum 			=	1
};

mClass_NightLi gClassNiLi = {
	.apPinArr 		= 	&sdefPinArr_NiLi,
	.aIfo			=	&sdefIfo_NiLi,
	.afInit 		=	sfdefNiLi_Init,
	.afUninit		=	sfdefNiLi_Uninit,
	.afSetStatus 	=	sfdefNiLi_SetStatus,
	.afGetStatus 	=	sfdefNiLi_GetStatus,
	.OnStatus		=	0x01
};










static int32 sfdefNiLi_Init(void){
	
	mGPIOPinIfo 	*tPin 	= gClassNiLi.apPinArr->apPin;

	gClassStr.afMemset( (uint8*)&gClassNiLi.aIfo, 0x00, sizeof(mClass_NiLiIfo) );
	
	// Setting IO dirction OUTPUT 
    gClassHAL.Pin->prfSetDirection( tPin, DC_HAL_PIN_SetOUT );
	// DF_Set_GPIO_DIRx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );	
    
#if DEBUG_INIT
	NVCPrint("The Night Light module Start! OK");
#endif
	return 0;
}


static int32 sfdefNiLi_Uninit(void){
	
	return 0;
}

static void sfdefNiLi_SetStatus(mClass_NiLiIfo *SetIfo){
	
	mGPIOPinIfo 	*tPin 	= gClassNiLi.apPinArr->apPin;
	uint8 			tCmd	= SetIfo->aStatus;
	
	if( tCmd&DC_NiLi_On ){
		if( gClassNiLi.OnStatus ){
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetHIGH );
			// DF_Set_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );
		}else{
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetLOW );
			// DF_Clr_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );	
		}
	}else if( tCmd&DC_NiLi_Off ){
		if( gClassNiLi.OnStatus ){
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetLOW );
			// DF_Clr_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );	
		}else{
            gClassHAL.Pin->prfSetExport( tPin , DC_HAL_PIN_SetHIGH );
			// DF_Set_GPIO_Outx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit );
		}
	}
}

static void sfdefNiLi_GetStatus(void){
	mGPIOPinIfo 	*tPin 	= gClassNiLi.apPinArr->apPin;
	
	gClassNiLi.aIfo->aStatus &= ~DC_NiLi_StaMask;
	// if( DF_Get_GPIO_INx( tPin[ 0 ].aGroup, tPin[ 0 ].aBit ) ){
    if( gClassHAL.Pin->prfGetPinValue( tPin ) ){
		if( gClassNiLi.OnStatus ){
			gClassNiLi.aIfo->aStatus |= DC_NiLi_On;
		}else{
			gClassNiLi.aIfo->aStatus |= DC_NiLi_Off;
		}
	}else{
		if( gClassNiLi.OnStatus ){
			gClassNiLi.aIfo->aStatus |= DC_NiLi_Off;
		}else{
			gClassNiLi.aIfo->aStatus |= DC_NiLi_On;
		}
	}
	
}
