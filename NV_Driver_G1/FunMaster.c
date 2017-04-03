#include "FunMaster.h"


//  -------------------------------> Local Definition
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
typedef struct{
	//struct cdev *i_cdev;
	wait_queue_head_t aOutQueue;
	uint32 aUID;
}mUserInfo;



//  -------------------------------> Local Function Definition
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
void sfFunMaster_WakeUpTheLock(void);



//  -------------------------------> Local variable
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
#define DC_FUNMASTER_SleepQueueNum 10
static uint16 sModuleCounts = 0;
static wait_queue_head_t *sSleepQueue[DC_FUNMASTER_SleepQueueNum];
static uint8 sSleepQueueNum;

//  -------------------------------> Location Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--

//--------------------------------------------------------------->
// 	Function NAME:
// 		__open
//	
// 	Param:
// 		_pinode
//		_pfile
//
// 	Return:
// 		0		Success
// 		-EFAULT	Register Driver error
// 	
//--------------------------------------------------------------->
static int __open (struct inode *_pinode, struct file *_pfile)
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
	sSleepQueue[sSleepQueueNum++] = &(tDataUserSelf->aOutQueue);
	
#if OPEN_DEBUG
	NVCPrint("__Open UserID:%d\r\n", (int)tDataUserSelf->aUID);
#endif	
	_pfile->private_data = (void*)tDataUserSelf;
	return 0;//nonseekable_open(inode, filp);
}
//--------------------------------------------------------------->
// 	Function NAME:
// 		__close
//	
// 	Param:
// 		_pinode
//		_pfile
//
// 	Return:
// 		0	Success
// 	
//--------------------------------------------------------------->
static int __close (struct inode *_pinode, struct file *_pfile)	
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
	

	if(gfIsUserExist(tDataUserSelf->aUID)&DC_MQIsUE_Ret_Sub){
#if OPEN_DEBUG
	NVCPrint("__Close UserID:%d\r\n", (int)tDataUserSelf->aUID);
#endif	
		gfMsgQueue_delSubUser(tDataUserSelf->aUID);
	}
	kfree(tDataUserSelf);
	
	return 0;
}


//--------------------------------------------------------------->
// 	Function NAME:
// 		_poll
//	
// 	Param:
// 		_pfile
//		wait
//
// 	Return:
// 		0	Success
// 	
//--------------------------------------------------------------->
static unsigned int _poll(struct file *_pfile, poll_table *_pWait){
	
	unsigned int theMask;
	mUserInfo *tDataUserSelf;
	
	tDataUserSelf = (mUserInfo*)_pfile->private_data;
	
	theMask = 0;

	poll_wait(_pfile, &(tDataUserSelf->aOutQueue), _pWait);

	if( 1 == gfIsSomeMsgExist(tDataUserSelf->aUID)){
		theMask = POLLIN | POLLRDNORM;
	}else{
		theMask = 0;
	}

	return theMask;
}



//--------------------------------------------------------------->
// 	Function NAME:
// 		__read
//	
// 	Param:
// 		_pfile
//		piUser
//		iSize
//		piLoff
//
// 	Return:
// 		0		Success
// 		EFAULT	
//	
//--------------------------------------------------------------->
static int __read(struct file *_pfile, char __user *piUser, size_t iSize,loff_t *piLoff){
	
	int theRet;
	mUserInfo *tDataUserSelf;
	uint8 tMsgBuf[256];
	DEFINE_WAIT(tWaitQ);
	
	tDataUserSelf = (mUserInfo*)_pfile->private_data;
	
	do{
	if( 1 == gfIsSomeMsgExist(tDataUserSelf->aUID)){
			break;
		}else{
			prepare_to_wait(&(tDataUserSelf->aOutQueue), &tWaitQ, TASK_INTERRUPTIBLE);
			if( 1 != gfIsSomeMsgExist(tDataUserSelf->aUID))
				schedule();
			finish_wait(&(tDataUserSelf->aOutQueue), &tWaitQ);
		}
	}while(1);
	
	theRet = gfGetAUserMsg(tDataUserSelf->aUID,tMsgBuf,256);
#if OPEN_DEBUG
	// NVCPrint_h("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*\r\n");
	// NVCPrint_h("gfGetAUserMsg ret : %d\r\n",theRet);
#endif
	if(theRet<=0){
		return -EFAULT;
	}else if(theRet>0){
		if(iSize<theRet){
			return -EFAULT;
		}else{
			if(copy_to_user(piUser,tMsgBuf,theRet)<0){
				return -EFAULT;
			}
#if OPEN_DEBUG
	// DbgPrinStr("Send: ",tMsgBuf,theRet);
	// Print_h("\r\n");
#endif
			
		}
	}
	return theRet;
}


