#ifndef __NVC_DEVICE_API_H__
#define __NVC_DEVICE_API_H__

#pragma pack(push, 4)

#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <errno.h>

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned char           NV_U8;
typedef unsigned short          NV_U16;
typedef unsigned int            NV_U32;

typedef signed char             NV_S8;
typedef short                   NV_S16;
typedef int                     NV_S32;

typedef int                     NV_STATUS;
typedef unsigned int            NV_HANDLE;

#ifndef _M_IX86
    typedef unsigned long long  NV_U64;
    typedef long long           NV_S64;
#else
    typedef __int64             NV_U64;
    typedef __int64             NV_S64;
#endif

typedef char                    NV_CHAR;
typedef long                    NV_LONG;
typedef float                   NV_FLOAT;
typedef double                  NV_DOUBLE;

#define NV_SUCCESS  0
#define NV_FAILURE  (-1)
#define DEBUG_FM1288	0

#define HEXCHK(c)		(((c) >= '0' && (c) <= '9') 			\
							|| ((c) >= 'a' && (c) <= 'f') 		\
							|| ((c) >= 'A' && (c) <= 'F'))

void nv_debug_message(NV_CHAR *msg, NV_CHAR *file, NV_S32 line, ...);
void nv_warn_message(NV_CHAR *msg, NV_CHAR *file, NV_S32 line, ...);
void nv_error_message(NV_CHAR *msg, NV_CHAR *file, NV_S32 line, ...);

#define NV_DEBUG(msg, ...) (nv_debug_message(msg, __FILE__, __LINE__ , ## __VA_ARGS__))

#define NV_WARN(msg, ...) (nv_warn_message(msg, __FILE__, __LINE__ , ## __VA_ARGS__))

#define NV_ERROR(msg, ...) (nv_error_message(msg, __FILE__, __LINE__ , ## __VA_ARGS__))

#define NV_PLGIN_ENABLE_DEBUG	1

#if NV_PLGIN_ENABLE_DEBUG
#define PLGIN_DEBUG(msg, ...) NV_DEBUG(msg, ## __VA_ARGS__)
#else
#define PLGIN_DEBUG(msg, ...)
#endif

#define NV_DRIVER_MSG_MAGIC_NUM		0xCA83 //51843
#define NV_CRUISE_PRESET_NUM 		16
#define NV_DRIVER_DEVICE       "/dev/NV_Driver"

typedef enum __NvcDriverErrCode
{
	NVC_DRIVER_SUCCESS			= 0, 
	NVC_DRIVER_ERR				= 1, // ͨ�ô���
	NVC_DRIVER_ERR_INIT 		= 2, // ��ʼ���豸����
	NVC_DRIVER_ERR_PARAM		= 3, // �����������
	NVC_DRIVER_ERR_BUSY 		= 4, // ������æ
	NVC_DRIVER_ERR_NOT_SUPPORT	= 5, // ��֧�ֵĲ���( �����豸֧�ֺ���ƣ�����֧�ֲ�ѯ�����״̬)
	NVC_DRIVER_ERR_INVALID		= 6, // ��Ч�豸
	NVC_DRIVER_ERR_UNAVAILABLE	= 7, // �豸��ʧ�����������ӶϿ�
	NVC_DRIVER_ERR_UNFINISHED   = 8, // ����δ���( ����̨�����·�����������ǰ���˶�����)
	NVC_DRIVER_ERR_LIMITED		= 9, // ����̨�ѵ��˵����λ����������Ч
}NvcDriverErrCode_E;

typedef enum __NvcChipType
{
    NVC_CHIP_3518C = 0x3518c,
    NVC_CHIP_3518E = 0x3518e,
    NVC_CHIP_BUTT
}NvcChipType_E;

typedef enum __NvcDeviceType
{
    NVC_DEVICE_D01 = 0xd01, // ����netview ����
    NVC_DEVICE_D02 = 0xd02, // ��δ����
    NVC_DEVICE_D03 = 0xd03, // DOREL MO136 ����
    NVC_DEVICE_D04 = 0xd04, // Բ��netview ����
    NVC_DEVICE_D11 = 0xd11, // Awox ���ݻ���
    NVC_DEVICEBUTT
}NvcDeviceType_E;

typedef enum __NvcButtonStatus
{
    Nvc_ButtonStatusUp 		= 0x01, 
    Nvc_ButtonStatusDown	= 0x02,
    Nvc_ButtonActionRaise 	= 0x04,
    Nvc_ButtonActionFall 	= 0x08,
}NvcButtonStatus_E;

