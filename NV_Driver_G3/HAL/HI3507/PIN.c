/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:

*/
//==============================================================================
// C
// Linux
// local
#include "HI3507_HAL.h"
// remote
#include "../HAL.h"

//=============================================================================
// DATA TYPE
typedef struct {
	void (*aHandler)(void);
	void *aNext;
} mPINInt_Ifo;

//==============================================================================
//extern
//local
static void sfPin_SetMuxCTRLToGPIO(void *ipPinInfo);
static void sfPin_SetDirection(void *ipPinInfo, uint8 iInOut);
static void sfPin_SetExport(void *ipPinInfo, uint8 iHighLow);
static uint32 sPin_GetValue(void *ipPinInfo);
static uint32 sfPin_WhichGrp(void *ipPinInfo);
static uint32 sPin_RegistInterrupt(uint32 iGrp, void (*iHandler)(void));
static uint32 sfPin_CancelRegistInterrupt(uint32 iGrp, void (*iHandler)(void));
static uint32 sfPin_IsIntExist(void *ipPinInfo);
static void sPin_ConfigInt(void *ipPinInfo, uint8 iCmd);

static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id,
        struct pt_regs *regs);
static irqreturn_t intf_GPIO_Grp2Function(int irq, void *dev_id,
        struct pt_regs *regs);
static irqreturn_t intf_GPIO_Grp3Function(int irq, void *dev_id,
        struct pt_regs *regs);
//global

//==============================================================================
//extern
//local
static mPINInt_Ifo *sPinIntLink[3] = { NULL, NULL, NULL };

//global
mClass_Pin const gcoClass_Pin = {
	.prfSetDirection = sfPin_SetDirection,
	.prfSetExport = sfPin_SetExport,
	.prfGetPinValue = sPin_GetValue,
	.prfWhichGrp = sfPin_WhichGrp,
	.prfPin_RegistInt = sPin_RegistInterrupt,
	.prfPin_CancelRegistInt = sfPin_CancelRegistInterrupt,
	// .prfClrConfig           = sfPin_ClrIntConfig,
	.prfPin_ConfigInt = sPin_ConfigInt,
	.prfIsIntStateExist = sfPin_IsIntExist
};

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  static void sfPin_SetMuxCTRLToGPIO(void *ipPinInfo)
@introduction:
    根据传进来的引脚信息，设置引脚的功能为 GPIO（普通输入输出模式）

@parameter:
    ipPinInfo
        指向引脚信息的指针

@return:
    void

