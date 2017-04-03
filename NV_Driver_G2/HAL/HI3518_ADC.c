#include "HAL.h"
#include "HI3518_HAL.h"
#include "../Tool/String.h"

////////////////////////////////////////////////////////////////////////////////
#define DC_HAL_ADC_DefSensiBit 	10
#define DC_HAL_ADC_DefSensi		1024
#define DC_HAL_ADC_DefVol		3300
#define DC_HAL_ADC_Channel		2		

// ----------> ADC module CLK 
#define DF_HAL_ADC_StartCLK		\
			HAL_writel( HAL_readl(HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))|0x02\
			, HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))

#define DF_HAL_ADC_StopCLK		\
			HAL_writel( HAL_readl(HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))&(~0x02)\
			, HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))

#define DF_HAL_ADC_Reset	\
			HAL_writel( HAL_readl(HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))|0x01\
			, HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))
#define DF_HAL_ADC_Set		\
			HAL_writel( HAL_readl(HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))&(~0x01)\
			, HAL_CRGx_Addr(HAL_OFST_CRG32_RsTADC))


#define DF_HAL_ADC_PowerUp	\
			HAL_writel( 0x00, HAL_ADC_REGAddr(HAL_OFST_ADC_POWERDOWN))
			
#define DF_HAL_ADC_PowerDown	\
			HAL_writel( 0x01, HAL_ADC_REGAddr(HAL_OFST_ADC_POWERDOWN))
			
			
// ----------> ADC module Status
#define DF_HAL_ADC_IsBusy	\
			( (HAL_readl(HAL_ADC_REGAddr(HAL_OFST_ADC_STATUS))&0x01)?1:0 )

#define DF_HAL_ADC_ClrBusy	\
			HAL_writel(0x01,HAL_ADC_REGAddr(HAL_OFST_ADC_STATUS))
			
// ----------> ADC module SetChanel and start
#define DF_HAL_ADC_StartTransform(_channel) \
			HAL_writel( ((_channel)<<16)+1 , HAL_ADC_REGAddr(HAL_OFST_ADC_CTRL))

#define DF_HAL_ADC_WhischChannel \
			( (HAL_readl(HAL_ADC_REGAddr(HAL_OFST_ADC_CTRL))&0x10000)?1:0)

	
// ----------> ADC module SetChanel and start
#define DF_HAL_ADC_EnInt \
			HAL_writel( 0x00 , HAL_ADC_REGAddr(HAL_OFST_ADC_INT_MASK))
			
#define DF_HAL_ADC_DisInt \
			HAL_writel( 0x01 , HAL_ADC_REGAddr(HAL_OFST_ADC_INT_MASK))
			
// ----------> ADC module Clear interrupt 
#define DF_HAL_ADC_ClearInt \
			HAL_writel( 0x01 , HAL_ADC_REGAddr(HAL_OFST_ADC_INT_CLR))

// ----------> ADC module get transform value
#define DF_HAL_ADC_GetResult \
			(HAL_readl(HAL_ADC_REGAddr(HAL_OFST_ADC_RESULT))&0x3FF)

static irqreturn_t ADC_int_Handler(int irq, void *id);


typedef struct{
#define DC_HAL_ADC_Busy			          0x80
#define DC_HAL_ADC_Blocked                0x10
#define DC_HAL_ADC_BLockMask	          0x0F
#define DC_HAL_ADC_BlockIncUnit	          0x01
	uint32		aStatus;
	uint16 		aCh0_Result;
	uint16 		aCh1_Result;
	void 		(*afCh0_CallBack)(uint16);
	void 		(*afCh1_CallBack)(uint16);
}mADCopt_Info;

static mAD_Ifo		soAD_Ifo={
	DC_HAL_ADC_DefVol,
	DC_HAL_ADC_DefSensi,
	DC_HAL_ADC_Channel
};

static mADCopt_Info soADCResoult;
#define DF_HAL_ADC_IncBlock	              { soADCResoult.aStatus+=DC_HAL_ADC_BlockIncUnit;}
#define DF_HAL_ADC_IsBlock                ((soADCResoult.aStatus&DC_HAL_ADC_Blocked)?1:0)
#define DF_HAL_ADC_ClrBlock \
						{soADCResoult.aStatus&=~(DC_HAL_ADC_Blocked+DC_HAL_ADC_BLockMask);}
//


