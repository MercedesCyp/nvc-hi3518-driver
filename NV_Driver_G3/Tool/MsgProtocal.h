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
#define DC_Protocol_Pro_F09				0xF09
#define DC_Protocol_Pro_F10				0xF10
#define DC_Protocol_Pro_F17				0xF17
#define DC_Protocol_Pro_G02				0x1002
#define DC_Protocol_Pro_G03				0x1003

#define DC_Protocol_Chip_3518C			0x3518C
#define DC_Protocol_Chip_3518E			0x3518E

// ------------------------------------------------------------>Protocol
// -------------------------------------------------->Header
// --------------------------------------->TYPE
#define DC_Protocal_MsgHeaderSize		12
// --------------------------------------->Magic word
#define DC_Protocal_MagicWord			0xCA83
// --------------------------------------->Err Code
// 错误码定义
#define     NVC_DRIVER_SUCCESS              0	// 无错误
#define     NVC_DRIVER_ERR_MGCWOD           1	// 消息透验证码错误(magic word)
#define     NVC_DRIVER_ERR_MSGTYPE_M        2	// 消息主类型错误(Massage type main)
#define     NVC_DRIVER_ERR_MSGTYPE_S        3	// 消息子类型错误(Massage type sub)
#define     NVC_DRIVER_ERR_PFAPP            4	// 应用层读写时，传递到驱动的指针为非法指针，不可操作(point from application layer)
#define     NVC_DRIVER_ERR_SLFAPP           5	// 读取数据时，应用层提供的空间不够(space length from application layer)
#define     NVC_DRIVER_ERR_BFORMAT          6	// 应用层提供的消息类型，与目标消息格式不符(package format)
#define     NVC_DRIVER_ERR_BUSY             7	// 驱动正忙(Driver busy)
#define     NVC_DRIVER_ERR_INIT             8	// 设备初始化错误
#define     NVC_DRIVER_ERR_PLBREAK          9	// 硬件设备检测不到（physical device lost connection）
#define     NVC_DRIVER_ERR_UNFINISHED       10	// 操作未完成
#define     NVC_DRIVER_ERR_MQUEUE           11	// 内存消息管理出错（不能写，读，或者直接内存溢出）
#define     NVC_DRIVER_ERR_NO_SUPP          12	// 主消息和子消息都是对的，但可能由于机型的原因导致某些操作不支持
#define     NVC_DRIVER_ERR_PTZ_NoEmpty      13	// 设置预置位的时候可能覆盖以前的值
typedef struct {
	uint16 aMagicWord;
	uint8 aSType;
	uint8 aMType;
	uint16 aLen;
	uint8 aUint;
	uint8 aErr;
	uint8 aRemain[4];
} mNVMsg_Head;

// --------------------------------------->Command
// 设备
// -----------------------------------------------------------------> CMD DEVICE
#define NVC_MSG_TYPE_DEVICE						0x0000
#define NVC_MSG_DEVICE_GET_INFO					1
#define NVC_MSG_DEVICE_GET_INFO_RESP			2
#define NVC_MSG_DEVICE_GET_CAP					3
#define NVC_MSG_DEVICE_GET_CAP_RESP				4
#define NVC_MSG_DEVICE_SUB_REPORT				5
#define NVC_MSG_DEVICE_SUB_REPORT_RESP			6
#define NVC_MSG_DEVICE_REPORT_DRIVER_ERR        7

// MsgBody
// Driver info
typedef struct {
	uint32 aChipInfo;
	uint32 aDeviceInfo;
	uint8 aVerInfo[16];
	uint8 aBuildData[32];
} mNVC_DRV_Ver_INFO;
extern mNVC_DRV_Ver_INFO NVCDriverInfo;

