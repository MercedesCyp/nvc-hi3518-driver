// ================================================================================
//| ע��:
//|		�����ڴ˹���ģ��ֻ���� D11 3518C �Ļ��ͣ�Ĭ�ϵĿ�������������
//|		0	����������þ�ͷ������	1	�����������ҹ�þ�ͷ������
//|		������һ��ͷ���Ʒ�ʽһ�����������в��Ļ���ֻҪ�滻
//|		gClassIfrFilt.apPinArr->apPin ָ����������鼴��
//|		
//|		
//|		
//| 
//|	Ŀǰ���ã�
//| 	D11
//| 
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "CTA.h"
#include "../HAL/HAL.h"
#include "../Tool/String.h"
#include "../GlobalParameter.h"










// --------------------------------------------------------------------------------
static	int32	sfdefDoubLens_Init(void);
static	int32	sfdefDoubLens_Uninit(void);
static	void	sfdefDoubLens_SetStatus(uint32 iCmd);
static	int32	sfdefDoubLens_GetStatus(void);








// --------------------------------------------------------------------------------
static mGPIOPinIfo sdefPin_DoubleLens[] = {
	{ 3, 1, 13, 0x00},
	{ 3, 0, 12, 0x00},
};

mGPIOPinArr sdefPinArr_DoubLens = {
	.apPin 	= 	sdefPin_DoubleLens,
	.aNum	=	2,
};

mClass_DubLens gClassDouobLens = {
	.apPinArr		= &sdefPinArr_DoubLens,
	.afInit         = sfdefDoubLens_Init,
	.afUninit       = sfdefDoubLens_Uninit,
	.afSetStatus    = sfdefDoubLens_SetStatus,
	.afGetStatus    = sfdefDoubLens_GetStatus,
};








// --------------------------------------------------------------------------------
static	int32	sfdefDoubLens_Init(void){
	mGPIOPinIfo		*tpDoubLensPin;
	tpDoubLensPin =	gClassDouobLens.apPinArr->apPin;
	
    gClassHAL.Pin->prfSetDirection( &tpDoubLensPin[0], DC_HAL_PIN_SetOUT );
    gClassHAL.Pin->prfSetDirection( &tpDoubLensPin[1], DC_HAL_PIN_SetOUT );
    /* 
	DC_PIN_SET_IO_OUT(\
			tpDoubLensPin[0].aGroup,\
			tpDoubLensPin[0].aBit,\
			tpDoubLensPin[0].aMuxVal,\
			tpDoubLensPin[0].aMux);
	DC_PIN_SET_IO_OUT(\
			tpDoubLensPin[1].aGroup,\
			tpDoubLensPin[1].aBit,\
			tpDoubLensPin[1].aMuxVal,\
			tpDoubLensPin[1].aMux);
	 */
	return 0;
}

static	int32	sfdefDoubLens_Uninit(void){
	return 0;
}

static	void	sfdefDoubLens_SetStatus(uint32 iCmd){
	mGPIOPinIfo		*tpDoubLensPin;
	tpDoubLensPin =	gClassDouobLens.apPinArr->apPin;
	
	if( iCmd&DC_DuLens_DayLens ){
        gClassHAL.Pin->prfSetExport( &tpDoubLensPin[1], DC_HAL_PIN_SetLOW );
        gClassHAL.Pin->prfSetExport( &tpDoubLensPin[0], DC_HAL_PIN_SetHIGH );
		// DF_Clr_GPIO_Outx( tpDoubLensPin[1].aGroup, tpDoubLensPin[1].aBit );
		// DF_Set_GPIO_Outx( tpDoubLensPin[0].aGroup, tpDoubLensPin[0].aBit );
		
	}else if( iCmd&DC_DuLens_NightLens ){
        gClassHAL.Pin->prfSetExport( &tpDoubLensPin[0], DC_HAL_PIN_SetLOW );
        gClassHAL.Pin->prfSetExport( &tpDoubLensPin[1], DC_HAL_PIN_SetHIGH );
		// DF_Clr_GPIO_Outx( tpDoubLensPin[0].aGroup, tpDoubLensPin[0].aBit );
		// DF_Set_GPIO_Outx( tpDoubLensPin[1].aGroup, tpDoubLensPin[1].aBit );
	}
}

static	int32	sfdefDoubLens_GetStatus(void){
	int32 tRet		= 0 ;
	mGPIOPinIfo		*tpDoubLensPin	=	gClassDouobLens.apPinArr->apPin;
    
	uint8 			tDPin			= gClassHAL.Pin->prfGetPinValue( &tpDoubLensPin[0] );
	uint8 			tNPin			= gClassHAL.Pin->prfGetPinValue( &tpDoubLensPin[1] );
	
	// uint8 			tDPin			= DF_Get_GPIO_INx( tpDoubLensPin[0].aGroup, tpDoubLensPin[0].aBit );
	// uint8 			tNPin			= DF_Get_GPIO_INx( tpDoubLensPin[1].aGroup, tpDoubLensPin[1].aBit );
	
	if( (tDPin == 1 )&&(tNPin == 0)){
		tRet = DC_DuLens_DayLens;
	}else if( (tDPin == 0 )&&(tNPin == 1)){
		tRet = DC_DuLens_NightLens;
	}
	
	return tRet;
}
