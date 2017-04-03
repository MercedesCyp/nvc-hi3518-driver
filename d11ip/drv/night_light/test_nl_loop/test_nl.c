/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *              Test ON or OFF for night light
 *  140905
 *         on and off loop in specify
 *
 *  Support Hisilicon's chips, as HI3518
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 * 
 * File name           : test_nl.c
 * Author              : qiang.feng
 * Create data         : 14.05.02
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
#include <arpa/inet.h>
#include <ctype.h>

#include "../nl.h"
#include "../type.h"

#define DRV_PATH      "/dev/nl"



struct nl_dev* nl_devp;


int main(int argc, char ** argv)
{
    int ifd;
    int iret;
    int ichose = 0;
	U32 u32_delay = 0;
 
    U32 u32_nl_state;      //ON or OFF
   
    ifd = open(DRV_PATH, O_RDWR);
    if(ifd < 0)
    {
        printf("Can't open %s.\n", DRV_PATH);
        return -1;
    }

	if(argc != 2)
	{
	    printf("Enter 1 paramete.\n");
		exit(1);
	}

	u32_delay = atoi(argv[1]);

    while(1)
	{

            ioctl(ifd, NL_SET_OFF);
			sleep(u32_delay);
            ioctl(ifd, NL_SET_ON);
			sleep(u32_delay);

	}


    system("clear");
    //input command repeatedly, if input 'q' or "quit" then end the programme
    while(1)
    {
       // iflags = 0;
        //com_head(); 

        scanf("%d", &ichose);
    //    printf("Had input %d\n",ichose);
        switch(ichose)
        { 
            case 1:
            {
               if(( iret = ioctl(ifd, NL_SET_OFF)) != 0)
               {
                   printf(" When set night light off iret = %d\n", iret);
                }
                break;
            }
            case 2:
            {
                if(( iret = ioctl(ifd, NL_SET_ON)) != 0)
                {
                    printf("When set night light ON  iret = %d\n", iret);
                }
                break;
            }
            case 3:
            {
                if(( iret = ioctl(ifd, NL_READ_STATE, &u32_nl_state)) != 0)
                {
                    printf(" iret = %d\n", iret);
                    break;
                }
                else
                {
                    if(u32_nl_state == ON)
                    {
                        printf("Night light is ON\n");
                }
                    if(u32_nl_state == OFF)
                    {
                        printf("Night light is OFF OFF \n");
                    }
                }
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
  
    return 0;
}
