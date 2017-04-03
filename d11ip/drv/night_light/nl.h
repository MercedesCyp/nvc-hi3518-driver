#ifndef  __NIGHT_LIGHT_H_
#define  __NIGHT_LIGHT_H_

#include "hi3518_com.h"

#define NL_BASE        GPIO5_BASE
#define NL_CTR_SN      (1)
#define NL_DIR         (NL_BASE + 0x400)
#define NL_CTR_REG     (NL_BASE + ((1 << NL_CTR_SN) << 2))


//define command of ioctl
#define NL_IOC_MAGIC  'n'

//command of reading the state of night light: ON or OFF
#define NL_READ_STATE   _IOR(NL_IOC_MAGIC, 1, int)

//command of set night light state to ON
#define NL_SET_ON       _IO(NL_IOC_MAGIC, 2)

//command of set night light state to OFF
#define  NL_SET_OFF     _IO(NL_IOC_MAGIC, 3)

#define IRC_IOC_MAXNR   4

//#define ON              (0x00)
//#define OFF             (0x01)

#endif  /* __NIGHT_LIGHT_H_ */