typedef enum __NvcDriverMsgOldType
{
    NVC_OLD_QUERY_DRIVER_INFO_REQ           = 1,    // ��ȡ������Ϣ (оƬ�����ͻ��ͣ��汾�ŵ�)
    NVC_OLD_QUERY_DRIVER_INFO_RESP          = 2,    // ��ȡ������Ϣ��Ӧ��Ϣ
    NVC_OLD_QUERY_DRIVER_CAPACITY_REQ       = 3,    // ��ȡ�豸����������
    NVC_OLD_QUERY_DRIVER_CAPACITY_RESP      = 4,    // ��ȡ�豸������������Ӧ��Ϣ
    NVC_OLD_SET_ATTACHED_DRIVER_MSG_REQ     = 5,    // �����Ƿ���������¼���״̬�仯����
    NVC_OLD_SET_ATTACHED_DRIVER_MSG_RESP    = 6,    // �����Ƿ���������¼���״̬�仯��Ϣ  
    NVC_OLD_QUERY_BUTTON_STATUS_REQ         = 7,    // ��ȡbutton��ǰ״̬
    NVC_OLD_QUERY_BUTTON_STATUS_RESP        = 8,    // ��ȡbutton��ǰ״̬��Ӧ��Ϣ
    NVC_OLD_REPORT_BUTTON_STATUS_MSG        = 9,    // �ϱ�button��ǰ״̬  
    NVC_OLD_QUERY_LDR_STATUS_REQ            = 11,   // ��ȡ�������赱ǰ״̬ (0 ���� 1 ��ҹ)
    NVC_OLD_QUERY_LDR_STATUS_RESP           = 12,   // ��ȡ�������赱ǰ״̬��Ӧ��Ϣ
    NVC_OLD_REPORT_LDR_STATUS_MSG           = 13,   // �ϱ��������赱ǰ״̬
    NVC_OLD_SET_LDR_SENSITIVITY_REQ         = 15,   // ���ù���������������
    NVC_OLD_SET_LDR_SENSITIVITY_RESP        = 16,   // ���ù�����������������Ӧ��Ϣ
    NVC_OLD_QUERY_LDR_SENSITIVITY_REQ       = 17,   // ��ȡ����������������
    NVC_OLD_QUERY_LDR_SENSITIVITY_RESP      = 18,   // ��ȡ������������������Ӧ��Ϣ
    NVC_OLD_QUERY_IRC_TYPE_REQ              = 19,   // ��ȡ�豸ircut����
    NVC_OLD_QUERY_IRC_TYPE_RESP             = 20,   // ��ȡ�豸ircut������Ӧ��Ϣ
    NVC_OLD_CONTROL_IRC_SWITCH_REQ          = 21,   // ����ircut�л�״̬
    NVC_OLD_CONTROL_IRC_SWITCH_RESP         = 22,   // ����ircut�л�״̬��Ӧ��Ϣ
    NVC_OLD_QUERY_IRC_STATUS_REQ            = 23,   // ��ѯircut��ǰ״̬
    NVC_OLD_QUERY_IRC_STATUS_RESP           = 24,   // ��ѯircut��ǰ״̬��Ӧ��Ϣ
    NVC_OLD_CONTROL_LAMP_SWITCH_REQ         = 25,   // ���ú���ƴ�/�ر�
    NVC_OLD_CONTROL_LAMP_SWITCH_RESP        = 26,   // ���ú���ƴ�/�ر���Ӧ��Ϣ
    NVC_OLD_QUERY_LAMP_STATUS_REQ           = 27,   // ��ѯ����ƴ�/�ر�״̬
    NVC_OLD_QUERY_LAMP_STATUS_RESP          = 28,   // ��ѯ����ƴ�/�ر�״̬��Ӧ��Ϣ
    NVC_OLD_CONTROL_STATE_LED_REQ           = 29,   // ����LED����ʾ��ʽ
    NVC_OLD_CONTROL_STATE_LED_RESP          = 30,   // ����LED����ʾ��ʽ��Ӧ��Ϣ
    NVC_OLD_QUERY_PTZ_INFO_REQ              = 31,   // ��̨��Ϣ��ѯ
    NVC_OLD_QUERY_PTZ_INFO_RESP             = 32,   // ��̨��Ϣ��ѯ��Ӧ��Ϣ
    NVC_OLD_CONTROL_PTZ_COMMON_REQ          = 33,   // ͨ����̨����ָ��
    NVC_OLD_CONTROL_PTZ_COMMON_RESP         = 34,   // ͨ����̨����ָ����Ӧ��Ϣ
    NVC_OLD_SET_PRESET_CRUISE_REQ           = 35,   // ������̨Ԥ��λѲ��
    NVC_OLD_SET_PRESET_CRUISE_RESP          = 36,   // ������̨Ԥ��λѲ����Ӧ��Ϣ
    NVC_OLD_CLEAR_PRESET_CRUISE_REQ         = 37,   // �����̨Ԥ��λѲ��
    NVC_OLD_CLEAR_PRESET_CRUISE_RESP        = 38,   // �����̨Ԥ��λѲ����Ӧ��Ϣ
    NVC_OLD_CONTROL_NIGHT_LIGHT_SWITCH_REQ  = 39,   // ����Сҹ�ƴ�/�ر�
    NVC_OLD_CONTROL_NIGHT_LIGHT_SWITCH_RESP = 40,   // ����Сҹ�ƴ�/�ر���Ӧ��Ϣ 
    NVC_OLD_QUERY_NIGHT_LIGHT_STATUS_REQ    = 41,   // ��ѯСҹ�ƴ�/�ر�״̬
    NVC_OLD_QUERY_NIGHT_LIGHT_STATUS_RESP   = 42,   // ��ѯСҹ�ƴ�/�ر�״̬��Ӧ��Ϣ
    NVC_OLD_CONTROL_AUDIO_PLUG_SWITCH_REQ   = 43,   // ������Ƶ��������/�ر�
    NVC_OLD_CONTROL_AUDIO_PLUG_SWITCH_RESP  = 44,   // ������Ƶ��������/�ر���Ӧ��Ϣ 
    NVC_OLD_QUERY_AUDIO_PLUG_STATUS_REQ     = 45,   // ��ѯ��Ƶ��������/�ر�״̬
    NVC_OLD_QUERY_AUDIO_PLUG_STATUS_RESP    = 46,   // ��ѯ��Ƶ��������/�ر�״̬��Ӧ��Ϣ
    NVC_OLD_SET_TEMPERATURE_TIMER_REQ       = 47,   // �����¶ȶ�ʱ�ɼ��ϱ�����
    NVC_OLD_SET_TEMPERATURE_TIMER_RESP      = 48,   // �����¶ȶ�ʱ�ɼ��ϱ���Ӧ��Ϣ   
    NVC_OLD_QUERY_TEMPERATURE_VALUE_REQ     = 49,   // ��ѯ�����¶�ֵ
    NVC_OLD_QUERY_TEMPERATURE_VALUE_RESP    = 50,   // ��ѯ�����¶�ֵ��Ӧ��Ϣ
    NVC_OLD_REPORT_TEMPERATURE_VALUE_MSG    = 51,   // �ϱ���ǰ�¶�ֵ��Ϣ 
    NVC_OLD_SET_HUMIDITY_TIMER_REQ          = 53,   // ����ʪ�ȶȶ�ʱ�ɼ��ϱ�����
    NVC_OLD_SET_HUMIDIT_TIMER_RESP          = 54,   // ����ʪ�ȶ�ʱ�ɼ��ϱ���Ӧ��Ϣ
    NVC_OLD_QUERY_HUMIDIT_VALUE_REQ         = 55,   // ��ѯ����ʪ��ֵ
    NVC_OLD_QUERY_HUMIDIT_VALUE_RESP        = 56,   // ��ѯ����ʪ��ֵ��Ӧ��Ϣ    
    NVC_OLD_REPORT_HUMIDIT_VALUE_MSG        = 57,   // �ϱ���ǰʪ��ֵ��Ϣ 
    NVC_OLD_GPIO_RESET_REQ                  = 59,   // ͨ��GPIO��λϵͳ 
    NVC_OLD_GPIO_RESET_RESP                 = 60,   // ͨ��GPIO��λϵͳ��Ӧ��Ϣ
    NVC_OLD_CONTROL_LENS_SWITCH_REQ         = 61,   // ����˫��ͷ�л�(���þ�ͷ/ҹ�þ�ͷ)
    NVC_OLD_CONTROL_LENS_SWITCH_RESP        = 62,   // ����˫��ͷ��Ӧ��Ϣ
    NVC_OLD_QUERY_LENS_STATUS_REQ           = 63,   // ��ѯ˫��ͷʹ��״̬
    NVC_OLD_QUERY_LENS_STATUS_RESP          = 64,   // ��ѯ˫��ͷʹ��״̬��Ӧ��Ϣ
    NVC_OLD_REPORT_PTZ_INFO_MSG				= 65,	// �ϱ���̨��Ϣ
}NvcDriverMsgOldType_E;

