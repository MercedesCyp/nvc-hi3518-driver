#include "Mqueue.h"

// the manage body size 128Bety
// 2Bety UserNum // 6*6Bety=36Bety User // 2Bety Sum MsgNum //
// 4Bety QueueBegin	// 4Bety QueueEnd // 2Bety Idle Space size //
//#define DC_MQ_DataFromatChar	0x00
#define DC_MQDFC				0x00
#define DC_MQSpace_ManageSize	128
#define DC_MQSpace_MsgHeadSize	(sizeof(mUMsgBody))
#define DC_MQSpace_SubMaxUser	4
#define DC_MQSpace_RmdMaxUser	7
#define DC_MQSpace_MaxMsgSize	(256-12)

// 
// 
#define DC_MQSpace_SubAddrBegin	(psDataStart+DC_MQSpace_ManageSize)
#define DC_MQSpace_SubAddrEnd	(psDataStart+DC_MQSpace_ManageSize+DC_MQSpace_SubDataSize-1)
#define DC_MQSpace_SubDataSize	(2048)
// size space size - 1536
#define DC_MQSpace_RmdAddrBegin	(psDataStart+DC_MQSpace_ManageSize+DC_MQSpace_SubDataSize)
#define DC_MQSpace_RmdAddrEnd	(psDataEnd)
#define DC_MQSpace_RmdDataSize	(sDataSize-DC_MQSpace_ManageSize-DC_MQSpace_SubDataSize)

// Subber difinition
#define DC_MQSpace_OFST_SubIdleBegin			0	//4		Bety
#define DC_MQSpace_OFST_SubIdleEnd				4	//4		Bety
#define DC_MQSpace_OFST_SubIdleSize				8	//4		Bety
#define DC_MQSpace_OFST_Sub1stNode				12	//4		Bety
#define DC_MQSpace_OFST_SubUserInfo				16	//32	Bety
#define DC_MQSpace_OFST_SubUserNum				48	//4		Bety
// Random difinition 
#define DC_MQSpace_OFST_RmdIdleBegin			52	//4		Bety
#define DC_MQSpace_OFST_RmdIdleEnd				56	//4		Bety
#define DC_MQSpace_OFST_RmdIdleSize				60	//4		Bety
#define DC_MQSpace_OFST_Rmd1stNode				64	//4		Bety
#define DC_MQSpace_OFST_RmdUserInfo				68	//56	Bety
#define DC_MQSpace_OFST_RmdUserNum				124	//1		Bety
// Sub/Rmd user num

typedef struct{
	uint32 aUID;
	void* aUMsg;
}mMQUserMsg;
typedef struct {
	void 		*paNext;	// void actuallypoint to itself
	uint16 	    aLen;
	union{
		uint16		aUserMask;
		uint16		aUserNum;
	};
}mUMsgBody;
typedef struct{
	void 		**aSubIdleBegin	;
	void 		**aSubIdleEnd	;
	uint32 		*aSubIdleSize	;
	mUMsgBody 	**aSub1stNode	;
	mMQUserMsg 	*aSubUserInfo	;
	uint8		*aSubUserNum	;
	
	void 		**aRmdIdleBegin	;
	void 		**aRmdIdleEnd	;
	uint32 		*aRmdIdleSize	;
	mUMsgBody 	**aRmd1stNode	;
	mMQUserMsg 	*aRmdUserInfo	;
	uint8		*aRmdUserNum	;
}mManageHeader;






static void sfMQSpace_GetManageHeader(void *piSpace,mManageHeader *ipData);

// command U User S Subber R Random H Header N Node
//#define DC_MQSMU_Cmd_GetSubNum			0x00000001	//	f 获取订阅用户数量 
#define DC_MQSMU_Cmd_SetSubUser			0x00000002	//  f 注册订阅用户
#define DC_MQSMU_Cmd_IsSubUserExist		0x00000004 	//  f 判断用户是否存在
// #define DC_MQSMU_Cmd_ClrSubUser			0x00000008	//  f 删除订阅用户
// #define DC_MQSMU_Cmd_SetSubUserHNode	0x00000010	//   设置消息节点信息
// #define DC_MQSMU_Cmd_ClrSubUserHNode	0x00000020	//   清除消息节点信息
#define DC_MQSMU_Cmd_GetSubUserHNode	0x00000040	//   获取消息节点信息
#define DC_MQSMU_Cmd_IsSubHNodeExist	0x00000080	//   获取消息节点信息

//#define DC_MQSMU_Cmd_GetRmdNum			0x00010000	//  f 获取自由用户数量 
#define DC_MQSMU_Cmd_SetRmdUser			0x00020000	//  f 注册自由用户
#define DC_MQSMU_Cmd_IsRmdUserExist		0x00040000 	//  f 判断用户是否存在
// #define DC_MQSMU_Cmd_ClrRmdUser			0x00080000	//  f 删除订阅用户
#define DC_MQSMU_Cmd_SetRmdUserHNode	0x00100000	//   设置消息节点信息
// #define DC_MQSMU_Cmd_ClrRmdUserHNode	0x00200000	//   清除消息节点信息 
#define DC_MQSMU_Cmd_GetRmdUserHNode	0x00400000	//   获取消息节点信息 
#define DC_MQSMU_Cmd_IsRmdHNodeExist	0x00800000	//   获取消息节点信息
// return return>0 返回用户数
#define DC_MQSMU_Success				0
#define DC_MQSMU_Err_InputParamErr		-1
#define DC_MQSMU_Err_UserNotExist		-2
#define DC_MQSMU_Err_UserSpaceFull		-3
#define DC_MQSMU_Err_UserExist			-4
#define DC_MQSMU_Err_UserSpaceEmpty		-5
#define DC_MQSMU_Err_UNNotExist			-6
static int32 sfMQSpace_ManageUser(uint32 iCmd, uint32 iUUID, void **UMsg);

#define DC_MQSCSU_ClrSub 0x01
#define DC_MQSCSU_ClrRmd 0x02
static int32 sfClearStubbornUser(uint8 iCmd);



