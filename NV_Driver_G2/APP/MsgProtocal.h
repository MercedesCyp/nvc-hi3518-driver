#ifndef __MsgProtocal_H
#define __MsgProtocal_H


// ================================================================================ 
// ------------------------------------------------------------>Message center
#define DC_Protocal_MaxMesgLen			256

// ------------------------------------------------------------>Support
#define DC_Protocol_Pro_D01				0xD01
#define DC_Protocol_Pro_D03				0xD03
#define DC_Protocol_Pro_D04				0xD04
#define DC_Protocol_Pro_D11				0xD11
#define DC_Protocol_Pro_F02				0xF02
#define DC_Protocol_Pro_F05				0xF05
#define DC_Protocol_Pro_F07				0xF07
#define DC_Protocol_Pro_F08				0xF08

#define DC_Protocol_Chip_3518C			0x3518C
#define DC_Protocol_Chip_3518E			0x3518E


// ------------------------------------------------------------>Protocol
// -------------------------------------------------->Header
// --------------------------------------->TYPE
#define DC_Protocal_MsgHeaderSize		12
// --------------------------------------->Magic word
#define DC_Protocal_MagicWord			0xCA83
// --------------------------------------->Err Code
#define NVC_DRIVER_SUCCESS          0 
#define NVC_DRIVER_ERR              1 // 通用错误
#define NVC_DRIVER_ERR_INIT         2 // 初始化设备错误
#define NVC_DRIVER_ERR_PARAM        3 // 输入参数出错
#define NVC_DRIVER_ERR_BUSY         4 // 驱动正忙
#define NVC_DRIVER_ERR_NOT_SUPPORT  5 // 不支持的操作(比如设备支持红外灯，但不支持查询红外灯状态)
#define NVC_DRIVER_ERR_INVALID      6 // 无效设备
#define NVC_DRIVER_ERR_UNAVAILABLE  7 // 设备丢失，如物理连接断开
#define NVC_DRIVER_ERR_UNFINISHED   8 // 操作未完成(如云台命令下发控制步数超过当前能运动步数)
#define NVC_DRIVER_ERR_LIMITED      9 // 如云台已到运动方向终点或限位导致命令无效
// --------------------------------------->Command
// 设备
// ------------------------------> CMD DEVICE
#define NVC_MSG_TYPE_DEVICE						0x0000

#define NVC_MSG_DEVICE_GET_INFO					1
#define NVC_MSG_DEVICE_GET_INFO_RESP			2
#define NVC_MSG_DEVICE_GET_CAP					3
#define NVC_MSG_DEVICE_GET_CAP_RESP				4
#define NVC_MSG_DEVICE_SUB_REPORT				5
#define NVC_MSG_DEVICE_SUB_REPORT_RESP			6

// ------------------------------> CMD Button
#define NVC_MSG_TYPE_BUTTON                     0x0100

#define NVC_MSG_BUTTON_GET_STATUS               1
#define NVC_MSG_BUTTON_GET_STATUS_RESP          2
#define NVC_MSG_BUTTON_REPORT_STTAUS            3


// ------------------------------> CMD LDR
#define NVC_MSG_TYPE_LDR                        0x0200

#define NVC_MSG_LDR_GET_STATE                   1
#define NVC_MSG_LDR_GET_STATE_RESP              2
#define NVC_MSG_LDR_REPORT_STATE                3
#define NVC_MSG_LDR_SET_SENSITIVE               5
#define NVC_MSG_LDR_SET_SENSITIVE_RESP          6
#define NVC_MSG_LDR_GET_SENSITIVE               7
#define NVC_MSG_LDR_GET_SENSITIVE_RESP			8


// ------------------------------> CMD Infrared filter
#define NVC_MSG_TYPE_IRC                        0x0300

#define NVC_MSG_IRC_GET_TYPE                    1
#define NVC_MSG_IRC_GET_TYPE_RESP               2
#define NVC_MSG_IRC_SET_STATUS                  3
#define NVC_MSG_IRC_SET_STATUS_RESP             4
#define NVC_MSG_IRC_GET_STATUS                  5
#define NVC_MSG_IRC_GET_STATUS_RESP             6


// ------------------------------> CMD infrared light
#define NVC_MSG_TYPE_IfrLIGHT                   0x0400

#define NVC_MSG_IfrLIGHT_SET_STATUS             1
#define NVC_MSG_IfrLIGHT_SET_STATUS_RESP        2
#define NVC_MSG_IfrLIGHT_GET_STATUS             3
#define NVC_MSG_IfrLIGHT_GET_STATUS_RESP        4


// ------------------------------> CMD State Light
#define NVC_MSG_TYPE_StaLIGHT                   0x0500

