#include "Button.h"
#include <linux/spinlock.h> //spinlock
#include <linux/interrupt.h>
#include <linux/slab.h>

#include "../ProInfo.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 0 1 2 11 Grp1
// 3 4 5 10 Grp2
// 6 7 8 9  Grp3
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro D01 Chip 3518c
#define DC_D01_3518c_BTNum		1
#define DC_D01_3518c_BTG1Num	1
#define DC_D01_3518c_BTG2Num	0
#define DC_D01_3518c_BTG3Num	0
// interrupt group 1 
#define DC_D01_3518c_B1_G		0
#define DC_D01_3518c_B1_B		1
#define DC_D01_3518c_B1_MUX		73
#define DC_D01_3518c_B1_MUXVal	0x00

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro D03 Chip 3518c 
#define DC_D03_3518c_BTNum		2
#define DC_D03_3518c_BTG1Num	1
#define DC_D03_3518c_BTG2Num	1
#define DC_D03_3518c_BTG3Num	0
// interrupt group 1 
#define DC_D03_3518c_B1_G		0
#define DC_D03_3518c_B1_B		1
#define DC_D03_3518c_B1_MUX		73
#define DC_D03_3518c_B1_MUXVal	0x00
// interrupt group 2
#define DC_D03_3518c_B2_G		5
#define DC_D03_3518c_B2_B		1
#define DC_D03_3518c_B2_MUX		46
#define DC_D03_3518c_B2_MUXVal	0x00

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro D04 Chip 3518c
#define DC_D04_3518c_BTNum		1
#define DC_D04_3518c_BTG1Num	1
#define DC_D04_3518c_BTG2Num	0
#define DC_D04_3518c_BTG3Num	0
// interrupt group 1
#define DC_D04_3518c_B1_G		0
#define DC_D04_3518c_B1_B		1
#define DC_D04_3518c_B1_MUX		73
#define DC_D04_3518c_B1_MUXVal	0x00

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro D11 Chip 3518c
#define DC_D11_3518c_BTNum		1
#define DC_D11_3518c_BTG1Num	1
#define DC_D11_3518c_BTG2Num	0
#define DC_D11_3518c_BTG3Num	0
// interrupt group 1
#define DC_D11_3518c_B1_G		0
#define DC_D11_3518c_B1_B		1
#define DC_D11_3518c_B1_MUX		73
#define DC_D11_3518c_B1_MUXVal	0x00


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro F07 Chip 3518e
// interrupt group 2
#define DC_F07_3518e_BTNum		1
#define DC_F07_3518e_BTG1Num	0
#define DC_F07_3518e_BTG2Num	1
#define DC_F07_3518e_BTG3Num	0

#define DC_F07_3518e_B1_G		5
#define DC_F07_3518e_B1_B		0
#define DC_F07_3518e_B1_MUX		45
#define DC_F07_3518e_B1_MUXVal	0x00
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro F05 Chip 3518e
// interrupt group 2
#define DC_F05_3518E_BTNum		1
#define DC_F05_3518E_BTG1Num	1
#define DC_F05_3518E_BTG2Num	0
#define DC_F05_3518E_BTG3Num	0
// interrupt group 1
#define DC_F05_3518E_B1_G		0
#define DC_F05_3518E_B1_B		1
#define DC_F05_3518E_B1_MUX		73
#define DC_F05_3518E_B1_MUXVal	0x00
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Pro F08 Chip 3518e
// interrupt group 2
#define DC_F08_3518E_BTNum		1
#define DC_F08_3518E_BTG1Num	1
#define DC_F08_3518E_BTG2Num	0
#define DC_F08_3518E_BTG3Num	0
// interrupt group 1
#define DC_F08_3518E_B1_G		0
#define DC_F08_3518E_B1_B		1
#define DC_F08_3518E_B1_MUX		73
#define DC_F08_3518E_B1_MUXVal	0x00

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


typedef struct{
	uint8		aNum;
	mGPIOPinMsg aPin;
}mButtonInfo;



static void sfInitButtonGPIO(mGPIOPinMsg *piButtonPin);
static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id);
static irqreturn_t intf_GPIO_Grp2Function(int irq, void *dev_id);
static irqreturn_t intf_GPIO_Grp3Function(int irq, void *dev_id);

