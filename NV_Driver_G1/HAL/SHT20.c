#include "SHT20.h"

#include "USERHAL.h"
#include "PeriodEvent.h"

#include <linux/delay.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Local function definition( for IIC )
static void sfIIC_INIT(void);
static void sfIIC_UNINIT(void);
static void sfIIC_Start(void);
static void sfIIC_Stop(void);
#define DC_IIC_ACK		0x01
#define DC_IIC_MACK		0x02
#define DC_IIC_NOACK	0x04
static uint8 sfIIC_ACK(uint8 iCmd);
static void sfIIC_WriteChar(uint8 iData);
static uint8 sfIIC_ReadChar(void);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Local function definition( for SHT20 )
static void sfSHT20_Reset(void);


#define DC_SHT20_Conf_RH12_T14 		0x00
#define DC_SHT20_Conf_RH8_T12 		0x01
#define DC_SHT20_Conf_RH10_T13 		0x80
#define DC_SHT20_Conf_RH11_T11 		0x81
#define DC_SHT20_Conf_RH_T_Mask		0x81
//
#define DC_SHT20_Conf_LOW_BATTERY 	0x40
#define DC_SHT20_Conf_Heater		0x04
#define DC_SHT20_Conf_Dis_OTP		0x02

#define DC_SHT20OptCMD_SetUserReg	0x01
#define DC_SHT20OptCMD_GetUserReg	0x02
static uint8 sfSH20_RW_UserRegister(uint8 iCmd, uint8 iData);



#define DC_SHT20OptCMD_MEASURE_Temp		0x01
#define DC_SHT20OptCMD_MEASURE_Humidity	0x02
static void sfSH20Opt_ReadEvnValue( uint32 iCmd );
static void sfSH20_GatheringMeasureVal(void);

#define DC_SHT20_UPDATE_PERIOD 1000
static void sfSHT20_UpdateTemp_Humidity(void);
static void sfSHT20_Status_RecordMode(uint8 iCmd);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SHT20 Part Code
#define SHT20_COMMAND_Write	0x80
#define SHT20_COMMAND_Read	0x81


// sensor command
#define  SHT20_CMD_Hold_T    	0xE3 	// command trig. temp meas. hold master
#define  SHT20_CMD_Hold_RH   	0xE5 	// command trig. humidity meas. hold master
#define  SHT20_CMD_Nohold_T  	0xF3 	// command trig. temp meas. no hold master
#define  SHT20_CMD_Nohold_RH 	0xF5 	// command trig. humidity meas. no hold master
#define  SHT20_CMD_UserReg_W    0xE6 	// command writing user register
#define  SHT20_CMD_UserReg_R    0xE7 	// command reading user register
#define  SHT20_CMD_RESET        0xFE 	// command soft reset


struct{
	int32 	aLastHumidity;
	int32 	aLastTemperature;
	uint32 	aStatus;
#define DC_SHT20_Sta_Working		0x0001
// Measure event has been fork
#define DC_SHT20_Sta_Busy			0x0002
// The Job of Measure has been working

#define DC_SHT20_Sta_UpdateT		0x0004
#define DC_SHT20_Sta_UpdateRH		0x0008
#define DC_SHT20_Sta_UpdateMask		(0x0008+0x0004)

#define DC_SHT20_Sta_ReadT			0x0010
#define DC_SHT20_Sta_ReadRH			0x0020
#define DC_SHT20_Sta_ReadMask		(0x0020+0x0010)

// 这个是个模式分别对应 SHT20 的采集模式
#define DC_SHT20_Sta_GatMode_0		0x0000
// 0 为 DC_SHT20_Conf_RH12_T14     
#define DC_SHT20_Sta_GatMode_1		0x0040
// 1 DC_SHT20_Conf_RH8_T12
#define DC_SHT20_Sta_GatMode_2		0x0080
// 2 DC_SHT20_Conf_RH10_T13
#define DC_SHT20_Sta_GatMode_3		(0x0040 + 0x0080)
// 3 DC_SHT20_Conf_RH11_T11

}soSHT20_RunStatus;


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Local function
// ================================================================
// ================================================================
void gfSHT20_INIT(void){
	uint8 tSHT20Mode;
	
	soSHT20_RunStatus.aLastHumidity 	= 0;
	soSHT20_RunStatus.aLastTemperature 	= 0;
	soSHT20_RunStatus.aStatus 			= DC_SHT20_Sta_GatMode_0;
	
	sfIIC_INIT();
	sfSHT20_Reset();
	tSHT20Mode = sfSH20_RW_UserRegister( \
					DC_SHT20OptCMD_SetUserReg, \
					DC_SHT20_Conf_RH11_T11|DC_SHT20_Conf_Dis_OTP);
	sfIIC_UNINIT();

	
	gfPE_RegisterEvent(sfSHT20_UpdateTemp_Humidity,DC_SHT20_UPDATE_PERIOD,DC_PE_TYPE_INT);

#if OPEN_DEBUG
	NVCPrint_h("---------->:%x\r\n",(unsigned int)tSHT20Mode);
	NVCPrint("SHT20 Started!\r\n");
#endif
	
}



