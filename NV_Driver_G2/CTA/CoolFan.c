// ================================================================================
//| Ĭ��:
//|		Ŀǰ��ֻ��D11���з��ȣ����ң����ȵ�������񣬻��������������Ƶ�
//|		D11
//|		������������ʱ�򣬲���Ҫ�����¶�ģ�飬��Ҫ�ڳ�ʼ����ʱ��Ҫ�ȳ�ʼ���¶ȹ���
//|		���� Init and Config_Report
//|		Ȼ���ٵ��ø��ĵ��ĳ�ʼ��������ʹ֮���¶�֮�佨����ϵ
//|		
//|		
//|		
//| 
//|	Ŀǰ���ã�
//| 	
//| 
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "../GlobalParameter.h"
#include "../HAL/HAL.h"

#define DC_CoolFan_PWMDuty			300
#define DC_CoolFan_PWMMinWidth		0		// 0
#define DC_CoolFan_PWMMaxWidth		300		// 1

#define DC_CoolFan_DEF_EXPORT		0
#define DC_CoolFan_MIN_EXPORT		180
#define DC_CoolFan_MAX_EXPORT		300

#define DC_COFAN_OH_Temp		500
#define DC_COFAN_OOH_Temp		650

#define DC_COFAN_LBreak			-300
#define DC_COFAN_HBreak			1200



#define DF_COOFAN_ExportIncreasing	{sExportValue+=4;}
#define DF_COOFAN_ExportDecreasing	{sExportValue-=4;}

static uint16 sExportValue = DC_CoolFan_DEF_EXPORT;

void gfCOOLFAN_DriveTheFan(int32 tTemp){
	//DC_COOLFAN_INCREASING_POWER
	if(tTemp >= DC_COFAN_OH_Temp){
		if(tTemp>=DC_COFAN_OOH_Temp){
			sExportValue = DC_CoolFan_MAX_EXPORT;
		}else{
			if(sExportValue<DC_CoolFan_MIN_EXPORT)
				sExportValue=DC_CoolFan_MIN_EXPORT;
			else if(sExportValue<DC_CoolFan_MAX_EXPORT)
				DF_COOFAN_ExportIncreasing;
		}
	}else{ //DC_COOLFAN_DECREASING_POWER
		if( tTemp < DC_COFAN_LBreak ){
			sExportValue = DC_CoolFan_MAX_EXPORT;
		}else{			
			if(sExportValue>DC_CoolFan_MIN_EXPORT)
				sExportValue = DC_CoolFan_DEF_EXPORT;
		}
	}
#if DEBUG_FAN
	NVCPrint("Temperature: %d\r\nExport: %d\r\n",(int)tTemp,(int)sExportValue);
#endif	
	gClassHAL.PWM->pInfo->aDuty	= DC_CoolFan_PWMDuty;
	gClassHAL.PWM->pInfo->aWidth = sExportValue;
	gClassHAL.PWM->pInfo->aNum 	= 0;
	gClassHAL.PWM->pInfo->aCmd 	= DC_PWM_CmdKeep;
	gClassHAL.PWM->prfStart();
}