typedef enum __NvcDriverMsgType
{
    NVC_QUERY_DRIVER_INFO_REQ            = 0X0001,  // ��ȡ������Ϣ (оƬ�����ͻ��ͣ��汾�ŵ�)
    NVC_QUERY_DRIVER_INFO_RESP           = 0X0002,  // ��ȡ������Ϣ��Ӧ��Ϣ
    NVC_QUERY_DRIVER_CAPACITY_REQ        = 0X0003,  // ��ȡ�豸����������
    NVC_QUERY_DRIVER_CAPACITY_RESP       = 0X0004,  // ��ȡ�豸������������Ӧ��Ϣ
    NVC_SET_ATTACHED_DRIVER_MSG_REQ      = 0X0005,  // �����Ƿ���������¼���״̬�仯����
    NVC_SET_ATTACHED_DRIVER_MSG_RESP     = 0X0006,  // �����Ƿ���������¼���״̬�仯��Ϣ  
	NVC_QUERY_BUTTON_STATUS_REQ          = 0X0101,  // ��ȡbutton��ǰ״̬
    NVC_QUERY_BUTTON_STATUS_RESP         = 0X0102,  // ��ȡbutton��ǰ״̬��Ӧ��Ϣ
    NVC_REPORT_BUTTON_STATUS_MSG         = 0X0103,  // �ϱ�button��ǰ״̬  
    NVC_QUERY_LDR_STATUS_REQ             = 0X0201,  // ��ȡ�������赱ǰ״̬ (0 ���� 1 ��ҹ)
    NVC_QUERY_LDR_STATUS_RESP            = 0X0202,  // ��ȡ�������赱ǰ״̬��Ӧ��Ϣ
    NVC_REPORT_LDR_STATUS_MSG            = 0X0203,  // �ϱ��������赱ǰ״̬
    NVC_SET_LDR_SENSITIVITY_REQ          = 0X0205,  // ���ù���������������
    NVC_SET_LDR_SENSITIVITY_RESP         = 0X0206,  // ���ù�����������������Ӧ��Ϣ
    NVC_QUERY_LDR_SENSITIVITY_REQ        = 0X0207,  // ��ȡ����������������
    NVC_QUERY_LDR_SENSITIVITY_RESP       = 0X0208,  // ��ȡ������������������Ӧ��Ϣ
    NVC_QUERY_IRC_TYPE_REQ               = 0X0301,  // ��ȡ�豸ircut����
    NVC_QUERY_IRC_TYPE_RESP              = 0X0302,  // ��ȡ�豸ircut������Ӧ��Ϣ
    NVC_CONTROL_IRC_SWITCH_REQ           = 0X0303,  // ����ircut�л�״̬
    NVC_CONTROL_IRC_SWITCH_RESP          = 0X0304,  // ����ircut�л�״̬��Ӧ��Ϣ
    NVC_QUERY_IRC_STATUS_REQ             = 0X0305,  // ��ѯircut��ǰ״̬
    NVC_QUERY_IRC_STATUS_RESP            = 0X0306,  // ��ѯircut��ǰ״̬��Ӧ��Ϣ
    NVC_CONTROL_LAMP_SWITCH_REQ          = 0X0401,  // ���ú���ƴ�/�ر�
    NVC_CONTROL_LAMP_SWITCH_RESP         = 0X0402,  // ���ú���ƴ�/�ر���Ӧ��Ϣ
    NVC_QUERY_LAMP_STATUS_REQ            = 0X0403,  // ��ѯ����ƴ�/�ر�״̬
    NVC_QUERY_LAMP_STATUS_RESP           = 0X0404,  // ��ѯ����ƴ�/�ر�״̬��Ӧ��Ϣ
    NVC_CONTROL_STATE_LED_REQ            = 0X0501,  // ����LED����ʾ��ʽ
    NVC_CONTROL_STATE_LED_RESP           = 0X0502,  // ����LED����ʾ��ʽ��Ӧ��Ϣ
    NVC_QUERY_PTZ_INFO_REQ               = 0X0601,  // ��̨��Ϣ��ѯ
    NVC_QUERY_PTZ_INFO_RESP              = 0X0602,  // ��̨��Ϣ��ѯ��Ӧ��Ϣ
    NVC_CONTROL_PTZ_COMMON_REQ           = 0X0603,  // ͨ����̨����ָ��
    NVC_CONTROL_PTZ_COMMON_RESP          = 0X0604,  // ͨ����̨����ָ����Ӧ��Ϣ
    NVC_SET_PRESET_CRUISE_REQ            = 0X0605,  // ������̨Ԥ��λѲ��
    NVC_SET_PRESET_CRUISE_RESP           = 0X0606,  // ������̨Ԥ��λѲ����Ӧ��Ϣ
    NVC_CLEAR_PRESET_CRUISE_REQ          = 0X0607,  // �����̨Ԥ��λѲ��
    NVC_CLEAR_PRESET_CRUISE_RESP         = 0X0608,  // �����̨Ԥ��λѲ����Ӧ��Ϣ
    NVC_REPORT_PTZ_INFO_MSG              = 0X0609,  // �ϱ���̨��Ϣ
    NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ   = 0X0701,  // ����Сҹ�ƴ�/�ر�
    NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP  = 0X0702,  // ����Сҹ�ƴ�/�ر���Ӧ��Ϣ 
    NVC_QUERY_NIGHT_LIGHT_STATUS_REQ     = 0X0703,  // ��ѯСҹ�ƴ�/�ر�״̬
    NVC_QUERY_NIGHT_LIGHT_STATUS_RESP    = 0X0704,  // ��ѯСҹ�ƴ�/�ر�״̬��Ӧ��Ϣ
    NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ    = 0X0801,  // ������Ƶ��������/�ر�
    NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP   = 0X0802,  // ������Ƶ��������/�ر���Ӧ��Ϣ 
    NVC_QUERY_AUDIO_PLUG_STATUS_REQ      = 0X0803,  // ��ѯ��Ƶ��������/�ر�״̬
    NVC_QUERY_AUDIO_PLUG_STATUS_RESP     = 0X0804,  // ��ѯ��Ƶ��������/�ر�״̬��Ӧ��Ϣ
    NVC_SET_TEMPERATURE_TIMER_REQ        = 0X0901,  // �����¶ȶ�ʱ�ɼ��ϱ�����
    NVC_SET_TEMPERATURE_TIMER_RESP       = 0X0902,  // �����¶ȶ�ʱ�ɼ��ϱ���Ӧ��Ϣ   
    NVC_QUERY_TEMPERATURE_VALUE_REQ      = 0X0903,  // ��ѯ�����¶�ֵ
    NVC_QUERY_TEMPERATURE_VALUE_RESP     = 0X0904,  // ��ѯ�����¶�ֵ��Ӧ��Ϣ
    NVC_REPORT_TEMPERATURE_VALUE_MSG     = 0X0905,  // �ϱ���ǰ�¶�ֵ��Ϣ 
    NVC_SET_HUMIDITY_TIMER_REQ           = 0X0A01,  // ����ʪ�ȶȶ�ʱ�ɼ��ϱ�����
    NVC_SET_HUMIDIT_TIMER_RESP           = 0X0A02,  // ����ʪ�ȶ�ʱ�ɼ��ϱ���Ӧ��Ϣ
    NVC_QUERY_HUMIDIT_VALUE_REQ          = 0X0A03,  // ��ѯ����ʪ��ֵ
    NVC_QUERY_HUMIDIT_VALUE_RESP         = 0X0A04,  // ��ѯ����ʪ��ֵ��Ӧ��Ϣ    
    NVC_REPORT_HUMIDIT_VALUE_MSG         = 0X0A05,  // �ϱ���ǰʪ��ֵ��Ϣ 
    NVC_CONTROL_LENS_SWITCH_REQ          = 0X0B01,  // ����˫��ͷ�л�(���þ�ͷ/ҹ�þ�ͷ)
    NVC_CONTROL_LENS_SWITCH_RESP         = 0X0B02,  // ����˫��ͷ��Ӧ��Ϣ
    NVC_QUERY_LENS_STATUS_REQ            = 0X0B03,  // ��ѯ˫��ͷʹ��״̬
    NVC_QUERY_LENS_STATUS_RESP           = 0X0B04,  // ��ѯ˫��ͷʹ��״̬��Ӧ��Ϣ
    NVC_GPIO_RESET_REQ                   = 0x0C01,  // ͨ��GPIO��λϵͳ 
    NVC_GPIO_RESET_RESP                  = 0x0C02,  // ͨ��GPIO��λϵͳ��Ӧ��Ϣ
    NVC_CONTROL_DOORBELL_SET_REQ	 	 = 0x0F01,	// ���������
    NVC_CONTROL_DOORBELL_SET_RESP	 	 = 0x0F02,	// �����������Ӧ��Ϣ
    NVC_CONTROL_DOORLOCK_SET_REQ	 	 = 0x1001,	// ����������/�ر�
    NVC_CONTROL_DOORLOCK_SET_RESP	 	 = 0x1002,	// ����������/�ر���Ӧ��Ϣ
    NVC_CONTROL_DOORLOCK_GET_REQ	 	 = 0x1003,	// ��ѯ����״̬��/�ر�
	NVC_CONTROL_DOORLOCK_GET_RESP	 	 = 0x1004,	// ��ѯ����״̬��/�ر���Ӧ��Ϣ
    NVC_CONTROL_FM1288_WRITE_REQ	 	 = 0x1101,	// �޸�FM1288�Ĵ�����ֵ
	NVC_CONTROL_FM1288_WRITE_RESP	 	 = 0x1102,	// �޸�FM1288�Ĵ�����ֵ��Ӧ��Ϣ
	NVC_CONTROL_FM1288_READ_REQ		 	 = 0x1103,	// ��ȡFM1288�Ĵ�����ֵ
	NVC_CONTROL_FM1288_READ_RESP	 	 = 0x1104,	// ��ȡFM1288�Ĵ�����ֵ��Ӧ��Ϣ
}NvcDriverMsgType_E;