// ================================================================
// ================================================================
void gfSHT20_UNINIT(void){
	
	gfPE_UnregisterEvent(sfSHT20_UpdateTemp_Humidity,DC_PE_TYPE_INT);
}





// ================================================================
// ================================================================
int32 gfSHT20Opt_GetMeasure(uint8 iCmd){
	
	if( iCmd&DC_SHT20OptCMD_Get_Temp ){
		return soSHT20_RunStatus.aLastTemperature;
	}else if( iCmd&DC_SHT20OptCMD_Get_Humidity ){
		return soSHT20_RunStatus.aLastHumidity;		
	}
	return 0;
}





 // ================================================================
// ================================================================
#define	DC_SHT20_Check_Period	5
static void sfSH20_SamplingProcess(void);
static void sfSHT20_UpdateTemp_Humidity(void){

	sfIIC_INIT();
	soSHT20_RunStatus.aStatus \
		|= 	DC_SHT20_Sta_Working\
		+	DC_SHT20_Sta_UpdateT\
		+	DC_SHT20_Sta_UpdateRH;
	
	gfPE_RegisterEvent( \
			sfSH20_SamplingProcess,\
			DC_SHT20_Check_Period,\
			DC_PE_TYPE_INT);
}


// ================================================================
// ================================================================
static void sfSH20_SamplingProcess(void){
	
	if( soSHT20_RunStatus.aStatus&DC_SHT20_Sta_Busy ){
		sfSH20_GatheringMeasureVal();
	}else{
		if( soSHT20_RunStatus.aStatus&DC_SHT20_Sta_UpdateT ){
			
			soSHT20_RunStatus.aStatus &= ~DC_SHT20_Sta_ReadMask;
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_ReadT;
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_Busy;
			sfSH20Opt_ReadEvnValue(DC_SHT20OptCMD_MEASURE_Temp);
			
		}else if( soSHT20_RunStatus.aStatus&DC_SHT20_Sta_UpdateRH ){
			
			soSHT20_RunStatus.aStatus &= ~DC_SHT20_Sta_ReadMask;
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_ReadRH;
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_Busy;
			sfSH20Opt_ReadEvnValue(DC_SHT20OptCMD_MEASURE_Humidity);
			
		}else{
			
			soSHT20_RunStatus.aStatus &= ~DC_SHT20_Sta_Working;
			
			gfPE_UnregisterEvent( \
				sfSH20_SamplingProcess,\
				DC_PE_TYPE_INT);
			sfIIC_UNINIT();
		}
	}
}



