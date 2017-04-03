/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *               test the driver of step motor
 * command    :
 *
 *  Support Hisilicon's chips, as HI3512
 *  qiang.feng<qiang.feng@netviewtech.com>
 *
 * 
 * File name : irc_test.c
 * Author    : qiang.feng
 * Date      : 2012.02.08
 * Version   : 1.00
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
#include <signal.h>

#include "test_irc.h"
#include "../irc.h"
//#include "command.h"

#define DRV_PATH   "/dev/irc"

//maj
#define SET_IRC_USER_PID       _IO(IRC_IOC_MAGIC, 19)
#define DAY_SIGNAL		16
#define NIGHT_SIGNAL	17
static struct sigaction irq1_act;
static struct sigaction irq2_act;

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
    printf("<<1 SET_IRC_DAY                 ");
    printf("<<2 SET_IRC_NIGHT              \n");
    printf("<<3 READ_IRC_STATE              ");
    printf("<<4 SET_IRLED_ON\n");
    printf("<<5 SET_IRLED_OFF               ");
    printf("<<6 READ_IRLED_STATE\n");
    printf("<<7 READ_CDS_VALUE              ");
    printf("<<8 SET_CDS_KEEP\n");
    printf("<<9 READ_CDS_KEEP               ");
    printf("<<10 SET_MD_TIME\n");
    printf("<<11 READ_MD_TIME               ");
    printf("<<12 READ_MD          \n");
    printf("<<13 SET_IRC_MODE_DAY\n");
    printf("<<14 SET_IRC_MODE_NIGHT          ");
    printf("<<15 SET_IRC_MODE_AUTO\n");
    printf("<<16 SET_IRC_MODE_RESET\n");
    printf("<<17 READ_IRC_MODE               ");


    printf("<< quit \n");
    printf("\n");
    printf("please inter the command.\n");
    printf("command :");
         
}
 
void sgSigIrCutB(int signum)
{
	printf("-------------------------------\r\n");
	printf("Is Black!\r\n");
	printf("-------------------------------\r\n");
}
void sgSigIrCutW(int signum)
{
	printf("-------------------------------\r\n");
	printf("Is Wight!\r\n");
	printf("-------------------------------\r\n");
}

