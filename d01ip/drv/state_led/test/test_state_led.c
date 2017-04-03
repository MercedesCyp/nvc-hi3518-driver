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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "test_state_led.h"
#include "../state_led.h"
#include "../type.h"

#define DRV_PATH   "/dev/state_led"



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
    printf("==================================================\n");
    printf("<<4 LED02 ON                     ");
    printf("<<5 LED02 OFF\n");
    printf("<<6 LED02 FLASH\n");
    printf("==================================================\n");
    printf("<<7 LED03 ON                    ");
    printf("<<8 LED03 OFF\n");
    printf("<<9 LED03 FLASH\n");

    printf("<< quit \n");
    printf("\n");
    printf("please inter the command.\n");
    printf("command :");
         
}


int main(int argc, char ** argv)
{
    S32 u32_fd;
    U32  u32_chose;

	flash_time ft;

    u32_fd = open(DRV_PATH, O_RDWR);
    if(u32_fd < 0)
    {
        printf("err: Can't open %s.", DRV_PATH);
        return FAULT;
    }

    //input command repeatedly, if input 'q' or "quit" then end the programme
    while(1)
    {
        com_head(); 

        scanf("%d", &u32_chose);
        printf("Had input %d\n",u32_chose);

        switch(u32_chose)
        { 
            case 1:
            {
                if(ioctl(u32_fd, SET_LED01_ON) != SUCCESS)
                {
                    perror("err: SET_LED01_ON");
                    exit(1);
                }
                break;
            }
            case 2:
            {
                if(ioctl(u32_fd, SET_LED01_OFF) != SUCCESS)
                {
                    perror("err: SET_LED01_ON");
                    exit(1);
                }
                break;
            }
			case 3:
            {
			    ft.u32_on = 10;
				ft.u32_off = 10;
                if(ioctl(u32_fd, SET_LED01_FLASH, &ft) != SUCCESS)
                {
                    perror("err: SET_LED01_FLASH");
                    exit(1);
                }
                break;
            }

            case 4:
            {
                if(ioctl(u32_fd, SET_LED02_ON) != SUCCESS)
                {
                    perror("err: SET_LED02_ON");
                    exit(1);
                }
                break;
            }
            case 5:
            {
                if(ioctl(u32_fd, SET_LED02_OFF) != SUCCESS)
                {
                    perror("err: SET_LED02_ON");
                    exit(1);
                }
                break;
            }
			case 6:
            {
			    ft.u32_on = 30;
				ft.u32_off = 40;
                if(ioctl(u32_fd, SET_LED02_FLASH, &ft) != SUCCESS)
                {
                    perror("err: SET_LED02_FLASH");
                    exit(1);
                }
                break;
            }


            case 7:
            {
                if(ioctl(u32_fd, SET_LED03_ON) != SUCCESS)
                {
                    perror("err: SET_LED03_ON");
                    exit(1);
                }
                break;
            }
            case 8:
            {
                if(ioctl(u32_fd, SET_LED03_OFF) != SUCCESS)
                {
                    perror("err: SET_LED03_ON");
                    exit(1);
                }
                break;
            }
			case 9:
            {
			    ft.u32_on = 50;
				ft.u32_off = 60;
                if(ioctl(u32_fd, SET_LED03_FLASH, &ft) != SUCCESS)
                {
                    perror("err: SET_LED03_FLASH");
                    exit(1);
                }
                break;
            }


            default:
            {
                break;
            }
        }

        if(u32_chose == 0 )
        {
            break;
        }
        getchar();
        fflush(stdin);

    }
 
    close(u32_fd);
    return 0;
}


//  http://wenku.baidu.com/view/d061ba283169a4517723a347.html