// Driver Capability
#define	CAP_SUPP_ButtonMonitor      0x00000001
#define	CAP_SUPP_LdrMonitor         0x00000002
#define	CAP_SUPP_Ircut              0x00000004
#define	CAP_SUPP_IfrLamp            0x00000008
#define	CAP_SUPP_DoubleLens         0x00000010
#define	CAP_SUPP_StateLed           0x00000020
#define	CAP_SUPP_PTZ                0x00000040
#define	CAP_SUPP_NightLight         0x00000080
#define NVC_SUPP_CoolFan            0x00000100	// 是否支持散热风扇功能
#define	CAP_SUPP_AudioPlug          0x00000200
#define	CAP_SUPP_TempMonitor        0x00000400
#define	CAP_SUPP_HumiMonitor        0x00000800
#define CAP_SUPP_GpioReset          0x00001000	// 通过GPIO 复位设备(重启设备)
#define CAP_SUPP_RTC                0x00002000
#define CAP_SUPP_PIR                0x00004000
#define CAP_SUPP_DoorBell           0x00008000
#define CAP_SUPP_DoorLock           0x00010000
#define CAP_SUPP_FM1288             0x00020000
#define CAP_SUPP_EAS                0x00040000
typedef struct {
	uint32 aDrvCapMask;
	uint8 aNumOfButton;
	uint8 aNumOfStaLED;
	uint8 aRemin[2];
} mNVC_DRV_CAP_INFO;
extern mNVC_DRV_CAP_INFO NVCDriverCap;

// Report Subscrible
#define DC_NVCMD_ATTACHED_SubMsg 	0x01
#define DC_NVCMD_ATTACHED_UnsubMsg 	0x00
typedef struct {
	uint8 aAttched;
	uint8 aRemain[3];
} mNVC_ATTACHED_Msg;

// -----------------------------------------------------------------> CMD Button
#define NVC_MSG_TYPE_BUTTON                     0x0100
#define NVC_MSG_BUTTON_GET_STATUS               1
#define NVC_MSG_BUTTON_GET_STATUS_RESP          2
#define NVC_MSG_BUTTON_REPORT_STATUS            3

// MsgBody
#define DC_NVBUTT_ButtonHight		0x01
#define DC_NVBUTT_ButtonLow			0x02
#define DC_NVBUTT_ButtonRaise		0x04
#define DC_NVBUTT_ButtonFall		0x08
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_BUTTON_STATUS;

// --------------------------------------------------------------------> CMD LDR
#define NVC_MSG_TYPE_LDR                        0x0200
#define NVC_MSG_LDR_GET_STATE                   1
#define NVC_MSG_LDR_GET_STATE_RESP              2
#define NVC_MSG_LDR_REPORT_STATE                3
#define NVC_MSG_LDR_SET_SENSITIVE               5
#define NVC_MSG_LDR_SET_SENSITIVE_RESP          6
#define NVC_MSG_LDR_GET_SENSITIVE               7
#define NVC_MSG_LDR_GET_SENSITIVE_RESP			8

// MsgBody
#define DC_NVANMonitor_NIGHT	0x00	//通红外光
#define DC_NVANMonitor_DAY		0x01	//不通红外光
typedef struct {
	uint8 aStatus;
	uint8 aRemain;
	uint16 aVal;
} mNVC_DNMONITOR_STATUS;

typedef struct {
	uint8 aSPoint;
	uint8 aDomain;
	uint8 aReamin[2];
} mNVC_DNMONITOR_SENSI;

// --------------------------------------------------------> CMD Infrared filter
#define NVC_MSG_TYPE_IRC                        0x0300
#define NVC_MSG_IRC_GET_TYPE                    1
#define NVC_MSG_IRC_GET_TYPE_RESP               2
#define NVC_MSG_IRC_SET_STATUS                  3
#define NVC_MSG_IRC_SET_STATUS_RESP             4
#define NVC_MSG_IRC_GET_STATUS                  5
#define NVC_MSG_IRC_GET_STATUS_RESP             6
#define NVC_MSG_IRC_REPORT_STATE				8

