#include "netview_driver_api.h"

void com_head(void)
{
    printf("\n\n\n");
    printf("======================================== =========");
    printf("==============================\n");
    printf("                      Input the command to control the module\n\n");
    printf(" 1) set irc switch to day                      2) set irc switch to night\n");
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

    if ((stCapInfo.u32CapMask & NVC_SUPP_LdrMonitor) == 0 || 
		(stCapInfo.u32CapMask & (NVC_SUPP_Ircut | NVC_SUPP_IfrLamp | NVC_SUPP_DoubleLens)) == 0)
 
	{
		NV_ERROR("Dont support irc switch for this device.");
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
		if(ichose == 1 || ichose == 2)
		{ 
			enIrcAutoStatus = ichose == 1 ? NV_IRC_STATUS_DAY : NV_IRC_STATUS_NIGHT;
            if ((stCapInfo.u32CapMask & NVC_SUPP_Ircut) && NV_SUCCESS == nv_drv_set_filter_switch(s32NvcFd, enIrcAutoStatus))
        	{
				printf("Switch filter to %s success!", ichose == 1 ? "day" : "night");
        	}
            if ((stCapInfo.u32CapMask & NVC_SUPP_IfrLamp) && NV_SUCCESS == nv_drv_set_lamp_switch(s32NvcFd, enIrcAutoStatus))
			{
				printf("Switch lamp to %s success!", ichose == 1 ? "off" : "on");
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
		else
		{
			printf("Wrong command!\n");
		}
		getchar();
		fflush(stdin);
		//printf("%s: no found\n", cacommand);
	 }
	close(s32NvcFd);
	return 0;
}

