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
    printf("                      Input the command to control the FM1288\n");
    printf("<<1 write data            ");
    printf("<<2 read register\n");
    printf("<<0 quit \n");
    printf("\n");
    printf("please inter the command.\n");
    printf("command :");
         
}


int main(int argc, char ** argv)
{
	int i=0;
	int iret;
	int ichose = 0;
	nv_plugin_fm1288_t stFM1288;
	memset(&stFM1288, 0, sizeof(stFM1288));
	int temp1 = 0;
	int temp2 = 0;
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

	if ((stCapInfo.u32CapMask & NVC_SUPP_FM1288) == 0)
	{
		NV_ERROR("Dont support fm1288 for this device.");
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
            	memset(&stFM1288, 0, sizeof(stFM1288));
            	stFM1288.enFM1288CtrlType = NV_FM1288_CTRL_WRITE;
            	printf("input the data length: ");
            	scanf("%d", &stFM1288.u32DataLength);
            	stFM1288.u32DataLength += 1;
            	stFM1288.u8Data = (uint8*)calloc(1, stFM1288.u32DataLength);
            	printf("u32DataLength: %d \n", stFM1288.u32DataLength);
            	printf("input the data: ");
            	stFM1288.u8Data[0] = 0xc0;
            	for(i=1; i<stFM1288.u32DataLength; i++)
            	{
            		scanf("%x,", &temp1);
					stFM1288.u8Data[i] = temp1;
            	}
            	for (i=0; i<stFM1288.u32DataLength; i++)
				{
					printf("after: u8Data[%d]: 0x%x \n", i, stFM1288.u8Data[i]);
				}
				usleep(10);
				// the data send to I2C
				iret = nv_drv_ctrl_fm1288(s32NvcFd, &stFM1288, 1);
				if( iret != 0)
				{
					printf("FM1288 write iret = %d\n", iret);
				}
				free(stFM1288.u8Data);
                break;
            }
            case 2:
            {
				memset(&stFM1288, 0, sizeof(stFM1288));
				stFM1288.enFM1288CtrlType = NV_FM1288_CTRL_READ;
				printf("input the register: ");
				scanf("%x, %x", &temp1, &temp2);
				stFM1288.u8RegHigh = temp1;
				stFM1288.u8RegLow = temp2;
				printf("after: u8RegHigh: 0x%x \n", stFM1288.u8RegHigh);
				printf("after: u8RegLow: 0x%x \n", stFM1288.u8RegLow);
				usleep(10);
				// the data read from register
				iret = nv_drv_ctrl_fm1288(s32NvcFd, &stFM1288, 1);
				if( iret != 0)
				{
					printf("FM1288 write iret = %d\n", iret);
				}
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

