#ifndef __MsgCenter_H
#define __MsgCenter_H
//------include
#include "GlobalParameter.h"
#include "Mqueue.h"
#include "String.h"
#include "ProInfo.h"
#include <linux/slab.h>


//.................................... Msg types
//|枚举定义 										//| 消息定义描述 |消息体结构体 |
#define NVC_QUERY_DRIVER_INFO_REQ				1	//| 获取驱动信息|无|
#define NVC_QUERY_DRIVER_INFO_RESP				2	//| 获取驱动信息响应信息|[Nvc_Driver_Ver_Info_S](#nvc_driver_ver_info_s)|

#define NVC_QUERY_DRIVER_CAPACITY_REQ			3	//| 获取设备外设能力集|无|
#define NVC_QUERY_DRIVER_CAPACITY_RESP			4	//| 获取设备外设能力集响应信息|[Nvc_Driver_Cap_Info_s](#nvc_driver_cap_info_s)|

#define NVC_SET_ATTACHED_DRIVER_MSG_REQ			5	//| 设置是否接收驱动事件及状态变化请求|[Nvc_Attached_Driver_Msg_s](#nvc_attached_driver_msg_s)|
#define NVC_SET_ATTACHED_DRIVER_MSG_RESP		6	//| 设置是否接收驱动事件及状态变化信息|无|
 
#define NVC_QUERY_BUTTON_STATUS_REQ				7	//| 获取button当前状态|无|
#define NVC_QUERY_BUTTON_STATUS_RESP			8	//| 获取button当前状态响应信息|[Nvc_Button_Status_S](#nvc_button_status_s)|

#define NVC_REPORT_BUTTON_STATUS_MSG			9	//| 上报button当前状态|[Nvc_Button_Status_S](#nvc_button_status_s)|

#define NVC_QUERY_LDR_STATUS_REQ				11	//| 获取光敏电阻当前状态|无|
#define NVC_QUERY_LDR_STATUS_RESP				12	//| 获取光敏电阻当前状态响应信息|[Nvc_Ldr_Status_S](#nvc_ldr_status_s)|

#define NVC_REPORT_LDR_STATUS_MSG				13	//| 上报光敏电阻当前状态|[Nvc_Ldr_Status_S](#nvc_ldr_status_s)|

#define NVC_SET_LDR_SENSITIVITY_REQ				15	//| 设置光敏电阻检测灵敏度|[Nvc_Ldr_Senitivity_S](#nvc_ldr_senitivity_s)|
#define NVC_SET_LDR_SENSITIVITY_RESP			16	//| 设置光敏电阻检测灵敏度响应信息|无|

#define NVC_QUERY_LDR_SENSITIVITY_REQ			17	//| 获取光敏电阻检测灵敏度|无|
#define NVC_QUERY_LDR_SENSITIVITY_RESP			18	//| 获取光敏电阻检测灵敏度响应信息|[Nvc_Ldr_Senitivity_S](#nvc_ldr_senitivity_s)|

#define NVC_QUERY_IRC_TYPE_REQ					19	//| 获取设备ircut类型|无|
#define NVC_QUERY_IRC_TYPE_RESP					20	//| 获取设备ircut类型响应信息|[Nvc_Ircut_Info_S](#nvc_ircut_info_s)|
 
#define NVC_CONTROL_IRC_SWITCH_REQ				21	//| 设置ircut切换状态|[Nvc_Ircut_Control_S](#nvc_ircut_info_s)|
#define NVC_CONTROL_IRC_SWITCH_RESP				22	//| 设置ircut切换状态响应信息|无|

#define NVC_QUERY_IRC_STATUS_REQ				23	//| 查询ircut当前状态|无|
#define NVC_QUERY_IRC_STATUS_RESP				24	//| 查询ircut当前状态响应信息|[Nvc_Ircut_Status_S](#nvc_ircut_status_s)|

#define NVC_CONTROL_LAMP_SWITCH_REQ				25	//| 设置红外灯打开/关闭|[Nvc_Lamp_Control_S](#nvc_lamp_control_s)|
#define NVC_CONTROL_LAMP_SWITCH_RESP			26	//| 设置红外灯打开/关闭响应信息|无|

