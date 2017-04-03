> 最后修改时间：2016年12月27日 <br/>
> 修订人：Chiyuan.Ma

<hr/>
<br/>
<p><h1><b>
驱动与应用程序通信接口协议
</b></h1></p>

<br/>
<p><h2><b>
1. 概述
</b></h2></p>
<br/>

驱动程序向应用程序屏蔽不同机型的外设区别和复杂底层处理，通过统一的接口，支持应用程序通过消息指令方式查询及控制所有外部设备。 目前，可把驱动和应用程序所有交互消息分成四种类型:

    1) 设备发送查询命令，驱动返回查询结果。
    2) 设备发送控制命令，驱动返回控制结果。
    3) 设备发送控制命令，不需要驱动返回控制结果。(暂无实际消息类型，未来如果有频繁控制请求再添加)
    4) 外部设备状态发生变化或触发事件时，驱动主动反馈消息给应用程序，不需要应用程序再发响应消息给驱动程序。

应用程序通过open驱动映射的设备节点，通过write和read标准I/O函数和驱动进行通讯。驱动支持应用程序订阅事件或外设状态变化消息， 默认不订阅，当应用程序通过订阅事件指令请求接收事件或外设状态变化消息时，驱动应该把所有事件或外设状态变化消息反馈给应用程序。

驱动应支持和多个应用程序之间的通讯。驱动和应用程序之间的通讯采用异步方式，但应该保持相同消息类型之间的处理同步。 举例来说，应用程序先后发送了云台控制和查询ircut状态指令，可以先返回ircut状态响应，后返回云台控制指令响应； 应用程序先后发送了两个云台控制指令1和指令2，则应该先返回指令1的控制指令响应，再返回指令2的控制指令响应。 事件或外设状态变化消息使用异步方式上报，无需判断当前是否在处理查询或控制请求。 驱动应及时返回响应消息给应用程序，一般的查询控制命令理论上应在3s内返回，云台控制命令应在15s内返回。

部分应用程序不需要参与的外设，加载驱动时，驱动应完成响应的设备初始化操作，只是后续无相应控制操作。

<br/>
<p><h2><b>
2. 驱动装载/卸载方式
</b></h2></p>
<br/>

通过以下命令格式装载/卸载驱动:

装载设备驱动:

    insmod /mnt/mtd/modules/Nv_Driver/Nv_Driver.ko Chip=3518C Pro=D01
    insmod /mnt/mtd/modules/Nv_Driver/Nv_Driver.ko Chip=3518C Pro=D11 Pro_Cmd=NiLi_ON（NiLi_OFF）

卸载设备驱动:

    rmmod Nv_Driver

>    其中，Chip 为设备使用的芯片方案名称，Pro 为设备产品类型，某些机型有特殊的命令需求，因此根据机型的不同装载设备驱动的时候，可能还要加一些别的参数，比如 D11 机型就有一个 NiLi 的参数，该参数是为了控制驱动加载时初始化小夜灯的状态而设置， NiLi_ON 代表灯亮，NiLi_OFF 代表灯灭，该参数不选用对加载驱动也不会产生影响，驱动对该参数设定的功能将会沿用默认设定。

<br/>
<p><h2><b>
3. 消息指令格式
</b></h2></p>
<br/>
驱动和应用程序之间的消息格式为：消息头+消息体。   
<br/>

表3.1 消息格式

<table>
	<tr>
		<th width="20%">消息</td>
		<th width="20%">字节数</td>
	</tr>
	<tr>
		<td>消息头</td>
		<td>12</td>
	</tr>
	<tr>
		<td>消息体</td>
		<td>……</td>
	</tr>
</table>

<br/>
表3.2 消息头格式

<table>
	<tr>
		<th width="20%">名称</th>
		<th width="20%">字节数</th>
		<th width="60%">说明</th>
	</tr>
	<tr>
		<td>指令头魔术字(N)</td>
		<td>2</td>
		<td>默认为51843(0xCA83)，如不一致，驱动可不返回响应消息</td>
	</tr>
	<tr>
		<td>消息子类型(N)</td>
		<td>1</td>
		<td>请求消息为奇数，响应消息为相应的请求消息+1，主动上报事件或状态的消息类型也为奇数，不需要响应消息</td>
	</tr>
	<tr>
		<td>消息主类型(N)</td>
		<td>1</td>
		<td></td>
	</tr>
	<tr>
		<td>消息体长度(N)</td>
		<td>2</td>
		<td>消息携带具体消息结构体长度，如果为 0，则消息体不存在</td>
	</tr>
	<tr>
		<td>设备号(N)</td>
		<td>1</td>
		<td>从0开始，比如设备有2个button，则0为第一个，1为第二个</td>
	</tr>
	<tr>
		<td>错误码(N)</td>
		<td>1</td>
		<td>请求消息填0，响应消息填0代表操作成功，否则操作失败，详见错误码定义</td>
	</tr>
	<tr>
		<td>预留</td>
		<td>4</td>
		<td></td>
	</tr>
</table>

        // 消息头宏定义
        #define     DC_Protocal_MsgHeaderSize		12      // 消息头大小
        #define     DC_Protocal_MagicWord			0xCA83  // 消息头魔术字
        
        // 错误码定义
        #define     NVC_DRIVER_SUCCESS              0   // 无错误
        #define     NVC_DRIVER_ERR_MGCWOD           1   // 消息透验证码错误(magic word)
        #define     NVC_DRIVER_ERR_MSGTYPE_M        2   // 消息主类型错误(Massage type main)
        #define     NVC_DRIVER_ERR_MSGTYPE_S        3   // 消息子类型错误(Massage type sub)
        #define     NVC_DRIVER_ERR_PFAPP            4   // 应用层读写时，传递到驱动的指针为非法指针，不可操作
                                                            (point from application layer)
        #define     NVC_DRIVER_ERR_SLFAPP           5   // 读取数据时，应用层提供的空间不够
                                                            (space length from application layer)
        #define     NVC_DRIVER_ERR_BFORMAT          6   // 应用层提供的消息类型，与目标消息格式不符(package format)
        #define     NVC_DRIVER_ERR_BUSY             7   // 驱动正忙(Driver busy)
        #define     NVC_DRIVER_ERR_INIT             8   // 设备初始化错误
        #define     NVC_DRIVER_ERR_PLBREAK          9   // 硬件设备检测不到
                                                            （physical device lost connection）
        #define     NVC_DRIVER_ERR_UNFINISHED       10  // 操作未完成
        #define     NVC_DRIVER_ERR_MQUEUE           11  // 内存消息管理出错（不能写，读，或者直接内存溢出）
        #define     NVC_DRIVER_ERR_NO_SUPP          12  // 主消息和子消息都是对的，但可能由于机型的原因导致某些操作不支持
        #define     NVC_DRIVER_ERR_PTZ_NoEmpty      13  // 设置预置位的时候可能覆盖以前的值
        
        typedef struct{
            uint16  aMagicWord;
            uint8   aSType;
            uint8   aMType;
            uint16  aLen;
            uint8   aUint;
            uint8   aErr;
            uint8   aRemain[4];
        }mNVMsg_Head;


> 约定： 
指令头魔术字 默认为51843(0xCA83)，如不一致，驱动可不返回响应消息
消息类型 请求消息为奇数，响应消息为相应的请求消息+1，主动上报事件或状态的消息类型也为奇数，不需要响应消息
消息体长度 消息携带具体消息结构体长度，如果为 0，则消息体不存在 
设备号 从0开始，比如设备有2个button，则0为第一个，1为第二个
错误码 请求消息填0，响应消息填0代表操作成功，否则操作失败，详见错误码定义

<br/>
<p><h2><b>
4. 各消息类型的定义
</b></h2></p>
<br/>

<p><h3><b>
4.1 消息主类型
</b></h3></p>
<br/>

>   消息的主类型主要定义一驱动中包含的功能模块的代号ID，（比如：获取设备驱动信息的代号为0），目前的协议中仅支持 0-255 种可能的功能。
>   之所以把消息类型中标识功能模块的 ID 放在功能模块对应详细操作 ID 的后面是基于 linux 中存储信息的方式为小端存储，换句话说，如果将八位的消息主类型和八位的消息子类型转换成16位的消息类型，表示产品功能的 ID将在高八位，而表示产品功能的详细操作将在低八位中。

下表定义了消息主类型的宏名、值、以及类型介绍

表4.1.1 主消息类型主消息类型
<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="40%">类型宏名</th>
		<th width="20%">类型值</th>
		<th width="30%">类型功能</th>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_DEVICE</td>
		<td>0x0000</td>
		<td>设备信息</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_BUTTON</td>
		<td>0x0100</td>
		<td>按键</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_LDR</td>
		<td>0x0200</td>
		<td>光敏电阻</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_IRC</td>
		<td>0x0300</td>
		<td>IRC</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_IfrLIGHT</td>
		<td>0x0400</td>
		<td>红外灯</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_StaLIGHT</td>
		<td>0x0500</td>
		<td>状态灯</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_PTZ</td>
		<td>0x0600</td>
		<td>云台</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_NitLIGHT</td>
		<td>0x0700</td>
		<td>小夜灯</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_AudioPLGU</td>
		<td>0x0800</td>
		<td>音频开关</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_TempMONITOR</td>
		<td>0x0900</td>
		<td>温度检测</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_HumiMONITOR</td>
		<td>0x0A00</td>
		<td>湿度检测</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_DoubLENS</td>
		<td>0x0B00</td>
		<td>双镜头</td>
	</tr>
	<tr>
		<td></td>
		<td>0x0C00</td>
		<td>复位</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_RTC</td>
		<td>0x0D00</td>
		<td>实时时钟</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_PIR</td>
		<td>0x0E00</td>
		<td>移动红外侦测</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_DoorBell</td>
		<td>0x0F00</td>
		<td>门铃</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_DoorLock</td>
		<td>0x1000</td>
		<td>门锁</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_FM1288</td>
		<td>0x1100</td>
		<td>FM1288</td>
	</tr>
	<tr>
		<td>NVC_MSG_TYPE_EAS</td>
		<td>0x1200</td>
		<td>电子防盗</td>
	</tr>
