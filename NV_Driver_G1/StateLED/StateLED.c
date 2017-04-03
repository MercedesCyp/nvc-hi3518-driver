#include "StateLED.h"

#include "../ProInfo.h"
#include "../HAL/PeriodEvent.h"
#include "../HAL/USERHAL.h"
#include <linux/slab.h>




//  -------------------------------> note the control pin number for every product's state light
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// Pro D01
#define DC_D01_3518C_StateLEDNum			3
static mGPIOPinMsg saD01_3518C_SLEDPin[DC_D01_3518C_StateLEDNum]={
	{0,4,76,0x00},
	{0,3,75,0x00},
	{0,2,74,0x00},
};
// Pro D03

#define DC_D03_3518C_StateLEDNum			2
static mGPIOPinMsg saD03_3518C_SLEDPin[DC_D03_3518C_StateLEDNum]={
	{0,3,75,0x00},		// green
	{0,4,76,0x00},		// read
};
// Pro D04 Chip 3518c
	// its the same as 3518C D01

// Pro D11
	#define DC_D11_3518C_StateLEDNum			3
	static mGPIOPinMsg saD11_3518C_SLEDPin[DC_D11_3518C_StateLEDNum]={
		{0,4,76,0x00},
		{0,2,74,0x00},
		{0,3,75,0x00},
	};
// Pro F07
	#define DC_F07_3518E_StateLEDNum			1
	static mGPIOPinMsg saF07_3518E_SLEDPin[DC_F07_3518E_StateLEDNum]={
		{2,4,10,0x00},
	};
// Pro F05
	#define DC_F05_3518E_StateLEDNum			3
	static mGPIOPinMsg saF05_3518E_SLEDPin[DC_F05_3518E_StateLEDNum]={
		{0,4,76,0x00},
		{0,3,75,0x00},
		{0,2,74,0x00},
	};
// Pro F08
	#define DC_F08_3518E_StateLEDNum			1
	static mGPIOPinMsg saF08_3518E_SLEDPin[DC_F08_3518E_StateLEDNum]={
		{0,3,75,0x00},
	};

// -------------------------------> macro
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// Switch the LED period
#define DC_StatusLED_LoopPeriod		1 // 1  eq 1ms


//  the Every state light state buffer
typedef struct{
	uint32 aPTime; // positive time
	uint32 aNTime; // negative time
	uint32 aCount;
	uint8  aState;
#define DC_SLCfg_Update 0x80
#define DC_SLCfg_On 	0x01
#define DC_SLCfg_Off 	0x00
}mSL_CfgParam;




// -------------------------------> local Function definition
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
#define DC_OptFunCMD_Mask		0x03
#define DC_OptFunCMD_On			0x01
#define DC_OptFunCMD_Off 		0x02
#define DC_OptFunCMD_Switch		0x03
#define DC_OptFunCMD_GET		0x80

#define DC_OptFunRET_On			0x01
#define DC_OptFunRET_Off		0x00

static uint8 sfOptFun_StateLight_forD01_3518C(uint8 icmd, uint8 iGrp, uint8 iBit);
static uint8 sfOptFun_StateLight_forD03_3518C(uint8 icmd, uint8 iGrp, uint8 iBit);
static uint8 sfOptFun_StateLight_forD11_3518C(uint8 icmd, uint8 iGrp, uint8 iBit);

static void sfLED_LoopProcessing(void);




// -------------------------------> local variable
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
static mGPIOPinMsg *spStatusLEDPin; 
static uint8 sLEDSumNum = 0;
static mSL_CfgParam *saSLcfgParam;
static uint8 (*sfOptFun_StateLight_Handler)(uint8 , uint8 , uint8 ) = NULL;