// variable
#define DF_BT_InitStatus(_reg)			{(_reg)=DC_NSB_ButtonUp;}
#define DF_BT_IsStatusExist(_reg,_x)	DF_STATUS_IsStatusExist((_reg),(_x))
#define DF_BT_SetStatus(_reg,_x)		DF_STATUS_SetStatus((_reg),(_x))
#define DF_BT_ClrStatus(_reg,_x)		DF_STATUS_ClrStatus((_reg),(_x))

static spinlock_t   	*sButton_SpinLock;
static uint8			sBTSumNum;
static uint8			*sStateGrp = NULL ;
static uint8			sBTGrpNUM[3];
static mButtonInfo 		*sBT_Grp1 = NULL;
static mButtonInfo 		*sBT_Grp2 = NULL;
static mButtonInfo 		*sBT_Grp3 = NULL;


//mGPIOPinMsg *sButton_Pin;
int32 gfInitDrv_ButtonMonitor(uint8 iRemain){
	
	uint16 tChip = DF_GLOBAL_GetChipInfo;
	uint16 tPro = DF_GLOBAL_GetDivInfo;
	
	sButton_SpinLock = (spinlock_t*)kmalloc(sizeof(spinlock_t),GFP_ATOMIC);
	
	spin_lock_init(sButton_SpinLock);
	gfMemset(sBTGrpNUM,0,3);
	
	
	switch(tChip){
	case DC_CHIP_3518C:// 3518C =================================================================
// ==============================================================================================
	switch(tPro){
		case DC_Pro_D01:
		case DC_Pro_D04:
		case DC_Pro_D11:{
			sBTSumNum = DC_D01_3518c_BTNum;
			sBTGrpNUM[0] = DC_D01_3518c_BTG1Num;
			sBTGrpNUM[1] = DC_D01_3518c_BTG2Num;
			// sBTGrpNUM[2] = DC_D01_3518c_BTG3Num;
			
			sStateGrp = (uint8*)kmalloc(sBTSumNum,GFP_ATOMIC);
			
			DF_BT_InitStatus(sStateGrp[0]);
			
			sBT_Grp1 = (mButtonInfo*)kmalloc(sizeof(mButtonInfo)*DC_D01_3518c_BTG1Num,GFP_ATOMIC);
			sBT_Grp1[0].aNum	= 0;
			sBT_Grp1[0].aPin.aGroup		=DC_D01_3518c_B1_G;
			sBT_Grp1[0].aPin.aBit		=DC_D01_3518c_B1_B;
			sBT_Grp1[0].aPin.aMux		=DC_D01_3518c_B1_MUX;
			sBT_Grp1[0].aPin.aMuxVal	=DC_D01_3518c_B1_MUXVal;
			
		}break;
		case DC_Pro_D03:{
			sBTSumNum = DC_D03_3518c_BTNum;
			sBTGrpNUM[0] = DC_D03_3518c_BTG1Num;
			sBTGrpNUM[1] = DC_D03_3518c_BTG2Num;
			// sBTGrpNUM[2] = DC_D03_3518c_BTG3Num;
			
			sStateGrp = (uint8*)kmalloc(sBTSumNum,GFP_ATOMIC);
			//gfMemset(sStateGrp,0,DC_D03_3518c_BTNum);
			DF_BT_InitStatus(sStateGrp[0]);
			DF_BT_InitStatus(sStateGrp[1]);
			
			sBT_Grp1 = (mButtonInfo*)kmalloc(sizeof(mButtonInfo)*DC_D03_3518c_BTG1Num,GFP_ATOMIC);
			sBT_Grp1[0].aNum	= 0;
			sBT_Grp1[0].aPin.aGroup		=DC_D03_3518c_B1_G;
			sBT_Grp1[0].aPin.aBit		=DC_D03_3518c_B1_B;
			sBT_Grp1[0].aPin.aMux		=DC_D03_3518c_B1_MUX;
			sBT_Grp1[0].aPin.aMuxVal	=DC_D03_3518c_B1_MUXVal;
			
			
			sBT_Grp2 = (mButtonInfo*)kmalloc(sizeof(mButtonInfo)*DC_D03_3518c_BTG2Num,GFP_ATOMIC);
			sBT_Grp2[0].aNum	= 1;
			sBT_Grp2[0].aPin.aGroup		=DC_D03_3518c_B2_G;
			sBT_Grp2[0].aPin.aBit		=DC_D03_3518c_B2_B;
			sBT_Grp2[0].aPin.aMux		=DC_D03_3518c_B2_MUX;
			sBT_Grp2[0].aPin.aMuxVal	=DC_D03_3518c_B2_MUXVal;
			
		}break;		
		default:return -1;
	}break;	

	case DC_CHIP_3518E:// 3518E =================================================================
// ==============================================================================================
	switch(DF_GLOBAL_GetDivInfo){
		case DC_Pro_F07:{
			sBTSumNum = DC_F07_3518e_BTNum;
			// sBTGrpNUM[0] = DC_F07_3518e_BTG1Num;
			sBTGrpNUM[1] = DC_F07_3518e_BTG2Num;
			// sBTGrpNUM[2] = DC_F07_3518e_BTG3Num;
			
			sStateGrp = (uint8*)kmalloc(sBTSumNum,GFP_ATOMIC);
			DF_BT_InitStatus(sStateGrp[0]);
			
			sBT_Grp2 = (mButtonInfo*)kmalloc(sizeof(mButtonInfo)*DC_F07_3518e_BTG2Num,GFP_ATOMIC);
			sBT_Grp2[0].aNum	= 0;
			sBT_Grp2[0].aPin.aGroup		=DC_F07_3518e_B1_G;
			sBT_Grp2[0].aPin.aBit		=DC_F07_3518e_B1_B;
			sBT_Grp2[0].aPin.aMux		=DC_F07_3518e_B1_MUX;
			sBT_Grp2[0].aPin.aMuxVal	=DC_F07_3518e_B1_MUXVal;
		}break;
		case DC_Pro_F08:
		case DC_Pro_F05:{
			sBTSumNum = DC_F05_3518E_BTNum;
			sBTGrpNUM[0] = DC_F05_3518E_BTG1Num;
			// sBTGrpNUM[1] = DC_F05_3518E_BTG2Num;
			// sBTGrpNUM[2] = DC_F05_3518E_BTG3Num;
			
			sStateGrp = (uint8*)kmalloc(sBTSumNum,GFP_ATOMIC);
			DF_BT_InitStatus(sStateGrp[0]);
			
			sBT_Grp1 = (mButtonInfo*)kmalloc(sizeof(mButtonInfo)*DC_F05_3518E_BTG1Num,GFP_ATOMIC);
			
			sBT_Grp1[0].aNum	= 0;
			sBT_Grp1[0].aPin.aGroup		=DC_F05_3518E_B1_G;
			sBT_Grp1[0].aPin.aBit		=DC_F05_3518E_B1_B;
			sBT_Grp1[0].aPin.aMux		=DC_F05_3518E_B1_MUX;
			sBT_Grp1[0].aPin.aMuxVal	=DC_F05_3518E_B1_MUXVal;
			
			
		}break;
		case DC_Pro_F02:
			break;
		default:return -1;
	}}
	
	if(sBTGrpNUM[0] != 0){
		uint8 _i = 0;
		
		if(\
		request_irq(\
			IRQ_GPIO29,\
			intf_GPIO_Grp1Function,\
			IRQF_SHARED,\
			"Key_Group_One",\
			&sBT_Grp1))
		return -1;
		
#if OPEN_DEBUG
	NVCPrint("Register IRQ_GPIO29 IntRequest Success!\r\n");
#endif
		for( _i=0; _i<sBTGrpNUM[0]; _i++ ){
			sfInitButtonGPIO(&sBT_Grp1[_i].aPin);
		}
		
	}
	
	if(sBTGrpNUM[1] != 0){
		uint8 _i;
		if\
		(request_irq(\
			IRQ_GPIO30,\
			intf_GPIO_Grp2Function,\
			IRQF_SHARED,\
			"Key_Group_Two",\
			&sBT_Grp2))
		return -1;
#if OPEN_DEBUG
	NVCPrint("Register IRQ_GPIO30 IntRequest Success!\r\n");
#endif
		for( _i=0; _i<sBTGrpNUM[1]; _i++ ){
			sfInitButtonGPIO(&sBT_Grp2[_i].aPin);
		}
	}
	
	if(sBTGrpNUM[2] != 0){
		uint8 _i;
		if(\
		request_irq(\
			IRQ_GPIO31,\
			intf_GPIO_Grp3Function,\
			IRQF_SHARED,\
			"Key_Group_Three",\
			&sBT_Grp3))
		return -1;
#if OPEN_DEBUG
	NVCPrint("Register IRQ_GPIO31 IntRequest Success!\r\n");
#endif
		for( _i=0; _i<sBTGrpNUM[2]; _i++ ){
			sfInitButtonGPIO(&sBT_Grp3[_i].aPin);
		}
	}
	
	kfree(sButton_SpinLock);
	

#if OPEN_DEBUG
	NVCPrint("Button Monitor Started!\r\n");
#endif	
	return 0;
}


