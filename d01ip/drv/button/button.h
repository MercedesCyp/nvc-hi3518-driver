#ifndef  __BUTTON_H_
#define  __BUTTON_H_

#include "hi3518_com.h"

#define BT_BASE                    GPIO0_BASE
#define BT_SN                     (1)

#define BT_REG                    (BT_BASE + ((1 << BT_SN) << 2))
#define BT_DIR                    (BT_BASE  + 0x400)
#define BT_IS                     (BT_BASE  + 0x404)
#define BT_IBE                    (BT_BASE  + 0x408)
#define BT_IEV                    (BT_BASE  + 0x40C)
#define BT_IE                     (BT_BASE  + 0x410)
#define BT_RIS                    (BT_BASE  + 0x414)
#define BT_MIS                    (BT_BASE  + 0x418)
#define BT_IC                     (BT_BASE  + 0x41C)

#endif  /* __BUTTON_H_ */