#define NVC_MSG_StaLIGHT_SET_STATUS             1
#define NVC_MSG_StaLIGHT_SET_STATUS_RESP        2


// ------------------------------> CMD Pan tilt
#define NVC_MSG_TYPE_PTZ                        0x0600

#define NVC_MSG_PTZ_GET_INFO                    1
#define NVC_MSG_PTZ_GET_INFO_RESP               2
#define NVC_MSG_PTZ_SET_ACTION                  3
#define NVC_MSG_PTZ_SET_ACTION_RESP             4
#define NVC_MSG_PTZ_SET_PRESET                  5
#define NVC_MSG_PTZ_SET_PRESET_RESP             6
#define NVC_MSG_PTZ_CLR_PRESET                  7
#define NVC_MSG_PTZ_CLR_PRESET_RESP             8
#define NVC_MSG_PTZ_REPORT_STATUS               9


// ------------------------------> CMD Night Light
#define NVC_MSG_TYPE_NitLIGHT                   0x0700

#define NVC_MSG_NitLIGHT_SET_STATUS             1
#define NVC_MSG_NitLIGHT_SET_STATUS_RESP        2
#define NVC_MSG_NitLIGHT_GET_STATUS             3
#define NVC_MSG_NitLIGHT_GET_STATUS_RESP        4


// ------------------------------> CMD audio plug
#define NVC_MSG_TYPE_AudioPLGU                  0x0800

#define NVC_MSG_AudioPLUG_SET_STATUS            1
#define NVC_MSG_AudioPLUG_SET_STATUS_RESP       2
#define NVC_MSG_AudioPLUG_GET_STATUS            3
#define NVC_MSG_AudioPLUG_GET_STATUS_RESP       4


// ------------------------------> CMD Temperature monitor
#define NVC_MSG_TYPE_TempMONITOR                0x0900

#define NVC_MSG_TempMONITOR_SUB_REPORT          1
#define NVC_MSG_TempMONITOR_SUB_REPORT_RESP     2
#define NVC_MSG_TempMONITOR_GET_VALUE           3
#define NVC_MSG_TempMONITOR_GET_VALUE_RESP      4
#define NVC_MSG_TempMONITOR_REPORT_VALUE        5


// ------------------------------> CMD Humidity monitor
#define NVC_MSG_TYPE_HumiMONITOR                0x0A00

#define NVC_MSG_HumiMONITOR_SUB_REPORT          1
#define NVC_MSG_HumiMONITOR_SUB_REPORT_RESP     2
#define NVC_MSG_HumiMONITOR_GET_VALUE           3
#define NVC_MSG_HumiMONITOR_GET_VALUE_RESP      4
#define NVC_MSG_HumiMONITOR_REPORT_VALUE        5


// ------------------------------> CMD Double Lens
#define NVC_MSG_TYPE_DoubLENS                   0x0B00

#define NVC_MSG_DoubLENS_SET_STATUS             1
#define NVC_MSG_DoubLENS_SET_STATUS_RESP        2
#define NVC_MSG_DoubLENS_GET_STATUS             3
#define NVC_MSG_DoubLENS_GET_STATUS_RESP        4



#define DF_GET_MSGTYPE(_v)						((_v)&0xFF00)
#define DF_GET_MSGCMD(_v)						((_v)&0xFF)





// -------------------------------------------------->Message Body
// ----------------------------------------> Driver Info

typedef struct{
	uint32 aChipInfo;
	uint32 aDeviceInfo;
	uint8 aVerInfo[16];
	uint8 aBuildData[32];
}mNVC_DRV_Ver_INFO;
extern mNVC_DRV_Ver_INFO NVCDriverInfo;

// ----------------------------------------> Product Ability Array
#define	CAP_SUPP_ButtonMonitor    0x00000001
#define	CAP_SUPP_LdrMonitor       0x00000002
#define	CAP_SUPP_Ircut            0x00000004
#define	CAP_SUPP_IfrLamp          0x00000008
#define	CAP_SUPP_DoubleLens       0x00000010
#define	CAP_SUPP_StateLed         0x00000020
#define	CAP_SUPP_PTZ              0x00000040
#define	CAP_SUPP_NightLight       0x00000080
#define	CAP_SUPP_AudioPlug        0x00000100
#define	CAP_SUPP_TempMonitor      0x00000200
#define	CAP_SUPP_HumiMonitor      0x00000400

typedef struct{
	uint32 aDrvCapMask;
	uint8 aNumOfButton;
	uint8 aNumOfStaLED;
	uint8 aRemin[2];
}mNVC_DRV_CAP_INFO;
extern mNVC_DRV_CAP_INFO NVCDriverCap;

// ----------------------------------------> Subscribe Report Msg

