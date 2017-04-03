#ifndef __MQUEUE_H
#define __MQUEUE_H

#include "GlobalParameter.h"
#include "String.h"
// #include <linux/signal.h>
// #include <asm/uaccess.h>
// #include <linux/semaphore.h>
// #include <linux/spinlock.h>



#ifndef NULL
#define NULL 0
#endif


//#define DC_MQOwner_PublicID		0xFFFFFFFF


extern void (*gfMqueue_AddUserMsgCallBack)(void);

extern void gfInitMsgQueue(void *piSpace,uint32 iSize);

extern int gfMsgQueue_addSubUser(uint32 iUID);
extern int gfMsgQueue_delSubUser(uint32 iUID);

extern int gfClearMsgBuf(void);
extern int gfIsSomeMsgExist(uint32 iUID);

#define DC_MQIsUE_Ret_Sub	0x01
#define DC_MQIsUE_Ret_Rmd	0x02
extern int gfIsUserExist(uint32 iUID);

extern int gfAddAUserMsgForSub(uint8 *piUserMsg, uint16 iMsgLen);
extern int gfAddAUserMsgForRmd(uint32 iUID, uint8 *piUserMsg, uint16 iMsgLen);

extern int gfGetAUserMsg(uint32 iUID,uint8 *piUserMsg,uint16 iMsgLen);
extern uint8 *gfGetMsgBlockAddress(void);


//一下程序用于调试
//#include <stdio.h>
#define MQ_PK printk
void DbgPrinStr(unsigned char *iStr,unsigned char *iPD,unsigned short iLen);
void DbgShowTheManagerField(void);
#endif