void (*gfMqueue_AddUserMsgCallBack)(void) = NULL;
void (*gfMqueue_UserFire)(uint32) = NULL;
static void *psDataStart=NULL;
static void *psQueueStart=NULL;
static void *psDataEnd=NULL;
static uint32 sDataSize=0;
static mManageHeader ssManageHeader;

mMQUEUE_Public soMQUEUE_Public = {
	NULL,
	NULL,
};

//static struct semaphore sDataSpaceSem;              /* mutual exclusion semaphore */

//return
//  >0		ReadDate
//	0		suuccess
//	-1		No message
//	-2		user space not enough


static int sfGetAUserMsg(uint32 iUID,uint8 *piUserMsg,uint16 iMsgLen){
	

	uint8 _i;
	int thRectLen = 0;
	
	for(_i=0;_i<DC_MQSpace_RmdMaxUser;_i++)
		if( ssManageHeader.aRmdUserInfo[_i].aUID == iUID )
			break;
	if( _i != DC_MQSpace_RmdMaxUser ){
		// get objective message4
		mUMsgBody *tNodeTools = (mUMsgBody*)ssManageHeader.aRmdUserInfo[_i].aUMsg;
		if( tNodeTools != NULL ){
			if( tNodeTools->aLen > iMsgLen ){
				return -2;
			}else{
				gClassStr.afCopy( piUserMsg, ((uint8*)tNodeTools + sizeof(mUMsgBody)), tNodeTools->aLen );
				thRectLen = tNodeTools->aLen;
			}
			ssManageHeader.aRmdUserInfo[_i].aUMsg = tNodeTools->paNext;
			
			if( *ssManageHeader.aRmd1stNode == (void*)tNodeTools ){
				void *tTools;
				void *t1stNode = DC_MQSpace_RmdAddrEnd + DC_MQSpace_RmdDataSize;
				void *tEndNode = NULL;
				for( _i = 0; _i<DC_MQSpace_RmdMaxUser; _i++ ){
					if( (ssManageHeader.aRmdUserInfo[_i].aUID != 0 )&&\
						(ssManageHeader.aRmdUserInfo[_i].aUMsg != NULL )){
						tTools = (void*)ssManageHeader.aRmdUserInfo[_i].aUMsg;
						
						if( tTools > *ssManageHeader.aRmdIdleEnd ){
							if( tTools < t1stNode){
								t1stNode = tTools;
								tEndNode = tTools;
							}
						}else{	
							if( (tTools+DC_MQSpace_RmdDataSize) < t1stNode){
								t1stNode = tTools+DC_MQSpace_RmdDataSize;
								tEndNode = tTools;
							}
						}
					}
				}
				if( tEndNode == NULL ){
					// The subber date buffer is empty
					*ssManageHeader.aRmdIdleBegin 	= DC_MQSpace_RmdAddrBegin;
					*ssManageHeader.aRmdIdleEnd 	= DC_MQSpace_RmdAddrEnd;
					*ssManageHeader.aRmdIdleSize 	= DC_MQSpace_RmdDataSize;
					*ssManageHeader.aRmd1stNode 	= NULL;
					
				}else{				
					// when the space is not emtpy
					// we should fund the header node
					// because the space was not only one users
					// This user next date node doesnt meanning is the header node.
					*ssManageHeader.aRmd1stNode = tEndNode;
					*ssManageHeader.aRmdIdleEnd = tEndNode-1;
					if( *ssManageHeader.aRmdIdleEnd < DC_MQSpace_RmdAddrBegin )
						*ssManageHeader.aRmdIdleEnd = DC_MQSpace_RmdAddrEnd;
					if( *ssManageHeader.aRmdIdleBegin > *ssManageHeader.aRmdIdleEnd ){
						*ssManageHeader.aRmdIdleSize = \
							(void*)(*ssManageHeader.aRmdIdleEnd) +\
							DC_MQSpace_RmdDataSize -\
							(void*)(*ssManageHeader.aRmdIdleBegin);
					}else{
						*ssManageHeader.aRmdIdleSize = \
							1 + *ssManageHeader.aRmdIdleEnd - *ssManageHeader.aRmdIdleBegin;
					}
				}
			}
			return thRectLen;
		}
	}
	for( _i=0;_i<DC_MQSpace_SubMaxUser;_i++ )
		if( ssManageHeader.aSubUserInfo[_i].aUID == iUID )
			break;
	if( _i != DC_MQSpace_SubMaxUser ){
		// get report message
		mUMsgBody *tNodeTools = (mUMsgBody*)ssManageHeader.aSubUserInfo[_i].aUMsg;
		if( tNodeTools != NULL ){
			if( tNodeTools->aLen > iMsgLen ){
				return -2;
			}else{
				gClassStr.afCopy( piUserMsg, ((uint8*)tNodeTools + sizeof(mUMsgBody)), tNodeTools->aLen );
				thRectLen = tNodeTools->aLen;
			}
			tNodeTools->aUserMask &= ~(0x01<<_i);
			ssManageHeader.aSubUserInfo[_i].aUMsg = tNodeTools->paNext;
			
			if( *ssManageHeader.aSub1stNode == tNodeTools ){
				if( tNodeTools->aUserMask == 0){
					*ssManageHeader.aSub1stNode = tNodeTools->paNext;
					if( *ssManageHeader.aSub1stNode == NULL ){
						*ssManageHeader.aSubIdleBegin = DC_MQSpace_SubAddrBegin;
						*ssManageHeader.aSubIdleEnd = DC_MQSpace_SubAddrEnd;
						*ssManageHeader.aSubIdleSize = DC_MQSpace_SubDataSize;
					}else{
						*ssManageHeader.aSubIdleEnd = tNodeTools->paNext-1;
						if( *ssManageHeader.aSubIdleEnd < DC_MQSpace_SubAddrBegin )
							*ssManageHeader.aSubIdleEnd = DC_MQSpace_SubAddrEnd;
						if( *ssManageHeader.aSubIdleBegin > *ssManageHeader.aSubIdleEnd ){
							*ssManageHeader.aSubIdleSize = \
								(void*)(*ssManageHeader.aSubIdleEnd) +\
								DC_MQSpace_SubDataSize -\
								(void*)(*ssManageHeader.aSubIdleBegin);
						}else{
							*ssManageHeader.aSubIdleSize = \
								1 + *ssManageHeader.aSubIdleEnd - *ssManageHeader.aSubIdleBegin;
						}

					}
				}
			}
			return thRectLen;
		}
	}
		return 0;
	
}

