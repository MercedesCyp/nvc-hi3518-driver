#include "netview_driver_api.h"

void com_head(void)
{
    printf("\n\n\n");
    printf("======================================== =========");
    printf("==============================\n");
    printf("                      Input the command to control the module\n\n");
    printf(" 1) Door Lock OFF                      2) Door Lock ON\n");
    printf(" 3) Read the state of Door Lock\n");
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_DoorLock) == 0)
	{
		NV_ERROR("Dont support DoorLock for this device.");
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
				if(nv_drv_set_doorlock_switch(s32NvcFd, DC_MsgDoorLock_Close) != 0)
				{
				   printf(" When set DoorLock OFF iret = %d\n", iret);
				}
				break;
			}
			case 2:
			{
				if(nv_drv_set_doorlock_switch(s32NvcFd, DC_MsgDoorLock_Open) != 0)
				{
					printf(" When set DoorLock ON iret = %d\n", iret);
				}
				break;
			}
			case 3:
			{
				if((iret = nv_drv_get_doorlock_switch(s32NvcFd, &enState)) != 0)
				{
					printf(" Get DoorLock status iret = %d\n", iret);
					break;
				}
				else
				{
					if(enState == 0)
					{
						printf("DoorLock status is OFF\n");
					}
					if(enState == 1)
					{
						printf("DoorLock status is ON \n");
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
	 }
	close(s32NvcFd);
	return 0;
}