// MsgBody
#define DC_NVIfrFilt_TypeA	    0x00000001
#define DC_NVIfrFilt_TypeB	    0x00000002
#define DC_NVIfrFilt_TypeC	    0x00000003
typedef struct {
	uint8 aType;
	uint8 aRemain[3];
} mNVC_IRFILT_TYPE;

#define DC_NVIfrFilt_PassInfr	0x01
#define DC_NVIfrFilt_BlockInfr	0x00
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_IRFILT_STATUS;

// ---------------------------------------------------------> CMD infrared light
#define NVC_MSG_TYPE_IfrLIGHT                   0x0400
#define NVC_MSG_IfrLIGHT_SET_STATUS             1
#define NVC_MSG_IfrLIGHT_SET_STATUS_RESP        2
#define NVC_MSG_IfrLIGHT_GET_STATUS             3
#define NVC_MSG_IfrLIGHT_GET_STATUS_RESP        4

// MsgBody
#define DC_NVIrLight_ON			0x01
#define DC_NVIrLight_OFF		0x00
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_IRLIGHT_STATUS;

// ------------------------------------------------------------> CMD State Light
#define NVC_MSG_TYPE_StaLIGHT                   0x0500
#define NVC_MSG_StaLIGHT_SET_STATUS             1
#define NVC_MSG_StaLIGHT_SET_STATUS_RESP        2

// state mode == mNVC_STALIGHT_SETINFO.aLightID
#define NVC_STATE_LIGHT_ID_DEFAULT               0
#define NVC_STATE_LIGHT_ID_RED                  1
#define NVC_STATE_LIGHT_ID_GREEN                2
#define NVC_STATE_LIGHT_ID_BLUE                 3
#define NVC_STATE_LIGHT_ID_BREATH               4
#define NVC_STATE_LIGHT_ID_RACING               5
typedef struct {
	uint32 aLightMode;
	union {
		uint32 aOnMes;
		uint32 aBrthFrq;
	};
	uint32 aOffMes;
} mNVC_STALIGHT_SETINFO;

// ---------------------------------------------------------------> CMD Pan tilt
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
#define NVC_MSG_PTZ_INPORRT_PRESET_P_REQ        11
#define NVC_MSG_PTZ_INPORRT_PRESET_P_RESP       12
#define NVC_MSG_PTZ_SET_CRUISE_PATH_REQ         13
#define NVC_MSG_PTZ_SET_CRUISE_PATH_RESP        14

