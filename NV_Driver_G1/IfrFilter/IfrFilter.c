#include "IfrFilter.h"

#include "../HAL/USERHAL.h"
#include "../ProInfo.h"
#include "../HAL/PeriodEvent.h"

#include <linux/spinlock.h> //spinlock
#include <linux/slab.h> //spinlock
//#include <asm/delay.h>
#include <linux/delay.h>


// Pro D01
#define DC_D01_3518c_IrCut_G		5
#define DC_D01_3518c_IrCut_B		3
#define DC_D01_3518c_IrCut_MUX		48
#define DC_D01_3518c_IrCut_MUXVal	0x00
// Pro D03
#define DC_D03_3518c_IrCut_G		0
#define DC_D03_3518c_IrCut_B		7
#define DC_D03_3518c_IrCut_MUX		79
#define DC_D03_3518c_IrCut_MUXVal	0x01
// Pro D04 Chip 3518c
#define DC_D04_3518c_IrCut_G		5
#define DC_D04_3518c_IrCut_B		3
#define DC_D04_3518c_IrCut_MUX		48
#define DC_D04_3518c_IrCut_MUXVal	0x00
// Pro D11
// none
// Pro F07
#define DC_F07_3518e_IrCut_G		1
#define DC_F07_3518e_IrCut_B		0
#define DC_F07_3518e_IrCut_MUX		0
#define DC_F07_3518e_IrCut_MUXVal	0x00


// Pro F05
// Pro F08
#define DC_F05_3518E_IrCut0_G		7
#define DC_F05_3518E_IrCut0_B		6
#define DC_F05_3518E_IrCut0_MUX		66
#define DC_F05_3518E_IrCut0_MUXVal	0x00

#define DC_F05_3518E_IrCut1_G		7
#define DC_F05_3518E_IrCut1_B		7
#define DC_F05_3518E_IrCut1_MUX		67
#define DC_F05_3518E_IrCut1_MUXVal	0x00
	


// as D01/D04/D03 	high is Night low is Day
// as F07 			high is Day low is Night
#define DF_IrFilt_SetDNPin			DF_Set_GPIO_Outx(sIrfiltPin[0].aGroup,sIrfiltPin[0].aBit)  
#define DF_IrFilt_ClrDNPin			DF_Clr_GPIO_Outx(sIrfiltPin[0].aGroup,sIrfiltPin[0].aBit)
#define DF_IrFilt_GetStatus			DF_Get_GPIO_INx(sIrfiltPin[0].aGroup,sIrfiltPin[0].aBit)

// #define DF_IrFilt_EnModule			DF_Clr_GPIO_Outx(sIrfiltPin[1].aGroup,sIrfiltPin[1].aBit)
// #define DF_IrFilt_DisModule			DF_Set_GPIO_Outx(sIrfiltPin[1].aGroup,sIrfiltPin[1].aBit)
#define DF_IrFilt_EnModule			DF_Clr_GPIO_Outx(sIrfiltPin[1].aGroup,sIrfiltPin[1].aBit)
#define DF_IrFilt_DisModule			DF_Clr_GPIO_Outx(sIrfiltPin[1].aGroup,sIrfiltPin[1].aBit)




////////////////////////////////////////////////////////////////////////////////
// --------> Local Function definition
#define DC_OptFunCMD_SwitchToDay	0x01
#define DC_OptFunCMD_SwitchToNight 	0x02
#define DC_OptFunCMD_GetStatus		0x04

#define DC_OptFunRET_DAY			0x01
#define DC_OptFunRET_NIGHT			0x02
#define DC_OptFunRET_TypeA			0x10
#define DC_OptFunRET_TypeB			0x20
#define DF_OptFunRET_GETType(_val)	(_val&0xF0)

static uint8 sfOptFun_IfrFilter_forD01_3518C(uint8 icmd);
//static uint8 sfOptFun_IfrFilter_forD03_3518C(uint8 icmd);
static uint8 sfOptFun_IfrFilter_forF07_3518E(uint8 icmd);
static uint8 sfOptFun_IfrFilter_forF05_3518E(uint8 icmd);


mGPIOPinMsg *sIrfiltPin = NULL ;
uint8 		 sIrfiltPinNum = 0;
static uint8 (*sfOptFun_IfrFilter_Handler)(uint8) = NULL;