typedef struct __NvcDriverMsgHdr
{
	NV_U16 u16Magic;
	NV_U16 u16MsgType;
	NV_U16 u16MsgLen;
	NV_U8  u8DevNo;
	NV_U8  u8ErrCode;
	NV_U8  u8Res[4];
}Nvc_Driver_Msg_Hdr_S;

typedef struct __Nvc_Driver_Ver_Info
{
    uint32  u32ChipType; // NvcChipType_E
    uint32  u32DeviceType; // NvcDeviceType_E
    char    szVerInfo[16];
    char    szBuildData[32];
}Nvc_Driver_Ver_Info_S;

typedef enum __NvcDriverCap
{
    NVC_SUPP_ButtonMonitor  = 0x00000001, // ���Ƿ�֧�ְ������
    NVC_SUPP_LdrMonitor 	= 0x00000002, // ���Ƿ�֧����ҹģʽ���(light dependent resistors detection) 
    NVC_SUPP_Ircut      	= 0x00000004, // ���Ƿ�֧���˹�Ƭ�л�
    NVC_SUPP_IfrLamp    	= 0x00000008, // ���Ƿ�֧�ֺ����
    NVC_SUPP_DoubleLens 	= 0x00000010, // ���Ƿ�֧��˫��ͷ
    NVC_SUPP_StateLed   	= 0x00000020, // ���Ƿ�֧��״̬����ʾ
    NVC_SUPP_PTZ        	= 0x00000040, // ���Ƿ�֧����̨����
    NVC_SUPP_NightLight		= 0x00000080, // ���Ƿ�֧��Сҹ�ƹ���
    NVC_SUPP_CoolFan    	= 0x00000100, // ���Ƿ�֧��ɢ�ȷ��ȹ���
    NVC_SUPP_AudioPlug  	= 0x00000200, // ���Ƿ�֧����Ƶ���ع���
    NVC_SUPP_TempMonitor	= 0x00000400, // ���Ƿ�֧���¶Ȳɼ�����
    NVC_SUPP_HumiMonitor	= 0x00000800, // ���Ƿ�֧��ʪ�Ȳɼ�����
    NVC_SUPP_GpioReset  	= 0x00001000, // ��ͨ��GPIO ��λ�豸(�����豸)
    NVC_SUPP_RTC        	= 0x00002000, // 
	NVC_SUPP_PIR        	= 0x00004000, // 
	NVC_SUPP_DoorBell   	= 0x00008000, // 
	NVC_SUPP_DoorLock   	= 0x00010000, // 
	NVC_SUPP_FM1288     	= 0x00020000, // 
}Nvc_Driver_Cap_E;

typedef struct __Nvc_Driver_Cap_Info
{
    uint32 u32CapMask;
    uint8  u8ButtonCnt;
    uint8  u8LedCnt;
    uint8  u8Res[2];
}Nvc_Driver_Cap_Info_S;

typedef struct __Nvc_Attached_Driver_Msg
{
    uint8   u8Attached; // 0 ����Ҫ�ϱ��¼�/״̬��Ϣ�� 1 ��Ҫ�ϱ��¼�/״̬��Ϣ
    uint8   u8Res[3];
}Nvc_Attached_Driver_Msg_S;

