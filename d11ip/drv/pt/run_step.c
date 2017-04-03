
#include "hi3518_com.h"
#include "run_step.h"
#include "type.h"

void h_stepa()
{
   SIZE_T ul_val;

   ul_val = hi_readl(GPIO6_DIR);
   set_bit(4, &ul_val);
   hi_writel(ul_val, GPIO6_DIR);

   hi_writel(0xFF, (GPIO6_BASE + (1 << (2 + 
}