typedef struct{
	uint8	aAttched;
	uint8	aRemain[3];
}mNVC_ATTACHED_Msg;
#define DC_NVCMD_ATTACHED_SubMsg 	0x01
#define DC_NVCMD_ATTACHED_UnsubMsg 	0x00


// ----------------------------------------> Button

typedef struct{
	uint8	aStatus;
	uint8	aRemain[3];
}mNVC_BUTTON_STATUS;

#define DC_NVBUTT_ButtonHight		0x01
#define DC_NVBUTT_ButtonLow			0x02
#define DC_NVBUTT_ButtonRaise		0x04
#define DC_NVBUTT_ButtonFall		0x08


// ----------------------------------------> Audio Plug

typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_AUDIOPLUG_Msg;
#define DC_NVAuPl_Off				0x00
#define DC_NVAuPl_On				0x01      


// ----------------------------------------> LDR

typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_DNMONITOR_STATUS;
#define DC_NVANMonitor_NIGHT	0x00		//通红外光
#define DC_NVANMonitor_DAY		0x01        //不通红外光

typedef struct{
	uint8 aSensitive;
	uint8 aReamin[3];
}mNVC_DNMONITOR_SENSI;


// ----------------------------------------> IRCut

typedef struct{
	uint32 aType;
}mNVC_IRFILT_TYPE;

typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_IRFILT_STATUS;
#define DC_NVIfrFilt_PassInfr	0x01
#define DC_NVIfrFilt_BlockInfr	0x00


// ----------------------------------------> Infrared Light

typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_IRLIGHT_STATUS;
#define DC_NVIrLight_ON			0x01
#define DC_NVIrLight_OFF		0x00


// ----------------------------------------> State Light

typedef struct{
	uint8 aLightID;
	uint32 aOnMes;
	uint32 aOffMes;
}mNVC_STALIGHT_SETINFO;
#define DC_LED_Green1_B	0
#define DC_LED_Green2_B	1
#define DC_LED_Red1_B	2	


// ----------------------------------------> Double Lens

typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_DUBLENS_SETINFO;
#define DC_DUBLENS_NIGHTLENS	0x01
#define DC_DUBLENS_DAYLENS		0x00


// ----------------------------------------> Temperature

typedef struct{
	uint32 aTimes;
}mNVC_TEMPMONITOR_Timer;

typedef struct{
	int32 aValue;
}mNVC_TEMPMONITOR_Value;


// ----------------------------------------> Humidity

typedef struct{
	uint32 aTimes;
}mNVC_HUMIDITY_Timer;

typedef struct{
	int32 aValue;
}mNVC_HUMIDITY_Value;


// ----------------------------------------> Night light

typedef struct{
	uint8	aStatus;
	uint8 	aLemLevel;
	uint8 	aRemain[2];
}mNVC_NIGHTLIGHT_STATUS;
#define DC_NIGHTLIGHT_Off		0x00
#define DC_NIGHTLIGHT_On		0x01



// ----------------------------------------> PTZ

////////////////////////////////////////////////////////////////////////////////
                                                                              //
#define	NVC_PTZ_SUPP_HMOVE  	 	0x00000001  // 是否支持水平运动           //
#define	NVC_PTZ_SUPP_VMOVE  	 	0x00000002  // 是否支持垂直运动           //
#define	NVC_PTZ_SUPP_HVMOVE 	 	0x00000004  // 是否支持水平垂直叠加运动(  //是否支持左上,左下, 右上, 右下命令 )
#define	NVC_PTZ_SUPP_HSCAN  	 	0x00000008  // 是否支持水平自动扫描       //
#define	NVC_PTZ_SUPP_VSCAN  	 	0x00000010  // 是否支持垂直自动扫描       //
#define	NVC_PTZ_SUPP_HLIMIT 	 	0x00000020  // 是否支持水平限位设置       //
#define	NVC_PTZ_SUPP_VLIMIT 	 	0x00000040  // 是否支持垂直限位设置       //
#define	NVC_PTZ_SUPP_ZERO   	 	0x00000080  // 是否支持零位检测/设置      //
#define	NVC_PTZ_SUPP_CURPOS 	 	0x00000100  // 是否支持获取当前云台位置   //
#define	NVC_PTZ_SUPP_ZOOM   	 	0x00000200  // 是否支持变倍               //
#define	NVC_PTZ_SUPP_FOCUS  	 	0x00000400  // 是否支持手动聚焦           //
#define	NVC_PTZ_SUPP_PRESET 	 	0x00000800  // 是否支持预置位             //
#define	NVC_PTZ_SUPP_CRUISE 	 	0x00001000  // 是否支持预置位巡航         //
                                                                              //
#define NVC_PTZ_STATUS_Initing		0x00000001	// 设备初始化完成             //
#define NVC_PTZ_STATUS_Busy			0x00000002	//                            //
#define NVC_PTZ_STATUS_CmdFull		0x00000004                                //
                                                                              //