//--------------------------------------------------------------->
// 	Function NAME:
// 		__write
//	
// 	Param:
// 		_pfile
//		piUser
//		iSize
//		piLoff
//
// 	Return:
// 		0		Success
// 		EFAULT	
//	
//--------------------------------------------------------------->
static int __write(struct file *_pfile,const char __user *piUser, size_t iSize,loff_t *piLoff){
	
	uint8 ptMsgBuf[128];
	mUserInfo *tDataUserSelf;
	
	//get msg from user
	tDataUserSelf = (mUserInfo*)_pfile->private_data;
	
	if(copy_from_user(ptMsgBuf,piUser,iSize))
		return -EFAULT;
	
#if OPEN_DEBUG
	// NVCPrint_h("$_$_$_$_$_$_$_$_$_$_$_$_$_$_$_$_$_\r\n");
	// DbgPrinStr("Rcv: ",ptMsgBuf,iSize);
	// NVCPrint_h("\r\n");
#endif
	
	if(gfMsgCenter_Processing(tDataUserSelf->aUID,(void*)ptMsgBuf,iSize))
		return -EFAULT;
	
	return iSize;
}



//  -------------------------------> Location definition
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
struct file_operations NVDIV_fops = {
	.owner				= THIS_MODULE,
	.open				= __open,
	.release			= __close,
	.read				= __read,
	.write				= __write,
	.poll				= _poll,
};


struct miscdevice NV_dev={
	  .minor = DC_NVDIV_MINOR,
	  .name  = DC_NVDRIV_NAME,
	  .fops  = &NVDIV_fops
};


//  -------------------------------> Global Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
//--------------------------------------------------------------->
// 	Function NAME:
// 		gfInit_NV_Driver
//	
// 	Param:
// 		void
//
// 	Return:
// 		0		Success
// 		-1		error
//	
//--------------------------------------------------------------->
int gfInit_NV_Driver(void){
	
	uint8 *tMallocPage;
	
	sModuleCounts = 0;
	sSleepQueueNum = 0;
	
	tMallocPage = (uint8*)get_zeroed_page(GFP_KERNEL);
	if(tMallocPage == NULL){
#if OPEN_DEBUG
	NVCPrint("Request message space failed!\r\n");
#endif
		return -1;
	}
	gfInitMsgQueue(tMallocPage,4096);
	gfMqueue_AddUserMsgCallBack = sfFunMaster_WakeUpTheLock;
	
	if(gfInitAppointPro()){
#if OPEN_DEBUG
	NVCPrint("It was fail to initialized hardware!\r\n");
#endif
		free_page((uint32)tMallocPage);
		return -1;
	}
	
	misc_register(&NV_dev); 
	
	return 0;
}

//--------------------------------------------------------------->
// 	Function NAME:
// 		gfUninit_NV_Driver
//	
// 	Param:
// 		void
//
// 	Return:
// 		void
//	
//--------------------------------------------------------------->
void gfUninit_NV_Driver(void)
{
 	uint8 *tMallocPage;
	
	sSleepQueueNum = 0;
	sModuleCounts = 0;
	
	gfUninitAppointPro();
	
	tMallocPage = gfGetMsgBlockAddress();
	free_page((uint32)tMallocPage);
	
	misc_deregister(&NV_dev); 
}


void sfFunMaster_WakeUpTheLock(void){
	
	uint8 _i; 
	for(_i=0;_i<sSleepQueueNum;_i++){
		wake_up_interruptible(sSleepQueue[_i]);
	}
}