static void sfADC_Reset(void){
	soADCResoult.aStatus = 0;
	DF_HAL_ADC_Reset;
	udelay(1);
	DF_HAL_ADC_Set;
	DF_HAL_ADC_PowerUp;
	DF_HAL_ADC_EnInt;
}

static int32 sfADC_Init(void){
	
	gClassStr.afMemset( (void*)&soADCResoult, 0x00, sizeof(mADCopt_Info) );
	DF_HAL_ADC_StartCLK;
	sfADC_Reset();
	if(request_irq(IRQ_SAR_ADC, ADC_int_Handler, IRQF_SHARED, "NVC_DRV_HAL_ADC",&soAD_Ifo)){
#if OPEN_DEBUG
	NVCPrint("Register IRQ_SAR_ADC IntRequest Fail!");
#endif
	}
#if OPEN_DEBUG
	NVCPrint("Register IRQ_SAR_ADC IntRequest Success!");
#endif
	return 0;
}

static int32 sfADC_Uninit(void){
	DF_HAL_ADC_PowerDown;
	DF_HAL_ADC_Reset;
	free_irq( IRQ_SAR_ADC, &soAD_Ifo);
	return 0;
}

static mAD_Ifo sfGetInfo(void){
	return soAD_Ifo;
}

static int32 gfADC_StartConvert(uint32 iCmd)
{
	uint8 _i;
	
	_i = 70;
	while( (_i>0) &&(soADCResoult.aStatus&DC_HAL_ADC_Busy) ){
		_i--;
		udelay(3);
	}
	if( _i == 0 ){
		DF_HAL_ADC_IncBlock
		if(DF_HAL_ADC_IsBlock){
			sfADC_Reset();
		}else{
			return DC_HAL_ADCRet_Busy;
		}
	}else{
		DF_HAL_ADC_ClrBlock;
	}
	
	if( iCmd&DC_HAL_ADCOpt_StartCov ){
		DF_HAL_ADC_StartTransform( iCmd&DC_HAL_ADCOpt_ChAreaMask );
		soADCResoult.aStatus |= DC_HAL_ADC_Busy;
	}
	return DC_HAL_ADCRet_Success;
}

static int32 sfGetChValue(uint32 iCmd){
	int32 tRet = 0;
	
	if( soADCResoult.aStatus&DC_HAL_ADC_Busy )
		tRet |= DC_HAL_ADCRet_Busy;
	
	switch( iCmd&DC_HAL_ADCOpt_ChAreaMask ){
		case 0:
			tRet |= soADCResoult.aCh0_Result;
		break;
		case 1:
			tRet |= soADCResoult.aCh1_Result;
		break;
		default:break;
	}
	return tRet;
}

static void sfRegCallback( \
					uint32 iCmd\
					,void (*ifCallbackHandle)(uint16) ){
	if( (iCmd&DC_HAL_ADCOpt_ChAreaMask) == 0 ){
		soADCResoult.afCh0_CallBack = ifCallbackHandle;
	}else if( (iCmd&DC_HAL_ADCOpt_ChAreaMask) == 1 ){
		soADCResoult.afCh1_CallBack = ifCallbackHandle;
	}
}








mClass_AD const gcoClass_AD = {
	.prfInit 		= sfADC_Init,
	.prfUninit 		= sfADC_Uninit,
	.prfGetInfo 	= sfGetInfo,
	.prfSetOpt 		= gfADC_StartConvert,
	.prfGetValue 	= sfGetChValue,
	.prfRegCallBack	= sfRegCallback,
};










static irqreturn_t ADC_int_Handler(int irq, void *id){
	
	if( DF_HAL_ADC_WhischChannel ){
		soADCResoult.aCh1_Result = DF_HAL_ADC_GetResult;
		if( soADCResoult.afCh1_CallBack != NULL )
			soADCResoult.afCh1_CallBack( soADCResoult.aCh1_Result );
	}else{
		soADCResoult.aCh0_Result = DF_HAL_ADC_GetResult;
		if( soADCResoult.afCh0_CallBack != NULL )
			soADCResoult.afCh0_CallBack( soADCResoult.aCh0_Result );
	}
	
	DF_HAL_ADC_ClearInt;
	DF_HAL_ADC_ClrBusy;
	soADCResoult.aStatus &= ~DC_HAL_ADC_Busy;
	return 0;
}