int32 gfInitDrv_IfrFilter(uint8 iRemain){
	
	uint8 	_i;
	uint16 	tChip 	= DF_GLOBAL_GetChipInfo;
	uint16 	tPro 	= DF_GLOBAL_GetDivInfo;
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:{// 3518C
	switch(tPro){
		case DC_Pro_D01:
		case DC_Pro_D04:{
			
			sIrfiltPinNum = 1;
			sIrfiltPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg),GFP_ATOMIC);

			sIrfiltPin->aGroup	=	DC_D01_3518c_IrCut_G;
			sIrfiltPin->aBit	=	DC_D01_3518c_IrCut_B;
			sIrfiltPin->aMux	=	DC_D01_3518c_IrCut_MUX;
			sIrfiltPin->aMuxVal	=	DC_D01_3518c_IrCut_MUXVal;
			sfOptFun_IfrFilter_Handler = sfOptFun_IfrFilter_forD01_3518C;
		}break;
		case DC_Pro_D03:{
			
			sIrfiltPinNum = 1;
			sIrfiltPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg),GFP_ATOMIC);

			sIrfiltPin->aGroup	=	DC_D03_3518c_IrCut_G;
			sIrfiltPin->aBit	=	DC_D03_3518c_IrCut_B;
			sIrfiltPin->aMux	=	DC_D03_3518c_IrCut_MUX;
			sIrfiltPin->aMuxVal	=	DC_D03_3518c_IrCut_MUXVal;
			sfOptFun_IfrFilter_Handler = sfOptFun_IfrFilter_forD01_3518C;
		}break;
		case DC_Pro_D11:
		return 1;
		default:return -1;
	};
	}break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:{
			sIrfiltPinNum = 1;
			sIrfiltPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg),GFP_ATOMIC);
			
			sIrfiltPin->aGroup	=	DC_F07_3518e_IrCut_G;
			sIrfiltPin->aBit	=	DC_F07_3518e_IrCut_B;
			sIrfiltPin->aMux	=	DC_F07_3518e_IrCut_MUX;
			sIrfiltPin->aMuxVal	=	DC_F07_3518e_IrCut_MUXVal;
			sfOptFun_IfrFilter_Handler = sfOptFun_IfrFilter_forF07_3518E;
		}break;
		case DC_Pro_F08:
		case DC_Pro_F05:{
			sIrfiltPinNum = 2;
			sIrfiltPin = (mGPIOPinMsg*)kmalloc(sizeof(mGPIOPinMsg)*2,GFP_ATOMIC);

			sIrfiltPin[0].aGroup	=	DC_F05_3518E_IrCut0_G;
			sIrfiltPin[0].aBit		=	DC_F05_3518E_IrCut0_B;
			sIrfiltPin[0].aMux		=	DC_F05_3518E_IrCut0_MUX;
			sIrfiltPin[0].aMuxVal	=	DC_F05_3518E_IrCut0_MUXVal;
			
			sIrfiltPin[1].aGroup	=	DC_F05_3518E_IrCut1_G;
			sIrfiltPin[1].aBit		=	DC_F05_3518E_IrCut1_B;
			sIrfiltPin[1].aMux		=	DC_F05_3518E_IrCut1_MUX;
			sIrfiltPin[1].aMuxVal	=	DC_F05_3518E_IrCut1_MUXVal;
			
			sfOptFun_IfrFilter_Handler = sfOptFun_IfrFilter_forF05_3518E;
		}break;
		case DC_Pro_F02:
		return 1;
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

	for( _i = 0 ;_i<sIrfiltPinNum; _i++  ){
		// Setting multi function
		DF_Set_GPIO_MUXx(sIrfiltPin[_i].aMuxVal,sIrfiltPin[_i].aMux);
		// Setting IO dirction OUTPUT
		DF_Set_GPIO_DIRx(sIrfiltPin[_i].aGroup,sIrfiltPin[_i].aBit);
	}
	
	gfDrv_InfraredFilterStatus(DC_IrFiltCMD_SetDAY);
	// gfDrv_InfraredFilterStatus(DC_IrFiltCMD_SetNight);
	
	return 0;
}




int32 gfUninitDrv_IfrFilter(uint8 iRemain){
	
	kfree(sIrfiltPin);	
	return 0;
}



uint16 gfDrv_InfraredFilterStatus(uint8 icmd){
	
	uint16 tIrFiltStatus = 0;
	
	if( sfOptFun_IfrFilter_Handler == NULL ){
		return DC_IrFiltRET_Err;
	}
	
	if( icmd&DC_IrFiltCMD_SetDAY ){
		sfOptFun_IfrFilter_Handler(DC_OptFunCMD_SwitchToDay);
	}else if( icmd&DC_IrFiltCMD_SetNight ){
		sfOptFun_IfrFilter_Handler(DC_OptFunCMD_SwitchToNight);
	}
	
	if( icmd&DC_IrFiltCMD_Get ){
		uint8 tRet;
		tRet = sfOptFun_IfrFilter_Handler(DC_OptFunCMD_GetStatus);
		if( tRet&DC_OptFunRET_DAY ){
			tIrFiltStatus |= DC_IrFiltRET_DAY;
		}else if( tRet&DC_OptFunRET_NIGHT ){
			tIrFiltStatus |= DC_IrFiltRET_NIGHT;
		}
		// else{
			// tIrFiltStatus |= DC_IrFiltRET_Err;
		// }
		if( DF_OptFunRET_GETType(tRet)==DC_OptFunRET_TypeA ){
			tIrFiltStatus |= DC_IrFiltRET_TypeA;
		}else if( DF_OptFunRET_GETType(tRet)==DC_OptFunRET_TypeB ){
			tIrFiltStatus |= DC_IrFiltRET_TypeB;
		}
		// else{
			// tIrFiltStatus |= DC_IrFiltRET_Err;
		// }
	}
	
	return tIrFiltStatus;
}



