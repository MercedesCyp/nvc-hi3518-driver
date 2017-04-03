// ================================================================================
//| 默认:
//|		
//|		
//|		
//|		
//| 
//|	目前适用：
//| 	
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
static 	int32 	sfdefIfrFilt_Init(void);
static 	int32 	sfdefIfrFilt_Uninit(void);
static	void 	sfdefIfrFilt_SetStatus_A(uint32);
static	void 	sfdefIfrFilt_SetStatus_InA(uint32);
static	void 	sfdefIfrFilt_SetStatus_B(uint32);
// static	void 	sfdefIfrFilt_SetStatus_InB(uint32);
static	int32	sfdefIfrFilt_GetStatus_A(void);
static	int32	sfdefIfrFilt_GetStatus_InA(void);
static	int32	sfdefIfrFilt_GetStatus_B(void);
// static	int32	sfdefIfrFilt_GetStatus_InB(void);









// --------------------------------------------------------------------------------
static mGPIOPinIfo sdefPin_IfrFilt_TypeA[] = {
	{ 5, 3, 48, 0x00},
};

mGPIOPinIfo sdefPin_IfrFilt_TypeB[] = {
	{ 7, 6, 66 ,0x00},
	{ 7, 7, 67 ,0x00},
};

mGPIOPinArr sdefPinArr_IfrFilt = {
	.apPin 	= 	sdefPin_IfrFilt_TypeA,
	.aNum	=	1,
};

mClass_IfrFilter gClassIfrFilt = {
	.apPinArr		= &sdefPinArr_IfrFilt,
	.afInit         = sfdefIfrFilt_Init,
	.afUninit       = sfdefIfrFilt_Uninit,
	.afSetStatus    = sfdefIfrFilt_SetStatus_A,
	.afGetStatus    = sfdefIfrFilt_GetStatus_A,
	.aType          = DC_IfrFlt_TypeA,
	// .OnStatus		= 0x00,
};












// --------------------------------------------------------------------------------
static int32 	sfdefIfrFilt_Init(void){
	uint8 _i;
	uint8 tPinNum = gClassIfrFilt.apPinArr->aNum;
	mGPIOPinIfo *tTools = gClassIfrFilt.apPinArr->apPin;
	
	for( _i=0; _i <tPinNum; _i++ ){
        gClassHAL.Pin->prfSetDirection( &tTools[_i], DC_HAL_PIN_SetOUT );
        
		/*
		DC_PIN_SET_IO_OUT( \
			 tTools[_i].aGroup\
			,tTools[_i].aBit\
			,tTools[_i].aMuxVal\
			,tTools[_i].aMux);
			
		NVCPrint("IRCUT  Drv_OUT-> G:%d\tB:%d\tMuxVal:%d\tMux:%d\t",\
		( int)tTools[_i].aGroup,\
		( int)tTools[_i].aBit,\
		( int)tTools[_i].aMuxVal,\
		( int)tTools[_i].aMux);
		*/
	}
	
	switch( gClassIfrFilt.aType ){
		case DC_IfrFltOpt_TypeA:
			gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_A;
			gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_A;
		break;
		case DC_IfrFltOpt_InTypeA:
			gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_InA;
			gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_InA;
		break;
		case DC_IfrFltOpt_TypeB:
			gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_B;
			gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_B;
		break;
		case DC_IfrFltOpt_InTypeB:
			// gClassIfrFilt.afSetStatus = sfdefIfrFilt_SetStatus_InB;
			// gClassIfrFilt.afGetStatus = sfdefIfrFilt_GetStatus_InB;
		break;
		default:
		return -1;
	}
	
	
	
#if DEBUG_INIT
	NVCPrint("The Infrared Filter module Start! OK");
#endif
	return 0;
}

static int32 	sfdefIfrFilt_Uninit(void){
	
	
	return 0;
}

static void 	sfdefIfrFilt_SetStatus_A(uint32 iCmd){
	mGPIOPinIfo *tTools = gClassIfrFilt.apPinArr->apPin;
	if( iCmd&DC_IfrFlt_PassLi ){//night
        gClassHAL.Pin->prfSetExport( tTools, DC_HAL_PIN_SetLOW );
		// DF_Clr_GPIO_Outx( tTools->aGroup, tTools->aBit );	
	}else if( iCmd&DC_IfrFlt_BlockLi ){//day
        gClassHAL.Pin->prfSetExport( tTools, DC_HAL_PIN_SetHIGH );
		// DF_Set_GPIO_Outx( tTools->aGroup, tTools->aBit );
	}
}

