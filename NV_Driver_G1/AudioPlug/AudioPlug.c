
#include "AudioPlug.h"
#include "../HAL/USERHAL.h"
#include "../ProInfo.h"

#include <linux/spinlock.h> //spinlock
#include <linux/slab.h> //spinlock


// Pro D01 Chip 3518c
#define DC_D01_3518c_AP_G		0
#define DC_D01_3518c_AP_B		6
#define DC_D01_3518c_AP_MUX		78
#define DC_D01_3518c_AP_MUXVal	0x00
// Pro D03 Chip 3518c
#define DC_D03_3518c_AP_G		0
#define DC_D03_3518c_AP_B		6
#define DC_D03_3518c_AP_MUX		78
#define DC_D03_3518c_AP_MUXVal	0x00
// Pro D04 Chip 3518c
#define DC_D04_3518c_AP_G		0
#define DC_D04_3518c_AP_B		6
#define DC_D04_3518c_AP_MUX		78
#define DC_D04_3518c_AP_MUXVal	0x00
// Pro D11 Chip 3518c
#define DC_D11_3518c_AP_G		0
#define DC_D11_3518c_AP_B		6
#define DC_D11_3518c_AP_MUX		78
#define DC_D11_3518c_AP_MUXVal	0x00
// Pro F07 Chip 3518e
#define DC_F07_3518e_AP_G		0
#define DC_F07_3518e_AP_B		6
#define DC_F07_3518e_AP_MUX		78
#define DC_F07_3518e_AP_MUXVal	0x00

// Pro F05 Chip 3518e
#define DC_F05_3518e_AP_G		0
#define DC_F05_3518e_AP_B		6
#define DC_F05_3518e_AP_MUX		78
#define DC_F05_3518e_AP_MUXVal	0x00
// Pro F08 Chip 3518e
#define DC_F08_3518e_AP_G		0
#define DC_F08_3518e_AP_B		6
#define DC_F08_3518e_AP_MUX		78
#define DC_F08_3518e_AP_MUXVal	0x00




#define DF_AuPl_On			DF_Clr_GPIO_Outx(sAuPl_Pin->aGroup,sAuPl_Pin->aBit)
#define DF_AuPl_Off			DF_Set_GPIO_Outx(sAuPl_Pin->aGroup,sAuPl_Pin->aBit) 

#define DF_AuPl_GetStatus(_g,_b)	DF_Get_GPIO_INx(_g,_b)

mGPIOPinMsg *sAuPl_Pin;


int32 gfInitDrv_AudioPlug(uint8 iRemain){
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	
	sAuPl_Pin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg),GFP_ATOMIC);
	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D01:
		case DC_Pro_D03:
		case DC_Pro_D04:
		case DC_Pro_D11:{
			sAuPl_Pin->aGroup=DC_D01_3518c_AP_G;
			sAuPl_Pin->aBit=DC_D01_3518c_AP_B;
			sAuPl_Pin->aMux=DC_D01_3518c_AP_MUX;
			sAuPl_Pin->aMuxVal=DC_D01_3518c_AP_MUXVal;
			
			// Setting multi function
			DF_Set_GPIO_MUXx(sAuPl_Pin->aMuxVal,sAuPl_Pin->aMux);
			// Setting IO dirction OUTPUT 
			DF_Set_GPIO_DIRx(sAuPl_Pin->aGroup,sAuPl_Pin->aBit);	
	}break;
		default:return -1;
	};break;
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F05:
		case DC_Pro_F08:
		case DC_Pro_F07:{
			sAuPl_Pin->aGroup=DC_F07_3518e_AP_G;
			sAuPl_Pin->aBit=DC_F07_3518e_AP_B;
			sAuPl_Pin->aMux=DC_F07_3518e_AP_MUX;
			sAuPl_Pin->aMuxVal=DC_F07_3518e_AP_MUXVal;
			
		}break;
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	
	// Setting multi function
	DF_Set_GPIO_MUXx(sAuPl_Pin->aMuxVal,sAuPl_Pin->aMux);
	// Setting IO dirction OUTPUT 
	DF_Set_GPIO_DIRx(sAuPl_Pin->aGroup,sAuPl_Pin->aBit);
	
	DF_AuPl_Off;
	// DF_AuPl_On;
	
#if OPEN_DEBUG
	NVCPrint(" Audio Plug Started!\r\n");
#endif
	return 0;
}

int32 gfUninitDrv_AudioPlug(uint8 iRemain){
	kfree(sAuPl_Pin);
	return 0;
}



uint8 gfDrv_AudioPlugStatus(uint8 icmd){
	
	uint8 tAudioPlugStatus = 0;
	
	if(icmd&DC_AuPl_CmdSet){
		if(icmd&DC_AuPl_On){
			DF_AuPl_On;
		}else{
			// if(icmd&DC_AuPl_Off){
			DF_AuPl_Off;
		}
		
		
	}else if(icmd == DC_AuPl_CmdGet){	
		if(DF_AuPl_GetStatus(sAuPl_Pin->aGroup,sAuPl_Pin->aBit)){
			tAudioPlugStatus = DC_AuPl_Off;
		}else{
			tAudioPlugStatus = DC_AuPl_On;
		}	
	}
	
	return tAudioPlugStatus;
}


