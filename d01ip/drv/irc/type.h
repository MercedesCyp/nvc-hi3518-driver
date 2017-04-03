
#ifndef __TYPE_H__
#define __TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           U8;
typedef unsigned char           UCHAR;
typedef unsigned short          U16;
typedef unsigned int            U32;

typedef signed char             S8;
typedef short                   S16;
typedef int                     S32;
#ifndef _M_IX86
typedef unsigned long long      U64;
typedef long long               S64;
#else
typedef __int64                 U64;
typedef __int64                 S64;
#endif

typedef char                    CHAR;
typedef char*                   PCHAR;

typedef float                   FLOAT;
typedef double                  DOUBLE;
typedef void                    VOID;

typedef unsigned long           SIZE_T;
typedef unsigned long           LENGTH_T;


/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    FALSE             = -1,
    TRUE              = 0,
} BOOL;


typedef enum {
    SUCCESS           = 0,
    FAULT             = -1,
} RESULT;


typedef enum{
  ACK                 = 0,     // send ack after receive byte
  NOACK               = 1      // send no ack after  
}GPIO_I2C_ACK;


typedef enum{
    SENSOR_UNKNOW     = 0,
    OV7725            = 7725,
    OV2643            = 2643,
    OV9712            = 9712,
} NVT_SENSOR_TYPE;


typedef enum{
    OV7725_DEV_ADD    = 0x42,
    OV2643_DEV_ADD    = 0x60,
	OV9712_DEV_ADD    = 0x60,
} SENSOR_DEV_ADDRESS;


typedef enum{
	OV9712_PIDH      = 0x97,
	OV9712_PIDL      = 0x11,
} NVT_SENSOR_ID;


typedef enum{
	OV9712_PIDH_ADD  = 0x0A,
	OV9712_PIDL_ADD  = 0x0B,
} SENSOR_ID_ADDRESS;



typedef enum{
    ON               = 0,
    OFF              = 1,
} SWITCH;


typedef enum{
    LOW              = 0,
    HIGHT            = 1,
} VOTAGE_LEVEL;


typedef enum{
    IR_PASS          = 0x0010,
    IR_END           = 0x0011,
} IR_CUT;




//#define OV7725_ADDR     (0x42)
//#define OV2643_ADDR     (0x60)


#ifndef NULL
#define NULL             0L
#endif

#define NULL_PTR         0L

//#define SUCCESS          0
//#define FAILURE          (-1)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TYPE_H__ */

