/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    默认:
    	在没有设定温度上报的前提下，每隔5s更新一次温度数据，
    	在有应用层有要求的情况下，则按照应用层的设定来决定

    	D11机型采集温度主要用于控制风扇，因此在初始化消息管理中心的时候
    	不需要挂在此模块，但是注意要在初始化的时候初始化该模块，
    	设置好温度采样频率，供以调节灯泡温度时候及时反馈

*/
//==============================================================================
// C
// Linux
// local
#include "CTA.h"
// remote
#include "../HAL/HAL.h"
#include "../Tool/NTC.h"
#include "../GlobalParameter.h"

//==============================================================================
//extern
//local
static int32 sfdefTempMonitor_Init(void);
static int32 sfdefTempMonitor_Uninit(void);
static void sfdefTempMonitor_SetReportTime(uint32);
static int32 sfdefTempMonitor_GetTemperature(void);
static int32 sfdefTempMonitor_RegCallBack(void (*CallbackHandle)(int32));
static void sfTempMonitor_CallbackFromADC(uint16 iValue);
static void sfTempMonitor_LoopCheck(void);
//global

//==============================================================================
//extern
//local
static int32 sTempBuf;
static void (*sdefTempMonitor_CallBack)(int32) = NULL;
//global
mClass_Temp gClassTempMonitor = {
	.afInit = &sfdefTempMonitor_Init,
	.afUninit = sfdefTempMonitor_Uninit,
	.afSetReportTime = sfdefTempMonitor_SetReportTime,
	.afGetTemperature = sfdefTempMonitor_GetTemperature,
	.afRegCallBack = sfdefTempMonitor_RegCallBack,
};

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static	int32	sfdefTempMonitor_Init(void)
@introduction:
    初始化温度检测模块

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefTempMonitor_Init(void)
{
	gClassHAL.ADC->prfRegCallBack(DC_HAL_ADChannel(1),
	                              sfTempMonitor_CallbackFromADC);
	gClassHAL.ADC->prfSetOpt(DC_HAL_ADChannel(1) | DC_HAL_ADCOpt_StartCov);

	return 0;
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefTempMonitor_Uninit(void)
@introduction:
    释放资源

@parameter:
    void

@return:
    0   SUCCESS

*/
static int32 sfdefTempMonitor_Uninit(void)
{
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
	                                sfTempMonitor_LoopCheck);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static	void	sfdefTempMonitor_SetReportTime(uint32	iTime)
@introduction:
    设置自动上报时间周期

@parameter:
    iTime
        单位为 S

@return:
    void

*/
static void sfdefTempMonitor_SetReportTime(uint32 iTime)
{
	if (iTime == 0) {
		gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,
		                                sfTempMonitor_LoopCheck);
	} else {
		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,
		                                  sfTempMonitor_LoopCheck,
		                                  iTime * 10);
	}
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefTempMonitor_GetTemperature(void)
@introduction:
    获取温度数据

@parameter:
    void

@return:
    返回温度值(精度：0.1  约定 0.1度/1)

*/
static int32 sfdefTempMonitor_GetTemperature(void)
{
	return sTempBuf;
}

//---------- ---------- ---------- ----------
/*  static	int32	sfdefTempMonitor_RegCallBack(void (*CallbackHandle)(int32))
@introduction:
    注册温度检测回调函数

@parameter:
    CallbackHandle
        回调函数指针

@return:
    0   SUCCESS

*/
static int32 sfdefTempMonitor_RegCallBack(void (*CallbackHandle)(int32))
{
	sdefTempMonitor_CallBack = CallbackHandle;
	return 0;
}

//---------- ---------- ---------- ----------
/*  asjkdaskdjasdk
@introduction:
    这个函数则是用来上报消息的 或者说是用来周期性查询温度值做用的
    这个函数在 D11 时，应该初始化的时候就注册好（D11并没有应用层来影响）
    而在 D03 中则是客户端需要温度数据，所以在 D03 初始化的时候记得把回调函数关联好

@parameter:
    void

@return:
    void

*/
static void sfTempMonitor_LoopCheck(void)
{
	gClassHAL.ADC->prfSetOpt(DC_HAL_ADChannel(1) | DC_HAL_ADCOpt_StartCov);
	if (sdefTempMonitor_CallBack != NULL) {
		sdefTempMonitor_CallBack(sTempBuf);
	}
}

//---------- ---------- ---------- ----------
/*  asjkdaskdjasdk
@introduction:
    这个函数主要用作接收ADC转换完成时的回调函数
    在D11中，温度数据以采集电压值进行转换的方式获得
    而在每次启动ADC采集一次数据后，在ADC模块中，采集的值不会立即返回，大约需要20us或者更久的样子
    所以一这一次启动后获得的数据可能要下次才能获取得到
    或者用 ADC 模块中强制获得数据的方式，等待数据采集完成（也就是忙等待），
    虽然这种方式能获得及时的数据，但明显浪费了CPU的很多资源
    所以在这里我们采用启动转换之后，等到ADC的中断到来，然后一起计算出温度值
    注意要在模块初始化的时候将此模块注册到 ADC 模块的回调函数中

@parameter:
    iValue
        AD 值

@return:
    void

*/
static void sfTempMonitor_CallbackFromADC(uint16 iValue)
{
	sTempBuf = (int32) ConvertTempFromVoltage(iValue);
}