typedef struct __Nvc_Button_Status_S
{
    uint8   u8Status; // NvcButtonStatus_E
    uint8   u8Res[3];
}Nvc_Button_Status_S;

typedef struct __Nvc_Ldr_Status
{
    uint8   u8Status; // 0 ��ҹ�� 1 ����
    uint8   u8Res[3];
}Nvc_Ldr_Status_S;

typedef struct __Nvc_Ldr_Senitivity
{
    uint8   u8Level; // 0 �������ȣ� 1 �������� 2 �������� (������Խ�ߣ�Խ�����л�����ҹ״̬)
    uint8   u8Res[3];
}Nvc_Ldr_Senitivity_S;

typedef struct __Nvc_Ircut_Info
{
    uint32 u32IrcType; // ĿǰΪ0�� Ĭ������
}Nvc_Ircut_Info_S;

typedef struct __Nvc_Ircut_Control
{
    uint8   u8Status; // 0 �����ֹ״̬��1 ͨ����״̬
    uint8   u8Res[3];
}Nvc_Ircut_Control_S;

typedef struct __Nvc_Ircut_Status
{
    uint8   u8Status; // 0 �����ֹ״̬��1 ͨ����״̬
    uint8   u8Res[3];
}Nvc_Ircut_Status_S;

typedef struct __Nvc_Lamp_Control
{
    uint8   u8Switch; // 0 �رպ���ƣ�1 �򿪺����
    uint8   u8Res[3];
}Nvc_Lamp_Control_S;

typedef struct __Nvc_Lamp_Status
{
    uint8   u8Status; // 0 �رպ���ƣ�1 �򿪺����
    uint8   u8Res[3];
}Nvc_Lamp_Status_S;


typedef struct __Nvc_Lens_Control
{
    uint8   u8SwitchLens; // 0 ʹ�����þ�ͷ��1 ʹ��ҹ�þ�ͷ
    uint8   u8Res[3];
}Nvc_Lens_Control_S;

typedef struct __Nvc_Lens_Status
{
    uint8   u8CurLens; // 0 ʹ�����þ�ͷ��1 ʹ��ҹ�þ�ͷ
    uint8   u8Res[3];
}Nvc_Lens_Status_S;

typedef enum __Nvc_State_Led_Color
{
    NV_LED_COLOR_DEFAULT    = 1,
    NV_LED_COLOR_RED        = 2,
    NV_LED_COLOR_GREEN      = 3,
}Nvc_State_Led_Color_E;

typedef struct __Nvc_State_Led_Control
{
    Nvc_State_Led_Color_E eColor;
    uint32  u32OnMesl; // ����ʱ�䣬��λms
    uint32  u32OffMesl;// ���ʱ�䣬��λms
}Nvc_State_Led_Control_S;

typedef enum __NvcPtzCap
{
    NVC_PTZ_SUPP_HMOVE  = 0x00000001, // �Ƿ�֧��ˮƽ�˶�
	NVC_PTZ_SUPP_VMOVE	= 0x00000002, // �Ƿ�֧�ִ�ֱ�˶�
	NVC_PTZ_SUPP_HVMOVE	= 0x00000004, // �Ƿ�֧��ˮƽ��ֱ�����˶�( �Ƿ�֧������,����, ����, �������� )
	NVC_PTZ_SUPP_HSCAN	= 0x00000008, // �Ƿ�֧��ˮƽ�Զ�ɨ��
	NVC_PTZ_SUPP_VSCAN	= 0x00000010, // �Ƿ�֧�ִ�ֱ�Զ�ɨ��
	NVC_PTZ_SUPP_HLIMIT = 0x00000020, // �Ƿ�֧��ˮƽ��λ����
	NVC_PTZ_SUPP_VLIMIT = 0x00000040, // �Ƿ�֧�ִ�ֱ��λ����
	NVC_PTZ_SUPP_ZERO	= 0x00000080, // �Ƿ�֧����λ���/����
	NVC_PTZ_SUPP_CURPOS	= 0x00000100, // �Ƿ�֧�ֻ�ȡ��ǰ��̨λ��
	NVC_PTZ_SUPP_ZOOM   = 0x00000200, // �Ƿ�֧�ֱ䱶
	NVC_PTZ_SUPP_FOCUS	= 0x00000400, // �Ƿ�֧���ֶ��۽�
	NVC_PTZ_SUPP_PRESET = 0x00000800, // �Ƿ�֧��Ԥ��λ
	NVC_PTZ_SUPP_CRUISE = 0x00001000, // �Ƿ�֧��Ԥ��λѲ��
}Nvc_Ptz_Cap_E;

typedef enum __NvcPtzStatus{
     NVC_PTZ_STATUS_INITIDONE = 0x00000001
}Nvc_Ptz_Status_E;	

typedef struct __Nvc_Ptz_Info
{
	uint32  u32PtzCapMask;			// Nvc_Ptz_Cap_E, ָʾ�豸֧����Щ��̨����
	/*************************    ���¶��壬���λ��Ϊ�Ƿ��ʼ����ʶ,********************************************/
	/*** ��uint32����bit [0~31]������ bit31: 0x1 ��ʾδ��ʼ����ɣ�0x0 ��ʾ�ѳ�ʼ����ɻ�ȡ��ʵ�ʲ���ֵ ***/
	//
	uint32	u32Status;
	
	uint32	u32HorizontalTotSteps;	// ˮƽ�����ܲ���,  ( u32Cap & NVC_PTZ_SUPP_HMOVE ) ��Ч
									// �����һֱ����/ ���˶����޷���ȡ�ܲ���, ��Ϊ0x7FFFFFFF, 
    uint32  u32HPerStepDegrees; 	// ˮƽ����ת��1  ����Ӧ��̨ת������, ��λ0.0001��, ( u32Cap & NVC_PTZ_SUPP_HMOVE ) ��Ч
	uint32	u32HorizontalMinSteps;  // ˮƽ����ת����С����, ( u32Cap & NVC_PTZ_SUPP_HMOVE ) ��Ч
	//
	uint32	u32VerticalTotSteps;	// ��ֱ�����ܲ���, ( u32Cap & NVC_PTZ_SUPP_VMOVE ) ��Ч
									// �����һֱ����/ ���˶����޷���ȡ�ܲ����� ��Ϊ0x7FFFFFFF, 
    uint32  u32VPerStepDegrees; 	// ��ֱ����ת��1  ����Ӧ��̨ת������, ��λ0.0001��, ( u32Cap & NVC_PTZ_SUPP_VMOVE ) ��Ч
	uint32	u32VerticalMinSteps;    // ��ֱ����ת����С����	 �� ( u32Cap & NVC_PTZ_SUPP_VMOVE ) ��Ч
	//
	uint32  u32ZeroHStepPos; 		// ��̨��λ�����̨���¶˲��� ( u32Cap & NVC_PTZ_SUPP_ZERO & NVC_PTZ_SUPP_HMOVE ) ʱ��Ч
	uint32  u32ZeroVStepPos; 		// ��̨��λ�����̨����˲���( u32Cap & NVC_PTZ_SUPP_ZERO & NVC_PTZ_SUPP_VMOVE ) ʱ��Ч
	uint32  u32CurHStepPos;			// ��̨��ǰλ��, �����̨���¶˲��� ( u32Cap & NVC_PTZ_SUPP_CURPOS & NVC_PTZ_SUPP_HMOVE ) ʱ��Ч
	uint32  u32CurVStepPos;			// ��̨��ǰλ��, �����̨����˲��� ( u32Cap & NVC_PTZ_SUPP_CURPOS & NVC_PTZ_SUPP_VMOVE ) ʱ��Ч
}Nvc_Ptz_Info_S;

