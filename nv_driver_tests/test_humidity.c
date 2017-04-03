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
    printf(" 1) get cur humidity                      2) monitor environment humidity\n");
    printf(" 0) quit");
    printf("\n");
    printf("please inter the command.\n\n");
    printf("command :");
}

void humidity_monitor_thread(void * arg)
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
		
		if ((nSize = nv_drv_wait_message(s32NvcFd, g_fDriver_version >= 1.0 ? NVC_REPORT_HUMIDIT_VALUE_MSG : NVC_OLD_REPORT_HUMIDIT_VALUE_MSG, szBuffer, sizeof(szBuffer), 30000)) <= 0)
			continue;

		printf("\nEnvironment cur humidity %f!\n", pDrvMsg->stHumidity.u32Humidity/100.0);
	}
}

int main()
{
	int ifd;
	int iret;
	int ichose = 0;
	Nvc_Driver_Cap_Info_S stCapInfo;
	float fHumidity;	   
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_HumiMonitor) == 0)
	{
		NV_ERROR("Dont support humidity for this device.");
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
			nv_drv_set_humidity_timer(s32NvcFd, 0, 1);
			nv_drv_set_attached_msg(s32NvcFd, 0);
			pthread_create_flag = 0;
		}
	//	  printf("Had input %d\n",ichose);
		switch(ichose)
		{ 
			case 1:
			{
			   if(iret = nv_drv_get_cur_humidity(s32NvcFd, &fHumidity))
			   {
				   printf(" iret = %d\n", iret);
				   break;
				}
				else
				{
					printf("Get cur humidity %f\n", fHumidity);
				}
				break;
			}
			case 2:
			{
				if (pthread_create_flag != 0)
					break;

				if (0 != pthread_create(&pthread_id, NULL, (void* (*)(void*))humidity_monitor_thread, NULL))
				{
					printf("pthread_create failed!\n");
					break;
				}
				nv_drv_set_attached_msg(s32NvcFd, 1);
				nv_drv_set_humidity_timer(s32NvcFd, 30, 1);
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


