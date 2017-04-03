/*
Coder:      aojie.meng
Date:       2015-9-15

Abstract:
    想驱动中需要用到定时，周期
*/
//==============================================================================
// C
// Linux
// local
#include "HAL.h"
// remote
#include "../Tool/String.h"

//=============================================================================
// DATA TYPE
typedef struct {
	void (*afHandler)(void);
	uint32 aPeriod;
	uint32 aCount;
} aPE_RegistIfo;

//==============================================================================
//extern
//local
static int32 sfInitDrv_PeroidEvent(void);
static int32 sfUnInitDrv_PeroidEvent(void);
static int32 sfPE_RegisterEvent(uint8 iMode, void (*iHandler)(void),
                                uint32 iTime);
static int32 sfPE_UnregisterEvent(uint8 iMode, void (*iHandler)(void));
static int32 sfPE_Control(uint8 iCmd);
static void sfPeoridEventHandleUNINT(unsigned long iarg);
static void sfPeoridEventHandleINT(void);
//global

//==============================================================================
//extern
//local
static uint8 sPEState;
#define DC_PES_10MS_BUSY	0x01
#define DC_PES_1MS_BUSY		0x02
//
static aPE_RegistIfo sPE_RegisterIfo_UNINT[DC_HAL_PE_UNINTCap];
static aPE_RegistIfo sPE_RegisterIfo_INT[DC_HAL_PE_INTCap];
struct timer_list PeroidEvent_Timer;
//global
mClass_PriodEvent const gcoClass_PeriodEvent = {
	.afInit = sfInitDrv_PeroidEvent,
	.afUninit = sfUnInitDrv_PeroidEvent,
	.afRegister = sfPE_RegisterEvent,
	.afCancel = sfPE_UnregisterEvent,
	.afControl = sfPE_Control,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfInitDrv_PeroidEvent(void)
@introduction:
    初始化事件调度器,注意，这个功能关系到驱动框架的正常运行，这里千万不要出差错

@parameter:
    void

@return:
    0       初始化成功
    -1      配置失败

*/
static int32 sfInitDrv_PeroidEvent(void)
{

	sPEState = 0;
	gClassStr.afMemset((uint8 *) sPE_RegisterIfo_UNINT, 0,
	                   sizeof(aPE_RegistIfo) * DC_HAL_PE_UNINTCap);
	gClassStr.afMemset((uint8 *) sPE_RegisterIfo_INT, 0,
	                   sizeof(aPE_RegistIfo) * DC_HAL_PE_INTCap);

	{
		int32 tRet;
		mTIMER_param tSet_Param = {
			DC_HAL_PE_INTPeriod * 1000,
			sfPeoridEventHandleINT,
			DC_HAL_TIMER_cmdPTMode
		};

		tRet =
		    gClassHAL.Timer->prfConfig(DC_HAL_TIMERChannel(0),
		                               &tSet_Param);

		if (tRet) {
			goto GT_PE_INIT_Err;
		}

		tRet = gClassHAL.Timer->prfStart(DC_HAL_TIMERChannel(0));

		if (tRet) {
			goto GT_PE_INIT_Err;
		} else {
#if DEBUG_INIT
			NVCPrint("Start the Int_Period_Event deamon");
#endif
		}
	}

	// kernel timer
	init_timer(&PeroidEvent_Timer);
	PeroidEvent_Timer.function = sfPeoridEventHandleUNINT;
	PeroidEvent_Timer.expires = jiffies + DC_HAL_PE_UNINTPeriod;
	add_timer(&PeroidEvent_Timer);

#if DEBUG_INIT
	NVCPrint("Start the Unint_Period_Event deamon");
#endif
	return 0;

GT_PE_INIT_Err:
#if DEBUG_INIT
	NVCPrint("Start the Int_Period_Event deamon");
#endif
	return -1;
}

//---------- ---------- ---------- ----------
/*  static int32 sfUnInitDrv_PeroidEvent(void)
@introduction:
    释放初始化时注册子资源

@parameter:
    void

@return:
    0   Success

*/
static int32 sfUnInitDrv_PeroidEvent(void)
{
	while (sPEState & DC_PES_1MS_BUSY) ;
	gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(0));

	while (sPEState & DC_PES_10MS_BUSY) ;
	del_timer(&PeroidEvent_Timer);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfPE_RegisterEvent(uint8 iMode,void (*iHandler)(void), uint32 iTime)
@introduction:
    向外部提供注册事件的接口，事件分为两种，一个是随内核调度的，运行在进程上下文
    中，最小时间周期 100ms，第二个是直接向硬件接口层注册的运行于定时器中断中，最
    小时间周期1ms，但为了防止干扰到正常时间片的调度，有关上报事件，以及负载相当
    大的事件应当注册在非中断类型的队列当中（随内核调度）。

@parameter:
    iMode
        DC_HAL_PE_UNINT
            运行内核中断上下文（进程调度间）
        DC_HAL_PE_INT
            运行与中断上下文
    iHandler
        无参，无返回
    iTime
        注册事件的周期，单位根据事件类型不同，会有所不同
@return:
    0   注册成功
    -1  注册失败

*/
#if DEBUG_PERIOD
static uint8 sPeriodEvent = 0;
static uint8 sIntPeriodEvent = 0;
#endif
static int32 sfPE_RegisterEvent(uint8 iMode, void (*iHandler)(void),
                                uint32 iTime)
{
	uint8 _i;

	if (iMode & DC_HAL_PE_UNINT) {
		for (_i = 0; _i < DC_HAL_PE_UNINTCap; _i++)
			if (sPE_RegisterIfo_UNINT[_i].afHandler == NULL) {
				break;
			}

		if (_i == DC_HAL_PE_UNINTCap) {
			return -1;
		} else {
#if DEBUG_PERIOD
			sPeriodEvent++;
#endif
			sPE_RegisterIfo_UNINT[_i].aPeriod = iTime;
			sPE_RegisterIfo_UNINT[_i].aCount = 0;
			sPE_RegisterIfo_UNINT[_i].afHandler = iHandler;
			//return 0;
		}
	} else if (iMode & DC_HAL_PE_INT) {
		for (_i = 0; _i < DC_HAL_PE_INTCap; _i++)
			if (sPE_RegisterIfo_INT[_i].afHandler == NULL) {
				break;
			}

		if (_i == DC_HAL_PE_INTCap) {
			return -1;
		} else {
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
	NVCPrint("Period unint event: %d", (int)sPeriodEvent);
	NVCPrint("Period int event: %d", (int)sIntPeriodEvent);
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfPE_UnregisterEvent( uint8 iMode, void (*iHandler)(void) )
@introduction:
    取消注册的事件

@parameter:
    iMode
        DC_HAL_PE_UNINT
            运行内核中断上下文（进程调度间）
        DC_HAL_PE_INT
            运行与中断上下文
    iHandler
        注册事件

@return:
    0   取消成功

*/
static int32 sfPE_UnregisterEvent(uint8 iMode, void (*iHandler)(void))
{
	uint8 _i;

	if (iMode & DC_HAL_PE_UNINT) {
		for (_i = 0; _i < DC_HAL_PE_UNINTCap; _i++)
			if (sPE_RegisterIfo_UNINT[_i].afHandler == iHandler) {
				break;
			}

		if (_i == DC_HAL_PE_UNINTCap) {
			return -1;
		} else {
#if DEBUG_PERIOD
			sPeriodEvent--;
#endif
			sPE_RegisterIfo_UNINT[_i].afHandler = NULL;
			sPE_RegisterIfo_UNINT[_i].aPeriod = 0;
			sPE_RegisterIfo_UNINT[_i].aCount = 0;
			// return 0;
		}
	} else if (iMode & DC_HAL_PE_INT) {
		for (_i = 0; _i < DC_HAL_PE_INTCap; _i++)
			if (sPE_RegisterIfo_INT[_i].afHandler == iHandler) {
				break;
			}

		if (_i == DC_HAL_PE_INTCap) {
			return -1;
		} else {
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
	NVCPrint("Period unint event: %d", (int)sPeriodEvent);
	NVCPrint("Period int event: %d", (int)sIntPeriodEvent);
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfPE_Control( uint8 iCmd)
@introduction:
    暂停中断类型事件

@parameter:
    iCmd
        DC_HAL_PE_INT_PUSE
            暂停
        DC_HAL_PE_INT_RESUME
            恢复

@return:
    0   Success

*/
static int32 sfPE_Control(uint8 iCmd)
{
	if (iCmd & DC_HAL_PE_INT_PUSE) {
		gClassHAL.Timer->prfStop(DC_HAL_TIMERChannel(0));
	} else if (iCmd & DC_HAL_PE_INT_RESUME) {
		gClassHAL.Timer->prfStart(DC_HAL_TIMERChannel(0));
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void sfPeoridEventHandleUNINT(unsigned long iarg)
@introduction:
    非中断类型的时间调度程序

@parameter:
    iarg
        然并卵

@return:
    void

*/
static void sfPeoridEventHandleUNINT(unsigned long iarg)
{
	uint8 _i;

	sPEState |= DC_PES_10MS_BUSY;

	mod_timer(&PeroidEvent_Timer, jiffies + DC_HAL_PE_UNINTPeriod);
	// printk("1");
	for (_i = 0; _i < DC_HAL_PE_UNINTCap; _i++) {
		if (sPE_RegisterIfo_UNINT[_i].afHandler != NULL) {
			sPE_RegisterIfo_UNINT[_i].aCount++;

			if (sPE_RegisterIfo_UNINT[_i].aCount >=
			    sPE_RegisterIfo_UNINT[_i].aPeriod) {
				sPE_RegisterIfo_UNINT[_i].afHandler();
				sPE_RegisterIfo_UNINT[_i].aCount = 0;
			}
		}
	}
	sPEState &= ~DC_PES_10MS_BUSY;

}

//---------- ---------- ---------- ----------
/*  static void sfPeoridEventHandleINT(void)
@introduction:
    中断类型的时间调度程序

@parameter:
    void

@return:
    void

*/
static void sfPeoridEventHandleINT(void)
{
	uint8 _i;

	sPEState |= DC_PES_1MS_BUSY;
	// printk("2");
	for (_i = 0; _i < DC_HAL_PE_INTCap; _i++) {
		if (sPE_RegisterIfo_INT[_i].afHandler != NULL) {
			sPE_RegisterIfo_INT[_i].aCount++;
			if (sPE_RegisterIfo_INT[_i].aCount >=
			    sPE_RegisterIfo_INT[_i].aPeriod) {
				sPE_RegisterIfo_INT[_i].afHandler();
				sPE_RegisterIfo_INT[_i].aCount = 0;
			}
		}
	}
	sPEState &= ~DC_PES_1MS_BUSY;
}
