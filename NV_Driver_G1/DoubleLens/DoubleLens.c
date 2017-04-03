#include "DoubleLens.h"


//  -------------------------------> Header file
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// internal
#include <linux/slab.h>
// external
#include "../HAL/USERHAL.h"
#include "../ProInfo.h"

//  -------------------------------> note the control pin number for every product's state light
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// Pro D01 Chip 3518c
// Pro D03 Chip 3518c
// Pro D04 Chip 3518c
// Pro D11 Chip 3518c
#define DC_D11_3518c_DLens_G		3			
#define DC_D11_3518c_DLens_B		1			
#define DC_D11_3518c_DLens_MUX		13			
#define DC_D11_3518c_DLens_MUXVal	0x00		

#define DC_D11_3518c_NLens_G		3
#define DC_D11_3518c_NLens_B		0
#define DC_D11_3518c_NLens_MUX		12
#define DC_D11_3518c_NLens_MUXVal	0x00


#define DF_DubLens_SwitchToDLens	{\
DF_Clr_GPIO_Outx(tDoubleLensPin[1].aGroup,tDoubleLensPin[1].aBit);\
DF_Set_GPIO_Outx(tDoubleLensPin[0].aGroup,tDoubleLensPin[0].aBit);}

#define DF_DubLens_SwitchToNLens	{ \
DF_Clr_GPIO_Outx(tDoubleLensPin[0].aGroup,tDoubleLensPin[0].aBit);\
DF_Set_GPIO_Outx(tDoubleLensPin[1].aGroup,tDoubleLensPin[1].aBit);}

#define DF_DubLens_GetDLensStat		(DF_Get_GPIO_INx(tDoubleLensPin[0].aGroup,tDoubleLensPin[0].aBit))
#define DF_DubLens_GetNLensStat		(DF_Get_GPIO_INx(tDoubleLensPin[1].aGroup,tDoubleLensPin[1].aBit))



static mGPIOPinMsg *tDoubleLensPin;

int32 gfInitDrv_DoubleLens(uint8 iRemain){
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	
	tDoubleLensPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg)*2, GFP_ATOMIC);
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:{ // 3518C
	switch(tPro){
		case DC_Pro_D11:{
			tDoubleLensPin[0].aGroup	= DC_D11_3518c_DLens_G;
			tDoubleLensPin[0].aBit      = DC_D11_3518c_DLens_B;
			tDoubleLensPin[0].aMux      = DC_D11_3518c_DLens_MUX;
			tDoubleLensPin[0].aMuxVal   = DC_D11_3518c_DLens_MUXVal;
			
			tDoubleLensPin[1].aGroup	= DC_D11_3518c_NLens_G;
			tDoubleLensPin[1].aBit      = DC_D11_3518c_NLens_B;
			tDoubleLensPin[1].aMux      = DC_D11_3518c_NLens_MUX;
			tDoubleLensPin[1].aMuxVal   = DC_D11_3518c_NLens_MUXVal;
			
			// Setting multi function
			DF_Set_GPIO_MUXx(tDoubleLensPin[0].aMuxVal,tDoubleLensPin[0].aMux);
			// Setting IO dirction INPUT 
			DF_Set_GPIO_DIRx(tDoubleLensPin[0].aGroup,tDoubleLensPin[0].aBit);

			// Setting multi function
			DF_Set_GPIO_MUXx(tDoubleLensPin[1].aMuxVal,tDoubleLensPin[1].aMux);
			// Setting IO dirction INPUT 
			DF_Set_GPIO_DIRx(tDoubleLensPin[1].aGroup,tDoubleLensPin[1].aBit);
			
			DF_DubLens_SwitchToDLens;

		}break;
		case DC_Pro_D01:
		case DC_Pro_D03:
		case DC_Pro_D04:
		return 1;
		default:return -1;
		}
	}break;
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:
		case DC_Pro_F02:
		case DC_Pro_F05:
		case DC_Pro_F08:
		return 1;
		default:return -1;
	};
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	default:break;
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
#if OPEN_DEBUG
	NVCPrint("Double Lens Started!\r\n");
#endif	
	return 0;
}



int32 gfUninitDrv_DoubleLens(uint8 iRemain){	
	kfree(tDoubleLensPin);
	return 0;
}



uint8 gfDoubLens_ASG_TheLensState(uint8 iCmd){
	
	uint8 tReturn = 0;
	
	if(iCmd&DC_DubLens_CmdSet){
		if(iCmd&DC_DubLens_DayLens){
			DF_DubLens_SwitchToDLens;
		}else if(iCmd&DC_DubLens_NightLens){
			DF_DubLens_SwitchToNLens;
		}
	}
	
	if(iCmd&DC_DubLens_CmdGet){
		tReturn = DC_DubLens_CmdErr;
		if((DF_DubLens_GetDLensStat)&&(!DF_DubLens_GetNLensStat)){
			tReturn = DC_DubLens_DayLens;
		}else if((!DF_DubLens_GetDLensStat)&&(DF_DubLens_GetNLensStat)){
			tReturn = DC_DubLens_NightLens;		
		}
	}
	
	return tReturn;
}