#define NVC_QUERY_LAMP_STATUS_REQ				27	//| 查询红外灯打开/关闭状态|无|
#define NVC_QUERY_LAMP_STATUS_RESP				28	//| 查询红外灯打开/关闭状态响应信息|[Nvc_Lamp_Status_S](#nvc_lamp_status_s)|

#define NVC_CONTROL_STATE_LED_REQ				29	//| 设置LED灯显示方式|[Nvc_State_Led_Control_S](#nvc_state_led_control_s)|
#define NVC_CONTROL_STATE_LED_RESP				30	//| 设置LED灯显示方式响应信息|无|



//////////////////////////////////////////////////////////////////////////////
#define NVC_QUERY_PTZ_INFO_REQ					31	//| 云台信息查询|无|
#define NVC_QUERY_PTZ_INFO_RESP					32	//| 云台信息查询响应信息|[Nvc_Ptz_Info_S](#nvc_ptz_info_s)|

#define NVC_CONTROL_PTZ_COMMON_REQ				33	//| 通用云台控制指令|[Nvc_Ptz_Control_S](#nvc_ptz_control_s)|
#define NVC_CONTROL_PTZ_COMMON_RESP				34	//| 通用云台控制指令响应信息|无|

#define NVC_SET_PRESET_CRUISE_REQ				35	//| 设置云台预置位巡航|[Nvc_Ptz_Cruise_S](#nvc_ptz_cruise_s)|
#define NVC_SET_PRESET_CRUISE_RESP				36	//| 设置云台预置位巡航响应信息|无|

#define NVC_CLEAR_PRESET_CRUISE_REQ				37	//| 清除云台预置位巡航|[Nvc_Ptz_Cruise_Idx_S](#nvc_ptz_cruise_idx_s)|
#define NVC_CLEAR_PRESET_CRUISE_RESP			38	//| 清除云台预置位巡航响应信息|无|
//////////////////////////////////////////////////////////////////////////////



#define NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ		39	//| 设置小夜灯打开/关闭|[Nvc_Night_Light_Control_s](#nvc_night_light_control_s)|
#define NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP		40	//| 设置小夜灯打开/关闭响应信息|无|

#define NVC_QUERY_NIGHT_LIGHT_STATUS_REQ		41	//| 查询小夜灯打开/关闭状态|无|
#define NVC_QUERY_NIGHT_LIGHT_STATUS_RESP		42	//| 查询小夜灯打开/关闭状态响应信息|[Nvc_Night_Light_status_s](#nvc_night_light_status_s)|

#define NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ		43	//| 设置音频扬声器打开/关闭|[Nvc_Audio_Plug_Control_s](#nvc_audio_plug_control_s)|
#define NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP		44	//| 设置音频扬声器打开/关闭响应信息|无|

#define NVC_QUERY_AUDIO_PLUG_STATUS_REQ			45	//| 查询音频扬声器打开/关闭状态|无|
#define NVC_QUERY_AUDIO_PLUG_STATUS_RESP		46	//| 查询音频扬声器打开/关闭状态响应信息|[Nvc_Audio_Plug_status_s](#nvc_audio_plug_status_s)|

#define NVC_SET_TEMPERATURE_TIMER_REQ			47	//| 设置温度定时采集上报请求|[Nvc_Temperature_Timer_S](#nvc_temperature_timer_s)|
#define NVC_SET_TEMPERATURE_TIMER_RESP			48	//| 设置温度定时采集上报响应信息|无|

#define NVC_QUERY_TEMPERATURE_VLAUE_REQ			49	//| 查询当前温度值|无|
#define NVC_QUERY_TEMPERATURE_VLAUE_RESP		50	//| 查询当前温度值响应信息|[Nvc_Temperature_Value_S](#nvc_temperature_value_s)|

#define NVC_REPORT_TEMPERATURE_VLAUE_MSG		51	//| 上报当前温度值消息|[Nvc_Temperature_Value_S](#nvc_temperature_value_s)|

#define NVC_SET_HUMIDITY_TIMER_REQ				53	//| 设置湿度度定时采集上报请求|[Nvc_Humidity_Timer_S](#nvc_humidity_timer_s)|
#define NVC_SET_HUMIDIT_TIMER_RESP				54	//| 设置湿度定时采集上报响应信息|无|

#define NVC_QUERY_HUMIDIT_VLAUE_REQ				55	//| 查询当前湿度值|无|
#define NVC_QUERY_HUMIDIT_VLAUE_RESP			56	//| 查询当前湿度值响应信息|[Nvc_Humidity_Value_S](#nvc_humidity_value_s)|

