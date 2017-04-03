#include "HAL_PWM.h"

#include "USERHAL.h"
#include "../ProInfo.h"

#include <linux/delay.h>

#define DC_PWM_CLK_FRQ			3000000
#define DC_PWM_FRQ				10000
#define DC_PWM_MaxWidth			300


//PWM0
#define DC_PWM0_ENABLE			1
#define DC_PWM0_PIN_G			5
#define DC_PWM0_PIN_B			2
#define DC_PWM0_MUX				47
#define DC_PWM0_MUXVal			0x01
//PWM1
#define DC_PWM1_ENABLE			1
#define DC_PWM1_PIN_G			5
#define DC_PWM1_PIN_B			3
#define DC_PWM1_MUX				48
#define DC_PWM1_MUXVal			0x01
//PWM2 Disable
// #define DC_PWM2_PIN_G			5
// #define DC_PWM2_PIN_B			2
// #define DC_PWM2_MUX				78
// #define DC_PWM2_MUXVal			0x00


#define DC_PWM_CLK_SOURCE 		HAL_CRGx_Addr(HAL_OFST_CRG14_RsTPWM)
#define DC_PWM_CLK_Enable		0x02
#define DC_PWM_MOD_Reset		0x01

#define DF_PWM_CLK_ENABLE		HAL_writel(HAL_readl(DC_PWM_CLK_SOURCE)|DC_PWM_CLK_Enable,DC_PWM_CLK_SOURCE)
#define DF_PWM_CLK_DISABLE		HAL_writel(HAL_readl(DC_PWM_CLK_SOURCE)&(~DC_PWM_CLK_Enable),DC_PWM_CLK_SOURCE)

#define DF_PWM_MOD_RESET		HAL_writel(HAL_readl(DC_PWM_CLK_SOURCE)|DC_PWM_MOD_Reset,DC_PWM_CLK_SOURCE)
#define DF_PWM_MOD_SET			HAL_writel(HAL_readl(DC_PWM_CLK_SOURCE)&(~DC_PWM_MOD_Reset),DC_PWM_CLK_SOURCE)

#define DF_PWMx_SET_Period(_g,_v)		HAL_writel(_v,HAL_OFST_PWMx_CFG0(_g))
#define DF_PWMx_SET_Width(_g,_v)		HAL_writel(_v,HAL_OFST_PWMx_CFG1(_g))
#define DF_PWMx_SET_Number(_g,_v)		HAL_writel(_v,HAL_OFST_PWMx_CFG2(_g))
#define DF_PWMx_SET_Config(_g,_v)		HAL_writel(_v,HAL_OFST_PWMx_CTRL(_g))
#define DF_PWMx_GET_IDLE(_g)			((HAL_readl(HAL_OFST_PWMx_STATE2(_g))&0x400)?0:1)
#define DC_PWMx_CFG_STA_Enable			0x01
#define DC_PWMx_CFG_STA_inv				0x02
#define DC_PWMx_CFG_STA_Keep			0x04



int gfINIT_HAL_PWM(void){
	

	// Setting multi function
	DF_Set_GPIO_MUXx(DC_PWM0_MUXVal,DC_PWM0_MUX);
	// Setting IO dirction OUTPUT 
	DF_Set_GPIO_DIRx(DC_PWM0_PIN_G,DC_PWM0_PIN_B);

	// Setting multi function
	DF_Set_GPIO_MUXx(DC_PWM1_MUXVal,DC_PWM1_MUX);
	// Setting IO dirction OUTPUT
	DF_Set_GPIO_DIRx(DC_PWM1_PIN_G,DC_PWM1_PIN_B);


	DF_PWM_CLK_DISABLE;
	DF_PWM_MOD_RESET;
	udelay(1);
	DF_PWM_MOD_SET;
	DF_PWM_CLK_ENABLE;
	
	DF_PWMx_SET_Config(0,0);
	DF_PWMx_SET_Config(1,0);
	
	{
		uint8 _i;
		for(_i=0;_i<210;_i++){
			if(DF_PWMx_GET_IDLE(0)){
				break;
			}
			udelay(1);
		}
		if(_i==210)
			return -1;	//initial error
			
		for(_i=0;_i<210;_i++){
			if(DF_PWMx_GET_IDLE(1)){
				break;
			}
			udelay(1);
		}
		if(_i==210)
			return -1;	//initial error
		
		DF_PWMx_SET_Period(0,DC_PWM_MaxWidth);
		DF_PWMx_SET_Period(1,DC_PWM_MaxWidth);
		DF_PWMx_SET_Width(0,0);
		DF_PWMx_SET_Width(1,0);
	}
	return 0;
}


int gfUNINIT_HAL_PWM(void){
	
	return 0;
}
	
	
int gfPWM_ModeSetWidth(uint8 iUnit,uint16 iWidth){
	
	uint8 _i;
	
	// if(iUnit>1)
		// return -1;
	if(iWidth>DC_PWM_MaxWidth)
		return -1;
	
	DF_PWMx_SET_Config(iUnit,0);
	for(_i=0;_i<210;_i++){
		if(DF_PWMx_GET_IDLE(iUnit)){
			break;
		}
		udelay(1);
	}
	if(_i==210)
		return -1;	//initial error
	
	DF_PWMx_SET_Width(iUnit,iWidth);
	
	DF_PWMx_SET_Config(iUnit,DC_PWMx_CFG_STA_Enable|DC_PWMx_CFG_STA_Keep);
	
	return 0;
}





