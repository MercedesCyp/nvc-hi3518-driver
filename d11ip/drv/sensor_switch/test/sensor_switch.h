#ifndef  __BUTTON_H_
#define  __BUTTON_H_

#nclude "hi3518_com.h"

#define SS_BASE                   GPIO0_BASE
#define S0_SN                     (1)
#define S1_SN                     (4)

#define S0_REG                    (SS_BASE + ((1 << S0_SN) << 2))
#define S1_REG                    (SS_BASE + ((1 << S1_SN) << 2))
#define SS_DIR                    (SS_BASE  + 0x400)
#define SS_IS                     (SS_BASE  + 0x404)
#define SS_IBE                    (SS_BASE  + 0x408)
#define SS_IEV                    (SS_BASE  + 0x40C)
#define SS_IE                     (SS_BASE  + 0x410)
#define SS_RIS                    (SS_BASE  + 0x414)
#define SS_MIS                    (SS_BASE  + 0x418)
#define SS_IC                     (SS_BASE  + 0x41C)



//define command of ioctl
#define DRV_IOC_MAGIC  'S'

#define SET_S0_EN                _IO(DRV_IOC_MAGIC, 1)
#define SET_S1_EN                _IO(DRV_IOC_MAGIC, 2)

#define DRV_IOC_MAXNR   3

#endif  /* __BUTTON_H_ */


