/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *     initialise two sensor and ISP one bye one with origin function 
 *
 *  Support Hisilicon's chips, as HI3518
 *
 * 
 * File name   : sen_init.c
 * Author      : qiang.feng qiang.feng<qiang.feng@netviewtech.com>
 * Creaet Date : 2014.05.01
 */

/*
 * function     : com_head
 * data         : void
 * return       : void
 * description  : output the head of command 
 */
#include "netview_driver_api.h"

static int g_lens_flag = 0;

void com_head(void)
{
	printf("\n");
	printf("==================================================");
	printf("==============================\n");
	printf("					   Input the command to control the IRC\n");
	if (g_lens_flag)
	{
		printf("<<1 SET_SENSOR00_EN (day)				 ");
		printf("<<2 SET_SENSOR01_EN	(night)		  \n");
	}
	else
	{
		printf("<<1 set ir-filter to day				 ");
		printf("<<2 set ir-filter to	night		  \n");
	}
	printf("<<0 quit \n");
	printf("\n");
	printf("please inter the command.\n");
	printf("command :");
  
}

int main()
{
	 int ifd;
	 int iret;
	 int ichose = 0;
	 static NV_S32 s32NvcFd = -1;
	 Nvc_Driver_Cap_Info_S stCapInfo;
	 NV_IRC_STATUS_E enIrcAutoStatus = -1;
	 
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

	 if (stCapInfo.u32CapMask  & NVC_SUPP_DoubleLens)
		 g_lens_flag = 1;
	 else if ((stCapInfo.u32CapMask & NVC_SUPP_Ircut) == 0)

	 {
		 NV_ERROR("Dont support ir-filter/lens switch for this device.");
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
	 //    printf("Had input %d\n",ichose);
		 if(ichose == 1 || ichose == 2)
		 { 
			 enIrcAutoStatus = ichose == 1 ? NV_IRC_STATUS_DAY : NV_IRC_STATUS_NIGHT;
			 if ((stCapInfo.u32CapMask & NVC_SUPP_Ircut) && NV_SUCCESS == nv_drv_set_filter_switch(s32NvcFd, enIrcAutoStatus))
			 {
				 printf("Switch filter to %s success!", ichose == 1 ? "day" : "night");
			 }
			 if ((stCapInfo.u32CapMask & NVC_SUPP_DoubleLens) && NV_SUCCESS == nv_drv_set_lens_switch(s32NvcFd, enIrcAutoStatus))
			 {
				 printf("Switch lens to %s success!", ichose == 1 ? "day" : "night");
			 }
		 }
		 else if(ichose == 0 )
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

