// ================================================================================
//| 默认：
//| 	AD通道0采集光敏电阻的阻值
//| 	当值低于32时判定为晚上
//| 	值高于64时判定为白天
//| 	
//| 
//| 适用：
//| 	D01 D03 D04 D11 F05 F08
//| 
//| 
//| 
//| 
//| 
// ================================================================================
// ------------------------------------------------------------>Included File
#include "CTA.h"
#include "../HAL/HAL.h"
#include "../GlobalParameter.h"










// ------------------------------------------------------------>Local Function Definition
static int32 	sfdefLDR_Init(void);
static int32 	sfdefLDR_Uninit(void);
static int32 	sfdefLDR_GetStatus(void);
static int32	sfdefLDR_RegistCallBack(void (*iHandle)(uint8));
static int32	sfdefLDR_GetSensitive(uint8 *);
static int32	sfdefLDR_SetSensitive(uint8);
static void		sfLDR_MotorCheck(void);
       void 	sfLDR_Judge(uint32 iVal);
       void 	sfLDR_FilterBuf_Assimilate(uint32 iVal);
       uint32 	sfLDR_Filter(uint32 iVal);
// ------------------------------------------------------------>Local Macro
#define DC_LDR_CheckFreq			3	//300ms

#define DC_LDR_FilterBreakLimit		100

#define DC_LDR_FilterBreakTimes		5

#define DC_LDR_FilterOrder1Steps	5

#define DC_LDR_DLim		64
#define DC_LDR_NLim		32

// ------------------------------------------------------------>Local data type
typedef struct{
	uint16 	aDLimit;
	uint16 	aNLimit;
	uint16 	aLSValue;
	uint8 	aStatus;
#define DC_DNMonitor_DAY 			0x01
#define DC_DNMonitor_NIGHT 			0x02
}mDNMonitor;











// ------------------------------------------------------------>Variable
static mGPIOPinArr sdefPinArr_LDR;

mClass_LDR	gClassLDR = {
    .apPinArr       =   &sdefPinArr_LDR,
	.afInit			=	sfdefLDR_Init,
	.afUninit		=	sfdefLDR_Uninit,
	.afGetStatus	=	sfdefLDR_GetStatus,
	.afRegCallBack	=	sfdefLDR_RegistCallBack,
	.afSetSensitive = 	sfdefLDR_SetSensitive,
	.afGetSensitive =	sfdefLDR_GetSensitive,
};
static mDNMonitor	sDNMonitor={
	.aDLimit 		= DC_LDR_DLim,
	.aNLimit 		= DC_LDR_NLim,
	.aStatus		= 0,
};
static uint32		saFltOrder1[DC_LDR_FilterOrder1Steps]={0};
static void (*gfDNMonitor_ReportStateChange)(uint8 iStatus) = NULL;










// ------------------------------------------------------------>Local Function
// ---------------------------------------->sfdefLDR_Init
static int32 sfdefLDR_Init(void){
	
	uint32 tVal;
	uint8 _i;
	gClassHAL.ADC->prfSetOpt( DC_HAL_ADCOpt_StartCov|DC_HAL_ADChannel(0) );
	_i=0;
	do{
		udelay(2);
		tVal = gClassHAL.ADC->prfGetValue(DC_HAL_ADChannel(0));
		_i++;
	}while((_i<150)&&(tVal&DC_HAL_ADCRet_Busy));
	
#if DEBUG_INIT
	if( _i == 150 ){
		NVCPrint("LDR IINI Failed");
		return -1;
	}
#endif
	
	sfLDR_FilterBuf_Assimilate( tVal&DC_HAL_ADCOpt_ValMask );
	sfLDR_Judge( tVal&DC_HAL_ADCOpt_ValMask );
	
	gClassHAL.PeriodEvent->afRegister( DC_HAL_PE_UNINT, sfLDR_MotorCheck, DC_LDR_CheckFreq );

#if DEBUG_INIT
	NVCPrint("The LDR module Start! OK");
#endif
	return 0;
}

// ---------------------------------------->sfdefLDR_Uninit
static int32 sfdefLDR_Uninit(void){
	
	gClassHAL.PeriodEvent->afCancel( DC_HAL_PE_UNINT, sfLDR_MotorCheck );
	return 0;
}

