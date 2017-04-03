


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



#include "hi3518_com.h"
#include "pt2803.h"
#include "pt_run.h"

extern struct pt_dev *pt_devp;


/*
 * functon     : v_check_pos
 * data        : void
 * return      : S32  VTML
 *                    VRGN
 *                    OTHER_POS
 * Description : Check the position now step motor is at, upmost, downmost,
 *               other posotion
 */
//#define V_CHECK_POS
S32 v_check_pos()
{
    return OTHER_POS;





    if(upmost() == 0)
    {
        printk(KERN_INFO"upmost\n");
        return  VTML;
    }
    else
    {
        if(downmost() == 0)
        {
            printk(KERN_INFO"downmost\n");
            return  VRGN;
        }
        else
        {
            return  OTHER_POS;
        }
    }
}

/*
 * functon     : h_check_pos
 * data        : void
 * return      : S32  HTML
 *                    HRGN
 *                    OTHER_POS
 * Description : Check the position now step motor is at, rightmost leftmost
 *               other posotion
 */
//#define H_CHECK_POS
S32 h_check_pos()
{
    return OTHER_POS;







    if(rightmost() == 0)
    {
        printk(KERN_INFO"rightmost\n");
        return  HTML;
    }
    else
    {
        if(leftmost() == 0)
        {
            printk(KERN_INFO"lefttmost\n");
            return  HRGN;
        }
        else
        {
            return OTHER_POS;
        }
    }
}





/*
 * function    : timer2_server
 * data        : void
 * ruturn      : viod
 * description : timer2 interrupt server for step motor running, in a moment
 *               it can run in only one direction.
 */