</table>



        // 设备功能类型宏定义在MsgProtocal.h中定义
        #define NVC_MSG_TYPE_DEVICE                     0x0000
        #define NVC_MSG_TYPE_BUTTON                     0x0100
        #define NVC_MSG_TYPE_LDR                        0x0200
        #define NVC_MSG_TYPE_IRC                        0x0300
        #define NVC_MSG_TYPE_IfrLIGHT                   0x0400
        #define NVC_MSG_TYPE_StaLIGHT                   0x0500
        #define NVC_MSG_TYPE_PTZ                        0x0600
        #define NVC_MSG_TYPE_NitLIGHT                   0x0700
        #define NVC_MSG_TYPE_AudioPLGU                  0x0800
        #define NVC_MSG_TYPE_TempMONITOR                0x0900
        #define NVC_MSG_TYPE_HumiMONITOR                0x0A00
        #define NVC_MSG_TYPE_DoubLENS                   0x0B00
        #define NVC_MSG_TYPE_RTC                        0x0D00
        #define NVC_MSG_TYPE_PIR                        0x0E00
        #define NVC_MSG_TYPE_DoorBell                   0x0F00
        #define NVC_MSG_TYPE_DoorLock                   0x1000
        #define NVC_MSG_TYPE_FM1288                     0x1100
        #define NVC_MSG_TYPE_EAS                        0x1200
	

<br/>
<p><h3><b>
4.2 消息子类型
</b></h3></p>
<br/>
<p><h4><b>
4.2.1 SubMessageType: DEVICE（0x0000）
</b></h4></p>
<br/>


>   设备的第一个功能模块，这个模块是每个产品都必须加载的，它支持查询当前驱动的状态、编译版本、加载的产品类型、支持的功能、以及设置自动上报信息功能。
>   特别要注意的是 NVC_MSG_DEVICE_REPORT_DRIVER_ERR 消息，该消息会上报因消息格式、硬件连接出现的突发错误，应用程序监视这条消息，能很方便快速的定位错误发生的位置。

详细协议内容见下表：

表4.2.1.1 SubMsg：DEVICE
<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_GET_INFO</td>
		<td>1</td>
		<td>获取驱动信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_GET_INFO_RESP</td>
		<td>2</td>
		<td>获取驱动信息响应信息</td>
		<td>mNVC_DRV_Ver_INFO</td>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_GET_CAP</td>
		<td>3</td>
		<td>获取设备外设能力集</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_GET_CAP_RESP</td>
		<td>4</td>
		<td>获取设备外设能力集响应信息</td>
		<td>mNVC_DRV_CAP_INFO</td>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_SUB_REPORT</td>
		<td>5</td>
		<td>设置是否接收驱动事件及状态变化请求</td>
		<td>mNVC_ATTACHED_Msg</td>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_SUB_REPORT_RESP</td>
		<td>6</td>
		<td>设置是否接收驱动事件及状态变化请求响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_DEVICE_REPORT_DRIVER_ERR</td>
		<td>7</td>
		<td>主动上报驱动警告，错误</td>
		<td>无</td>
	</tr>
</table>

        // DDEVICE MODE 中包含的消息子类型宏定义
        #define NVC_MSG_TYPE_DEVICE						0x0000
        #define NVC_MSG_DEVICE_GET_INFO					1
        #define NVC_MSG_DEVICE_GET_INFO_RESP			2
        #define NVC_MSG_DEVICE_GET_CAP					3
        #define NVC_MSG_DEVICE_GET_CAP_RESP				4
        #define NVC_MSG_DEVICE_SUB_REPORT				5
        #define NVC_MSG_DEVICE_SUB_REPORT_RESP			6
        #define NVC_MSG_DEVICE_REPORT_DRIVER_ERR        7


        /* NVC_MSG_DEVICE_GET_INFO_RESP 获取驱动信息响应信息的结构体
            aChipInfo       芯片类型信息，见第二节
            aDeviceInfo     产品类型信息，见第二节
            aVerInfo        驱动版本类型，由字符串组成，第一个数字标示大版本号，第二个数字，标示累计版本号。
                            测试版会在后面就加上 Beta 字样,如果是稳定版，则不会加后缀。
                                如：V:03:001:Beta V:03.002
            aBuildData      编译信息，格式为 作者+编译时间
                                如：15-8-1 13:00
        */
        typedef struct{
            uint32 aChipInfo;
            uint32 aDeviceInfo;
            uint8  aVerInfo[16];
            uint8  aBuildData[32];
        }mNVC_DRV_Ver_INFO;


        /* NVC_MSG_DEVICE_GET_CAP_RESP 获取设备外设能力集响应信息的结构体
            aDrvCapMask     能力集：【注1： 附件2 —— 表1】
            aNumOfButton    按键个数
            aNumOfStaLED    状态灯个数
            aRemain          保留
        */
        typedef struct{
            uint32 aDrvCapMask;
            uint8  aNumOfButton;
            uint8  aNumOfStaLED;
            uint8  aRemin[2];
        }mNVC_DRV_CAP_INFO;
        
        
        /* NVC_MSG_DEVICE_SUB_REPORT_RESP 设置是否接收驱动事件及状态信息的结构体
            aAttched        1：注册 上报事件/状态信息   DC_NVCMD_ATTACHED_SubMsg
                            0：注销 上报事件/状态信息   DC_NVCMD_ATTACHED_UnsubMsg
            aRemain         保留
        */
        #define DC_NVCMD_ATTACHED_SubMsg 	0x01
        #define DC_NVCMD_ATTACHED_UnsubMsg 	0x00
        typedef struct{
            uint8	aAttched;
            uint8	aRemain[3];
        }mNVC_ATTACHED_Msg;

<br/>
<p><h4><b>
4.2.2 SubMessageType:BUTTON（0x0100）
</b></h4></p>
<br/>

>   不同的产品中可能拥有不同数量的 Button，如果应用层想要获取当前加载的驱动中的 Button 的数量可以发送 NVC_MSG_DEVICE_GET_CAP 在返回的消息体 mNVC_DRV_CAP_INFO 中的 aNumOfButton 会包含 Button 的总数。
>   另外在获取 Button 的状态时，要在消息头 mNVMsg_Head 的 aUint 字段标示需要获取的是哪个 Button （数值为 0 表示第一个 button）；同样想要知道 Button 上报消息是属于哪个 Button 的时候，查看消息头 mNVMsg_Head 的 aUint 字段即可。

表4.2.2.1 SubMsg：BUTTON

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>

	</tr>
	<tr>
		<td>NVC_MSG_BUTTON_GET_STATUS</td>
		<td>1</td>
		<td>获取Button当前状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_BUTTON_GET_STATUS_RESP</td>
		<td>2</td>
		<td>获取Button当前状态响应信息</td>
		<td>mNVC_BUTTON_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_BUTTON_REPORT_STATUS</td>
		<td>3</td>
		<td>上报Button当前状态</td>
		<td>mNVC_BUTTON_STATUS</td>
	</tr>
</table>

        // BUTTON MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_BUTTON                     0x0100
        #define     NVC_MSG_BUTTON_GET_STATUS               1
        #define     NVC_MSG_BUTTON_GET_STATUS_RESP          2
        #define     NVC_MSG_BUTTON_REPORT_STATUS            3
        
        
        /* NVC_MSG_BUTTON_GET_STATUS_RESP 和 NVC_MSG_BUTTON_REPORT_STATUS
            u8Status        0: 上报-弹起事件发生  查询-按键处于正常状态(未按下)
                            1: 上报-按下事件发生  查询-按键已经按下
            aRemain         保留
        */
        #define     DC_NVBUTT_ButtonHight		0x01
        #define     DC_NVBUTT_ButtonLow			0x02
        #define     DC_NVBUTT_ButtonRaise		0x04
        #define     DC_NVBUTT_ButtonFall		0x08
        typedef struct{
            uint8	aStatus;        // 0 未被按下，1 被按下
            uint8	aRemain[3];
        }mNVC_BUTTON_STATUS;

<br/>
<p><h4><b>
4.2.3 SubMessageType:LDR（0x0200）
</b></h4></p>
<br/>

>   白天黑夜检测模块（LDR or Day&Night Monitor），负责检测环境中的照度，有照度状态变化时，驱动层会主动上报应用层。 
>   应用层可以修改驱动中该模块的检测灵敏度，其中调节灵敏度主要从从两个方面设置，一个是临界点 aSPoint，一个是消抖区间大小 aDomain，详细配置见配置灵敏度结构体 mNVC_DNMONITOR_SENSI。

表4.2.3.1 SubMsg：LDR
<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_GET_STATE</td>
		<td>1</td>
		<td>获取光敏电阻当前状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_GET_STATE_RESP</td>
		<td>2</td>
		<td>获取光敏电阻当前状态响应信息</td>
		<td>mNVC_DNMONITOR_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_REPORT_STATE</td>
		<td>3</td>
		<td>上报光敏电阻当前状态</td>
		<td>mNVC_DNMONITOR_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_SET_SENSITIVE</td>
		<td>5</td>
		<td>设置光敏电阻检测灵敏度</td>
		<td>mNVC_DNMONITOR_SENSI</td>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_SET_SENSITIVE_RESP</td>
		<td>6</td>
		<td>设置光敏电阻检测灵敏度响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_GET_SENSITIVE</td>
		<td>7</td>
		<td>获取光敏电阻检测灵敏度</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_LDR_GET_SENSITIVE_RESP</td>
		<td>8</td>
		<td>获取光敏电阻检测灵敏度响应信息</td>
		<td>mNVC_DNMONITOR_SENSI</td>
	</tr>
</table>


        // LDR MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_LDR                        0x0200
        #define     NVC_MSG_LDR_GET_STATE                   1
        #define     NVC_MSG_LDR_GET_STATE_RESP              2
        #define     NVC_MSG_LDR_REPORT_STATE                3
        #define     NVC_MSG_LDR_SET_SENSITIVE               5
        #define     NVC_MSG_LDR_SET_SENSITIVE_RESP          6
        #define     NVC_MSG_LDR_GET_SENSITIVE               7
        #define     NVC_MSG_LDR_GET_SENSITIVE_RESP			8


        /* NVC_MSG_LDR_GET_STATE_RESP 和 NVC_MSG_LDR_REPORT_STATE 的消息结构体
            u8Status        0: 当前环境状态为黑夜   DC_NVANMonitor_NIGHT
                            1: 当前环境状态为白天   DC_NVANMonitor_DAY
            aRemain         保留
            aVal            光敏电阻采样值
        */
        #define     DC_NVANMonitor_NIGHT	0x00		//通红外光
        #define     DC_NVANMonitor_DAY		0x01        //不通红外光
        typedef struct{
        	uint8   aStatus;
        	uint8   aRemain;
            uint16  aVal;
        }mNVC_DNMONITOR_STATUS;

        
        /* NVC_MSG_LDR_SET_SENSITIVE 和 NVC_MSG_LDR_GET_SENSITIVE_RESP 的消息结构体
            aSPoint         调节检测白天黑夜的临界点，调节范围（0-100）
                            设置为0会没有黑夜状态，也不会上报状态
                            设置为100会没有白天状态，也不会上报状态
            aDomain         调节白天黑夜消除抖动的缓冲大小，调节范围（0-100）
                            设置为0，严格的根据临界值做判断
                            设置成其他值则驱动会适当的改变临界值的判断边界
                    以上两个参数超过设置范围会触发驱动上报异常（消息类型NVC_MSG_DEVICE_REPORT_DRIVER_ERR）
            aReamin         保留
        */
        typedef struct{
        	uint8  aSPoint;
            uint8  aDomain;
        	uint8  aReamin[2];
        }mNVC_DNMONITOR_SENSI;