// -------------------------------> function
// --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
int32 gfInitDrv_StateLED(uint8 iRemain)
{
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	uint8 _i;
	
	
	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D01:
		case DC_Pro_D04:{
			
			sLEDSumNum = DC_D01_3518C_StateLEDNum;
			spStatusLEDPin = saD01_3518C_SLEDPin;
			
			saSLcfgParam = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*sLEDSumNum,GFP_ATOMIC);
			
			sfOptFun_StateLight_Handler = sfOptFun_StateLight_forD01_3518C;
		}break;
		case DC_Pro_D11:{
			
			sLEDSumNum = DC_D11_3518C_StateLEDNum;
			spStatusLEDPin = saD11_3518C_SLEDPin;
			
			saSLcfgParam = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*sLEDSumNum,GFP_ATOMIC);
			
			sfOptFun_StateLight_Handler = sfOptFun_StateLight_forD11_3518C;
		}break;
		case DC_Pro_D03:{

			sLEDSumNum = DC_D03_3518C_StateLEDNum;
			spStatusLEDPin = saD03_3518C_SLEDPin;
			
			saSLcfgParam = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*sLEDSumNum,GFP_ATOMIC);
			sfOptFun_StateLight_Handler = sfOptFun_StateLight_forD03_3518C;
		}break;		
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:{
			sLEDSumNum = DC_F07_3518E_StateLEDNum;
			spStatusLEDPin = saF07_3518E_SLEDPin;
			saSLcfgParam = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*sLEDSumNum,GFP_ATOMIC);
			sfOptFun_StateLight_Handler = sfOptFun_StateLight_forD01_3518C;
		}break;
		case DC_Pro_F05:{
			sLEDSumNum = DC_F05_3518E_StateLEDNum;
			spStatusLEDPin = saF05_3518E_SLEDPin;
			saSLcfgParam = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*sLEDSumNum,GFP_ATOMIC);
			sfOptFun_StateLight_Handler = sfOptFun_StateLight_forD01_3518C;
		}break;
		case DC_Pro_F08:{
			sLEDSumNum = DC_F08_3518E_StateLEDNum;
			spStatusLEDPin = saF08_3518E_SLEDPin;
			saSLcfgParam = (mSL_CfgParam*)kmalloc(sizeof(mSL_CfgParam)*sLEDSumNum,GFP_ATOMIC);
			sfOptFun_StateLight_Handler = sfOptFun_StateLight_forD01_3518C;
		}break;
		
		default:return -1;
	};
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	
	for(_i=0;_i<sLEDSumNum;_i++){
		
		// Setting multi function
		DF_Set_GPIO_MUXx(spStatusLEDPin[_i].aMuxVal,spStatusLEDPin[_i].aMux);
		// Setting IO dirction OUTPUT 
		DF_Set_GPIO_DIRx(spStatusLEDPin[_i].aGroup,spStatusLEDPin[_i].aBit);
		
		
		saSLcfgParam[_i].aNTime =  0xFF;
		saSLcfgParam[_i].aPTime =  0x00;
		saSLcfgParam[_i].aState =  DC_SLCfg_Update;
		
		if(sfOptFun_StateLight_Handler(DC_OptFunCMD_GET, spStatusLEDPin[_i].aGroup,spStatusLEDPin[_i].aBit)){
			saSLcfgParam[_i].aState |= DC_SLCfg_On;
		}else{
			saSLcfgParam[_i].aState &= ~DC_SLCfg_On;
		}
	}
	
		// saSLcfgParam[0].aNTime =  0xFF;
		// saSLcfgParam[0].aPTime =  0xFF;
	gfPE_RegisterEvent(sfLED_LoopProcessing,DC_StatusLED_LoopPeriod,DC_PE_TYPE_INT);
	
	
#if OPEN_DEBUG
	NVCPrint("State Led  Started!\r\n");
#endif
	if( sfOptFun_StateLight_Handler == NULL )
		return -1;
	return 0;
}

int32 gfUninitDrv_StateLED(uint8 iRemain)
{
	uint8 _i;

	gfPE_UnregisterEvent(sfLED_LoopProcessing,DC_PE_TYPE_UNINT);
	kfree(saSLcfgParam);
	
	for(_i=0;_i<sLEDSumNum;_i++){
		sfOptFun_StateLight_Handler( DC_OptFunCMD_Off, spStatusLEDPin[_i].aGroup, spStatusLEDPin[_i].aBit );
	}
	sLEDSumNum = 0;
	return 0;
}

int32 gfStaLED_GetLEDNumber(void){
	return (int32)sLEDSumNum;
}

int32 gfStaLED_SetLEDStatus(uint32 iPTime,uint32 iNTime,uint8 iSL_ID){
	
	if(iSL_ID>sLEDSumNum)
		return -1;
	
	saSLcfgParam[iSL_ID].aPTime = iPTime;
	saSLcfgParam[iSL_ID].aNTime = iNTime;
	saSLcfgParam[iSL_ID].aState |= DC_SLCfg_Update;
	
	if(sfOptFun_StateLight_Handler( DC_OptFunCMD_GET, spStatusLEDPin[iSL_ID].aGroup,spStatusLEDPin[iSL_ID].aBit))
		saSLcfgParam[iSL_ID].aState |= DC_SLCfg_On;
	else
		saSLcfgParam[iSL_ID].aState &= ~DC_SLCfg_On;
	
	return 0;
}