int main(int argc, char **argv)
{
    S32 s32_fd;

	U32 u32_chose;
	U32 u32_tmp;
	S32 s32_tmp;

	U16 u16_val;
	
	//maj
	int _pid;
	int ret;
	
	irq1_act.sa_handler = sgSigIrCutB;
	irq1_act.sa_flags = 0;
	sigemptyset(&irq1_act.sa_mask);
	
	irq2_act.sa_handler = sgSigIrCutW;
	irq2_act.sa_flags = 0;
	sigemptyset(&irq2_act.sa_mask);
	
	ret = sigaction(DAY_SIGNAL,&irq1_act,NULL);
	
	if(ret < 0)
		printf("request signal failed!\r\n");
	
	ret = sigaction(NIGHT_SIGNAL,&irq2_act,NULL);
	
	if(ret < 0)
		printf("request signal failed!\r\n");
	
	
    s32_fd = open(DRV_PATH, O_RDWR);
    if(s32_fd < 0)
    {
        printf("err: Can't open %s.\n", DRV_PATH);
        return FAULT;
    }
	
	//maj
	_pid = getpid();
	printf("the PID is :%d\r\n",_pid);
	ioctl(s32_fd,SET_IRC_USER_PID,&_pid);
	
	
    while(1)
    {
        com_head(); 

        scanf("%d", &u32_chose);
        printf("Had input %d\n",u32_chose);

        switch(u32_chose)
        { 
            case 1:
        	// IRC switch to day 
            {
                if(ioctl(s32_fd, SET_IRC_DAY) != 0)
                {
                    perror("ioctl-- SET_IRC_DAY error\n");
                    exit(1);
                }
                break;
            }
            case 2:
        	// IRC switch to night
            {
                if(ioctl(s32_fd, SET_IRC_NIGHT) != 0)
                {
                    perror("ioctl-- SET_IRC_NIGHT error\n");
                    exit(1);
                }
                break;
            }
            case 3:
            {
                if(ioctl(s32_fd, READ_IRC_STATE, &u32_tmp) != 0)
                {
                    perror("ioctl-- READ_IRC_STATE error\n");
                    exit(1);
                }
                if(u32_tmp == IR_END)
                {
                    printf("IRC's state is DAY\n");
                }
                else
                {
                    if(u32_tmp == IR_PASS)
                    {
                        printf("IRC's state is NIGHT\n");
                    }
                    else
                    {
                        printf("Something wrong happend when READ_IRC_STATE\n");
                    }
                }
                break;
            }
            case 4:
            {
                if(ioctl(s32_fd, SET_IRLED_ON) != 0)
                {
                    perror("ioctl-- SET_IRLED_ON error\n");
                    exit(1);
                }
                break;
            }
            case 5:
                {
                    if(ioctl(s32_fd, SET_IRLED_OFF) != 0)
                    {
                        perror("ioctl-- SET_IRLED_OFF error\n");
                        exit(1);
                    }
                    break;
                }
            case 6:
            {
                if(ioctl(s32_fd, READ_IRLED_STATE, &u32_tmp) != 0)
                {
                    perror("ioctl-- READ_IRLED_STATE error\n");
                    exit(1);
                }
    
                if(u32_tmp == ON)
                {
                    printf("IRLED is now ON\n");
                }
                else
                {
                    if(u32_tmp == OFF)
                    {
                        printf("IRLED is now OFF\n");
                    }
                    else
                    {
                        printf("Something wrong happend when READ_IRCLED_STATE\n");
                    }
                }
                break;
            }
            case 7:
            {
                if(ioctl(s32_fd, READ_CDS_VALUE, &u16_val) != 0)
                {
                    perror("ioctl-- READ_CDS_STATE error\n");
                    exit(1);
                }
    
                printf("CDS's value is %u\n", u16_val);
     
                break;
            }
            case 8:
			{
			    scanf("%d", &u32_tmp);
                if(ioctl(s32_fd, SET_CDS_KEEP, &u32_tmp) != 0)
                {
                    perror("ioctl-- SET_CDS_KEEP error");
                    exit(1);
                }
                break;
			}
            case 9: 
            {
                if(ioctl(s32_fd, READ_CDS_KEEP, &u32_tmp) != 0)
                {
                    perror("ioctl-- READ_IRC_DELAY error\n");
                    exit(1);
                }
    
                printf("IRC CDS KEEP time  is %u  second\n", u32_tmp);
     
                break;
            }
    
            case 10:
            {
                scanf("%d", &u32_tmp);
                if(ioctl(s32_fd, SET_MD_TIME, &u32_tmp) != 0)
                {
                    perror("ioctl-- SET_SWITCH_BEFOR_TIME error");
                    exit(1);
                }
                break;
            }
            case 11: 
            {
                if(ioctl(s32_fd, READ_MD_TIME, &u32_tmp) != 0)
                {
                    perror("ioctl-- READ_SWITCH_BEFOR_TIME error\n");
                    exit(1);
                }
    
                printf("switch befor time is %u\n", u32_tmp);
                break;
            }
            case 12:
            {
                if(ioctl(s32_fd, READ_MD, &s32_tmp) != 0)
                {
                    perror("ioctl-- READ_SWITCH_BEFOR error\n");
                    exit(1);
                }

					if(s32_tmp == TRUE)
					{
					    printf("IR-CUT system switch? YES YES\n");
					}
					else
					{
					    printf("IR-CUT system switch? NO NO\n");
					}
   /* 
                switch(u32_tmp)
                {
                    case DAY_2_NIGHT:
                        {
                            printf("IRC switch from day to night\n");
                            break;
                        }
                    case NIGHT_2_DAY:
                        {
                            printf("IRC switch from night to day\n");
                            break;
                        }
                    case DAY_NO_SWITCH:
                        {
                            printf("IRC switch keep in   day   and no switch \n");
                            break;
                        }
                    case NIGHT_NO_SWITCH:
                        {
                            printf("IRC switch keep in   night   and no switch \n");
                            break;
                        }
                    default:
                        {
                            perror("ioctl-- READ_SWITCH_BEFOR  return error\n");
                            printf(" ");
                            break;
                        }
                }
					*/
                break;
            }
            case 13:
            {
                if(ioctl(s32_fd, SET_IRC_MODE_DAY) != 0)
                {
                    perror("ioctl-- SET_IRC_MODE_DAY error\n");
                    exit(1);
                }
                break;
            }
            case 14:
            {
                if(ioctl(s32_fd, SET_IRC_MODE_NIGHT) != 0)
                {
                    perror("ioctl-- SET_IRC_MODE_NIGHT error\n");
                    exit(1);
                }
                break;
            }
     
            case 15:
            {
                if(ioctl(s32_fd, SET_IRC_MODE_AUTO) != 0)
                {
                    perror("ioctl-- SET_IRC_MODE_AUTO error\n");
                    exit(1);
                }
                break;
            }
            case 16:
            {
                if(ioctl(s32_fd, SET_IRC_MODE_RESET) != 0)
                {
                    perror("ioctl-- SET_IRC_MODE_RESET error\n");
                    exit(1);
                }
                break;
            }
            case 17:
            {
                if(ioctl(s32_fd, READ_IRC_MODE, &u32_tmp) != 0)
                {
                    perror("ioctl-- READ_IRC_MODE error\n");
                    exit(1);
                }
    
                switch(u32_tmp)
                {
                    case MOD_DAY:
                        {
                             printf("IRC's mode is DAY\n");
                             break;
                        }
                    case MOD_NIGHT:
                        {
                            printf("IRC's mode is NIGHT\n");
                            break;
                        }
                    case MOD_AUTO:
                        {
                            printf("IRC's mode is auto\n");
                            break;
                        }
                    case MOD_USER:
                        {
                            printf("IRC's mode is user\n");
                            break;
                        }
                    default:
                        {
                            printf("Something wrong happend when READ_IRC_MODE\n");
                            break;
                        }
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
