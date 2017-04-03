/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 * command    :
 *
 *  Support Hisilicon's chips, as HI3518
 *
 * 
 * File name   : test_ss.c
 * Author      : qiang.feng qiang.feng<qiang.feng@netviewtech.com>
 * Creaet Date : 2012.02.08
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

#include "sensor_select.h"
#include "../sensor_select.h"
#include "../type.h"

#define DRV_PATH   "/dev/sensor_select"



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
    printf("<<1 SET_SENSOR00_EN                 ");
    printf("<<2 SET_SENSOR01_EN              \n");

    printf("<< quit \n");
    printf("\n");
    printf("please inter the command.\n");
    printf("command :");
         
}


int main(int argc, char **argv)
{
    S32 s32_fd;

	U32 u32_chose;

    s32_fd = open(DRV_PATH, O_RDWR);
    if(s32_fd < 0)
    {
        printf("err: Can't open %s.\n", DRV_PATH);
        return FAULT;
    }

    while(1)
    {
        com_head(); 

        scanf("%d", &u32_chose);
        printf("Had input %d\n",u32_chose);

        switch(u32_chose)
        { 
            case 1:
            {
                if(ioctl(s32_fd, SET_S0_EN) != 0)
                {
                    perror("ioctl-- SET_S0_EN error\n");
                    exit(1);
                }
                break;
            }
            case 2:
            {
                if(ioctl(s32_fd, SET_S1_EN) != 0)
                {
                    perror("ioctl-- SET_S1_EN error\n");
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
    close(s32_fd);
    return 0;
}