//////////////////////////////////////////////////////////////////////////////////
//	parameter:                                                                  //
//		piSpace	begin of the data buffer start                                  //
//		iSize	this  data space size                                           //
//	return:                                                                     //
//		void                                                                    //
//////////////////////////////////////////////////////////////////////////////////
static void sfInitMsgQueue(void *piSpace,uint32 iSize){
	
	psDataStart		= piSpace;
	psQueueStart	= piSpace + DC_MQSpace_ManageSize;
	psDataEnd 		= piSpace + iSize - 1;
	sDataSize		= iSize;

	gClassStr.afMemset( psDataStart, DC_MQDFC, DC_MQSpace_ManageSize );
	sfMQSpace_GetManageHeader(psDataStart,&ssManageHeader);
	
	*ssManageHeader.aSubIdleBegin 	= DC_MQSpace_SubAddrBegin;
	*ssManageHeader.aSubIdleEnd		= DC_MQSpace_SubAddrEnd;
	*ssManageHeader.aSubIdleSize	= DC_MQSpace_SubDataSize;
	
	*ssManageHeader.aRmdIdleBegin 	= DC_MQSpace_RmdAddrBegin;
	*ssManageHeader.aRmdIdleEnd		= DC_MQSpace_RmdAddrEnd;
	*ssManageHeader.aRmdIdleSize  	= DC_MQSpace_RmdDataSize;
}



