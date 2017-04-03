#include "PanTilt.h"
#include "USERHAL.h"
#include "HAL_Timer.h"

#include <linux/slab.h>  	//kmalloc
#include <linux/interrupt.h>
#include <linux/spinlock.h>
//#include <linux/types.h>
//#include <linux/fcntl.h>
#include <linux/string.h>

//#include <linux/kernel.h>
//#include <linux/init.h>
//#include <asm/system.h>		//  
//#include <asm/io.h>

////////////////////////////////////////////////////////////////////////////////
// Statement Local Funcation
static irqreturn_t _itrpt_timer0(int irq, void *id);
static void sfOutputAPhasesToVSM(uint8 _cmd);
static void sfOutputAPhasesToHSM(uint8 _cmd);
static void sfInitSMGPIO(void);
static void sfStepMotorDriver(void);
static void sfDriveToFindOutCoordinate(void);

////////////////////////////////////////////////////////////////////////////////
// Statement Variable
static mSMD_Struct *spSMD_Struct = NULL ;
static uint8 sStepMotorPhases[10]={0x01,0x03,0x02,0x06,0x04,0x0C,0x08,0x09,0x00};
static uint32 gSMStatus;
static uint16 gHSMCap;
static uint16 gVSMCap;
static uint32 gRaducingRatio;
static uint32 gRaducingCount;
mCoodinate gCoodinate;
static DEFINE_SPINLOCK(gLock_cfgPanTilt);
static void (*spfTimer0ServerFunction)(void);


////////////////////////////////////////////////////////////////////////////////
// Global Function
void gfInitSMHIVPort(void)
{
	uint32 theInitLockFlag;
	
	spin_lock_irqsave(&gLock_cfgPanTilt,theInitLockFlag);
	sfInitSMGPIO();
	spin_unlock_irqrestore(&gLock_cfgPanTilt,theInitLockFlag);
	
	{//初始化
		//初始化Time0硬件
		int theRct;
		spin_lock_irqsave(&gLock_cfgPanTilt,theInitLockFlag);
		spfTimer0ServerFunction = NULL;
		fgConfigTimer0(100,DC_Timer_PeriodMode|DC_Timer_Intenable|DC_Timer_Timerpre_256|DC_Timer_Count_32bit);
		spin_unlock_irqrestore(&gLock_cfgPanTilt,theInitLockFlag);
		
		spSMD_Struct = (mSMD_Struct*)kmalloc(sizeof(mSMD_Struct), GFP_KERNEL);
		spSMD_Struct->irq_num = IRQ_TM0_TM1;
		spSMD_Struct->irq_mode = IRQF_SHARED;
		spSMD_Struct->irq_name = PanTilt_NAME;

		theRct = request_irq(	spSMD_Struct->irq_num,\
								_itrpt_timer0,\
								spSMD_Struct->irq_mode,\
								spSMD_Struct->irq_name,\
								spSMD_Struct);
		
		if(theRct==0){
			spin_lock_irqsave(&gLock_cfgPanTilt,theInitLockFlag);
			DF_Timer0_Enable;
			spin_unlock_irqrestore(&gLock_cfgPanTilt,theInitLockFlag);
			printk("Regist success!\r\n");
		}
	}
	{// Scanning Current position
		//wait_queue_head_t 	aWQH_Return;
		gSMStatus = 0;
		spfTimer0ServerFunction = sfDriveToFindOutCoordinate;
		gCoodinate.aXRod = 0;
		gCoodinate.aYRod = 0;
		// Set initialize status to
		DF_SMS_SetStatus(DC_SMS_HIniting);
		DF_SMS_SetStatus(DC_SMS_VIniting);
		// start to find the origin
		DF_SMS_SetStatus(DC_SMS_VRunning);
		DF_SMS_SetStatus(DC_SMS_VPeriodDone);
		DF_SMS_SetStatus(DC_SMS_HRunning);
		DF_SMS_SetStatus(DC_SMS_HPeriodDone);
	}
//	gSMStatus = 0;
//	spfTimer0ServerFunction = sfStepMotorDriver;
	return;
}


void gfUninitSMHIVPort(void)
{
	uint32 theUninitLockFlag;
	spin_lock_irqsave(&gLock_cfgPanTilt,theUninitLockFlag);
	DF_Timer0_ClrIntFlag;
	DF_Timer0_Disable;
	spin_unlock_irqrestore(&gLock_cfgPanTilt,theUninitLockFlag);
	
	free_irq(spSMD_Struct->irq_num,spSMD_Struct);
	kfree(spSMD_Struct);
	spSMD_Struct = NULL;
}