// MsgBody
#define DEF_PanTile
#ifdef  DEF_PanTile
////////////////////////////////////////////////////////////////////////////////
//
#define	NVC_PTZ_SUPP_HMOVE  	 	0x00000001	// 是否支持水平运动           //
#define	NVC_PTZ_SUPP_VMOVE  	 	0x00000002	// 是否支持垂直运动           //
#define	NVC_PTZ_SUPP_HVMOVE 	 	0x00000004	// 是否支持水平垂直叠加运动(  //是否支持左上,左下, 右上, 右下命令 )
#define	NVC_PTZ_SUPP_HSCAN  	 	0x00000008	// 是否支持水平自动扫描       //
#define	NVC_PTZ_SUPP_VSCAN  	 	0x00000010	// 是否支持垂直自动扫描       //
#define	NVC_PTZ_SUPP_HLIMIT 	 	0x00000020	// 是否支持水平限位设置       //
#define	NVC_PTZ_SUPP_VLIMIT 	 	0x00000040	// 是否支持垂直限位设置       //
#define	NVC_PTZ_SUPP_ZERO   	 	0x00000080	// 是否支持零位检测/设置      //
#define	NVC_PTZ_SUPP_CURPOS 	 	0x00000100	// 是否支持获取当前云台位置   //
#define	NVC_PTZ_SUPP_PRESET 	 	0x00000800	// 是否支持预置位             //
#define	NVC_PTZ_SUPP_CRUISE 	 	0x00001000	// 是否支持预置位巡航         //
//
#define NVC_PTZ_STATUS_Initing		0x00000001	// 设备初始化完成             //
#define NVC_PTZ_STATUS_Busy			0x00000002	//                            //
#define NVC_PTZ_STATUS_CmdFull		0x00000004	//
//
typedef struct {		//
	uint32 aCmdMask;	//
	uint32 aStatus;		//
	uint32 aHRange;		//
	uint32 aDgrPerHSteps;	//
	uint32 aUnitHSteps;	//
	uint32 aVRange;		//
	uint32 aDgrPerVSteps;	//
	uint32 aUnitVSteps;	//
	uint32 aZeroXPos;	//
	uint32 aZeroYPos;	//
	uint32 aXPos;		//
	uint32 aYPos;		//
} mNVC_PANTILT_INFO;		//
//
#define NV_PTZ_STOP        		0	// 云台停止                               //
#define NV_PTZ_UP          		1	// 上                                     //
#define NV_PTZ_DOWN        		2	// 下                                     //
#define NV_PTZ_LEFT        		3	// 左                                     //
#define NV_PTZ_RIGHT       		4	// 右                                     //
#define NV_PTZ_LEFT_UP     		5	// 左上                                   //
#define NV_PTZ_LEFT_DOWN   		6	// 左下                                   //
#define NV_PTZ_RIGHT_UP    		7	// 右上                                   //
#define NV_PTZ_RIGHT_DOWN  		8	// 右下                                   //
#define NV_PTZ_MVT_PRESET       9	// 移动至指定预置位                       //
#define NV_PTZ_STAR_CRUISE      10	// 开始巡航                               //
#define NV_PTZ_AUTO_SCAN   		13	// 自动扫描                               //
#define NV_PTZ_UP_LIMIT    		14	// 上限位设置                             //
#define NV_PTZ_DOWN_LIMIT  		15	// 下限位设置                             //
#define NV_PTZ_LEFT_LIMIT  		16	// 左限位设置                             //
#define NV_PTZ_RIGHT_LIMIT 		17	// 右限位设置                             //
//#define NV_PTZ_GOTO_ZERO              23  // 零位检测，云台初始位置             //
//
//                                                                            //
#define NV_PTZ_SpeedDefault		0	//68 69                                    //
#define NV_PTZ_SpeedMax			100	//
#define NV_PTZ_SpeedMin			1	//
typedef struct {		//
	uint8 aCmd;		//
	union {			//
		uint8 aParaType;	//0 steps 1 angle 2 coordinate                       //
		uint8 aNo;	//
	};			//
	uint8 aSpeed;		//
	uint8 aReamin[1];	//
	uint32 aHParam;		//
	uint32 aVParam;		//
} mNVC_PANTILT_SET;		//
//
#define NVC_CRUISE_PRESET_NUM 16	//
typedef struct {		//
	uint8 aPreSetNo;	//
	uint8 aSpeed;		//
	uint16 aStaySeconds;	//
} mNVC_PTCruiseUnit;		//
// ---                                                                        //
typedef struct {		//
	uint16 aXPos;		//
	uint16 aYPos;		//
} mNVC_PTZCurPos;		//
// ---                                                                        //
typedef struct {		//
	uint8 aMvPath[NVC_CRUISE_PRESET_NUM];	//
} mNVC_PTZMvPath;		//
//                                                                            //
typedef struct {		//
	mNVC_PTCruiseUnit aInfo[NVC_CRUISE_PRESET_NUM];	//
	mNVC_PTZCurPos aCoo[NVC_CRUISE_PRESET_NUM];	//
} mNVC_PTZPreSetConfF;		//
//                                                                            //
#define NV_PTZ_STA_Success 			0x01	//
#define NV_PTZ_STA_RemainSpace		0x02	//
typedef struct {		//
	uint8 aStatus;		//
	uint8 aRemain[3];	//
	uint32 aXPos;		//
	uint32 aYPos;		//
} mNVC_PANTILT_Respons;		//
//
////////////////////////////////////////////////////////////////////////////////
#endif