//////////////////////////////////////////////////////////////////////////////////
// parameter:                                                                   //
// 		iUID	the delete user descriptor                                      //
//			 	the type of UUID is start from 1								//
//				if typed zero, this function will return the action illegal		//
//				under the user space full condition, re-add a register user		//
//				the function will return -3 first 								//
// return:                                                                      //
// 		0 	success                                                             //
//		-1	input paramenter illegal											//
//		-2	User already exist                                                  //
//		-3	Message space user full                                             //
//		-4	unknow error		                                                //
//////////////////////////////////////////////////////////////////////////////////
static int sfMsgQueue_addSubUser(uint32 iUID){
	int32 t32Rect;
	
	if(iUID == 0)
		return -1;
	t32Rect = sfMQSpace_ManageUser(DC_MQSMU_Cmd_SetSubUser,iUID,NULL);
	
	if(t32Rect){
		if( t32Rect == DC_MQSMU_Err_UserExist )
			return -2;
		else if( t32Rect == DC_MQSMU_Err_UserSpaceFull )
			return -3;
		else 
			return -4;
	}
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
// parameter:                                                                   //
// 		iUID	the delete user descriptor                                      //
// return:                                                                      //
// 		0 	success                                                             //
// 		-1 	illegal User                                                        //
// 		-2	the user space didn't have initialized                              //
//////////////////////////////////////////////////////////////////////////////////
static int sfMsgQueue_delSubUser(uint32 iUID){
	
	uint8 _i;
	mUMsgBody *tTools;
	uint16 tUserMask;
	uint8 tStatus = 0;
	
	if(iUID==0)
		return -1;
	
	for(_i=0; _i<DC_MQSpace_SubMaxUser; _i++){
		if(ssManageHeader.aSubUserInfo[_i].aUID == iUID){
			break;
		}
	}
	if(_i==DC_MQSpace_SubMaxUser)
		return -2;
	
	//tClrTools = (asdcbas*)asdfasd.asasdads[_i].asdfasda;
	tTools = (mUMsgBody*)ssManageHeader.aSubUserInfo[_i].aUMsg;
	tUserMask = 0x01<<_i;
	
	ssManageHeader.aSubUserInfo[_i].aUID=0;
	ssManageHeader.aSubUserInfo[_i].aUMsg=NULL;
	(*ssManageHeader.aSubUserNum)--;
	
	if(*ssManageHeader.aSubUserNum){
		
		while( tTools != NULL ){
			tTools->aUserMask &= ~tUserMask;
			if(!(tStatus&0x01)){
				if( tTools->aUserMask != 0 ){
					tStatus |= 0x01;
					*ssManageHeader.aSub1stNode = tTools;
				}
			}
			tTools = tTools->paNext;
		}
		if(tStatus&0x01){
			
			*ssManageHeader.aSubIdleEnd = ((void*)*ssManageHeader.aSub1stNode)-1;
			
			if( *ssManageHeader.aSubIdleEnd < DC_MQSpace_SubAddrBegin )
				*ssManageHeader.aSubIdleEnd = DC_MQSpace_SubAddrEnd;
			
			if( *ssManageHeader.aSubIdleBegin > *ssManageHeader.aSubIdleEnd ){
				*ssManageHeader.aSubIdleSize = \
					(void*)(*ssManageHeader.aSubIdleEnd) +\
					DC_MQSpace_SubDataSize -\
					(void*)(*ssManageHeader.aSubIdleBegin);
				
			}else{
				*ssManageHeader.aSubIdleSize = \
					1 + *ssManageHeader.aSubIdleEnd - *ssManageHeader.aSubIdleBegin;
			}
		}else{
			*ssManageHeader.aSub1stNode = NULL;
			*ssManageHeader.aSubIdleBegin 	= DC_MQSpace_SubAddrBegin;
			*ssManageHeader.aSubIdleEnd		= DC_MQSpace_SubAddrEnd;
			*ssManageHeader.aSubIdleSize	= DC_MQSpace_SubDataSize;
		}
	}else{
		*ssManageHeader.aSubIdleBegin 	= DC_MQSpace_SubAddrBegin;
		*ssManageHeader.aSubIdleEnd		= DC_MQSpace_SubAddrEnd;
		*ssManageHeader.aSubIdleSize	= DC_MQSpace_SubDataSize;	
		*ssManageHeader.aSub1stNode		= NULL;
		gClassStr.afMemset(\
			(uint8*)ssManageHeader.aSubUserInfo,0,\
			sizeof(mMQUserMsg)*DC_MQSpace_SubMaxUser);
		*ssManageHeader.aSubUserNum		= 0;
	}
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
//	parameter:                                                                  //
//		void                                                                    //
//	return:                                                                     //
//		0		success                                                         //
//		-101	Get down_interruptible err                                      //
//////////////////////////////////////////////////////////////////////////////////
static int sfClearMsgBuf(void){
	mMQUserMsg *tUserInfo;
	uint8 _i;
	
	tUserInfo = ssManageHeader.aSubUserInfo;
	for(_i=0; _i< *ssManageHeader.aSubUserNum; _i++)
		tUserInfo[_i].aUMsg =NULL;
	
	tUserInfo = ssManageHeader.aRmdUserInfo;
	for(_i=0; _i< *ssManageHeader.aRmdUserNum; _i++)
		tUserInfo[_i].aUMsg =NULL;
	
	*ssManageHeader.aSubIdleBegin 	= DC_MQSpace_SubAddrBegin;
	*ssManageHeader.aSubIdleEnd		= DC_MQSpace_SubAddrEnd;
	*ssManageHeader.aSubIdleSize	= DC_MQSpace_SubDataSize;
	
	*ssManageHeader.aRmdIdleBegin 	= DC_MQSpace_RmdAddrBegin;
	*ssManageHeader.aRmdIdleEnd		= DC_MQSpace_RmdAddrEnd;
	*ssManageHeader.aRmdIdleSize  	= DC_MQSpace_RmdDataSize;
	
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
//	parameter:                                                                  //
//		iUID	User Descriptor                                                 //
//	return:                                                                     //
//		1		success                                  						//
//		0		none message exist of ths user                                  //
//////////////////////////////////////////////////////////////////////////////////
static int sfIsSomeMsgExist(uint32 iUID){
//	int32 t32Ret;
	//DbgShowTheManagerField();
	if(!sfMQSpace_ManageUser(DC_MQSMU_Cmd_IsSubHNodeExist,iUID,NULL)){
		return 1;
	}
	if(!sfMQSpace_ManageUser(DC_MQSMU_Cmd_IsRmdHNodeExist,iUID,NULL)){
		return 1;
	}
	
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
//parameter:                                                                    //
//	iUID	User Descriptor                                                     //
//return:                                                                       //
//	0		User not Exist                                                      //
//	1 		Sub User Exist                                                      //
//	2		Rmd User Exist                                                      //
//	3		Sub & Rmd User Exist                                                //
//	-101	Get down_interruptible err                                          //
//////////////////////////////////////////////////////////////////////////////////
static int sfIsUserExist(uint32 iUID){
	int tBack = 0;
	if( 0 == sfMQSpace_ManageUser(DC_MQSMU_Cmd_IsSubUserExist,iUID,NULL) )
		tBack |= DC_MQIsUE_Ret_Sub;
	if( 0 == sfMQSpace_ManageUser(DC_MQSMU_Cmd_IsRmdUserExist,iUID,NULL) )
		tBack |= DC_MQIsUE_Ret_Rmd;
	return tBack;
}


//////////////////////////////////////////////////////////////////////////////////
//parameter:                                                                    //
//	piUserMsg	the point to message of the user want to save                   //
//	iMsgLen		the message length                                              //
//return:                                                                       //
//	0		success                                                             //
//	-1		no need report message , no subber user                             //
//////////////////////////////////////////////////////////////////////////////////
static int sfAddAUserMsgForSub(uint8 *piUserMsg, uint16 iMsgLen){
	uint8 		tUserMask = 0;
	uint16 		tSendLen;
	void 		*tTools_mov,*tTools_ori;
	uint8 _i;
	
	tSendLen =  iMsgLen+DC_MQSpace_MsgHeadSize;
	
		if( *ssManageHeader.aSubIdleSize < 3*DC_MQSpace_ManageSize )
			sfClearStubbornUser(DC_MQSCSU_ClrSub);
		
		if( tSendLen > ( 1+DC_MQSpace_SubAddrEnd-(*ssManageHeader.aSubIdleBegin) ) ){
			*ssManageHeader.aSubIdleBegin = DC_MQSpace_SubAddrBegin;
			*ssManageHeader.aSubIdleSize = \
				1 + *ssManageHeader.aSubIdleEnd - *ssManageHeader.aSubIdleBegin;
		}

	if( 0 == *ssManageHeader.aSubUserNum )
		return -1;
	for(_i=0; _i<*ssManageHeader.aSubUserNum; _i++)
		tUserMask |= (0x01<<_i);
		
	{
		mUMsgBody 	tMsgHead;
		
		tMsgHead.aUserMask 	= tUserMask;
		tMsgHead.paNext		=  NULL;
		tMsgHead.aLen		=  iMsgLen;
		
		tTools_ori = tTools_mov = *ssManageHeader.aSubIdleBegin;
		
		tTools_mov = gClassStr.afCopy(tTools_mov,(uint8*)&tMsgHead,sizeof(mUMsgBody) );
		tTools_mov = gClassStr.afCopy(tTools_mov,(uint8*)piUserMsg,iMsgLen );
		
	}
	
	{
		mUMsgBody *tMsgTail;
		uint8 _i;
		
		if( *ssManageHeader.aSub1stNode == NULL ){
			*ssManageHeader.aSub1stNode = tTools_ori;
		}else{
			tMsgTail = *ssManageHeader.aSub1stNode;
			while( tMsgTail->paNext != NULL )
				tMsgTail = tMsgTail->paNext;
			tMsgTail->paNext = tTools_ori;
		}
	
		for( _i =0; _i < *ssManageHeader.aSubUserNum; _i++ )
			if( ssManageHeader.aSubUserInfo[_i].aUMsg == NULL )
				ssManageHeader.aSubUserInfo[_i].aUMsg = tTools_ori;
	}
	
	
	if( tTools_mov >= DC_MQSpace_SubAddrEnd )
		tTools_mov = DC_MQSpace_SubAddrBegin;
	*ssManageHeader.aSubIdleBegin = tTools_mov;
	
	if( *ssManageHeader.aSubIdleBegin > *ssManageHeader.aSubIdleEnd ){
		*ssManageHeader.aSubIdleSize = \
			(void*)(*ssManageHeader.aSubIdleEnd) +\
			DC_MQSpace_SubDataSize -\
			(void*)(*ssManageHeader.aSubIdleBegin);
	}else{
		*ssManageHeader.aSubIdleSize = \
			1 + (void*)(*ssManageHeader.aSubIdleEnd) -\
			(void*)(*ssManageHeader.aSubIdleBegin);
	}
	if( soMQUEUE_Public.infMsgWriteDoneCallBack != NULL )
		soMQUEUE_Public.infMsgWriteDoneCallBack();
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
//parameter:                                                                    //
//	piUserMsg	the point to message of the user want to save                   //
//	iMsgLen		the message length                                              //
//return:                                                                       //
//	0		success                                                             //
//	-1		no need report message , no subber user                             //
//////////////////////////////////////////////////////////////////////////////////
static int sfAddAUserMsgForRmd(uint32 iUID, uint8 *piUserMsg, uint16 iMsgLen){

	int32 t32Ret;
	uint8 tStatus = 0;
	// 0x01 刚刚新注册的用户
	// 0x02 已存在用户
	// 0x04 无头结点
	// 0x08 有头结点
	
	mUMsgBody 	*tNode;
	uint16 		tSendLen;
	void 		*tTools_mov,*tTools_ori;
	
	tSendLen =  iMsgLen+DC_MQSpace_MsgHeadSize;
	
	if( *ssManageHeader.aRmdIdleSize < 3*DC_MQSpace_ManageSize ){
		sfClearStubbornUser(DC_MQSCSU_ClrRmd);
	}
	if( tSendLen > ( 1+DC_MQSpace_RmdAddrEnd-((void*)*ssManageHeader.aRmdIdleBegin) ) ){
		*ssManageHeader.aSubIdleBegin = DC_MQSpace_RmdAddrBegin;
		*ssManageHeader.aSubIdleSize = \
			1 + *ssManageHeader.aSubIdleEnd - *ssManageHeader.aSubIdleBegin;
	}
	
	
		t32Ret = sfMQSpace_ManageUser(DC_MQSMU_Cmd_SetRmdUser,iUID,NULL);
		if( t32Ret == 0 ){
			// printf("- - - >3\r\n");
			tStatus |= (0x01+0x04);
		}else if( t32Ret ==  DC_MQSMU_Err_UserExist ){
			
			tStatus |= 0x02;
			t32Ret = sfMQSpace_ManageUser(DC_MQSMU_Cmd_GetRmdUserHNode,iUID, (void*)&tNode);
			if( t32Ret == DC_MQSMU_Err_UNNotExist){
				tStatus |= 0x04;
			}else{ 
				tStatus |= 0x08;
				while( tNode->paNext != NULL )
					tNode = tNode->paNext;
			}
		}else if( t32Ret ==  DC_MQSMU_Err_UserSpaceFull ){
			
			sfClearStubbornUser(DC_MQSCSU_ClrRmd);
			sfMQSpace_ManageUser(DC_MQSMU_Cmd_SetRmdUser,iUID,NULL);
			tStatus |= (0x01+0x04);
		}
	
	{
		uint8 _i;
		mUMsgBody 	tMsgHead;
		
		for( _i = 0; _i<DC_MQSpace_RmdMaxUser;_i++)
			if( ssManageHeader.aRmdUserInfo[_i].aUID == iUID )
				break;
		
		tMsgHead.aUserNum 	= _i;
		tMsgHead.paNext		= NULL;
		tMsgHead.aLen		= iMsgLen;
		
		tTools_ori = tTools_mov = *ssManageHeader.aRmdIdleBegin;
		
		tTools_mov = gClassStr.afCopy(tTools_mov,(uint8*)&tMsgHead,sizeof(mUMsgBody) );
		tTools_mov = gClassStr.afCopy(tTools_mov,(uint8*)piUserMsg,iMsgLen );	
	}
	
		if( tStatus&0x08 )
			tNode->paNext = tTools_ori;
		else
			sfMQSpace_ManageUser(DC_MQSMU_Cmd_SetRmdUserHNode,iUID,&tTools_ori);
		
		
		
		if( NULL == *ssManageHeader.aRmd1stNode )
			*ssManageHeader.aRmd1stNode = tTools_ori;
	
	if( tTools_mov >= DC_MQSpace_RmdAddrEnd )
		tTools_mov = DC_MQSpace_RmdAddrBegin;
	*ssManageHeader.aRmdIdleBegin = tTools_mov;
	
	if( *ssManageHeader.aRmdIdleBegin > *ssManageHeader.aRmdIdleEnd )
		*ssManageHeader.aRmdIdleSize = \
			(void*)(*ssManageHeader.aRmdIdleEnd) +\
			DC_MQSpace_RmdDataSize -\
			(void*)(*ssManageHeader.aRmdIdleBegin);
	else
		*ssManageHeader.aRmdIdleSize = \
			1 + (void*)(*ssManageHeader.aRmdIdleEnd) -\
			(void*)(*ssManageHeader.aRmdIdleBegin);
	if( soMQUEUE_Public.infMsgWriteDoneCallBack != NULL )
		soMQUEUE_Public.infMsgWriteDoneCallBack();
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
//	parameter:                                                                  //
//		void                                             					    //
//	return:                                                                     //
//		uint8* return the manage block start address.                           //
//////////////////////////////////////////////////////////////////////////////////
static uint8 *sfGetMsgBlockAddress(void)
{
	return (uint8*)psDataStart;
}




//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
                                                                                //
                                                                                //
                                                                                //
                                                                                //
                                                                                //
                                                                                //
                                                                                //
                                                                                //
                                                                                //
static void sfMQSpace_GetManageHeader(void *piSpace,mManageHeader *ipData){
	ipData->aSubIdleBegin 	= piSpace+DC_MQSpace_OFST_SubIdleBegin;
	ipData->aSubIdleEnd	 	= piSpace+DC_MQSpace_OFST_SubIdleEnd  ;
	ipData->aSubIdleSize 	= piSpace+DC_MQSpace_OFST_SubIdleSize ;
	ipData->aSub1stNode 	= piSpace+DC_MQSpace_OFST_Sub1stNode  ;
	ipData->aSubUserInfo 	= piSpace+DC_MQSpace_OFST_SubUserInfo ;
	
	ipData->aRmdIdleBegin 	= piSpace+DC_MQSpace_OFST_RmdIdleBegin;
	ipData->aRmdIdleEnd	 	= piSpace+DC_MQSpace_OFST_RmdIdleEnd  ;
	ipData->aRmdIdleSize 	= piSpace+DC_MQSpace_OFST_RmdIdleSize ;
	ipData->aRmd1stNode 	= piSpace+DC_MQSpace_OFST_Rmd1stNode  ;
	ipData->aRmdUserInfo 	= piSpace+DC_MQSpace_OFST_RmdUserInfo ;
	
	ipData->aSubUserNum	 	= piSpace+DC_MQSpace_OFST_SubUserNum  ;
	ipData->aRmdUserNum		= piSpace+DC_MQSpace_OFST_RmdUserNum  ;
}



static int32 sfMQSpace_ManageUser(uint32 iCmd, uint32 iUUID, void **UMsg){
	uint8 _i = 0;
	
	if( iCmd&DC_MQSMU_Cmd_SetSubUser ){
		// add subber user
		if( *ssManageHeader.aSubUserNum >= DC_MQSpace_SubMaxUser ){
			return DC_MQSMU_Err_UserSpaceFull;
		}else{
			for( _i = 0; _i < DC_MQSpace_SubMaxUser; _i++  )
				if(ssManageHeader.aSubUserInfo[_i].aUID == iUUID)
					return DC_MQSMU_Err_UserExist;
			for( _i = 0; _i < DC_MQSpace_SubMaxUser; _i++ )
				if( 0 == ssManageHeader.aSubUserInfo[_i].aUID )
					break;
			
			ssManageHeader.aSubUserInfo[_i].aUID = iUUID;
			ssManageHeader.aSubUserInfo[_i].aUMsg = NULL;
			(*ssManageHeader.aSubUserNum)++;
		}
	}else if( iCmd&DC_MQSMU_Cmd_SetRmdUser ){
		// add Not subber user
		if( *ssManageHeader.aRmdUserNum >= DC_MQSpace_RmdMaxUser ){
			return DC_MQSMU_Err_UserSpaceFull;
		}else{
			for( _i = 0; _i < DC_MQSpace_RmdMaxUser; _i++  )
				if(ssManageHeader.aRmdUserInfo[_i].aUID == iUUID)
					return DC_MQSMU_Err_UserExist;
			for( _i = 0; _i < DC_MQSpace_RmdMaxUser; _i++  )
				if( 0 == ssManageHeader.aRmdUserInfo[_i].aUID )
					break;
				
			ssManageHeader.aRmdUserInfo[*ssManageHeader.aRmdUserNum].aUID = iUUID;
			ssManageHeader.aRmdUserInfo[*ssManageHeader.aRmdUserNum].aUMsg = NULL;
			(*ssManageHeader.aRmdUserNum)++;
		}
	}
	
	if( iCmd&DC_MQSMU_Cmd_GetSubUserHNode ||\
		iCmd&DC_MQSMU_Cmd_IsSubHNodeExist )
			iCmd |= DC_MQSMU_Cmd_IsSubUserExist;
	else
	if( iCmd&DC_MQSMU_Cmd_SetRmdUserHNode ||\
		iCmd&DC_MQSMU_Cmd_SetRmdUserHNode ||\
		iCmd&DC_MQSMU_Cmd_IsRmdHNodeExist )
			iCmd |= DC_MQSMU_Cmd_IsRmdUserExist;
	
	if( iCmd&DC_MQSMU_Cmd_IsSubUserExist ){
		if( *ssManageHeader.aSubUserNum == 0 )
			return DC_MQSMU_Err_UserSpaceEmpty;
		for( _i=0; _i<DC_MQSpace_SubMaxUser;_i++)
			if( ssManageHeader.aSubUserInfo[_i].aUID == iUUID )
				break;
		if( _i == DC_MQSpace_SubMaxUser )
			return DC_MQSMU_Err_UserNotExist;
	}else if( iCmd&DC_MQSMU_Cmd_IsRmdUserExist ){
		if( *ssManageHeader.aRmdUserNum == 0 )
			return DC_MQSMU_Err_UserSpaceEmpty;
		for( _i=0; _i < DC_MQSpace_RmdMaxUser; _i++)
			if( ssManageHeader.aRmdUserInfo[_i].aUID == iUUID )
				break;
		if( _i == DC_MQSpace_RmdMaxUser )
			return DC_MQSMU_Err_UserNotExist;
	}
	
	// clear register user
	// if( iCmd&DC_MQSMU_Cmd_ClrSubUser ){
		// delete subber user
		// ssManageHeader.aSubUserInfo[_i].aUID = 0;
		// ssManageHeader.aSubUserInfo[_i].aUMsg = NULL;
		// (*ssManageHeader.aSubUserNum)--;
		// return 0;
	// }else if( iCmd&DC_MQSMU_Cmd_ClrRmdUser ){
		// delete not subber user
		// ssManageHeader.aRmdUserInfo[_i].aUID = 0;
		// ssManageHeader.aRmdUserInfo[_i].aUMsg = NULL;
		
		// (*ssManageHeader.aRmdUserNum)--;
		// return 0;
	// }
	
	if( iCmd&DC_MQSMU_Cmd_IsSubHNodeExist ){
		if( NULL != ssManageHeader.aSubUserInfo[_i].aUMsg )
			return 0;
		else 
			return DC_MQSMU_Err_UNNotExist;
	}else if( iCmd&DC_MQSMU_Cmd_IsRmdHNodeExist ){
		if( NULL != ssManageHeader.aRmdUserInfo[_i].aUMsg )
			return 0;
		else 
			return DC_MQSMU_Err_UNNotExist;
	}
	
	if( iCmd&DC_MQSMU_Cmd_GetSubUserHNode ){
		*UMsg = ssManageHeader.aSubUserInfo[_i].aUMsg;
		if( *UMsg == NULL )
			return DC_MQSMU_Err_UNNotExist;
		return 0;
	}else if( iCmd&DC_MQSMU_Cmd_GetRmdUserHNode ){
		*UMsg = ssManageHeader.aRmdUserInfo[_i].aUMsg;
		if( *UMsg == NULL )
			return DC_MQSMU_Err_UNNotExist;
		return 0;
	}
	
	// if( iCmd&DC_MQSMU_Cmd_SetSubUserHNode ){
		// ssManageHeader.aSubUserInfo[_i].aUMsg = *UMsg;
		// return 0;
	// }else 
	if( iCmd&DC_MQSMU_Cmd_SetRmdUserHNode ){
		ssManageHeader.aRmdUserInfo[_i].aUMsg = *UMsg;
		return 0;
	}
	// if( iCmd&DC_MQSMU_Cmd_ClrSubUserHNode ){
		// ssManageHeader.aSubUserInfo[_i].aUMsg = NULL;
		// return 0;
	// }else 
	// if( iCmd&DC_MQSMU_Cmd_ClrRmdUserHNode ){
		// ssManageHeader.aRmdUserInfo[_i].aUMsg = NULL;
		// return 0;
	// }
	
	// if( iCmd&DC_MQSMU_Cmd_GetSubNum ){
		// return (*ssManageHeader.aSubUserNum);
	// }else if( iCmd&DC_MQSMU_Cmd_GetRmdNum ){
		// return (*ssManageHeader.aRmdUserNum);
	// }
	return 0;
}

static int32 sfClearStubbornUser(uint8 iCmd){
	
	
	if( iCmd&DC_MQSCSU_ClrSub ){
		mUMsgBody *tTools;
		uint8 _i;
		uint16 tUserMask;
		uint8 tStatus = 0;
		
		tTools = *ssManageHeader.aSub1stNode;
		// if( tTools == NULL )
			// return -1;
		tUserMask = tTools->aUserMask;
		// if( tUserMask == 0 )
			// return -1;
		
		for( _i=0; _i<DC_MQSpace_SubMaxUser; _i++){
			if( (0x01<<_i)&tUserMask ){
				if( gfMqueue_UserFire != NULL )
					gfMqueue_UserFire(ssManageHeader.aSubUserInfo[_i].aUID);
				ssManageHeader.aSubUserInfo[_i].aUID = 0;
				ssManageHeader.aSubUserInfo[_i].aUMsg = NULL;
				(*ssManageHeader.aSubUserNum)--;
			}
		}
		
		if(*ssManageHeader.aSubUserNum){
			
			while( tTools != NULL ){
				tTools->aUserMask &= ~tUserMask;
				if(!(tStatus&0x01)){
					if( tTools->aUserMask != 0 ){
						tStatus |= 0x01;
						*ssManageHeader.aSub1stNode = tTools;
					}
				}
				tTools = tTools->paNext;
			}
			if(tStatus&0x01){
				
				*ssManageHeader.aSubIdleEnd = ((void*)*ssManageHeader.aSub1stNode)-1;
				
				if( *ssManageHeader.aSubIdleEnd < DC_MQSpace_SubAddrBegin )
					*ssManageHeader.aSubIdleEnd = DC_MQSpace_SubAddrEnd;
				
				if( *ssManageHeader.aSubIdleBegin > *ssManageHeader.aSubIdleEnd ){
					*ssManageHeader.aSubIdleSize = \
						(void*)(*ssManageHeader.aSubIdleEnd) +\
						DC_MQSpace_SubDataSize -\
						(void*)(*ssManageHeader.aSubIdleBegin);
					
				}else{
					*ssManageHeader.aSubIdleSize = \
						1 + *ssManageHeader.aSubIdleEnd - *ssManageHeader.aSubIdleBegin;
				}
			}else{
				*ssManageHeader.aSub1stNode = NULL;
				*ssManageHeader.aSubIdleBegin 	= DC_MQSpace_SubAddrBegin;
				*ssManageHeader.aSubIdleEnd		= DC_MQSpace_SubAddrEnd;
				*ssManageHeader.aSubIdleSize	= DC_MQSpace_SubDataSize;
			}
		}else{
			*ssManageHeader.aSubIdleBegin 	= DC_MQSpace_SubAddrBegin;
			*ssManageHeader.aSubIdleEnd		= DC_MQSpace_SubAddrEnd;
			*ssManageHeader.aSubIdleSize	= DC_MQSpace_SubDataSize;	
			*ssManageHeader.aSub1stNode		= NULL;
			gClassStr.afMemset(\
				(uint8*)ssManageHeader.aSubUserInfo,0,\
				sizeof(mMQUserMsg)*DC_MQSpace_SubMaxUser);
			*ssManageHeader.aSubUserNum		= 0;
		}
	}else if( iCmd&DC_MQSCSU_ClrRmd ){
		
			// 看内存中是否有消息
			// 有消息删除最初的那条消息，的拥有者，和拥有者消息
			// 如果没消息，就清空注册空间
		uint16 tUserNum;
		void *tTools; 
		
		
		tTools = (void*)*ssManageHeader.aRmd1stNode;
		if( NULL == tTools ){
			goto GT_sfClearStubbornUser;
		}else{
			tUserNum = ((mUMsgBody*)tTools)->aUserNum;
			ssManageHeader.aRmdUserInfo[tUserNum].aUID=0;
			ssManageHeader.aRmdUserInfo[tUserNum].aUMsg=NULL;
			(*ssManageHeader.aRmdUserNum)--;
			if( *ssManageHeader.aRmdUserNum ){
				
				uint8 _i;
				void *t1stNode = DC_MQSpace_RmdAddrEnd + DC_MQSpace_RmdDataSize;
				void *tEndNode = NULL;
				void *tLstNode = NULL;
				void *tBeginNode = NULL;
				
				for( _i = 0; _i<DC_MQSpace_RmdMaxUser; _i++ ){
					if( (ssManageHeader.aRmdUserInfo[_i].aUID != 0 )&&\
						(ssManageHeader.aRmdUserInfo[_i].aUMsg != 0 )){
						tTools = (void*)ssManageHeader.aRmdUserInfo[_i].aUMsg;
						
						if( tTools > *ssManageHeader.aRmdIdleEnd ){
							if( tTools < t1stNode){
								t1stNode = tTools;
								tEndNode = tTools;
							}
						}else{	
							if( (tTools+DC_MQSpace_RmdDataSize) < t1stNode){
								t1stNode = tTools+DC_MQSpace_RmdDataSize;
								tEndNode = tTools;
							}
						}
						
						while( ((mUMsgBody*)tTools)->paNext != NULL )
							tTools = (void*)((mUMsgBody*)tTools)->paNext;
						
						if( tTools > *ssManageHeader.aRmdIdleEnd ){
							if( (tTools-DC_MQSpace_RmdDataSize) > tLstNode){
								
								tLstNode = tTools - DC_MQSpace_RmdDataSize;
								tBeginNode = tTools;
							}
						}else{
							if( tTools > tLstNode){
								tLstNode = tTools;
								tBeginNode = tTools;
							}
						}
					}
				}
				
				if( tEndNode != NULL ){
					*ssManageHeader.aRmd1stNode = tEndNode;
					
					*ssManageHeader.aRmdIdleEnd = tEndNode-1;
					if( *ssManageHeader.aRmdIdleEnd < DC_MQSpace_RmdAddrBegin )
						*ssManageHeader.aRmdIdleEnd = DC_MQSpace_RmdAddrEnd;
				}
				if( tBeginNode != NULL ){					
					*ssManageHeader.aRmdIdleBegin = \
						tBeginNode + sizeof(mUMsgBody) + ((mUMsgBody*)tBeginNode)->aLen;
					if( *ssManageHeader.aRmdIdleBegin >  DC_MQSpace_RmdAddrEnd)
						*ssManageHeader.aRmdIdleBegin = DC_MQSpace_RmdAddrBegin;
				}
				
				if( *ssManageHeader.aRmdIdleBegin > *ssManageHeader.aRmdIdleEnd ){
					*ssManageHeader.aRmdIdleSize = \
						(void*)(*ssManageHeader.aRmdIdleEnd) +\
						DC_MQSpace_RmdDataSize -\
						(void*)(*ssManageHeader.aRmdIdleBegin);
				}else{
					*ssManageHeader.aRmdIdleSize = \
						1 + *ssManageHeader.aRmdIdleEnd - *ssManageHeader.aRmdIdleBegin;
				}
				
				
			}else{
				goto GT_sfClearStubbornUser;
			}
		}
	}
return  0;
GT_sfClearStubbornUser:
	*ssManageHeader.aRmdIdleBegin 	= DC_MQSpace_RmdAddrBegin;
	*ssManageHeader.aRmdIdleEnd		= DC_MQSpace_RmdAddrEnd;
	*ssManageHeader.aRmdIdleSize	= DC_MQSpace_RmdDataSize;	
	*ssManageHeader.aRmd1stNode		= NULL;
	gClassStr.afMemset(\
		(uint8*)ssManageHeader.aRmdUserInfo,DC_MQDFC,\
		sizeof(mMQUserMsg)*DC_MQSpace_RmdMaxUser);
	*ssManageHeader.aRmdUserNum		= 0;
return 0;
		
}





mMQUEUE_ProtectFun const scoMQUEUE_ProtectFun = {
	.prfINIT_Queue			=	sfInitMsgQueue,		
	.prfACT_GetSpaceAddress	=	sfGetMsgBlockAddress,
	.prfACT_ClrBuf			=	sfClearMsgBuf,		
	.prfACT_AddSubUser		=	sfMsgQueue_addSubUser,
	.prfACT_DelSubUser		=	sfMsgQueue_delSubUser,
	.prfACT_AddSubMsg		=	sfAddAUserMsgForSub,
	.prfACT_AddRmdMsg		=	sfAddAUserMsgForRmd,
	.prfACT_GetMsg			=	sfGetAUserMsg,		
	.prfIS_MsgExist			=	sfIsSomeMsgExist,	
	.prfIS_UserExist		=	sfIsUserExist,		
};


mMQUEUE_Class goMQUEUE_Class = {
	&scoMQUEUE_ProtectFun,
	&soMQUEUE_Public
};






//以下程序用于测试
unsigned char sgStandCharBlack[]="0123456789ABCDEF";
void DbgPrinStr(unsigned char *iStr,unsigned char *iPD,unsigned short iLen)
{
	unsigned short _i,_j;
	unsigned char tTppl[4];
	tTppl[2] = ' ';
	tTppl[3] = '\0';
	
	MQ_PK("%s",iStr);
	_i = 0;
	while(_i<iLen){
		for(_j=0;(_j<20)&&(_i<iLen);_j++)
		{
			if(_j == 10)
				MQ_PK(" ");
			tTppl[0] = sgStandCharBlack[(iPD[_i]&0xF0)>>4];
			tTppl[1] = sgStandCharBlack[iPD[_i]&0x0F];	
			MQ_PK("%s",tTppl);
			_i++;
		}
		MQ_PK("\r\n");
	}	
}

void DbgShowTheManagerField(void){
	MQ_PK("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\r\n");
	MQ_PK("Sub-->Begin:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aSubIdleBegin));
	MQ_PK("Sub-->End:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aSubIdleEnd));
	MQ_PK("Sub-->Size:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aSubIdleSize));
	MQ_PK("Sub-->1stNode:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aSub1stNode));
	MQ_PK("Sub-->UserNum:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aSubUserNum));
	DbgPrinStr("Sub-->UserInfo:\r\n",(uint8*)(ssManageHeader.aSubUserInfo),sizeof(mMQUserMsg)*DC_MQSpace_SubMaxUser);
	
	MQ_PK("Rmd-->Begin:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aRmdIdleBegin));
	MQ_PK("Rmd-->End:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aRmdIdleEnd));
	MQ_PK("Rmd-->Size:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aRmdIdleSize));
	MQ_PK("Rmd-->1stNode:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aRmd1stNode));
	MQ_PK("Rmd-->UserNum:\t\t%x\r\n",(unsigned int)(*ssManageHeader.aRmdUserNum));
	DbgPrinStr("Rmd-->UserInfo:\r\n",(uint8*)(ssManageHeader.aRmdUserInfo),sizeof(mMQUserMsg)*DC_MQSpace_RmdMaxUser);
	
	MQ_PK("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\r\n");
}