int32 gfUninitDrv_ButtonMonitor(uint8 iRemain){
	
	if(sBTGrpNUM[0]){
		free_irq(IRQ_GPIO29,&sBT_Grp1);
		kfree(sBT_Grp1);
	}
	
	if(sBTGrpNUM[1]){
		free_irq(IRQ_GPIO30,&sBT_Grp2);
		kfree(sBT_Grp2);
	}
	
	if(sBTGrpNUM[2]){
		free_irq(IRQ_GPIO31,&sBT_Grp3);
		kfree(sBT_Grp3);
	}
	
	
	if(sStateGrp != NULL)
		kfree(sStateGrp);
	
	return 0;
}


/**
param:
		// 0xFF 所有Button
return:
		// 00 无操作 
		// 01 操作中
		// 02 按键按下
		// 04 按键弹起
		// 10 高电平
		// 20 低电平
*/
uint8 gfDrv_ButtonStatus(uint8 iUnit,uint8 icmd){
	
	uint8 tButtonStatus = 0;
	
	if(iUnit>=sBTSumNum){
		return -1;
	}
	
	if(icmd == 1){
		if(DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_ButtonUp))
			tButtonStatus |= 0x10;
		else
			tButtonStatus |= 0x20;
	}else if(icmd == 2){

		if(DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_Int)){
			
			tButtonStatus |= 0x01;
			
			if(DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_RasingEdge)||DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_FallingEdge)){
				DF_BT_ClrStatus(sStateGrp[iUnit],DC_NSB_RasingEdge|DC_NSB_FallingEdge);
				DF_BT_ClrStatus(sStateGrp[iUnit],DC_NSB_Slow1);
				
				
			}else{
				if(DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_Slow1)){
					
					if( (DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_OriLeve))&&(!DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_ButtonUp))  ){
						tButtonStatus = DC_NSB_FallingEdge;
					}else if( (!DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_OriLeve))&&(DF_BT_IsStatusExist(sStateGrp[iUnit],DC_NSB_ButtonUp))  ){
						tButtonStatus = DC_NSB_RasingEdge;
					}
					DF_BT_ClrStatus(sStateGrp[iUnit],DC_NSB_Slow1|DC_NSB_Int);		
				}else{
					DF_BT_SetStatus(sStateGrp[iUnit],DC_NSB_Slow1);
				}
			}
			
		}else{
			tButtonStatus = 0;
		}	
		
	}
	
	return tButtonStatus;
}

