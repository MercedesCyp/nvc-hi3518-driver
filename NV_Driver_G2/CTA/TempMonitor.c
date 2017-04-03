// ================================================================================
//| Ĭ��:
//|		��û���趨�¶��ϱ���ǰ���£�ÿ��5s����һ���¶����ݣ�
//|		����Ӧ�ò���Ҫ�������£�����Ӧ�ò���趨������
//|		
//|		D11���Ͳɼ��¶���Ҫ���ڿ��Ʒ��ȣ�����ڳ�ʼ����Ϣ�������ĵ�ʱ��
//| 	����Ҫ���ڴ�ģ�飬����ע��Ҫ�ڳ�ʼ����ʱ���ʼ����ģ�飬
//| 	���ú��¶Ȳ���Ƶ�ʣ����Ե��ڵ����¶�ʱ��ʱ����
//| 	
//| 	
//| 	
//|	Ŀǰ���ã�
//| 	D11
//| 
//| 
//| 
//| 
//| 
//| 
// ================================================================================
#include "CTA.h"
#include "../HAL/HAL.h"
#include "../Tool/NTC.h"
#include "../GlobalParameter.h"









// --------------------------------------------------------------------------------
static	int32	sfdefTempMonitor_Init(void);
static	int32	sfdefTempMonitor_Uninit(void);
static	void	sfdefTempMonitor_SetReportTime(uint32);
static	int32	sfdefTempMonitor_GetTemperature(void);
static	int32	sfdefTempMonitor_RegCallBack(void (*CallbackHandle)(int32));
static	void	sfTempMonitor_CallbackFromADC(uint16 iValue);
static	void	sfTempMonitor_LoopCheck(void);









// --------------------------------------------------------------------------------
mClass_Temp gClassTempMonitor = {
	.afInit				=	&sfdefTempMonitor_Init,
	.afUninit			=	sfdefTempMonitor_Uninit,
	.afSetReportTime	=	sfdefTempMonitor_SetReportTime,
	.afGetTemperature	=	sfdefTempMonitor_GetTemperature,
	.afRegCallBack		=	sfdefTempMonitor_RegCallBack,
};

static int32 sTempBuf;
static void (*sdefTempMonitor_CallBack)(int32) = NULL;










// --------------------------------------------------------------------------------
// ---------------------------------------------------------------------->Local Function
// ------------------------------------------------------------>
static	int32	sfdefTempMonitor_Init(void){
	gClassHAL.ADC->prfRegCallBack( DC_HAL_ADChannel(1), sfTempMonitor_CallbackFromADC);
	gClassHAL.ADC->prfSetOpt( DC_HAL_ADChannel(1)|DC_HAL_ADCOpt_StartCov );
	
	return 0;
}

static	int32	sfdefTempMonitor_Uninit(void){
	gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,sfTempMonitor_LoopCheck);
	return 0;
}

static	void	sfdefTempMonitor_SetReportTime(uint32	iTime){// ��λΪ s
	if(iTime==0){
		gClassHAL.PeriodEvent->afCancel(DC_HAL_PE_UNINT,sfTempMonitor_LoopCheck);
	}else {
		gClassHAL.PeriodEvent->afRegister(DC_HAL_PE_UNINT,sfTempMonitor_LoopCheck, iTime*10);
	}
}



static	int32	sfdefTempMonitor_GetTemperature(void){
	return sTempBuf;
}


// ------------------------------------------------------------>sfdefTempMonitor_RegCallBack
static	int32	sfdefTempMonitor_RegCallBack(void (*CallbackHandle)(int32)){
	sdefTempMonitor_CallBack = CallbackHandle;
	return 0;
}

// ------------------------------------------------------------>sfTempMonitor_LoopCheck
// ����������������ϱ���Ϣ�� ����˵�����������Բ�ѯ�¶�ֵ���õ�
// ��������� D11 ʱ��Ӧ�ó�ʼ����ʱ���ע��ã�D11��û��Ӧ�ò���Ӱ�죩
// ���� D03 �����ǿͻ�����Ҫ�¶����ݣ������� D03 ��ʼ����ʱ��ǵðѻص�����������
static void sfTempMonitor_LoopCheck(void){
	gClassHAL.ADC->prfSetOpt( DC_HAL_ADChannel(1)|DC_HAL_ADCOpt_StartCov );
	if( sdefTempMonitor_CallBack != NULL )
		sdefTempMonitor_CallBack( sTempBuf );
}

// ------------------------------------------------------------>sfTempMonitor_CallbackFromADC
// ���������Ҫ��������ADCת�����ʱ�Ļص�����
// ��D11�У��¶������Բɼ���ѹֵ����ת���ķ�ʽ���
// ����ÿ������ADC�ɼ�һ�����ݺ���ADCģ���У��ɼ���ֵ�����������أ���Լ��Ҫ20us���߸��õ�����
// ����һ��һ���������õ����ݿ���Ҫ�´β��ܻ�ȡ�õ�
// ������ ADC ģ����ǿ�ƻ�����ݵķ�ʽ���ȴ����ݲɼ���ɣ�Ҳ����æ�ȴ�����
// ��Ȼ���ַ�ʽ�ܻ�ü�ʱ�����ݣ��������˷���CPU�ĺܶ���Դ
// �������������ǲ�������ת��֮�󣬵ȵ�ADC���жϵ�����Ȼ��һ�������¶�ֵ
// ע��Ҫ��ģ���ʼ����ʱ�򽫴�ģ��ע�ᵽ ADC ģ��Ļص�������
static void sfTempMonitor_CallbackFromADC(uint16 iValue){
	sTempBuf = (int32 )ConvertTempFromVoltage(iValue);
}

