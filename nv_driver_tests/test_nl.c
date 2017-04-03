/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *              Test ON or OFF for night light
 *
 *  Support Hisilicon's chips, as HI3518
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 * 
 * File name           : test_nl.c
 * Author              : qiang.feng
 * Create data         : 14.05.02
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
    printf(" 1) night lignt off                 2) night lignt ON\n");
    printf(" 3) read the state of night light\n");
    printf(" 0) quit");
    printf("\n");
    printf("please inter the command.\n\n");
    printf("command :");
         
}

int main()
{
	int ifd;
	int iret;
	int ichose = 0;
	static NV_S32 s32NvcFd = -1;
	Nvc_Driver_Cap_Info_S stCapInfo;
	nv_plugin_nightlight_t stCtrlPlug;	
	NV_S32 enState;	   //ON or OFF
	
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_NightLight) == 0)
	{
		NV_ERROR("Dont support night light for this device.");
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
	//	  printf("Had input %d\n",ichose);
		switch(ichose)
		{ 
			case 1:
			{
			   stCtrlPlug.enNightLightStatus = NV_NIGHTLIGHT_STATUS_OFF;
			   stCtrlPlug.u8Luminance = 100;
			   if(nv_drv_ctrl_night_light(s32NvcFd, &stCtrlPlug, 1) != 0)
			   {
				   printf(" When set night light off iret = %d\n", iret);
			    }
				break;
			}
			case 2:
			{
				stCtrlPlug.enNightLightStatus = NV_NIGHTLIGHT_STATUS_ON;
				stCtrlPlug.u8Luminance = 100;
				if(nv_drv_ctrl_night_light(s32NvcFd, &stCtrlPlug, 1) != 0)
				{
					printf(" When set night light on iret = %d\n", iret);
				 }
				 break;
			}
			
			case 3:
			{
				if((iret = nv_drv_get_night_light_switch(s32NvcFd, &enState)) != 0)
				{
					printf(" iret = %d\n", iret);
					break;
				}
				else
				{
					if(enState == 0)
					{
						printf("Night light status is OFF\n");
					}
					if(enState == 1)
					{
						printf("Night light status is On \n");
					}
				}
				break;
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