int32 gfButton_GetButtonNumber(void){
	return (int32)sBTSumNum;
}


static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id)
{
	uint8 _i;
	uint8 tStateNum;
	uint8 theGPIOStatus;
	for(_i=0; _i<sBTGrpNUM[0]; _i++){
		
		tStateNum = sBT_Grp1[_i].aNum;
		if(DF_Get_GPIO_ADDx(sBT_Grp1[_i].aPin.aGroup,sBT_Grp1[_i].aPin.aBit,HAL_GPIOx_RIS(sBT_Grp1[_i].aPin.aGroup))){
			// GPIO was interrupt
			theGPIOStatus = DF_Get_GPIO_INx(sBT_Grp1[_i].aPin.aGroup,sBT_Grp1[_i].aPin.aBit);
			if(DF_BT_IsStatusExist(sStateGrp[tStateNum],DC_NSB_Int)){
				if(theGPIOStatus){// rasing edge
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_RasingEdge|DC_NSB_ButtonUp);
					
				}else{
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_FallingEdge);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_ButtonUp);
				}
			}else{
				DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_Int);
				
				if(theGPIOStatus){// rasing edge
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_RasingEdge|DC_NSB_ButtonUp);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_OriLeve);
				}else{
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_FallingEdge|DC_NSB_OriLeve);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_ButtonUp);
				}
			}
			
			if(theGPIOStatus){// rasing edge
				
				DF_Clr_GPIO_ADDx(sBT_Grp1[_i].aPin,sBT_Grp1[_i].aPin.aBit,HAL_GPIOx_IEV(sBT_Grp1[_i].aPin.aGroup));
#if OPEN_DEBUG
	NVCPrint_h("Button up!\r\n");
#endif

			}else{// falling edge
				
				DF_Set_GPIO_ADDx(sBT_Grp1[_i].aPin.aGroup,sBT_Grp1[_i].aPin.aBit,HAL_GPIOx_IEV(sBT_Grp1[_i].aPin.aGroup));
#if OPEN_DEBUG
	NVCPrint_h("Button down!\r\n");
#endif	
			}
			
			DF_Clr_GPIO_ADDx(sBT_Grp1[_i].aPin.aGroup,sBT_Grp1[_i].aPin.aBit,HAL_GPIOx_IC(sBT_Grp1[_i].aPin.aGroup));
		
		
		}}
	return 0;
}


