#ifndef __LinuxAPI_H
#define __LinuxAPI_H
// =============================================================================
// INCLUDE
// C
// Linux
#include <linux/miscdevice.h>
#include <linux/types.h>	// MAJOR MKDEV
#include <linux/fs.h>		// file_operations
#include <asm/uaccess.h>	//access_ok get_put_user
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/sched.h>	//schedule()
#include <linux/module.h>	//ThIS_MODULE
//#include <asm/page.h>//PAGE_SIZE
// local
#include "GlobalParameter.h"
// remote
#include "Tool/Mqueue.h"
#include "Tool/MsgCenter.h"

#endif
