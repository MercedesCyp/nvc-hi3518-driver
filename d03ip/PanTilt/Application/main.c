#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <ctype.h>


#define IOCMD_NV_TYPE	'p'
#define IOCMD_PT_HClk_AS			_IO(IOCMD_NV_TYPE, 10)
#define IOCMD_PT_HAclk_AS			_IO(IOCMD_NV_TYPE, 11)
#define IOCMD_PT_VUp_AS				_IO(IOCMD_NV_TYPE, 13)
#define IOCMD_PT_VDn_AS				_IO(IOCMD_NV_TYPE, 12) 


#define IOCMD_PT_HClk_Dgr30			_IO(IOCMD_NV_TYPE, 55) 
#define IOCMD_PT_HAclk_Dgr30		_IO(IOCMD_NV_TYPE, 56)
#define IOCMD_PT_VUp_Dgr15			_IO(IOCMD_NV_TYPE, 58)
#define IOCMD_PT_VDn_Dgr15			_IO(IOCMD_NV_TYPE, 57)

#define IOCMD_PT_HClk_SS			_IOW(IOCMD_NV_TYPE, 62, int )
#define IOCMD_PT_HAclk_SS			_IOW(IOCMD_NV_TYPE, 63, int )
#define IOCMD_PT_VUp_SS				_IOW(IOCMD_NV_TYPE, 65, int )
#define IOCMD_PT_VDn_SS				_IOW(IOCMD_NV_TYPE, 64, int )


#define IOCMD_PT_HClk_DgrS			_IOW(IOCMD_NV_TYPE, 66, int )
#define IOCMD_PT_HAclk_DgrS			_IOW(IOCMD_NV_TYPE, 67, int )
#define IOCMD_PT_VUp_DgrS			_IOW(IOCMD_NV_TYPE, 69, int )
#define IOCMD_PT_VDn_DgrS			_IOW(IOCMD_NV_TYPE, 68, int )

int main(void)
{
	int theDivHandle;
	int theRetVal;
	int theSteps=20;
	//printf("Hello world!\r\n");
	theDivHandle = open("/dev/Pan_Tilt",O_RDWR);
	
	printf("%x\r\n",IOCMD_PT_HClk_SS);
	theRetVal = ioctl(theDivHandle, IOCMD_PT_HAclk_DgrS,&theSteps);
	
	//printf("the ioctl return is: %d \r\n",theRetVal);
	
	close(theDivHandle);
	return 0;
}