// ================================================================
// ================================================================
static void sfSH20Opt_ReadEvnValue( uint32 iCmd ){
	
	sfSH20_RW_UserRegister( \
			DC_SHT20OptCMD_SetUserReg,\
			DC_SHT20_Conf_RH11_T11|DC_SHT20_Conf_Dis_OTP);
	
	sfIIC_Start();
	sfIIC_WriteChar(SHT20_COMMAND_Write);
	sfIIC_ACK(DC_IIC_ACK);
	
	if( iCmd&DC_SHT20OptCMD_MEASURE_Temp ){
		sfIIC_WriteChar(SHT20_CMD_Nohold_T);
	}else if( iCmd&DC_SHT20OptCMD_MEASURE_Humidity ){
		sfIIC_WriteChar(SHT20_CMD_Nohold_RH);
	}
	sfIIC_ACK(DC_IIC_ACK);
}

// ================================================================
// ================================================================
static void sfSH20_GatheringMeasureVal(void){
	
	sfIIC_Start();
	sfIIC_WriteChar(SHT20_COMMAND_Read);
	if( 0 == sfIIC_ACK(DC_IIC_ACK) ){
		uint32 tSHT20Value = 0;
		
		tSHT20Value |= sfIIC_ReadChar();
		sfIIC_ACK(DC_IIC_MACK);
		
		tSHT20Value <<= 8;
		tSHT20Value |= sfIIC_ReadChar();
		sfIIC_ACK(DC_IIC_MACK);
		
		tSHT20Value <<= 8;
		tSHT20Value |= sfIIC_ReadChar();
		sfIIC_ACK( DC_IIC_NOACK );
		sfIIC_Stop();
		
		// printk("---------->the temp:%x\r\n",(unsigned int)tSHT20Value);
		
		tSHT20Value = tSHT20Value>>8;
		if( (soSHT20_RunStatus.aStatus&DC_SHT20_Sta_ReadMask) == DC_SHT20_Sta_ReadT ){
			int32 tTemp;
			
			tTemp = ((tSHT20Value>>2)*17572)/16384 - 4685;
			soSHT20_RunStatus.aLastTemperature = tTemp;
			
			soSHT20_RunStatus.aStatus &= \
				~(	DC_SHT20_Sta_UpdateT\
				|	DC_SHT20_Sta_Busy);
			// printk("temp: %d \r\n",(int)tTemp);
		}else if( (soSHT20_RunStatus.aStatus&DC_SHT20_Sta_ReadMask) == DC_SHT20_Sta_ReadRH ){
			
			int32 tHumidity;
			
			tHumidity = ((tSHT20Value>>2)*12500)/16384 - 600;
			soSHT20_RunStatus.aLastHumidity = tHumidity;
			
			soSHT20_RunStatus.aStatus &= \
				~(	DC_SHT20_Sta_UpdateRH\
				|	DC_SHT20_Sta_Busy);
			// printk("tHumidity: %d \r\n",(int)tHumidity);
		}
	}
}



// ================================================================
// ================================================================
static void sfSHT20_Status_RecordMode(uint8 iCmd){
	
	switch( iCmd&DC_SHT20_Conf_RH_T_Mask ){
		case DC_SHT20_Conf_RH12_T14:{
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_GatMode_0;
		}break;
		case DC_SHT20_Conf_RH8_T12 :{
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_GatMode_1;
			
		}break;
		case DC_SHT20_Conf_RH10_T13:{
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_GatMode_2;
			
		}break;
		case DC_SHT20_Conf_RH11_T11:{
			soSHT20_RunStatus.aStatus |= DC_SHT20_Sta_GatMode_3;

		}break;
		default:break;
	}
}