<br/>
<p><h4><b>
4.2.4 SubMessageType:IRCut（0x0300）
</b></h4></p>
<br/>


表4.2.4.1 SubMsg：IRCut
<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_IRC_GET_TYPE</td>
		<td>1</td>
		<td>获取设备IRCut类型</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_IRC_GET_TYPE_RESP</td>
		<td>2</td>
		<td>获取设备IRCut类型响应信息</td>
		<td>mNVC_IRFILT_TYPE</td>
	</tr>
	<tr>
		<td>NVC_MSG_IRC_SET_STATUS</td>
		<td>3</td>
		<td>设置IRCut切换状态</td>
		<td>mNVC_IRFILT_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_IRC_SET_STATUS_RESP</td>
		<td>4</td>
		<td>设置IRCut切换状态响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_IRC_GET_STATUS</td>
		<td>5</td>
		<td>查询IRCut当前状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_IRC_GET_STATUS_RESP</td>
		<td>6</td>
		<td>查询IRCut当前状态响应信息</td>
		<td>mNVC_IRFILT_STATUS</td>
	</tr>
</table>


        // IRC MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_IRC                        0x0300
        #define     NVC_MSG_IRC_GET_TYPE                    1
        #define     NVC_MSG_IRC_GET_TYPE_RESP               2
        #define     NVC_MSG_IRC_SET_STATUS                  3
        #define     NVC_MSG_IRC_SET_STATUS_RESP             4
        #define     NVC_MSG_IRC_GET_STATUS                  5
        #define     NVC_MSG_IRC_GET_STATUS_RESP             6


        /* NVC_MSG_IRC_GET_TYPE_RESP 的消息结构体
            aType           上报 IRC 的类型，目前有两种，
                            1. TYPEA 8002芯片搭建的驱动电路
                            2. TYPEB 分立元件搭建的电路
                            作用： 暂时只是标示不同机型的不同 IRC 模块，并没有其他作用
            aRemain         保留            
        */
        #define     DC_NVIfrFilt_TypeA	    0x00000001
        #define     DC_NVIfrFilt_TypeB	    0x00000002
        #define     DC_NVIfrFilt_TypeC	    0x00000003
        typedef struct{
        	uint8 aType;
            uint8 aRemain[3];
        }mNVC_IRFILT_TYPE;
        
        
        /* IRC_SET_SWITCH_REQ & IRC_GET_STATUS_RESP 的消息结构体
            aStatus         0   代表控制 IRC 模块切换至阻挡红外光的状态     DC_NVIfrFilt_BlockInfr
                            1   代表控制 IRC 模块切换至允许红外光通过的状态     DC_NVIfrFilt_PassInfr
            aRemain         保留
        */
        #define DC_NVIfrFilt_PassInfr	0x01
        #define DC_NVIfrFilt_BlockInfr	0x00
        typedef struct{
        	uint8 aStatus;
        	uint8 aRemain[3];
        }mNVC_IRFILT_STATUS;

<br/>
<p><h4><b>
4.2.5 SubMessageType:Infrared Light（0x0400）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_IfrLIGHT_SET_STATUS</td>
		<td>1</td>
		<td>设置红外灯打开/关闭</td>
		<td>mNVC_IRLIGHT_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_IfrLIGHT_SET_STATUS_RESP</td>
		<td>2</td>
		<td>设置红外灯打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_IfrLIGHT_GET_STATUS</td>
		<td>3</td>
		<td>查询红外灯打开/关闭状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_IfrLIGHT_GET_STATUS_RESP</td>
		<td>4</td>
		<td>查询红外灯打开/关闭状态响应信息</td>
		<td>mNVC_IRLIGHT_STATUS</td>
	</tr>
</table>


        // IfrLight MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_IfrLIGHT                   0x0400
        #define     NVC_MSG_IfrLIGHT_SET_STATUS             1
        #define     NVC_MSG_IfrLIGHT_SET_STATUS_RESP        2
        #define     NVC_MSG_IfrLIGHT_GET_STATUS             3
        #define     NVC_MSG_IfrLIGHT_GET_STATUS_RESP        4


        /*  NVC_MSG_IfrLIGHT_SET_STATUS & NVC_MSG_IfrLIGHT_GET_STATUS_RESP 的消息结构体
            aStatus         0   关闭红外灯  DC_NVIrLight_OFF
                            1   打开红外灯  DC_NVIrLight_ON
            aRemain         保留
        */
        #define     DC_NVIrLight_ON			0x01
        #define     DC_NVIrLight_OFF		0x00
        typedef struct{
        	uint8 aStatus;
        	uint8 aRemain[3];
        }mNVC_IRLIGHT_STATUS;

<br/>
<p><h4><b>
4.2.6 SubMessageType:State Light（0x0500）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_StaLIGHT_SET_STATUS</td>
		<td>1</td>
		<td>设置状态灯工作方式</td>
		<td>mNVC_STALIGHT_SETINFO</td>
	</tr>
	<tr>
		<td>NVC_MSG_StaLIGHT_SET_STATUS_RESP</td>
		<td>2</td>
		<td>设置状态灯工作方式响应信息</td>
		<td>无</td>
	</tr>
</table>

        // StateLight MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_StaLIGHT                   0x0500
        #define     NVC_MSG_StaLIGHT_SET_STATUS             1
        #define     NVC_MSG_StaLIGHT_SET_STATUS_RESP        2

        
        /* NVC_MSG_StaLIGHT_SET_STATUS 的消息结构体
            注意：这里只有控制灯的状态
            aLightMode:
                    如果一个产品（设备）里面拥有多个个灯的话，指示操作哪一个灯
                    需要在消息头 mNVMsg_Head 的 aUnit 字段里面指明，
                    默认从0开始，0代表第一个灯。如果某些灯一个中含有多种颜色，
                    那么，我们要在 aLightMode 字段指明（如红灯：即 NVC_STATE_LIGHT_ID_RED）。
            aOnMes:
            aOffMes:
                    这两个字段分别标示，灯亮的时间，灯灭的时间，单位为1ms，
                    如此便可控制灯的开关，也可控制灯的闪烁，如果要常亮，
                    将aOffMes设置成0，aOnMes设置非零，即可，
                    不过可能因为硬件特性的原因，建议非零的那个值越大越好。
            aBrthFrq：
                    当 aLightMode 为 NVC_STATE_LIGHT_ID_BREATH 模式的时候，
                    这个参数会用来调节呼吸灯的频率快慢，范围从 0-10，
                    当选择 0 的的时候会选择默认值，选择 1-10 频率会逐步上升，
                    超过这些限定值的时候操作会视为无效操作。
        */
        #define     NVC_STATE_LIGHT_ID_DEFAULT              0       
        #define     NVC_STATE_LIGHT_ID_RED                  1       
        #define     NVC_STATE_LIGHT_ID_GREEN                2       
        #define     NVC_STATE_LIGHT_ID_BLUE                 3       
        #define     NVC_STATE_LIGHT_ID_BREATH               4       
        #define     NVC_STATE_LIGHT_ID_RACING               5
        typedef struct{
        	uint32 aLightMode;
        	union{
                uint32 aOnMes;      // 亮灯时间，单位：ms
                uint32 aBrthFrq;    // 呼吸灯的频率
            };
        	uint32 aOffMes;         // 灭灯时间，单位：ms
        }mNVC_STALIGHT_SETINFO;

<br/>
<p><h4><b>
4.2.7 SubMessageType:PTZ（0x0600）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_GET_INFO</td>
		<td>1</td>
		<td>获取云台信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_GET_INFO_RESP</td>
		<td>2</td>
		<td>获取云台信息响应信息</td>
		<td>mNVC_PANTILT_INFO</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_SET_ACTION</td>
		<td>3</td>
		<td>设置云台控制指令</td>
		<td>mNVC_PANTILT_SET</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_SET_ACTION_RESP</td>
		<td>4</td>
		<td>设置云台控制指令响应信息</td>
		<td>mNVC_PANTILT_Respons</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_SET_PRESET</td>
		<td>5</td>
		<td>设置云台预置位巡航</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_SET_PRESET_RESP</td>
		<td>6</td>
		<td>设置云台预置位巡航响应信息</td>
		<td>mNVC_PTZCurPos</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_CLR_PRESET</td>
		<td>7</td>
		<td>清除云台预置位巡航</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_CLR_PRESET_RESP</td>
		<td>8</td>
		<td>清除云台预置位巡航响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_REPORT_STATUS</td>
		<td>9</td>
		<td>上报云台状态信息</td>
		<td>mNVC_PANTILT_INFO</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_INPORRT_PRESET_P_REQ</td>
		<td>11</td>
		<td></td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_INPORRT_PRESET_P_RESP</td>
		<td>12</td>
		<td></td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_SET_CRUISE_PATH_REQ</td>
		<td>13</td>
		<td>设置云台预置位巡航区域</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PTZ_SET_CRUISE_PATH_RESP</td>
		<td>14</td>
		<td>设置云台预置位巡航区域响应信息</td>
		<td>无</td>
	</tr>
