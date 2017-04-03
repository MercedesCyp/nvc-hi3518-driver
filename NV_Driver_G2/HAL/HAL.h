#ifndef __HAL_H
#define __HAL_H
#include "../GlobalParameter.h"

#include <asm/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>







////////////////////////////////////////////////////////////////////////////////
typedef struct{                                                               //
	uint8 	aGroup;                                                           //
	uint8 	aBit;                                                             //
	uint8 	aMux;                                                             //
	uint32	aMuxVal;                                                          //
}mGPIOPinIfo;                                                                 //
                                                                              //
                                                                              //
typedef struct{                                                               //
	mGPIOPinIfo	  *apPin;                                                     //
	uint8 			aNum;                                                     //
}mGPIOPinArr;                                                                 //
                                                                              //
                                                                              //
typedef struct{                                                               //
#define DC_HAL_PIN_SetIN	0x01                                              //
#define DC_HAL_PIN_SetOUT	0x02                                              //
	void 	(*prfSetDirection)( void *ipPinInfo , uint8 iInOut );             //
#define DC_HAL_PIN_SetLOW	0x01                                              //
#define DC_HAL_PIN_SetHIGH	0x02                                              //
#define DC_HAL_PIN_SetSWITCH 0x04
	void 	(*prfSetExport)( void *ipPinInfo, uint8 iHighLow);                //
#define DC_HAL_PIN_GETLOW	0x00                                              //
#define DC_HAL_PIN_GETHIGH	0x01                                              //
	uint32 	(*prfGetPinValue)(void *ipPinInfo);                               //
#define DC_HAL_PIN_Grp0     0x00
#define DC_HAL_PIN_Grp1     0x01
#define DC_HAL_PIN_Grp2     0x02
#define DC_HAL_PIN_GrpErr   0xFF  
    uint32 (*prfWhichGrp)(void *ipPinInfo);
#define DC_HAL_PIN_RetSuccess	0x00                                          //
#define DC_HAL_PIN_RetIntErr	0x01                                          //
	uint32 (*prfPin_RegistInt)( uint32 iGrp, void (*iHandler)(void) );        //
    void   (*prfClrConfig)(void);                                             //
#define DC_HAL_PIN_IntHighLevel	0x01                                          //
#define DC_HAL_PIN_IntLowLevel	0x02                                          //
#define DC_HAL_PIN_IntRiseEdge	0x04                                          //
#define DC_HAL_PIN_IntFallEdge  0x08                                          //
#define DC_HAL_PIN_IntIN        0x80
#define DC_HAL_PIN_IntClrState  0x40
#define DC_HAL_PIN_IntEnable    0x20
    void   (*prfPin_ConfigInt)( void *ipPinInfo, uint8 iIntType );            //
    uint32 (*prfIsIntStateExist)(void *ipPinInfo);
    
}mClass_Pin;                                                                  //
////////////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////
typedef struct{                                                               //
	uint32 	aMainFreq;                                                        //
	uint32	aDuty;                                                            //
	uint32	aWidth;                                                           //
	uint16	aCycleNum;                                                        //
	uint8 	aNum;                                                             //
	union{                                                                    //
	uint8	aCmd;                                                             //
	uint8 	aStatus;                                                          //
#define DC_PWM_CmdKeep	0x01                                                  //
#define DC_PWM_Cmdinv	0x02                                                  //
#define DC_PWM_StaBusy	0x04                                                  //
#define DC_PWM_StaKeep	0x08                                                  //
#define DC_PWM_Err		0x80                                                  //
	};                                                                        //
}mPWM_Info;                                                                   //
//                                                                            //
typedef struct{                                                               //
	mPWM_Info	*pInfo;                                                       //
	void		(*prfGetInfo)(void);                          		 		  //
	void 		(*prfStart)(void);                            		 		  //
	void 		(*prfStop)(uint8 iWhichOne);                                  //
}mClass_PWM;                                                                  //
////////////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////
typedef struct{                                                               //
	uint32	aRefVol;                                                          //
	uint32	aSensitive;                                                       //
	uint8	aHowManyChannel;                                                  //
#define DC_HAL_ADChannel(_n)	(_n)                                          //
}mAD_Ifo;                                                                     //
//
#define DC_HAL_ADCOpt_StartCov		0x00000080
#define DC_HAL_ADCOpt_ChAreaMask	0x0000000F
#define DC_HAL_ADCRet_Success		0x00000000
#define DC_HAL_ADCRet_Busy			0x80000000
#define DC_HAL_ADCOpt_ValMask		0x0000FFFF
typedef struct{                                                               //
	int32		(*prfInit)(void);                                             //
	int32		(*prfUninit)(void);                                           //
	mAD_Ifo 	(*prfGetInfo)(void);                                          //
	int32		(*prfSetOpt)(uint32);		                                  //
	int32		(*prfGetValue)(uint32);
	void		(*prfRegCallBack)(uint32,void (*ifCallbackHandle)(uint16));
}mClass_AD;                                                                   //
////////////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////////
#define DC_HAL_TIMER_NUM		2                                             //
#define DC_HAL_TIMER_MaxPeriod	10000                                         //
#define DC_HAL_TIMER_MinPeriod	1000                                          //
#define DC_HAL_TIMERChannel(_n)	(_n)    // 0 or 1                             //
typedef struct{                                                               //
	uint32 aCLK;                                                              //
	uint32 aSumNum;                                                           //
	uint32 aPeriod;                                                           //
	uint32 aCountNum;                                                         //
}mTIMER_ifo;                                                                  //
//	                                                                          //
typedef struct{                                                               //
	uint32	aTime;                                                            //
	void 	(*aHandler)(void);                                                //
	uint8 	aCmd;                                                             //
#define DC_HAL_TIMER_cmdLimMode		0x01                                      //
#define DC_HAL_TIMER_cmdPTMode		0x02                                      //
}mTIMER_param;                                                                //
                                                                              //
typedef struct{                                                               //
	int32 (*prfInit)(void);                                                   //
	int32 (*prfUninit)(void);                                                 //
	int32 (*prfConfig)(uint8 iChannel,mTIMER_param*);                         //
	int32 (*prfStart)(uint8 iChannel);                                        //
	int32 (*prfStop)(uint8 iChannel);                                         //
}mClass_Timer;                                                                //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#define DC_HAL_PE_UNINTPeriod	10	
#define DC_HAL_PE_INTPeriod		1	
#define DC_HAL_PE_UNINTCap		8	
#define DC_HAL_PE_INTCap		8
typedef struct{
	int32	(*afInit)(void);
	int32	(*afUninit)(void);
	// Param	peroid_event function_handle Period_Timer 
	// Period_event
#define DC_HAL_PE_INT 				0x01
#define DC_HAL_PE_UNINT				0x02
	int32	(*afRegister)(uint8,void (*)(void),uint32);
	int32	(*afCancel)(uint8,void (*)(void));
#define DC_HAL_PE_INT_PUSE			0x01
#define DC_HAL_PE_INT_RESUME		0x02
	int32	(*afControl)(uint8);
}mClass_PriodEvent;
////////////////////////////////////////////////////////////////////////////////




typedef struct{
	const	mClass_Pin			*Pin;
	const	mClass_PWM			*PWM;
	const	mClass_AD			*ADC;
	const	mClass_Timer		*Timer;
	const	mClass_PriodEvent	*PeriodEvent;
}mClass_Hal;

extern mClass_Hal const gClassHAL;

// gClassHAL.Pin->
// gClassHAL.PWM->
// gClassHAL.ADC->
// gClassHAL.Timer->
// gClassHAL.PeriodEvent->


#endif
