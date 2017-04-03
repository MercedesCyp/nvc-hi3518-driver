/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *               test the driver of state_led
 * command    :
 *
 *  Support Hisilicon's chips, as HI3518C
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 * 
 * File name        : test_state_led.c
 * Author           : qiang.feng
 * Create Date      : 2012.02.08
 */
#include "netview_driver_api.h"

static Nvc_Driver_Cap_Info_S stCapInfo;

/*
 * function     : com_head
 * data         : void
 * return       : void
 * description  : output the head of command 
 */
void com_head(void)
{
    printf("\n");
    printf("==================================================");
    printf("==============================\n");
    printf("                      Input the command to control the IRC\n");
    printf("<<1 LED01 ON                    ");
    printf("<<2 LED01 OFF\n");
    printf("<<3 LED01 FLASH\n");
	if (stCapInfo.u8LedCnt > 1)
	{
		printf("==================================================\n");
	    printf("<<4 LED02 ON                     ");
	    printf("<<5 LED02 OFF\n");
	    printf("<<6 LED02 FLASH\n");
	}
	if (stCapInfo.u8LedCnt > 2)
	{
		printf("==================================================\n");
	    printf("<<7 LED03 ON                    ");
	    printf("<<8 LED03 OFF\n");
	    printf("<<9 LED03 FLASH\n");
	}
    printf("<<0 quit \n");
    printf("\n");
    printf("please inter the command.\n");
    printf("command :");
         
}


int main(int argc, char ** argv)
{
	int ifd;
	int iret;
	int ichose = 0;
	nv_plugin_led_t stLedCtrl;
	stLedCtrl.enLedColor = NV_LED_GREEN;
	NV_S32 s32NvcFd = -1;
	
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_StateLed) == 0 || stCapInfo.u8LedCnt <= 0)
	{
		NV_ERROR("Dont support state led for this device.");
		close(s32NvcFd);
		return -3;
	}
	system("clear");
	//input command repeatedly, if input 'q' or "quit" then end the programme
    while(1)
    {
        com_head(); 

        scanf("%d", &ichose);
        printf("Had input %d\n",ichose);

        switch(ichose)
        { 
            case 1:
            {
				stLedCtrl.u32On = 1000;
				stLedCtrl.u32Off = 0;
				nv_drv_ctrl_state_led(s32NvcFd, 0, &stLedCtrl, 1);
                break;
            }
            case 2:
            {
				stLedCtrl.u32On = 0;
				stLedCtrl.u32Off = 1000;
				nv_drv_ctrl_state_led(s32NvcFd, 0, &stLedCtrl, 1);
                break;
            }
			case 3:
            {
				stLedCtrl.u32On = 100;
				stLedCtrl.u32Off = 100;
				nv_drv_ctrl_state_led(s32NvcFd, 0, &stLedCtrl, 1);
                break;
            }

            case 4:
            {
				 stLedCtrl.u32On = 1000;
				 stLedCtrl.u32Off = 0;
				 nv_drv_ctrl_state_led(s32NvcFd, 1, &stLedCtrl, 1);
                break;
            }
            case 5:
            {
				stLedCtrl.u32On = 0;
				stLedCtrl.u32Off = 1000;
				nv_drv_ctrl_state_led(s32NvcFd, 1, &stLedCtrl, 1);
                break;
            }
			case 6:
            {
				stLedCtrl.u32On = 300;
				stLedCtrl.u32Off = 400;
				nv_drv_ctrl_state_led(s32NvcFd, 1, &stLedCtrl, 1);
                break;
            }


            case 7:
            {
				stLedCtrl.u32On = 1000;
				stLedCtrl.u32Off = 0;
				nv_drv_ctrl_state_led(s32NvcFd, 2, &stLedCtrl, 1);
                break;
            }
            case 8:
            {
				stLedCtrl.u32On = 0;
				stLedCtrl.u32Off = 1000;
				nv_drv_ctrl_state_led(s32NvcFd, 2, &stLedCtrl, 1);
                break;
            }
			case 9:
            {
				stLedCtrl.u32On = 500;
				stLedCtrl.u32Off = 600;
				nv_drv_ctrl_state_led(s32NvcFd, 2, &stLedCtrl, 1);
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
		getchar();
		fflush(stdin);
		//printf("%s: no found\n", cacommand);
	 }
	close(s32NvcFd);
	return 0;
}