*/
static void sfPin_SetMuxCTRLToGPIO(void *ipPinInfo)
{
	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;
	uint32 tBIT = 0;
	uint8 tValue = 0;

	if (tPinTool->aGroup == 4) {
		tValue = 1;
		tBIT = HAL_BIT_MUXCTRL_VI0_DATA;
	} else if (tPinTool->aGroup == 5) {
		tValue = 1;
		tBIT = HAL_BIT_MUXCTRL_VI3_DATA;
	} else if (tPinTool->aGroup == 0) {
		if ((tPinTool->aBit == 0) || (tPinTool->aBit == 1)) {
			tBIT = HAL_BIT_MUXCTRL_VI0_CLK;
		} else if ((tPinTool->aBit == 2) || (tPinTool->aBit == 3)
		           || (tPinTool->aBit == 4)) {
			tBIT = HAL_BIT_MUXCTRL_SIO1;
		} else if ((tPinTool->aBit == 5) || (tPinTool->aBit == 6)) {
			tBIT = HAL_BIT_MUXCTRL_VI2_CLK;
		} else {
			tBIT = HAL_BIT_MUXCTRL_DDR2;
		}
	} else if (tPinTool->aGroup == 1) {
		if (tPinTool->aBit == 2) {
			tValue = 1;
			tBIT = HAL_BIT_MUXCTRL_SMI_EBIADR24;
		} else {
			if (tPinTool->aBit == 0) {
				tBIT = HAL_BIT_MUXCTRL_DDR2;
			} else if (tPinTool->aBit == 1) {
				tBIT = HAL_BIT_MUXCTRL_SMI_EBICS1N;
			} else if (tPinTool->aBit == 7) {
				tBIT = HAL_BIT_MUXCTRL_PCI;
			} else {
				tBIT = HAL_BIT_MUXCTRL_MMC;
			}
		}
	} else if (tPinTool->aGroup == 2) {
		if (tPinTool->aBit == 3) {
			tValue = 1;
			tBIT = HAL_BIT_MUXCTRL_IR;
		} else {
			if (tPinTool->aBit == 0) {
				tBIT = HAL_BIT_MUXCTRL_PCI;
			} else if ((tPinTool->aBit == 1)
			           || (tPinTool->aBit == 2)) {
				tBIT = HAL_BIT_MUXCTRL_MMC;
			} else {
				tBIT = HAL_BIT_MUXCTRL_UART1;
			}
		}
	} else if (tPinTool->aGroup == 3) {
		if (tPinTool->aBit == 5) {
			tBIT = HAL_BIT_MUXCTRL_SIO0_ACKOUT;
		} else {
			tValue = 1;
			if ((tPinTool->aBit == 3) || (tPinTool->aBit == 4)) {
				tBIT = HAL_BIT_MUXCTRL_I2C;
			} else if ((tPinTool->aBit == 6)
			           || (tPinTool->aBit == 7)) {
				tBIT = HAL_BIT_MUXCTRL_VI2_DATA;
			} else {
				tBIT = HAL_BIT_MUXCTRL_ETH;
			}
		}
	} else if (tPinTool->aGroup == 6) {
		if (tPinTool->aBit == 2) {
			tBIT = HAL_BIT_MUXCTRL_SIO0_XFS;
		} else if (tPinTool->aBit == 3) {
			tBIT = HAL_BIT_MUXCTRL_SIO0_XCK;
		} else if ((tPinTool->aBit == 0) || (tPinTool->aBit == 1)) {
			tBIT = HAL_BIT_MUXCTRL_SPI;
		} else {
			tValue = 1;
			tBIT = HAL_BIT_MUXCTRL_VI1_DATA;
		}
	} else if (tPinTool->aGroup == 7) {
		tValue = 1;
		if (tPinTool->aBit == 4) {
			tBIT = HAL_BIT_MUXCTRL_SMI_EBIRDYN;
		} else if (tPinTool->aBit == 7) {
			tBIT = HAL_BIT_MUXCTRL_VI2_DATA;
		} else if ((tPinTool->aBit == 5) || (tPinTool->aBit == 6)) {
			tBIT = HAL_BIT_MUXCTRL_SPI;
		} else {
			tBIT = HAL_BIT_MUXCTRL_VI1_DATA;
		}
	}
	{
		uint32 tRegVal = HAL_readl(HAL_ADDR_IOMUXCTRL);
		// NVCPrint("tBIT:%d tValue:%d tRegVal:%d",(int)tBIT,(int)tValue,(int)tRegVal);
		if (tValue) {
			tRegVal |= tBIT;
		} else {
			tRegVal &= ~tBIT;
		}

		HAL_writel(tRegVal, HAL_ADDR_IOMUXCTRL);
	}
}

//---------- ---------- ---------- ----------
/*  static void sfPin_SetDirection( void *ipPinInfo , uint8 iInOut )
@introduction:
    设置引脚的输入输出方向

@parameter:
    ipPinInfo
        指向引脚信息的指针
    iInOut
        DC_HAL_PIN_SetOUT
            设置为输出
        DC_HAL_PIN_SetIN
            设置为输入

@return:
    void


*/
static void sfPin_SetDirection(void *ipPinInfo, uint8 iInOut)
{
	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;

	sfPin_SetMuxCTRLToGPIO(ipPinInfo);
	if (iInOut & DC_HAL_PIN_SetOUT) {
		DF_Set_GPIO_DIRx(tPinTool->aGroup, tPinTool->aBit);
	} else if (iInOut & DC_HAL_PIN_SetIN) {
		DF_Clr_GPIO_DIRx(tPinTool->aGroup, tPinTool->aBit);
	} else {
		return;
	}
}