// ================================================================
// ================================================================
static uint8 sfSH20_RW_UserRegister(uint8 iCmd, uint8 iData){
	uint8 tUserReg = 0;
	
	if( iCmd&DC_SHT20OptCMD_GetUserReg ){
		sfIIC_Start();
		sfIIC_WriteChar(SHT20_COMMAND_Write);
		sfIIC_ACK(DC_IIC_ACK);
		sfIIC_WriteChar(SHT20_CMD_UserReg_R);
		sfIIC_ACK(DC_IIC_ACK);
		sfIIC_Start();
		sfIIC_WriteChar(SHT20_COMMAND_Read);
		sfIIC_ACK(DC_IIC_ACK);
		tUserReg = sfIIC_ReadChar();
		sfIIC_ACK(DC_IIC_NOACK);
	}
	
	if( iCmd&DC_SHT20OptCMD_SetUserReg ){
		tUserReg &= ~DC_SHT20_Conf_RH_T_Mask;
		tUserReg |= iData;
		
		sfIIC_Start();
		sfIIC_WriteChar(SHT20_COMMAND_Write);
		sfIIC_ACK(DC_IIC_ACK);
		sfIIC_WriteChar(SHT20_CMD_UserReg_W);
		sfIIC_ACK(DC_IIC_ACK);
		sfIIC_WriteChar(tUserReg);
		sfIIC_ACK(DC_IIC_ACK);
	}
	sfIIC_Stop();
	sfSHT20_Status_RecordMode(iData);
	return tUserReg;
}

// ================================================================
// ================================================================
static void sfSHT20_Reset(void){
	sfIIC_Start();
	sfIIC_WriteChar(SHT20_COMMAND_Write);
	sfIIC_ACK(DC_IIC_ACK);
	sfIIC_WriteChar(SHT20_CMD_RESET);
	sfIIC_ACK(DC_IIC_ACK);
	sfIIC_Stop();
	mdelay(50);
}


























































////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// IIC Part Code

#define IIC_DELAY(_x)			udelay(_x)

#define DC_D03_3518c_SCK_G		2
#define DC_D03_3518c_SCK_B		1
#define DC_D03_3518c_SCK_MUX	7
#define DC_D03_3518c_SCK_MUXVal	0x00

#define DC_D03_3518c_SDA_G		2
#define DC_D03_3518c_SDA_B		0
#define DC_D03_3518c_SDA_MUX	6
#define DC_D03_3518c_SDA_MUXVal	0x00

#define IIC_CLK_OUT		DF_Set_GPIO_DIRx(DC_D03_3518c_SCK_G,DC_D03_3518c_SCK_B)
#define IIC_CLK_IN		DF_Clr_GPIO_DIRx(DC_D03_3518c_SCK_G,DC_D03_3518c_SCK_B)
#define IIC_CLK_HIGH	DF_Set_GPIO_Outx(DC_D03_3518c_SCK_G,DC_D03_3518c_SCK_B)
#define IIC_CLK_LOW		DF_Clr_GPIO_Outx(DC_D03_3518c_SCK_G,DC_D03_3518c_SCK_B)

#define IIC_SDA_OUT		DF_Set_GPIO_DIRx(DC_D03_3518c_SDA_G,DC_D03_3518c_SDA_B)
#define IIC_SDA_IN		DF_Clr_GPIO_DIRx(DC_D03_3518c_SDA_G,DC_D03_3518c_SDA_B)
#define IIC_SDA_HIGH	DF_Set_GPIO_Outx(DC_D03_3518c_SDA_G,DC_D03_3518c_SDA_B)
#define IIC_SDA_LOW		DF_Clr_GPIO_Outx(DC_D03_3518c_SDA_G,DC_D03_3518c_SDA_B)
#define IIC_SDA_VAL		DF_Get_GPIO_INx(DC_D03_3518c_SDA_G,DC_D03_3518c_SDA_B)



typedef struct{
	uint32	aMux;
	uint32	aDir;
	// uint32	aOUT;
}mPinField;

static mPinField sIIC_Pin_OriStatus[2];

