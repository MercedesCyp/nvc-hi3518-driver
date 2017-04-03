/*
Coder:      aojie.meng
Date:       2015-8-15

Abstract:
    提供驱动在挂载之后，应用层与驱动交互的 linux 内核回调接口，主要有 open,close，
    write,read,poll
*/
//==============================================================================
// C
// Linux
// local
#include "LinuxAPI.h"
// remote
#include "Tool/MsgCenter.h"

//=============================================================================
// DATA TYPE
typedef struct {
	wait_queue_head_t aOutQueue;
	uint32 aUID;
} mUserInfo;

//=============================================================================
// MACRO
// CONSTANT
#define DC_FUNMASTER_SleepQueueNum 10
// FUNCTION

//==============================================================================
//extern
extern int32 gfProBltyInit__(void);
extern int32 gfProBltyUninit__(void);
//local
static int ___onOpen(struct inode *_pinode, struct file *_pfile);
static int ___onClose(struct inode *_pinode, struct file *_pfile);
static unsigned int ___optPoll(struct file *_pfile, poll_table *_pWait);
static int ___optRead(struct file *_pfile, char __user *piUser, size_t iSize,
                      loff_t *piLoff);
static int ___optWrite(struct file *_pfile, const char __user *piUser,
                       size_t iSize, loff_t *piLoff);
static void sfWakeUpAllLock(void);
//global
int gfINIT_NV_LoadAndConfig(void);
void gfUNINIT_NV_LoadAndConfig(void);

//==============================================================================
//extern
//local
static uint16 sModuleCounts = 0;
static wait_queue_head_t *sSleepQueue[DC_FUNMASTER_SleepQueueNum];
static uint8 sSleepQueueNum = 0;
//global
struct file_operations NVDIV_fops = {
	.owner = THIS_MODULE,
	.open = ___onOpen,
	.release = ___onClose,
	.read = ___optRead,
	.write = ___optWrite,
	.poll = ___optPoll,
};

struct miscdevice NV_dev = {
	.minor = DC_NVDIV_MINOR,
	.name = DC_NVDRIV_NAME,
	.fops = &NVDIV_fops
};

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int gfINIT_NV_LoadAndConfig(void)
@introduction:
    该方法的执行排在检查了装载驱动的合法性之后，目的是初始化驱动运行环境，包括数
    据缓冲区，各个方法模块和消息处理模块的初始化，等等

@parameter:
    void

@return:
    0		Success
    -1      failed

*/
int gfINIT_NV_LoadAndConfig(void)
{

	// 初始化被重写的函数
	// 初始化各个部分功能
	// 向系统注册接口
	// 初始化解锁回调机制
	// 申请消息管理内存空间
	// 初始化消息管理内存机制
	// 加在驱动配置信息
	// 初始化整个驱动

	{
		uint8 *tMallocPage;
		tMallocPage = (uint8 *) get_zeroed_page(GFP_KERNEL);
		if (tMallocPage == NULL) {
#if DEBUG_LAPI
			NVCPrint("Request message space failed!");
#endif
			goto GT_NV_LoadAndConfigErr;
		}
		// The following two code generally won't appear mistake,
		// if happened, and is likely to be memory pages application is not successful.
		goMQUEUE_Class.cpProtect->prfINIT_Queue(tMallocPage, 4096);
		goMQUEUE_Class.pPublic->infMsgWriteDoneCallBack =
		    sfWakeUpAllLock;
	}

	sModuleCounts = 0;
	sSleepQueueNum = 0;

	if (gfProBltyInit__()) {
		goto GT_NV_LoadAndConfigErr;
	}

	if (gClassMsgCent.afInit(0)) {
		goto GT_NV_LoadAndConfigErr;
	}

	misc_register(&NV_dev);
	return 0;
GT_NV_LoadAndConfigErr:
	gfUNINIT_NV_LoadAndConfig();
	return -1;
}

