/*
Coder:      yan.xu
Date:       2016-2-29

Abstract:

*/
//==============================================================================
// C
// Linux
// local
// remote
#include "HAL.h"
#include "../Tool/String.h"
#include "IIC.h"

//==============================================================================
//extern
//IIC

mGPIOPinIfo gPCF_PinArr[] = {
	{5, 5, 69, 0}
	,
	{5, 6, 70, 0}
	,
};

// ============================ IIC ===============================//
#define DC_PCF_SCL  0
#define DC_PCF_SDA  1

#define DF_SET_SDA_OUT      gClassHAL.Pin->prfSetDirection(&gPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetOUT)
#define DF_SET_SDA_IN       gClassHAL.Pin->prfSetDirection(&gPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetIN)
#define DF_SET_SDA_HIGHT    gClassHAL.Pin->prfSetExport(&gPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetHIGH)
#define DF_SET_SDA_LOW      gClassHAL.Pin->prfSetExport(&gPCF_PinArr[DC_PCF_SDA],DC_HAL_PIN_SetLOW)
#define DF_GET_SDA_VAL      gClassHAL.Pin->prfGetPinValue(&gPCF_PinArr[DC_PCF_SDA])

#define DF_SET_SCL_OUT      gClassHAL.Pin->prfSetDirection(&gPCF_PinArr[DC_PCF_SCL],DC_HAL_PIN_SetOUT)
#define DF_SET_SCL_HIGHT    gClassHAL.Pin->prfSetExport(&gPCF_PinArr[DC_PCF_SCL],DC_HAL_PIN_SetHIGH)
#define DF_SET_SCL_LOW      gClassHAL.Pin->prfSetExport(&gPCF_PinArr[DC_PCF_SCL],DC_HAL_PIN_SetLOW)
#define DF_GET_SCL_VAL      gClassHAL.Pin->prfGetPinValue(&gPCF_PinArr[DC_PCF_SCL])

void IIC_set_SCL_high(void)
{
	int i = 0;
	DF_SET_SCL_HIGHT;
	for (i = 0; i < 10; i++) {
		if (!DF_GET_SCL_VAL) {
			DF_SET_SCL_HIGHT;
			udelay(10);
		} else {
			break;
		}
	}
	if (i == 10) {
		NVCPrint("IIC SCL is busy, Set SCL High failed!");
	}
}

// 保证被次操作结尾都给该操作缓冲时间，缓冲时间为 3us
void IIC_Init(mGPIOPinIfo *pPCF_PinArr)
{
	gPCF_PinArr[0] = pPCF_PinArr[0];
	gPCF_PinArr[1] = pPCF_PinArr[1];
	DF_SET_SDA_OUT;
	udelay(10);
	DF_SET_SCL_OUT;
	udelay(10);
	DF_SET_SDA_HIGHT;
	udelay(10);
	IIC_set_SCL_high();
	udelay(10);
}

void IIC_Start(void)
{
	DF_SET_SDA_OUT;
	udelay(10);
	DF_SET_SDA_HIGHT;
	udelay(10);
	IIC_set_SCL_high();
	udelay(10);
	DF_SET_SDA_LOW;		// 在时钟线为高电平时，拉低数据线，产生起始信号
	udelay(10);
	DF_SET_SCL_LOW;
	udelay(10);
}

void IIC_Stop(void)
{
	DF_SET_SCL_LOW;
	udelay(10);
	DF_SET_SDA_OUT;
	udelay(10);
	DF_SET_SDA_LOW;
	udelay(10);
	IIC_set_SCL_high();
	udelay(10);
	DF_SET_SDA_HIGHT;	// 时钟时线为高电平时，拉高数据线，产生停止信号
	udelay(10);
}

uint8 IIC_ACK(uint8 iCmd)
{
	uint8 tRet = 0;
	uint8 _i;

	if (iCmd & DC_IIC_DETECTACK) {
		DF_SET_SDA_IN;
		udelay(10);
		DF_SET_SCL_LOW;
		udelay(10);
		DF_SET_SDA_HIGHT;
		udelay(10);
		IIC_set_SCL_high();
		_i = 10;
		do {
			if (_i == 0) {
				tRet = 1;
				break;
			}
			//NVCPrint("[%d]: %d", _i, DF_GET_SDA_VAL);
			udelay(10);
			_i--;
		} while (DF_GET_SDA_VAL);
		DF_SET_SCL_LOW;

	} else if (iCmd & DC_IIC_NOACK) {
		DF_SET_SDA_IN;
		udelay(10);
		IIC_set_SCL_high();
		udelay(10);
		DF_SET_SCL_LOW;

	} else if (iCmd & DC_IIC_MACK) {
		DF_SET_SDA_OUT;
		udelay(10);
		DF_SET_SDA_LOW;
		udelay(10);
		IIC_set_SCL_high();
		udelay(10);
		DF_SET_SCL_LOW;
	}
	udelay(10);
	return tRet;
}

void IIC_WriteChar(uint8 iDat)
{
	uint8 _i;

	local_irq_disable();

	DF_SET_SDA_OUT;
	udelay(10);

	for (_i = 0; _i < 8; _i++) {
		if (iDat & 0x80) {
			DF_SET_SDA_HIGHT;
		} else {
			DF_SET_SDA_LOW;
		}
		udelay(10);
		IIC_set_SCL_high();
		udelay(10);
		DF_SET_SCL_LOW;
		udelay(10);
		iDat <<= 1;
	}
	local_irq_enable();
}

uint8 IIC_ReadChar(void)
{
	uint8 tRet = 0;
	uint8 _i;

	local_irq_disable();
	DF_SET_SDA_IN;
	udelay(10);
	for (_i = 0; _i < 8; _i++) {
		IIC_set_SCL_high();
		udelay(10);

		tRet <<= 1;
		if (DF_GET_SDA_VAL) {
			tRet |= 0x01;
		}
		DF_SET_SCL_LOW;
		udelay(10);
	}
	local_irq_enable();
	udelay(10);

	return tRet;
}
