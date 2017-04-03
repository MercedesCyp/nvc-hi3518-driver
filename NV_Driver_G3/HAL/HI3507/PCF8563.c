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

//=============================================================================
// MACRO
// CONSTANT
// Chip Addr
#define DC_PCF_ADDR_CHIP         0xA2
#define DC_PCF_ADDR_Ctrl1        0x00
#define DC_PCF_OPT_STOP          0x20
// Chip config Addr
#define DC_PCF_ADDR_Ctrl2        0x01
#define DC_PCF_ADDR_VL_Second    0x02
#define DC_PCF_ADDR_Minute       0x03
#define DC_PCF_ADDR_hour         0x04
#define DC_PCF_ADDR_Day          0x05
#define DC_PCF_ADDR_Weekday      0x06
#define DC_PCF_ADDR_Mmonth       0x07
#define DC_PCF_ADDR_Year         0x08
#define DC_PCF_ADDR_ala_Minute   0x09
#define DC_PCF_ADDR_ala_Hour     0x0A
#define DC_PCF_ADDR_ala_Day      0x0B
#define DC_PCF_ADDR_ala_Weekday  0x0C
#define DC_PCF_ADDR_clk_CTRL     0x0D
#define DC_PCF_ADDR_timer_CTRL   0x0E
#define DC_PCF_ADDR_timer        0x0F
// FUNCTION

//==============================================================================
//extern
//local
static void sfPCF_IIC_Init(void);
static void sfPCF_IIC_Start(void);
static void sfPCF_IIC_Stop(void);
#define DC_PCF_IIC_DETECTACK    0x01
#define DC_PCF_IIC_NOACK        0x02
#define DC_PCF_IIC_MACK         0x04
static uint8 sfPCF_IIC_ACK(uint8 iCmd);
static void sfPCF_IIC_WriteChar(uint8 iDat);
static uint8 sfPCF_IIC_ReadChar(void);
//global

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int32 gfPCF8563_INIT(void)
@introduction:
    初始化 PCF8563

@parameter:
    void

@return:
    0   Success

*/
int32 gfPCF8563_INIT(void)
{
	sfPCF_IIC_Init();
	return 0;
}

//---------- ---------- ---------- ----------
/*  int32 gfPCF8563_GetTime( mTime_Struct *iData )
@introduction:
    从 PCF8563 中获取时间

@parameter:
    iData
        返回时间

@return:
    0x00000000          nomarl
    0x00000001  VL      low power
    0x00000002  C       year to 0 from 99
*/
int32 gfPCF8563_GetTime(mTime_Struct *iData)
{
	int32 tRet = 0;

	sfPCF_IIC_Start();
	sfPCF_IIC_WriteChar(DC_PCF_ADDR_CHIP);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);
	sfPCF_IIC_WriteChar(DC_PCF_ADDR_VL_Second);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);
	sfPCF_IIC_Stop();

	sfPCF_IIC_Start();
	sfPCF_IIC_WriteChar(DC_PCF_ADDR_CHIP | DC_IIC_READ);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);
	// iData->aSecond   = sfPCF_IIC_ReadChar() & 0x7F ;
	iData->aSecond = sfPCF_IIC_ReadChar();
	sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	iData->aMinute = sfPCF_IIC_ReadChar() & 0x7F;
	sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	iData->aHour = sfPCF_IIC_ReadChar() & 0x3F;
	sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	iData->aDay = sfPCF_IIC_ReadChar() & 0x3F;
	sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	iData->aWeekday = sfPCF_IIC_ReadChar() & 0x07;
	sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	// iData->aMonth    = sfPCF_IIC_ReadChar() & 0x1F ;
	iData->aMonth = sfPCF_IIC_ReadChar();
	sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	// iData->aYear     = sfPCF_IIC_ReadChar() & 0xFF ;
	iData->aYear = sfPCF_IIC_ReadChar();
	// sfPCF_IIC_ACK(DC_PCF_IIC_MACK);
	sfPCF_IIC_Stop();

	if (iData->aSecond & 0x80) {
		tRet |= 0x00000001;
		iData->aSecond &= 0x7F;
	}
	if (iData->aMonth & 0x80) {
		tRet |= 0x00000002;
		iData->aMonth &= 0x1F;
	}
	return tRet;
}