//---------- ---------- ---------- ----------
/*  static void sfPin_SetExport( void *ipPinInfo, uint8 iHighLow)
@introduction:
    设置引脚输出电平状态

@parameter:
    ipPinInfo
        指向引脚信息的指针
    iHighLow
        DC_HAL_PIN_SetLOW
            设置引脚状态为低
        DC_HAL_PIN_SetHIGH
            设置引脚状态为低
        DC_HAL_PIN_SetSWITCH
            设置引脚状态翻转

@return:
    void

*/
static void sfPin_SetExport(void *ipPinInfo, uint8 iHighLow)
{
	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;
	if (iHighLow & DC_HAL_PIN_SetLOW) {
		DF_Clr_GPIO_Outx(tPinTool->aGroup, tPinTool->aBit);
	} else if (iHighLow & DC_HAL_PIN_SetHIGH) {
		DF_Set_GPIO_Outx(tPinTool->aGroup, tPinTool->aBit);
	} else if (iHighLow & DC_HAL_PIN_SetSWITCH) {
		DF_Switch_GPIO_Outx(tPinTool->aGroup, tPinTool->aBit);
	}

}

//---------- ---------- ---------- ----------
/*  static uint32 sPin_GetValue( void *ipPinInfo )
@introduction:
    获取引脚当前引脚的状态

@parameter:
    ipPinInfo
        指向引脚信息的指针

@return:
    1   引脚为高电平
    0   引脚为低电平

*/
static uint32 sPin_GetValue(void *ipPinInfo)
{
	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;
	return (uint32)(DF_Get_GPIO_INx(tPinTool->aGroup, tPinTool->aBit));
}

//---------- ---------- ---------- ----------
/*  static uint32 sfPin_WhichGrp( void *ipPinInfo )
@introduction:
    查询当前引脚所处中断组

@parameter:
    ipPinInfo
        指向引脚信息的指针

@return:
    0   ·   一组
    1   ·   二组
    2   ·   三组

*/
static uint32 sfPin_WhichGrp(void *ipPinInfo)
{
	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;

	return (uint32)(DF_PIN_ClassifyGrp(tPinTool->aGroup));
	// if(( tPinTool->aGroup == 0 )){
	// return DC_HAL_PIN_Grp0;
	// }else if(( tPinTool->aGroup == 1 )){
	// return DC_HAL_PIN_Grp1;
	// }else{
	// return DC_HAL_PIN_Grp2;
	// }
}

//---------- ---------- ---------- ----------
/*  static uint32 sPin_RegistInterrupt( uint32 iGrp, void (*iHandler)(void) )
@introduction:
    注册外部中断服务事件

@parameter:
    iGrp
        中断组
    iHandler
        事件过程

@return:
    DC_HAL_PIN_RetSuccess
        注册成功
    DC_HAL_PIN_RetIntErr
        注册失败
*/
static uint32 sPin_RegistInterrupt(uint32 iGrp, void (*iHandler)(void))
{

	// printk("iGrp:%d\r\n",(int)iGrp);

	if (sPinIntLink[iGrp] == NULL) {
		sPinIntLink[iGrp] =
		    (mPINInt_Ifo *) kmalloc(sizeof(mPINInt_Ifo), GFP_ATOMIC);

		sPinIntLink[iGrp]->aHandler = iHandler;
		sPinIntLink[iGrp]->aNext = NULL;

		if (iGrp == DC_HAL_PIN_Grp0) {
			if (request_irq(IRQ_GPIO0,
			                intf_GPIO_Grp1Function,
			                SA_SHIRQ,
			                "NV_Drv_PinGrp1", &sPinIntLink[0])) {
				return DC_HAL_PIN_RetIntErr;
			}
		} else if (iGrp == DC_HAL_PIN_Grp1) {

			if (request_irq(IRQ_GPIO1,
			                intf_GPIO_Grp2Function,
			                SA_SHIRQ,
			                "NV_Drv_PinGrp2", &sPinIntLink[1])) {
				return DC_HAL_PIN_RetIntErr;
			}
		} else if (iGrp == DC_HAL_PIN_Grp2) {
			if (request_irq(IRQ_GPIOetc,
			                intf_GPIO_Grp3Function,
			                SA_SHIRQ,
			                "NV_Drv_PinGrp3", &sPinIntLink[2])) {
				return DC_HAL_PIN_RetIntErr;
			}
		}

	} else {
		mPINInt_Ifo *tLinkTools = sPinIntLink[iGrp];
		mPINInt_Ifo *tLinkTools_2;

		for (;;) {
			if (tLinkTools->aNext == NULL) {
				break;
			} else {
				tLinkTools = tLinkTools->aNext;
			}
		}

		tLinkTools_2 =
		    (mPINInt_Ifo *) kmalloc(sizeof(mPINInt_Ifo), GFP_ATOMIC);

		tLinkTools_2->aHandler = iHandler;
		tLinkTools_2->aNext = NULL;

		tLinkTools->aNext = tLinkTools_2;

		// 初始化 IO 中断
	}
	return DC_HAL_PIN_RetSuccess;
}

