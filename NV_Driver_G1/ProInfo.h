#ifndef __ProInfo_H
#define __ProInfo_H

//  -------------------------------> Include file
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
#include "GlobalParameter.h"
#include "String.h"
#include <linux/slab.h>



//  -------------------------------> The Product macro
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
#define DC_GLOBAL_DriverVersion "Beta: 0.1"
//#define DC_GLOBAL_DriverVersion "Release: 0.2"
#define DC_GLOBAL_BuildData 	"Build By maj."

// extern uint16 gGLOBAL_ChipType;
// extern uint16 gGLOBAL_DeviceNum; 
//extern uint32 gInsmodProType;
#define DC_DrivNumErrMask 0x80000000
#define DF_GLOBAL_GetChipInfo	((gInsmodProType>>12)&0xFF)
#define DF_GLOBAL_GetDivInfo	(gInsmodProType&0xFFF)
#define DF_GLOBAL_GetDivCmd		((gInsmodProType>>20)&0xFF)



//Chip ID
#define DC_ChipSum_Num 2
#define DC_CHIP_3518C_name		"3518C"
#define DC_CHIP_3518E_name		"3518E"

#define DC_CHIP_3518C_Appname	0x3518C
#define DC_CHIP_3518E_Appname	0x3518E

#define DC_CHIP_3518C			0x00
#define DC_CHIP_3518E			0x01


// product ID
#define DC_ProSum_Num	8
#define DC_Pro1_Name	"D01"
#define DC_Pro2_Name	"D03"
#define DC_Pro3_Name	"D04"
#define DC_Pro4_Name	"D11"
#define DC_Pro5_Name	"F07"
#define DC_Pro6_Name	"F02"
#define DC_Pro7_Name	"F05"
#define DC_Pro8_Name	"F08"

#define DC_Pro1_AppName 0xD01
#define DC_Pro2_AppName 0xD03
#define DC_Pro3_AppName 0xD04
#define DC_Pro4_AppName 0xD11
#define DC_Pro5_AppName 0xF07
#define DC_Pro6_AppName 0xF02
#define DC_Pro7_AppName 0xF05
#define DC_Pro8_AppName	0xF08


#define DC_Pro_D01	0x0000
#define DC_Pro_D03	0x0001
#define DC_Pro_D04	0x0002
#define DC_Pro_D11	0x0003
#define DC_Pro_F07	0x0004
#define DC_Pro_F02	0x0005
#define DC_Pro_F05	0x0006
#define DC_Pro_F08	0x0007


#define DC_ProCmdSum_Num 	2
#define DC_ProCmd1_AppName	"OFF"
#define DC_ProCmd2_AppName	"ON"


extern uint32 pagAppChipType[DC_ChipSum_Num];
extern uint32 pagAppProType[DC_ProSum_Num];


//Products capability
#define	NVC_SUPP_ButtonMonitor 	 0x00000001	// 按键检测功能
#define	NVC_SUPP_DNMonitor		 0x00000002	// 环境监测功能
#define	NVC_SUPP_IfrFilter		 0x00000004	// 滤光片切换
#define	NVC_SUPP_IfrLight		 0x00000008	// 红外灯切换
#define	NVC_SUPP_DoubleLens		 0x00000010	// 双镜头切换
#define	NVC_SUPP_StateLED 		 0x00000020	// 状态灯切换
#define	NVC_SUPP_PanTilt 		 0x00000040	// 云台控制
#define	NVC_SUPP_NightTLight	 0x00000080	// 小夜灯
#define	NVC_SUPP_CoolFan		 0x00000100	// 散热风扇
#define	NVC_SUPP_AudioPlug		 0x00000200	// 音频开关
#define	NVC_SUPP_TempMonitor	 0x00000400	// 温度检测
#define	NVC_SUPP_HumiMonitor	 0x00000800	// 湿度检测
#define	NVC_SUPP_ResetIO		 0x00001000	// 复位IO





//0x0000022f
#define DC_CAP_D01 \
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_AudioPlug)

//0x000002Ef
#define DC_CAP_D03	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_PanTilt|\
NVC_SUPP_NightTLight|\
NVC_SUPP_AudioPlug|\
NVC_SUPP_TempMonitor|\
NVC_SUPP_HumiMonitor\
)

//0x0000022f
#define DC_CAP_D04	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_AudioPlug\
)

//0x000007fb
#define DC_CAP_D11	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_DoubleLens|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_PanTilt|\
NVC_SUPP_NightTLight|\
NVC_SUPP_CoolFan|\
NVC_SUPP_AudioPlug|\
NVC_SUPP_TempMonitor\
)

//0x00000000
#define DC_CAP_F07	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_AudioPlug\
)


#define DC_CAP_F02	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_AudioPlug\
)

#define DC_CAP_F05	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_AudioPlug )

#define DC_CAP_F08	\
(NVC_SUPP_ButtonMonitor|\
NVC_SUPP_DNMonitor|\
NVC_SUPP_IfrFilter|\
NVC_SUPP_IfrLight|\
NVC_SUPP_StateLED|\
NVC_SUPP_AudioPlug )

//  -------------------------------> External Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
/*
return 
	mask 0x 8000 0000	Err

	mask 0xFF00  	Chip
	mask 0xFF  		Pro
*/
extern int32 gfIsTheInputNumExist(uint8 *iChip,uint8 *iPro,uint8 *iPro_Cmd);

extern int32 gfInitAppointPro(void);

extern int32 gfUninitAppointPro(void);


/**
int32 gfInitDrv_HALLeyer(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_HALLeyer(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_MsgCenter(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_MsgCenter(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_ButtonMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_ButtonMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_DNMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_DNMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_IfrFilter(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_IfrFilter(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_IfrLight(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_IfrLight(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_DoubleLens(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_DoubleLens(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_StateLED(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_StateLED(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_PanTilt(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_PanTilt(uint16 iPro,uint8 iMcu,uint8 iRemain);


int32 gfInitDrv_NightLight(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_NightLight(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_CoolFan(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_CoolFan(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_AudioPlug(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_AudioPlug(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_TempMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_TempMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_HumidityMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_HumidityMonitor(uint16 iPro,uint8 iMcu,uint8 iRemain);

int32 gfInitDrv_ResetIO(uint16 iPro,uint8 iMcu,uint8 iRemain);
int32 gfUninitDrv_ResetIO(uint16 iPro,uint8 iMcu,uint8 iRemain);
*/


#endif