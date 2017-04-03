#ifndef __Master_H
#define __Master_H
#include "GlobalParameter.h"


//  -------------------------------> Include file
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
//internal
#include <linux/miscdevice.h>
#include <linux/types.h> // MAJOR MKDEV
#include <linux/fs.h>// file_operations 
#include <asm/uaccess.h>//access_ok get_put_user
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/sched.h> //schedule()
//#include <linux/module.h>//ThIS_MODULE
//#include <asm/page.h>//PAGE_SIZE
//external
#include "Mqueue.h"
#include "MsgCenter.h"

//  -------------------------------> Macro
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--



//  -------------------------------> The external function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
//extern struct miscdevice NV_dev;

extern int gfInit_NV_Driver(void);
extern void gfUninit_NV_Driver(void);

#endif