//---------- ---------- ---------- ----------
/*  static uint32 sfPin_CancelRegistInterrupt( uint32 iGrp, void (*iHandler)(void) )
@introduction:
    根据用户注册的函数句柄判断是否存在注册函数，然后在该链表中取消注册
    注意当链表删空了之后，该函数会自动取消引脚的的中断注册函数

@parameter:
    iGrp
        注册方法所属组
    iHandler
        注册方法的句柄

@return:
    -1
        所属链中不存在该句柄的注册点
    0
        取消成功

*/
static uint32 sfPin_CancelRegistInterrupt(uint32 iGrp, void (*iHandler)(void))
{
	mPINInt_Ifo **tLinkTools_1;
	mPINInt_Ifo *tLinkTools_2;

	if (sPinIntLink[iGrp] == NULL) {
		return -1;
	}

	tLinkTools_1 = &sPinIntLink[iGrp];
	tLinkTools_2 = sPinIntLink[iGrp];

	for (;;)
		if ((tLinkTools_2 != NULL)
		    || (tLinkTools_2->aHandler == iHandler)) {
			break;
		} else {
			tLinkTools_1 =
			    (mPINInt_Ifo **)(&(tLinkTools_2->aNext));
			tLinkTools_2 = tLinkTools_2->aNext;
		}

	if (tLinkTools_2 == NULL) {
		return -1;
	}

	*tLinkTools_1 = tLinkTools_2->aNext;
	kfree(tLinkTools_2);

	if (sPinIntLink[iGrp] == NULL) {
		uint8 irq_num[3] = { IRQ_GPIO0, IRQ_GPIO1, IRQ_GPIOetc };
		free_irq(irq_num[iGrp], &sPinIntLink[iGrp]);
	}

	return 0;
}

//---------- ---------- ---------- ----------
/*  static uint32 sfPin_IsIntExist( void *ipPinInfo )
@introduction:
    根据引脚信息，查询当前引脚是否产生中断

@parameter:
    ipPinInfo
        指向引脚信息的指针

@return:
    1 产生中断
    0 未产生
*/
static uint32 sfPin_IsIntExist(void *ipPinInfo)
{
	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;
	// uint32 tRet;
	// tRet = DC_Get_GPIO_RIS( tPinTool->aGroup, tPinTool->aBit );
	// return tRet;
	return DC_Get_GPIO_RIS(tPinTool->aGroup, tPinTool->aBit);
}

