# 驱动和应用程序接口文档</br>
## 一. 概述


&emsp;&emsp;驱动程序向应用程序屏蔽不同机型的外设区别和复杂底层处理，通过统一的接口，支持应用程序通过消息指令方式查询及控制所有外部设备。目前，可把驱动和应用程序所有交互消息分成四种类型:   
&emsp;1. 设备发送查询命令，驱动返回查询结果。   
&emsp;2. 设备发送控制命令，驱动返回控制结果。   
&emsp;3. 设备发送控制命令，不需要驱动返回控制结果。(暂无实际消息类型，未来如果有频繁控制请求再添加)   
&emsp;4. 外部设备状态发生变化或触发事件时，驱动主动反馈消息给应用程序，不需要应用程序再发响应消息给驱动程序。   
&emsp;&emsp;应用程序通过open驱动映射的设备节点，通过write和read标准I/O函数和驱动进行通讯。驱动支持应用程序订阅事件或外设状态变化消息，默认不订阅，当应用程序通过订阅事件指令请求接收事件或外设状态变化消息时，驱动应该把所有事件或外设状态变化消息反馈给应用程序。   
&emsp;&emsp;驱动应支持和多个应用程序之间的通讯。驱动和应用程序之间的通讯采用异步方式，但应该保持相同消息类型之间的处理同步。举例来说，应用程序先后发送了云台控制和查询ircut状态指令，可以先返回ircut状态响应，后返回云台控制指令响应；应用程序先后发送了两个云台控制指令1和指令2，则应该先返回指令1的控制指令响应，再返回指令2的控制指令响应。事件或外设状态变化消息使用异步方式上报，无需判断当前是否在处理查询或控制请求。驱动应及时返回响应消息给应用程序，一般的查询控制命令理论上应在3s内返回，云台控制命令应在15s内返回。   
&emsp;&emsp;部分应用程序不需要参与的外设，加载驱动时，驱动应完成响应的设备初始化操作，只是后续无相应控制操作。

## 二. 通讯装载方式
&emsp;&emsp;通过以下命令格式装载/卸载驱动:   
&emsp;&emsp;装载设备驱动:&nbsp;insmod /mnt/mtd/modules/Nv_Driver/Nv_Driver.ko chip_type=3518C dev_type=D01   
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;insmod /mnt/mtd/modules/Nv_Driver/Nv_Driver.ko chip_type=3518C dev_type=D11 night_light=ON   
&emsp;&emsp;卸载设备驱动:&nbsp;rmmod Nv_Driver   
&emsp;&emsp;其中，chip_type为设备使用的芯片方案名称，dev_type为设备产品类型，night_light为小夜灯初始状态，其中chip_type和dev_type为必填项，night_light为可选项，取值为ON或者OFF，如果设备支持小夜灯但未填写，默认做关闭处理。chip_type和dev_type取值具体定义如下：   



#### **NvcChipType_E:&nbsp;芯片方案名称**

 
    
        typedef enum __NvcChipType
        {
            NVC_CHIP_3518C = 0x3518c,
	        NVC_CHIP_3518E = 0x3518e,
	        NVC_CHIP_BUTT
        }NvcChipType_E;

#### **NvcDeviceType_E:&nbsp;设备产品类型**

        typedef enum __NvcDeviceType
        {
            NVC_DEVICE_D01 = 0xd01, // 矩形netview 机型
	        NVC_DEVICE_D02 = 0xd02, // 暂未定义
	        NVC_DEVICE_D03 = 0xd03, // DOREL MO136 机型
	        NVC_DEVICE_D04 = 0xd04, // 圆形netview 机型
        	NVC_DEVICE_D11 = 0xd11, // Awox 球泡机型
	        NVC_DEVICEBUTT
        }NvcDeviceType_E;


## 三. 消息指令格式
&emsp;&emsp;驱动和应用程序之间的消息采用指令头+消息体的方式，其中，消息体为各消息类型的具体结构体定义。如果指令头中的消息体长度为 0，则消息体不存在。

#### **指令头格式定义:**
|2&nbsp;Bytes |1&nbsp;Bytes |1&nbsp;Bytes |2&nbsp;Bytes |1&nbsp;Byte |1&nbsp;Byte |4&nbsp;Byte |
|	----	|	---- |  ----  |	----  |	----  |	----  |	----  |
|指令头魔术字(N)|消息子类型(N)|消息主类型(N)|消息体长度(N)|设备号(N)|错误码(N)|预留|
约定：   
指令头魔术字 —— 默认为51843(0xCA83)，如不一致，驱动可不返回响应消息   
消息子类型 —— 主要定义属于能力范围类的消息类型;请求消息为奇数，响应消息为相应的请求消息+1，主动上报事件或状态消息的消息类型也为奇数，不需要响应消息
消息主类型 —— 主要标识设备的单个能力
消息体长度 —— 消息携带具体消息结构体长度，如果为 0，则消息体不存在   
设备号 —— 从0开始，比如设备有2个button，则0为第一个，1为第二个   
错误码 —— 请求消息填0，响应消息填0代表操作成功，否则操作失败，详见错误码定义 
  
		typedef struct __NvcDriverMsgHdr
		{
			uint16 u16Magic;
			uint16 u16MsgType;
			uint16 u16MsgLen;
			uint8  u8DevNo;
			uint8  u8ErrCode;
			uint8  u8Res[4];
		}Nvc_Driver_Msg_Hdr_S;

## 四. 消息类型定义
### 1. 消息主类型
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;消息的主类型主要定义一驱动中包含的功能模块的代号 ID，（比如：获取设备信息的代号为0）。目前的协议中仅支持 0-255 种可能的功能，已经定义—13（0-12）种。之所以把消息类型中标识功能模块的 ID 放在功能模块对应详细操作 ID 的后面是基于 linux 中存储信息的方式为小端存储，换句话说，如果将八位的消息主类型和八位的消息子类型转换成16位的消息类型，表示产品功能的 ID 将在高八位，而表示产品功能的详细操作将在低八位中。

| （功能）能力类型 | 值 | 备注（获取/设置/上报方面的能力） | 
| --- | --- | --- |
| NVC_MsgType_DEVICE 		| 0x00 | 设备信息 |
| NVC_MsgType_BUTTON	 	| 0x01 | 按键 |
| NVC_MsgType_LDR    		| 0x02 | 光敏电阻 |
| NVC_MsgType_IRC   		| 0x03 | IRC |
| NVC_MsgType_IFRRED_LIGHT	| 0x04 | 红外灯 |
| NVC_MsgType_STATE_LIGHT 	| 0x05 | 状态灯 |
| NVC_MsgType_PTZ 			| 0x06 | 云台 |
| NVC_MsgType_NIGHT_LIGHT 	| 0x07 | 小夜灯 |
| NVC_MsgType_AUDIO_PLUG 	| 0x08 | 音频开关 |
| NVC_MsgType_TEMP_MONITOR 	| 0x09 | 温度检测 |
| NVC_MsgType_HUMI_MONITOR 	| 0x0A | 湿度检测 | 
| NVC_MsgType_DOUB_LENS 	| 0x0B | 双镜头 |
| NVC_MsgType_RESET_IO      | 0x0C | 复位   |
	
	typedef enum _NvcMsgType{      
		NVC_MsgType_DEVICE        = 0x00，
		NVC_MsgType_BUTTON        = 0x01，
		NVC_MsgType_LDR           = 0x02，
		NVC_MsgType_IRC           = 0x03，
		NVC_MsgType_IFRRED_LIGHT  = 0x04，
		NVC_MsgType_STATE_LIGHT   = 0x05，
		NVC_MsgType_PTZ           = 0x06，
		NVC_MsgType_NIGHT_LIGHT   = 0x07，
		NVC_MsgType_AUDIO_PLUG    = 0x08，
		NVC_MsgType_TEMP_MONITOR  = 0x09，	
		NVC_MsgType_HUMI_MONITOR  = 0x0A，
		NVC_MsgType_DOUB_LENS     = 0x0B，
		NVC_MsgType_RESET_IO      = 0x0C，
	}NvcMsgType_E
 - - - 