static irqreturn_t intf_GPIO_Grp2Function(int irq, void *dev_id){
	uint8 _i;
	uint8 tStateNum;
	uint8 theGPIOStatus;
	
	for(_i=0; _i<sBTGrpNUM[1]; _i++){
		tStateNum = sBT_Grp2[_i].aNum;
		if(DF_Get_GPIO_ADDx(sBT_Grp2[_i].aPin.aGroup,sBT_Grp2[_i].aPin.aBit,HAL_GPIOx_RIS(sBT_Grp2[_i].aPin.aGroup))){
			// GPIO was interrupt
			theGPIOStatus = DF_Get_GPIO_INx(sBT_Grp2[_i].aPin.aGroup,sBT_Grp2[_i].aPin.aBit);
			if(DF_BT_IsStatusExist(sStateGrp[tStateNum],DC_NSB_Int)){
				if(theGPIOStatus){// rasing edge
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_RasingEdge|DC_NSB_ButtonUp);
					
				}else{
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_FallingEdge);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_ButtonUp);
				}
			}else{
				DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_Int);
				
				if(theGPIOStatus){// rasing edge
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_RasingEdge|DC_NSB_ButtonUp);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_OriLeve);
				}else{
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_FallingEdge|DC_NSB_OriLeve);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_ButtonUp);
				}
			}
			
			if(theGPIOStatus){// rasing edge
				
				//change falling edge
				DF_Clr_GPIO_ADDx(sBT_Grp2[_i].aPin,sBT_Grp2[_i].aPin.aBit,HAL_GPIOx_IEV(sBT_Grp2[_i].aPin.aGroup));
#if OPEN_DEBUG
	NVCPrint_h("Button up!\r\n");
#endif
			
			}else{// falling edge
			
				//change rasing edge
				DF_Set_GPIO_ADDx(sBT_Grp2[_i].aPin.aGroup,sBT_Grp2[_i].aPin.aBit,HAL_GPIOx_IEV(sBT_Grp2[_i].aPin.aGroup));
#if OPEN_DEBUG
	NVCPrint_h("Button down!\r\n");
#endif
			}
			
			DF_Clr_GPIO_ADDx(sBT_Grp2[_i].aPin.aGroup,sBT_Grp2[_i].aPin.aBit,HAL_GPIOx_IC(sBT_Grp2[_i].aPin.aGroup));
		}
	}

	return 0;
}

