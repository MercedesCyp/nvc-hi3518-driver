#ifndef __PANTILT_FILEOPTS_H
#define __PANTILT_FILEOPTS_H

#include <linux/miscdevice.h>

#define DC_PanTilt_NAME 	"Pan_Tilt"
#define DRV_VERSION 		"V.0.01"

extern struct miscdevice PanTile_dev;

extern void gfInitSMHIVPort(void);
extern void gfUninitSMHIVPort(void);
#endif