int gfPanTiltIsBusy(void){
	if(DF_SMS_IsStatusExist(DC_SMS_VIniting)||DF_SMS_IsStatusExist(DC_SMS_HIniting)){
		return -1;	
	}else{
		return 0;		
	}
}

int gfStartSMMove(uint16 iHSteps,uint16 iVSteps,uint8 iCmd)
{
	if(iCmd&DC_SSMM_HSet){
		
		if(iCmd&DC_SSMM_HDgr){
			uint16 theCount_1;
			theCount_1 = iHSteps<<7;
			iHSteps = theCount_1/45;
		}
		
		if(iHSteps==0)
			return DC_SSMM_BE_HSetButNoData;
		if((iHSteps+gCoodinate.aXRod)>DC_LIMIT_HTML)
			return DC_SSMM_BE_VOverRange;
		
		if(iCmd&DC_SSMM_HAntiClockWise){
			DF_SMS_SetStatus(DC_SMS_HAntiRoll);		
		}else{
			DF_SMS_ClrStatus(DC_SMS_HAntiRoll);
		}
		
		gHSMCap = iHSteps;
		
		DF_SMS_SetStatus(DC_SMS_HRunning);
		DF_SMS_SetStatus(DC_SMS_HPeriodDone);	
	}
	if(iCmd&DC_SSMM_VSet){
		
		if(iCmd&DC_SSMM_VDgr){
			uint16 theCount_1;
			theCount_1 = iVSteps<<7;
			iHSteps = theCount_1/45;
		}
		
		if(iVSteps==0)
			return DC_SSMM_BE_HSetButNoData;
		if((iVSteps+gCoodinate.aYRod)>DC_LIMIT_VTML)
			return DC_SSMM_BE_VOverRange;
		
		if(iCmd&DC_SSMM_VAntiClockWise){
			DF_SMS_SetStatus(DC_SMS_VAntiRoll);
		}else{
			DF_SMS_ClrStatus(DC_SMS_VAntiRoll);
		}
		gVSMCap = iVSteps;
		DF_SMS_SetStatus(DC_SMS_VRunning);
		DF_SMS_SetStatus(DC_SMS_VPeriodDone);	
		
		if(iCmd&DC_SSMM_HSet){
			gRaducingCount = 0;
			if(iHSteps>iVSteps){
				gRaducingRatio = (uint32)(iHSteps<<7)/iVSteps;
				DF_SMS_SetStatus(DC_SMS_VReducing);
			}else{
				gRaducingRatio = (uint32)(iVSteps<<7)/iHSteps;
				DF_SMS_SetStatus(DC_SMS_HReducing);
			}
		}
	}
	return DC_SSMM_B_Success;
}


////////////////////////////////////////////////////////////////////////////////
// Local Function
static irqreturn_t _itrpt_timer0(int irq, void *id)
{
	DF_Timer0_ClrIntFlag;
	if(spfTimer0ServerFunction!=NULL)
		spfTimer0ServerFunction();
	return 0;
}


static void sfStepMotorDriver(void)
{
	uint8 theCurrentPhases;
	//	
	// 	init
	//	gHSMCap = 2;
	//	DF_SMS_SetStatus(DC_SMS_HRunning);
	//	DF_SMS_SetStatus(DC_SMS_HPeriodDone);
	//	
	if(DF_SMS_IsStatusExist(DC_SMS_HRunning)){
		if(DF_SMS_IsStatusExist(DC_SMS_HReducing)){
			gRaducingCount+=1<<7;
			if(gRaducingCount>=gRaducingRatio){
				gRaducingCount = gRaducingCount-gRaducingRatio;
			}else{
				goto GT_DriveHStepMotorEnd;
			}
		}
		if(DF_SMS_IsStatusExist(DC_SMS_HPeriodDone)){
			if((0<gHSMCap)&&(gHSMCap<=1024)){
				gHSMCap--;
			}else if(gHSMCap==0){ // if(gHSMCap>=1024)//超出360度
				DF_SMS_ClrStatus(DC_SMS_HRunning);
				DF_SMS_ClrStatus(DC_SMS_HPeriodDone);
				// It should have been checked yes or not seted up
				// but it dont effect the system normal running,
				// and for reducing unnecessary wasted.
				// i decide to ignore this step.
				DF_SMS_ClrStatus(DC_SMS_HIniting);
				
				if(DF_SMS_IsStatusExist(DC_SMS_HRspEnding)){
					// 反馈结束位
					//DF_SMS_SetStatus();
				}
				sfOutputAPhasesToHSM(sStepMotorPhases[8]);
				goto GT_DriveHStepMotorEnd;//JumpOutTheLoop
			}
			DF_SMS_ClrStatus(DC_SMS_HPeriodDone);
		}
		if(DF_SMS_IsStatusExist(DC_SMS_HAntiRoll))
			theCurrentPhases = 7-DF_SMS_GetHRhythm;
		else
			theCurrentPhases = DF_SMS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_SMS_HaddRhythm;
	}
GT_DriveHStepMotorEnd:

	if(DF_SMS_IsStatusExist(DC_SMS_VRunning)){
		if(DF_SMS_IsStatusExist(DC_SMS_VReducing)){
			gRaducingCount+=1<<7;
			if(gRaducingCount>=gRaducingRatio){
				gRaducingCount = gRaducingCount-gRaducingRatio;
			}else{
				goto GT_DriveVStepMotorEnd;
			}
		}
		if(DF_SMS_IsStatusExist(DC_SMS_VPeriodDone)){
			if((0<gVSMCap)&&(gVSMCap<=1024)){
				gVSMCap--;
			}else if(gVSMCap==0){
				DF_SMS_ClrStatus(DC_SMS_VRunning);
				DF_SMS_ClrStatus(DC_SMS_VPeriodDone);
				// 
				// 
				// ditto!
				// 
				DF_SMS_ClrStatus(DC_SMS_VIniting);
				
				
				if(DF_SMS_IsStatusExist(DC_SMS_VRspEnding)){
					// 反馈结束位
					//DF_SMS_SetStatus();
				}
				sfOutputAPhasesToVSM(sStepMotorPhases[8]);
				goto GT_DriveVStepMotorEnd;//JumpOutTheLoop
			}
			DF_SMS_ClrStatus(DC_SMS_VPeriodDone);
		}
		if(DF_SMS_IsStatusExist(DC_SMS_VAntiRoll))
			theCurrentPhases = 7 - DF_SMS_GetVRhythm;
		else
			theCurrentPhases = DF_SMS_GetVRhythm;
		
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_SMS_VaddRhythm;
	}
GT_DriveVStepMotorEnd:
return ;
}

