/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
// remote
#include "../HAL.h"

//==============================================================================
// MACRO
// CONSTANT
#define DC_TLV320_SpeakerON     0x01
#define DC_TLV320_SpeakerOFF    0x02
#define DC_TLV320_MiCroON       0x03
#define DC_TLV320_MiCroOFF      0x04
// =========================TLV320AIC23B ADDRESS IIC
#define DC_TLV_ADDR_CHIP            0x34	// CS:0
// #define DC_TLV_ADDR_CHIP         0x36        // CS:1
#define DC_TLV_ADDR_LLICVC          0x00	//  Left line input channel volume control
#define DC_TLV_ADDR_RLICVC          0x01	//  Right line input channel volume control
#define DC_TLV_ADDR_LCHVC           0x02	//  Left channel headphone volume control
#define DC_TLV_ADDR_RCHVC           0x03	//  Right channel headphone volume control
#define DC_TLV_ADDR_AAPC            0x04	//  Analog audio path control
#define DC_TLV_ADDR_DAPC            0x05	//  Digital audio path control
#define DC_TLV_ADDR_PDC             0x06	//  Power down control
#define DC_TLV_ADDR_DAIF            0x07	//  Digital audio interface format
#define DC_TLV_ADDR_SRC             0x08	//  Sample rate control
#define DC_TLV_ADDR_DIA             0x09	//  Digital interface activation
#define DC_TLV_ADDR_RR              0x0F	//  Reset register
// FUNCTION

//==============================================================================
//extern
//local
//IIC
static void sfTLV_IIC_Init(void);
static void sfTLV_IIC_Start(void);
static void sfTLV_IIC_Stop(void);
#define DC_TLV_IIC_DETECTACK    0x01
#define DC_TLV_IIC_NOACK        0x02
#define DC_TLV_IIC_MACK         0x04
static uint8 sfTLV_IIC_ACK(uint8 iCmd);
static void sfTLV_IIC_WriteChar(uint8 iDat);
// static uint8 sfTLV_IIC_ReadChar(void);
//TLV
static int32 sfTLV320_InitChip(void);
// static int32 sfTLV320_ResetChip(void);
static int32 sfTLV320_SendConfig(uint8 iAddr, uint16 iData);
//global

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int32 gfTLV320_Init(void)
@introduction:
    初始化 TLV320AIC23B

@parameter:

@return:


