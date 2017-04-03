#ifndef __DIV_MAIN_H
#define __DIV_MAIN_H
// Header file
#include <linux/types.h>
#include <linux/miscdevice.h>

#include "GlobalParameter.h"
#include "PanTilt_FileOpts.h"



// macro
#define DC_DriverNameSize 32

// 
typedef struct{
	uint8 *aDrvName;
	void (*afModeInital)(void);
	void (*afModeUninital)(void);
	struct miscdevice *aDriverInfo;
}mDIV_Regist;





#endif