#include "LinuxAPI.h"
#include "Tool/MsgCenter.h"


//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> Parameter type Definition                  //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
//////////////////////////////////////////////////////////////////////////////////
typedef struct{
	wait_queue_head_t aOutQueue;
	uint32 aUID;
}mUserInfo; 


//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> Local variable                             //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
//////////////////////////////////////////////////////////////////////////////////
#define DC_FUNMASTER_SleepQueueNum 10
static uint16 sModuleCounts = 0;
static wait_queue_head_t *sSleepQueue[DC_FUNMASTER_SleepQueueNum];
static uint8 sSleepQueueNum = 0;


//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> Location Function                          //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
// 	---------->	Linux Interface                                                 //
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		___onOpen                                                               //
//	                                                                            //
// 	Param:                                                                      //
// 		_pinode                                                                 //
//		_pfile                                                                  //
//                                                                              //
// 	Return:                                                                     //
// 		0		Success                                                         //
// 		-EFAULT	Register Driver error                                           //
//////////////////////////////////////////////////////////////////////////////////
static int ___onOpen (struct inode *_pinode, struct file *_pfile)
{
	mUserInfo *tDataUserSelf;
	tDataUserSelf = (mUserInfo*)kmalloc(sizeof(mUserInfo),GFP_KERNEL);
	if(tDataUserSelf == NULL)
		return -EFAULT;
	init_waitqueue_head(&(tDataUserSelf->aOutQueue));
	sModuleCounts++;
	tDataUserSelf->aUID = sModuleCounts;
	
	if( sSleepQueueNum > DC_FUNMASTER_SleepQueueNum ){
#if OPEN_DEBUG
	NVCPrint("The Driver is no longer support more user");
#endif			
		return -EFAULT;
	}
	printk("------>sSleepQueueNum: %d\r\n",(int)sSleepQueueNum );
	sSleepQueue[sSleepQueueNum++] = &(tDataUserSelf->aOutQueue);
	
#if OPEN_DEBUG
	NVCPrint("__Open UserID:%d", (int)tDataUserSelf->aUID);
#endif	
	_pfile->private_data = (void*)tDataUserSelf;
	return 0;//nonseekable_open(inode, filp);
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		___onClose                                                              //
//	                                                                            //
// 	Param:                                                                      //
// 		_pinode                                                                 //
//		_pfile                                                                  //
//                                                                              //
// 	Return:                                                                     //
// 		0	Success                                                             //
// 	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
static int ___onClose (struct inode *_pinode, struct file *_pfile)	
{
	uint8 _i;
	mUserInfo *tDataUserSelf;
	tDataUserSelf = (mUserInfo*)_pfile->private_data;
	
	for(_i=0;_i<sSleepQueueNum;_i++){
		if(sSleepQueue[_i]==&(tDataUserSelf->aOutQueue)){
			break;
		}
	}
	for(;_i<(sSleepQueueNum-1);_i++){
		sSleepQueue[_i] = sSleepQueue[_i+1];
	}
	sSleepQueueNum--;
	
#if OPEN_DEBUG
		NVCPrint("__Close UserID:%d", (int)tDataUserSelf->aUID);
#endif
 	if( goMQUEUE_Class.cpProtect->prfIS_UserExist(tDataUserSelf->aUID)&DC_MQIsUE_Ret_Sub )
	{
		goMQUEUE_Class.cpProtect->prfACT_DelSubUser(tDataUserSelf->aUID);

#if OPEN_DEBUG
		NVCPrint("Close User Subscribe!");
#endif
#if DEBUG_RWMSG
		DbgShowTheManagerField();
#endif
	}
	kfree(tDataUserSelf);
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		_poll                                                                   //
//	                                                                            //
// 	Param:                                                                      //
// 		_pfile                                                                  //
//		wait                                                                    //
//                                                                              //
// 	Return:                                                                     //
// 		0	Success                                                             //
// 	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
static unsigned int ___optPoll(struct file *_pfile, poll_table *_pWait){
	
	unsigned int theMask;
	mUserInfo *tDataUserSelf;
	
	// 获取用户 ID
	// 获取用户消息队列
	tDataUserSelf = (mUserInfo*)_pfile->private_data;	
	theMask = 0;
	
	poll_wait(_pfile, &(tDataUserSelf->aOutQueue), _pWait);
 	if( 1 == goMQUEUE_Class.cpProtect->prfIS_MsgExist(tDataUserSelf->aUID)){
		theMask = POLLIN | POLLRDNORM;
	}else{
		theMask = 0;
	}

	return theMask;
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		__read                                                                  //
//	                                                                            //
// 	Param:                                                                      //
// 		_pfile                                                                  //
//		piUser                                                                  //
//		iSize                                                                   //
//		piLoff                                                                  //
//                                                                              //
// 	Return:                                                                     //
// 		0		Success                                                         //
// 		EFAULT	                                                                //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
static int ___optRead(struct file *_pfile, char __user *piUser, size_t iSize,loff_t *piLoff){
	
	int theRet;
	mUserInfo *tDataUserSelf;
	uint8 tMsgBuf[256];
	DEFINE_WAIT(tWaitQ);
	
	tDataUserSelf = (mUserInfo*)_pfile->private_data;
	
 	do{
	if( 1 == goMQUEUE_Class.cpProtect->prfIS_MsgExist(tDataUserSelf->aUID)){
			break;
		}else{
			prepare_to_wait(&(tDataUserSelf->aOutQueue), &tWaitQ, TASK_INTERRUPTIBLE);
			if( 1 != goMQUEUE_Class.cpProtect->prfIS_MsgExist(tDataUserSelf->aUID))
				schedule();
			finish_wait(&(tDataUserSelf->aOutQueue), &tWaitQ);
		}
	}while(1);
	
	theRet = goMQUEUE_Class.cpProtect->prfACT_GetMsg(tDataUserSelf->aUID,tMsgBuf,256);
	if(theRet<=0){
		return -EFAULT;
	}else if(theRet>0){
		if(iSize<theRet){
			return -EFAULT;
		}else{
#if DEBUG_RWMSG
	DbgPrinStr("/*  READ   */\r\n",tMsgBuf,theRet  );
#endif
			if(copy_to_user(piUser,tMsgBuf,theRet)<0){
				return -EFAULT;
			}
		}
	}
	return theRet;
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		__write                                                                 //
//	                                                                            //
// 	Param:                                                                      //
// 		_pfile                                                                  //
//		piUser                                                                  //
//		iSize                                                                   //
//		piLoff                                                                  //
//                                                                              //
// 	Return:                                                                     //
// 		0		Success                                                         //
// 		EFAULT	                                                                //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
static int ___optWrite(struct file *_pfile,const char __user *piUser, size_t iSize,loff_t *piLoff){
	
	uint8 ptMsgBuf[128];
	mUserInfo *tDataUserSelf;
	
	//get msg from user
	tDataUserSelf = (mUserInfo*)_pfile->private_data;
	
	if(copy_from_user(ptMsgBuf,piUser,iSize))
		return -EFAULT;
#if DEBUG_RWMSG
	DbgPrinStr("/*  WRITE   */\r\n",ptMsgBuf,iSize  );
#endif
	
	if( gClassMsgCent.afProcess(tDataUserSelf->aUID,(void*)ptMsgBuf,iSize) )
		return -EFAULT; 
	
	return iSize;
}





//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> Location Function                          //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
// 	---------->	ServerFunction                                                  //
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		sfWakeUpAllLock                                                         //
//	                                                                            //
// 	Param:                                                                      //
// 		void                                                                    //
//                                                                              //
// 	Return:                                                                     //
// 		void		                                                            //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
static void sfWakeUpAllLock(void){
	
	uint8 _i; 
	for(_i=0;_i<sSleepQueueNum;_i++){
		wake_up_interruptible(sSleepQueue[_i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
struct file_operations NVDIV_fops = {                                           //
	.owner				= THIS_MODULE,                                          //
	.open				= ___onOpen,                                            //
	.release			= ___onClose,                                           //
	.read				= ___optRead,                                           //
	.write				= ___optWrite,                                          //
	.poll				= ___optPoll,                                           //
};                                                                              //
//                                                                              //
//                                                                              //
struct miscdevice NV_dev={                                                      //
	  .minor = DC_NVDIV_MINOR,                                                  //
	  .name  = DC_NVDRIV_NAME,                                                  //
	  .fops  = &NVDIV_fops                                                      //
};                                                                              //
//////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> Global Function                            //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		gfInit_NV_Driver                                                        //
//	                                                                            //
// 	Param:                                                                      //
// 		void                                                                    //
//                                                                              //
// 	Return:                                                                     //
// 		0		Success                                                         //
// 		-1		error                                                           //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
void gfUNINIT_NV_LoadAndConfig(void);
extern int32 gfProBltyInit__(void);
extern int32 gfProBltyUninit__(void);
int gfINIT_NV_LoadAndConfig(void){
	
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
		tMallocPage = (uint8*)get_zeroed_page(GFP_KERNEL);
		if(tMallocPage == NULL){
#if OPEN_DEBUG
	NVCPrint("Request message space failed!");
#endif
		goto GT_NV_LoadAndConfigErr;
		}
		// The following two code generally won't appear mistake,
		// if happened, and is likely to be memory pages application is not successful. 
		goMQUEUE_Class.cpProtect->prfINIT_Queue(tMallocPage,4096);
		goMQUEUE_Class.pPublic->infMsgWriteDoneCallBack = sfWakeUpAllLock;
	}
	
	sModuleCounts = 0;
	sSleepQueueNum = 0;	
	
	if( gfProBltyInit__() ){
		goto GT_NV_LoadAndConfigErr;
	}
	
	if( gClassMsgCent.afInit(0) ){
		goto GT_NV_LoadAndConfigErr;
	}
	
	misc_register(&NV_dev);
	return 0;
GT_NV_LoadAndConfigErr:
	gfUNINIT_NV_LoadAndConfig();
	return -1;
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		gfUninit_NV_Driver                                                      //
//	                                                                            //
// 	Param:                                                                      //
// 		void                                                                    //
//                                                                              //
// 	Return:                                                                     //
// 		void                                                                    //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
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
		tMallocPage = goMQUEUE_Class.cpProtect->prfACT_GetSpaceAddress(); 
		if( tMallocPage != NULL )
			free_page((uint32)tMallocPage);
	}
	
	misc_deregister(&NV_dev);
}
