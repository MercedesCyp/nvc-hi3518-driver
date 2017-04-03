#ifndef __GlobalParameter_H
#define __GlobalParameter_H
// =============================================================================
// INCLUDE
// C
// Linux
#ifdef COMPILER_DRIVER
#include<linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#endif
// local
// remote

//==============================================================================
//extern
extern void DbgPrinStr(unsigned char *, unsigned char *, unsigned short);

// =============================================================================
// DATA TYPE
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef long long int64;
typedef unsigned long long uint64;

typedef struct {
	uint8 *aKey;
	uint32 aID;
} mArr;

// ----------------------------------------> Time Struct
typedef struct {
	uint8 aSecond;
	uint8 aMinute;
	uint8 aHour;
	uint8 aDay;
	uint8 aWeekday;
	uint8 aMonth;
	uint8 aYear;
} mTime_Struct;

//=============================================================================
// MACRO
// CONSTANT
#define DC_NVDIV_MINOR		20
#define DC_NVDRIV_NAME 		"NV_Driver"
#define DC_NVDRIV_VERSION   	"V: 01.13"
#define DC_NVDRIV_BUILD     	__DATE__

//=============================================================================
//PLATFORM      defined in Makefile
//#define PLATFORM_3507
//#define PLATFORM_3518
//#define PLATFORM_3518EV200
//=============================================================================

#ifndef NULL
#define NULL 0
#endif

#define BIT0     (0x00000001)
#define BIT1     (0x00000002)
#define BIT2     (0x00000004)
#define BIT3     (0x00000008)
#define BIT4     (0x00000010)
#define BIT5     (0x00000020)
#define BIT6     (0x00000040)
#define BIT7     (0x00000080)
#define BIT8     (0x00000100)
#define BIT9     (0x00000200)
#define BIT10    (0x00000400)
#define BIT11    (0x00000800)
#define BIT12    (0x00001000)
#define BIT13    (0x00002000)
#define BIT14    (0x00004000)
#define BIT15    (0x00008000)
#define BIT16    (0x00010000)
#define BIT17    (0x00020000)
#define BIT18    (0x00040000)
#define BIT19    (0x00080000)
#define BIT20    (0x00100000)
#define BIT21    (0x00200000)
#define BIT22    (0x00400000)
#define BIT23    (0x00800000)
#define BIT24    (0x01000000)
#define BIT25    (0x02000000)
#define BIT26    (0x04000000)
#define BIT27    (0x08000000)
#define BIT28    (0x10000000)
#define BIT29    (0x20000000)
#define BIT30    (0x40000000)
#define BIT31    (0x80000000)
// IIC protocol
#define DC_IIC_READ     0x01
#define DC_IIC_WRITE    0x00

// FUNCTION
#define DF_STATUS_IsStatusExist(_Pool,_bit)	(((_Pool)&(_bit))?1:0)
#define DF_STATUS_SetStatus(_Pool,_bit)		{(_Pool)|=(_bit);}
#define DF_STATUS_ClrStatus(_Pool,_bit)		{(_Pool)&= ~(_bit);}

// =============================================================================
// ----------------------------------------> debug
#define DEBUG_INIT	0
#define DEBUG_LAPI      0	//Linux API
#define DEBUG_MSGCENT	0
#define DEBUG_RWMSG	0
#define DEBUG_PTZ	0
#define DEBUG_FAN	0
#define DEBUG_LDR	0
#define DEBUG_PERIOD	0
#define DEBUG_BUTTON	0
#define DEBUG_DEVELOP	0
#define DEBUG_PCF8563   0
#define DEBUG_PIR       0
#define DEBUG_IRC	0
#define DEBUG_IFRLIGHT	0
#define DEBUG_FM1288	0

// belong to test application
#define NONE         "\e[m"
#define RED          "\e[0;32;31m"
#define LIGHT_RED    "\e[1;31m"
#define GREEN        "\e[0;32;32m"
#define LIGHT_GREEN  "\e[1;32m"
#define BLUE         "\e[0;32;34m"
#define LIGHT_BLUE   "\e[1;34m"
#define DARY_GRAY    "\e[1;30m"
#define CYAN         "\e[0;36m"
#define LIGHT_CYAN   "\e[1;36m"
#define PURPLE       "\e[0;35m"
#define LIGHT_PURPLE "\e[1;35m"
#define BROWN        "\e[0;33m"
#define YELLOW       "\e[1;33m"
#define LIGHT_GRAY   "\e[0;37m"
#define WHITE        "\e[1;37m"

// Driver
#define NVCPrint(fmt, args...) 	\
	printk( KERN_INFO "By Driver:\t"LIGHT_GREEN fmt  NONE"\r\n", ## args)

#define NVCPrint_err(fmt, args...) 	\
	printk( KERN_INFO "Error:\t"LIGHT_RED fmt NONE"\r\n", ## args)

#define NVCPrint_h(fmt, args...) \
	printk( KERN_DEBUG "By Driver:"YELLOW fmt NONE"\r\n", ## args)

// APP
#define APPPrint(fmt, args...) \
	printf( LIGHT_PURPLE"APP\t"NONE         fmt "\r\n", ## args)

#define APPPrint_r(fmt, args...) \
	printf( LIGHT_PURPLE"APP\t"LIGHT_RED    fmt NONE"\r\n", ## args)

#define APPPrint_g(fmt, args...) \
	printf( LIGHT_PURPLE"APP\t"LIGHT_GREEN  fmt NONE"\r\n", ## args)

#define APPPrint_b(fmt, args...) \
	printf( LIGHT_PURPLE"APP\t"LIGHT_BLUE   fmt NONE"\r\n", ## args)

#define APPPrint_y(fmt, args...) \
	printf( LIGHT_PURPLE"APP\t"YELLOW       fmt NONE"\r\n", ## args)

#endif
