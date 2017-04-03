#ifndef __MQUEUE_H
#define __MQUEUE_H
// =============================================================================
// INCLUDE
// C
// Linux
// local
#include "String.h"
// remote
#include "../GlobalParameter.h"
// #include <linux/signal.h>
// #include <asm/uaccess.h>
// #include <linux/semaphore.h>
// #include <linux/spinlock.h>

//=============================================================================
// DATA TYPE
// ----------> mMQUEUE_ProtectFun
typedef struct {
	void (*prfINIT_Queue)(void *piSpace, uint32 iSize);
	uint8 *(*prfACT_GetSpaceAddress)(void);
	int (*prfACT_ClrBuf)(void);
	int (*prfACT_AddSubUser)(uint32 iUID);
	int (*prfACT_DelSubUser)(uint32 iUID);
	int (*prfACT_AddSubMsg)(uint8 *piUserMsg, uint16 iMsgLen);
	int (*prfACT_AddRmdMsg)(uint32 iUID, uint8 *piUserMsg,
	                        uint16 iMsgLen);
	int (*prfACT_GetMsg)(uint32 iUID, uint8 *piUserMsg, uint16 iMsgLen);
	int (*prfIS_MsgExist)(uint32 iUID);
	int (*prfIS_UserExist)(uint32 iUID);
#define DC_MQIsUE_Ret_Sub	0x01
#define DC_MQIsUE_Ret_Rmd	0x02
} mMQUEUE_ProtectFun;
// ----------> mMQUEUE_Public
typedef struct {
	void (*infMsgWriteDoneCallBack)(void);
	void (*infMsgReadDoneCallBack)(void);
} mMQUEUE_Public;
// ----------> mMQUEUE_Class
typedef struct {
	const mMQUEUE_ProtectFun *cpProtect;
	mMQUEUE_Public *pPublic;
} mMQUEUE_Class;

//=============================================================================
// MACRO
// CONSTANT
// FUNCTION

//==============================================================================
//extern
extern mMQUEUE_Class goMQUEUE_Class;
//local
//global

//==============================================================================
//Others
//一下程序用于调试s
#define MQ_PK printk
void DbgPrinStr(unsigned char *iStr, unsigned char *iPD, unsigned short iLen);
void DbgShowTheManagerField(void);
#endif