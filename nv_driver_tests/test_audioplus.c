#include "netview_driver_api.h"

void com_head(void)
{
    printf("\n\n\n");
    printf("======================================== =========");
    printf("==============================\n");
    printf("                      Input the command to control the module\n\n");
    printf(" 1) audio plus off                      2) audio plus on\n");
    printf(" 3) read the state of audio plus\n");
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
	NV_S32 s32NvcFd = -1;
	NV_AUDIOPLUS_STATUS_E enState;	   //ON or OFF
	Nvc_Driver_Cap_Info_S stCapInfo;
	nv_plugin_audioplus_t stCtrlPlug;
	
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_AudioPlug) == 0)
	{
		NV_ERROR("Dont support audio plus for this device.");
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
			   stCtrlPlug.enAudioPlusStatus = NV_AUDIOPLUS_STATUS_OFF;
			   if(nv_drv_ctrl_audio_plug(s32NvcFd, &stCtrlPlug, 1000) != 0)
			   {
				   printf(" When set audio plug off iret = %d\n", iret);
				}
				break;
			}
			case 2:
			{
				stCtrlPlug.enAudioPlusStatus = NV_AUDIOPLUS_STATUS_ON;
				if(nv_drv_ctrl_audio_plug(s32NvcFd, &stCtrlPlug, 1000) != 0)
				{
					printf("When set audio plug ON iret = %d\n", iret);
				}
				break;
			}
			case 3:
			{
				if((iret = nv_drv_get_audio_plug_status(s32NvcFd, &enState)) != 0)
				{
					printf(" iret = %d\n", iret);
					break;
				}
				else
				{
					if(enState == NV_AUDIOPLUS_STATUS_ON)
					{
						printf("Audio plug is ON\n");
					}
					if(enState == NV_AUDIOPLUS_STATUS_OFF)
					{
						printf("Audio plug is OFF \n");
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

