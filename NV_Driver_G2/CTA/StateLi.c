// ================================================================================
//| 默认:
//|		大部分产品都只用到一个状态灯
//|		但状态灯的控制IO有所不同
//|		
//|		
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
#include "../HAL/HAL.h"
#include "../Tool/String.h"
#include "../GlobalParameter.h"









// --------------------------------------------------------------------------------
#define DC_StaLi_LoopProcessingFrq	1 		// 1ms
//  the Every state light state buffer
typedef struct{
	mGPIOPinIfo *apPin;
	uint32 		aPTime; // positive time
	uint32 		aNTime; // negative time
	uint32 		aCount;
	uint8  		aState;
#define DC_SLCfg_On 	0x01
#define DC_SLCfg_Off 	0x00
}mSL_CfgParam;

static int32	sfdefStaLi_Init(void);
static int32	sfdefStaLi_Uninit(void);
static void 	sfdefStaLi_SetStatus(mStaLi_SetParam *iCmd);
static void		sfStaLi_LoopProcessing(void);
#define DC_StaLi_OptCMD_Mask		0x03
#define DC_StaLi_OptCMD_On			0x01
#define DC_StaLi_OptCMD_Off 		0x02
#define DC_StaLi_OptCMD_Switch		0x03
#define DC_StaLi_OptCMD_GET			0x80

#define DC_StaLi_OptRET_On			0x01
#define DC_StaLi_OptRET_Off			0x00
static uint32	sfdefStaLi_OptMode(uint32 , mGPIOPinIfo *iPin  );
static uint32 	sfdefStaLi_InOptMode(uint32 , mGPIOPinIfo *iPin );
// static void sfdefStaLi_SetIOOuput(mGPIOPinIfo *ipPin);









// --------------------------------------------------------------------------------
static mSL_CfgParam *spStaLi_Info = NULL;
static uint8		sStaLi_Num = 0;
static mGPIOPinIfo sdefPin_StaLi[3] = {
	{0,3,75,0x00},
	{0,2,74,0x00},
	{0,4,76,0x00},
};

static mGPIOPinArr sdefPinArr_StaLi = {
	.apPin			=	sdefPin_StaLi,
	.aNum 			=	3
};

mClass_StateLi gClassStateLi = {
	.apPinArr		=	&sdefPinArr_StaLi,
	.afInit			=	sfdefStaLi_Init,
	.afUninit		=	sfdefStaLi_Uninit,
	.afSetStatus	=	sfdefStaLi_SetStatus,
	.OnStatus		=	0x01,
};

static uint32 (*sfvStaLi_OptMode)(uint32, mGPIOPinIfo *);










// --------------------------------------------------------------------------------
// ---------------------------------------------------------------------->Local Function
// ------------------------------------------------------------>External
// -------------------------------------------------->sfdefStaLi_Init
static int32 sfdefStaLi_Init(void){
	uint8 _i;
	uint8 tNum;
	sStaLi_Num = 0;
	tNum = gClassStateLi.apPinArr->aNum;
	spStaLi_Info = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*tNum,GFP_ATOMIC);
	gClassStr.afMemset( (uint8*)spStaLi_Info, 0x00, sizeof(mSL_CfgParam)*tNum );
	
	for( _i = 0; _i<tNum; _i++ )
        gClassHAL.Pin->prfSetDirection( &(gClassStateLi.apPinArr->apPin[_i]), DC_HAL_PIN_SetOUT );
		// sfdefStaLi_SetIOOuput( &(gClassStateLi.apPinArr->apPin[_i]) );
	
	if( gClassStateLi.OnStatus ){
		sfvStaLi_OptMode = sfdefStaLi_OptMode;
	}else{
		sfvStaLi_OptMode = sfdefStaLi_InOptMode;
	}
	
	gClassHAL.PeriodEvent->afRegister( \
				DC_HAL_PE_INT,\
				sfStaLi_LoopProcessing,\
				DC_StaLi_LoopProcessingFrq );
				
	// {
		// mStaLi_SetParam tTest;
		// tTest.aUnit = 0;
		// tTest.aPTime = 0xFF;
		// tTest.aNTime = 0xFF;
		// sfdefStaLi_SetStatus(&tTest);
		
		// tTest.aUnit = 1;
		// sfdefStaLi_SetStatus(&tTest);
		// tTest.aUnit = 2;
		// sfdefStaLi_SetStatus(&tTest);
		
		// tTest.aPTime = 1000;
		// tTest.aNTime = 1000;
		// tTest.aUnit = 0;
		// sfdefStaLi_SetStatus(&tTest);
		
		// tTest.aPTime = 0;
		// tTest.aNTime = 0;
		// tTest.aUnit = 0;
		// sfdefStaLi_SetStatus(&tTest);
		
		// tTest.aPTime = 100;
		// tTest.aNTime = 100;
		// tTest.aUnit = 0;
		// sfdefStaLi_SetStatus(&tTest);
		
	// }

