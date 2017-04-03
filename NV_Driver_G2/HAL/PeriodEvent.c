#include "HAL.h"
#include "../Tool/String.h"








// ================================================================================
// ---------------------------------------------------------------------->Definition
typedef struct{
	void (*afHandler)(void);
	uint32 aPeriod;
	uint32 aCount;
}aPE_RegistIfo;










// ---------------------------------------------------------------------->Quote
static void sfPeoridEventHandleUNINT(unsigned long iarg);
static void sfPeoridEventHandleINT(void);










// ---------------------------------------------------------------------->Variable
aPE_RegistIfo	sPE_RegisterIfo_UNINT[DC_HAL_PE_UNINTCap];
aPE_RegistIfo	sPE_RegisterIfo_INT[DC_HAL_PE_INTCap];

struct timer_list PeroidEvent_Timer;


static uint8 sPEState;
#define DC_PES_10MS_BUSY	0x01
#define DC_PES_1MS_BUSY		0x02










// ---------------------------------------------------------------------->Local Function
// ------------------------------------------------------------>External
static int32 sfInitDrv_PeroidEvent(void){
	
	sPEState = 0;
	gClassStr.afMemset((uint8*)sPE_RegisterIfo_UNINT,0,sizeof(aPE_RegistIfo)*DC_HAL_PE_UNINTCap);
	gClassStr.afMemset((uint8*)sPE_RegisterIfo_INT,0,sizeof(aPE_RegistIfo)*DC_HAL_PE_INTCap);
	
	{
		int32 tRet;
		mTIMER_param tSet_Param={
			DC_HAL_PE_INTPeriod*1000,
			sfPeoridEventHandleINT,
			DC_HAL_TIMER_cmdPTMode
		};
		
		tRet = gClassHAL.Timer->prfConfig( DC_HAL_TIMERChannel(0), &tSet_Param );
		
		if( tRet )
			goto GT_PE_INIT_Err;
		
		tRet = gClassHAL.Timer->prfStart(DC_HAL_TIMERChannel(0));
		
		if( tRet ){
			goto GT_PE_INIT_Err;
		}else{
#if DEBUG_INIT
			NVCPrint("Start the Int_Period_Event deamon");
#endif
		}
	}
	
	// kernel timer
	init_timer(&PeroidEvent_Timer);
	PeroidEvent_Timer.function	= sfPeoridEventHandleUNINT;
	PeroidEvent_Timer.expires	= jiffies + DC_HAL_PE_UNINTPeriod;
	add_timer(&PeroidEvent_Timer);
	
#if DEBUG_INIT
	NVCPrint("Start the Unint_Period_Event deamon");
#endif
	return 0;
	
GT_PE_INIT_Err:
#if OPEN_DEBUG
	NVCPrint("Start the Int_Period_Event deamon");
#endif
return -1;
}




static int32 sfUnInitDrv_PeroidEvent(void){
	while(sPEState&DC_PES_1MS_BUSY);
	gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(0));
	
	
	while(sPEState&DC_PES_10MS_BUSY);
	del_timer(&PeroidEvent_Timer);
	
	return 0;
}



#if DEBUG_PERIOD
static uint8 sPeriodEvent=0;
static uint8 sIntPeriodEvent=0;
#endif
static int32 sfPE_RegisterEvent(uint8 iMode,void (*iHandler)(void), uint32 iTime)
{
	uint8 _i;
	
	if(iMode&DC_HAL_PE_UNINT){
		for(_i=0; _i<DC_HAL_PE_UNINTCap;_i++)
			if( sPE_RegisterIfo_UNINT[_i].afHandler == NULL )
				break;
		
		if(_i == DC_HAL_PE_UNINTCap){
			return -1;
		}else{
#if DEBUG_PERIOD
			sPeriodEvent++;
#endif
			sPE_RegisterIfo_UNINT[_i].aPeriod = iTime;
			sPE_RegisterIfo_UNINT[_i].aCount = 0;
			sPE_RegisterIfo_UNINT[_i].afHandler = iHandler;
			//return 0;
		}
	}else if(iMode&DC_HAL_PE_INT){
		for(_i=0; _i<DC_HAL_PE_INTCap;_i++)
			if( sPE_RegisterIfo_INT[_i].afHandler == NULL )
				break;
		
		if(_i == DC_HAL_PE_INTCap){
			return -1;
		}else{
#if DEBUG_PERIOD
			sIntPeriodEvent++;
#endif
			sPE_RegisterIfo_INT[_i].aPeriod = iTime;
			sPE_RegisterIfo_INT[_i].aCount = 0;
			sPE_RegisterIfo_INT[_i].afHandler = iHandler;
			//return 0;
		}
	}
#if DEBUG_PERIOD
	NVCPrint("Period unint event: %d",(int)sPeriodEvent );
	NVCPrint("Period int event: %d", (int)sIntPeriodEvent);
#endif
	return 0;
}

