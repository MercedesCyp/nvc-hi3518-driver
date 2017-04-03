#include "IfrLight.h"

#include "../HAL/USERHAL.h"
#include "../ProInfo.h"

#include <linux/spinlock.h> //spinlock
#include <linux/slab.h> //spinlock


// Pro D01
#define DC_D01_3518c_IrLight_G		0
#define DC_D01_3518c_TrLight_B		0
#define DC_D01_3518c_TrLight_MUX	72
#define DC_D01_3518c_TrLight_MUXVal	0x00
// Pro D03
#define DC_D03_3518c_IrLight_G		0
#define DC_D03_3518c_TrLight_B		0
#define DC_D03_3518c_TrLight_MUX	72
#define DC_D03_3518c_TrLight_MUXVal	0x00
// Pro D04 Chip 3518c
#define DC_D04_3518c_IrLight_G		0
#define DC_D04_3518c_TrLight_B		0
#define DC_D04_3518c_TrLight_MUX	72
#define DC_D04_3518c_TrLight_MUXVal	0x00
// Pro D11
#define DC_D11_3518c_IrLight_G		0
#define DC_D11_3518c_TrLight_B		0
#define DC_D11_3518c_TrLight_MUX	72
#define DC_D11_3518c_TrLight_MUXVal	0x00
// Pro F07
#define DC_F07_3518e_IrLight_G		2
#define DC_F07_3518e_TrLight_B		2
#define DC_F07_3518e_TrLight_MUX	8
#define DC_F07_3518e_TrLight_MUXVal	0x00
// Pro F05
#define DC_F05_3518e_IrLight_G		0
#define DC_F05_3518e_TrLight_B		0
#define DC_F05_3518e_TrLight_MUX	72
#define DC_F05_3518e_TrLight_MUXVal	0x00
// Pro F08
#define DC_F08_3518e_IrLight_G		0
#define DC_F08_3518e_TrLight_B		0
#define DC_F08_3518e_TrLight_MUX	72
#define DC_F08_3518e_TrLight_MUXVal	0x00



#define DF_IrLight_On			DF_Set_GPIO_Outx(sIrLightPin->aGroup,sIrLightPin->aBit)
#define DF_IrLight_Off			DF_Clr_GPIO_Outx(sIrLightPin->aGroup,sIrLightPin->aBit) 

#define DF_IrLight_GetStatus		DF_Get_GPIO_INx(sIrLightPin->aGroup,sIrLightPin->aBit)




mGPIOPinMsg *sIrLightPin;


int32 gfInitDrv_IfrLight(uint8 iRemain){
	
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	
	sIrLightPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg),GFP_ATOMIC);

///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D01:
		case DC_Pro_D04:
		case DC_Pro_D03:
		case DC_Pro_D11:{
			sIrLightPin->aGroup		=	DC_D01_3518c_IrLight_G;
			sIrLightPin->aBit		=	DC_D01_3518c_TrLight_B;
			sIrLightPin->aMux		=	DC_D01_3518c_TrLight_MUX;
			sIrLightPin->aMuxVal	=	DC_D01_3518c_TrLight_MUXVal;
		}break;	
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:{
			sIrLightPin->aGroup		=	DC_F07_3518e_IrLight_G;
			sIrLightPin->aBit		=	DC_F07_3518e_TrLight_B;
			sIrLightPin->aMux		=	DC_F07_3518e_TrLight_MUX;
			sIrLightPin->aMuxVal	=	DC_F07_3518e_TrLight_MUXVal;
		}break;
		case DC_Pro_F08:
		case DC_Pro_F05:{
			sIrLightPin->aGroup		=	DC_F05_3518e_IrLight_G;
			sIrLightPin->aBit		=	DC_F05_3518e_TrLight_B;
			sIrLightPin->aMux		=	DC_F05_3518e_TrLight_MUX;
			sIrLightPin->aMuxVal	=	DC_F05_3518e_TrLight_MUXVal;
		}break;
		case DC_Pro_F02:
		return 1;
		
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	
	

	// Setting multi function
	DF_Set_GPIO_MUXx(sIrLightPin->aMuxVal,sIrLightPin->aMux);
	// Setting IO dirction OUTPUT 
	DF_Set_GPIO_DIRx(sIrLightPin->aGroup,sIrLightPin->aBit);

	// DF_IrLight_On;
	DF_IrLight_Off;
	
	
#if OPEN_DEBUG
	NVCPrint("Infrared Light Started!\r\n");
#endif	
	return 0;
}

int32 gfUninitDrv_IfrLight(uint8 iRemain){
	
	kfree(sIrLightPin);
	
	return 0;
}



uint8 gfDrv_InfraredLightStatus(uint8 iCmd){
	
	uint8 tIrLightStatus = 0;
	if(iCmd&DC_IrLight_CMDSet){
		if(iCmd&DC_IrLight_StaOn)
			DF_IrLight_On;
		if(iCmd&DC_IrLight_StaOff)
			DF_IrLight_Off;
	}
	//else if(iCmd == DC_AuPl_CmdGet){
		
	//}
	if(iCmd&DC_IrLight_CMDGet){	
		if(DF_IrLight_GetStatus){
			tIrLightStatus |= DC_IrLight_StaOn;
		}else{
			tIrLightStatus |= DC_IrLight_StaOff;
		}
	}
	return tIrLightStatus;
}