//#define TIMER2_SERVER
void timer2_server(void)
{   

    //Clear timer2 interrupt
    hi_writel(0xFF, TIMER2_INTCLR);


   // return ;






#ifdef TIMER2_SERVER
    printk(KERN_INFO"Timer2 server.\n");
#endif

    if(pt_devp->ui_h_mode != STAND_BY)
    {
        switch(pt_devp->ui_h_mode)
        {
            case SELF_CHECK:
                {
				    //printk("84\n");
                    h_self_check();
                    break;
                }
            case TRACK:
                {
                 //   h_track();
                    break;
                }
           case STEPS_RUN:
                {
                    printk(KERN_INFO"H sr\n");
                    h_run_steps();
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    if( STAND_BY != pt_devp->ui_v_mode)
    {
	    //printk("80");
        switch(pt_devp->ui_v_mode)
        {
            case SELF_CHECK:
                {
	                //printk("81");
                    v_self_check();
                    break;
                }
            case TRACK:
                {
                    v_track();
                    break;
                }
           case STEPS_RUN:
                {
                    printk(KERN_INFO"v sr\n");
                    v_run_steps();
                    break;
                }
            default:
                {
                    return;
                }
        }
    }
}





/*
 * function    : timer3_alarmirq
 * data        : void
 * ruturn      : viod
 * description : timer3 interrupt server for horizontal runing
 */

//#define TIMER3_ALARMIRQ
void timer3_alarmirq(void)
{
//    static unsigned long int uljiffies_rem = 0;
//    static unsigned int ui_v_step_count = 0;
//    static unsigned int sui_timer3_bgload = TIMER3_BGLOAD_INIT;

    hi_writel(0x0, TIMER3_INTCLR);
#ifdef TIMER3_ALARMIRQ
    printk(KERN_INFO"hs %x\n", pt_devp->ui_h_mode);
#endif
    switch(pt_devp->ui_h_mode)
    {
        case SELF_CHECK:
            {
                h_self_check();
                break;
            }
        case TRACK:
            {
             //   h_track();
                break;
            }
        case DEGREE_RUN:
            {
              //  h_degree_run();
                break;
            }
        case STEPS_RUN:
            {
                h_run_steps();
                break;
            }
    }
}



/*
 * function    : timer_2_3_alarm
 * data        : int irq
 *               void *id
 *               struct pt_regs *r
 * return      : irqreturn_t 
 * description : timer2 timer3 interrupt server. Check timer2  interrupt
 *               had alarmed, and goto specifically function.
 */
//#define TIMER_2_3 1
//
//static irqreturn_t timer2_3_alarm(int irq, void *dev_id, struct pt_regs *regs)
irqreturn_t timer_2_3_alarm(S32 irq, void *dev_id)
{
#ifdef TIMER_2_3
    printk(KERN_INFO"timer2 alarm\n");
#endif
    if((hi_readl(TIMER2_RIS) && 0x01) == 1)
    {
       timer2_server();
#ifdef TIMER_2_3
        printk(KERN_INFO"t2\n");
#endif
    }

    return IRQ_HANDLED;
}





/*
 * functon     : h_run_steps
 * data        : unsigned int ui_steps -------- how many steps to be run
 *               unsigned int ui_dir   -------- direction to be run  DIR_HRGN; DIR_HTML 
 * return      : int                0  -------- sucess
 *                                  1  -------- direction is wrong
 * Description : Runt some steps in horizontal
 */
//#define H_RUN_STEPS 1


/*
 * functon     : h_run_steps
 * data        : unsigned int ui_steps -------- how many steps to be run
 *               unsigned int ui_dir   -------- direction to be run  DIR_HRGN; DIR_HTML 
 * return      : int                0  -------- sucess
 *                                  1  -------- direction is wrong
 * Description : Runt some steps in horizontal
 */
//#define H_RUN_STEPS 1
S32  h_run_steps(void)
{
#ifdef H_RUN_STEPS
    printk(KERN_INFO"hws %d.\n", pt_devp->i_h_will_steps);
#endif
    if((pt_devp->ui_h_will_dir == DIR_HRGN) && (leftmost() == 0))
    {
        pt_devp->i_h_will_steps = 0;
        pt_devp->ui_h_will_dir = DIR_NONE;
        pt_devp->ui_h_pst = HRGN; 
        pt_devp->ui_h_mode = STAND_BY;
        h_timer_stop();
        H_RELEASE();
        wake_up(&pt_devp->wqh_return);
        return 0;
    }
    if((pt_devp->ui_h_will_dir == DIR_HTML) && (rightmost() == 0))
    {
        pt_devp->i_h_will_steps = 0;
        pt_devp->ui_h_will_dir = DIR_NONE;
        pt_devp->ui_h_pst = HTML; 
        pt_devp->ui_h_mode = STAND_BY;
        h_timer_stop();
        H_RELEASE();
        wake_up(&pt_devp->wqh_return);
        return 0;
    }

    h_full_step(pt_devp->ui_h_will_dir);
    if(--pt_devp->i_h_will_steps == 0)
    {
        pt_devp->i_h_will_steps = 0;
        pt_devp->ui_h_will_dir = DIR_NONE;
        pt_devp->ui_h_mode = STAND_BY;
        pt_devp->ui_h_pst = OTHER_POS; 
        h_timer_stop();
        wake_up(&pt_devp->wqh_return);
        H_RELEASE();
    }
    return 0;
}


/* function    : h_degree_run
 * data        : void
 * return      : void
 * Description : Run serval degree in horizontal
 */
void h_degree_run(void)
{
    static unsigned int uihdir = DIR_VTML;
    unsigned long int uldata;

    //printk(KERN_INFO"T3 %d.\n", pt_devp->i_h_will_steps);
    if((pt_devp->ui_h_will_dir == DIR_HRGN) && (leftmost() == 0))
    {
        pt_devp->i_h_will_steps = 0;
        pt_devp->ui_h_will_dir = DIR_NONE;
        uldata = hi_readl(TIMER3_CONTROL);
        clear_bit(7, &uldata);                             // 0x62 = 0101 0010 B disenable
        hi_writel(uldata, TIMER3_CONTROL);
        H_RELEASE();
    }
    if((pt_devp->ui_h_will_dir == DIR_HTML) && (rightmost() == 0))
    {
        pt_devp->i_h_will_steps = 0;
        pt_devp->ui_h_will_dir = DIR_NONE;
        uldata = hi_readl(TIMER3_CONTROL);
        clear_bit(7, &uldata);                             // 0x62 = 0101 0010 B disenable
        hi_writel(uldata, TIMER3_CONTROL);
        H_RELEASE();
    }

    uihdir = pt_devp->ui_h_will_dir;
 

    h_full_step(uihdir);
    if(--pt_devp->i_h_will_steps == 0)
    {
        pt_devp->i_h_will_steps = 0;
        pt_devp->ui_h_will_dir = DIR_NONE;
        uldata = hi_readl(TIMER3_CONTROL);
        clear_bit(7, &uldata);                             // 0x62 = 0101 0010 B disenable
        hi_writel(uldata, TIMER3_CONTROL);
        H_RELEASE();
    }
}




/*
 * function     : step_motor_init
 * data         : (void)
 * return       : (none)
 * description  : Count how many step from upmost to downmost and leftmost
 *                to rightmost
 */

int step_motor_init(void)
{
//    unsigned int uistep_count = 0;
#ifdef DEBUG_STEP_MOTOR_INIT
    printk(KERN_INFO "init_sm_2003 is runing.\n");
#endif


    
    return 0;
}





/*
 * function    : h_track
 * data        : void
 * return      : void
 * Description : Run on one track
 */
void h_track(void)
{
}




/*
 * function    : h_self_check
 * data        : void
 * return      : void
 * description : Vertical slef check.
 */
//#define H_SELF_CHECK 1
void h_self_check(void)
{
    static unsigned int uidir = DIR_HRGN;
    static int si_h_step_count = 0; 
    static int si_h_start_left_steps = 0;
    static int si_h_left_right_steps = 0;

#ifdef H_SELF_CHECK
//    printk(KERN_INFO"hsc.\n");
//    printk(KERN_INFO"hs %x\n", pt_devp->ui_h_start_pos);
#endif

    if(pt_devp->ui_h_start_pos == OTHER_POS)
    {

        if((si_h_start_left_steps != 0) && (uidir == DIR_HRGN)
          && (si_h_step_count >= (si_h_left_right_steps - si_h_start_left_steps)))
        {
            if(STAND_BY == pt_devp->ui_v_mode)
            {
                timer2_stop();
            }
                    
           // printk(KERN_INFO"%d\n",si_h_left_right_steps );
            pt_devp->ui_h_mode = STAND_BY;
            H_RELEASE();
#ifdef H_SELF_CHECK 
         ///   printk(KERN_INFO"Self check end.\n");
#endif
            return;
        }
        else
        {
            if(leftmost() == 0)
            {
                uidir = DIR_HTML; 
                if(si_h_step_count > si_h_start_left_steps)
                { 
                    si_h_start_left_steps = si_h_step_count;
                }
#ifdef H_SELF_CHECK
        //        printk(KERN_INFO"start_left_steps = %d\n", si_h_start_left_steps);
#endif
                si_h_step_count = 0;
            }    
            else
            {
                if(rightmost() == 0)
                {
                    uidir = DIR_HRGN;
                    if(si_h_step_count > si_h_left_right_steps)
                    {
                        si_h_left_right_steps = si_h_step_count;
                    }
                    si_h_step_count = 0;
                }
            }
        }
    }
    else
    {
        if(pt_devp->ui_h_start_pos == HTML)
        {
            if(leftmost() == 0)
            {
                uidir = DIR_HTML;
                if(si_h_step_count > si_h_left_right_steps)
                {
                    si_h_left_right_steps = si_h_step_count;
                }
                si_h_step_count = 0;
            }
            if(rightmost() == 0)
            {
                if(si_h_left_right_steps > si_h_step_count)
                {
                    if(STAND_BY == pt_devp->ui_v_mode)
                    {
                        timer2_stop();
                    }
 
                    pt_devp->ui_h_mode = STAND_BY;
                    H_RELEASE();
                    return ;
                }
                uidir = DIR_HRGN;
                si_h_step_count = 0;
            }
        }
        else
        {
            if(leftmost() == 0)
            {
                uidir = DIR_HTML;
                if(si_h_left_right_steps > si_h_step_count)
                {
                    if(STAND_BY == pt_devp->ui_v_mode)
                    {
                        timer2_stop();
                    }
 
                    pt_devp->ui_h_mode = STAND_BY;
                    H_RELEASE();
                    return;
                }
               si_h_step_count = 0;
            }
            if(rightmost() == 0)
            {
                if(si_h_step_count > si_h_left_right_steps)
                {
                    si_h_left_right_steps = si_h_step_count;
                }
                uidir = DIR_HRGN;
                si_h_step_count = 0;
            }
        }
    }
#ifdef H_SELF_CHECK
    if(0 == (si_h_step_count % 500))
    {
        printk(KERN_INFO"%d\n", si_h_step_count);
    }
#endif
  
   //if the horizontal step motor had been hold up, such as somebody hold it 
   //break up; 
    if(si_h_step_count++ > H_TOTAL_STEPS + 200)
    {
        if(STAND_BY == pt_devp->ui_v_mode)
        {
            timer2_stop();
        }
        pt_devp->ui_h_mode = STAND_BY;
        H_RELEASE();
        //pt_devp->ui_h_mode = SC_ERROR;
    }
    h_full_step(uidir);
}



void timer2_stop()
{
    v_timer_stop();
}

void timer2_start()
{
    v_timer_start();
}

/*
 * function    : h_start
 * data        : void
 * return      : void
 * Description : Start to run in horizontal.
 */
void h_timer_start(void)
{ 
    unsigned long uldata;
    uldata = hi_readl(TIMER3_CONTROL);
    uldata |= 0xE2;                              // 0xE2 = 1110 0010 B enable
    hi_writel(uldata, TIMER3_CONTROL);
}



/*
 * function    : h_timer_stop
 * data        : void
 * return      : void
 * Description : Stop to run in horizontal.
 */
void h_timer_stop(void)
{ 

    unsigned long uldata;
    uldata = hi_readl(TIMER3_CONTROL);
    clear_bit(7, &uldata);
    hi_writel(uldata, TIMER3_CONTROL);

    v_timer_stop();
}






/*
 * function    : v_start
 * data        : void
 * return      : void
 * Description : Vertical start to run 
 */
void v_timer_start(void)
{
    unsigned long int ul_data;
    ul_data = hi_readl(TIMER2_CONTROL);
    set_bit(7, &ul_data);                         
    hi_writel(ul_data, TIMER2_CONTROL);
}



/*
 * function    : v_timer_stop
 * data        : void
 * return      : void
 * Description : Disable timer2
 */
void v_timer_stop(void)
{
    unsigned long int ul_data;
    ul_data = hi_readl(TIMER2_CONTROL);
    clear_bit(7, &ul_data);
    hi_writel(ul_data, TIMER2_CONTROL);
}



/*
 * function    : upmost
 * data        : void
 * return      : int 0   -------- uptmost
 *               int 1   -------- not yet
 * description : check if step motor had run to upmost in the direction of 
 *               vetical.
 */
//#define F_UPMOST 1
int upmost(void)
{
    unsigned long ul_reg;
 
    return 1;



   
   /* 
    local_irq_disable();
    ul_reg = hi_readl(SC_PERCTRL1);
    set_bit(23, &ul_reg);                       //Set io as GPIO7_[0...3]
    hi_writel(ul_reg, SC_PERCTRL1);
    ul_reg = hi_readl(END_DIR);
    clear_bit(U_MOST, &ul_reg);           
    hi_writel(ul_reg, END_DIR);
*/
    ul_reg = hi_readl(UPMOST_P);
    if(ul_reg != 0)
    {
        return 1;
    }
#ifdef F_UPMOST
    printk(KERN_INFO"UP Most.\n");
#endif
 //   local_irq_enable();
    return 0;
}


/*
 * function    : downmost
 * data        : void
 * return      : int 0   -------- downtmost
 *               int 1   -------- not yet
 * description : check if step motor had run to downmost in the direction of 
 *               vetical.
 */
//#define F_DOWNMOST 1
int downmost(void)
{
    unsigned int uireg;

    return 1;




    uireg = hi_readl(DOWNMOST_P);
    if(uireg != 0)
    {
        return 1;
    }
#ifdef F_DOWNMOST
    printk(KERN_INFO"Down Most.\n");
#endif
    return 0;
}



/*
 * function    : rightmost
 * data        : void
 * return      : int 0   -------- rightmost
 *               int 1   -------- not yet
 * description : check if step motor had run to rightmost in the direction of 
 *               horizontal.
 */
//#define F_RIGHTMOST 1
int rightmost(void)
{
    unsigned long  ul_reg;

    return 1;







/*
    local_irq_disable();
    ul_reg = hi_readl(SC_PERCTRL1);
    set_bit(23, &ul_reg);                       //Set io as GPIO7_[0...3]
    hi_writel(ul_reg, SC_PERCTRL1);
    ul_reg = hi_readl(END_DIR);
    clear_bit(R_MOST, &ul_reg);           
    clear_bit(L_MOST, &ul_reg);           
    hi_writel(ul_reg, END_DIR);
*/
 
    ul_reg = hi_readl(RIGHTMOST_P);
    if(ul_reg != 0)
    {
        return 1;
    }
#ifdef F_RIGHTMOST
    printk(KERN_INFO"Right Most.\n");
#endif
 //   local_irq_enable();
    return 0;
}



/*
 * function    : leftmost
 * data        : void
 * return      : int 0   -------- leftmost
 *               int 1   -------- not yet
 * description : check if step motor had run to leftmost in the direction of 
 *               horizontal.
 */
//#define F_LEFTMOST 1
int leftmost(void)
{
    unsigned long  ul_reg;
    return 1;





/*
    local_irq_disable();
    ul_reg = hi_readl(SC_PERCTRL1);
    set_bit(23, &ul_reg);                       //Set io as GPIO7_[0...3]
    hi_writel(ul_reg, SC_PERCTRL1);
    ul_reg = hi_readl(END_DIR);
    clear_bit(R_MOST, &ul_reg);           
    clear_bit(L_MOST, &ul_reg);           
    hi_writel(ul_reg, END_DIR);

*/ 
    ul_reg = hi_readl(LEFTMOST_P);
    if(ul_reg != 0)
    {
        return 1;
    }
#ifdef F_LEFTMOST
    printk(KERN_INFO"Left Most.\n");
#endif
 //   local_irq_enable();
    return 0;
}




/*
 * function    : v_self_check
 * data        : void
 * return      : void
 * description : Vertical slef check.
 */
//#define V_SELF_CHECK 1
void v_self_check(void)
{
    //Remember Runing direction
    static unsigned int uidir = DIR_VTML;

    //Remember steps had run
    static int si_v_step_count = 0; 

    //Remember steps from start to upmost
    static int si_v_start_up_steps = 0;

    //Remember steps from upmost to down most
    static int si_v_up_down_steps = 0;

#ifdef V_SELF_CHECK
//    printk(KERN_INFO"hsc.\n");
    printk(KERN_INFO"vs %x\n", pt_devp->ui_v_start_pos);
#endif

    if(pt_devp->ui_v_start_pos == OTHER_POS)
    {

        if((si_v_start_up_steps != 0) && (uidir == DIR_VTML)
          && (si_v_step_count >= (si_v_up_down_steps - si_v_start_up_steps)))
        {
            if(STAND_BY == pt_devp->ui_h_mode)
            {
                v_timer_stop();
            }
            //printk(KERN_INFO"%d\n",si_v_up_down_steps );
            pt_devp->ui_v_mode = STAND_BY;
            V_RELEASE();
#ifdef V_SELF_CHECK 
            printk(KERN_INFO"Self check end.\n");
#endif
            return;
        }
        else
        {
            if(upmost() == 0)
            {
                uidir = DIR_VRGN; 
                if(si_v_step_count > si_v_start_up_steps)
                { 
                    si_v_start_up_steps = si_v_step_count;
                }
#ifdef V_SELF_CHECK
                printk(KERN_INFO"start_up_steps = %d\n", si_v_start_up_steps);
#endif
                si_v_step_count = 0;
            }    
            else
            {
                if(downmost() == 0)
                {
                    uidir = DIR_VTML;
                    if(si_v_step_count > si_v_up_down_steps)
                    {
                        si_v_up_down_steps = si_v_step_count;
                    }
                    si_v_step_count = 0;
                }
            }
        }
    }
    else
    {
        if(pt_devp->ui_v_start_pos == VRGN)
        {
            if(upmost() == 0)
            {
                uidir = DIR_VRGN;
                if(si_v_step_count > si_v_up_down_steps)
                {
                    si_v_up_down_steps = si_v_step_count;
                }
                si_v_step_count = 0;
            }
            if(downmost() == 0)
            {
                if(si_v_up_down_steps > si_v_step_count)
                {
                    if(STAND_BY == pt_devp->ui_h_mode)
                   {
                       v_timer_stop();
                   }
 
                    pt_devp->ui_v_mode = STAND_BY;
                    V_RELEASE();
                    return ;
                }
                uidir = DIR_VTML;
                si_v_step_count = 0;
            }
        }
        else
        {
            if(upmost() == 0)
            {
                uidir = DIR_VRGN;
                if(si_v_up_down_steps > si_v_step_count)
                {
                   if(STAND_BY == pt_devp->ui_h_mode)
                   {
                       v_timer_stop();
                   }
 
                    pt_devp->ui_v_mode = STAND_BY;
                    V_RELEASE();
                    return;
                }
               si_v_step_count = 0;
            }
            if(downmost() == 0)
            {
                if(si_v_step_count > si_v_up_down_steps)
                {
                    si_v_up_down_steps = si_v_step_count;
                }
                uidir = DIR_VTML;
                si_v_step_count = 0;
            }
        }
    }
#ifdef V_SELF_CHECK
    printk(KERN_INFO"%d\n", si_v_step_count);
#endif
  
   //if the horizontal step motor had been hold up, such as somebody hold it 
   //break up; 
    if(si_v_step_count++ > V_TOTAL_STEPS + 200)
    {
        v_timer_stop();
        pt_devp->ui_v_mode = STAND_BY;
        V_RELEASE();
        pt_devp->ui_v_mode = SC_ERROR;
    }
    v_full_step(uidir);
    //h_full_step(DIR_HTML);
}



/*
 * functon     : v_run_steps
 * data        : void
 * return      : void
 * Description : Runt some steps in vertical
 */
//#define V_RUN_STEPS 1
int  v_run_steps(void)
{
#ifdef V_RUN_STEPS
    printk(KERN_INFO"v_run\n"); 
    printk(KERN_INFO"%x %x\n", pt_devp->ui_v_will_dir, pt_devp->i_v_will_steps); 
#endif
    if((pt_devp->ui_v_will_dir == DIR_VTML) && (upmost() == 0))
    {
        pt_devp->i_v_will_steps = 0;
        pt_devp->ui_v_will_dir = DIR_NONE;
        pt_devp->ui_v_mode = STAND_BY;
        pt_devp->ui_v_pst = VTML; 
#ifdef V_RUN_STEPS
        printk(KERN_INFO"run upmost i_v_pos = %d\n", pt_devp->i_v_pos);
#endif
        v_timer_stop();
        V_RELEASE();
        wake_up(&pt_devp->wqh_return);
        return 0;
    }
    if((pt_devp->ui_v_will_dir == DIR_VRGN) && (downmost() == 0))
    {
        pt_devp->i_v_will_steps = 0;
#ifdef V_RUN_STEPS
        printk(KERN_INFO"run downmost i_v_pos = %d\n", pt_devp->i_v_pos);
#endif
        pt_devp->i_v_pos = 0;
        pt_devp->ui_v_will_dir = DIR_NONE;
        pt_devp->ui_v_mode = STAND_BY;
        pt_devp->ui_v_pst = VRGN; 
        v_timer_stop();
        V_RELEASE();
        wake_up(&pt_devp->wqh_return);
        return 0;
    }

    v_full_step( pt_devp->ui_v_will_dir);
    if(--pt_devp->i_v_will_steps == 0)
    {
        pt_devp->i_v_will_steps = 0;
        pt_devp->ui_v_will_dir = DIR_NONE;
        pt_devp->ui_v_mode = STAND_BY;
        pt_devp->ui_v_pst = OTHER_POS; 
        v_timer_stop();
        V_RELEASE();
        wake_up(&pt_devp->wqh_return);
    }
    return 0;
}


/*
 * function    : v_track
 * data        : void
 * return      : void
 * Description : Run on one track
 */
void v_track(void)
{
    if((pt_devp->ui_v_track_point == 0) &&
            (pt_devp->i_v_pos != 0))
    {
        //Run to origin
        pt_devp->i_v_will_steps = pt_devp->i_v_pos;
        pt_devp->ui_v_will_dir = DIR_VRGN;
        v_timer_start();
    }
}




/*
 * function    : v_half_step
 * data        : unsigned int uivdir
 * return      : void
 * Description : Run half step int vertical direction
 */
void v_half_step(unsigned int uivdir)
{
    unsigned int i = 0;
    i++;
    if(uivdir == DIR_VTML)
    {
        switch(i % 8)
        {
            case 0:
                {
                    V_HALF_STEP0();
                    break;
                }
            case 1:
                {
                    V_HALF_STEP1();
                    break;
                }
            case 2:
                {
                    V_HALF_STEP2();
                    break;
                }
            case 3:
                {
                    V_HALF_STEP3();
                    break;
                }
            case 4:
                {
                    V_HALF_STEP4();
                    break;
                }
    
            case 5:
                {
                    V_HALF_STEP5();
                    break;
                }
    
             case 6:
                {
                    V_HALF_STEP6();
                    break;
                }
           case 7:
                {
                    V_HALF_STEP7();
                    break;
                }
           default:
                {
                    break;
                }
        }
        return ;
    }
    else
    {
        if(uivdir == DIR_VRGN)
        {
             switch(7 - (i % 8))
            {
                case 0:
                    {
                        V_HALF_STEP0();
                        break;
                    }
                case 1:
                    {
                        V_HALF_STEP1();
                        break;
                    }
                case 2:
                    {
                        V_HALF_STEP2();
                        break;
                    }
                case 3:
                    {
                        V_HALF_STEP3();
                        break;
                    }
                case 4:
                    {
                        V_HALF_STEP4();
                        break;
                    }
        
                case 5:
                    {
                        V_HALF_STEP5();
                        break;
                    }
        
                 case 6:
                    {
                        V_HALF_STEP6();
                        break;
                    }
               case 7:
                    {
                        V_HALF_STEP7();
                        break;
                    }
               default:
                    {
                        break;
                    }
        }
        return ;

        }
    }
} 


/*
 * function    : h_half_step
 * data        : unsigned int uihdir
 * return      : void
 * Description : Run half step int horizontal direction
 */
void h_half_step(unsigned int uihdir)
{
    unsigned int i = 0;
    i++;
    printk(KERN_INFO"%d\n", i);
    if(uihdir == DIR_HTML)
    {
        switch(i % 8)
        {
            case 0:
                {
                    H_HALF_STEP0();
                    break;
                }
            case 1:
                {
                    H_HALF_STEP1();
                    break;
                }
            case 2:
                {
                    H_HALF_STEP2();
                    break;
                }
            case 3:
                {
                    H_HALF_STEP3();
                    break;
                }
            case 4:
                {
                    H_HALF_STEP4();
                    break;
                }
    
            case 5:
                {
                    H_HALF_STEP5();
                    break;
                }
    
             case 6:
                {
                    H_HALF_STEP6();
                    break;
                }
           case 7:
                {
                    H_HALF_STEP7();
                    break;
                }
           default:
                {
                    break;
                }
        }
        return ;
    }
    else
    {
        if(uihdir == DIR_HRGN)
        {
             switch(7 - (i % 8))
            {
                case 0:
                    {
                        H_HALF_STEP0();
                        break;
                    }
                case 1:
                    {
                        H_HALF_STEP1();
                        break;
                    }
                case 2:
                    {
                        H_HALF_STEP2();
                        break;
                    }
                case 3:
                    {
                        H_HALF_STEP3();
                        break;
                    }
                case 4:
                    {
                        H_HALF_STEP4();
                        break;
                    }
        
                case 5:
                    {
                        H_HALF_STEP5();
                        break;
                    }
        
                 case 6:
                    {
                        H_HALF_STEP6();
                        break;
                    }
               case 7:
                    {
                        H_HALF_STEP7();
                        break;
                    }
               default:
                    {
                        break;
                    }
        }
        return ;

        }
    }

}





/*
 * function    : h_full_step
 * data        : unsigned int uihdir
 * return      : void
 * Description : Run full step int horizontal direction
 */
void h_full_step(unsigned int uihdir)
{
    static unsigned int i = 0;
    i++;
// printk(KERN_INFO"%d\n", i);
    if(uihdir == DIR_HTML)
    {
        switch(i % 4)
        {
            case 0:
                {
                    H_STEPA();
                    break;
                }
            case 1:
                {
                    H_STEPB();
                    break;
                }
            case 2:
                {
                    H_STEPC();
                    break;
                }
            case 3:
                {
                    H_STEPD();
                    break;
                }
           default:
                {
                    break;
                }
        }
    }
    else
    {
        if(uihdir == DIR_HRGN)
        {
             switch(3 - (i % 4))
            {
                case 0:
                    {
                        H_STEPA();
                        break;
                    }
                case 1:
                    {
                        H_STEPB();
                        break;
                    }
                case 2:
                    {
                        H_STEPC();
                        break;
                    }
                case 3:
                    {
                        H_STEPD();
                        break;
                    }
               default:
                    {
                        break;
                    }
            }
        }
    }
    if(i >= 8)
    {
        i = 0;
    }
}


/*
 * function    : v_full_step
 * data        : unsigned int uihdir
 * return      : void
 * Description : Run full step int vertical direction
 */
//#define V_FULL_STEP
void v_full_step(unsigned int uihdir)
{
    static unsigned int i = 0;
    i++;
#ifdef V_FULL_STEP
    printk(KERN_INFO"VFS %d\n", i);
#endif
    if(uihdir == DIR_VTML)
    {
        switch(i % 4)
        {
            case 0:
                {
                    V_STEPA();
                    break;
                }
            case 1:
                {
                    V_STEPB();
                    break;
                }
            case 2:
                {
                    V_STEPC();
                    break;
                }
            case 3:
                {
                    V_STEPD();
                    break;
                }
           default:
                {
                    break;
                }
        }
    }
    else
    {
        if(uihdir == DIR_VRGN)
        {
             switch(3 - (i % 4))
            {
                case 0:
                    {
                        V_STEPA();
                        break;
                    }
                case 1:
                    {
                        V_STEPB();
                        break;
                    }
                case 2:
                    {
                        V_STEPC();
                        break;
                    }
                case 3:
                    {
                        V_STEPD();
                        break;
                    }
               default:
                    {
                        break;
                    }
            }
        }
    }
    if(i >= 8)
    {
        i = 0;
    }
}