static void sfLED_LoopProcessing(void){
	
	uint8 _i;
	
	for(_i=0;_i<sLEDSumNum;_i++){
		if(saSLcfgParam[_i].aState&DC_SLCfg_Update){
			if(saSLcfgParam[_i].aState&DC_SLCfg_On){
				if(saSLcfgParam[_i].aNTime != 0){
				
					sfOptFun_StateLight_Handler( DC_OptFunCMD_Off, spStatusLEDPin[_i].aGroup,spStatusLEDPin[_i].aBit);
					
					saSLcfgParam[_i].aCount = saSLcfgParam[_i].aNTime;
					
					saSLcfgParam[_i].aState &= ~DC_SLCfg_On;
					saSLcfgParam[_i].aState &= ~DC_SLCfg_Update;
				}else{
					saSLcfgParam[_i].aState |= DC_SLCfg_On;
				}
			}else{
				if(saSLcfgParam[_i].aPTime != 0){
					
					sfOptFun_StateLight_Handler( DC_OptFunRET_On, spStatusLEDPin[_i].aGroup,spStatusLEDPin[_i].aBit);
					saSLcfgParam[_i].aCount = saSLcfgParam[_i].aPTime;
					saSLcfgParam[_i].aState |= DC_SLCfg_On;
					saSLcfgParam[_i].aState &= ~DC_SLCfg_Update;
				}else{
					saSLcfgParam[_i].aState &= ~DC_SLCfg_On;
				}
			}
		}
		
		if(saSLcfgParam[_i].aCount > 1)
			saSLcfgParam[_i].aCount--;
		else if(saSLcfgParam[_i].aCount == 1)
			saSLcfgParam[_i].aState |= DC_SLCfg_Update;
		
	}
}






////////////////////////////////////////////////////////////////////////////////
// --------> Local Function( static function )



static uint8 sfOptFun_StateLight_forD01_3518C(uint8 icmd, uint8 iGrp, uint8 iBit){
	uint8 tRet = 0;
	
	switch( icmd&DC_OptFunCMD_Mask ){
		case DC_OptFunCMD_On:{
			DF_Clr_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_OptFunCMD_Off:{
			DF_Set_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_OptFunCMD_Switch:{
			DF_Switch_GPIO_Outx( iGrp , iBit );
		}break;
	}
	
	if( icmd&DC_OptFunCMD_GET){
		if(DF_Get_GPIO_INx( iGrp , iBit )){
			tRet |= DC_OptFunRET_Off;
		}else{
			tRet |= DC_OptFunRET_On;
		}
	}
	
	return tRet;
}


static uint8 sfOptFun_StateLight_forD03_3518C(uint8 icmd, uint8 iGrp, uint8 iBit){
	uint8 tRet = 0;
	
	switch( icmd&DC_OptFunCMD_Mask ){
		case DC_OptFunCMD_On:{
			DF_Set_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_OptFunCMD_Off:{
			DF_Clr_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_OptFunCMD_Switch:{
			DF_Switch_GPIO_Outx( iGrp , iBit );
		}break;
	}
	
	if( icmd&DC_OptFunCMD_GET){
		if(DF_Get_GPIO_INx( iGrp , iBit )){
			tRet |= DC_OptFunRET_On;
		}else{
			tRet |= DC_OptFunRET_Off;
		}
	}
	
	return tRet;
}


static uint8 sfOptFun_StateLight_forD11_3518C(uint8 icmd, uint8 iGrp, uint8 iBit){
	uint8 tRet = 0;
	
	switch( icmd&DC_OptFunCMD_Mask ){
		case DC_OptFunCMD_On:{
			DF_Set_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_OptFunCMD_Off:{
			DF_Clr_GPIO_Outx( iGrp , iBit );
		}break;
		case DC_OptFunCMD_Switch:{
			DF_Switch_GPIO_Outx( iGrp , iBit );
		}break;
	}
	
	if( icmd&DC_OptFunCMD_GET){
		if(DF_Get_GPIO_INx( iGrp , iBit )){
			tRet |= DC_OptFunRET_On;
		}else{
			tRet |= DC_OptFunRET_Off;
		}
	}
	
	return tRet;
}