static irqreturn_t intf_GPIO_Grp3Function(int irq, void *dev_id){
	uint8 _i;
	uint8 tStateNum;
	uint8 theGPIOStatus;
	
	
	for(_i=0; _i<sBTGrpNUM[0]; _i++){
		tStateNum = sBT_Grp3[_i].aNum;
		
		if(DF_Get_GPIO_ADDx(sBT_Grp3[_i].aPin.aGroup,sBT_Grp3[_i].aPin.aBit,HAL_GPIOx_RIS(sBT_Grp3[_i].aPin.aGroup))){
			// GPIO was interrupt
			theGPIOStatus = DF_Get_GPIO_INx(sBT_Grp3[_i].aPin.aGroup,sBT_Grp3[_i].aPin.aBit);
			if(DF_BT_IsStatusExist(sStateGrp[tStateNum],DC_NSB_Int)){
				if(theGPIOStatus){// rasing edge
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_RasingEdge|DC_NSB_ButtonUp);
					
				}else{
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_FallingEdge);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_ButtonUp);
				}
			}else{
				DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_Int);
				
				if(theGPIOStatus){// rasing edge
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_RasingEdge|DC_NSB_ButtonUp);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_OriLeve);
				}else{
					DF_BT_SetStatus(sStateGrp[tStateNum],DC_NSB_FallingEdge|DC_NSB_OriLeve);
					DF_BT_ClrStatus(sStateGrp[tStateNum],DC_NSB_ButtonUp);
				}
			}
			
			if(theGPIOStatus){// rasing edge
				
				//change falling edge
				DF_Clr_GPIO_ADDx(sBT_Grp3[_i].aPin,sBT_Grp3[_i].aPin.aBit,HAL_GPIOx_IEV(sBT_Grp3[_i].aPin.aGroup));
#if OPEN_DEBUG
	NVCPrint_h("Button up!\r\n");
#endif
			
			}else{// falling edge
			
				//change rasing edge
				DF_Set_GPIO_ADDx(sBT_Grp3[_i].aPin.aGroup,sBT_Grp3[_i].aPin.aBit,HAL_GPIOx_IEV(sBT_Grp3[_i].aPin.aGroup));
#if OPEN_DEBUG
	NVCPrint_h("Button down!\r\n");
#endif
			}
			
			DF_Clr_GPIO_ADDx(sBT_Grp3[_i].aPin.aGroup,sBT_Grp3[_i].aPin.aBit,HAL_GPIOx_IC(sBT_Grp3[_i].aPin.aGroup));
		}
	}

	return 0;
}



static void sfInitButtonGPIO(mGPIOPinMsg *piButtonPin){
	
	uint32 theSpinLockType;
	spin_lock_irqsave(sButton_SpinLock,theSpinLockType);
	
	// Setting multi function
	DF_Set_GPIO_MUXx(piButtonPin->aMuxVal,piButtonPin->aMux);
	// Setting IO dirction INPUT 
	DF_Clr_GPIO_DIRx(piButtonPin->aGroup,piButtonPin->aBit);
	// Setting Interrupt type	edge
	// DF_Clr_GPIO_ADDx(piButtonPin->aGroup,piButtonPin->aBit,HAL_GPIOx_IS(piButtonPin->aGroup));
	DF_Set_GPIO_ADDx(piButtonPin->aGroup,piButtonPin->aBit,HAL_GPIOx_IS(piButtonPin->aGroup));
	// single edge
	DF_Clr_GPIO_ADDx(piButtonPin->aGroup,piButtonPin->aBit,HAL_GPIOx_IBE(piButtonPin->aGroup));
	// Setting Interrupt type	down(Hight to low)
	DF_Clr_GPIO_ADDx(piButtonPin->aGroup,piButtonPin->aBit,HAL_GPIOx_IEV(piButtonPin->aGroup));;
	// Enable interrupt
	DF_Clr_GPIO_ADDx(piButtonPin->aGroup,piButtonPin->aBit,HAL_GPIOx_IC(piButtonPin->aGroup));
	// Enable interrupt
	DF_Set_GPIO_ADDx(piButtonPin->aGroup,piButtonPin->aBit,HAL_GPIOx_IE(piButtonPin->aGroup));
	
	spin_unlock_irqrestore(sButton_SpinLock,theSpinLockType);
}






