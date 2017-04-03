#include "NightLight.h"

#include "../HAL/USERHAL.h"
#include "../HAL/HAL_PWM.h"
#include "../ProInfo.h"
#include "../CoolFan/CoolFan.h"

#include <linux/slab.h>

//  -------------------------------> note the control pin number for every product's Neight Light
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// Pro D01
// Pro D03
#define DC_D03_3518c_NiLi_G			5
#define DC_D03_3518c_NiLi_B			3
#define DC_D03_3518c_NiLi_MUX		48
#define DC_D03_3518c_NiLi_MUXVal	0x00
// Pro D04
// Pro D11
#define DC_D11_3518c_NiLi_G			5
#define DC_D11_3518c_NiLi_B			1
#define DC_D11_3518c_NiLi_MUX		46
#define DC_D11_3518c_NiLi_MUXVal	0x00

#define DC_D11_3518c_NiLi_PWM_G			5
#define DC_D11_3518c_NiLi_PWM_B			3
#define DC_D11_3518c_NiLi_PWM_MUX		48
#define DC_D11_3518c_NiLi_PWM_MUXVal	0x01


#define DC_NiLi_Color_Rating		100
#define DC_NiLi_Color_Increase		3

#define DF_NiLi_On			DF_Set_GPIO_Outx(tNLGPIOPin[0].aGroup,tNLGPIOPin[0].aBit)
#define DF_NiLi_Off			DF_Clr_GPIO_Outx(tNLGPIOPin[0].aGroup,tNLGPIOPin[0].aBit) 
#define DF_NiLi_GetState	(DF_Get_GPIO_INx(tNLGPIOPin[0].aGroup,tNLGPIOPin[0].aBit))



static mGPIOPinMsg *tNLGPIOPin;
static uint16 sColorBrightness;


int32 gfInitDrv_NightLight(uint8 iRemain){
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){		
		case DC_Pro_D03:{
			tNLGPIOPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg) ,GFP_ATOMIC);
			tNLGPIOPin[0].aGroup	=	DC_D03_3518c_NiLi_G;
			tNLGPIOPin[0].aBit		=	DC_D03_3518c_NiLi_B;
			tNLGPIOPin[0].aMux		=	DC_D03_3518c_NiLi_MUX;
			tNLGPIOPin[0].aMuxVal	=	DC_D03_3518c_NiLi_MUXVal;
			
			
			// Setting multi function
			DF_Set_GPIO_MUXx(tNLGPIOPin[0].aMuxVal,tNLGPIOPin[0].aMux);
			// Setting IO dirction OUTPUT 
			DF_Set_GPIO_DIRx(tNLGPIOPin[0].aGroup,tNLGPIOPin[0].aBit);
			
			if(DF_GLOBAL_GetDivCmd==0){
				DF_NiLi_Off;
			}else if(DF_GLOBAL_GetDivCmd==1){
				DF_NiLi_On;
			}
			
		}break;
		case DC_Pro_D11:{
			tNLGPIOPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg)*2 ,GFP_ATOMIC);
			
			tNLGPIOPin[0].aGroup	=	DC_D11_3518c_NiLi_G;
			tNLGPIOPin[0].aBit		=	DC_D11_3518c_NiLi_B;
			tNLGPIOPin[0].aMux		=	DC_D11_3518c_NiLi_MUX;
			tNLGPIOPin[0].aMuxVal	=	DC_D11_3518c_NiLi_MUXVal;
			
			tNLGPIOPin[1].aGroup	=	DC_D11_3518c_NiLi_PWM_G;
			tNLGPIOPin[1].aBit		=	DC_D11_3518c_NiLi_PWM_B;
			tNLGPIOPin[1].aMux		=	DC_D11_3518c_NiLi_PWM_MUX;
			tNLGPIOPin[1].aMuxVal	=	DC_D11_3518c_NiLi_PWM_MUXVal;
			
			// Setting multi function
			DF_Set_GPIO_MUXx(tNLGPIOPin[0].aMuxVal,tNLGPIOPin[0].aMux);
			// Setting IO dirction OUTPUT 
			DF_Set_GPIO_DIRx(tNLGPIOPin[0].aGroup,tNLGPIOPin[0].aBit);
			
			// Setting multi function
			DF_Set_GPIO_MUXx(tNLGPIOPin[1].aMuxVal,tNLGPIOPin[1].aMux);
			// Setting IO dirction OUTPUT 
			DF_Set_GPIO_DIRx(tNLGPIOPin[1].aGroup,tNLGPIOPin[1].aBit);
			
			gfPWM_ModeSetWidth(1,100*DC_NiLi_Color_Increase);
			if(DF_GLOBAL_GetDivCmd==0){
				DF_NiLi_Off;
			}else if(DF_GLOBAL_GetDivCmd==1){
				DF_NiLi_On;
			}
		}break;
		case DC_Pro_D01:
		case DC_Pro_D04:
		return 1;
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F02:
		case DC_Pro_F05:
		case DC_Pro_F07:
		case DC_Pro_F08:
		return 1;
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	
#if OPEN_DEBUG
	NVCPrint("Brilliant Lamp ^-^ Started!\r\n");
#endif
	return 0;
}

int32 gfUninitDrv_NightLight(uint8 iRemain){
	kfree(tNLGPIOPin);
	return 0;
}



int32 gfNightLight_AdjustLight(uint8 iCmd,uint8 iDegree)//0-100
{
	int32 tRet = -1 ;
	
	if(iCmd&DC_NiLi_CmdGet){
		if(DF_NiLi_GetState){
			tRet = DC_NiLi_On;
		}else{
			tRet = DC_NiLi_Off;
		}
		
		tRet |= sColorBrightness<<16;
	}

	if(iCmd&DC_NiLi_CmdSet){
		tRet = 0;
		
		if(iCmd&DC_NiLi_AdjustBrightness){
			if(iDegree > 100)
				return -1;
			sColorBrightness = iDegree;
			gfPWM_ModeSetWidth(1,sColorBrightness*DC_NiLi_Color_Increase);
		}
		
		if(iCmd&DC_NiLi_On){
			DF_NiLi_On;
		}else{ //if(iCmd&DC_NiLi_Off){
			DF_NiLi_Off;
		}
	}
	return tRet;
}