typedef struct __Nvc_Ptz_Param
{
    uint32  u32PtzCapMask;
    float   fHPerStepDegrees; 	// ˮƽ����ת��1  ����Ӧ��̨ת������, ��λ1��, ( u32Cap & NVC_PTZ_SUPP_HMOVE ) ��Ч
    float   fVPerStepDegrees; 	// ��ֱ����ת��1  ����Ӧ��̨ת������, ��λ1��, ( u32Cap & NVC_PTZ_SUPP_VMOVE ) ��Ч
	uint32	u32HorizontalMinSteps;  // ˮƽ����ת����С����, ( u32Cap & NVC_PTZ_SUPP_HMOVE ) ��Ч
	uint32	u32VerticalMinSteps;    // ��ֱ����ת����С����	 �� ( u32Cap & NVC_PTZ_SUPP_VMOVE ) ��Ч
}Nvc_Ptz_Param;

typedef enum __Nvc_Ptz_Cmd
{
    NV_PTZ_STOP             = 0, // ��ֹ̨ͣ
    NV_PTZ_UP               = 1, // ��
    NV_PTZ_DOWN             = 2, // ��
    NV_PTZ_LEFT             = 3, // ��
    NV_PTZ_RIGHT            = 4, // ��
    NV_PTZ_LEFT_UP          = 5, // ����
    NV_PTZ_LEFT_DOWN        = 6, // ����
    NV_PTZ_RIGHT_UP         = 7, // ����
    NV_PTZ_RIGHT_DOWN       = 8, // ����
    NV_PTZ_ZOOM_IN          = 9,  // �䱶+
    NV_PTZ_ZOOM_OUT         = 10, // �䱶-
    NV_PTZ_FOCUS_NEAR       = 11, // �۽���
    NV_PTZ_FOCUS_FAR        = 12, // �۽�Զ
    NV_PTZ_AUTO_SCAN        = 13, // ˮƽ�Զ�ɨ��
    NV_PTZ_UP_LIMIT         = 14, // ����λ����
    NV_PTZ_DOWN_LIMIT       = 15, // ����λ����
    NV_PTZ_LEFT_LIMIT       = 16, // ����λ����
    NV_PTZ_RIGHT_LIMIT      = 17, // ����λ����
    NV_PTZ_PRESET_SET       = 18, // ����Ԥ��λ
    NV_PTZ_PRESET_CLR       = 19, // ɾ��Ԥ��λ
    NV_PTZ_PRESET_CALL      = 20, // ����Ԥ��λ
    NV_PTZ_START_CRUISE     = 21, // ��ʼѲ��
    NV_PTZ_STOP_CRUISE      = 22, // ֹͣѲ��
    NV_PTZ_GOTO_ZERO        = 23, // ��λ��⣬��̨��ʼλ��
}Nvc_Ptz_Cmd_E;

typedef struct __Nvc_Ptz_Control_S
{
    uint8   u8PtzCmd;	    // Nvc_Ptz_Cmd_E ��̨����
    union {
        uint8   u8ParaType; // 0: ����, 1 �Ƕ�,Ŀǰ��֧�ֲ�������
        uint8   u8No;       // Ԥ��λ�Ż�Ѳ����
    };
    uint8   u8Speed;	// ˮƽ�ٶ� (1 ~ 100, Ĭ��50) 
    uint8   u8Res;	    // ��ֱ�ٶ� (1 ~ 100, Ĭ��50) 
    uint32  u32HSteps;  // ˮƽ���� 
    uint32  u32VSteps;  // ��ֱ����
}Nvc_Ptz_Control_S;

typedef struct __Nvc_Cruise_PRESET
{
    uint8 u8PresetNo;
    uint8 u8Speed;
    uint16 u16StaySeconds; // ��λ: ��
}Nvc_Cruise_Preset_S;

typedef struct __Nvc_Ptz_Cruise
{
    uint8 u8CruiseNo; // ��0��ʼ
    uint8 u8PresetCnt;
    uint8 u8Res[2];
    Nvc_Cruise_Preset_S stPresets[NV_CRUISE_PRESET_NUM];
}Nvc_Ptz_Cruise_S;

typedef struct __Nvc_Ptz_Cruise_Idx
{
    uint8 u8CruiseNo; // ��0��ʼ
    uint8 u8Res[3];
}Nvc_Ptz_Cruise_Idx_S;

typedef struct __Nvc_Night_Light_Control
{
    uint8   u8Switch; // 0 �ر�Сҹ�ƣ�1 ��Сҹ��
    uint8   u8LumLevel; // ���ȣ��ȼ� 1 - 100
    uint8   u8Res[2];
}Nvc_Night_Light_Control_S;

typedef struct __Nvc_Night_Light_Status
{
    uint8   u8Status; // 0 �ر�Сҹ�ƣ�1 ��Сҹ��
    uint8   u8LumLevel; // ���ȣ��ȼ� 1 - 100
    uint8   u8Res[2];
}Nvc_Night_Light_Status_S;

typedef struct __Nvc_Audio_Plug_Control
{
    uint8   u8Switch; // 0 �ر���Ƶ��������1 ����Ƶ������
    uint8   u8Res[3];
}Nvc_Audio_Plug_Control_S;

typedef struct __Nvc_Audio_Plug_Status
{
    uint8   u8Status; // 0 �رգ�1 ��
    uint8   u8Res[3];
}Nvc_Audio_Plug_Status_S;

typedef struct __Nvc_Temperature_Timer
{
    uint32  u32DistTime; // ���������ɼ�1���¶ȣ�0Ϊ���ɼ� (Ĭ��)
}Nvc_Temperature_Timer_S;

