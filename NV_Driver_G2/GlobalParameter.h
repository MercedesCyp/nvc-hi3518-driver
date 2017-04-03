#ifndef __GlobalParameter_H
#define __GlobalParameter_H
// usually header file
#include<linux/kernel.h>
#include <linux/slab.h>

// ================================================================================
// ----------------------------------------> Data Type
// definition
typedef  	char					int8;
typedef  	unsigned char			uint8;
typedef 	short 					int16;
typedef  	unsigned short 			uint16;
typedef  	long 					int32;
typedef  	unsigned long 			uint32;
typedef  	long long 				int64;
typedef 	unsigned long long 		uint64;

#ifndef NULL
#define NULL 0
#endif


// PLATFORM
// PLATFORM_3507
// PLATFORM_3518
typedef struct{
	uint8	*aKey;
	uint32	 aID;
}mArr;
// ================================================================================
// ----------------------------------------> Basic information
#define DC_NVDIV_MINOR		20
#define DC_NVDRIV_NAME 		"NV_Driver"

/*
	status operating macro
*/
#define DF_STATUS_IsStatusExist(_Pool,_bit)	(((_Pool)&(_bit))?1:0)
#define DF_STATUS_SetStatus(_Pool,_bit)		{(_Pool)|=(_bit);}
#define DF_STATUS_ClrStatus(_Pool,_bit)		{(_Pool)&= ~(_bit);}

// ================================================================================
// ----------------------------------------> debug
#define DEBUG_INIT		1
#define DEBUG_MSGCENT	0
#define DEBUG_RWMSG		0
#define DEBUG_PTZ		0
#define DEBUG_FAN		0
#define DEBUG_LDR		0
#define DEBUG_PERIOD	0
#define DEBUG_BUTTON	1
#define DEBUG_DEVELOP	0


#define OPEN_DEBUG	1

#define NVCPrint(fmt, args...) 	\
printk( KERN_INFO "By Driver:\t" fmt "\r\n", ## args)

#define NVCPrint_err(fmt, args...) 	\
printk( KERN_INFO "Error:\t" fmt "\r\n", ## args)

#define NVCPrint_h(fmt, args...) \
printk( KERN_DEBUG "By Driver:" fmt "\r\n", ## args)


#endif
