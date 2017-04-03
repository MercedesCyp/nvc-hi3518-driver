#include "Humidity.h"

#include "../HAL/SHT20.h"
#include "../HAL/PeriodEvent.h"
#include "../ProInfo.h"







int32 gfInitDrv_HumidityMonitor(uint8 iRemain){
	
	uint16 tChip = DF_GLOBAL_GetChipInfo;	
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	switch(tChip){
	case DC_CHIP_3518C:{// 3518C
	switch(tPro){
		case DC_Pro_D03:{
			
		}break;
		case DC_Pro_D01:
		case DC_Pro_D04:
		case DC_Pro_D11:
		return 1;
		default:return -1;
	}}break;
	case DC_CHIP_3518E:{
	switch(tPro){
		case DC_Pro_F02:
		case DC_Pro_F05:
		case DC_Pro_F07:
		case DC_Pro_F08:
		return 1;
		default:return -1;
	}}break;
	default:return -1;
	}
#if OPEN_DEBUG
	NVCPrint("Humidity Monitor ^-^ Started!\r\n");
#endif
	return 0;
}

int32 gfUninitDrv_HumidityMonitor(uint8 iRemain){
	
	uint16 tChip = DF_GLOBAL_GetChipInfo;	
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	switch(tChip){
	case DC_CHIP_3518C:{// 3518C
	switch(tPro){
		case DC_Pro_D03:{
			
		}break;
		default:return -1;
	}}break;
	case DC_CHIP_3518E:{
	switch(tPro){
		default:return -1;
	}}break;
	default:return -1;
	}
	return 0;
}



int32 gfHumidity_GetNowValue(void){
	int32 tHumidityValue = 0;
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_D03:{
			tHumidityValue = gfSHT20Opt_GetMeasure(DC_SHT20OptCMD_Get_Humidity);
		}break;
	}
	return tHumidityValue;
}


static void sfHumidityMonitor_MotorCheck(void);
void gfHumidityMonitor_SetReport(uint8 iCmd , uint32 iFreq){
	uint32 MotoChackFreq;
	
	MotoChackFreq = iFreq*10;
	if( iCmd&DC_HUMIDITY_REPORT_Start ){
		gfPE_RegisterEvent( sfHumidityMonitor_MotorCheck, MotoChackFreq ,DC_PE_TYPE_UNINT );
		
	}else if( iCmd&DC_HUMIDITY_REPORT_Stop ){
		gfPE_UnregisterEvent(sfHumidityMonitor_MotorCheck,DC_PE_TYPE_UNINT);
	}
	
}

void (*pgfHumidityMonitor_PeriodEvent)(int32 iHumidity) = NULL;
static void sfHumidityMonitor_MotorCheck(void){
	
	if( pgfHumidityMonitor_PeriodEvent != NULL )
		pgfHumidityMonitor_PeriodEvent(gfHumidity_GetNowValue());
	
}
