#include "HI3518_HAL.h"
#include "HAL.h"




// ================================================================================
// ------------------------------------------------------------>Local Definition
#define DF_Timer_ClrIntFlag(_g)		{DF_TIMER_SetINTCLR(0x01,(_g));}

#define DF_IsTimerInt(_x) 			((DF_TIMER_GetRIS(_x))?1:0)

#define DF_ClrIntFlag(_x) 			DF_Timer_ClrIntFlag(_x)


#define DF_TimerCLK_Set3MCLK(_x)	{\
									uint32 _v;\
									_v = HAL_readl(HAL_SCCTRL_ADDR);\
									_v &= ~DC_TimerCLK_ov(_x);\
									HAL_writel(_v,(HAL_SCCTRL_ADDR));}

#define DF_Timer_Enable(_g)		{uint32 _v;\
								_v=DF_TIMER_GetControl(_g);\
								_v|=0x80;\
								DF_TIMER_SetControl(_v,(_g));}

#define DF_Timer_Disable(_g)	{uint32 _v;\
								_v = DF_TIMER_GetControl(_g);\
								_v &= ~0x80;\
								DF_TIMER_SetControl(_v,(_g));}






// ------------------------------------------------------------>Quote
static irqreturn_t sfTIME_2_3_IntFunction(int irq, void *id);






// ------------------------------------------------------------>Variable
#define DC_TIMER_STATUS_Busy2	0x01
#define DC_TIMER_STATUS_Busy3	0x02
static uint8 sStatus;
static void (*spfTimer2ServerFunction)(void);
static void (*spfTimer3ServerFunction)(void);





// ------------------------------------------------------------>Local Function
// -------------------------------------------------->External
static int32 sfTIMER_Init(void){
	spfTimer2ServerFunction = NULL;
	spfTimer3ServerFunction = NULL;
	sStatus = 0;
	
	if( request_irq( IRQ_TM2_TM3, sfTIME_2_3_IntFunction,IRQF_SHARED,"NVC_HAL_TIMER",&sStatus) ){
#if OPEN_DEBUG
	NVCPrint("The Service Timer of PTZ requests fail!");
#endif
		return -1;
	}
#if OPEN_DEBUG
	NVCPrint("The Service Timer of PTZ requests Success!");
#endif
	return 0;
}
static int32 sfTIMER_Uninit(void){
	free_irq( IRQ_TM2_TM3, &sStatus );
	return 0;
}


static int32 sfTIMER_Config(uint8 iChannel,mTIMER_param* iIfo){
	uint32 tTConf = 0;
	uint32 tLoadVal;
	
	switch(iChannel){
		case 0:{
			if( sStatus&DC_TIMER_STATUS_Busy2 )
				return -2;
			iChannel = 2;
			DF_Timer_Disable(2);
			spfTimer2ServerFunction = iIfo->aHandler;
			
		}break;
		case 1:{
			if( sStatus&DC_TIMER_STATUS_Busy3 )
				return -2;
			iChannel = 3;
			DF_Timer_Disable(3);
			spfTimer3ServerFunction = iIfo->aHandler;
			
		}break;
		default:break;
	}
	DF_TimerCLK_Set3MCLK(iChannel);
	
	tTConf = 0x40|0x20|0x04|0x02;
	DF_TIMER_SetControl(tTConf,iChannel);
	
	tLoadVal = (iIfo->aTime*3)>>4;
	DF_TIMER_SetLoad(tLoadVal,iChannel);
	DF_TIMER_SetBLoad(tLoadVal,iChannel);
	return 0;
}

static int32 sfTIMER_Start(uint8 iChannel){
	if( iChannel == 0 ){
		DF_Timer_Enable(2);
	}else if( iChannel == 1 ){
		DF_Timer_Enable(3);
	}else{
			return -1;
	}
	return 0;
}

static int32 sfTIMER_Stop(uint8 iChannel){
	if( iChannel == 0 ){
		DF_Timer_Disable(2);
	}else if( iChannel == 1 ){
		DF_Timer_Disable(3);
	}else{
			return -1;
	}
	return 0;
}







mClass_Timer const gcoClass_Timer = {
	.prfInit 		= sfTIMER_Init,
	.prfUninit		= sfTIMER_Uninit,
	.prfConfig		= sfTIMER_Config,
	.prfStart		= sfTIMER_Start,
	.prfStop		= sfTIMER_Stop
};







// -------------------------------------------------->Internal
static irqreturn_t sfTIME_2_3_IntFunction(int irq, void *id){
	if(DF_IsTimerInt(2)){
		if( spfTimer2ServerFunction != NULL )
			spfTimer2ServerFunction();
		DF_ClrIntFlag(2);	
		return IRQ_HANDLED;
	}
	if(DF_IsTimerInt(3)){
		if( spfTimer3ServerFunction != NULL )
			spfTimer3ServerFunction();
		DF_ClrIntFlag(3);
		return IRQ_HANDLED;
	}	
	return -1;
}