*/
int32 gfTLV320_Init(void)
{
	// int32 tRet = 0;
	sfTLV_IIC_Init();

	// if(sfTLV320_ResetChip())
	// return -1;
	if (sfTLV320_InitChip()) {
		return -1;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  int32 gfTLV320_Set( uint8 iCmd )
@introduction:
    控制 TLV320AIC23B 打开关闭功放和麦克风的通道

@parameter:
    iCmd
        DC_TLV320_SpeakerON
        DC_TLV320_SpeakerOFF
        DC_TLV320_MiCroON
        DC_TLV320_MiCroOFF

@return:
    0   SUCCESS

*/
int32 gfTLV320_Set(uint8 iCmd)
{
	switch (iCmd) {
	case DC_TLV320_SpeakerON:
		if (sfTLV320_SendConfig(DC_TLV_ADDR_LCHVC, 0x01FF)) {
			return -1;
		}
		break;
	case DC_TLV320_SpeakerOFF:
		if (sfTLV320_SendConfig(DC_TLV_ADDR_LCHVC, 0x0100)) {
			return -1;
		}
		break;
	case DC_TLV320_MiCroON:
		if (sfTLV320_SendConfig(DC_TLV_ADDR_PDC, 0x01)) {
			return -1;
		}
		break;
	case DC_TLV320_MiCroOFF:
		if (sfTLV320_SendConfig(DC_TLV_ADDR_PDC, 0x03)) {
			return -1;
		}
		break;
	default:
		break;
	}
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfTLV320_SendConfig(uint8 iAddr,uint16 iData)
@introduction:
    设置 TLV320AIC23B 的值

@parameter:
    iAddr
        TLV320AIC23B 的地址
    iData
        TLV320AIC23B 对应地址的数据

@return:
    0   SUCCESS
    -1  FAIL
*/
static int32 sfTLV320_SendConfig(uint8 iAddr, uint16 iData)
{
	uint8 tM1, tM2;
	tM1 = (iAddr << 1) + (iData & 0x100);
	tM2 = iData & 0xFF;
	sfTLV_IIC_Start();
	sfTLV_IIC_WriteChar(DC_TLV_ADDR_CHIP);
	if (sfTLV_IIC_ACK(DC_TLV_IIC_DETECTACK)) {
		return -1;
	}
	sfTLV_IIC_WriteChar(tM1);
	if (sfTLV_IIC_ACK(DC_TLV_IIC_DETECTACK)) {
		return -1;
	}
	sfTLV_IIC_WriteChar(tM2);
	if (sfTLV_IIC_ACK(DC_TLV_IIC_DETECTACK)) {
		return -1;
	}
	sfTLV_IIC_Stop();
	return 0;
}

//---------- ---------- ---------- ----------
/*  static int32 sfTLV320_InitChip(void)
@introduction:
    配置初始设置

@parameter:
    void

@return:
    0   SUCCESS
    -1  FAIL
*/
static int32 sfTLV320_InitChip(void)
{

	if (sfTLV320_SendConfig(DC_TLV_ADDR_LCHVC, 0x01FF)) {
		return -1;
	}

	if (sfTLV320_SendConfig(DC_TLV_ADDR_RCHVC, 0x00FF)) {
		return -1;
	}

	if (sfTLV320_SendConfig(DC_TLV_ADDR_AAPC, 0x015)) {
		return -1;
	}

	if (sfTLV320_SendConfig(DC_TLV_ADDR_DAPC, 0x00)) {
		return -1;
	}

	if (sfTLV320_SendConfig(DC_TLV_ADDR_PDC, 0x01)) {
		return -1;
	}

	if (sfTLV320_SendConfig(DC_TLV_ADDR_DAIF, 0x42)) {
		return -1;
	}
	//32 K
	//if( sfTLV320_SendConfig( DC_TLV_ADDR_SRC, 0x18 ) )
	// return -1;
	// 8K
	if (sfTLV320_SendConfig(DC_TLV_ADDR_SRC, 0x0C)) {
		return -1;
	}

	if (sfTLV320_SendConfig(DC_TLV_ADDR_DIA, 0x01)) {
		return -1;
	}
	printk("------------------------------------\r\n");
	return 0;
}

//==============================================================================
//Others
// ============================ TLV IIC Driver ===============================//
#define DC_PCF_SCL  0
#define DC_PCF_SDA  1

#define DF_SET_SDA_OUT      gClassHAL.Pin->prfSetDirection(&sPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetOUT)
#define DF_SET_SDA_IN       gClassHAL.Pin->prfSetDirection(&sPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetIN)
#define DF_SET_SDA_HIGHT    gClassHAL.Pin->prfSetExport(&sPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetHIGH)
#define DF_SET_SDA_LOW      gClassHAL.Pin->prfSetExport(&sPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetLOW)
#define DF_GET_SDA_VAL      gClassHAL.Pin->prfGetPinValue(&sPCF_PinArr[DC_PCF_SDA])

#define DF_SET_SCL_OUT      gClassHAL.Pin->prfSetDirection(&sPCF_PinArr[DC_PCF_SCL],DC_HAL_PIN_SetOUT)
#define DF_SET_SCL_HIGHT    gClassHAL.Pin->prfSetExport(&sPCF_PinArr[DC_PCF_SCL],DC_HAL_PIN_SetHIGH)
#define DF_SET_SCL_LOW      gClassHAL.Pin->prfSetExport(&sPCF_PinArr[DC_PCF_SCL],DC_HAL_PIN_SetLOW)

static mGPIOPinIfo sPCF_PinArr[] = {
	{3, 4, 0, 0},
	{3, 3, 0, 0},
};

// 保证被次操作结尾都给该操作缓冲时间，缓冲时间为 3us
static void sfTLV_IIC_Init(void)
{
	DF_SET_SDA_OUT;
	DF_SET_SCL_OUT;
	DF_SET_SDA_HIGHT;
	DF_SET_SCL_HIGHT;
}

static void sfTLV_IIC_Start(void)
{
	DF_SET_SDA_OUT;
	DF_SET_SDA_HIGHT;
	DF_SET_SCL_HIGHT;
	udelay(3);
	DF_SET_SDA_LOW;
	udelay(3);
	DF_SET_SCL_LOW;
	udelay(3);
}

static void sfTLV_IIC_Stop(void)
{
	DF_SET_SCL_LOW;
	DF_SET_SDA_OUT;
	DF_SET_SDA_LOW;
	udelay(3);
	DF_SET_SDA_HIGHT;
	udelay(3);
	DF_SET_SCL_HIGHT;
}

static uint8 sfTLV_IIC_ACK(uint8 iCmd)
{
	uint8 tRet = 0;
	uint8 _i;

	if (iCmd & DC_TLV_IIC_DETECTACK) {
		DF_SET_SDA_IN;
		udelay(3);
		DF_SET_SCL_HIGHT;
		_i = 10;
		do {
			if (_i == 0) {
				tRet = 1;
				break;
			}
			udelay(1);
			_i--;
		} while ((DF_GET_SDA_VAL); DF_SET_SCL_LOW;
	} else if (iCmd & DC_TLV_IIC_NOACK) {
		DF_SET_SDA_IN;
		udelay(3);
		DF_SET_SCL_HIGHT;
		udelay(3);
		DF_SET_SCL_LOW;
	} else if (iCmd & DC_TLV_IIC_MACK) {
		DF_SET_SDA_OUT;
		DF_SET_SDA_LOW;
		udelay(3);
		DF_SET_SCL_HIGHT;
		udelay(3);
		DF_SET_SCL_LOW;
	}
	udelay(3);
	return tRet;
}

static void sfTLV_IIC_WriteChar(uint8 iDat)
{
	uint8 _i;
	DF_SET_SDA_OUT;
	udelay(3);
	for (_i = 0x80; _i > 0;) {
		if (iDat & _i) {
			DF_SET_SDA_HIGHT;
		} else {
			DF_SET_SDA_LOW;
		}
		_i >>= 1;
		udelay(1);
		DF_SET_SCL_HIGHT;
		udelay(1);
		DF_SET_SCL_LOW;
		udelay(1);
	}
}

/*
static uint8 sfTLV_IIC_ReadChar(void){
	uint8 tRet = 0;
	uint8 _i;
    DF_SET_SDA_IN;
    udelay(3);

	for( _i=0; _i<8; _i++ ){
		DF_SET_SCL_HIGHT;
        udelay(2);

		tRet <<= 1;
		if( DF_GET_SDA_VAL )
			tRet |= 0x01;

		DF_SET_SCL_LOW;
        udelay(2);
	}
	return tRet;
}
 */