static int32 sfPE_UnregisterEvent( uint8 iMode, void (*iHandler)(void) )
{
	uint8 _i;
	
	if(iMode&DC_HAL_PE_UNINT){
		for(_i=0; _i<DC_HAL_PE_UNINTCap;_i++)
			if( sPE_RegisterIfo_UNINT[_i].afHandler == iHandler )
				break;
		
		if(_i == DC_HAL_PE_UNINTCap){
			return -1;
		}else{
#if DEBUG_PERIOD
			sPeriodEvent--;
#endif
			sPE_RegisterIfo_UNINT[_i].afHandler = NULL;
			sPE_RegisterIfo_UNINT[_i].aPeriod = 0;
			sPE_RegisterIfo_UNINT[_i].aCount = 0;
			// return 0;
		}
	}else if(iMode&DC_HAL_PE_INT){
		for(_i=0; _i<DC_HAL_PE_INTCap;_i++)
			if( sPE_RegisterIfo_INT[_i].afHandler == iHandler )
				break;
		
		if(_i == DC_HAL_PE_INTCap){
			return -1;
		}else{
#if DEBUG_PERIOD
			sIntPeriodEvent--;
#endif
			sPE_RegisterIfo_INT[_i].afHandler = NULL;
			sPE_RegisterIfo_INT[_i].aPeriod = 0;
			sPE_RegisterIfo_INT[_i].aCount = 0;
			// return 0;
		}
	}
	
#if DEBUG_PERIOD
	NVCPrint("Period unint event: %d",(int)sPeriodEvent );
	NVCPrint("Period int event: %d", (int)sIntPeriodEvent);
#endif
	return 0;
}

static int32 sfPE_Control( uint8 iCmd){
	if( iCmd&DC_HAL_PE_INT_PUSE ){
		gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(0));
	}else if( iCmd&DC_HAL_PE_INT_RESUME ){
		gClassHAL.Timer->prfStart(DC_HAL_TIMERChannel(0));
	}
	return 0;
}


mClass_PriodEvent const gcoClass_PeriodEvent={
	.afInit 			= sfInitDrv_PeroidEvent,
	.afUninit			= sfUnInitDrv_PeroidEvent,
	.afRegister			= sfPE_RegisterEvent,
	.afCancel			= sfPE_UnregisterEvent,
	.afControl			= sfPE_Control,
};

// ------------------------------------------------------------>Internal
static void sfPeoridEventHandleUNINT(unsigned long iarg){
	uint8 _i;
	
	sPEState |= DC_PES_10MS_BUSY;
	
	mod_timer(&PeroidEvent_Timer, jiffies + DC_HAL_PE_UNINTPeriod);
	
	for(_i=0; _i<DC_HAL_PE_UNINTCap;_i++){
		if( sPE_RegisterIfo_UNINT[_i].afHandler != NULL ){
			sPE_RegisterIfo_UNINT[_i].aCount++;
			
			if(sPE_RegisterIfo_UNINT[_i].aCount >= sPE_RegisterIfo_UNINT[_i].aPeriod ){
				sPE_RegisterIfo_UNINT[_i].afHandler();
				sPE_RegisterIfo_UNINT[_i].aCount = 0;
			}
		}
	}
	sPEState &= ~DC_PES_10MS_BUSY;
	
}

static void sfPeoridEventHandleINT(void){
	uint8 _i;
	
	sPEState |= DC_PES_1MS_BUSY;
	
	for(_i=0; _i<DC_HAL_PE_INTCap;_i++){
		if( sPE_RegisterIfo_INT[_i].afHandler != NULL ){
			sPE_RegisterIfo_INT[_i].aCount++;
			if(sPE_RegisterIfo_INT[_i].aCount >= sPE_RegisterIfo_INT[_i].aPeriod ){
				sPE_RegisterIfo_INT[_i].afHandler();
				sPE_RegisterIfo_INT[_i].aCount = 0;
			}
		}
	}
	sPEState &= ~DC_PES_1MS_BUSY;
}
