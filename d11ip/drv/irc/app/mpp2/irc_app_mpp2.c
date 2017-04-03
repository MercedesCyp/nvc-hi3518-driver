#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <unistd.h>

#include "mpi_isp.h"
#include "sample_comm.h"
#include "../../type.h"

#define DRV_PATH               "/dev/irc"
#define MAXFILE                (65536)


ISP_SATURATION_ATTR_S st_a;
U8 u8SatTargetREC = 0;
S32 s32_fd;


/*
 * function     : color_2_monochrome
 * data         : S32 signum
 * return       : void
 * description  : video model change to monochrome
 */
void color_2_monochrome(S32 signum)
{    
    S32 s32_ret = 0;
	S32 i;
    U32 u32_data;

	read(s32_fd, &u32_data, 4);
    printf("SIGIO   %ud\n", u32_data);

	if(u32_data == IR_END)
    {
        for(i = 0; i < 5; i++)
    	{
            s32_ret = HI_MPI_ISP_GetSaturationAttr(&st_a);
    		if(s32_ret == HI_SUCCESS)
    		{
    		    break;
    		}
    		sleep(1);
    	}
    	u8SatTargetREC = st_a.u8SatTarget;
    
        st_a.u8SatTarget = 0;
        for(i = 0; i < 5; i++)
    	{
            HI_MPI_ISP_SetSaturationAttr(&st_a);
    		if(s32_ret == HI_SUCCESS)
    		{
    		    break;
    		}
    		sleep(1);
    	}
	}
	if(u32_data == IR_PASS)
	{
      	for(i = 0; i < 5; i++)
      	{
            s32_ret = HI_MPI_ISP_GetSaturationAttr(&st_a);
      		if(s32_ret == HI_SUCCESS)
      		{
      		    break;
      		}
      		sleep(1);
      	}
      	st_a.u8SatTarget = u8SatTargetREC;
      
        for(i = 0; i < 5; i++)
      	{
           //16HI_MPI_ISP_SetSaturationAttr
           HI_MPI_ISP_SetSaturationAttr(&st_a);
      		if(s32_ret == HI_SUCCESS)
      		{
      		    break;
      		}
      		sleep(1);
      	}
	}
}


int main(int argc, char *argv[])
{
    S32 s32_ret;
	S32 s32_oflags;
	pid_t pid = -1;
	S32 i;

	U32 u32_data;

/*
    //create an new process
	pid = fork();
	if(pid < 0)
	{
	    printf("error: fork\n");
	    return FAULT;
	}
	//father exit
	if(pid > 0)
	{
	    printf("father exit.\n");
	    return 0;
	}

    printf("child say hellow\n");
    pid = setsid();
	if(pid < 0)
	{
	    printf("err: setsid()\n");
		return FAULT;
	}

	chdir("/");
	umask(0);

	for(i = 0; i < MAXFILE; i++)
	{
    	close(i);
	}
*/


	s32_fd = open(DRV_PATH, O_RDWR, S_IRUSR | S_IWUSR);
	if(s32_fd < 0)
	{
	    printf("err: can't open %s\n", DRV_PATH);
		return FAULT;
	}

    s32_ret = HI_MPI_ISP_GetSaturationAttr(&st_a);
    u8SatTargetREC = st_a.u8SatTarget;

	read(s32_fd, &u32_data, 4);
    printf("SIGIO   %ud\n", u32_data);

	if(u32_data == IR_END)
    {
        printf("color\n");
        for(i = 0; i < 5; i++)
    	{
            s32_ret = HI_MPI_ISP_GetSaturationAttr(&st_a);
    		if(s32_ret == HI_SUCCESS)
    		{
    		    break;
    		}
    		sleep(1);
    	}
    	u8SatTargetREC = st_a.u8SatTarget;
    
        st_a.u8SatTarget = 0;
        for(i = 0; i < 5; i++)
    	{
            HI_MPI_ISP_SetSaturationAttr(&st_a);
    		if(s32_ret == HI_SUCCESS)
    		{
    		    break;
    		}
    		sleep(1);
    	}
	}
	if(u32_data == IR_PASS)
	{
      	for(i = 0; i < 5; i++)
      	{
            s32_ret = HI_MPI_ISP_GetSaturationAttr(&st_a);
      		if(s32_ret == HI_SUCCESS)
      		{
      		    break;
      		}
      		sleep(1);
      	}
      	st_a.u8SatTarget = u8SatTargetREC;
      
        for(i = 0; i < 5; i++)
      	{
            HI_MPI_ISP_SetSaturationAttr(&st_a);
      		if(s32_ret == HI_SUCCESS)
      		{
      		    break;
      		}
      		sleep(1);
      	}
	}


	signal(SIGIO, color_2_monochrome);
//	signal(SIGUSR2, monochrome_2_color);
	fcntl(s32_fd, F_SETOWN, getpid());
	s32_oflags = fcntl(s32_fd, F_GETFL);
	fcntl(s32_fd, F_SETFL, s32_oflags | FASYNC);

	printf("I am child.\n");

    while(1)
	{
	    sleep(1000);
	}
    return 0;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