#if DEBUG_INIT
	NVCPrint("The State Light module Start! OK");
#endif
	
	return 0;
}

// -------------------------------------------------->sfdefStaLi_Uninit
static int32 sfdefStaLi_Uninit(void){
	gClassHAL.PeriodEvent->afCancel( \
				DC_HAL_PE_INT,\
				sfStaLi_LoopProcessing);
	kfree(spStaLi_Info);
	return 0;
}

// -------------------------------------------------->sfdefStaLi_SetStatus
static void  sfdefStaLi_SetStatus(mStaLi_SetParam *iCmd){
	uint8 tNum;
	tNum = gClassStateLi.apPinArr->aNum;
	
	if( iCmd->aUnit < tNum ){
		mGPIOPinIfo *tTools;
		uint8 _i = 0;
		tTools = &(gClassStateLi.apPinArr->apPin[iCmd->aUnit]);

		while( (spStaLi_Info[ _i ].apPin != tTools) && ( _i < sStaLi_Num ) )
			_i++;
		
		if( iCmd->aPTime || iCmd->aNTime ){
			// mount state light task
			
			if( _i != sStaLi_Num ){
				spStaLi_Info[ _i ].aPTime = iCmd->aNTime;
				spStaLi_Info[ _i ].aNTime = iCmd->aPTime;				
			}else{
				spStaLi_Info[sStaLi_Num].apPin	= tTools;
				spStaLi_Info[sStaLi_Num].aPTime = iCmd->aNTime;
				spStaLi_Info[sStaLi_Num].aNTime = iCmd->aPTime;
				spStaLi_Info[sStaLi_Num].aCount = 0;
				spStaLi_Info[sStaLi_Num].aState = DC_SLCfg_Off;
				sStaLi_Num++;
			}
			
		}else{
			// unmount state light task
			
			if( _i != sStaLi_Num ){
				gClassHAL.PeriodEvent->afControl(DC_HAL_PE_INT_PUSE);
			// 在这里防止中断出现，应当隔离禁用系统中断
				for( ;_i<(sStaLi_Num - 1);_i++ ){
					spStaLi_Info[ _i ].apPin	=	spStaLi_Info[ _i + 1 ].apPin ;
					spStaLi_Info[ _i ].aPTime	=	spStaLi_Info[ _i + 1 ].aPTime;
					spStaLi_Info[ _i ].aNTime	=	spStaLi_Info[ _i + 1 ].aNTime;
					spStaLi_Info[ _i ].aCount	=	spStaLi_Info[ _i + 1 ].aCount;
					spStaLi_Info[ _i ].aState	=	spStaLi_Info[ _i + 1 ].aState;					
				}				
				spStaLi_Info[ _i ].apPin	= NULL;
				spStaLi_Info[ _i ].aPTime	= 0;
				spStaLi_Info[ _i ].aNTime	= 0;
				spStaLi_Info[ _i ].aCount	= 0;
				spStaLi_Info[ _i ].aState	= 0;
				
				sStaLi_Num--;
				gClassHAL.PeriodEvent->afControl(DC_HAL_PE_INT_RESUME);
			}
			
			sfvStaLi_OptMode( DC_StaLi_OptCMD_Off, tTools);
		}
	}
}