</table>


        // PTZ MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_PTZ                        0x0600
        #define     NVC_MSG_PTZ_GET_INFO                    1
        #define     NVC_MSG_PTZ_GET_INFO_RESP               2
        #define     NVC_MSG_PTZ_SET_ACTION                  3
        #define     NVC_MSG_PTZ_SET_ACTION_RESP             4
        #define     NVC_MSG_PTZ_SET_PRESET                  5
        #define     NVC_MSG_PTZ_SET_PRESET_RESP             6
        #define     NVC_MSG_PTZ_CLR_PRESET                  7
        #define     NVC_MSG_PTZ_CLR_PRESET_RESP             8
        #define     NVC_MSG_PTZ_REPORT_STATUS               9
        #define     NVC_MSG_PTZ_INPORRT_PRESET_P_REQ        11
        #define     NVC_MSG_PTZ_INPORRT_PRESET_P_RESP       12
        #define     NVC_MSG_PTZ_SET_CRUISE_PATH_REQ         13
        #define     NVC_MSG_PTZ_SET_CRUISE_PATH_RESP        14


        // PTZ MASK 云台控制支持的功能
        #define	    NVC_PTZ_SUPP_HMOVE  	 	0x00000001  // 是否支持水平运动
        #define	    NVC_PTZ_SUPP_VMOVE  	 	0x00000002  // 是否支持垂直运动
        #define	    NVC_PTZ_SUPP_HVMOVE 	 	0x00000004  // 是否支持水平垂直叠加运动
                                                            (//是否支持左上,左下, 右上, 右下命令)
        #define	    NVC_PTZ_SUPP_HSCAN  	 	0x00000008  // 是否支持水平自动扫描
        #define	    NVC_PTZ_SUPP_VSCAN  	 	0x00000010  // 是否支持垂直自动扫描
        #define	    NVC_PTZ_SUPP_HLIMIT 	 	0x00000020  // 是否支持水平限位设置
        #define	    NVC_PTZ_SUPP_VLIMIT 	 	0x00000040  // 是否支持垂直限位设置
        #define	    NVC_PTZ_SUPP_ZERO   	 	0x00000080  // 是否支持零位检测/设置
        #define	    NVC_PTZ_SUPP_CURPOS 	 	0x00000100  // 是否支持获取当前云台位置
        #define	    NVC_PTZ_SUPP_PRESET 	 	0x00000800  // 是否支持预置位
        #define	    NVC_PTZ_SUPP_CRUISE 	 	0x00001000  // 是否支持预置位巡航

        // 标示云台当前的状态
        #define     NVC_PTZ_STATUS_Initing		0x00000001	// 设备初始化完成
        #define     NVC_PTZ_STATUS_Busy			0x00000002	//
        #define     NVC_PTZ_STATUS_CmdFull		0x00000004  //
        
        /* NVC_MSG_PTZ_GET_INFO_RESP & NVC_MSG_PTZ_REPORT_STATUS 的消息结构体
                云台在启动的时候，某些机型会进行初始状态检测，所以可能会导致一个情况，
                云台还没有初始化完，应用层就来获取云台的信息，这样就会导致信息不全面。
                所以在云台自检完成后，驱动会主动上报一条云台的消息，结构类型为 mNVC_PANTILT_INFO。
            aCmdMask
                    由一个个标志位构成，标示云台所具有的功能，包括是否支持垂直水平，有无限位等
            aStatus
                    标示当前云台状态，是否初始化完，是否处于忙碌状态
            aHRange
                    水平方向总步数, (aCmdMask & NVC_PTZ_SUPP_HMOVE) 时有效，
                    如果可一直向左/右运动或无法获取总步数，则为0x7FFFFFFF
            aDgrPerHSteps        
                    水平方向转动1步对应云台转动度数，单位0.000001度， 
                    (u32Cap & NVC_PTZ_SUPP_HMOVE) 时有效
            aDgrPerHSteps        
                    水平方向转动最小单位的步数，(aCmdMask & NVC_PTZ_SUPP_HMOVE) 时有效
            aVRange        
                    垂直方向总步数, (aCmdMask & NVC_PTZ_SUPP_VMOVE) 时有效，
                    如果可一直向上/下运动或无法获取总步数， 则为0x7FFFFFFF
            aDgrPerVSteps        
                    垂直方向转动1步对应云台转动度数，单位0.000001度，
                    (aCmdMask & NVC_PTZ_SUPP_VMOVE) 有效
            aUnitVSteps        
                    垂直方向转动最小步数，(aCmdMask & NVC_PTZ_SUPP_VMOVE) 时有效
            aZeroXPos        
                    云台零位相对云台最下端步数
                    (aCmdMask & NVC_PTZ_SUPP_ZERO & NVC_PTZ_SUPP_HMOVE) 时有效
            aZeroYPos
                    云台零位相对云台最左端步数
                    (aCmdMask & NVC_PTZ_SUPP_ZERO & NVC_PTZ_SUPP_VMOVE) 时有效
            aXPos        
                    云台当前位置, 相对云台最下端步数
                    (aCmdMask & NVC_PTZ_SUPP_CURPOS & NVC_PTZ_SUPP_HMOVE) 时有效
            aYPos        
                    云台当前位置, 相对云台最左端步数
                    (aCmdMask & NVC_PTZ_SUPP_CURPOS & NVC_PTZ_SUPP_VMOVE) 时有效
            结构体 mNVC_PANTILT_INFO 除 aCmdMask 外的所有成员 ，最高位均为是否初始化标识，
            如：uint32类型bit[0~31]
                bit31:      0x1 表示未初始化完成
                            0x0 表示已初始化完成，获取到实际参数值
        */
        typedef struct{                                                               //
        	uint32 aCmdMask;        // 指示设备支持哪些云台功能
        	uint32 aStatus;         // 标示当前云台初始化状态
        	uint32 aHRange;         // 水平方向总步数
        	uint32 aDgrPerHSteps;   // 水平方向转动1步对应云台转动度数
        	uint32 aUnitHSteps;     // 水平方向转动最小单位的步数
        	uint32 aVRange;         // 垂直方向总步数
        	uint32 aDgrPerVSteps;   // 垂直方向转动1步对应云台转动度数
        	uint32 aUnitVSteps;     // 垂直方向转动最小步数
        	uint32 aZeroXPos;       // 云台零位相对云台最下端步数
        	uint32 aZeroYPos;       // 云台零位相对云台最左端步数
        	uint32 aXPos;           // 云台当前位置, 相对云台最下端步数
        	uint32 aYPos;           // 云台当前位置, 相对云台最左端步数
        }mNVC_PANTILT_INFO;  


        // 云台的控制命令
        #define     NV_PTZ_STOP        		0   // 云台停止
        #define     NV_PTZ_UP          		1   // 上
        #define     NV_PTZ_DOWN        		2   // 下
        #define     NV_PTZ_LEFT        		3   // 左
        #define     NV_PTZ_RIGHT       		4   // 右
        #define     NV_PTZ_LEFT_UP     		5   // 左上
        #define     NV_PTZ_LEFT_DOWN   		6   // 左下
        #define     NV_PTZ_RIGHT_UP    		7   // 右上
        #define     NV_PTZ_RIGHT_DOWN  		8   // 右下
        #define     NV_PTZ_MVT_PRESET       9   // 移动至指定预置位
        #define     NV_PTZ_STAR_CRUISE      10  // 开始巡航
        #define     NV_PTZ_AUTO_SCAN   		13  // 自动扫描
        #define     NV_PTZ_UP_LIMIT    		14  // 上限位设置
        #define     NV_PTZ_DOWN_LIMIT  		15  // 下限位设置
        #define     NV_PTZ_LEFT_LIMIT  		16  // 左限位设置
        #define     NV_PTZ_RIGHT_LIMIT 		17  // 右限位设置
        //#define   NV_PTZ_GOTO_ZERO   		23  // 零位检测，云台初始位置

        /* NVC_MSG_PTZ_SET_ACTION 的消息结构体
            云台分为三种操作模式，步数，角度，坐标控制，在没有限位的机型中，坐标控制是不准确的。
            aCmd
                    控制命令，拥有的控制方式见云台控制命令
            aParaType
                    云台控制模式，步数，角度，坐标
                    0   步数    steps
                    1   角度    angle
                    2   坐标    coordinate
            aNo
                    如果控制命令是针对预置位巡航的，则这个字段标示预置位的下标
            aSpeed
                    云台的运转速度，值范围：0-100，默认为 0，大约为 36， 超出范围将会产生警告
            aHParam
                    水平方向控制分量
            aVParam
                    垂直方向控制分量
            aReamin
                    保留
        */
        #define     NV_PTZ_SpeedDefault		0       // 云台默认速度
        #define     NV_PTZ_SpeedMax			100     // 云台最大速度
        #define     NV_PTZ_SpeedMin			1       // 云台最小速度
        typedef struct{
        	uint8  aCmd; 
        	union{ 
        		uint8 aParaType;        // 0 steps， 1 angle， 2 coordinate
        		uint8 aNo;              // 预置位号或巡航号
        	};
        	uint8  aSpeed;              // 云台速度（1--100）
        	uint8  aReamin[1];          // 保留
        	uint32 aHParam;             // 水平方向控制分量
        	uint32 aVParam;             // 垂直方向控制分量
        }mNVC_PANTILT_SET;


        /*
            设置预置位的某一个节点
            aPreSetNo
                    预置位巡航点序号
            aSpeed
                    速度，指从别的点移动到该点的速度
            aStaySeconds
                    停留时间，指在进行自动预置位巡航的时候，停留在该点的时间，
                    如果只是将云台移至该点，那么云台在下次操作前都会一直停留在该点。
        */
        #define     NVC_CRUISE_PRESET_NUM       16      // 预置位巡航点的个数
        typedef struct{
        	uint8 	aPreSetNo;          // 预置位巡航点序号
        	uint8 	aSpeed;             // 速度
        	uint16 	aStaySeconds;       // 停留时间，单位：秒
        }mNVC_PTCruiseUnit; 


        /*
            预置位巡航点节点的相对位置坐标
            aXPos
                    云台当前位置, 相对云台最下端步数
            aYPos
                    云台当前位置, 相对云台最左端步数
        */
        typedef struct{
            uint16  aXPos;      // 云台当前位置, 相对云台最下端步数
            uint16  aYPos;      // 云台当前位置, 相对云台最左端步数
        }mNVC_PTZCurPos;


        typedef struct{
            uint8   aMvPath[NVC_CRUISE_PRESET_NUM];             //
        }mNVC_PTZMvPath;
        //
        typedef struct{
            mNVC_PTCruiseUnit   aInfo[NVC_CRUISE_PRESET_NUM];   //
            mNVC_PTZCurPos      aCoo[NVC_CRUISE_PRESET_NUM];    //
        }mNVC_PTZPreSetConfF;
        //
        #define     NV_PTZ_STA_Success 			0x01            //
        #define     NV_PTZ_STA_RemainSpace		0x02            //
        typedef struct{
        	uint8  aStatus;         //
        	uint8  aRemain[3];      //
        	uint32 aXPos;           //
        	uint32 aYPos;           //
        }mNVC_PANTILT_Respons;


<br/>
<p><h4><b>
4.2.8 SubMessageType:NightLight（0x0700）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_NitLIGHT_SET_STATUS</td>
		<td>1</td>
		<td>设置小夜灯状态打开/关闭</td>
		<td>mNVC_NIGHTLIGHT_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_NitLIGHT_SET_STATUS_RESP</td>
		<td>2</td>
		<td>设置小夜灯状态打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_NitLIGHT_GET_STATUS</td>
		<td>3</td>
		<td>查询小夜灯状态打开/关闭</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_NitLIGHT_GET_STATUS_RESP</td>
		<td>4</td>
		<td>查询小夜灯状态打开/关闭响应信息</td>
		<td>mNVC_NIGHTLIGHT_STATUS</td>
	</tr>
</table>

        // NightLight MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_NitLIGHT                   0x0700
        #define     NVC_MSG_NitLIGHT_SET_STATUS             1
        #define     NVC_MSG_NitLIGHT_SET_STATUS_RESP        2
        #define     NVC_MSG_NitLIGHT_GET_STATUS             3
        #define     NVC_MSG_NitLIGHT_GET_STATUS_RESP        4

        
        /* NVC_MSG_NitLIGHT_SET_STATUS & NVC_MSG_NitLIGHT_GET_STATUS_RESP 的消息结构体
            aStatus
                    0   关闭小夜灯      DC_NIGHTLIGHT_Off
                    1   开启小夜灯      DC_NIGHTLIGHT_On
            aLemLevel
                    1-100   超限会产生警告
            aRemain
                    保留
        */
        #define     DC_NIGHTLIGHT_Off		0x00
        #define     DC_NIGHTLIGHT_On		0x01
        typedef struct{
        	uint8	aStatus;
        	uint8 	aLemLevel;
        	uint8 	aRemain[2];
        }mNVC_NIGHTLIGHT_STATUS;


<br/>
<p><h4><b>
4.2.9 SubMessageType:Audio Plug（0x0800）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_SPEAKER_SET_STATUS</td>
		<td>1</td>
		<td>设置音频扬声器打开/关闭</td>
		<td>mNVC_AUDIOPLUG_Msg</td>
	</tr>
	<tr>
		<td>NVC_MSG_SPEAKER_SET_STATUS_RESP</td>
		<td>2</td>
		<td>设置音频扬声器打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_SPEAKER_GET_STATUS</td>
		<td>3</td>
		<td>查询音频扬声器打开/关闭状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_SPEAKER_GET_STATUS_RESP</td>
		<td>4</td>
		<td>查询音频扬声器打开/关闭状态响应信息</td>
		<td>mNVC_AUDIOPLUG_Msg</td>
	</tr>
	<tr>
		<td>NVC_MSG_MICROPH_SET_STATUS</td>
		<td>5</td>
		<td>设置音频麦克风打开/关闭</td>
		<td>mNVC_AUDIOPLUG_Msg</td>
	</tr>
	<tr>
		<td>NVC_MSG_MICROPH_SET_STATUS_RESP</td>
		<td>6</td>
		<td>设置音频麦克风打开/关闭状态响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_MICROPH_GET_STATUS</td>
		<td>7</td>
		<td>查询音频麦克风打开/关闭状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_MICROPH_GET_STATUS_RESP</td>
		<td>8</td>
		<td>查询音频麦克风打开/关闭状态响应信息</td>
		<td>mNVC_AUDIOPLUG_Msg</td>
	</tr>
</table>

        // AudioPlug MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_AudioPLGU                  0x0800
        #define     NVC_MSG_SPEAKER_SET_STATUS              1
        #define     NVC_MSG_SPEAKER_SET_STATUS_RESP         2
        #define     NVC_MSG_SPEAKER_GET_STATUS              3
        #define     NVC_MSG_SPEAKER_GET_STATUS_RESP         4
        #define     NVC_MSG_MICROPH_SET_STATUS              5
        #define     NVC_MSG_MICROPH_SET_STATUS_RESP         6
        #define     NVC_MSG_MICROPH_GET_STATUS              7
        #define     NVC_MSG_MICROPH_GET_STATUS_RESP         8
        
        
        /* NVC_MSG_SPEAKER_SET_STATUS & NVC_MSG_SPEAKER_GET_STATUS_RESP &
        NVC_MSG_MICROPH_SET_STATUS & NVC_MSG_MICROPH_GET_STATUS_RESP 的消息结构体
            aStatus
                    0   关闭      DC_NVAuPl_Off
                    1   打开      DC_NVAuPl_On
                    2   不支持    DC_NVAuPl_NOSUPP
            aRemain
                    保留
        */
        #define     DC_NVAuPl_Off				0x00
        #define     DC_NVAuPl_On				0x01
        #define     DC_NVAuPl_NOSUPP            0x02 
        typedef struct{
        	uint8 aStatus;
        	uint8 aRemain[3];
        }mNVC_AUDIOPLUG_Msg;


<br/>
<p><h4><b>
4.2.A SubMessageType:Temp Monitor（0x0900）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_TempMONITOR_SUB_REPORT</td>
		<td>1</td>
		<td>设置温度定时采集上报</td>
		<td>mNVC_TEMPMONITOR_Timer</td>
	</tr>
	<tr>
		<td>NVC_MSG_TempMONITOR_SUB_REPORT</td>
		<td>2</td>
		<td>设置温度定时采集上报响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_TempMONITOR_GET_VALUE</td>
		<td>3</td>
		<td>查询当前温度值</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_TempMONITOR_GET_VALUE_RESP</td>
		<td>4</td>
		<td>查询当前温度值响应信息</td>
		<td>mNVC_TEMPMONITOR_Value</td>
	</tr>
	<tr>
		<td>NVC_MSG_TempMONITOR_REPORT_VALUE</td>
		<td>5</td>
		<td>上报当前温度值消息</td>
		<td>mNVC_TEMPMONITOR_Value</td>
	</tr>
</table>

        // Temperature MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_TempMONITOR                0x0900
        #define     NVC_MSG_TempMONITOR_SUB_REPORT          1
        #define     NVC_MSG_TempMONITOR_SUB_REPORT_RESP     2
        #define     NVC_MSG_TempMONITOR_GET_VALUE           3
        #define     NVC_MSG_TempMONITOR_GET_VALUE_RESP      4
        #define     NVC_MSG_TempMONITOR_REPORT_VALUE        5
        
        
        /* NVC_MSG_TempMONITOR_SUB_REPORT 的消息结构体
            aTimes
                    设置上报温度数据的间隔时间，单位为 1s， 
                    如果设置为 0，如果之前有上报机制，则会取消上报机制，
                    如果没有，则不会产生任何影响。
        */
        typedef struct{
        	uint32 aTimes;
        }mNVC_TEMPMONITOR_Timer;
        
        /*  NVC_MSG_TempMONITOR_GET_VALUE_RESP & NVC_MSG_TempMONITOR_REPORT_VALUE 的消息结构体
            aValue  
                    获取或上报的温度值，精度为 0.01 度。
        */
        typedef struct{
        	int32 aValue;
        }mNVC_TEMPMONITOR_Value;


<br/>
<p><h4><b>
4.2.B SubMessageType:Humidity Monitor（0x0A00）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_HumiMONITOR_SUB_REPORT</td>
		<td>1</td>
		<td>设置湿度定时采集上报</td>
		<td>mNVC_HUMIDITY_Timer</td>
	</tr>
	<tr>
		<td>NVC_MSG_HumiMONITOR_SUB_REPORT_RESP</td>
		<td>2</td>
		<td>设置湿度定时采集上报响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_HumiMONITOR_GET_VALUE</td>
		<td>3</td>
		<td>查询当前湿度值</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_HumiMONITOR_GET_VALUE_RESP</td>
		<td>4</td>
		<td>查询当前湿度值响应信息</td>
		<td>mNVC_HUMIDITY_Value</td>
	</tr>
	<tr>
		<td>NVC_MSG_HumiMONITOR_REPORT_VALUE</td>
		<td>5</td>
		<td>上报当前湿度值消息</td>
		<td>mNVC_HUMIDITY_Value</td>
	</tr>
</table>


        // Humidity MODE 中包含的 消息子类型宏定义
        #define     NVC_MSG_TYPE_HumiMONITOR                0x0A00
        #define     NVC_MSG_HumiMONITOR_SUB_REPORT          1
        #define     NVC_MSG_HumiMONITOR_SUB_REPORT_RESP     2
        #define     NVC_MSG_HumiMONITOR_GET_VALUE           3
        #define     NVC_MSG_HumiMONITOR_GET_VALUE_RESP      4
        #define     NVC_MSG_HumiMONITOR_REPORT_VALUE        5
        
        
        /* NVC_MSG_HumiMONITOR_SUB_REPORT 的消息结构体
            aTimes  
                    设置上报湿度数据的间隔时间，单位为 1s， 
                    如果设置为 0，如果之前有上报机制，则会取消上报机制，
                    如果没有，则不会产生任何影响。
        */
        typedef struct{
        	uint32 aTimes;
        }mNVC_HUMIDITY_Timer;
        
        /* NVC_MSG_HumiMONITOR_GET_VALUE_RESP & NVC_MSG_HumiMONITOR_REPORT_VALUE 的消息结构体
            aValue  
                    获取或上报的湿度值，精度为 0.01%。
        */
        typedef struct{
        	int32 aValue;
        }mNVC_HUMIDITY_Value;


<br/>
<p><h4><b>
4.2.C SubMessageType:Double Lens（0x0B00）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_DoubLENS_SET_STATUS</td>
		<td>1</td>
		<td>设置双镜头切换(日用/夜用镜头)</td>
		<td>mNVC_DUBLENS_SETINFO</td>
	</tr>
	<tr>
		<td>NVC_MSG_DoubLENS_SET_STATUS_RESP</td>
		<td>2</td>
		<td>设置双镜头切换响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_DoubLENS_GET_STATUS</td>
		<td>3</td>
		<td>查询双镜头使用状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_DoubLENS_GET_STATUS_RESP</td>
		<td>4</td>
		<td>查询双镜头使用状态响应信息</td>
		<td>mNVC_DUBLENS_SETINFO</td>
	</tr>
</table>


        // DoubleLens MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_DoubLENS                   0x0B00
        #define     NVC_MSG_DoubLENS_SET_STATUS             1
        #define     NVC_MSG_DoubLENS_SET_STATUS_RESP        2
        #define     NVC_MSG_DoubLENS_GET_STATUS             3
        #define     NVC_MSG_DoubLENS_GET_STATUS_RESP        4
        
        
        /* NVC_MSG_DoubLENS_SET_STATUS & NVC_MSG_DoubLENS_GET_STATUS_RESP 的消息结构类型
            aStatus
                    0   切换至日用镜头      DC_DUBLENS_DAYLENS
                    1   切换至夜用镜头      DC_DUBLENS_NIGHTLENS
        */
        #define     DC_DUBLENS_NIGHTLENS	0x01
        #define     DC_DUBLENS_DAYLENS		0x00
        typedef struct{
        	uint8 aStatus;          // 0 使用日用镜头，1 使用夜用镜头
        	uint8 aRemain[3];
        }mNVC_DUBLENS_SETINFO;


<br/>
<p><h4><b>
4.2.D SubMessageType:RTC（0x0D00）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_RTC_SET_TIME</td>
		<td>1</td>
		<td>设置（同步）实时时钟时间</td>
		<td>mNVC_RTC_DATA</td>
	</tr>
	<tr>
		<td>NVC_MSG_RTC_SET_TIME_RESP</td>
		<td>2</td>
		<td>设置（同步）实时时钟时间响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_RTC_GET_TIME</td>
		<td>3</td>
		<td>获取实时时钟当前时间</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_RTC_GET_TIME_RESP</td>
		<td>4</td>
		<td>获取实时时钟当前时间响应信息</td>
		<td>mNVC_RTC_DATA</td>
	</tr>
</table>

        
        // RTC MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_RTC                        0x0D00
        #define     NVC_MSG_RTC_SET_TIME                    1
        #define     NVC_MSG_RTC_SET_TIME_RESP               2
        #define     NVC_MSG_RTC_GET_TIME                    3
        #define     NVC_MSG_RTC_GET_TIME_RESP               4
        
        
        /* NVC_MSG_RTC_SET_TIME & NVC_MSG_RTC_GET_TIME_RESP 消息结构体
            data-time 类型
        */
        typedef struct{
            uint8 aSecond;
            uint8 aMinute;
            uint8 aHour;
            uint8 aDay;
            uint8 aWeekday;
            uint8 aMonth;
            uint8 aYear;
        }mNVC_RTC_DATA;


<br/>
<p><h4><b>
4.2.E SubMessageType:PIR（0x0E00）
</b></h4></p>
<br/>

>   注意：如果应用层利用 NVC_MSG_TYPE_DEVICE 的 NVC_MSG_DEVICE_SUB_REPORT 订阅了上报的消息， PIR 检测到有物体的活动，就会上报给应用层，但应用层去主动获取的当时可能并没有检测到有 Activity，也不知道过去的那段时间检测到了几次 Activity，因此此时返回上来的是驱动挂载起到当下所检测到的次数总和。

>   PIR 的检测基于红外热电堆的电荷堆积，因此检测的时候是不可预测的，可能单个现实生活中的 Activity 会产生 N 次 PIR 触发，因此在第一次检测到之后建议事先设置一段死区（只需设置一次），这样可以滤掉不必要的干扰。用 NVC_MSG_TYPE_PIR 的 NVC_MSG_PIR_SET_DeadTime 设置死区时间（默认 3s）。


<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_PIR_GET_STATUS</td>
		<td>1</td>
		<td>获取热释电红外检测模块状态</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_PIR_GET_STATUS_RESP</td>
		<td>2</td>
		<td>获取热释电红外检测模块状态响应信息</td>
		<td>mNVC_PIR_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_PIR_REPORT_STATUS</td>
		<td>3</td>
		<td>上报热释电红外检测模块状态</td>
		<td>mNVC_PIR_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_PIR_SET_DeadTime</td>
		<td>5</td>
		<td>设置死区时间</td>
		<td>mNVC_PIR_DEAD_TIME</td>
	</tr>
	<tr>
		<td>NVC_MSG_PIR_SET_DeadTime_RESP</td>
		<td>6</td>
		<td>设置死去时间响应消息</td>
		<td>无</td>
	</tr>
</table>


        // PIR MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_PIR                        0x0E00
        #define     NVC_MSG_PIR_GET_STATUS                  1
        #define     NVC_MSG_PIR_GET_STATUS_RESP             2
        #define     NVC_MSG_PIR_REPORT_STATUS               3
        #define     NVC_MSG_PIR_SET_DeadTime                5
        #define     NVC_MSG_PIR_SET_DeadTime_RESP           6
        
        
        /* NVC_MSG_PIR_GET_STATUS_RESP & NVC_MSG_PIR_REPORT_STATUS 消息结构体
            aStatus
                    0   没有检测到异常      DC_MsgPIR_Discover_Normal
                    1   检测到异常          DC_MsgPIR_Discover_Unnormal
            aRemain[3]
                    保留
            aCounts
                    当应用层主动去获取是否有事件发生时，会得到此数据，该字段记录侦测到异常的总次数。
            aDistance
                    被检测物体距当前检测物体的距离，单位：厘米
        */
        #define     DC_MsgPIR_Discover_Normal      0x00
        #define     DC_MsgPIR_Discover_Unnormal    0x01
        typedef struct{
            union{
                struct{
                    uint8   aStatus;
                    uint8   aRemain[3];
                };
                uint32  aCounts;
            };
            uint32  aDistance;
        }mNVC_PIR_STATUS;
        
        /* NVC_MSG_PIR_SET_DeadTime 的消息结构类型
            aTime
                    设置死区时间，单位为 0.1s
        */
        typedef struct{
            uint32 aTime;
        }mNVC_PIR_DEAD_TIME;


<br/>
<p><h4><b>
4.2.F SubMessageType:DoorBell（0x0F00）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_DoorBell_SET_Status</td>
		<td>1</td>
		<td>发送DoorBell配置信息</td>
		<td>mNVC_DoorBell_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_DoorBell_SET_Status_RESP</td>
		<td>2</td>
		<td>发送DoorBell配置信息响应信息</td>
		<td>无</td>
	</tr>
</table>


        // DoorBell MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_DoorBell                   0x0F00
        #define     NVC_MSG_DoorBell_SET_Status             1
        #define     NVC_MSG_DoorBell_SET_Status_RESP        2
        
        
        /* NVC_MSG_DoorBell_SET_Status 消息结构体
            aStatus
                    1       触发门铃    DC_MsgDoorBell_TapBell
                    others  没反应
            aRemain
                    保留
        */
        #define     DC_MsgDoorBell_TapBell     0x01
        typedef struct{
            uint8   aStatus;
            uint8   aRemain[3];
        }mNVC_DoorBell_STATUS;


<br/>
<p><h4><b>
4.2.10 SubMessageType:DoorLock（0x1000）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_DoorLock_SET_STATUS</td>
		<td>1</td>
		<td>发送DoorLock配置信息</td>
		<td>mNVC_DoorLock_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_DoorLock_SET_STATUS_RESP</td>
		<td>2</td>
		<td>发送DoorLock配置信息响应信息</td>
		<td>无</td>
	</tr>
</table>


        // DoorLock MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_DoorLock                   0x1000
        #define     NVC_MSG_DoorLock_SET_Status             1
        #define     NVC_MSG_DoorLock_SET_Status_RESP        2
        
        
        /* NVC_MSG_DoorBell_SET_Status 消息结构体
            aStatus
                    0       关闭门锁    DC_MsgDoorLock_Close
                    1       打开门锁    DC_MsgDoorLock_Open
            aRemain
                    保留
        */
        #define     DC_MsgDoorLock_Close       0x00        // 关闭门锁
        #define     DC_MsgDoorLock_Open        0x01        // 打开门锁
        typedef struct{
        	uint8 aStatus;
        	uint8 aRemain[3];
        }mNVC_DoorLock_STATUS;


<br/>
<p><h4><b>
4.2.11 SubMessageType:FM1288（0x1100）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_FM1288_WRITE_REGISTER</td>
		<td>1</td>
		<td>发送DoorLock配置信息</td>
		<td>mNVC_DoorLock_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_FM1288_WRITE_REGISTER_RESP</td>
		<td>2</td>
		<td>发送DoorLock配置信息响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_FM1288_READ_REGISTER</td>
		<td>3</td>
		<td>发送DoorLock配置信息</td>
		<td>mNVC_DoorLock_STATUS</td>
	</tr>
	<tr>
		<td>NVC_MSG_FM1288_READ_REGISTER_RESP</td>
		<td>4</td>
		<td>发送DoorLock配置信息响应信息</td>
		<td>无</td>
	</tr>
</table>


        // FM1288 MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_FM1288                     0x1100
        #define     NVC_MSG_FM1288_WRITE_REGISTER           1
        #define     NVC_MSG_FM1288_WRITE_REGISTER_RESP      2
        #define     NVC_MSG_FM1288_READ_REGISTER            3
        #define     NVC_MSG_FM1288_READ_REGISTER_RESP       4
        
        
        /* NVC_MSG_FM1288_WRITE_REGISTER & NVC_MSG_FM1288_READ_REGISTER 消息结构体
            u8Data
                    在向寄存器中写入数值时，指向要被写入FM1288的数据地址
            u32DataLength
                    在向寄存器中写入数值时，要被写入FM1288的数据长度
            u8RegHigh
                    在读取寄存器值时，要读取寄存器地址的高字节
            u8RegLow
                    在读取寄存器值时，要读取寄存器地址的低字节
        */
        typedef struct{
        	uint8  *u8Data;         // the data which want to write
        	uint32 u32DataLength;   // data length
        	uint8  u8RegHigh;       // register address high byte
        	uint8  u8RegLow;        // register address low byte
        }mNVC_FM1288_Msg;


<br/>
<p><h4><b>
4.2.12 SubMessageType:EAS（0x1200）
</b></h4></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="35%">命令类型</th>
		<th width="8%">类型值</th>
		<th width="30%">类型说明</th>
		<th width="25%">消息类型</th>
	</tr>
	<tr>
		<td>NVC_MSG_EAS_GET_STATUS</td>
		<td>1</td>
		<td>获取当前防拆状态</td>
		<td>mNVC_EAS_Msg</td>
	</tr>
	<tr>
		<td>NVC_MSG_EAS_GET_STATUS_RESP</td>
		<td>2</td>
		<td>获取当前防拆状态响应信息</td>
		<td>无</td>
	</tr>
	<tr>
		<td>NVC_MSG_EAS_REPORT_STATUS</td>
		<td>3</td>
		<td>获取当前防拆状态</td>
		<td>mNVC_EAS_Msg</td>
	</tr>
</table>


        // EAS MODE 中包含的消息子类型宏定义
        #define     NVC_MSG_TYPE_EAS                        0x1200
        #define     NVC_MSG_EAS_GET_STATUS                  1
        #define     NVC_MSG_EAS_GET_STATUS_RESP             2
        #define     NVC_MSG_EAS_REPORT_STATUS               3
        
        
        /* NVC_MSG_EAS_GET_STATUS & NVC_MSG_EAS_REPORT_STATUS 消息结构体
            aStatus
                    0       安全状态        DC_NVEAS_SAFTY
                    1       被拆开状态      DC_NVEAS_EMERGENCY
            aRemain
                    保留
        */
        #define     DC_NVEAS_SAFTY          0x00
        #define     DC_NVEAS_EMERGENCY      0x01
        typedef struct{
            uint8   aStatus;
            uint8   aRemain[3];
        }mNVC_EAS_Msg;

<br/>
<p><h2><b>
附件1：兼容老协议 MsgType
</b></h2></p>
<br/>

最初的协议为{魔术字(2Bety)+消息类型(2Bety)+消息长度(2Bety)+Unit(1Bety)+Error(1Bety)+REMAIN(4)(4Bety)}, 与现在的协议区别主要在消息类型部分，以前的协议消息类型间的区分不清楚。


表5.1 Old Protocol

<table class="table table-bordered table-striped table-condensed">
	<tr>
	    <th width="15%">消息类型值</th>
		<th width="40%">消息枚举定义</th>
		<th width="25%">消息定义描述</th>
		<th width="20%">消息结构体</th>
	</tr>
	<tr>
	    <td>0X0001</td>
		<td>NVC_QUERY_DRIVER_INFO_REQ</td>
		<td>获取驱动信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0002</td>
		<td>NVC_QUERY_DRIVER_INFO_RESP</td>
		<td>获取驱动信息响应信息</td>
		<td>Nvc_Driver_Ver_Info_S</td>
	</tr>
	<tr>
	    <td>0X0003</td>
		<td>NVC_QUERY_DRIVER_CAPACITY_REQ</td>
		<td>获取设备外设能力集</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0004</td>
		<td>NVC_QUERY_DRIVER_CAPACITY_RESP</td>
		<td>获取设备外设能力集响应信息</td>
		<td>Nvc_Driver_Cap_Info_s</td>
	</tr>
	<tr>
	    <td>0X0005</td>
		<td>NVC_SET_ATTACHED_DRIVER_MSG_REQ</td>
		<td>设置是否接收驱动事件及状态变化请求</td>
		<td>Nvc_Attached_Driver_Msg_s</td>
	</tr>
	<tr>
	    <td>0X0006</td>
		<td>NVC_SET_ATTACHED_DRIVER_MSG_RESP</td>
		<td>设置是否接收驱动事件及状态变化信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0001</td>
		<td>NVC_QUERY_DRIVER_INFO_REQ</td>
		<td>获取驱动信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0101</td>
		<td>NVC_QUERY_BUTTON_STATUS_REQ</td>
		<td>获取button当前状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0102</td>
		<td>NVC_QUERY_BUTTON_STATUS_RESP</td>
		<td>获取button当前状态响应信息</td>
		<td>Nvc_Button_Status_S</td>
	</tr>
	<tr>
	    <td>0X0103</td>
		<td>NVC_REPORT_BUTTON_STATUS_MSG</td>
		<td>上报button当前状态</td>
		<td>Nvc_Button_Status_S</td>
	</tr>
	<tr>
	    <td>0X0201</td>
		<td>NVC_QUERY_LDR_STATUS_REQ</td>
		<td>获取光敏电阻当前状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0202</td>
		<td>NVC_QUERY_LDR_STATUS_RESP</td>
		<td>获取光敏电阻当前状态响应信息</td>
		<td>Nvc_Ldr_Status_S</td>
	</tr>
	<tr>
	    <td>0X0203</td>
		<td>NVC_REPORT_LDR_STATUS_MSG</td>
		<td>上报光敏电阻当前状态</td>
		<td>Nvc_Ldr_Status_S</td>
	</tr>
	<tr>
	    <td>0X0205</td>
		<td>NVC_SET_LDR_SENSITIVITY_REQ</td>
		<td>设置光敏电阻检测灵敏度</td>
		<td>Nvc_Ldr_Senitivity_S</td>
	</tr>
	<tr>
	    <td>0X0206</td>
		<td>NVC_SET_LDR_SENSITIVITY_RESP</td>
		<td>设置光敏电阻检测灵敏度响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0207</td>
		<td>NVC_QUERY_LDR_SENSITIVITY_REQ</td>
		<td>获取光敏电阻检测灵敏度</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0208</td>
		<td>NVC_QUERY_LDR_SENSITIVITY_RESP</td>
		<td>获取光敏电阻检测灵敏度响应信息</td>
		<td>Nvc_Ldr_Senitivity_S</td>
	</tr>
	<tr>
	    <td>0X0301</td>
		<td>NVC_QUERY_IRC_TYPE_REQ</td>
		<td>获取设备ircut类型</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0302</td>
		<td>NVC_QUERY_IRC_TYPE_RESP</td>
		<td>获取设备ircut类型响应信息</td>
		<td>Nvc_Ircut_Info_S</td>
	</tr>
	<tr>
	    <td>0X0303</td>
		<td>NVC_CONTROL_IRC_SWITCH_REQ</td>
		<td>设置ircut切换状态</td>
		<td>Nvc_Ircut_Control_S</td>
	</tr>
	<tr>
	    <td>0X0304</td>
		<td>NVC_CONTROL_IRC_SWITCH_RESP</td>
		<td>设置ircut切换状态响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0305</td>
		<td>NVC_QUERY_IRC_STATUS_REQ</td>
		<td>查询ircut当前状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0306</td>
		<td>NVC_QUERY_IRC_STATUS_RESP</td>
		<td>查询ircut当前状态响应信息</td>
		<td>Nvc_Ircut_Status_S</td>
	</tr>
	<tr>
	    <td>0X0401</td>
		<td>NVC_CONTROL_LAMP_SWITCH_REQ</td>
		<td>设置红外灯打开/关闭</td>
		<td>Nvc_Lamp_Control_S</td>
	</tr>
	<tr>
	    <td>0X0402</td>
		<td>NVC_CONTROL_LAMP_SWITCH_RESP</td>
		<td>设置红外灯打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0403</td>
		<td>NVC_QUERY_LAMP_STATUS_REQ</td>
		<td>查询红外灯打开/关闭状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0404</td>
		<td>NVC_QUERY_LAMP_STATUS_RESP</td>
		<td>查询红外灯打开/关闭状态响应信息</td>
		<td>Nvc_Lamp_Status_S</td>
	</tr>
	<tr>
	    <td>0X0501</td>
		<td>NVC_CONTROL_STATE_LED_REQ</td>
		<td>设置LED灯显示方式</td>
		<td>Nvc_State_Led_Control_S</td>
	</tr>
	<tr>
	    <td>0X0502</td>
		<td>NVC_CONTROL_STATE_LED_RESP</td>
		<td>设置LED灯显示方式响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0601</td>
		<td>NVC_QUERY_PTZ_INFO_REQ</td>
		<td>云台信息查询</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0602</td>
		<td>NVC_QUERY_PTZ_INFO_RESP</td>
		<td>云台信息查询响应信息</td>
		<td>Nvc_Ptz_Info_S</td>
	</tr>
	<tr>
	    <td>0X0603</td>
		<td>NVC_CONTROL_PTZ_COMMON_REQ</td>
		<td>通用云台控制指令</td>
		<td>Nvc_Ptz_Control_S</td>
	</tr>
	<tr>
	    <td>0X0604</td>
		<td>NVC_CONTROL_PTZ_COMMON_RESP</td>
		<td>通用云台控制指令响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0605</td>
		<td>NVC_SET_PRESET_CRUISE_REQ</td>
		<td>设置云台预置位巡航</td>
		<td>Nvc_Ptz_Cruise_S</td>
	</tr>
	<tr>
	    <td>0X0606</td>
		<td>NVC_SET_PRESET_CRUISE_RESP</td>
		<td>设置云台预置位巡航响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0607</td>
		<td>NVC_CLEAR_PRESET_CRUISE_REQ</td>
		<td>清除云台预置位巡航</td>
		<td>Nvc_Ptz_Cruise_Idx_S</td>
	</tr>
	<tr>
	    <td>0X0608</td>
		<td>NVC_CLEAR_PRESET_CRUISE_RESP</td>
		<td>清除云台预置位巡航响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0609</td>
		<td>NVC_REPORT_PTZ_INFO_MSG</td>
		<td>上报云台信息</td>
		<td>Nvc_Ptz_Info_S</td>
	</tr>
	<tr>
	    <td>0X0701</td>
		<td>NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ</td>
		<td>设置小夜灯打开/关闭</td>
		<td>Nvc_Night_Light_Control_s</td>
	</tr>
	<tr>
	    <td>0X0702</td>
		<td>NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP</td>
		<td>设置小夜灯打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0703</td>
		<td>NVC_QUERY_NIGHT_LIGHT_STATUS_REQ</td>
		<td>查询小夜灯打开/关闭状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0704</td>
		<td>NVC_QUERY_NIGHT_LIGHT_STATUS_RESP</td>
		<td>查询小夜灯打开/关闭状态响应信息</td>
		<td>Nvc_Night_Light_status_s</td>
	</tr>
	<tr>
	    <td>0X0801</td>
		<td>NVC_CONTROL_Speaker_SWITCH_REQ</td>
		<td>设置音频扬声器打开/关闭</td>
		<td>Nvc_Audio_Plug_Control_s</td>
	</tr>
	<tr>
	    <td>0X0802</td>
		<td>NVC_CONTROL_Speaker_SWITCH_RESP</td>
		<td>设置音频扬声器打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0803</td>
		<td>NVC_QUERY_Speaker_STATUS_REQ</td>
		<td>查询音频扬声器打开/关闭状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0804</td>
		<td>NVC_QUERY_Speaker_STATUS_RESP</td>
		<td>查询音频扬声器打开/关闭状态响应信息</td>
		<td>Nvc_Audio_Plug_status_s</td>
	</tr>
	<tr>
	    <td>0X0805</td>
		<td>NVC_CONTROL_Microph_SWITCH_REQ</td>
		<td>设置音频麦克风打开/关闭</td>
		<td>Nvc_Audio_Plug_Control_s</td>
	</tr>
	<tr>
	    <td>0X0806</td>
		<td>NVC_CONTROL_Microph_SWITCH_RESP</td>
		<td>设置音频麦克风打开/关闭响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0807</td>
		<td>NVC_QUERY_Microph_STATUS_REQ</td>
		<td>查询音频麦克风状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0808</td>
		<td>NVC_QUERY_Microph_STATUS_RESP</td>
		<td>查询音频麦克风状态响应信息</td>
		<td>Nvc_Audio_Plug_status_s</td>
	</tr>
	<tr>
	    <td>0X0901</td>
		<td>NVC_SET_TEMPERATURE_TIMER_REQ</td>
		<td>设置温度定时采集上报请求</td>
		<td>Nvc_Temperature_Timer_S</td>
	</tr>
	<tr>
	    <td>0X0902</td>
		<td>NVC_SET_TEMPERATURE_TIMER_RESP</td>
		<td>设置温度定时采集上报响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0903</td>
		<td>NVC_QUERY_TEMPERATURE_VALUE_REQ</td>
		<td>查询当前温度值</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0904</td>
		<td>NVC_QUERY_TEMPERATURE_VALUE_RESP</td>
		<td>查询当前温度值响应信息</td>
		<td>Nvc_Temperature_Value_S</td>
	</tr>
	<tr>
	    <td>0X0905</td>
		<td>NVC_REPORT_TEMPERATURE_VALUE_MSG</td>
		<td>上报当前温度值消息</td>
		<td>Nvc_Temperature_Value_S</td>
	</tr>
	<tr>
	    <td>0X0A01</td>
		<td>NVC_SET_HUMIDITY_TIMER_REQ</td>
		<td>设置湿度度定时采集上报请求</td>
		<td>Nvc_Humidity_Timer_S</td>
	</tr>
	<tr>
	    <td>0X0A02</td>
		<td>NVC_SET_HUMIDIT_TIMER_RESP</td>
		<td>设置湿度定时采集上报响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0A03</td>
		<td>NVC_QUERY_HUMIDIT_VALUE_REQ</td>
		<td>查询当前湿度值</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0A04</td>
		<td>NVC_QUERY_HUMIDIT_VALUE_RESP</td>
		<td>查询当前湿度值响应信息</td>
		<td>Nvc_Humidity_Value_S</td>
	</tr>
	<tr>
	    <td>0X0A05</td>
		<td>NVC_REPORT_HUMIDIT_VALUE_MSG</td>
		<td>上报当前湿度值消息</td>
		<td>Nvc_Humidity_Value_S</td>
	</tr>
	<tr>
	    <td>0X0B01</td>
		<td>NVC_CONTROL_LENS_SWITCH_REQ</td>
		<td>设置双镜头切换(日用镜头/夜用镜头)</td>
		<td>Nvc_Lens_Control_S</td>
	</tr>
	<tr>
	    <td>0X0B02</td>
		<td>NVC_CONTROL_LENS_SWITCH_RESP</td>
		<td>设置双镜头切换响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0B03</td>
		<td>NVC_QUERY_LENS_STATUS_REQ</td>
		<td>查询双镜头使用状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0B04</td>
		<td>NVC_QUERY_LENS_STATUS_RESP</td>
		<td>查询双镜头使用状态响应信息</td>
		<td>Nvc_Lens_Status_S</td>
	</tr>
	<tr>
	    <td>0X0C01</td>
		<td>NVC_GPIO_RESET_REQ</td>
		<td>通过GPIO复位系统</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0X0C02</td>
		<td>NVC_GPIO_RESET_RESP</td>
		<td>通过GPIO复位系统响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0x0D01</td>
		<td>NVC_RTC_SET_TIME_REQ</td>
		<td>设置（同步）实时时钟时间</td>
		<td>Nvc_Time_Struct_S</td>
	</tr>
	<tr>
	    <td>0x0D02</td>
		<td>NVC_RTC_SET_TIME_RESP</td>
		<td>设置（同步）实时时钟时间响应信息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0x0D03</td>
		<td>NVC_RTC_GET_TIME_REQ</td>
		<td>获取实时时钟当前时间</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0x0D04</td>
		<td>NVC_RTC_GET_TIME_RESP</td>
		<td>获取实时时钟当前时间响应信息</td>
		<td>Nvc_Time_Struct_S</td>
	</tr>
	<tr>
	    <td>0x0E01</td>
		<td>NVC_PIR_GET_STATUS_REQ</td>
		<td>获取当前热设红外检测模块状态</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0x0E02</td>
		<td>NVC_PIR_GET_STATUS_RESP</td>
		<td>获取当前热设红外检测模块状态响应信息</td>
		<td>Nvc_PIRStatus_Struct_S</td>
	</tr>
	<tr>
	    <td>0x0E03</td>
		<td>NVC_PIR_REPORT_STATUS</td>
		<td>上报当前热设红外检测模块状态</td>
		<td>Nvc_PIRStatus_Struct_S</td>
	</tr>
	<tr>
	    <td>0x0E05</td>
		<td>NVC_PIR_SET_DEAD_TIME_REQ</td>
		<td>设置死区时间</td>
		<td>Nvc_PIRConf_Struct_S</td>
	</tr>
	<tr>
	    <td>0x0E06</td>
		<td>NVC_PIR_SET_DEAD_TIME_RESP</td>
		<td>设置死去时间响应消息</td>
		<td>无</td>
	</tr>
	<tr>
	    <td>0x0F01</td>
		<td>NVC_DOORBELL_SET_STATUS_REQ</td>
		<td>发送DoorBell配置信息</td>
		<td>Nvc_DoorBellConf_Status_S</td>
	</tr>
	<tr>
	    <td>0x0F02</td>
		<td>NVC_DOORBELL_SET_STATUS_RESP</td>
		<td>发送DoorBell配置响应信息</td>
		<td>无</td>
	</tr>

</table>



<br/>
<p><h2><b>
附件2：设备能力集
</b></h2></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
	    <th width="8%">序号</th>
		<th width="30%">能力集</th>
		<th width="20%">掩码</th>
		<th width="50%">能力集备注</th>
	</tr>
	<tr>
	    <td>1</td>
		<td>CAP_SUPP_ButtonMonitor</td>
		<td>0x00000001</td>
		<td>是否支持按键功能</td>
	</tr>
	<tr>
	    <td>2</td>
		<td>CAP_SUPP_LdrMonitor</td>
		<td>0x00000002</td>
		<td>是否支持日夜模式检测功能</td>
	</tr>
	<tr>
	    <td>3</td>
		<td>CAP_SUPP_Ircut</td>
		<td>0x00000004</td>
		<td>是否支持滤光片切换功能</td>
	</tr>
	<tr>
	    <td>4</td>
		<td>CAP_SUPP_IfrLamp</td>
		<td>0x00000008</td>
		<td>是否支持红外灯功能</td>
	</tr>
	<tr>
		<td>5</td>
		<td>CAP_SUPP_DoubleLens</td>
		<td>0x00000010</td>
		<td>是否支持双镜头功能</td>
	</tr>
	<tr>
		<td>6</td>
		<td>CAP_SUPP_StateLed</td>
		<td>0x00000020</td>
		<td>是否支持状态灯指示功能</td>
	</tr>
	<tr>
		<td>7</td>
		<td>CAP_SUPP_PTZ</td>
		<td>0x00000040</td>
		<td>是否支持云台功能</td>
	</tr>
	<tr>
		<td>8</td>
		<td>CAP_SUPP_NightLight</td>
		<td>0x00000080</td>
		<td>是否支持小夜灯功能</td>
	</tr>
	<tr>
		<td>9</td>
		<td>NVC_SUPP_CoolFan</td>
		<td>0x00000100</td>
		<td>是否支持散热风扇功能</td>
	</tr>
	<tr>
		<td>10</td>
		<td>CAP_SUPP_AudioPlug</td>
		<td>0x00000200</td>
		<td>是否支持音频开关功能</td>
	</tr>
	<tr>
		<td>11</td>
		<td>CAP_SUPP_TempMonitor</td>
		<td>0x00000400</td>
		<td>是否支持温度采集功能</td>
	</tr>
	<tr>
		<td>12</td>
		<td>CAP_SUPP_HumiMonitor</td>
		<td>0x00000800</td>
		<td>是否支持湿度采集功能</td>
	</tr>
	<tr>
		<td>13</td>
		<td>CAP_SUPP_GpioReset</td>
		<td>0x00001000</td>
		<td>通过GPIO复位设备(重启设备)功能</td>
	</tr>
	<tr>
		<td>14</td>
		<td>CAP_SUPP_RTC</td>
		<td>0x00002000</td>
		<td>是否支持实时时钟（RTC）功能</td>
	</tr>
	<tr>
		<td>15</td>
		<td>CAP_SUPP_PIR</td>
		<td>0x00004000</td>
		<td>是否支持移动红外侦测（PIR）功能</td>
	</tr>
	<tr>
		<td>16</td>
		<td>CAP_SUPP_DoorBell</td>
		<td>0x00008000</td>
		<td>是否支持门铃（DoorBell）功能</td>
	</tr>
	<tr>
		<td>17</td>
		<td>CAP_SUPP_DoorLock</td>
		<td>0x00010000</td>
		<td>是否支持门锁（DoorLock）功能</td>
	</tr>
	<tr>
		<td>18</td>
		<td>CAP_SUPP_FM1288</td>
		<td>0x00020000</td>
		<td>是否支持回音消除（FM1288）功能</td>
	</tr>
	<tr>
		<td>19</td>
		<td>CAP_SUPP_EAS</td>
		<td>0x00040000</td>
		<td>是否支持电子防拆功能</td>
	</tr>
</table>


<br/>
<p><h2><b>
附件3：修改记录
</b></h2></p>
<br/>

<table class="table table-bordered table-striped table-condensed">
	<tr>
		<th width="12%">修改日期</th>
		<th width="12%">修改人</th>
		<th width="70%">修改内容</th>
	</tr>
	<tr>
		<td>2015-8-10</td>
		<td>孟奥杰</td>
		<td>忘了</td>
	</tr>
	<tr>
		<td>2015-8-24</td>
		<td>孟奥杰</td>
		<td>针对 F10 机型添加了 PIR（红外硬件移动侦测） 
		    DoorBell（门铃控制）两种消息类型，
		    修改 StateLight 类型，添加呼吸灯控制
            PIR 消息类型的修改内容请详见 [4.1 消息主类型][4.2.F SubMessageType:PIR][5. 附件1——兼容老协议 MsgType]
            DoorBell 消息类型的修改内容针详见 [4.1 消息主类型][4.2.10 SubMessageType:DoorBell][5. 附件1——兼容老协议 MsgType]
            呼吸灯功能，修改的内容详见 [4.2.6 SubMessageType:State Light] 章节。
        </td>
	</tr>
	<tr>
		<td>2016-12-27</td>
		<td>马驰原</td>
		<td>在老协议的基础上，重新制作生成新协议，同步最新使用协议宏定义名、变量名，添加新支持的子类型及功能。</td>
	</tr>
</table>


