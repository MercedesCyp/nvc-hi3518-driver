/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *              This is test for pt2803.ko 
 * Modify:
 *              Create 14.04.30
 * command    : 
 *              Chose the number follow the message as print in the screen.
 *
 *  Support Hisilicon's chips, as HI3518
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 * 
 * File name : test_pt2803.c
 * Author    : qiang.feng
 */
#include "netview_driver_api.h"
/*
 * function     : com_head
 * data         : void
 * return       : void
 * description  : output the head of command 
 */
void com_head(void)
{
    printf("\n\n\n");
    printf("======================================== =========");
    printf("==============================\n");
    printf("                      Input the command to control the module\n\n");
    printf(" 1) run up 150 steps             2) run down 150 steps\n");
	printf("\n");
    printf(" 3) run left 300 steps             4) run right 300 stpes\n");
	printf("\n");
    printf(" 5) run left 300, up 150 steps             6) run left 300, down 150 steps\n");
	printf("\n");
    printf(" 7) run right 300, up 150 steps            8) run right 300, down 150 steps\n");
	printf("\n");
    printf(" 9) run up 15 degree             10) run down 15 degree\n");
	printf("\n");
    printf(" 11) run left 30 degree             12) run right 30 degree\n");
	printf("\n");
    printf(" 13) run left 30, up 15 degree             14) run left 30, down 15 degree\n");
	printf("\n");
    printf(" 15) run right 30, up 15 degree            16) run right 30, down 15 degree\n");
	printf("\n");
    printf(" 0) quit");
    printf("\n");
    printf("please inter the command.\n\n");
    printf("command :");
}

int main(int argc, char ** argv)
{
    int nSize;
    int ichose = 0;
    NV_S32 s32NvcFd = -1;
	Nvc_Driver_Cap_Info_S stCapInfo;
	static Nvc_Ptz_Param stPtzParam = {0};
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
   //
	s32NvcFd = open(NV_DRIVER_DEVICE, O_RDWR | O_NONBLOCK);
	if (s32NvcFd < 0)
	{
		NV_ERROR("Can't open %s.", NV_DRIVER_DEVICE);
		return -1;
	}
	if (NV_SUCCESS != nv_drv_init_device_info(s32NvcFd, NULL, &stCapInfo))
	{
		close(s32NvcFd);
		return -2;
	}
	if ((stCapInfo.u32CapMask & NVC_SUPP_PTZ) == 0)
	{
		NV_ERROR("Dont support ptz for this device.");
		close(s32NvcFd);
		return -3;
	}
	if (NV_SUCCESS != nv_drv_init_ptz_info(s32NvcFd, &stPtzParam))
	{
		NV_ERROR("Get ptz parameter error for this device!");
		close(s32NvcFd);
		return -3;
	}
	//
    system("clear");
    nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_PTZ_COMMON_REQ : NVC_OLD_CONTROL_PTZ_COMMON_REQ, sizeof(Nvc_Ptz_Control_S), 0);
	pDrvMsg->stPtzCtrl.u8Speed = 50;
    //input command repeatedly, if input 'q' or "quit" then end the programme
    while(1)
    {
       // iflags = 0;
        com_head(); 
        scanf("%d", &ichose);
		pDrvMsg->stPtzCtrl.u8ParaType = ichose >= 9 ? 1 : 0;
		pDrvMsg->stPtzCtrl.u8PtzCmd = ichose >= 9 ? ichose - 8 : ichose;
    //    printf("Had input %d\n",ichose);
        switch(ichose)
        { 
            case 1:
			case 2:
            {
				pDrvMsg->stPtzCtrl.u32HSteps = 0;
				pDrvMsg->stPtzCtrl.u32VSteps = 150;
                break;
            }

  
            case 3:
			case 4:
            {
				pDrvMsg->stPtzCtrl.u32HSteps = 300;
				pDrvMsg->stPtzCtrl.u32VSteps = 0;
               break;
            }

            case 5:
            case 6:
            case 7:
            case 8:
            {
				pDrvMsg->stPtzCtrl.u32HSteps = 300;
				pDrvMsg->stPtzCtrl.u32VSteps = 150;
                break;
            }

		  case 9:
		  case 10:
		  {
			  pDrvMsg->stPtzCtrl.u32HSteps = 0;
			  pDrvMsg->stPtzCtrl.u32VSteps = 15;
			  break;
		  }
		  case 11:
	      case 12:
		  {
			  pDrvMsg->stPtzCtrl.u32HSteps = 30;
			  pDrvMsg->stPtzCtrl.u32VSteps = 0;
			 break;
		  }
		  
		  
		  case 13:
	      case 14:
	      case 15:
		  case 16:
		  {
			  pDrvMsg->stPtzCtrl.u32HSteps = 30;
			  pDrvMsg->stPtzCtrl.u32VSteps = 15;
			  break;
		  }

          default:
            {
                break;
            }
        }
        if(ichose == 0 )
        {
            break;
        }
		else if (ichose <= 16 )
		{
			if (nv_drv_send_message(s32NvcFd, szBuffer, sizeof(Nvc_Driver_Msg_Hdr_S) 
				+ sizeof(Nvc_Ptz_Control_S)) != sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Ptz_Control_S))
			{
				NV_ERROR("Transfer Ptz Ctrl Request Error!");
				break;
			}
			if ((nSize = nv_drv_wait_message(s32NvcFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_PTZ_COMMON_RESP : NVC_OLD_CONTROL_PTZ_COMMON_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
			{
				NV_ERROR("Wait Ptz Ctrl Resp Error!");
				break;
			}
			if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
			{
				NV_ERROR("Ptz Ctr Error %d!", pDrvMsg->stMsgHdr.u8ErrCode);
			}
		}
        getchar();
        fflush(stdin);
        //printf("%s: no found\n", cacommand);
     }
    return 0;
}

