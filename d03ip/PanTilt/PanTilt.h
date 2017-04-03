#ifndef __PANTILT_H
#define __PANTILT_H
#include "GlobalParameter.h"
////////////////////////////////////////////////////////////////////////////////
/*
Vertical
V0	GPIO6_0
V1	GPIO6_1
V2	GPIO1_1
V3	GPIO6_3

Horizontal
H0	GPIO6_4
H1	GPIO6_5
H2	GPIO6_6
H3	GPIO6_7

步距：5.625
驱动方式；四相八拍
减速比：64
八拍移动：0.3515625
移动360度：1024个八拍
*/
#define DC_Time0_UnitFrq		100//Hz
//#define DC_Time_UnitFrq		1000//Hz
//#define DC_VSM_MaxStartFrq	500	//Hz
//#define DC_VSM_MaxWorkFrq	800	//Hz
//#define DC_HSM_MaxStartFrq	500	//Hz
//#define DC_HSM_MaxWorkFrq	800	//Hz

#define DC_LIMIT_HORG	0
#define DC_LIMIT_HTML	665
#define DC_LIMIT_VORG	0
#define DC_LIMIT_VTML	73//?

#define PanTilt_NAME 	"Pan_Tilt"

////////////////////////////////////////////////////////////////////////////////
// PanTilt GPIO macro
#define DC_HSM_G0	6
#define DC_HSM_G1	6
#define DC_HSM_G2	1
#define DC_HSM_G3	6

#define DC_HSM_B0	0
#define DC_HSM_B1	1
#define DC_HSM_B2	1
#define DC_HSM_B3	3

#define DC_VSM_G0	6
#define DC_VSM_G1	6
#define DC_VSM_G2	6
#define DC_VSM_G3	6

#define DC_VSM_B0	4
#define DC_VSM_B1	5
#define DC_VSM_B2	6
#define DC_VSM_B3	7

#define DC_HSM0_MUX	36
#define DC_HSM1_MUX	37
#define DC_HSM2_MUX	38
#define DC_HSM3_MUX	39

#define DC_VSM0_MUX	32
#define DC_VSM1_MUX	33
#define DC_VSM2_MUX	1
#define DC_VSM3_MUX	35

//
//	Position Limit Horizontal origin 				GPIO2_3	9
//	Position Limit Horizontal termination/ending 	GPIO2_5 11	
//	Position Limit Vertical origin 					GPIO5_2	47
//	Position Limit Vertical Termination/ending 		GPIO2_2	8
//
#define DC_HSMLim_ORG_G	2
#define DC_HSMLim_TML_G	2
#define DC_HSMLim_ORG_B	3
#define DC_HSMLim_TML_B	5

#define DC_VSMLim_ORG_G	5
#define DC_VSMLim_TML_G	2
#define DC_VSMLim_ORG_B	2
#define DC_VSMLim_TML_B	2


#define DC_HSMLim_ORG_MUX	9
#define DC_HSMLim_TML_MUX	11

#define DC_VSMLim_ORG_MUX	47
#define DC_VSMLim_TML_MUX	8


	
#define DF_Set_SM_DIRx(_g,_b)	{\
								uint32 _v;\
								_v= HAL_readl(HAL_GPIOx_DIR(_g));\
								_v|=HAL_GPIO_Bit(_b);\
								HAL_writel(_v,HAL_GPIOx_DIR(_g));}
#define DF_Clr_SM_DIRx(_g,_b)	{\
								uint32 _v;\
								_v= HAL_readl(HAL_GPIOx_DIR(_g));\
								_v&=~HAL_GPIO_Bit(_b);\
								HAL_writel(_v,HAL_GPIOx_DIR(_g));}

#define DF_Set_SM_MUXx(_v,_Mux)	HAL_writel(_v,HAL_MUXCTRL_regx(_Mux))

