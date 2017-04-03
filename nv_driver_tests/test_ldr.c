/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *              This program is test for button.
 *
 *  Support Hisilicon's chips, as HI3518
 * 
 * File name : test_button.c
 * Author    : qiang.feng
 */
 
#include "netview_driver_api.h"

static NV_S32 s32NvcFd = -1;
static int pthread_run = 0;

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
    printf(" 1) get ldr status                      2) monitor ldr status\n");
    printf(" 0) quit");
    printf("\n");
    printf("please inter the command.\n\n");
    printf("command :");
}

void ldr_monitor_thread(void * arg)
{
	int nSize = 0;
	char szBuffer[128] ={0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	//
	pthread_run = 1;
	while(pthread_run == 1)
	{
        if (nv_drv_select_fd(s32NvcFd, 1000) != 1)
			continue;

		if ((nSize = nv_drv_wait_message(s32NvcFd, g_fDriver_version >= 1.0 ? NVC_REPORT_LDR_STATUS_MSG : NVC_OLD_REPORT_LDR_STATUS_MSG, szBuffer, sizeof(szBuffer), 2000)) <= 0)
			continue;

		printf("\nReport ldr status %s!\n", pDrvMsg->stLdrStatus.u8Status == 0 ? "night" : "day");
	}
}

int main()
{
	int ifd;
	int iret;
	int ichose = 0;
	int nButtonNo = 0;
	Nvc_Driver_Cap_Info_S stCapInfo;
	NV_IRC_STATUS_E enState;	   
	int pthread_create_flag = 0;
	pthread_t pthread_id;
	
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_LdrMonitor) == 0)
	{
		NV_ERROR("Dont support ldr monitor for this device.");
		close(s32NvcFd);
		return -3;
	}
	system("clear");
	//input command repeatedly, if input 'q' or "quit" then end the programme
	while(1)
	{
	   // iflags = 0;
		com_head(); 
	   
		scanf("%d", &ichose);
		if (pthread_create_flag != 0 && ichose != 2)
		{
			pthread_run = 0;
			pthread_join(pthread_id, NULL);
			nv_drv_set_attached_msg(s32NvcFd, 0);
			pthread_create_flag = 0;
		}
	//	  printf("Had input %d\n",ichose);
		switch(ichose)
		{ 
			case 1:
			{
				if(iret = nv_drv_get_ldr_status(s32NvcFd, &enState))
				{
					printf(" iret = %d\n", iret);
					break;
				}
				else
				{
					if(enState == NV_IRC_STATUS_DAY)
					{
						printf("Cur ldr status day!\n");
					}
					if(enState == NV_IRC_STATUS_NIGHT)
					{
						printf("Cur ldr status night!\n");
					}
				}
				break;
			}
			case 2:
			{
				if (pthread_create_flag != 0)
					break;

				if (0 != pthread_create(&pthread_id, NULL, (void* (*)(void*))ldr_monitor_thread, NULL))
				{
					printf("pthread_create failed!\n");
					break;
				}
				nv_drv_set_attached_msg(s32NvcFd, 1);
				pthread_create_flag = 1;
			}
		}

		if(ichose == 0 )
		{
			break;
		}
		getchar();
		fflush(stdin);
		//printf("%s: no found\n", cacommand);
	 }
	close(s32NvcFd);
	return 0;
}