typedef struct __Nvc_Temperature_Value
{
    uint32 u32Temperature; // ��λ: ���϶�0.01
}Nvc_Temperature_Value_S;

typedef struct __Nvc_Humidity_Timer
{
    uint32  u32DistTime; // ���������ɼ�1��ʪ�ȣ�0Ϊ���ɼ� (Ĭ��)
}Nvc_Humidity_Timer_S;

typedef struct __Nvc_Humidity_Value_S
{
    uint32 u32Humidity; // �ٷֱ�,0.01%
}Nvc_Humidity_Value_S;

#define DC_MsgDoorBell_Tap     0x01			// Tap the door bell
typedef struct __Nvc_DoorBell_Control
{
    uint8   u8Switch; // 0 �ر�������1 ������
    uint8   u8Res[3];
}Nvc_DoorBell_Control_S;

#define DC_MsgDoorLock_Close       0x00        // �ر�����
#define DC_MsgDoorLock_Open        0x01        // ������
typedef struct __Nvc_DoorLock_Control
{
    uint8   u8Switch; // 0 �ر�������1 ������
    uint8   u8Res[3];
}Nvc_DoorLock_Control_S;

typedef struct __Nvc_FM1288_Value_S
{
    uint8 *u8Data; // the data which want to write
	uint32 u32DataLength; // data length
	uint8 u8RegHigh; // register address high byte
	uint8 u8RegLow; // register address low byte
}Nvc_FM1288_Control_S;

typedef struct __Nvc_Drv_Message
{
	Nvc_Driver_Msg_Hdr_S stMsgHdr;
	union 
	{
		Nvc_Driver_Ver_Info_S		stVerInfo;
		Nvc_Driver_Cap_Info_S		stCapInfo;
		Nvc_Attached_Driver_Msg_S	stAttachedMsg;
		Nvc_Button_Status_S 		stButtonStatus;
		Nvc_Ldr_Status_S			stLdrStatus;
		Nvc_Ldr_Senitivity_S		stLdrSense;
		Nvc_Ircut_Info_S			stIrcInfo;
		Nvc_Ircut_Control_S 		stIrcCtrl;
		Nvc_Ircut_Status_S			stIrcStatus;
		Nvc_Lamp_Control_S			stLampCtrl;
		Nvc_Lamp_Status_S			stLampStatus;
		Nvc_Lens_Control_S			stLensCtrl;
		Nvc_Lens_Status_S			stLensStatus;
		Nvc_State_Led_Control_S 	stLedCtrl;
		Nvc_Ptz_Info_S				stPtzInfo;
		Nvc_Ptz_Control_S			stPtzCtrl;
		Nvc_Ptz_Cruise_S			stCruise;
		Nvc_Ptz_Cruise_Idx_S		stCruiseIdx;
		Nvc_Night_Light_Control_S	stLightCtrl;
		Nvc_Night_Light_Status_S	stLightStatus;
		Nvc_Audio_Plug_Control_S	stAplugCtrl;
		Nvc_Audio_Plug_Status_S 	stAplugStatus;
		Nvc_Temperature_Timer_S 	stTCapTimer;
		Nvc_Temperature_Value_S 	stTemperature;
		Nvc_Humidity_Timer_S		stHCapTimer;
		Nvc_Humidity_Value_S		stHumidity;
		Nvc_DoorBell_Control_S		stDoorBellCtrl;
		Nvc_DoorLock_Control_S		stDoorLockCtrl;
		Nvc_FM1288_Control_S		stFM1288;
    };
}Nvc_Drv_Message;



typedef enum	// direction definition
{
	NV_PTZ_DIRECTION_UP,	// vertical upwards
	NV_PTZ_DIRECTION_DOWN,	// vertical downward
	NV_PTZ_DIRECTION_LEFT,	// horizontal to the left
	NV_PTZ_DIRECTION_RIGHT	// horizontal to the right
} NV_PTZ_DIRECTION_E;

typedef enum	// nightlight status definition
{
	NV_NIGHTLIGHT_STATUS_OFF,	// close nightlight
	NV_NIGHTLIGHT_STATUS_ON,	// open nightlight
	NV_NIGHTLIGHT_TOGGLE		// toggle nightlight
} NV_NIGHTLIGHT_STATUS_E;

typedef enum	// type definition triggered by a key operation
{
	NV_BUTTON_LOOSEN_UP,
    NV_BUTTON_PRESS_DOWN,
} NV_BUTTON_STATUS_E;

typedef enum	// led status definition
{
	NV_LED_GREEN,		// green led
	NV_LED_RED			// red led
} NV_LED_COLOR_E;

typedef enum	// nightlight status definition
{
	NV_AUDIOPLUS_STATUS_OFF,	// close audioplus
	NV_AUDIOPLUS_STATUS_ON		// open audioplus
} NV_AUDIOPLUS_STATUS_E;

typedef enum	//
{
	NV_IRC_DETECT_AUTO,
	NV_IRC_DETECT_MANUAL
} NV_IRC_DETECT_TYPE_E;

typedef enum	// type definition triggered by a irc operation
{
	NV_IRC_STATUS_DAY,	// irc status day
	NV_IRC_STATUS_NIGHT	// irc status night
} NV_IRC_STATUS_E;

typedef enum	// type definition triggered by a sd hotplug operation
{
	NV_SD_HOTPLUG_STATUS_REMOVE,	// sd hotplug status remove
	NV_SD_HOTPLUG_STATUS_ADD	// sd hotplug status add
} NV_SD_HOTPLUG_STATUS_E;

typedef enum	// type definition fm1288 operation
{
	NV_FM1288_CTRL_WRITE,	// fm1288 write
	NV_FM1288_CTRL_READ		// fm1288 read
} NV_FM1288_CTRL_TYPE_E;

/********************************************************
 * structure of ptz plugin 
 ********************************************************/
typedef struct __nv_plugin_ptz_t {
	NV_PTZ_DIRECTION_E enDirectionVertical;		// direction of vertical
	NV_U32 u32DegreeVertical;					// degree of vertical, the unit is 0.01 degree
	NV_PTZ_DIRECTION_E enDirectionHorizontal;	// direction of horizontal
	NV_U32 u32DegreeHorizontal;					// degree of horizontal, the unit is 0.01 degree
} nv_plugin_ptz_t;

/********************************************************
 * structure of nightlight plugin 
 ********************************************************/
typedef struct __nv_plugin_nightlight_t {
	NV_NIGHTLIGHT_STATUS_E enNightLightStatus;	// nightlight status
	NV_U8                 u8Luminance; // 1~100
	NV_U8                 u8Res[3];
} nv_plugin_nightlight_t;

