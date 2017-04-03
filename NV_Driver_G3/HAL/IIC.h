#ifndef __IIC_H
#define __IIC_H
#include "../GlobalParameter.h"

extern void IIC_Init(mGPIOPinIfo *pPCF_PinArr);
extern void IIC_Start(void);
extern void IIC_Stop(void);
#define DC_IIC_DETECTACK    0x01
#define DC_IIC_NOACK        0x02
#define DC_IIC_MACK         0x04
extern uint8 IIC_ACK(uint8 iCmd);
extern void IIC_WriteChar(uint8 iDat);
extern uint8 IIC_ReadChar(void);

extern mGPIOPinIfo gPCF_PinArr[];

void IIC_set_SCL_high(void);

#endif