// ------------------------------------------------------------> CMD Night Light
#define NVC_MSG_TYPE_NitLIGHT                   0x0700

#define NVC_MSG_NitLIGHT_SET_STATUS             1
#define NVC_MSG_NitLIGHT_SET_STATUS_RESP        2
#define NVC_MSG_NitLIGHT_GET_STATUS             3
#define NVC_MSG_NitLIGHT_GET_STATUS_RESP        4

// MsgBody
#define DC_NIGHTLIGHT_Off		0x00
#define DC_NIGHTLIGHT_On		0x01
typedef struct {
	uint8 aStatus;
	uint8 aLemLevel;
	uint8 aRemain[2];
} mNVC_NIGHTLIGHT_STATUS;

// -------------------------------------------------------------> CMD Audio plug
#define NVC_MSG_TYPE_AudioPLGU                  0x0800

#define NVC_MSG_SPEAKER_SET_STATUS            1
#define NVC_MSG_SPEAKER_SET_STATUS_RESP       2
#define NVC_MSG_SPEAKER_GET_STATUS            3
#define NVC_MSG_SPEAKER_GET_STATUS_RESP       4
#define NVC_MSG_MICROPH_SET_STATUS            5
#define NVC_MSG_MICROPH_SET_STATUS_RESP       6
#define NVC_MSG_MICROPH_GET_STATUS            7
#define NVC_MSG_MICROPH_GET_STATUS_RESP       8

// MsgBody
#define DC_NVAuPl_Off				0x00
#define DC_NVAuPl_On				0x01
#define DC_NVAuPl_NOSUPP            0x02
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_AUDIOPLUG_Msg;

// ----------------------------------------------------> CMD Temperature monitor
#define NVC_MSG_TYPE_TempMONITOR                0x0900
#define NVC_MSG_TempMONITOR_SUB_REPORT          1
#define NVC_MSG_TempMONITOR_SUB_REPORT_RESP     2
#define NVC_MSG_TempMONITOR_GET_VALUE           3
#define NVC_MSG_TempMONITOR_GET_VALUE_RESP      4
#define NVC_MSG_TempMONITOR_REPORT_VALUE        5

// MsgBody
typedef struct {
	uint32 aTimes;
} mNVC_TEMPMONITOR_Timer;

typedef struct {
	int32 aValue;
} mNVC_TEMPMONITOR_Value;

// -------------------------------------------------------> CMD Humidity monitor
#define NVC_MSG_TYPE_HumiMONITOR                0x0A00

#define NVC_MSG_HumiMONITOR_SUB_REPORT          1
#define NVC_MSG_HumiMONITOR_SUB_REPORT_RESP     2
#define NVC_MSG_HumiMONITOR_GET_VALUE           3
#define NVC_MSG_HumiMONITOR_GET_VALUE_RESP      4
#define NVC_MSG_HumiMONITOR_REPORT_VALUE        5

// MsgBody
typedef struct {
	uint32 aTimes;
} mNVC_HUMIDITY_Timer;

typedef struct {
	int32 aValue;
} mNVC_HUMIDITY_Value;

// ------------------------------------------------------------> CMD Double Lens
#define NVC_MSG_TYPE_DoubLENS                   0x0B00
#define NVC_MSG_DoubLENS_SET_STATUS             1
#define NVC_MSG_DoubLENS_SET_STATUS_RESP        2
#define NVC_MSG_DoubLENS_GET_STATUS             3
#define NVC_MSG_DoubLENS_GET_STATUS_RESP        4

// MsgBody
#define DC_DUBLENS_NIGHTLENS	0x01
#define DC_DUBLENS_DAYLENS		0x00
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_DUBLENS_SETINFO;