/********************************************************
 * structure of temperature and humidity sensor plugin 
 ********************************************************/
typedef struct __nv_plugin_temperature_t {
	NV_FLOAT fTemperature;			// temperature values
} nv_plugin_temperature_t;

/********************************************************
 * structure of humidity sensor plugin 
 ********************************************************/
typedef struct __nv_plugin_humidity_t {
	NV_FLOAT fHumidity;				// humidity values
} nv_plugin_humidity_t;

/********************************************************
 * structure of button plugin 
 ********************************************************/
typedef struct __nv_plugin_button_t {
	NV_BUTTON_STATUS_E enButtonStatus;					// trigger type of button
} nv_plugin_button_t;

/********************************************************
 * structure of led plugin 
 ********************************************************/
typedef struct __nv_plugin_led_t {
	NV_LED_COLOR_E enLedColor;
	NV_U32 u32On;					// the time that turn on the led, the unit is 10 millisecond.
	NV_U32 u32Off;					// the time that turn off the led, the unit is 10 millisecond.
} nv_plugin_led_t;

/********************************************************
 * structure of rtc plugin 
 ********************************************************/
typedef struct __nv_plugin_rtc_t {
	NV_U32 u32Year;
	NV_U32 u32Month;
	NV_U32 u32Date;
	NV_U32 u32Hour;
	NV_U32 u32Minute;
	NV_U32 u32Second;
} nv_plugin_rtc_t;

/********************************************************
 * structure of irc plugin 
 ********************************************************/
typedef struct __nv_plugin_irc_t {
	NV_IRC_DETECT_TYPE_E enIrcCheckType;
	NV_IRC_STATUS_E enIrcStatus; // trigger status of day or light,  support get, only support set when enIrcDecType == NV_IRC_DETECT_MANUAL
} nv_plugin_irc_t;

/********************************************************
 * structure of audioplus plugin 
 ********************************************************/
typedef struct __nv_plugin_audioplus_t {
	NV_AUDIOPLUS_STATUS_E enAudioPlusStatus;	// audioplus status
} nv_plugin_audioplus_t;

/********************************************************
 * structure of fm1288 plugin 
 ********************************************************/
typedef struct __nv_plugin_fm1288_t {
	NV_FM1288_CTRL_TYPE_E enFM1288CtrlType;
	uint8 *u8Data; // the data which want to write
	uint32 u32DataLength; // data length
	uint8 u8RegHigh; // register address high byte
	uint8 u8RegLow; // register address low byte
} nv_plugin_fm1288_t;

extern float g_fDriver_version;

int nv_drv_ctrl_fm1288(int nFd, nv_plugin_fm1288_t *pFM1288, int nWaitResp);


int nv_drv_recv_message(int nFd, char *pMsgBuf, int nBufLen, int nTimeOut);

int nv_drv_send_message(int nFd, char *pMsgBuf, int nMsgLen);

int nv_drv_package_msg_hdr(char *pHdrBuff, int nMsgType, int nMsgLen, int nDevno);

int nv_drv_pack_msg_hdr(char *pHdrBuff, int nMsgType, int nMsgLen, int nDevno);

int nv_drv_parse_msg_hdr(char *pMsgBuff, int nMsgLen, int *pErrCode);

int nv_drv_set_attached_msg(int nFd, NV_U32 u32Attached);

int nv_drv_init_device_info(int nFd, NvcDeviceType_E *pDevType, Nvc_Driver_Cap_Info_S *pCapInfo);

int nv_drv_init_ptz_info(int nFd, Nvc_Ptz_Param *pPtzParam);

int nv_drv_trans_ptz_param(Nvc_Ptz_Param *pPtzParam, Nvc_Ptz_Info_S  *pPtzInfo);

int nv_drv_get_night_light_status(int nFd, nv_plugin_nightlight_t *pNl);

int nv_drv_get_cur_temperature(int nFd, NV_FLOAT *pTemperature);

int nv_drv_get_cur_humidity(int nFd, NV_FLOAT *pHumidity);

int nv_drv_get_button_status(int nFd, int nButtonNo, NV_BUTTON_STATUS_E *pButtonStatus);

int nv_drv_get_audio_plug_status(int nFd, NV_AUDIOPLUS_STATUS_E *pAudioPlug);

int nv_drv_get_ldr_status(int nFd, NV_IRC_STATUS_E *pLdrStatus);

int nv_drv_set_filter_switch(int nFd, NV_S32 s32DayOrNight);

int nv_drv_set_IfrLight_switch(int nFd, NV_S32 s32DayOrNight);

int nv_drv_get_IfrLight_switch(int nFd, NV_S32 *pU32Satus);

int nv_drv_set_doorlock_switch(int nFd, NV_S32 s32OpenOrClose);

int nv_drv_get_doorlock_switch(int nFd, NV_S32 *pU32Satus);

int nv_drv_set_doorbell_tap(int nFd, NV_S32 s32DoorBellTap);

int nv_drv_set_lens_switch(int nFd, NV_S32 s32DayOrNight);

int nv_drv_set_temperature_timer(int nFd, NV_S32 s32CapSecInv, int nWaitResp);

int nv_drv_set_humidity_timer(int nFd, NV_S32 s32CapSecInv, int nWaitResp);

int nv_drv_package_ptz_ctrl_msg(char *pBuffer, nv_plugin_ptz_t *pPtzCtrl, Nvc_Ptz_Param *pPtzParam);

int nv_drv_ctrl_ptz(int nFd, nv_plugin_ptz_t *pPtzCtrl, Nvc_Ptz_Param *pPtzParam, int nWaitResp);

int nv_drv_package_night_light_ctrl_msg(char *pBuffer, nv_plugin_nightlight_t *pNlCtrl);

int nv_drv_ctrl_night_light(int nFd, nv_plugin_nightlight_t *pNlCtrl, int nWaitResp);

int nv_drv_get_night_light_switch(int nFd, NV_S32 *pS32Satus);

int nv_drv_package_state_led_ctrl_msg(char *pBuffer, int nDevNo, nv_plugin_led_t *pLedCtrl);

int nv_drv_ctrl_state_led(int nFd, int nDevno, nv_plugin_led_t *pLedCtrl, int nWaitResp);

int nv_drv_package_audio_plug_ctrl_msg(char *pBuffer, nv_plugin_audioplus_t *pAPlusCtrl);

int nv_drv_ctrl_audio_plug(int nFd, nv_plugin_audioplus_t *pAPlusCtrl, int nWaitResp);

void nv_drv_recv_and_process_msg (NV_HANDLE handle, NV_S32 fd, void * pstUserData, NV_STATUS status);

#pragma pack(pop)

#endif //__NVC_CAMERA_H__