#define NVC_REPORT_HUMIDIT_VLAUE_MSG			57	//| 上报当前湿度值消息|[Nvc_Humidity_Value_S](#nvc_humidity_value_s)|

#define NVC_GPIO_RESET_REQ						59	//| 通过GPIO复位系统	|无|
#define NVC_GPIO_RESET_RESP						60	//| 通过GPIO复位系统响应信息|无|

#define NVC_CONTROL_LENS_SWITCH_REQ				61	//| 设置双镜头切换(日用镜头/夜用镜头)	Nvc_Lens_Control_S
#define NVC_CONTROL_LENS_SWITCH_RESP			62	//| 设置双镜头切换响应信息	无
#define NVC_QUERY_LENS_STATUS_REQ				63	//| 查询双镜头使用状态	无
#define NVC_QUERY_LENS_STATUS_RESP				64	//| 查询双镜头使用状态响应信息	Nvc_Lens_Status_S

#define	NVC_REPORT_PTZ_INFO_MSG					65  //| 上报云台信息	Nvc_Ptz_Info_S


// 
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

//  -------------------------------> Message center 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- 
#define DC_MsgCenter_MaxMesgLen		256
#define DC_MsgCenter_MsgHeaderSize	12
#define DC_MsgCenter_MagicWord	0xCA83

#define DC_DrvSend_Initiative		0x80
#define DC_DrvSend_Passive			0x00


typedef struct{
	uint16 	aMagicWord;
	uint16 	aMsgTypes;
	uint16 	aMsgLen;
	uint8 	aUnit;
	uint8 	aErrCode;
	uint32 	aRemain;
}mCmdHead;


//  -------------------------------> Driver Version information
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
//	NvChipType aChipInfo;
	uint32 aChipInfo;
//	NvDeviceType aDeviceInfo;
	uint32 aDeviceInfo;
	uint8 aVerInfo[16];
	uint8 aBuildData[32];
}mNVC_DRV_Ver_INFO;

//  -------------------------------> Driver Support capability for product 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint32 aDrvCapMask;
	uint8 aNumOfButton;
	uint8 aNumOfStaLED;
	uint8 aRemin[2];
}mNVC_DRV_CAP_INFO;


//  -------------------------------> Subscription Drv Msg 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8	aAttched;
	uint8	aRemain;
}mNVC_ATTACHED_Msg;
#define DC_NVCMD_ATTACHED_SubMsg 	0x01
#define DC_NVCMD_ATTACHED_UnsubMsg 	0x00


//  -------------------------------> ButtonMonitor
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8	aStatus;
	uint8	aRemain[3];
}mNVC_BUTTON_STATUS;

#define DC_NVBUTT_ButtonUp			0x01
#define DC_NVBUTT_ButtonDown		0x02
#define DC_NVBUTT_ButtonRaise		0x04
#define DC_NVBUTT_ButtonFall		0x08

#define DC_NVCCMD_Button1			0x01
//#define DC_NVCCMD_Button2			0x02

//  -------------------------------> AudioPlug 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_AUDIOPLUG_Msg;
#define DC_NVAuPl_Off				0x00
#define DC_NVAuPl_On				0x01


//  -------------------------------> DNMonitor 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_DNMONITOR_STATUS;
#define DC_NVANMonitor_NIGHT	0x00
#define DC_NVANMonitor_DAY		0x01

//  -------------------------------> IfrFilter 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint32 aType;
}mNVC_IRFILT_TYPE;

typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_IRFILT_STATUS;
#define DC_NVIfrFilt_PassInfr	0x01
#define DC_NVIfrFilt_BlockInfr	0x00

//  -------------------------------> IfrLight 
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8 aStatus;
	uint8 aRemain[3];
}mNVC_IRLIGHT_STATUS;
#define DC_NVIrLight_ON			0x01
#define DC_NVIrLight_OFF		0x00


//  -------------------------------> State light
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8 aLightID;
	uint32 aOnMes;
	uint32 aOffMes;
}mNVC_STALIGHT_SETINFO;
#define DC_LED_Green1_B	0
#define DC_LED_Green2_B	1
#define DC_LED_Red1_B	2	