static void sfDriveToFindOutCoordinate(void)
{
	uint8  theCurrentPhases;
		//gCoodinate.aXRod = 0;
		//gCoodinate.aYRod = 0;
	/// Test the horizontal Rotation range
	if(DF_SMS_IsStatusExist(DC_SMS_HRunning)){
		if(DF_SMS_IsStatusExist(DC_SMS_HPeriodDone)){
			gCoodinate.aXRod++;
			
			DF_SMS_ClrStatus(DC_SMS_HPeriodDone);
			if(!DF_Get_SM_INx(DC_HSMLim_ORG_G,DC_HSMLim_ORG_B)){
				//DF_SMS_ClrStatus(DC_SMS_HIniting);
				DF_SMS_ClrStatus(DC_SMS_HRunning);
				//if(!DF_SMS_IsStatusExist(DC_SMS_VIniting))
				if(!DF_SMS_IsStatusExist(DC_SMS_VRunning))
					goto GT_DriveToFindOutCoordinate;
			}
		}
		theCurrentPhases = DF_SMS_GetHRhythm;
		sfOutputAPhasesToHSM(theCurrentPhases);
		DF_SMS_HaddRhythm;
	}
	/// Test the vertical Rotation range
	if(DF_SMS_IsStatusExist(DC_SMS_VRunning)){
		if(DF_SMS_IsStatusExist(DC_SMS_VPeriodDone)){
			gCoodinate.aYRod++;
			DF_SMS_ClrStatus(DC_SMS_VPeriodDone);
			if(!DF_Get_SM_INx(DC_VSMLim_TML_G,DC_VSMLim_TML_B)){
				//DF_SMS_ClrStatus(DC_SMS_VIniting);
				DF_SMS_ClrStatus(DC_SMS_VRunning);
				//if(!DF_SMS_IsStatusExist(DC_SMS_HIniting))
				if(!DF_SMS_IsStatusExist(DC_SMS_HRunning))
					goto GT_DriveToFindOutCoordinate;
			}
		}
		theCurrentPhases = 7 - DF_SMS_GetVRhythm;
		sfOutputAPhasesToVSM(theCurrentPhases);
		DF_SMS_VaddRhythm;
	}
	return;
	
GT_DriveToFindOutCoordinate:
	{
		int theRet;
		uint8 theSetCmd = 0;
		uint16 theHSetData;
		uint16 theVSetData;
		
		spfTimer0ServerFunction = sfStepMotorDriver;
		
		theHSetData = gCoodinate.aXRod-1;
		theVSetData = gCoodinate.aYRod-1;
		
		printk("gCoodinate.aXRod = %d\r\ngCoodinate.aYRod = %d\r\n",gCoodinate.aXRod,gCoodinate.aYRod);
		if(gCoodinate.aXRod>0){
			theSetCmd |= (DC_SSMM_HSet|DC_SSMM_HAntiClockWise); 
		}	
		if(gCoodinate.aYRod>0){
			theSetCmd |= DC_SSMM_VSet; 
		}
		gCoodinate.aXRod = 0;
		gCoodinate.aYRod = 0;
		theRet = gfStartSMMove(theHSetData,theVSetData,theSetCmd);
		printk("return %d\r\n",theRet);
		
	}
	return;
}