static void sfIIC_INIT(void){
	
	// sIIC_Pin_OriStatus[0].aMux = HAL_readl( HAL_MUXCTRL_regx(DC_D03_3518c_SCK_MUX) );
	// sIIC_Pin_OriStatus[0].aDir = HAL_readl( HAL_GPIOx_DIR(DC_D03_3518c_SCK_B) );
	// sIIC_Pin_OriStatus[1].aMux = HAL_readl( HAL_MUXCTRL_regx(DC_D03_3518c_SDA_MUX) );
	// sIIC_Pin_OriStatus[1].aDir = HAL_readl( HAL_GPIOx_DIR(DC_D03_3518c_SDA_B) );
	
	
	DF_Set_GPIO_MUXx(DC_D03_3518c_SCK_MUXVal ,DC_D03_3518c_SCK_MUX);
	DF_Set_GPIO_MUXx(DC_D03_3518c_SDA_MUXVal ,DC_D03_3518c_SDA_MUX);
	IIC_CLK_OUT;
	IIC_SDA_OUT;
	IIC_CLK_HIGH;
	IIC_SDA_HIGH;
}
static void sfIIC_UNINIT(void){
	// HAL_writel( sIIC_Pin_OriStatus[0].aMux, HAL_MUXCTRL_regx(DC_D03_3518c_SCK_MUX) );
	// HAL_writel( sIIC_Pin_OriStatus[1].aMux, HAL_MUXCTRL_regx(DC_D03_3518c_SDA_MUX) );
	
	// HAL_writel( sIIC_Pin_OriStatus[0].aDir, HAL_GPIOx_DIR(DC_D03_3518c_SCK_B) );
	// HAL_writel( sIIC_Pin_OriStatus[1].aDir, HAL_GPIOx_DIR(DC_D03_3518c_SDA_B) );
}
static void sfIIC_Start(void){
	IIC_SDA_OUT;
	IIC_SDA_HIGH;
	IIC_CLK_HIGH;
	IIC_DELAY(5);
	IIC_SDA_LOW;
	IIC_DELAY(5);
	IIC_CLK_LOW;
	IIC_DELAY(5);
}
static void sfIIC_Stop(void){
	IIC_CLK_LOW;
	IIC_SDA_OUT;
	IIC_SDA_LOW;
	IIC_DELAY(5);
	IIC_CLK_HIGH;
	IIC_DELAY(2);
	IIC_SDA_HIGH;
}


static uint8 sfIIC_ACK(uint8 iCmd){
	uint8 tRet = 0;
	uint8 _i;
	
	if( iCmd&DC_IIC_ACK ){
		// IIC_CLK_LOW;
		IIC_SDA_IN;
		IIC_DELAY(2);
		IIC_CLK_HIGH;
		_i = 50;
		while( (_i != 0)&&( IIC_SDA_VAL ) ){
			IIC_DELAY(2);
			_i--;
		}
		
		if( _i == 0 ){
			tRet = 1;
		}
		
		IIC_CLK_LOW;
		IIC_DELAY(15);
	}else if( iCmd&DC_IIC_NOACK ){
		IIC_SDA_IN;
		IIC_DELAY(2);
		IIC_CLK_HIGH;
		IIC_DELAY(2);
		IIC_CLK_LOW;
		IIC_DELAY(15);
	}else if( iCmd&DC_IIC_MACK	){
		IIC_SDA_OUT;
		IIC_SDA_LOW;
		IIC_DELAY(3);
		IIC_CLK_HIGH;
		IIC_DELAY(5);
		IIC_CLK_LOW;
		IIC_DELAY(15);
	}
	return tRet;
}

static void sfIIC_WriteChar(uint8 iData){
	uint8 _i;
	IIC_SDA_OUT;
	IIC_DELAY(5);
	
	for(_i = 0x80; _i >0 ;){
		if( iData&_i ){
			IIC_SDA_HIGH;
		}else{
			IIC_SDA_LOW;
		}
		_i>>=1;
		IIC_DELAY(1);
		IIC_CLK_HIGH;
		IIC_DELAY(5);
		IIC_CLK_LOW;
		IIC_DELAY(1);
	}
}

static uint8 sfIIC_ReadChar(void){
	uint8 tRet = 0;
	uint8 _i;
	IIC_SDA_IN;
	IIC_DELAY(2);
	
	for( _i=0; _i<8; _i++ ){
		IIC_CLK_HIGH;
		IIC_DELAY(2);
		
		tRet <<= 1;
		if( IIC_SDA_VAL )
			tRet |= 0x01;
		
		IIC_DELAY(1);
		IIC_CLK_LOW;
		IIC_DELAY(2);
	}
	return tRet;
}