//  -------------------------------> Double Lens
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8 aState;
	uint8 aRemain[3];
}mNVC_DUBLENS_SETINFO;
#define DC_DUBLENS_NIGHTLENS	0x01
#define DC_DUBLENS_DAYLENS		0x00

//  -------------------------------> Temperature monitor
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint32 aTimes;
}mNVC_TEMPMONITOR_Timer;

typedef struct{
	int32 aValue;
}mNVC_TEMPMONITOR_Value;


//  -------------------------------> Humidity monitor
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint32 aTimes;
}mNVC_HUMIDITY_Timer;

typedef struct{
	int32 aValue;
}mNVC_HUMIDITY_Value;


//  -------------------------------> Temperature monitor
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	uint8	aStatus;
	uint8 	aLemLevel;
	uint8 	aRemain[2];
}mNVC_NIGHTLIGHT_STATUS;


//  -------------------------------> Temperature monitor
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// defined by PanTilt.h
#define	NVC_PTZ_SUPP_HMOVE  	 	0x00000001  // 是否支持水平运动
#define	NVC_PTZ_SUPP_VMOVE  	 	0x00000002  // 是否支持垂直运动
#define	NVC_PTZ_SUPP_HVMOVE 	 	0x00000004  // 是否支持水平垂直叠加运动( 是否支持左上,左下, 右上, 右下命令 )
#define	NVC_PTZ_SUPP_HSCAN  	 	0x00000008  // 是否支持水平自动扫描
#define	NVC_PTZ_SUPP_VSCAN  	 	0x00000010  // 是否支持垂直自动扫描
#define	NVC_PTZ_SUPP_HLIMIT 	 	0x00000020  // 是否支持水平限位设置
#define	NVC_PTZ_SUPP_VLIMIT 	 	0x00000040  // 是否支持垂直限位设置
#define	NVC_PTZ_SUPP_ZERO   	 	0x00000080  // 是否支持零位检测/设置
#define	NVC_PTZ_SUPP_CURPOS 	 	0x00000100  // 是否支持获取当前云台位置
#define	NVC_PTZ_SUPP_ZOOM   	 	0x00000200  // 是否支持变倍
#define	NVC_PTZ_SUPP_FOCUS  	 	0x00000400  // 是否支持手动聚焦
#define	NVC_PTZ_SUPP_PRESET 	 	0x00000800  // 是否支持预置位
#define	NVC_PTZ_SUPP_CRUISE 	 	0x00001000  // 是否支持预置位巡航

#define NVC_PTZ_STATUS_Initing		0x00000001	// 设备初始化完成
#define NVC_PTZ_STATUS_Busy			0x00000002	//
#define NVC_PTZ_STATUS_CmdFull		0x00000004

typedef struct{
	uint32 aCmdMask;
	uint32 aStatus;
	uint32 aHRange;
	uint32 aDgrPerHSteps;
	uint32 aUnitHSteps;
	uint32 aVRange;
	uint32 aDgrPerVSteps;
	uint32 aUnitVSteps;
	uint32 aZeroXPos;
	uint32 aZeroYPos;
	uint32 aXPos;
	uint32 aYPos;
}mNVC_PANTILT_INFO;


////////////////////////////////////////////////////////////////////////////////
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
#define NV_PTZ_Cordin			255  // 坐标                                  //
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
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
#define NV_PTZ_STA_Success 			0x01                                      //
#define NV_PTZ_STA_RemainSpace		0x02                                      //
typedef struct{                                                               //
	uint8  aStatus;                                                           //
	uint8  aRemain[3];                                                        //
	uint32 aXPos;                                                             //
	uint32 aYPos;                                                             //
}mNVC_PANTILT_Respons;                                                        //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ////////  ////////  ////////  ////////  ////////  ////////  //////// ////////
////////////////////////////////////////////////////////////////////////////////  
extern int32 gfInitDrv_MsgCenter(uint8 iRemain);       						  //
extern int32 gfUninitDrv_MsgCenter(uint8 iRemain);     						  //
//                                                                            //
//                                                                            //
//extern int32 gfMsgCenter_GetMsgHeader(void *iTar,mCmdHead *iCmdHeader);     //
extern int32 gfMsgCenter_Processing(uint32 iUID,void *iCmdBuf,uint16 iBufLen);//
//////////////////////////////////////////////////////////////////////////////// 


#endif
