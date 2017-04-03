#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#include "./../GlobalParameter.h"


#define DF_TIMER_SetLoad(_v,_g)	\
		HAL_writel(_v,HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_LOAD)))


#define DF_TIMER_SetBLoad(_v,_g)	\
		HAL_writel(_v,HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_BGLOAD)))
		
#define DF_TIMER_GetLoad(_g)	\
		HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_LOAD)))
		
#define DF_TIMER_GetBLoad(_g)	\
		HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_BGLOAD)))
		
#define DF_TIMER_SetControl(_v,_g)	\
		HAL_writel(_v,HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_CONTROL)))
		
#define DF_TIMER_GetControl(_g)	\
		HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_CONTROL)))
		
		
#define DF_TIMER_SetINTCLR(_v,_g)	\
		HAL_writel(_v,HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_INTCLR)))
/*
//#define DF_TIMER_GetINTCLR(_g)	\
		{HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_INTCLR)));}
//#define DF_TIMER_SetRIS(_v,_g)	\
		{HAL_writel(_v,HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_RIS)));}
*/	
#define DF_TIMER_GetRIS(_g)	\
		HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_RIS)))
/*		
//#define DF_TIMER_SetMIS(_v,_g)	\
		{HAL_writel(_v,HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(HAL_OFST_TIME_MIS)));}
*/
#define DF_TIMER_GetMIS(_g)	\
		HAL_readl(HAL_TIMER_ADDR(_g,HAL_TIMER_OFST(_g,HAL_OFST_TIME_MIS)))
		
#define DC_TIMER_0	0
#define DC_TIMER_1	1
#define DC_TIMER_2	2
#define DC_TIMER_3	3


#define DF_Timer_Enable(_g)		{uint32 _v;\
								_v=DF_TIMER_GetControl(_g);\
								_v|=0x80;\
								DF_TIMER_SetControl(_v,(_g));}

#define DF_Timer_Disable(_g)	{uint32 _v;\
								_v = DF_TIMER_GetControl(_g);\
								_v &= ~0x80;\
								DF_TIMER_SetControl(_v,(_g));}



#define DF_Timer_ClrIntFlag(_g)	{DF_TIMER_SetINTCLR(0x01,(_g));}


		
/*
#define DF_Timer_SingleMode(_g)	{\
								uint32 _v;\
								_v = DF_TIMER_GetControl(_g);\
								_v &= ~0x40;\
								DF_TIMER_SetControl(_v,(_g));}
								
#define DF_Timer_PeriodMode(_g)	{\
								uint32 _v;\
								_v = DF_TIMER_GetControl(_g);\
								_v |= 0x40;\
								DF_TIMER_SetControl(_v,(_g));}
*/

#define DC_TimerCLK_0ov	0x00010000
#define DC_TimerCLK_01v 0x00040000
#define DC_TimerCLK_02v 0x00100000
#define DC_TimerCLK_03v 0x00400000




//--------------------------------------------------------user code
/*
#define DF_Timer0_Enable 			DF_Timer_Enable(0)
#define DF_Timer0_Disable 			DF_Timer_Disable(0)
#define DF_Timer0_ClrIntFlag 		DF_Timer_ClrIntFlag(0)
#define DF_Timer0_SetLoad(_v)		DF_TIMER_SetLoad(_v,0)
#define DF_Timer0CLK_SetSysCLK	{\
								uint32 _v;\
								_v = HAL_readl(HAL_SCCTRL_ADDR);\
								_v |= DC_TimerCLK_0ov;\
								HAL_writel(_v,(HAL_SCCTRL_ADDR));}
*/
#define DF_Timer3_Enable 			DF_Timer_Enable(3)
#define DF_Timer3_Disable 			DF_Timer_Disable(3)
#define DF_Timer3_IntFlag 			((DF_TIMER_GetRIS(3))?1:0)
#define DF_Timer3_ClrIntFlag 		DF_Timer_ClrIntFlag(3)
#define DF_Timer3_SetLoad(_v)		DF_TIMER_SetLoad(_v,3)
#define DF_Timer3_SetBLoad(_v)		DF_TIMER_SetBLoad(_v,3)
#define DF_Timer3CLK_Set3MCLK	{\
								uint32 _v;\
								_v = HAL_readl(HAL_SCCTRL_ADDR);\
								_v &= ~DC_TimerCLK_03v;\
								HAL_writel(_v,(HAL_SCCTRL_ADDR));}
#define DF_Timer3CLK_Clr3MCLK	{\
								uint32 _v;\
								_v = HAL_readl(HAL_SCCTRL_ADDR);\
								_v |= DC_TimerCLK_03v;\
								HAL_writel(_v,(HAL_SCCTRL_ADDR));}


#define DF_Timer2_Enable 			DF_Timer_Enable(2)
#define DF_Timer2_Disable 			DF_Timer_Disable(2)
#define DF_Timer2_IntFlag 			((DF_TIMER_GetRIS(2))?1:0)
#define DF_Timer2_ClrIntFlag 		DF_Timer_ClrIntFlag(2)
#define DF_Timer2_SetLoad(_v)		DF_TIMER_SetLoad(_v,2)
#define DF_Timer2_SetBLoad(_v)		DF_TIMER_SetBLoad(_v,2)
#define DF_Timer2CLK_Set3MCLK	{\
								uint32 _v;\
								_v = HAL_readl(HAL_SCCTRL_ADDR);\
								_v &= ~DC_TimerCLK_02v;\
								HAL_writel(_v,(HAL_SCCTRL_ADDR));}
#define DF_Timer2CLK_Clr3MCLK	{\
								uint32 _v;\
								_v = HAL_readl(HAL_SCCTRL_ADDR);\
								_v |= DC_TimerCLK_02v;\
								HAL_writel(_v,(HAL_SCCTRL_ADDR));}


//#define DC_Timer_SingleMode		
#define DC_Timer_OneShot		0x01
#define DC_Timer_Count_32bit	0x02
#define DC_Timer_Timerpre		0x0a
#define DC_Timer_Timerpre_1		0x00
#define DC_Timer_Timerpre_16	0x04
#define DC_Timer_Timerpre_256	0x08
#define DC_Timer_Intenable		0x20
#define DC_Timer_PeriodMode		0x40

extern void fgConfigTimer3(uint16 iFrq,uint8 iCmd);

extern void	gfINIT_HAL_Timer2(void);
extern void gfUNINIT_HAL_Timer2(void);

#endif