// --------------------------------------------------------------------> CMD RTC
#define NVC_MSG_TYPE_RTC                        0x0D00
#define NVC_MSG_RTC_SET_TIME                    1
#define NVC_MSG_RTC_SET_TIME_RESP               2
#define NVC_MSG_RTC_GET_TIME                    3
#define NVC_MSG_RTC_GET_TIME_RESP               4

// MsgBody
typedef struct {
	uint8 aSecond;
	uint8 aMinute;
	uint8 aHour;
	uint8 aDay;
	uint8 aWeekday;
	uint8 aMonth;
	uint8 aYear;
} mNVC_RTC_DATA;

// --------------------------------------------------------------------> CMD PIR
#define NVC_MSG_TYPE_PIR                        0x0E00
#define NVC_MSG_PIR_GET_STATUS                    1
#define NVC_MSG_PIR_GET_STATUS_RESP               2
#define NVC_MSG_PIR_REPORT_STATUS                 3
#define NVC_MSG_PIR_SET_DeadTime                  5
#define NVC_MSG_PIR_SET_DeadTime_RESP             6

// MsgBody
#define DC_MsgPIR_Discover_Normal      0x00
#define DC_MsgPIR_Discover_Unnormal    0x01
typedef struct {
	union {
		struct {
			uint8 aStatus;
			uint8 aRemain[3];
		};
		uint32 aCounts;
	};
	uint32 aDistance;
} mNVC_PIR_STATUS;
typedef struct {
	uint32 aTime;
} mNVC_PIR_DEAD_TIME;

// --------------------------------------------------------------> CMD Door Bell
#define NVC_MSG_TYPE_DoorBell                   0x0F00
#define NVC_MSG_DoorBell_SET_Status               1
#define NVC_MSG_DoorBell_SET_Status_RESP          2

// MsgBody
#define DC_MsgDoorBell_TapBell     0x01
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_DoorBell_STATUS;

// --------------------------------------------------------------------> CMD Door Lock
#define NVC_MSG_TYPE_DoorLock                   0x1000
#define NVC_MSG_DoorLock_SET_STATUS             1
#define NVC_MSG_DoorLock_SET_STATUS_RESP        2
#define NVC_MSG_DoorLock_GET_STATUS             3
#define NVC_MSG_DoorLock_GET_STATUS_RESP        4

// MsgBody
#define DC_MsgDoorLock_Close       0x00	// 关闭门锁
#define DC_MsgDoorLock_Open        0x01	// 打开门锁
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_DoorLock_STATUS;

// --------------------------------------------------------------------> CMD FM1288
#define NVC_MSG_TYPE_FM1288                0x1100
#define NVC_MSG_FM1288_WRITE_REGISTER             1
#define NVC_MSG_FM1288_WRITE_REGISTER_RESP        2
#define NVC_MSG_FM1288_READ_REGISTER              3
#define NVC_MSG_FM1288_READ_REGISTER_RESP         4

// MsgBody
typedef struct {
	uint8 *u8Data;		// the data which want to write
	uint32 u32DataLength;	// data length
	uint8 u8RegHigh;	// register address high byte
	uint8 u8RegLow;		// register address low byte
} mNVC_FM1288_Msg;

// --------------------------------------------------------------------> CMD EAS
#define NVC_MSG_TYPE_EAS                        0x1200
#define NVC_MSG_EAS_GET_STATUS                  1
#define NVC_MSG_EAS_GET_STATUS_RESP             2
#define NVC_MSG_EAS_REPORT_STATUS               3

#define DC_NVEAS_SAFTY          0x00
#define DC_NVEAS_EMERGENCY      0x01
// MsgBody
typedef struct {
	uint8 aStatus;
	uint8 aRemain[3];
} mNVC_EAS_Msg;

// --------------------------------------------------------------------> CMD END
#define DF_GET_MSGTYPE(_v)						((_v)&0xFF00)
#define DF_GET_MSGCMD(_v)						((_v)&0xFF)

#endif