// ---------------------------------------->sfdefLDR_GetStatus
static int32 sfdefLDR_GetStatus(void){
	int32 tRet = DC_LDR_DAY;
	if( sDNMonitor.aStatus&DC_DNMonitor_DAY ){
		tRet = DC_LDR_DAY;
	}else if( sDNMonitor.aStatus&DC_DNMonitor_NIGHT ){
		tRet = DC_LDR_Night;
	}
	return tRet;
}

// ---------------------------------------->sfdefLDR_RegistCallBack
static int32 sfdefLDR_RegistCallBack(void (*iHandle)(uint8)){
	gfDNMonitor_ReportStateChange = iHandle;
	return 0;
}


static int32	sfdefLDR_SetSensitive(uint8 iDgr){
	sDNMonitor.aDLimit	= 64;
	sDNMonitor.aNLimit	= 32;
#if DEBUG_LDR
	NVCPrint("LDR Set Sensitive is not supported!\r\n");
#endif
	return 0;
}


static int32	sfdefLDR_GetSensitive(uint8 *ipDgr){
	*ipDgr = 0;
	return 0;
}


// ---------------------------------------->sfLDR_MotorCheck
static void	sfLDR_MotorCheck(void){
	uint32 tVal;
	tVal = gClassHAL.ADC->prfGetValue(DC_HAL_ADChannel(0));
	gClassHAL.ADC->prfSetOpt( DC_HAL_ADCOpt_StartCov | DC_HAL_ADChannel(0) );
	
	tVal &= DC_HAL_ADCOpt_ValMask;
	sfLDR_Filter( tVal );
}

// ---------------------------------------->sfLDR_Filter
// Filter weight 8 == 1 1 1 1 2 2
uint32 sfLDR_Filter(uint32 iVal){
	static uint32 tOri = 0;
	static uint8 tCount = 0;
	uint8 _i;
	uint32 tSumVal;
	
	{
		uint32 tDiff;
		if( iVal > tOri ){
			tDiff = iVal - tOri;
		}else{ //if( iVal <= tOri ){
			tDiff = tOri - iVal;
		}
		if( tDiff >= DC_LDR_FilterBreakLimit ){
			tCount++;
			if( tCount == DC_LDR_FilterBreakTimes ){
				sfLDR_FilterBuf_Assimilate(iVal);
				tCount = 0;
			}
		}else{
			tCount = 0;
		}
	}
	
	tSumVal = (iVal<<3)\
			+ (saFltOrder1[0])\
			+ (saFltOrder1[1])\
			+ (saFltOrder1[2])\
			+ (saFltOrder1[3])\
			+ (saFltOrder1[4]<<2);
	tSumVal >>= 4;
	
	for( _i = 0; _i < DC_LDR_FilterOrder1Steps-1; _i++ ){
		saFltOrder1[_i] = saFltOrder1[ _i+1 ];
	}
	saFltOrder1[_i] = tSumVal;
	
	if( tCount == 0 )
		tOri = tSumVal;
	
	sfLDR_Judge(tSumVal);
#if DEBUG_LDR
	// NVCPrint_h("The LDR Value:%d\r\n",(int)tSumVal);
	NVCPrint("The LDR Value:%d",(int)tSumVal);
#endif
	
	return tSumVal;
}

// ---------------------------------------->sfLDR_FilterBuf_Assimilate
void sfLDR_FilterBuf_Assimilate(uint32 iVal){//homogenization or assimilate
	uint8 _i;
	for( _i=0;_i<DC_LDR_FilterOrder1Steps;_i++ )
		saFltOrder1[ _i ] = iVal;
}

// ---------------------------------------->sfLDR_Judge
void sfLDR_Judge(uint32 iVal){
	
	if( iVal > sDNMonitor.aDLimit ){
		if( sDNMonitor.aStatus&DC_DNMonitor_NIGHT )
			if( gfDNMonitor_ReportStateChange )
				gfDNMonitor_ReportStateChange( DC_LDR_DAY );
		sDNMonitor.aStatus &= ~DC_DNMonitor_NIGHT;
		sDNMonitor.aStatus |= DC_DNMonitor_DAY;
		
	}else if( iVal < sDNMonitor.aNLimit ){
		if( sDNMonitor.aStatus&DC_DNMonitor_DAY )
			if( gfDNMonitor_ReportStateChange )
				gfDNMonitor_ReportStateChange( DC_LDR_Night );
		sDNMonitor.aStatus &= ~DC_DNMonitor_DAY;
		sDNMonitor.aStatus |= DC_DNMonitor_NIGHT;
		
	}
	sDNMonitor.aLSValue = (uint16)iVal;
}
