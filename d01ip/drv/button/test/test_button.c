/*
 * Copyright (c) 2011 Netviewtech Co.,Ltd.
 *
 * Description:
 *              This program is test for button.
 *
 *  Support Hisilicon's chips, as HI3518
 * 
 * File name : test_button.c
 * Author    : qiang.feng
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
#include <time.h>
#include <signal.h>

#include "../type.h"

#define DRV_PATH      "/dev/button"
#define CREATE_FILE   "create"

#define SLEEP_TIME    (2)
#define TEST_TIMES    ((60 * 60 * 24) / SLEEP_TIME)

/*
 * function     : com_head
 * data         : void
 * return       : void
 * description  : output the head of command 
 */
void com_head(int signum)
{
    S32 s32_fd;
    static S32 i = 0;
    char c[50];
    int wr;

     
    s32_fd = open(CREATE_FILE, O_RDWR | O_CREAT);
    if(s32_fd < 0)
    {
        printf("Can't open %s.\n", CREATE_FILE);
    }

    i++;
    sprintf(c, "%d\n", i);
    c[2] = '\0';
    wr = write(s32_fd, c, strlen(c)); 
    printf(" %d %s\n", wr, c);
    printf("Button had been push times: %d \n", i);
    close(s32_fd);        
}




int main(int argc, char ** argv)
{
    S32 s32_btn_fd;
    S32 s32_oflags;
    

    //Get system time
    time_t now;
    struct tm *time_now;

    time(&now);
    time_now = localtime(&now);
    printf("Local times is %s\n", asctime(time_now));


    s32_btn_fd = open(DRV_PATH, O_RDWR, S_IRUSR | S_IWUSR);
    if(s32_btn_fd < 0)
    {
        printf("Can't open %s.\n", DRV_PATH);
        return -1;
    }

    signal(SIGIO, com_head);
    fcntl(s32_btn_fd, F_SETOWN, getpid());
    s32_oflags = fcntl(s32_btn_fd, F_GETFL);
    fcntl(s32_btn_fd, F_SETFL, s32_oflags | FASYNC);

    while(1)
    {
       sleep(1000);
     }
    getchar();
    close(s32_btn_fd);
    return 0;
}