//---------- ---------- ---------- ----------
/*  static void sPin_ConfigInt( void *ipPinInfo , uint8 iCmd )
@introduction:
    配置引脚关于中断相关功能

@parameter:
    ipPinInfo
        指向引脚信息的指针
    iCmd
        DC_HAL_PIN_IntDisable
            禁能中断
        DC_HAL_PIN_IntIN
            设置为输入
        DC_HAL_PIN_IntHighLevel
            设置高电平中断
        DC_HAL_PIN_IntLowLevel
            设置低电平中断
        DC_HAL_PIN_IntRiseEdge
            设置上升沿中断
        DC_HAL_PIN_IntFallEdge
            设置下降沿中断
        DC_HAL_PIN_IntClrState
            清除中断标志位
        DC_HAL_PIN_IntEnable
            使能中断

@return:
    void

*/
static void sPin_ConfigInt(void *ipPinInfo, uint8 iCmd)
{

	mGPIOPinIfo *tPinTool = (mGPIOPinIfo *) ipPinInfo;

	if (iCmd & DC_HAL_PIN_IntDisable) {
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IE(tPinTool->aGroup));
	}

	if (iCmd & DC_HAL_PIN_IntIN) {
		sfPin_SetMuxCTRLToGPIO(tPinTool);
		DF_Clr_GPIO_DIRx(tPinTool->aGroup, tPinTool->aBit);	// Setting IO dirction INPUT
	}

	if (iCmd == DC_HAL_PIN_IntHighLevel) {
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Set_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IEV(tPinTool->aGroup));

	} else if (iCmd == DC_HAL_PIN_IntLowLevel) {
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IEV(tPinTool->aGroup));

	} else if (iCmd == DC_HAL_PIN_IntRiseEdge) {
		DF_Set_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Set_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IEV(tPinTool->aGroup));
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IBE(tPinTool->aGroup));

	} else if (iCmd == DC_HAL_PIN_IntFallEdge) {
		DF_Set_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IEV(tPinTool->aGroup));
		DF_Clr_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IBE(tPinTool->aGroup));
	}

	if (iCmd & DC_HAL_PIN_IntClrState) {
		// Clear Int status
		DC_Clr_GPIO_INT_Sta(tPinTool->aGroup, tPinTool->aBit);
	}

	if (iCmd & DC_HAL_PIN_IntEnable) {
		// Enable interrupt
		DF_Set_GPIO_ADDx(tPinTool->aGroup, tPinTool->aBit,
		                 HAL_GPIOx_IE(tPinTool->aGroup));
	}
}

//---------- ---------- ---------- ----------
/*  static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id ,struct pt_regs * regs)
@introduction:
    想内核注册的第一组的中断服务函数

@parameter:
    详参 linux 内核说明文档

@return:
    详参 linux 内核说明文档

*/
static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id,
        struct pt_regs *regs)
{
	mPINInt_Ifo *tPinTool_1 = sPinIntLink[0];

	for (; tPinTool_1 != NULL;) {
		if (tPinTool_1->aHandler != NULL) {
			tPinTool_1->aHandler();
		}
		tPinTool_1 = tPinTool_1->aNext;
	}
	return IRQ_HANDLED;
	// return 0;
}

//---------- ---------- ---------- ----------
/*  asjkdaskdjasdk
@introduction:
    想内核注册的第二组的中断服务函数

@parameter:
    详参 linux 内核说明文档

@return:
    详参 linux 内核说明文档

*/
static irqreturn_t intf_GPIO_Grp2Function(int irq, void *dev_id,
        struct pt_regs *regs)
{
	mPINInt_Ifo *tPinTool_1 = sPinIntLink[1];

	for (; tPinTool_1 != NULL;) {
		if (tPinTool_1->aHandler != NULL) {
			tPinTool_1->aHandler();
		}
		tPinTool_1 = tPinTool_1->aNext;
	}
	return IRQ_HANDLED;
	// return 0;
}

//---------- ---------- ---------- ----------
/*  asjkdaskdjasdk
@introduction:
    想内核注册的第三组的中断服务函数

@parameter:
    详参 linux 内核说明文档

@return:
    详参 linux 内核说明文档

*/
static irqreturn_t intf_GPIO_Grp3Function(int irq, void *dev_id,
        struct pt_regs *regs)
{
	mPINInt_Ifo *tPinTool_1 = sPinIntLink[2];

	for (; tPinTool_1 != NULL;) {
		if (tPinTool_1->aHandler != NULL) {
			tPinTool_1->aHandler();
		}
		tPinTool_1 = tPinTool_1->aNext;
	}
	return IRQ_HANDLED;
	// return 0;
}
