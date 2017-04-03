#ifndef __SM_IRQ_H_
#define __SM_IRQ_H_

#include "sm_ioctl.h"

 irqreturn_t dual_timer1_alarmirq(int irq, void *id, struct pt_regs *r);
 irqreturn_t gpio0_alarmirq(int irq, void *id, struct pt_regs *r);


#endif