//---------- ---------- ---------- ----------
/*  void gfUNINIT_NV_LoadAndConfig(void)
@introduction:
    释放资源，释放缓冲区

@parameter:
    void

@return:
    void

*/
void gfUNINIT_NV_LoadAndConfig(void)
{
	// 停止，注销，各个功能模块
	// 结束消息管理器的工作
	// 释放消息管理器内存
	// 通知系统注销此驱动
	// gClassMsgCent.afUninit(0x00);
	gfProBltyUninit__();

	gClassMsgCent.afUninit(0);
	sSleepQueueNum = 0;
	sModuleCounts = 0;

	{
		uint8 *tMallocPage;
		tMallocPage =
		    goMQUEUE_Class.cpProtect->prfACT_GetSpaceAddress();
		if (tMallocPage != NULL) {
			free_page((uint32) tMallocPage);
		}
	}

	misc_deregister(&NV_dev);
}

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int ___onOpen (struct inode *_pinode, struct file *_pfile)
@introduction:
    给 open 该驱动的用户分配 ID ，并为其初始化资源锁，创建等待队列。

@parameter:
    _pinode
    _pfile

@return:
    0		Success
    -EFAULT	Register Driver error

*/
static int ___onOpen(struct inode *_pinode, struct file *_pfile)
{
	mUserInfo *tDataUserSelf;
	tDataUserSelf = (mUserInfo *) kmalloc(sizeof(mUserInfo), GFP_KERNEL);
	if (tDataUserSelf == NULL) {
		return -EFAULT;
	}
	init_waitqueue_head(&(tDataUserSelf->aOutQueue));
	sModuleCounts++;
	tDataUserSelf->aUID = sModuleCounts;

	if (sSleepQueueNum > DC_FUNMASTER_SleepQueueNum) {
#if DEBUG_LAPI
		NVCPrint("The Driver is no longer support more user");
#endif
		return -EFAULT;
	}
	// printk("------>sSleepQueueNum: %d\r\n",(int)sSleepQueueNum );
	sSleepQueue[sSleepQueueNum++] = &(tDataUserSelf->aOutQueue);

	_pfile->private_data = (void *)tDataUserSelf;

#if DEBUG_RWMSG
	DbgShowTheManagerField();
#endif
#if DEBUG_LAPI
	NVCPrint("__Open UserID:%d", (int)tDataUserSelf->aUID);
#endif
	return 0;		//nonseekable_open(inode, filp);
}

//---------- ---------- ---------- ----------
/*  static int ___onClose (struct inode *_pinode, struct file *_pfile)
@introduction:
    释放资源,主要是针对残留在消息缓冲区里的数据，防止内存溢出，减小 CPU 分析内存
    时的负担。

@parameter:
    _pinode
    _pfile

@return:
    0		Success

*/
static int ___onClose(struct inode *_pinode, struct file *_pfile)
{
	uint8 _i;
	mUserInfo *tDataUserSelf;
	tDataUserSelf = (mUserInfo *) _pfile->private_data;

	for (_i = 0; _i < sSleepQueueNum; _i++) {
		if (sSleepQueue[_i] == &(tDataUserSelf->aOutQueue)) {
			break;
		}
	}
	for (; _i < (sSleepQueueNum - 1); _i++) {
		sSleepQueue[_i] = sSleepQueue[_i + 1];
	}
	sSleepQueueNum--;

	if (goMQUEUE_Class.
	    cpProtect->prfIS_UserExist(tDataUserSelf->
	                               aUID) & DC_MQIsUE_Ret_Sub) {
		goMQUEUE_Class.cpProtect->
		prfACT_DelSubUser(tDataUserSelf->aUID);

#if DEBUG_LAPI
		NVCPrint("Close User Subscribe!");
#endif
#if DEBUG_RWMSG
		DbgShowTheManagerField();
#endif
	}
	kfree(tDataUserSelf);

#if DEBUG_LAPI
	NVCPrint("__Close UserID:%d", (int)tDataUserSelf->aUID);
#endif
	return 0;
}

//---------- ---------- ---------- ----------
/*  static unsigned int ___optPoll(struct file *_pfile, poll_table *_pWait)
@introduction:
    提供读检测，不提供写检测

@parameter:
    _pinode
    wait

@return:
    0		不可读不可写
    POLLIN
    POLLRDNORM
            以上两者同时出现标示有数据存在数据缓冲区，可度

*/
static unsigned int ___optPoll(struct file *_pfile, poll_table *_pWait)
{

	unsigned int theMask;
	mUserInfo *tDataUserSelf;
	// 获取用户 ID
	// 获取用户消息队列
	tDataUserSelf = (mUserInfo *) _pfile->private_data;
	theMask = 0;

	poll_wait(_pfile, &(tDataUserSelf->aOutQueue), _pWait);
	if (1 == goMQUEUE_Class.cpProtect->prfIS_MsgExist(tDataUserSelf->aUID)) {
		theMask = POLLIN | POLLRDNORM;
	} else {
		theMask = 0;
	}

	return theMask;
}