typedef struct{                                                               //
	uint32 aCmdMask;                                                          //
	uint32 aStatus;                                                           //
	uint32 aHRange;                                                           //
	uint32 aDgrPerHSteps;                                                     //
	uint32 aUnitHSteps;                                                       //
	uint32 aVRange;                                                           //
	uint32 aDgrPerVSteps;                                                     //
	uint32 aUnitVSteps;                                                       //
	uint32 aZeroXPos;                                                         //
	uint32 aZeroYPos;                                                         //
	uint32 aXPos;                                                             //
	uint32 aYPos;                                                             //
}mNVC_PANTILT_INFO;                                                           //
                                                                              //
#define NV_PTZ_STOP        		0   // 云台停止                               //
#define NV_PTZ_UP          		1   // 上                                     //
#define NV_PTZ_DOWN        		2   // 下                                     //
#define NV_PTZ_LEFT        		3   // 左                                     //
#define NV_PTZ_RIGHT       		4   // 右                                     //
#define NV_PTZ_LEFT_UP     		5   // 左上                                   //
#define NV_PTZ_LEFT_DOWN   		6   // 左下                                   //
#define NV_PTZ_RIGHT_UP    		7   // 右上                                   //
#define NV_PTZ_RIGHT_DOWN  		8   // 右下                                   //
#define NV_PTZ_ZOOM_IN     		9   // 变倍+                                  //
#define NV_PTZ_ZOOM_OUT    		10  // 变倍-                                  //
#define NV_PTZ_FOCUS_NEAR  		11  // 聚焦近                                 //
#define NV_PTZ_FOCUS_FAR   		12  // 聚焦远                                 //
#define NV_PTZ_AUTO_SCAN   		13  // 自动扫描                               //
#define NV_PTZ_UP_LIMIT    		14  // 上限位设置                             //
#define NV_PTZ_DOWN_LIMIT  		15  // 下限位设置                             //
#define NV_PTZ_LEFT_LIMIT  		16  // 左限位设置                             //
#define NV_PTZ_RIGHT_LIMIT 		17  // 右限位设置                             //
#define NV_PTZ_PRESET_SET  		18  // 设置预置位                             //
#define NV_PTZ_PRESET_CLR  		19  // 删除预置位                             //
#define NV_PTZ_PRESET_CALL 		20  // 调用预置位                             //
#define NV_PTZ_START_CRUISE		21  // 开始巡航                               //
#define NV_PTZ_STOP_CRUISE 		22  // 停止巡航                               //
#define NV_PTZ_GOTO_ZERO   		23  // 零位检测，云台初始位置                 //
// #define NV_PTZ_Cordin			255  // 坐标                              //
//                                                                            //
#define NV_PTZ_SpeedDefault		0  //68 69                                    //
#define NV_PTZ_SpeedMax			100                                           //
#define NV_PTZ_SpeedMin			1                                             //
typedef struct{                                                               //
	uint8 	aCmd;                                                             //
	union{                                                                    //
		uint8 aParaType; //0 steps 1 angle 2 coordinate                       //
		uint8 aNo;                                                            //
	};                                                                        //
	uint8 aSpeed;                                                             //
	uint8 aReamin[1];                                                         //
	uint32 aHParam;                                                           //
	uint32 aVParam;	                                                          //
}mNVC_PANTILT_SET;                                                            //
                                                                              //
#define NVC_CRUISE_PRESET_NUM 16                                              //
typedef struct{                                                               //
	uint8 	aPreSetNo;                                                        //
	uint8 	aSpeed;                                                           //
	uint16 	aStaySeconds;                                                     //
}mPTCruiseUnit;                                                               //
// ---                                                                        //
typedef struct{                                                               //
	uint8 			aCruiseNo;                                                //
	uint8 			aPerSetCnt;                                               //
	uint8			aRemain[2];                                               //
	mPTCruiseUnit 	aPreSet[NVC_CRUISE_PRESET_NUM];                           //
}mNVC_PANTITE_Cruise;                                                         //
// ---                                                                        //
typedef struct{                                                               //
	uint8 aCruiseNo;                                                          //
	uint8 aRemain[3];                                                         //
}mNVC_PANTITE_ClrPreSet;                                                      //
//                                                                            //
#define NV_PTZ_STA_Success 			0x01                                      //
#define NV_PTZ_STA_RemainSpace		0x02                                      //
typedef struct{                                                               //
	uint8  aStatus;                                                           //
	uint8  aRemain[3];                                                        //
	uint32 aXPos;                                                             //
	uint32 aYPos;                                                             //
}mNVC_PANTILT_Respons;                                                        //
                                                                              //
////////////////////////////////////////////////////////////////////////////////


#endif
