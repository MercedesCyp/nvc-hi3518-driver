#ifndef  __SENSOR_SELECT_H_
#define  __SENSOR_SELECT_H_

#include "hi3518_com.h"

#define SS_BASE                   GPIO3_BASE
#define SEN_DAY_SN                (1)
#define SEN_NIGHT_SN              (0)

#define SEN_DAY_REG               (SS_BASE + ((1 << SEN_DAY_SN) << 2))
#define SEN_NIGHT_REG             (SS_BASE + ((1 << SEN_NIGHT_SN) << 2))
#define SS_DIR                    (SS_BASE  + 0x400)
#define SS_IS                     (SS_BASE  + 0x404)
#define SS_IBE                    (SS_BASE  + 0x408)
#define SS_IEV                    (SS_BASE  + 0x40C)
#define SS_IE                     (SS_BASE  + 0x410)
#define SS_RIS                    (SS_BASE  + 0x414)
#define SS_MIS                    (SS_BASE  + 0x418)
#define SS_IC                     (SS_BASE  + 0x41C)



//define command of ioctl
#define DRV_IOC_MAGIC  's'

#define SET_S0_EN                _IO(DRV_IOC_MAGIC, 1)
#define SET_S1_EN                _IO(DRV_IOC_MAGIC, 2)
#define SET_SEN_DAY_EN           _IO(DRV_IOC_MAGIC, 3)
#define SET_SEN_NIGHT_EN         _IO(DRV_IOC_MAGIC, 4)


#define DRV_IOC_MAXNR   5

#endif  /* __SENSOR_SELECT_H_ */