//---------- ---------- ---------- ----------
/*  void gfPCF8563_SetTime( mTime_Struct *iData )
@introduction:
    设置修正 PCF8563 芯片的时间

@parameter:
    iData
        设置的时间

@return:
    void

*/
void gfPCF8563_SetTime(mTime_Struct *iData)
{

	sfPCF_IIC_Start();
	sfPCF_IIC_WriteChar(DC_PCF_ADDR_CHIP);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(DC_PCF_ADDR_Ctrl1);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(DC_PCF_OPT_STOP);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(0x00);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aSecond);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aMinute);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aHour);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aDay);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aWeekday);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aMonth);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(iData->aYear);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);
	sfPCF_IIC_Stop();

	sfPCF_IIC_Start();
	sfPCF_IIC_WriteChar(DC_PCF_ADDR_CHIP);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(DC_PCF_ADDR_Ctrl1);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_WriteChar(0x00);
	sfPCF_IIC_ACK(DC_PCF_IIC_DETECTACK);

	sfPCF_IIC_Stop();

}

//---------- ---------- ---------- ----------
/*  void gfPCF8563_ShowTime( mTime_Struct *iData )
@introduction:
    显示时间 ———— 用于调试

@parameter:
    iData
        显示的时间

@return:
    void

*/
#if DEBUG_PCF8563
void gfPCF8563_ShowTime(mTime_Struct *iData)
{
	NVCPrint
	("======================================================================");
	NVCPrint("Second:\t%x", (unsigned int)iData->aSecond);
	NVCPrint("Minute:\t%x", (unsigned int)iData->aMinute);
	NVCPrint("Hour:\t%x", (unsigned int)iData->aHour);
	NVCPrint("Days:\t%x", (unsigned int)iData->aDay);
	NVCPrint("Weekday:%x", (unsigned int)iData->aWeekday);
	NVCPrint("Month:\t%x", (unsigned int)iData->aMonth);
	NVCPrint("Year:\t%x", (unsigned int)iData->aYear);
	NVCPrint
	("======================================================================");
}
#endif

//==============================================================================
//Others
// ============================ PCF IIC Driver ===============================//
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
	{6, 1, 0, 0},
	{6, 2, 0, 0},
};

// 保证被次操作结尾都给该操作缓冲时间，缓冲时间为 3us
static void sfPCF_IIC_Init(void)
{
	DF_SET_SDA_OUT;
	DF_SET_SCL_OUT;
	DF_SET_SDA_HIGHT;
	DF_SET_SCL_HIGHT;
}

static void sfPCF_IIC_Start(void)
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

static void sfPCF_IIC_Stop()
{
	DF_SET_SCL_LOW;
	DF_SET_SDA_OUT;
	DF_SET_SDA_LOW;
	udelay(3);
	DF_SET_SDA_HIGHT;
	udelay(3);
	DF_SET_SCL_HIGHT;
}

static uint8 sfPCF_IIC_ACK(uint8 iCmd)
{
	uint8 tRet = 0;
	uint8 _i;

	if (iCmd & DC_PCF_IIC_DETECTACK) {
		DF_SET_SDA_IN;
		udelay(3);
		DF_SET_SCL_HIGHT;
		_i = 10;
		do {
			udelay(1);
			_i--;
		} while ((_i == 0) || (DF_GET_SDA_VAL));
		if (_i == 0) {
			tRet = 1;
		}
		DF_SET_SCL_LOW;

	} else if (iCmd & DC_PCF_IIC_NOACK) {
		DF_SET_SDA_IN;
		udelay(3);
		DF_SET_SCL_HIGHT;
		udelay(3);
		DF_SET_SCL_LOW;

	} else if (iCmd & DC_PCF_IIC_MACK) {
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

static void sfPCF_IIC_WriteChar(uint8 iDat)
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

static uint8 sfPCF_IIC_ReadChar(void)
{
	uint8 tRet = 0;
	uint8 _i;
	DF_SET_SDA_IN;
	udelay(3);

	for (_i = 0; _i < 8; _i++) {
		DF_SET_SCL_HIGHT;
		udelay(2);

		tRet <<= 1;
		if (DF_GET_SDA_VAL) {
			tRet |= 0x01;
		}

		DF_SET_SCL_LOW;
		udelay(2);
	}
	return tRet;
}