static void 	sfdefIfrFilt_SetStatus_InA(uint32 iCmd){
	mGPIOPinIfo *tTools = gClassIfrFilt.apPinArr->apPin;
	if( iCmd&DC_IfrFlt_PassLi ){//night
        gClassHAL.Pin->prfSetExport( tTools, DC_HAL_PIN_SetHIGH );
		// DF_Set_GPIO_Outx( tTools->aGroup, tTools->aBit );
		
	}else if( iCmd&DC_IfrFlt_BlockLi ){//day
        gClassHAL.Pin->prfSetExport( tTools, DC_HAL_PIN_SetLOW );
		// DF_Clr_GPIO_Outx( tTools->aGroup, tTools->aBit );
		
	}
}

static	int32	sfdefIfrFilt_GetStatus_A(void){
	int32 tRet = 0;
	mGPIOPinIfo *tTools = gClassIfrFilt.apPinArr->apPin;
	if( gClassHAL.Pin->prfGetPinValue( tTools )){
    
	// if( DF_Get_GPIO_INx( tTools->aGroup, tTools->aBit ) ){
		tRet = DC_IfrFlt_BlockLi;
	}else{
		tRet = DC_IfrFlt_PassLi;
	}
	
	return tRet;
}
static	int32	sfdefIfrFilt_GetStatus_InA(void){
	int32 tRet = 0;
	mGPIOPinIfo *tTools = gClassIfrFilt.apPinArr->apPin;
	
	if( gClassHAL.Pin->prfGetPinValue( tTools )){
	// if( DF_Get_GPIO_INx( tTools->aGroup, tTools->aBit ) ){
		tRet = DC_IfrFlt_PassLi;
	}else{
		tRet = DC_IfrFlt_BlockLi;
	}
	return tRet;
}


// #define DC_OptFunPRM_Frq	500
// static void		sfdefIfrFilt_DisableModule_B(void);
static	void 	sfdefIfrFilt_SetStatus_B(uint32 iCmd){
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;
	
    // Enable 8002
    gClassHAL.Pin->prfSetExport( &tPinArr[1], DC_HAL_PIN_SetLOW );

	if( iCmd&DC_IfrFlt_PassLi ){//night
				// printk("---------->night \r\n");
		// DF_Clr_GPIO_Outx(tPinArr[1].aGroup,tPinArr[1].aBit)
		// mdelay(20);
        
        gClassHAL.Pin->prfSetExport( &tPinArr[0], DC_HAL_PIN_SetHIGH );
		// DF_Set_GPIO_Outx(tPinArr[0].aGroup,tPinArr[0].aBit);
		/*
		gClassHAL.PeriodEvent->afRegister(\
									DC_HAL_PE_INT,\
									sfdefIfrFilt_DisableModule_B,\
									DC_OptFunPRM_Frq);	
		*/		
	}else if( iCmd&DC_IfrFlt_BlockLi ){//day
				// printk("---------->Day \r\n");
		// DF_Clr_GPIO_Outx(tPinArr[1].aGroup,tPinArr[1].aBit)
		// mdelay(20);
        gClassHAL.Pin->prfSetExport( &tPinArr[0], DC_HAL_PIN_SetLOW );
		// DF_Clr_GPIO_Outx(tPinArr[0].aGroup,tPinArr[0].aBit);
		/*
		gClassHAL.PeriodEvent->afRegister(\
									DC_HAL_PE_INT,\
									sfdefIfrFilt_DisableModule_B,\
									DC_OptFunPRM_Frq);	
		*/
	}
	
}
/*
static void		sfdefIfrFilt_DisableModule_B(void){
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;
				printk("---------->Cancel \r\n");

	DF_Set_GPIO_Outx(tPinArr[1].aGroup,tPinArr[1].aBit);
		gClassHAL.PeriodEvent->afCancel(\
									DC_HAL_PE_INT,\
									sfdefIfrFilt_DisableModule_B);	
}
*/
static	int32	sfdefIfrFilt_GetStatus_B(void){
	mGPIOPinIfo *tPinArr = gClassIfrFilt.apPinArr->apPin;
	uint32 		tRet = 0;
	if( gClassHAL.Pin->prfGetPinValue( &tPinArr[0] ) ){
	// if(DF_Get_GPIO_INx(tPinArr[0].aGroup,tPinArr[0].aBit)){
		tRet |= DC_IfrFlt_PassLi;
	}else{
		tRet |= DC_IfrFlt_BlockLi;
	}
	
	return 0;
}
