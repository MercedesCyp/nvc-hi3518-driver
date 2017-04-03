#include "PeriodEvent.h"

#include <linux/timer.h>
#include "HAL_Timer.h"
#include "USERHAL.h"




//the timer unit is 100ms
#define DC_PE_TimerUnit	10
#define DC_PE_EventCap_10MS	8
#define DC_PE_EventCap_1MS	4


typedef struct{
	void (*afHandler)(void);
	uint16 aPeriod;
	uint16 aCount;
}aPE_RegisterInfo;



static uint8 sPEState;
#define DC_PES_10MS_BUSY	0x01
#define DC_PES_1MS_BUSY		0x02
 
//注册的这个时间运行于进程上下文，所以不会存在与驱动操作冲突的情况
aPE_RegisterInfo sPE_RegisterInfo_10ms[DC_PE_EventCap_10MS];
//运行于硬件中断中，中断的产生会有可能干扰到所有正在运行的程序
aPE_RegisterInfo sPE_RegisterInfo_1ms[DC_PE_EventCap_1MS];

struct timer_list PeroidEvent_Timer;

static void sfPeoridEventHandle10MS(unsigned long iarg);
static irqreturn_t sfPeoridEventHandle1MS(int irq, void *id);


int32 gfInitDrv_PeroidEvent(void){
	
	int theRct;
	
	sPEState = 0;
	
	gfMemset((uint8*)sPE_RegisterInfo_1ms,0,sizeof(aPE_RegisterInfo)*DC_PE_EventCap_1MS);
	gfMemset((uint8*)sPE_RegisterInfo_10ms,0,sizeof(aPE_RegisterInfo)*DC_PE_EventCap_10MS);
	
	
	theRct = request_irq(IRQ_TM2_TM3, sfPeoridEventHandle1MS,IRQF_SHARED,"NVC_PeriodEvent",&sPE_RegisterInfo_1ms);
	if(theRct==0){
		DF_Timer2_Enable;
#if OPEN_DEBUG
	NVCPrint("Register IRQ_TM2_TM3 For PeriodEvent Success!\r\n");
#endif
	}
	
	// kernel timer
	init_timer(&PeroidEvent_Timer);
	PeroidEvent_Timer.function	= sfPeoridEventHandle10MS;
	PeroidEvent_Timer.expires	= jiffies + DC_PE_TimerUnit;
	add_timer(&PeroidEvent_Timer);
	
	
	return 0;
}


int32 gfUnInitDrv_PeroidEvent(void){
	while(sPEState&DC_PES_1MS_BUSY);
	free_irq(IRQ_TM2_TM3,&sPE_RegisterInfo_1ms);
	
	
	while(sPEState&DC_PES_10MS_BUSY);
	del_timer(&PeroidEvent_Timer);
	
	return 0;
}

static uint8 sPeriodEvent=0;
static uint8 sIntPeriodEvent=0;
int32 gfPE_RegisterEvent( void (*iHandler)(void), uint16 iTime ,uint8 iMode)
{
	uint8 _i;
	
	if(iMode&DC_PE_TYPE_UNINT){
		sPeriodEvent++;
		for(_i=0; _i<DC_PE_EventCap_10MS;_i++)
			if( sPE_RegisterInfo_10ms[_i].afHandler == NULL )
				break;
		
		if(_i == DC_PE_EventCap_10MS){
			return -1;
		}else{
			sPE_RegisterInfo_10ms[_i].afHandler = iHandler;
			sPE_RegisterInfo_10ms[_i].aPeriod = iTime;
			//return 0;
		}
	}else if(iMode&DC_PE_TYPE_INT){
		sIntPeriodEvent++;
		for(_i=0; _i<DC_PE_EventCap_1MS;_i++)
			if( sPE_RegisterInfo_1ms[_i].afHandler == NULL )
				break;
		
		if(_i == DC_PE_EventCap_1MS){
			return -1;
		}else{
			sPE_RegisterInfo_1ms[_i].afHandler = iHandler;
			sPE_RegisterInfo_1ms[_i].aPeriod = iTime;
			//return 0;
		}
	}
	return 0;
}



int32 gfPE_UnregisterEvent( void (*iHandler)(void) ,uint8 iMode)
{
	uint8 _i;
	
	if(iMode&DC_PE_TYPE_UNINT){
		for(_i=0; _i<DC_PE_EventCap_10MS;_i++)
			if( sPE_RegisterInfo_10ms[_i].afHandler == iHandler )
				break;
		
		if(_i == DC_PE_EventCap_10MS){
			return -1;
		}else{
			sPE_RegisterInfo_10ms[_i].afHandler = NULL;
			sPE_RegisterInfo_10ms[_i].aPeriod = 0;
			sPE_RegisterInfo_10ms[_i].aCount = 0;
			return 0;
		}
	}else if(iMode&DC_PE_TYPE_INT){
		for(_i=0; _i<DC_PE_EventCap_1MS;_i++)
			if( sPE_RegisterInfo_1ms[_i].afHandler == iHandler )
				break;
		
		if(_i == DC_PE_EventCap_1MS){
			return -1;
		}else{
			sPE_RegisterInfo_1ms[_i].afHandler = NULL;
			sPE_RegisterInfo_1ms[_i].aPeriod = 0;
			sPE_RegisterInfo_1ms[_i].aCount = 0;
			return 0;
		}
	}
	
	return 0;
}

static void sfPeoridEventHandle10MS(unsigned long iData)
{
	uint8 _i;
	
	sPEState |= DC_PES_10MS_BUSY;
	
	mod_timer(&PeroidEvent_Timer, jiffies + DC_PE_TimerUnit);
	
	for(_i=0; _i<DC_PE_EventCap_10MS;_i++){
		
		if( sPE_RegisterInfo_10ms[_i].afHandler != NULL ){
			sPE_RegisterInfo_10ms[_i].aCount++;
			
			if(sPE_RegisterInfo_10ms[_i].aCount >= sPE_RegisterInfo_10ms[_i].aPeriod ){
				sPE_RegisterInfo_10ms[_i].afHandler();
				sPE_RegisterInfo_10ms[_i].aCount = 0;
			}
		}
	}
	sPEState &= ~DC_PES_10MS_BUSY;
}


static irqreturn_t sfPeoridEventHandle1MS(int irq, void *id){
	uint8 _i;
	
	sPEState |= DC_PES_1MS_BUSY;
	if(DF_Timer2_IntFlag){
		for(_i=0; _i<DC_PE_EventCap_1MS;_i++){
			if( sPE_RegisterInfo_1ms[_i].afHandler != NULL ){
				
				sPE_RegisterInfo_1ms[_i].aCount++;
				
				if(sPE_RegisterInfo_1ms[_i].aCount >= sPE_RegisterInfo_1ms[_i].aPeriod ){
					sPE_RegisterInfo_1ms[_i].afHandler();
					sPE_RegisterInfo_1ms[_i].aCount = 0;
				}
			}
		}	
		DF_Timer2_ClrIntFlag;	
	}
	sPEState &= ~DC_PES_1MS_BUSY;
	return IRQ_HANDLED;
}

