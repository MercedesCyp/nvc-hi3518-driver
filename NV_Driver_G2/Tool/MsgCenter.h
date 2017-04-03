#ifndef __MsgCenter_H
#define __MsgCenter_H

#include "../GlobalParameter.h"



// ================================================================================
// ----------------------------------------------------------------------> Definition
typedef struct{
	uint32 	 aUUID;                // 处理该驱动的用户 ID
	void 	*apTarMsg;             // 回应消息缓存池指针
	int32	*apTarLen;             // 当作为入参的为消息缓存池的大小
	                               // 当作为出参时为生成回应消息的大小
	void 	*apMsg;                // 待处理消息类型
	uint16 	  aLen;                // 消息长度，除去头之后的长度
	uint16 	  aCmd;                // 消息类型
	uint8	  aUnitNum;            // 操作设备序列号
}mRcvParam;



typedef struct{
	int32 	(*aProcessing)(mRcvParam*);       // 处理消息的模块入口
	uint16 	aID;                              // 消息模块类型
}mMsgModule;




typedef struct{
	int32	(*afInit)(uint32);
	int32	(*afUninit)(uint32);
	int32	(*afProcess)(uint32,void *,uint16);
}mClass_MsgCenter;





// ================================================================================

extern mClass_MsgCenter gClassMsgCent;

#endif
