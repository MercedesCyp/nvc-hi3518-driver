#ifndef  __PT_RUN_H_
#define  __PT_RUN_H_


#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

#include <linux/interrupt.h>

#include "hi3518_com.h"
#include "pt2803.h"
#include "type.h"


S32 h_run_steps(void);
void h_degree_run(void);
void h_track(void);
void h_self_check(void);
void h_timer_start(void);
void h_timer_stop(void);
S32 h_check_pos(void);
void h_full_step(unsigned int uihdir);
void h_half_step(unsigned int uihdir);


void v_self_check(void);
void v_timer_start(void);
void v_timer_stop(void);
void v_track(void);
S32 v_check_pos(void);
S32 v_run_steps(void);

void v_half_step(unsigned int uivdir);
void v_full_step(unsigned int uivdir);

S32 upmost(void);
S32 downmost(void);
S32 leftmost(void);
S32 rightmost(void);

void timer2_stop(void);
void timer2_start(void);

irqreturn_t timer_2_3_alarm(S32 irq, void *dev_id);
//irqreturn_t timer_2_3_alarm(int irq, void *id, struct pt_regs *r);

#endif  //__PT_RUN_H_