###消息的子类型

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;总的消息类型可分为三种：应用端的请求，驱动的响应消息，驱动主动上报的消息。其中来自应用端的消息最低位为1（奇数）；而驱动回应应用端的消息最低位为请求消息+1；主动上报的消息最低位也为1，但上报的消息不需要得到应用端的应答（驱动上报到应用端的消息没有应答信息）。

| 数据位（bit） | 备注 |
| --- | --- |
| 7 ～ 1 | 命令类型 | 
| 0 | 0: 响应信息 <br> 1: 请求信息/上报消息 | 

<br>
**DEVIC**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;设备的第一个功能模块，支持应用层查询当前驱动的状态，编译版本，加载的产品类型，支持的功能，以及订阅自动上报消息。详细的协议见下表：

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| DEVICE_GET_INFO_REQ          | 1 |获取驱动信息                     |无|
| DEVICE_GET_INFO_RESP         | 2 |获取驱动信息响应信息              |[Nvc_Driver_Ver_Info_S](#nvc_driver_ver_info_s)|
| DEVICE_GET_CAPACITY_REQ      | 3 |获取设备外设能力集                |无|
| DEVICE_GET_CAPACITY_RESP     | 4 |获取设备外设能力集响应信息         |[Nvc_Driver_Cap_Info_s](#nvc_driver_cap_info_s)|
| DEVICE_SUB_REPORT_MSG_REQ    | 5 |设置是否接收驱动事件及状态变化请求  |[Nvc_Attached_Driver_Msg_s](#nvc_attached_driver_msg_s)|
| DEVICE_SUB_REPORT_MSG_RESP   | 6 |设置是否接收驱动事件及状态变化信息  |无|

    typedef enum _NVC_DeviceCmd{
        DEVICE_GET_INFO_REQ             = 1,
        DEVICE_GET_INFO_RESP            = 2,
        DEVICE_GET_CAPACITY_REQ         = 3,
        DEVICE_GET_CAPACITY_RESP        = 4,
        DEVICE_SUB_REPORT_MSG_REQ       = 5,
        DEVICE_SUB_REPORT_MSG_RESP      = 6,
    }NVC_DeviceCmd_E;


<br>
**BUTTON**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于按键功能的规定。


| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| BUTTON_GET_STATUS_REQ		| 1 |获取button当前状态				|无|
| BUTTON_GET_STATUS_RESP		| 2 |获取button当前状态响应信息		|[Nvc_Button_Status_S](#nvc_button_status_s)|
| BUTTON_REPORT_STATUS_MSG		| 3 |上报button当前状态				|[Nvc_Button_Status_S](#nvc_button_status_s)|

    typedef enum _Nvc_ButStateCmd{
        BUTTON_GET_STATUS_REQ       = 1，
        BUTTON_GET_STATUS_RESP      = 2，
        BUTTON_REPORT_STATUS_MSG    = 3，
    }Nvc_ButStateCmd_E;


<br>
**LDR**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于光敏电阻功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| LDR_GET_STATUS_REQ			| 1 |获取光敏电阻当前状态				|无|
| LDR_GET_STATUS_RESP			| 2 |获取光敏电阻当前状态响应信息		|[Nvc_Ldr_Status_S](#nvc_ldr_status_s)|
| LDR_REPORT_STATUS_MSG			| 3 |上报光敏电阻当前状态				|[Nvc_Ldr_Status_S](#nvc_ldr_status_s)|
| LDR_SET_SENSITIVITY_REQ		| 5 |设置光敏电阻检测灵敏度			|[Nvc_Ldr_Senitivity_S](#nvc_ldr_senitivity_s)|
| LDR_SET_SENSITIVITY_RESP		| 6 |设置光敏电阻检测灵敏度响应信息	    |无|
| LDR_GET_SENSITIVITY_REQ		| 7 |获取光敏电阻检测灵敏度			|无|
| LDR_GET_SENSITIVITY_RESP		| 8 |获取光敏电阻检测灵敏度响应信息	    |[Nvc_Ldr_Senitivity_S](#nvc_ldr_senitivity_s)|

    typedef enum _Nvc_LdrCmd{
        LDR_GET_STATUS_REQ          = 1,
        LDR_GET_STATUS_RESP         = 2,
        LDR_REPORT_STATUS_MSG       = 3,
        LDR_SET_SENSITIVITY_REQ     = 5,
        LDR_SET_SENSITIVITY_RESP    = 6,
        LDR_GET_SENSITIVITY_REQ     = 7,
        LDR_GET_SENSITIVITY_RESP    = 8,
    }Nvc_LdrCmd_E;


<br>
**IRC**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于红外滤光片功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| IRC_GET_TYPE_REQ			| 1 |获取设备ircut类型			|无|
| IRC_GET_TYPE_RESP			| 2 |获取设备ircut类型响应信息		|[Nvc_Ircut_Info_S](#nvc_ircut_info_s)|
| IRC_SET_SWITCH_REQ		| 3 |设置ircut切换状态			|[Nvc_Ircut_Control_S](#nvc_ircut_info_s)|
| IRC_SET_SWITCH_RESP		| 4 |设置ircut切换状态响应信息		|无|
| IRC_GET_STATUS_REQ		| 5 |查询ircut当前状态			|无|
| IRC_GET_STATUS_RESP		| 6 |查询ircut当前状态响应信息		|[Nvc_Ircut_Status_S](#nvc_ircut_status_s)|

    typedef enum _NvcIRCCmd{
        IRC_GET_TYPE_REQ    = 1,
        IRC_GET_TYPE_RESP   = 2,
        IRC_SET_SWITCH_REQ  = 3,
        IRC_SET_SWITCH_RESP = 4,
        IRC_GET_STATUS_REQ  = 5,
        IRC_GET_STATUS_RESP = 6,
    }NvcIRCCmd_E;


<br>
**INFRARED LIGHT**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于红外灯功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| IfrLIGHT_SET_SWITCH_REQ           | 1 |设置红外灯打开/关闭				|[Nvc_Lamp_Control_S](#nvc_lamp_control_s)|
| IfrLIGHT_SET_SWITCH_RESP		    | 2 |设置红外灯打开/关闭响应信息		|无|
| IfrLIGHT_GET_STATUS_REQ			| 3 |查询红外灯打开/关闭状态			|无|
| IfrLIGHT_GET_STATUS_RESP			| 4 |查询红外灯打开/关闭状态响应信息	|[Nvc_Lamp_Status_S](#nvc_lamp_status_s)|
 
    typedef enum _NvcIfrLight{
        IfrLIGHT_SET_SWITCH_REQ     = 1,
        IfrLIGHT_SET_SWITCH_RESP    = 2,
        IfrLIGHT_GET_STATUS_REQ     = 3,
        IfrLIGHT_GET_STATUS_RESP    = 4,
    }NvcIfrLight_E;


<br>
**STATE LIGHT**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于状态灯功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| StaLIGHT_SET_STATUS_REQ			| 1 |设置LED灯显示方式				|[Nvc_State_Led_Control_S](#nvc_state_led_control_s)|
| StaLIGHT_SET_STATUS_RESP			| 2 |设置LED灯显示方式响应信息	        |无|

    typedef enum _NvcStaLight{
        StaLIGHT_SET_STATUS_REQ     = 1,
        StaLIGHT_SET_STATUS_RESP    = 2,
    }NvcStaLight_E;


<br>
**PTZ**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于云台功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| PTZ_GET_INFO_REQ				| 1 |云台信息查询				     |无|
| PTZ_GET_INFO_RESP             | 2 |云台信息查询响应信息		     |[Nvc_Ptz_Info_S](#nvc_ptz_info_s)|
| PTZ_SET_COMMON_REQ			| 3 |通用云台控制指令			     |[Nvc_Ptz_Control_S](#nvc_ptz_control_s)|
| PTZ_SET_COMMON_RESP		    | 4 |通用云台控制指令响应信息	     |无|
| PTZ_SET_PRESET_CRUISE_REQ		| 5 |设置云台预置位巡航		     |[Nvc_Ptz_Cruise_S](#nvc_ptz_cruise_s)|
| PTZ_SET_PRESET_CRUISE_RESP	| 6 |设置云台预置位巡航响应信息	     |无|
| PTZ_CLEAR_PRESET_CRUISE_REQ	| 7 |清除云台预置位巡航		     |[Nvc_Ptz_Cruise_Idx_S](#nvc_ptz_cruise_idx_s)|
| PTZ_CLEAR_PRESET_CRUISE_RESP	| 8 |清除云台预置位巡航响应信息	     |无|
| PTZ_REPORT_INFO_MSG			| 9 |上报云台信息				     |[Nvc_Ptz_Info_S](#nvc_ptz_info_s)|
    
    typedef enum _NvcPTZCmd{
        PTZ_GET_INFO_REQ             = 1,
        PTZ_GET_INFO_RESP            = 2,
        PTZ_SET_COMMON_REQ           = 3,
        PTZ_SET_COMMON_RESP          = 4,
        PTZ_SET_PRESET_CRUISE_REQ    = 5,
        PTZ_SET_PRESET_CRUISE_REQ    = 6,
        PTZ_CLEAR_PRESET_CRUISE_REQ  = 7,
        PTZ_CLEAR_PRESET_CRUISE_RESP = 8,
        PTZ_REPORT_INFO_MSG          = 9,
    }NvcPTZCmd_E

 
<br>
**NIGHT LIGHT**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于照明灯功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| NiLIGHT_SET_STATUS_REQ	| 1 |设置小夜灯打开/关闭				|[Nvc_Night_Light_Control_s](#nvc_night_light_control_s)|
| NiLIGHT_SET_STATUS_RESP| 2 |设置小夜灯打开/关闭响应信息		|无|
| NiLIGHT_GET_STATUS_REQ	| 3 |查询小夜灯打开/关闭状态			|无|
| NiLIGHT_GET_STATUS_RESP	| 4 |查询小夜灯打开/关闭状态响应信息	|[Nvc_Night_Light_status_s](#nvc_night_light_status_s)|

    typedef enum _NvcNiLightCmd{
        NiLIGHT_SET_STATUS_REQ      = 1,
        NiLIGHT_SET_STATUS_RESP     = 2,
        NiLIGHT_GET_STATUS_REQ      = 3,
        NiLIGHT_GET_STATUS_RESP     = 4,
    }NvcNiLightCmd;


<br>
**AUDIO PLUG**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于音频功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| AuPLUG_SET_STATUS_REQ     | 1 |设置音频扬声器打开/关闭			|[Nvc_Audio_Plug_Control_s](#nvc_audio_plug_control_s)|
| AuPLUG_SET_STATUS_RESP	| 2 |设置音频扬声器打开/关闭响应信息	|无|
| AuPLUG_GET_STATUS_REQ	    | 3 |查询音频扬声器打开/关闭状态		|无|
| AuPLUG_GET_STATUS_RESP	| 4 |查询音频扬声器打开/关闭状态响应信息|[Nvc_Audio_Plug_status_s](#nvc_audio_plug_status_s)|

    typedef enum _NvcAudioPlugCmd{
        AuPLUG_SET_STATUS_REQ       = 1,
        AuPLUG_SET_STATUS_RESP      = 2,
        AuPLUG_GET_STATUS_REQ       = 3,
        AuPLUG_GET_STATUS_RESP      = 4,
    }NvcAudioPlugCmd;


<br>
**TEMP MONITOR**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于温度检测功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| TmpMONITOR_SET_REPORT_REQ         | 1 |设置温度定时采集上报请求			|[Nvc_Temperature_Timer_S](#nvc_temperature_timer_s)|
| TmpMONITOR_SET_REPORT_RESP        | 2 |设置温度定时采集上报响应信息		|无|
| TmpMONITOR_GET_VALUE_REQ          | 3 |查询当前温度值					|无|
| TmpMONITOR_GET_VALUE_RESP         | 4 |查询当前温度值响应信息			|[Nvc_Temperature_Value_S](#nvc_temperature_value_s)|
| TmpMONITOR_REPORT_VALUE_MSG       | 5 |上报当前温度值消息				|[Nvc_Temperature_Value_S](#nvc_temperature_value_s)|

    typedef enum _NvcTempMonitorCmd{
        TmpMONITOR_SET_REPORT_REQ       = 1,
        TmpMONITOR_SET_REPORT_RESP      = 2,
        TmpMONITOR_GET_VALUE_REQ        = 3,
        TmpMONITOR_GET_VALUE_RESP       = 4,
        TmpMONITOR_REPORT_VALUE_MSG     = 5,
    }NvcTempMonitorCmd;


<br>
**HUMI MONITOR**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于湿度功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| HumMONITOR_SET_REPORT_REQ         | 1 |设置湿度度定时采集上报请求		|[Nvc_Humidity_Timer_S](#nvc_humidity_timer_s)|
| HumMONITOR_SET_REPORT_RESP        | 2 |设置湿度定时采集上报响应信息		|无|
| HumMONITOR_GET_VALUE_REQ          | 3 |查询当前湿度值					|无|
| HumMONITOR_GET_VALUE_RESP         | 4 |查询当前湿度值响应信息			|[Nvc_Humidity_Value_S](#nvc_humidity_value_s)|
| HumMONITOR_REPORT_VALUE_MSG       | 5 |上报当前湿度值消息				|[Nvc_Humidity_Value_S](#nvc_humidity_value_s)|

    typedef enum _NvcHimiMonitorCmd{
       HumMONITOR_SET_REPORT_REQ        = 1,
        HumMONITOR_SET_REPORT_RESP      = 2,
        HumMONITOR_GET_VALUE_REQ        = 3,
        HumMONITOR_GET_VALUE_RESP       = 4,
        HumMONITOR_REPORT_VALUE_MSG     = 5,
    }NvcHimiMonitorCmd;

<br>
**TYPE DOUB LENS CMD**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于双镜头功能的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| DobLENS_SET_STATUS_REQ        | 1 |设置双镜头切换(日用镜头/夜用镜头)|[Nvc_Lens_Control_S](#nvc_lens_control_s)|
| DobLENS_SET_STATUS_RESP       | 2 |设置双镜头切换响应信息			|无|
| DobLENS_GET_STATUS_REQ        | 3 |查询双镜头使用状态				|无|
| DobLENS_GET_STATUS_RESP       | 4 |查询双镜头使用状态响应信息		|[Nvc_Lens_Status_S](#nvc_Lens_status_s)|

    typedef enum _NvcDoubLensCmd{
        DobLENS_SET_STATUS_REQ      = 1,
        DobLENS_SET_STATUS_RESP     = 2,
        DobLENS_GET_STATUS_REQ      = 3,
        DobLENS_GET_STATUS_RESP     = 4,
    }NvcDoubLensCmd_E;

<br>
**TYPE RESETIO CMD**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于复位IO的规定。

| 命令类型 | 值 | 说明 | 消息类型 |
| --- | --- | --- | --- |
| ResetIO_SET_STATUS_REQ        | 1 |设置双镜头切换(日用镜头/夜用镜头)|[Nvc_Lens_Control_S](#nvc_lens_control_s)|
| ResetIO_SET_STATUS_RESP       | 2 |设置双镜头切换响应信息			|无|

    typedef enum _NvcDoubLensCmd{
        ResetIO_SET_STATUS_REQ      = 1,
        ResetIO_SET_STATUS_RESP     = 2,
    }NvcDoubLensCmd_E;

- - - 


 
**老的协议版本**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;为了兼容前一版本的协议，枚举类型 __NvcDriverMsgType 中的初始值要相应的修改成新版本对应的值

|消息类型 |枚举定义 |消息定义描述 |消息体结构体 |
|	----:	|	----  |	----  |	----  |
| 0X0001 |NVC_QUERY_DRIVER_INFO_REQ|获取驱动信息|无|
| 0X0002 |NVC_QUERY_DRIVER_INFO_RESP|获取驱动信息响应信息|[Nvc_Driver_Ver_Info_S](#nvc_driver_ver_info_s)|
| 0X0003 |NVC_QUERY_DRIVER_CAPACITY_REQ|获取设备外设能力集|无|
| 0X0004 |NVC_QUERY_DRIVER_CAPACITY_RESP|获取设备外设能力集响应信息|[Nvc_Driver_Cap_Info_s](#nvc_driver_cap_info_s)|
| 0X0005 |NVC_SET_ATTACHED_DRIVER_MSG_REQ|设置是否接收驱动事件及状态变化请求|[Nvc_Attached_Driver_Msg_s](#nvc_attached_driver_msg_s)|
| 0X0006 |NVC_SET_ATTACHED_DRIVER_MSG_RESP|设置是否接收驱动事件及状态变化信息|无|
| 0X0101 |NVC_QUERY_BUTTON_STATUS_REQ|获取button当前状态|无|
| 0X0102 |NVC_QUERY_BUTTON_STATUS_RESP|获取button当前状态响应信息|[Nvc_Button_Status_S](#nvc_button_status_s)|
| 0X0103 |NVC_REPORT_BUTTON_STATUS_MSG|上报button当前状态|[Nvc_Button_Status_S](#nvc_button_status_s)|
| 0X0201 |NVC_QUERY_LDR_STATUS_REQ|获取光敏电阻当前状态|无|
| 0X0202 |NVC_QUERY_LDR_STATUS_RESP|获取光敏电阻当前状态响应信息|[Nvc_Ldr_Status_S](#nvc_ldr_status_s)|
| 0X0203 |NVC_REPORT_LDR_STATUS_MSG|上报光敏电阻当前状态|[Nvc_Ldr_Status_S](#nvc_ldr_status_s)|
| 0X0205 |NVC_SET_LDR_SENSITIVITY_REQ|设置光敏电阻检测灵敏度|[Nvc_Ldr_Senitivity_S](#nvc_ldr_senitivity_s)|
| 0X0206 |NVC_SET_LDR_SENSITIVITY_RESP|设置光敏电阻检测灵敏度响应信息|无|
| 0X0207 |NVC_QUERY_LDR_SENSITIVITY_REQ|获取光敏电阻检测灵敏度|无|
| 0X0208 |NVC_QUERY_LDR_SENSITIVITY_RESP|获取光敏电阻检测灵敏度响应信息|[Nvc_Ldr_Senitivity_S](#nvc_ldr_senitivity_s)|
| 0X0301 |NVC_QUERY_IRC_TYPE_REQ|获取设备ircut类型|无|
| 0X0302 |NVC_QUERY_IRC_TYPE_RESP|获取设备ircut类型响应信息|[Nvc_Ircut_Info_S](#nvc_ircut_info_s)|
| 0X0303 |NVC_CONTROL_IRC_SWITCH_REQ|设置ircut切换状态|[Nvc_Ircut_Control_S](#nvc_ircut_info_s)|
| 0X0304 |NVC_CONTROL_IRC_SWITCH_RESP|设置ircut切换状态响应信息|无|
| 0X0305 |NVC_QUERY_IRC_STATUS_REQ|查询ircut当前状态|无|
| 0X0306 |NVC_QUERY_IRC_STATUS_RESP|查询ircut当前状态响应信息|[Nvc_Ircut_Status_S](#nvc_ircut_status_s)|
| 0X0401 |NVC_CONTROL_LAMP_SWITCH_REQ|设置红外灯打开/关闭|[Nvc_Lamp_Control_S](#nvc_lamp_control_s)|
| 0X0402 |NVC_CONTROL_LAMP_SWITCH_RESP|设置红外灯打开/关闭响应信息|无|
| 0X0403 |NVC_QUERY_LAMP_STATUS_REQ|查询红外灯打开/关闭状态|无|
| 0X0404 |NVC_QUERY_LAMP_STATUS_RESP|查询红外灯打开/关闭状态响应信息|[Nvc_Lamp_Status_S](#nvc_lamp_status_s)|
| 0X0501 |NVC_CONTROL_STATE_LED_REQ|设置LED灯显示方式|[Nvc_State_Led_Control_S](#nvc_state_led_control_s)|
| 0X0502 |NVC_CONTROL_STATE_LED_RESP|设置LED灯显示方式响应信息|无|
| 0X0601 |NVC_QUERY_PTZ_INFO_REQ|云台信息查询|无|
| 0X0602 |NVC_QUERY_PTZ_INFO_RESP|云台信息查询响应信息|[Nvc_Ptz_Info_S](#nvc_ptz_info_s)|
| 0X0603 |NVC_CONTROL_PTZ_COMMON_REQ|通用云台控制指令|[Nvc_Ptz_Control_S](#nvc_ptz_control_s)|
| 0X0604 |NVC_CONTROL_PTZ_COMMON_RESP|通用云台控制指令响应信息|无|
| 0X0605 |NVC_SET_PRESET_CRUISE_REQ|设置云台预置位巡航|[Nvc_Ptz_Cruise_S](#nvc_ptz_cruise_s)|
| 0X0606 |NVC_SET_PRESET_CRUISE_RESP|设置云台预置位巡航响应信息|无|
| 0X0607 |NVC_CLEAR_PRESET_CRUISE_REQ|清除云台预置位巡航|[Nvc_Ptz_Cruise_Idx_S](#nvc_ptz_cruise_idx_s)|
| 0X0608 |NVC_CLEAR_PRESET_CRUISE_RESP|清除云台预置位巡航响应信息|无|
| 0X0609 |NVC_REPORT_PTZ_INFO_MSG|上报云台信息|[Nvc_Ptz_Info_S](#nvc_ptz_info_s)|
| 0X0701 |NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ|设置小夜灯打开/关闭|[Nvc_Night_Light_Control_s](#nvc_night_light_control_s)|
| 0X0702 |NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP|设置小夜灯打开/关闭响应信息|无|
| 0X0703 |NVC_QUERY_NIGHT_LIGHT_STATUS_REQ|查询小夜灯打开/关闭状态|无|
| 0X0704 |NVC_QUERY_NIGHT_LIGHT_STATUS_RESP|查询小夜灯打开/关闭状态响应信息|[Nvc_Night_Light_status_s](#nvc_night_light_status_s)|
| 0X0801 |NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ|设置音频扬声器打开/关闭|[Nvc_Audio_Plug_Control_s](#nvc_audio_plug_control_s)|
| 0X0802 |NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP|设置音频扬声器打开/关闭响应信息|无|
| 0X0803 |NVC_QUERY_AUDIO_PLUG_STATUS_REQ|查询音频扬声器打开/关闭状态|无|
| 0X0804 |NVC_QUERY_AUDIO_PLUG_STATUS_RESP|查询音频扬声器打开/关闭状态响应信息|[Nvc_Audio_Plug_status_s](#nvc_audio_plug_status_s)|
| 0X0901 |NVC_SET_TEMPERATURE_TIMER_REQ|设置温度定时采集上报请求|[Nvc_Temperature_Timer_S](#nvc_temperature_timer_s)|
| 0X0902 |NVC_SET_TEMPERATURE_TIMER_RESP|设置温度定时采集上报响应信息|无|
| 0X0903 |NVC_QUERY_TEMPERATURE_VALUE_REQ|查询当前温度值|无|
| 0X0904 |NVC_QUERY_TEMPERATURE_VALUE_RESP|查询当前温度值响应信息|[Nvc_Temperature_Value_S](#nvc_temperature_value_s)|
| 0X0905 |NVC_REPORT_TEMPERATURE_VALUE_MSG|上报当前温度值消息|[Nvc_Temperature_Value_S](#nvc_temperature_value_s)|
| 0X0A01 |NVC_SET_HUMIDITY_TIMER_REQ|设置湿度度定时采集上报请求|[Nvc_Humidity_Timer_S](#nvc_humidity_timer_s)|
| 0X0A02 |NVC_SET_HUMIDIT_TIMER_RESP|设置湿度定时采集上报响应信息|无|
| 0X0A03 |NVC_QUERY_HUMIDIT_VALUE_REQ|查询当前湿度值|无|
| 0X0A04 |NVC_QUERY_HUMIDIT_VALUE_RESP|查询当前湿度值响应信息|[Nvc_Humidity_Value_S](#nvc_humidity_value_s)|
| 0X0A05 |NVC_REPORT_HUMIDIT_VALUE_MSG|上报当前湿度值消息|[Nvc_Humidity_Value_S](#nvc_humidity_value_s)|
| 0X0B01 |NVC_CONTROL_LENS_SWITCH_REQ|设置双镜头切换(日用镜头/夜用镜头)|[Nvc_Lens_Control_S](#nvc_lens_control_s)|
| 0X0B02 |NVC_CONTROL_LENS_SWITCH_RESP|设置双镜头切换响应信息|无|
| 0X0B03 |NVC_QUERY_LENS_STATUS_REQ|查询双镜头使用状态|无|
| 0X0B04 |NVC_QUERY_LENS_STATUS_RESP|查询双镜头使用状态响应信息|[Nvc_Lens_Status_S](#nvc_Lens_status_s)|
| 0X0C01 |NVC_GPIO_RESET_REQ|通过GPIO复位系统|无|
| 0X0C02 |NVC_GPIO_RESET_RESP|通过GPIO复位系统响应信息|无|


		typedef enum __NvcDriverMsgType
		{
			NVC_QUERY_DRIVER_INFO_REQ            = 0X0001,  // 获取驱动信息 (芯片方案和机型，版本号等)
			NVC_QUERY_DRIVER_INFO_RESP           = 0X0002,  // 获取驱动信息响应信息
			NVC_QUERY_DRIVER_CAPACITY_REQ        = 0X0003,  // 获取设备外设能力集
			NVC_QUERY_DRIVER_CAPACITY_RESP       = 0X0004,  // 获取设备外设能力集响应信息
			NVC_SET_ATTACHED_DRIVER_MSG_REQ      = 0X0005,  // 设置是否接收驱动事件及状态变化请求
			NVC_SET_ATTACHED_DRIVER_MSG_RESP     = 0X0006,  // 设置是否接收驱动事件及状态变化信息	
			NVC_QUERY_BUTTON_STATUS_REQ          = 0X0101,  // 获取button当前状态
			NVC_QUERY_BUTTON_STATUS_RESP         = 0X0102,  // 获取button当前状态响应信息
			NVC_REPORT_BUTTON_STATUS_MSG         = 0X0103,  // 上报button当前状态  
			NVC_QUERY_LDR_STATUS_REQ             = 0X0201,  // 获取光敏电阻当前状态 (0 白天 1 黑夜)
			NVC_QUERY_LDR_STATUS_RESP            = 0X0202,  // 获取光敏电阻当前状态响应信息
			NVC_REPORT_LDR_STATUS_MSG            = 0X0203,  // 上报光敏电阻当前状态
			NVC_SET_LDR_SENSITIVITY_REQ          = 0X0205,  // 设置光敏电阻检测灵敏度
			NVC_SET_LDR_SENSITIVITY_RESP         = 0X0206,  // 设置光敏电阻检测灵敏度响应信息
			NVC_QUERY_LDR_SENSITIVITY_REQ        = 0X0207,  // 获取光敏电阻检测灵敏度
			NVC_QUERY_LDR_SENSITIVITY_RESP       = 0X0208,  // 获取光敏电阻检测灵敏度响应信息
			NVC_QUERY_IRC_TYPE_REQ               = 0X0301,  // 获取设备ircut类型
        	NVC_QUERY_IRC_TYPE_RESP              = 0X0302,  // 获取设备ircut类型响应信息
			NVC_CONTROL_IRC_SWITCH_REQ           = 0X0303,	// 设置ircut切换状态
			NVC_CONTROL_IRC_SWITCH_RESP          = 0X0304,	// 设置ircut切换状态响应信息
			NVC_QUERY_IRC_STATUS_REQ             = 0X0305,	// 查询ircut当前状态
			NVC_QUERY_IRC_STATUS_RESP            = 0X0306,	// 查询ircut当前状态响应信息
			NVC_CONTROL_LAMP_SWITCH_REQ          = 0X0401,	// 设置红外灯打开/关闭
			NVC_CONTROL_LAMP_SWITCH_RESP         = 0X0402,	// 设置红外灯打开/关闭响应信息
			NVC_QUERY_LAMP_STATUS_REQ            = 0X0403,	// 查询红外灯打开/关闭状态
			NVC_QUERY_LAMP_STATUS_RESP           = 0X0404,	// 查询红外灯打开/关闭状态响应信息
			NVC_CONTROL_STATE_LED_REQ            = 0X0501,	// 设置LED灯显示方式
			NVC_CONTROL_STATE_LED_RESP           = 0X0502,	// 设置LED灯显示方式响应信息
			NVC_QUERY_PTZ_INFO_REQ               = 0X0601,	// 云台信息查询
			NVC_QUERY_PTZ_INFO_RESP              = 0X0602,	// 云台信息查询响应信息
			NVC_CONTROL_PTZ_COMMON_REQ           = 0X0603,	// 通用云台控制指令
			NVC_CONTROL_PTZ_COMMON_RESP          = 0X0604,	// 通用云台控制指令响应信息
			NVC_SET_PRESET_CRUISE_REQ            = 0X0605,	// 设置云台预置位巡航
			NVC_SET_PRESET_CRUISE_RESP           = 0X0606,	// 设置云台预置位巡航响应信息
			NVC_CLEAR_PRESET_CRUISE_REQ          = 0X0607,	// 清除云台预置位巡航
			NVC_CLEAR_PRESET_CRUISE_RESP         = 0X0608,	// 清除云台预置位巡航响应信息
			NVC_REPORT_PTZ_INFO_MSG              = 0X0609,  // 上报云台信息
			NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ   = 0X0701,	// 设置小夜灯打开/关闭
			NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP  = 0X0702,	// 设置小夜灯打开/关闭响应信息 
			NVC_QUERY_NIGHT_LIGHT_STATUS_REQ     = 0X0703,	// 查询小夜灯打开/关闭状态
			NVC_QUERY_NIGHT_LIGHT_STATUS_RESP    = 0X0704,	// 查询小夜灯打开/关闭状态响应信息
			NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ    = 0X0801,	// 设置音频扬声器打开/关闭
			NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP   = 0X0802,	// 设置音频扬声器打开/关闭响应信息 
			NVC_QUERY_AUDIO_PLUG_STATUS_REQ      = 0X0803,	// 查询音频扬声器打开/关闭状态
			NVC_QUERY_AUDIO_PLUG_STATUS_RESP     = 0X0804,	// 查询音频扬声器打开/关闭状态响应信息
			NVC_SET_TEMPERATURE_TIMER_REQ        = 0X0901,	// 设置温度定时采集上报请求
			NVC_SET_TEMPERATURE_TIMER_RESP       = 0X0902,	// 设置温度定时采集上报响应信息	
			NVC_QUERY_TEMPERATURE_VALUE_REQ      = 0X0903,	// 查询当期温度值
			NVC_QUERY_TEMPERATURE_VALUE_RESP     = 0X0904,	// 查询当期温度值响应信息
		    NVC_REPORT_TEMPERATURE_VALUE_MSG     = 0X0905,  // 上报当前温度值消息 
			NVC_SET_HUMIDITY_TIMER_REQ           = 0X0A01,	// 设置湿度度定时采集上报请求
			NVC_SET_HUMIDIT_TIMER_RESP           = 0X0A02,	// 设置湿度定时采集上报响应信息
			NVC_QUERY_HUMIDIT_VALUE_REQ          = 0X0A03,	// 查询当期湿度值
			NVC_QUERY_HUMIDIT_VALUE_RESP         = 0X0A04,	// 查询当期湿度值响应信息	
		    NVC_REPORT_HUMIDIT_VALUE_MSG         = 0X0A05, 	// 上报当前湿度值消息 
			NVC_CONTROL_LENS_SWITCH_REQ          = 0X0B01,	// 设置双镜头切换(日用镜头/夜用镜头)
			NVC_CONTROL_LENS_SWITCH_RESP         = 0X0B02,	// 设置双镜头响应信息
			NVC_QUERY_LENS_STATUS_REQ            = 0X0B03,	// 查询双镜头使用状态
			NVC_QUERY_LENS_STATUS_RESP           = 0X0B04,	// 查询双镜头使用状态响应信息
            NVC_GPIO_RESET_REQ                   = 0x0C01,  // 通过GPIO复位系统 
            NVC_GPIO_RESET_RESP                  = 0x0C02,  // 通过GPIO复位系统响应信息
		}NvcDriverMsgType_E;

## 五. 错误码定义   

        typedef enum __NvcDriverErrCode
        {
            NVC_DRIVER_SUCCESS          = 0, 
			NVC_DRIVER_ERR              = 1, // 通用错误
			NVC_DRIVER_ERR_INIT         = 2, // 初始化设备错误
			NVC_DRIVER_ERR_PARAM        = 3, // 输入参数出错
			NVC_DRIVER_ERR_BUSY         = 4, // 驱动正忙
			NVC_DRIVER_ERR_NOT_SUPPORT  = 5, // 不支持的操作(比如设备支持红外灯，但不支持查询红外灯状态)
			NVC_DRIVER_ERR_INVALID      = 6, // 无效设备
			NVC_DRIVER_ERR_UNAVAILABLE  = 7, // 设备丢失，如物理连接断开
			NVC_DRIVER_ERR_UNFINISHED   = 8, // 操作未完成(如云台命令下发控制步数超过当前能运动步数)
			NVC_DRIVER_ERR_LIMITED      = 9, // 如云台已到运动方向终点或限位导致命令无效
        }NvcDriverErrCode_E;

## 六. 驱动指令消息体定义

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.1:&nbsp;查询设备信息**
#### **&emsp;&emsp;6.1.1:&nbsp;查询驱动信息**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_DRIVER_INFO_REQ 消息体：无      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_DRIVER_INFO_RESP 消息体：Nvc_Driver_Ver_Info_S   
&emsp;&emsp;相关数据结构定义： 
#### **nvc_driver_ver_info_s**
    	typedef struct __Nvc_Driver_Ver_Info
        {
        	uint32	u32ChipType; // NvcChipType_E
			uint32	u32DeviceType; // NvcDeviceType_E
			char	szVerInfo[16];
			char	szBuildData[32];
        }Nvc_Driver_Ver_Info_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.1.2:&nbsp;查询设备外设能力集**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_DRIVER_CAPACITY_REQ 消息体：无      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_DRIVER_CAPACITY_RESP 消息体：Nvc_Driver_Cap_Info_S   
&emsp;&emsp;相关数据结构定义： 
#### **nvc_driver_cap_info_s**
		typedef enum __NvcDriverCap
		{
			NVC_SUPP_ButtonMonitor   = 0x00000001,    // 是否支持按键检测
			NVC_SUPP_LdrMonitor      = 0x00000002,    // 是否支持日夜模式检测(light dependent resistors detection) 
			NVC_SUPP_Ircut           = 0x00000004,    // 是否支持滤光片切换
			NVC_SUPP_IfrLamp         = 0x00000008,    // 是否支持红外灯
			NVC_SUPP_DoubleLens      = 0x00000010,    // 是否支持双镜头
			NVC_SUPP_StateLed        = 0x00000020,    // 是否支持状态灯显示
			NVC_SUPP_PTZ             = 0x00000040,    // 是否支持云台功能
			NVC_SUPP_NightLight      = 0x00000080,    // 是否支持小夜灯功能
            NVC_SUPP_CoolFan         = 0x00000100,    // 是否支持散热风扇功能
			NVC_SUPP_AudioPlug       = 0x00000200,    // 是否支持音频开关功能
			NVC_SUPP_TempMonitor     = 0x00000400,    // 是否支持温度采集功能
			NVC_SUPP_HumiMonitor     = 0x00000800,    // 是否支持湿度采集功能
            NVC_SUPP_GpioReset       = 0x00001000,    // 通过GPIO 复位设备(重启设备)
		}Nvc_Driver_Cap_E;
		
		typedef struct __Nvc_Driver_Cap_Info
        {
        	uint32 u32CapMask;
			uint8  u8ButtonCnt;
			uint8  u8LedCnt;
			uint8  u8Res[2];
        }Nvc_Driver_Cap_Info_S;

&emsp;&emsp;说明:&nbsp;假设某种设备支持button, ptz,和audioplug,则 u32CapMask = （NVC_SUPP_ButtonMonitor | NVC_SUPP_PTZ | NVC_SUPP_AudioPlug);

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.2:&nbsp;设置是否接收事件及状态变化信息**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息： NVC_SET_ATTACHED_DRIVER_MSG_REQ 消息体：NVC_ATTACHED_DRIVER_MSG_S：   
&emsp;&emsp;驱动程序 --> 应用程序 响应消息： NVC_SET_ATTACHED_DRIVER_MSG_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_attached_driver_msg_s**
		typedef struct __Nvc_Attached_Driver_Msg
        {
        	uint8	u8Attached; // 0 不需要上报事件/状态信息， 1 需要上报事件/状态信息
			uint8	u8Res[3];
        }Nvc_Attached_Driver_Msg_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.3:&nbsp;button控制**
#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.3.1:&nbsp;查询button当前状态**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_BUTTON_STATUS_REQ 消息体：无      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_BUTTON_STATUS_RESP 消息体：Nvc_Button_Status_S   

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.3.2:&nbsp;button事件上报**
&emsp;&emsp;当button状态发生变化时，被按下或松开，驱动上报button事件消息给应用程序(如果应用程序设置了需要上报事件)   
&emsp;&emsp;驱动程序 --> 应用程序 请求消息：NVC_REPORT_BUTTON_STATUS_MSG 消息体：Nvc_Button_Status_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_button_status_s**
		typedef struct __Nvc_Button_Status_S
        {
        	uint8	u8Status; // 0 未被按下， 1 被按下
			uint8	u8Res[3];
        }Nvc_Button_Status_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4:&nbsp;光敏电阻/ircut/红外灯/双镜头控制相关**
&emsp;&emsp;通常情况下，光敏电阻判断当前是白天时，关闭红外灯，滤光片切换到红外截止状态(或使用日用镜头)，图像转到彩色状态。光敏电阻判断当前是黑夜时，则打开红外灯，滤光片切换到通红外状态(或使用夜用镜头)，图像转成黑白。但在某些场景下或客户需求下，晚上不需要如上联动操作，故驱动只向应用程序提供获取光明电阻状态、切换滤光片、打开关闭红外灯、及切换镜头接口，由主控来处理日夜切换联动操作。

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.1:&nbsp;获取光敏电阻当前状态**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_LDR_STATUS_REQ 消息体：无      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_LDR_STATUS_RESP 消息体：Nvc_Ldr_Status_S   

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.2:&nbsp;上报光敏电阻当前状态**
&emsp;&emsp;当光敏电阻状态发生变化时，驱动上报当前照度信息给应用程序(如果应用程序设置了需要上报事件)   
&emsp;&emsp;驱动程序 --> 应用程序 请求消息：NVC_REPORT_LDR_STATUS_MSG 消息体：Nvc_Button_Status_S  
&emsp;&emsp;相关数据结构定义：   
#### **nvc_ldr_status_s**
		typedef struct __Nvc_Ldr_Status
        {
        	uint8	u8Status; // 0 黑夜， 1 白天
			uint8	u8Res[3];
        }Nvc_Ldr_Status_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.3:&nbsp;设置光敏电阻检测灵敏度**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_SET_LDR_SENSITIVITY_REQ 消息体：Nvc_Ldr_Senitivity_S      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_SET_LDR_SENSITIVITY_RESP 消息体：无  

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.4:&nbsp;获取光敏电阻检测灵敏度**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_LDR_SENSITIVITY_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_LDR_SENSITIVITY_RESP 消息体：Nvc_Ldr_Senitivity_S    
&emsp;&emsp;相关数据结构定义：   
#### **nvc_ldr_senitivity_s**
		typedef struct __Nvc_Ldr_Senitivity
        {
        	uint8	u8Level; // 0 低灵敏度， 1 中灵敏度 2 高灵敏度 (灵敏度越高，越容易切换到黑夜状态)
			uint8	u8Res[3];
        }Nvc_Ldr_Senitivity_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.5:&nbsp;获取设备ircut类型**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_IRC_TYPE_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_IRC_TYPE_RESP 消息体：Nvc_Ircut_Info_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_ircut_info_s**
		typedef struct __Nvc_Ircut_Info
        {
        	uint32 u32IrcType; // 目前为0， 默认类型
        }Nvc_Ircut_Info_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.6:&nbsp;设置ircut切换状态**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_IRC_SWITCH_REQ 消息体：Nvc_Ircut_Control_S           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_IRC_SWITCH_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_ircut_control_s**
		typedef struct __Nvc_Ircut_Control
        {
        	uint8	u8Status; // 0 红外截止状态，1 通红外状态
			uint8	u8Res[3];
        }Nvc_Ircut_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.7:&nbsp;获取ircut切换状态**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_IRC_STATUS_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_IRC_STATUS_RESP 消息体：Nvc_Ircut_Status_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_ircut_status_s**
		typedef struct __Nvc_Ircut_Status
        {
        	uint8	u8Status; // 0 红外截止状态，1 通红外状态
			uint8	u8Res[3];
        }Nvc_Ircut_Status_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.6:&nbsp;设置红外灯打开/关闭**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_LAMP_SWITCH_REQ 消息体：Nvc_Lamp_Control_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_LAMP_SWITCH_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_lamp_control_s**
		typedef struct __Nvc_Lamp_Control
        {
        	uint8	u8Switch; // 0 关闭红外灯，1 打开红外灯
			uint8	u8Res[3];
        }Nvc_Lamp_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.7:&nbsp;获取红外灯打开/关闭**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_LAMP_STATUS_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_LAMP_STATUS_RESP 消息体：Nvc_Lamp_Status_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_lamp_status_s**
		typedef struct __Nvc_Lamp_Status
        {
        	uint8	u8Status; // 0 关闭红外灯，1 打开红外灯
			uint8	u8Res[3];
        }Nvc_Lamp_Status_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.8:&nbsp;设置双镜头切换**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_LENS_SWITCH_REQ 消息体：Nvc_Lens_Control_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_LENS_SWITCH_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_lens_control_s**
		typedef struct __Nvc_Lens_Control
        {
        	uint8	u8SwitchLens; // 0 使用日用镜头，1 使用夜用镜头
			uint8	u8Res[3];
        }Nvc_Lens_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.4.9:&nbsp;获取双镜头使用状态**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_LENS_STATUS_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_LENS_STATUS_RESP 消息体：Nvc_Lens_Status_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_lens_status_s**
		typedef struct __Nvc_Lens_Status
        {
        	uint8	u8CurLens; // 0 使用日用镜头，1 使用夜用镜头
			uint8	u8Res[3];
        }Nvc_Lens_Status_S;


#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.5:&nbsp;Led状态灯控制**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_STATE_LED_REQ 消息体：Nvc_State_Led_Control_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_STATE_LED_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_state_led_control_s**
		typedef enum __Nvc_State_Led_Color
        {
        	NV_LED_COLOR_DEFAULT	= 1,
			NV_LED_COLOR_RED		= 2,
			NV_LED_COLOR_GREEN		= 3,
        }Nvc_State_Led_Color_E;

		typedef struct __Nvc_State_Led_Control
        {
        	Nvc_State_Led_Color_E eColor;
			uint32  u32OnMesl; // 亮灯时间，单位ms
			uint32  u32OffMesl;// 灭灯时间，单位ms
        }Nvc_State_Led_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.6:&nbsp;云台控制**
#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.6.1:&nbsp;云台信息查询**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_PTZ_INFO_REQ 消息体：无            
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_PTZ_INFO_RESP 消息体：Nvc_Ptz_Info_S      

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.6.1:&nbsp;上报云台信息**  
&emsp;&emsp;驱动程序 --> 应用程序 请求消息：NVC_REPORT_PTZ_INFO_MSG 消息体：Nvc_Ptz_Info_S   
&emsp;&emsp;云台初始化完成时，如应用程序已打开驱动句柄，则向应用程序发送此信息。   
&emsp;&emsp;相关数据结构定义：

#### **nvc_ptz_info_s**

		typedef enum __NvcPtzCap
		{
		    NVC_PTZ_SUPP_HMOVE  = 0x00000001, // 是否支持水平运动
			NVC_PTZ_SUPP_VMOVE	= 0x00000002, // 是否支持垂直运动
			NVC_PTZ_SUPP_HVMOVE	= 0x00000004, // 是否支持水平垂直叠加运动( 是否支持左上,左下, 右上, 右下命令 )
			NVC_PTZ_SUPP_HSCAN	= 0x00000008, // 是否支持水平自动扫描
			NVC_PTZ_SUPP_VSCAN	= 0x00000010, // 是否支持垂直自动扫描
			NVC_PTZ_SUPP_HLIMIT = 0x00000020, // 是否支持水平限位设置
			NVC_PTZ_SUPP_VLIMIT = 0x00000040, // 是否支持垂直限位设置
			NVC_PTZ_SUPP_ZERO	= 0x00000080, // 是否支持零位检测/设置
			NVC_PTZ_SUPP_CURPOS	= 0x00000100, // 是否支持获取当前云台位置
			NVC_PTZ_SUPP_ZOOM   = 0x00000200, // 是否支持变倍
			NVC_PTZ_SUPP_FOCUS	= 0x00000400, // 是否支持手动聚焦
			NVC_PTZ_SUPP_PRESET = 0x00000800, // 是否支持预置位
			NVC_PTZ_SUPP_CRUISE = 0x00001000, // 是否支持预置位巡航
		}Nvc_Ptz_Cap_E;
		
		typedef enum __NvcPtzStatus{
			NVC_PTZ_STATUS_INITIDONE = 0x00000001
			NVC_PTZ_STATUS_Busy      = 0x00000002
			NVC_PTZ_STATUS_CmdFull   = 0x00000004
		}Nvc_Ptz_Status_E;

		typedef struct __Nvc_Ptz_Info
		{
			uint32  u32PtzCapMask;			// Nvc_Ptz_Cap_E, 指示设备支持哪些云台命令
			/***************************** 以下定义，最高位均为是否初始化标识 ****************************************/
			/****** 如uint32类型bit [0~31]，其中 bit31: 0x1 表示未初始化完成，0x0 表示已初始化完成,获取到实际参数值 ******/
			//
			uint32  u32Status;
			uint32	u32HorizontalTotSteps;	// 水平方向总步数, (u32Cap & NVC_PTZ_SUPP_HMOVE) 有效
											// 如果可一直向左/右运动或无法获取总步数, 则为0x7FFFFFFF, 
		    uint32  u32HPerStepDegrees; 	// 水平方向转动1步对应云台转动度数, 单位0.000001度, (u32Cap & NVC_PTZ_SUPP_HMOVE) 有效
			uint32	u32HorizontalMinSteps;  // 水平方向转动最小步数, (u32Cap & NVC_PTZ_SUPP_HMOVE) 有效
			//
			uint32	u32VerticalTotSteps;	// 垂直方向总步数, (u32Cap & NVC_PTZ_SUPP_VMOVE) 有效
											// 如果可一直向上/下运动或无法获取总步数， 则为0x7FFFFFFF, 
		    uint32  u32VPerStepDegrees; 	// 垂直方向转动1步对应云台转动度数, 单位0.000001度, (u32Cap & NVC_PTZ_SUPP_VMOVE) 有效
			uint32	u32VerticalMinSteps;    // 垂直方向转动最小步数，(u32Cap & NVC_PTZ_SUPP_VMOVE) 有效
			//
			uint32  u32ZeroHStepPos; 		// 云台零位相对云台最下端步数 (u32Cap & NVC_PTZ_SUPP_ZERO & NVC_PTZ_SUPP_HMOVE) 时有效
			uint32  u32ZeroVStepPos; 		// 云台零位相对云台最左端步数(u32Cap & NVC_PTZ_SUPP_ZERO & NVC_PTZ_SUPP_VMOVE) 时有效
			uint32  u32CurHStepPos;			// 云台当前位置, 相对云台最下端步数 (u32Cap & NVC_PTZ_SUPP_CURPOS & NVC_PTZ_SUPP_HMOVE) 时有效
			uint32  u32CurVStepPos;			// 云台当前位置, 相对云台最左端步数 (u32Cap & NVC_PTZ_SUPP_CURPOS & NVC_PTZ_SUPP_VMOVE) 时有效
		}Nvc_Ptz_Info_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.6.3:&nbsp;通用云台控制命令**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_PTZ_COMMON_REQ 消息体：Nvc_Ptz_Control_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_PTZ_COMMON_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_ptz_control_s**
		typedef enum __Nvc_Ptz_Cmd
        {
			NV_PTZ_STOP				= 0, // 云台停止
        	NV_PTZ_UP				= 1, // 上
			NV_PTZ_DOWN				= 2, // 下
      		NV_PTZ_LEFT				= 3, // 左
			NV_PTZ_RIGHT		    = 4, // 右
        	NV_PTZ_LEFT_UP		    = 5, // 左上
			NV_PTZ_LEFT_DOWN		= 6, // 左下
      		NV_PTZ_RIGHT_UP			= 7, // 右上
			NV_PTZ_RIGHT_DOWN		= 8, // 右下
        	NV_PTZ_ZOOM_IN		    = 9,  // 变倍+
			NV_PTZ_ZOOM_OUT			= 10, // 变倍-
        	NV_PTZ_FOCUS_NEAR		= 11, // 聚焦近
			NV_PTZ_FOCUS_FAR		= 12, // 聚焦远
			NV_PTZ_AUTO_SCAN		= 13, // 自动扫描
		    NV_PTZ_UP_LIMIT         = 14, // 上限位设置
		    NV_PTZ_DOWN_LIMIT       = 15, // 下限位设置
		    NV_PTZ_LEFT_LIMIT       = 16, // 左限位设置
		    NV_PTZ_RIGHT_LIMIT      = 17, // 右限位设置
		    NV_PTZ_PRESET_SET       = 18, // 设置预置位
		    NV_PTZ_PRESET_CLR       = 19, // 删除预置位
		    NV_PTZ_PRESET_CALL      = 20, // 调用预置位
		    NV_PTZ_START_CRUISE     = 21, // 开始巡航
		    NV_PTZ_STOP_CRUISE      = 22, // 停止巡航
		    NV_PTZ_GOTO_ZERO        = 23, // 零位检测，云台初始位置
        }Nvc_Ptz_Cmd_E;

		typedef struct __Nvc_Ptz_Control_S
		{
		    uint8   u8PtzCmd;	// Nvc_Ptz_Cmd_E 云台命令
			union {
		    	uint8   u8ParaType; // 0: 步数, 1 角度,目前仅支持步数控制
				uint8	u8No;		// 预置位号或巡航号
			};
	    	uint8   u8Speed;	// 云台速度 (1 ~ 100, 默认50) 
		    uint8	u8Res;		// 预留
		    uint32	u32HSteps;	// 水平步数 
		    uint32	u32VSteps;	// 垂直步数
		}Nvc_Ptz_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.6.4:&nbsp;设置云台预置位巡航**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_SET_PRESET_CRUISE_REQ 消息体：Nvc_Ptz_Cruise_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_SET_PRESET_CRUISE_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：  
&emsp;&emsp;
#### **nvc_ptz_cruise_s**
		#define NV_CRUISE_PRESET_NUM 16
		 
		typedef struct __Nvc_Cruise_PRESET
        {
			uint8 u8PresetNo;
			uint8 u8Speed;
			uint16 u16StaySeconds; // 单位: 秒
        }Nvc_Ptz_Cruise_S;

		typedef struct __Nvc_Ptz_Cruise
		{
		    uint8 u8CruiseNo; // 从0开始
		    uint8 u8PresetCnt;
		    uint8 u8Res[2];
		    Nvc_Cruise_Preset_S stPresets[NV_CRUISE_PRESET_NUM];
		}Nvc_Ptz_Cruise_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.6.5:&nbsp;清除云台预置位巡航**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CLEAR_PRESET_CRUISE_REQ 消息体：Nvc_Ptz_Cruise_Idx_S                 
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CLEAR_PRESET_CRUISE_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义： 
#### **nvc_ptz_cruise_idx_s**	
		typedef struct __Nvc_Ptz_Cruise_Idx
        {
			uint8 u8CruiseNo; // 从0开始
			uint8 u8Res[3];
        }Nvc_Ptz_Cruise_Idx_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.7:&nbsp;小夜灯控制**

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.7.1:&nbsp;设置小夜灯打开/关闭**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ 消息体：Nvc_Night_Light_Control_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_night_light_control_s**
		typedef struct __Nvc_Night_Light_Control
        {
        	uint8	u8Switch; // 0 关闭小夜灯，1 打开小夜灯
			uint8	u8LumLevel; // 亮度，等级 1 - 100
			uint8   u8Res[2];
        }Nvc_Night_Light_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.7.2:&nbsp;获取小夜灯打开/关闭**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_NIGHT_LIGHT_STATUS_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_NIGHT_LIGHT_STATUS_RESP 消息体：Nvc_Night_Light_Status_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_night_light_status_s**
		typedef struct __Nvc_Night_Light_Status
        {
        	uint8	u8Status; // 0 关闭小夜灯，1 打开小夜灯
			uint8	u8LumLevel; // 亮度，等级 1 - 100
			uint8  u8Res[2];
        }Nvc_Night_Light_Status_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.8:&nbsp;音频扬声器开关控制**

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.8.1:&nbsp;音频扬声器开关打开/关闭**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ 消息体：Nvc_Audio_Plug_Control_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_audio_plug_control_s**
		typedef struct __Nvc_Audio_Plug_Control
        {
        	uint8	u8Switch; // 0 关闭音频扬声器，1 打开音频扬声器
			uint8	u8Res[3];
        }Nvc_Audio_Plug_Control_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.8.2:&nbsp;音频扬声器开关打开/关闭**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_AUDIO_PLUG_STATUS_REQ 消息体：无           
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_AUDIO_PLUG_STATUS_RESP 消息体：Nvc_Audio_Plug_Status_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_audio_plug_status_s**
		typedef struct __Nvc_Audio_Plug_Status
        {
        	uint8	u8Status; // 0 关闭，1 打开
			uint8	u8Res[3];
        }Nvc_Audio_Plug_Status_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.9:&nbsp;温度采集**
#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.9.1:&nbsp;设置温度定时采集上报**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_SET_TEMPERATURE_TIMER_REQ 消息体：Nvc_Temperature_Timer_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_SET_TEMPERATURE_TIMER_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：
#### **nvc_temperature_timer_S**
		typedef struct __Nvc_Temperature_Timer
        {
        	uint32	u8DistTime; // 间隔多少秒采集1次温度，0为不采集 (默认)
        }Nvc_Temperature_Timer_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.9.2:&nbsp;查询当前温度值**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_TEMPERATURE_VALUE_REQ 消息体：无      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_TEMPERATURE_VALUE_RESP 消息体：Nvc_Temperature_Value_S   

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.9.3:&nbsp;上报当前温度值**  
&emsp;&emsp;驱动程序 --> 应用程序 请求消息：NVC_REPORT_TEMPERATURE_VALUE_MSG 消息体：Nvc_Temperature_Value_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_temperature_value_s**
		typedef struct __Nvc_Temperature_Value
        {
        	int32 s32Temperature; // 单位: 0.01摄氏度
        }Nvc_Temperature_Value_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.10:&nbsp;湿度采集**
#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.10.1:&nbsp;设置湿度定时采集上报**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_SET_HUMIDITY_TIMER_REQ 消息体：Nvc_humidity_Timer_S              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_SET_HUMIDIT_TIMER_RESP 消息体：无   
&emsp;&emsp;相关数据结构定义：
#### **nvc_humidity_timer_s**
		typedef struct __Nvc_Humidity_Timer
        {
        	uint32	u8DistTime; // 间隔多少秒采集1次湿度，0为不采集 (默认)
        }Nvc_Humidity_Timer_S;

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.10.2:&nbsp;查询当前湿度值**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_QUERY_HUMIDIT_VALUE_REQ 消息体：无      
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_QUERY_HUMIDIT_VALUE_RESP 消息体：Nvc_humidity_Value_S   

#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.10.3:&nbsp;上报当前湿度值**  
&emsp;&emsp;驱动程序 --> 应用程序 请求消息：NVC_REPORT_HUMIDIT_VALUE_MSG 消息体：Nvc_humidity_Value_S   
&emsp;&emsp;相关数据结构定义：   
#### **nvc_humidity_value_s**
		typedef struct __Nvc_Humidity_Value_S
        {
        	uint32 u32Humidity; // 百分比,单位 0.01%，比如当前湿度为60%， 则返回6000
        }Nvc_Humidity_Value_S;


#### **&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6.11:&nbsp;通过GPIO复位系统**
&emsp;&emsp;应用程序 --> 驱动程序 请求消息：NVC_GPIO_RESET_REQ 消息体：无              
&emsp;&emsp;驱动程序 --> 应用程序 响应消息：NVC_GPIO_RESET_RESP 消息体：无   