// ------------------------------------------------------------>Internal
// -------------------------------------------------->sfStaLi_LoopProcessing
static void	 sfStaLi_LoopProcessing(void){
	uint8 _i;
	for( _i =0; _i < sStaLi_Num; _i++ ){
		if( spStaLi_Info[_i].aCount == 0 ){
			if( spStaLi_Info[_i].aState&DC_SLCfg_On ){
				spStaLi_Info[_i].aState &= ~DC_SLCfg_On;
				if( spStaLi_Info[_i].aNTime > 0 ){
					spStaLi_Info[_i].aCount =  spStaLi_Info[_i].aNTime;
					//set off
					sfvStaLi_OptMode( DC_StaLi_OptCMD_Off , spStaLi_Info[_i].apPin );
				}
			}else{
				spStaLi_Info[_i].aState |= DC_SLCfg_On;
				if( spStaLi_Info[_i].aPTime > 0 ){
					spStaLi_Info[_i].aCount =  spStaLi_Info[_i].aPTime;
					// set on
					sfvStaLi_OptMode( DC_StaLi_OptCMD_On , spStaLi_Info[_i].apPin );
					
				}
			}
		}
		if( spStaLi_Info[_i].aCount > 0 )
			spStaLi_Info[_i].aCount--;
	}
}

// -------------------------------------------------->sfdefStaLi_OptMode
static uint32 sfdefStaLi_OptMode(uint32 iCmd, mGPIOPinIfo *iPin ){
	uint32 tRet = 0;
	switch( iCmd&DC_StaLi_OptCMD_Mask ){
		case DC_StaLi_OptCMD_On:{
            gClassHAL.Pin->prfSetExport( iPin, DC_HAL_PIN_SetHIGH );
			// DF_Set_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_StaLi_OptCMD_Off:{
            gClassHAL.Pin->prfSetExport( iPin, DC_HAL_PIN_SetLOW );
			// DF_Clr_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_StaLi_OptCMD_Switch:{
            gClassHAL.Pin->prfSetExport( iPin, DC_HAL_PIN_SetSWITCH );
			// DF_Switch_GPIO_Outx( iGrp , iBit );
		}break;
	}
	
	if( iCmd&DC_StaLi_OptCMD_GET){
        if( gClassHAL.Pin->prfGetPinValue( iPin ) ){
		// if(DF_Get_GPIO_INx( iGrp , iBit )){
			tRet |= DC_StaLi_OptRET_On;
		}else{
			tRet |= DC_StaLi_OptRET_Off;
		}
	}
	return tRet;
}

// -------------------------------------------------->sfdefStaLi_InOptMode
static uint32 sfdefStaLi_InOptMode(uint32 iCmd, mGPIOPinIfo *iPin ){//inverse
	uint32 tRet = 0;
	switch( iCmd&DC_StaLi_OptCMD_Mask ){
		case DC_StaLi_OptCMD_On:{
            gClassHAL.Pin->prfSetExport( iPin, DC_HAL_PIN_SetLOW );
			// DF_Clr_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_StaLi_OptCMD_Off:{
            gClassHAL.Pin->prfSetExport( iPin, DC_HAL_PIN_SetHIGH );
			// DF_Set_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_StaLi_OptCMD_Switch:{
            gClassHAL.Pin->prfSetExport( iPin, DC_HAL_PIN_SetSWITCH );
			// DF_Switch_GPIO_Outx( iGrp , iBit );
		}break;
	}
	
	if( iCmd&DC_StaLi_OptCMD_GET){
        if( gClassHAL.Pin->prfGetPinValue( iPin ) ){
		// if(DF_Get_GPIO_INx( iGrp , iBit )){
			tRet |= DC_StaLi_OptRET_Off;
		}else{
			tRet |= DC_StaLi_OptRET_On;
		}
	}
	return tRet;
}

/* 
// -------------------------------------------------->sfdefStaLi_SetIOOuput
static void sfdefStaLi_SetIOOuput(mGPIOPinIfo *ipPin){
	// Setting multi function
	DF_Set_GPIO_MUXx(ipPin->aMuxVal,ipPin->aMux);
	// Setting IO dirction OUTPUT 
	DF_Set_GPIO_DIRx(ipPin->aGroup,ipPin->aBit);
}
 */