////////////////////////////////////////////////////////////////////////////////
// --------> Local Function( static function )
static uint8 sfOptFun_IfrFilter_forD01_3518C(uint8 icmd){
	
	uint8 tRet = 0;
	
	
	if(icmd&DC_OptFunCMD_SwitchToDay){
		DF_IrFilt_ClrDNPin;
	}else if(icmd&DC_OptFunCMD_SwitchToNight){
		DF_IrFilt_SetDNPin;
	}
	
	if( icmd&DC_OptFunCMD_GetStatus ){
		tRet |= DC_OptFunRET_TypeA;
		if(DF_IrFilt_GetStatus){
			tRet |= DC_OptFunRET_NIGHT ;
		}else{
			tRet |= DC_OptFunRET_DAY;
		}
	}
	return  tRet;
}

// static uint8 sfOptFun_IfrFilter_forD03_3518C(uint8 icmd){
	
	// uint8 tRet = 0;
	// printk("D03 ------> 1 \r\n");
	// if(icmd&DC_OptFunCMD_SwitchToDay){
	// printk("D03 ------> Day \r\n");
		// DF_IrFilt_ClrDNPin;
	// }else if(icmd&DC_OptFunCMD_SwitchToNight){
	// printk("D03 ------> Night \r\n");
		// DF_IrFilt_SetDNPin;
	// }
	
	// if( icmd&DC_OptFunCMD_GetStatus ){
		// tRet |= DC_OptFunRET_TypeA;
		// if(DF_IrFilt_GetStatus){
			// tRet |= DC_OptFunRET_NIGHT;
		// }else{
			// tRet |= DC_OptFunRET_DAY;
		// }
	// }
	// return  tRet;
// }


static uint8 sfOptFun_IfrFilter_forF07_3518E(uint8 icmd){
		
	uint8 tRet = 0;
	if(icmd&DC_OptFunCMD_SwitchToDay){
		DF_IrFilt_SetDNPin;
	}else if(icmd&DC_OptFunCMD_SwitchToNight){
		DF_IrFilt_ClrDNPin;
	}
	
	if( icmd&DC_OptFunCMD_GetStatus ){
		tRet |= DC_OptFunRET_TypeA;
		if(DF_IrFilt_GetStatus){
			tRet |= DC_OptFunRET_DAY;
		}else{
			tRet |= DC_OptFunRET_NIGHT;
		}
	}
	return  tRet;
}

#define DC_OptFunPRM_Frq	300
static void sfOptFun_Dis8002_forF05_3518E(void);
static uint8 sfOptFun_IfrFilter_forF05_3518E(uint8 icmd){
	
	uint8 tRet = 0;
	if(icmd&DC_OptFunCMD_SwitchToDay){
		DF_IrFilt_EnModule;
		// mdelay(20);
		DF_IrFilt_SetDNPin;
		gfPE_RegisterEvent(\
				sfOptFun_Dis8002_forF05_3518E,\
				DC_OptFunPRM_Frq,\
				DC_PE_TYPE_INT);
		
	}else if(icmd&DC_OptFunCMD_SwitchToNight){
		DF_IrFilt_EnModule;	
		// mdelay(20);
		DF_IrFilt_ClrDNPin;
		gfPE_RegisterEvent(\
				sfOptFun_Dis8002_forF05_3518E,\
				DC_OptFunPRM_Frq,\
				DC_PE_TYPE_INT);	
	}
	
	if( icmd&DC_OptFunCMD_GetStatus ){
		tRet |= DC_OptFunRET_TypeB;
		if(DF_IrFilt_GetStatus){
			tRet |= DC_OptFunRET_DAY;
		}else{
			tRet |= DC_OptFunRET_NIGHT;
		}
	}
	return  tRet;
}


static void sfOptFun_Dis8002_forF05_3518E(void){
	DF_IrFilt_DisModule;
	gfPE_UnregisterEvent(sfOptFun_Dis8002_forF05_3518E,DC_PE_TYPE_INT);
}




