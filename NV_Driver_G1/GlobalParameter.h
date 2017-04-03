#ifndef __GlobalParameter_H
#define __GlobalParameter_H
typedef  	char					int8;
typedef  	unsigned char			uint8;
typedef 	short 					int16;
typedef  	unsigned short 			uint16;
typedef  	long 					int32;
typedef  	unsigned long 			uint32;
typedef  	long long 				int64;
typedef 	unsigned long long 		uint64;

#define OPEN_DEBUG	1

#define NVCPrint(fmt, args...) 	\
printk( KERN_INFO "By Driver:\t" fmt, ## args)

#define NVCPrint_h(fmt, args...) \
printk( KERN_DEBUG "By Driver:\t" fmt, ## args)

#define Print_h(fmt, args...) \
printk( KERN_DEBUG fmt, ## args)


// usually header file
#include<linux/kernel.h>
/*
	macro
*/
#define DC_NVDIV_MINOR	20
#define DC_NVDRIV_NAME "NV_Driver"

/*
	status operating macro
*/
#define DF_STATUS_IsStatusExist(_Pool,_bit)	(((_Pool)&(_bit))?1:0)
#define DF_STATUS_SetStatus(_Pool,_bit)		{(_Pool)|=(_bit);}
#define DF_STATUS_ClrStatus(_Pool,_bit)		{(_Pool)&= ~(_bit);}

/*
	variable declare
*/
extern uint32 gInsmodProType;


#endif