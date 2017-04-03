/*************************************************************
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *			Test the driver of fm1288
 *			Write data to FM1288 register
 *			Data from outer file g06_fm1288_tuner.h
 * command    :
 * 
 *
 *  Support Hisilicon's chips, as HI3518C
 *  Chiyuan.Ma<chiyuan.ma@netviewtech.com>
 *
 * 
 * File name        : test_write_fm1288.c
 * Author           : Chiyuan.Ma
 * Create Date      : 2016.10.12
 *************************************************************/
#include "netview_driver_api.h"

#define DATA_FILE	"g06_fm1288_tuner.h"

int HexChar_to_DecInt(char *hex);
int read_data_from_file(char *readdata);


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
	char u8DataBuf[1024] = {0};
	
	s32NvcFd = open(NV_DRIVER_DEVICE, O_RDWR | O_NONBLOCK);
	printf("s32NvcFd = %d\n", s32NvcFd);
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
	//system("clear");
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
			
				stFM1288.u32DataLength = read_data_from_file(u8DataBuf);
					
				//stFM1288.u32DataLength += 1;
				stFM1288.u8Data = (uint8*)calloc(1, stFM1288.u32DataLength);
				if (NULL == stFM1288.u8Data)
				{
					printf("%s calloc failed!\n", __func__);
				}
				else
				{
					printf("u32DataLength: %d \n", stFM1288.u32DataLength);
				}

				stFM1288.u8Data[0] = 0xc0;
				for(i=1; i<stFM1288.u32DataLength; i++)
				{
					stFM1288.u8Data[i] = u8DataBuf[i];
				}
/*					
				printf("stFM1288.u8Data[]:\n");
				for (i=0; i<stFM1288.u32DataLength; i++)
				{
					//printf("after: stFM1288.u8Data[%d]: 0x%x \n", i, stFM1288.u8Data[i]);		
					if (0xFC == stFM1288.u8Data[i])
					{
						printf("\n");
					}
					printf("0x%2X  ", stFM1288.u8Data[i]);
				}
				printf("\n");
*/				
				usleep(10);
					
				// Send data to I2C
				printf("Data ready!\n");
				iret = nv_drv_ctrl_fm1288(s32NvcFd, &stFM1288, 1);
				//printf("nv_drv_ctrl_fm1288() iret = %d\n", iret);
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

/*************************************************************
Function:
		read_data_from_file

Description:
	从外部数据文件中将数组数据读取到程序中
	并将读取到的十六进制字符串数据转换为整形数据。

Return:
		The number of data read from outer file.
*************************************************************/

int read_data_from_file(char *readdata)
{
	int fd = 0;
	int ret = 0;
	char tmp = 0;
	char tmpbak = 0;
	int data_index = 0;
	int flag = 0;
	char buf[5] = {0};
	int index = 0;

	fd = open(DATA_FILE, O_RDONLY);
	if (-1 == fd)
	{
		perror("File open");
		return -1;
	}
	printf("File %s opened!\n", DATA_FILE);
	
	while(-1 != ret)
	{
		ret = read(fd, &tmp, 1);
		if (-1 == ret)
		{
			perror("Data read");
			return -1;
		}

		if ('{' == tmp)
		{
			flag = 1;
			printf("Find data!\n");
		}
		else if('}' == tmp)
		{
			flag = 0;
			buf[index] = '\0';
			*(readdata + data_index) = HexChar_to_DecInt(buf);
			data_index++;
			printf("\nData over!\n");
			
			break;
		}

		if (!(HEXCHK(tmp) || (tmp == ',') || (tmp == 'x') || (tmp == 'X')) || (0 == flag))
		{
			continue;	
		}


		if (('0' == tmpbak) && ('x' == tmp))
		{
			memset(buf, 0, sizeof(buf));
			index = 0;
			
			continue;
		}

		if (',' == tmp)
		{
			buf[index] = '\0';
			*(readdata + data_index) = HexChar_to_DecInt(buf);
			data_index++;
		}
		else
		{
			buf[index] = tmp;
			index++;
		}
		tmpbak = tmp;
	}
	
	close(fd);
	
	printf("Data read OK!\n");
	printf("Totle %d numbers!\n", data_index);
/*	
	for (index = 0; index < data_index; index++)
	{
		if (0xFC == readdata[index])
		{
			printf("\n");
		}
		printf("0x%X  ", *(readdata + index));
	}
*/
	printf("\nCheck Data OK!\n");

	return data_index;		//返回从外部文件读取到的数据个数
}




/****************************************
Function:
		HexChar_to_DecInt()

Description:
		Change the Hex char to int.
	
Return:
		The data successful Changed.
*****************************************/

int HexChar_to_DecInt(char *hex)
{
	const char hex_arr[] = "0123456789ABCDEF";
	int i = 0;
	int tmp = 0;
	for (i = 0; i < 16; i++)
	{
		if (hex[0] == hex_arr[i])
		{
			tmp += i * 16;
		}
		
		if (hex[1] ==hex_arr[i])
		{
			tmp += i;
		}
	}

	return tmp;
}

