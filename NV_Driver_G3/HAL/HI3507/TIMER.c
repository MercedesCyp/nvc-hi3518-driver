/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
#include "HI3507_HAL.h"
// remote
#include "../HAL.h"

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION
// 上电初始化后初始时钟为 3M
#define DF_Timer_ClrIntFlag(_g)		{DF_TIMER_SetINTCLR(0x01,(_g));}

#define DF_IsTimerInt(_x) 			((DF_TIMER_GetRIS(_x))?1:0)

#define DF_ClrIntFlag(_x) 			DF_Timer_ClrIntFlag(_x)

#define DF_TimerCLK_Set3MCLK(_x)	{\
		uint32 _v;\
		_v = HAL_readl(HAL_ADDR_SYSCTRL);\
		_v &= ~DC_TimerCLK_ov(_x);\
		HAL_writel(_v,(HAL_ADDR_SYSCTRL));}

#define DF_Timer_Enable(_g)		{uint32 _v;\
		_v=DF_TIMER_GetControl(_g);\
		_v|=0x80;\
		DF_TIMER_SetControl(_v,(_g));}

#define DF_Timer_Disable(_g)	{uint32 _v;\
		_v = DF_TIMER_GetControl(_g);\
		_v &= ~0x80;\
		DF_TIMER_SetControl(_v,(_g));}

//==============================================================================
//extern
//local
static int32 sfTIMER_Init(void);
static int32 sfTIMER_Uninit(void);
static int32 sfTIMER_Config(uint8 iChannel, mTIMER_param *iIfo);
static int32 sfTIMER_Start(uint8 iChannel);
static int32 sfTIMER_Stop(uint8 iChannel);
static irqreturn_t sfTIME_2_3_IntFunction(int irq, void *id,
        struct pt_regs *regs);
//global

//==============================================================================
//extern
//local
static uint8 sStatus;
#define DC_TIMER_STATUS_Busy2	0x01
#define DC_TIMER_STATUS_Busy3	0x02
static void (*spfTimer2ServerFunction)(void);
static void (*spfTimer3ServerFunction)(void);
//global
mClass_Timer const gcoClass_Timer = {
	.prfInit = sfTIMER_Init,
	.prfUninit = sfTIMER_Uninit,
	.prfConfig = sfTIMER_Config,
	.prfStart = sfTIMER_Start,
	.prfStop = sfTIMER_Stop
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int32 sfTIMER_Init(void)
@introduction:
    初始化定时器

@parameter:
    void

@return:
    0   初始化成功
    -1  初始化失败

*/
static int32 sfTIMER_Init(void)
{
	spfTimer2ServerFunction = NULL;
	spfTimer3ServerFunction = NULL;
	sStatus = 0;

	if (request_irq
	    (IRQ_TM1, sfTIME_2_3_IntFunction, SA_SHIRQ, "NVC_HAL_TIMER",
	     &sStatus)) {
#if DEBUG_INIT
		NVCPrint("The Service Timer of PTZ requests fail!");
#endif
		return -1;
	}
#if DEBUG_INIT
	NVCPrint("The Service Timer of PTZ requests Success!");
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfTIMER_Uninit(void)
@introduction:
    释放初始化时注册的资源

@parameter:
    void

@return:
    0   Success

*/
static int32 sfTIMER_Uninit(void)
{
	free_irq(IRQ_TM1, &sStatus);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfTIMER_Config(uint8 iChannel,mTIMER_param* iIfo)
@introduction:
    根据传入的参数，配置定时器

@parameter:
    iChannel
        定时器通道，由于定时器 1 和 2 为一组的 定时器1 已经被内核占用，因此我们
        在这里只提供 定时器 2 3 的配置，如操作通道超出，将会报错
    iIfo
        将要配置的信息

@return:
    0   SUCCESS
    -2  FAIL

*/
static int32 sfTIMER_Config(uint8 iChannel, mTIMER_param *iIfo)
{
	uint32 tTConf = 0;
	uint32 tLoadVal;

	switch (iChannel) {
	case 0: {
		if (sStatus & DC_TIMER_STATUS_Busy2) {
			return -2;
		}
		iChannel = 2;
		DF_Timer_Disable(2);
		spfTimer2ServerFunction = iIfo->aHandler;

	}
	break;
	case 1: {
		if (sStatus & DC_TIMER_STATUS_Busy3) {
			return -2;
		}
		iChannel = 3;
		DF_Timer_Disable(3);
		spfTimer3ServerFunction = iIfo->aHandler;

	}
	break;
	default:
		break;
	}
	DF_TimerCLK_Set3MCLK(iChannel);

	tTConf = 0x40 | 0x20 | 0x04 | 0x02;
	DF_TIMER_SetControl(tTConf, iChannel);

	tLoadVal = (iIfo->aTime * 3) >> 4;
	DF_TIMER_SetLoad(tLoadVal, iChannel);
	DF_TIMER_SetBLoad(tLoadVal, iChannel);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfTIMER_Start(uint8 iChannel)
@introduction:
    启动相应的定时器

@parameter:
    iChannel、
        定时器通道

@return:
    0   SUCESS
    -1  FAIL

*/
static int32 sfTIMER_Start(uint8 iChannel)
{
	if (iChannel == 0) {
		DF_Timer_Enable(2);
	} else if (iChannel == 1) {
		DF_Timer_Enable(3);
	} else {
		return -1;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfTIMER_Stop(uint8 iChannel)
@introduction:
    停止相应的定时器

@parameter:
    iChannel
        通道号

@return:
    0   SUCCESS
    -1  FAIL

*/
static int32 sfTIMER_Stop(uint8 iChannel)
{
	if (iChannel == 0) {
		DF_Timer_Disable(2);
	} else if (iChannel == 1) {
		DF_Timer_Disable(3);
	} else {
		return -1;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static irqreturn_t sfTIME_2_3_IntFunction(int irq, void *id,struct pt_regs * regs)
@introduction:
    注册至 Linux 内核的定时器中断服务函数

@parameter:
    (详参 Linux 内核说明文档)

@return:
    (详参 Linux 内核说明文档)


*/
static irqreturn_t sfTIME_2_3_IntFunction(int irq, void *id,
        struct pt_regs *regs)
{
	if (DF_IsTimerInt(2)) {
		if (spfTimer2ServerFunction != NULL) {
			spfTimer2ServerFunction();
		}
		DF_ClrIntFlag(2);
		return IRQ_HANDLED;
	}
	if (DF_IsTimerInt(3)) {
		if (spfTimer3ServerFunction != NULL) {
			spfTimer3ServerFunction();
		}
		DF_ClrIntFlag(3);
		return IRQ_HANDLED;
	}
	return -1;
}
