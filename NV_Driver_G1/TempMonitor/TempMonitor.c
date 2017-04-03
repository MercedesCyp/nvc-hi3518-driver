#include "TempMonitor.h"
#include "NTC.h"

#include "../HAL/HAL_ADC.h"
#include "../HAL/PeriodEvent.h"
#include "../HAL/SHT20.h"
#include "../CoolFan/CoolFan.h"
#include "../ProInfo.h"



#define DC_TEMPMONITOR_UPDATE_PERIOD	1000 //	1s
static void sfTempMonitor_UpdateTemp_For_D11(void);




struct{
	int32 	aTemp;
}soTemp_RunStatus;



int32 gfInitDrv_TempMonitor(uint8 iRemain){
	
	uint16 tChip = DF_GLOBAL_GetChipInfo;	
	uint16 tPro = DF_GLOBAL_GetDivInfo;

	switch(tChip){
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D11:{
			uint16 	tTADCValue;
			tTADCValue = gfADC_GetAndStartConvert(DC_ADC_UserID_For_TS);
			tTADCValue = gfADC_GetAndStartConvert(DC_ADC_UserID_For_TS);
			soTemp_RunStatus.aTemp = ConvertTempFromVoltage(tTADCValue);
			gfCOOLFAN_DriveTheFan(soTemp_RunStatus.aTemp);	
			
			gfPE_RegisterEvent(sfTempMonitor_UpdateTemp_For_D11,DC_TEMPMONITOR_UPDATE_PERIOD,DC_PE_TYPE_INT);
			
		}break;
		case DC_Pro_D03:{
			
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
	NVCPrint("Temperature Monitor ^-^ Started!\r\n");
#endif
	return 0;
}



int32 gfUninitDrv_TempMonitor(uint8 iRemain){
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	switch(tChip){
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518C:// 3518C
	switch(tPro){
		case DC_Pro_D11:{
			gfPE_UnregisterEvent(sfTempMonitor_UpdateTemp_For_D11,DC_PE_TYPE_INT);
		}break;
		default:return -1;
	};break;	
///////// ///////// ///////// ///////// ///////// ///////// ///////// ///////// 
	case DC_CHIP_3518E:// 3518E
	switch(tPro){
		default:return -1;
	};	
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	}
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
	return 0;
}


static void sfTempMonitor_MotorCheck(void);
void (*pgfTempMonitor_PeriodEvent)(int32) = NULL ;
void gfTempMonitor_SetReport(uint32 iCmd,uint32 iTime){
	
	if( iCmd&DC_TEMPMONITOR_REPORT_Start ){	
		gfPE_RegisterEvent(sfTempMonitor_MotorCheck,iTime*10,DC_PE_TYPE_UNINT);		
		
	}else if( iCmd&DC_TEMPMONITOR_REPORT_Stop ){
		gfPE_UnregisterEvent(sfTempMonitor_MotorCheck,DC_PE_TYPE_UNINT);
		
	}
}

int32 gfTempMonitor_GetNowValue(void){
	int32 tTempValue = 0;
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_D03:{
			tTempValue = gfSHT20Opt_GetMeasure(DC_SHT20OptCMD_Get_Temp);
		}break;
		case DC_Pro_D11:{			
			tTempValue = soTemp_RunStatus.aTemp;
		}break;
	}
	return tTempValue;
}




static void sfTempMonitor_MotorCheck(void){
	
	if(pgfTempMonitor_PeriodEvent != NULL)
		//计算周期，单位为此时间的注册时间周期
		pgfTempMonitor_PeriodEvent(gfTempMonitor_GetNowValue());

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void sfTempMonitor_UpdateTemp_For_D11(void){
	uint16 	tTADCValue;
	tTADCValue = gfADC_GetAndStartConvert(DC_ADC_UserID_For_TS);
	soTemp_RunStatus.aTemp = ConvertTempFromVoltage(tTADCValue);
	gfCOOLFAN_DriveTheFan(soTemp_RunStatus.aTemp);	
}

// The Temperature is sampled from SHT20
// The Current Temperature will be update when SHT20 initial.
// static void sfTempMonitor_UpdateTemp_For_D03(void){
// }