static void sfInitSMGPIO(void)
{
	// select Pin GPIO mode
	// Drive step motor
	DF_Set_SM_MUXx(0x00,DC_VSM0_MUX);
	DF_Set_SM_MUXx(0x00,DC_VSM1_MUX);
	DF_Set_SM_MUXx(0x00,DC_VSM2_MUX);
	DF_Set_SM_MUXx(0x00,DC_VSM3_MUX);
	
	DF_Set_SM_MUXx(0x00,DC_HSM0_MUX);
	DF_Set_SM_MUXx(0x02,DC_HSM1_MUX);
	DF_Set_SM_MUXx(0x00,DC_HSM2_MUX);
	DF_Set_SM_MUXx(0x00,DC_HSM3_MUX);
	//	collect Limit message
	DF_Set_SM_MUXx(0x00,DC_HSMLim_ORG_MUX);
	DF_Set_SM_MUXx(0x00,DC_HSMLim_TML_MUX);
	
	DF_Set_SM_MUXx(0x00,DC_VSMLim_ORG_MUX);
	DF_Set_SM_MUXx(0x00,DC_VSMLim_TML_MUX);
	
	
	// Set GPIO direction
	// Drive Output
	DF_Set_SM_DIRx(DC_HSM_G0,DC_HSM_B0);
	DF_Set_SM_DIRx(DC_HSM_G1,DC_HSM_B1);
	DF_Set_SM_DIRx(DC_HSM_G2,DC_HSM_B2);
	DF_Set_SM_DIRx(DC_HSM_G3,DC_HSM_B3);
	
	DF_Set_SM_DIRx(DC_VSM_G0,DC_VSM_B0);
	DF_Set_SM_DIRx(DC_VSM_G1,DC_VSM_B1);
	DF_Set_SM_DIRx(DC_VSM_G2,DC_VSM_B2);
	DF_Set_SM_DIRx(DC_VSM_G3,DC_VSM_B3);

	// Limit Input
	DF_Clr_SM_DIRx(DC_HSMLim_ORG_G,DC_HSMLim_ORG_B);
	DF_Clr_SM_DIRx(DC_HSMLim_TML_G,DC_HSMLim_TML_B);
	DF_Clr_SM_DIRx(DC_VSMLim_ORG_G,DC_VSMLim_ORG_B);
	DF_Clr_SM_DIRx(DC_VSMLim_TML_G,DC_VSMLim_TML_B);
	
	
	sfOutputAPhasesToHSM(sStepMotorPhases[8]);
	sfOutputAPhasesToVSM(sStepMotorPhases[8]);	
}

static void sfOutputAPhasesToHSM(uint8 _cmd)
{
	//printk("H-->%x\r\n",_cmd);
	if(0x01&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_HSM_G0,DC_HSM_B0);
	}else{
		DF_Clr_SM_Outx(DC_HSM_G0,DC_HSM_B0);
	}
	if(0x02&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_HSM_G1,DC_HSM_B1);
	}else{
		DF_Clr_SM_Outx(DC_HSM_G1,DC_HSM_B1);
	}
	if(0x04&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_HSM_G2,DC_HSM_B2);
	}else{
		DF_Clr_SM_Outx(DC_HSM_G2,DC_HSM_B2);
	}
	if(0x08&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_HSM_G3,DC_HSM_B3);
	}else{
		DF_Clr_SM_Outx(DC_HSM_G3,DC_HSM_B3);
	}
}


static void sfOutputAPhasesToVSM(uint8 _cmd)
{
	//printk("V-->%x\r\n",_cmd);
	if(0x01&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_VSM_G0,DC_VSM_B0);
	}else{
		DF_Clr_SM_Outx(DC_VSM_G0,DC_VSM_B0);
	}
	if(0x02&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_VSM_G1,DC_VSM_B1);
	}else{
		DF_Clr_SM_Outx(DC_VSM_G1,DC_VSM_B1);
	}
	if(0x04&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_VSM_G2,DC_VSM_B2);
	}else{
		DF_Clr_SM_Outx(DC_VSM_G2,DC_VSM_B2);
	}
	if(0x08&sStepMotorPhases[_cmd]){
		DF_Set_SM_Outx(DC_VSM_G3,DC_VSM_B3);
	}else{
		DF_Clr_SM_Outx(DC_VSM_G3,DC_VSM_B3);
	}
}