//---------- ---------- ---------- ----------
/*  static int ___optRead(struct file *_pfile, char __user *piUser, size_t iSize,loff_t *piLoff)
@introduction:
    检查用户是否可读，不可读，阻塞，可读，分析地址空间合法性，读取数据，返回读取到得字节

@parameter:
    _pfile
    piUser
    iSize
    piLoff

@return:
    -EFAULT     读写错误
    0           一般不会出现，除非出bug了
    >0          返回读到的字节数

*/
static int ___optRead(struct file *_pfile, char __user *piUser, size_t iSize,
                      loff_t *piLoff)
{

	int theRet;
	mUserInfo *tDataUserSelf;
	uint8 tMsgBuf[256];
	DEFINE_WAIT(tWaitQ);

	tDataUserSelf = (mUserInfo *) _pfile->private_data;

	do {
		if (1 ==
		    goMQUEUE_Class.cpProtect->
		    prfIS_MsgExist(tDataUserSelf->aUID)) {
			break;
		} else {
			prepare_to_wait(&(tDataUserSelf->aOutQueue), &tWaitQ,
			                TASK_INTERRUPTIBLE);
			if (1 !=
			    goMQUEUE_Class.
			    cpProtect->prfIS_MsgExist(tDataUserSelf->aUID)) {
				schedule();
			}
			finish_wait(&(tDataUserSelf->aOutQueue), &tWaitQ);
		}
	} while (1);

	theRet =
	    goMQUEUE_Class.cpProtect->prfACT_GetMsg(tDataUserSelf->aUID,
	            tMsgBuf, 256);
	if (theRet <= 0) {
		return -EFAULT;
	} else if (theRet > 0) {
		if (iSize < theRet) {
			return -EFAULT;
		} else {
#if DEBUG_RWMSG
			DbgPrinStr("/*  READ   */\r\n", tMsgBuf, theRet);
#endif
			if (copy_to_user(piUser, tMsgBuf, theRet) < 0) {
				return -EFAULT;
			}
		}
	}
	return theRet;
}

//---------- ---------- ---------- ----------
/*  static int ___optWrite(struct file *_pfile,const char __user *piUser, size_t iSize,loff_t *piLoff)
@introduction:
    检查合法性，获取应用层数据，交由消息处理单元的接口处理

@parameter:
    _pfile
    piUser
    iSize
    piLoff

@return:
    -EFAULT     读写错误
    0           一般不会出现，除非出bug了
    >0          返回写入的字节数

*/
static int ___optWrite(struct file *_pfile, const char __user *piUser,
                       size_t iSize, loff_t *piLoff)
{

	uint8 ptMsgBuf[512];
	mUserInfo *tDataUserSelf;

	//get msg from user
	tDataUserSelf = (mUserInfo *) _pfile->private_data;

	if (copy_from_user(ptMsgBuf, piUser, iSize)) {
#if DEBUG_RWMSG
		printk("__optWrite: copy_from_user failed!\n");
#endif
		return -EFAULT;
	}
#if DEBUG_RWMSG
	DbgPrinStr("/*  WRITE   */\r\n", ptMsgBuf, iSize);
#endif

	if (gClassMsgCent.afProcess
	    (tDataUserSelf->aUID, (void *)ptMsgBuf, iSize)) {
#if DEBUG_RWMSG
		printk("__optWrite: process failed!\n");
#endif
		return -EFAULT;
	}

	return iSize;
}

//---------- ---------- ---------- ----------
/*  static void sfWakeUpAllLock(void)
@introduction:
    恢复队列里所有线程

@parameter:
    void

@return:
    void

*/
static void sfWakeUpAllLock(void)
{
	uint8 _i;
	for (_i = 0; _i < sSleepQueueNum; _i++) {
		wake_up_interruptible(sSleepQueue[_i]);
	}
}

//==============================================================================
//Others