#define DF_Set_SM_Outx(_g,_b)	{\
								uint32 _v;\
								_v= HAL_readl(HAL_GPIOx_SDATA(_g,_b));\
								_v|=HAL_GPIO_Bit(_b);\
								HAL_writel(_v,HAL_GPIOx_SDATA(_g,_b));}

#define DF_Clr_SM_Outx(_g,_b)	{\
								uint32 _v;\
								_v= HAL_readl(HAL_GPIOx_SDATA(_g,_b));\
								_v&=~HAL_GPIO_Bit(_b);\
								HAL_writel(_v,HAL_GPIOx_SDATA(_g,_b));}

#define DF_Get_SM_INx(_g,_b)	((HAL_readl(HAL_GPIOx_SDATA(_g,_b))&HAL_GPIO_Bit(_b))?1:0)

////////////////////////////////////////////////////////////////////////////////
//status
// static uint32 gSMStatus;
#define DC_SMS_HRhythmArea	0x00000007
#define DC_SMS_HRhythmUnit	0x00000001

#define DC_SMS_HPeriodDone	0x00000008
#define DC_SMS_HRunning		0x00000010
#define DC_SMS_HAntiRoll	0x00000020			// Anticlockwise
#define DC_SMS_HReducing	0x00000040
#define DC_SMS_HIniting		0x00000080			// The Horizontal step motor has initializing
#define DC_SMS_HBanSum		0x00000100
#define DC_SMS_HRspEnding	0x00000200



#define DC_SMS_VRhythmArea	0x00070000
#define DC_SMS_VRhythmUnit	0x00010000

#define DC_SMS_VPeriodDone	0x00080000
#define DC_SMS_VRunning		0x00100000
#define DC_SMS_VAntiRoll	0x00200000			//anticlockwise
#define DC_SMS_VReducing	0x00400000
#define DC_SMS_VIniting		0x00800000			// The Vertical step motor has initializin
#define DC_SMS_VBanSum		0x01000000
#define DC_SMS_VRspEnding	0x02000000

#define DF_SMS_GetHRhythm			(gSMStatus&DC_SMS_HRhythmArea)
#define DF_SMS_GetVRhythm			((gSMStatus&DC_SMS_VRhythmArea)>>16)
#define DF_SMS_HaddRhythm			{gSMStatus+=DC_SMS_HRhythmUnit;}	
#define DF_SMS_VaddRhythm			{gSMStatus+=DC_SMS_VRhythmUnit;}

#define DF_SMS_IsStatusExist(_x)	((gSMStatus&(_x))?1:0)
#define DF_SMS_SetStatus(_x)		{gSMStatus|=(_x);}
#define DF_SMS_ClrStatus(_x)		{gSMStatus&=~(_x);}


//	struct timer_list ADC_Time;    //Read ADC valut and output at regular time
typedef struct{
	unsigned int 	irq_num;
	unsigned long 	irq_mode;
	const char  	*irq_name;
}mSMD_Struct;

typedef struct{
	int16	aXRod;
	int16	aYRod;
}mCoodinate;

extern void gfInitSMHIVPort(void);
extern void gfUninitSMHIVPort(void);
extern int gfPanTiltIsBusy(void);
extern int gfStartSMMove(uint16 iHSteps,uint16 iVSteps,uint8 iCmd);
#define DC_SSMM_HSet			0x01
#define DC_SSMM_VSet			0x02
#define DC_SSMM_HDgr			0x04
#define DC_SSMM_VDgr			0x08
#define DC_SSMM_HAntiClockWise	0x10
#define DC_SSMM_VAntiClockWise	0x20

#define DC_SSMM_B_Success			0
#define DC_SSMM_BW_HAdd				1
#define DC_SSMM_BW_VAdd				2
#define DC_SSMM_BW_BothAdd			3
#define DC_SSMM_BE_HSetButNoData	-1
#define DC_SSMM_BE_VSetButNoData	-2
#define DC_SSMM_BE_HOverRange		-3
#define DC_SSMM_BE_VOverRange		-4

#endif







