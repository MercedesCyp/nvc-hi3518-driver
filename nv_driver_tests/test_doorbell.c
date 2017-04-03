#include "netview_driver_api.h"

void com_head(void)
{
    printf("\n\n\n");
    printf("======================================== =========");
    printf("==============================\n");
    printf("                      Input the command to control the module\n\n");
    printf(" 1) Door Bell Tap\n");//                      2) Door Lock ON\n");
    //printf(" 3) Read the state of Door Lock\n");
    printf(" 0) Quit");
    printf("\n");
    printf("Please inter the command.\n\n");
    printf("Command :");
}

int main()
{
	int ifd;
	int iret;
	int ichose = 0;
	static NV_S32 s32NvcFd = -1;
	Nvc_Driver_Cap_Info_S stCapInfo;
	NV_S32 enState;	   //Open or Close
	
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_DoorBell) == 0)
	{
		NV_ERROR("Dont support DoorBell for this device.");
		close(s32NvcFd);
		return -3;
	}
	system("clear");

	while(1)
	{
		com_head(); 
		scanf("%d", &ichose);
		switch(ichose)
		{ 
			case 1:
			{
				if(nv_drv_set_doorbell_tap(s32NvcFd, DC_MsgDoorBell_Tap) != 0)
				{
				   printf(" When set DoorBall Tap iret = %d\n", iret);
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
	 }
	close(s32NvcFd);
	return 0;
}

