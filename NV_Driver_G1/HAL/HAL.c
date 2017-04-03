#include "HAL_ADC.h"

#include "../GlobalParameter.h"
#include "../ProInfo.h"

#include "USERHAL.h"
#include "HAL_Timer.h"
#include "HAL_PWM.h"
#include "PeriodEvent.h"
#include "SHT20.h"


int32 gfInitDrv_HALLeyer(uint8 iRemain)
{
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	
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
			
			if(gfINIT_HAL_ADC()){
#if OPEN_DEBUG
	NVCPrint("ADC Interrupt request err!\r\n");
#endif
			return -1;
			}
			
			gfINIT_HAL_Timer2();
			
			gfInitDrv_PeroidEvent();
			
			// if(DC_Pro_D03 == tPro){
				// gfSHT20_INIT();
			// }
			
			if(DC_Pro_D11 == tPro){
				gfINIT_HAL_PWM();
			}
		}break;
		
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:
		case DC_Pro_F02:
		case DC_Pro_F08:
		case DC_Pro_F05:{
			if(gfINIT_HAL_ADC()){
#if OPEN_DEBUG
	NVCPrint("ADC Interrupt request err!\r\n");
#endif
			return -1;
			}
			
			gfINIT_HAL_Timer2();
			gfInitDrv_PeroidEvent();
			
		}break;
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	
	return 0;
}

int32 gfUninitDrv_HALLeyer(uint8 iRemain)
{
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
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
			
			gfUNINIT_HAL_Timer2();
			gfUnInitDrv_PeroidEvent();
			gfUNINIT_HAL_ADC();
			
			if(DC_Pro_D11 == tPro){
				gfUNINIT_HAL_PWM();
			}
			
			
			// if(DC_Pro_D03 == tPro){
				// gfSHT20_UNINIT();
			// }
			
		}break;		
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:
		case DC_Pro_F08:
		case DC_Pro_F05:{
			gfUNINIT_HAL_Timer2();
			gfUnInitDrv_PeroidEvent();
			gfUNINIT_HAL_ADC();
			
		}break;
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	

	return 0;
}

