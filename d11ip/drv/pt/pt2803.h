#ifndef  __PT_H_
#define  __PT_H_

#include "pt_ioctl.h"
#include "type.h"



//static int sm_threadfun(void *data);
/*
struct track
{
    unsigned int ui_pos[10][1][1]; //Preset point
    unsigned int ui_v_arrive;         //Vertical had arrived preset point
};
*/
struct pt_dev
{
    struct timer_list sm_h_timer;
    unsigned int ui_h_timer_flags;     //Indicate h_timer on or off
    struct timer_list sm_v_timer;
    unsigned int ui_v_timer_flags;     //Indicate v_timer on or off

    struct timer_list sm_track_timer;  //Timer for cruise
    unsigned int ui_track_timer_flags; 
    unsigned int ui_track_num;         //Indecate whick track to be run
    unsigned int ui_v_mode;           //Vertical state: self check, track,
    unsigned int ui_v_track_point;     //Track point in vertical          
    unsigned int ui_v_pst;             //position after running 
                                       //ORIGIN, TERMINAL, OTHER



    //degree run
    unsigned int ui_h_mode;            //Running mode: self check, run degree
    unsigned int ui_h_self_check;      //Indicate self check in horizotal
    unsigned int ui_h_start_pos;       //Indicate h motor start position 
    unsigned int ui_h_total_steps;       //record how many step from leftmost to
             int  i_h_will_steps;      //Record steps will run in horizontal
    unsigned int ui_h_will_dir;        //Direction in horizontal(LEFT or RIGHT)
    unsigned int ui_h_pst;             //position after running 
                                       //ORIGIN, TERMINAL, OTHER

             U32 u32_v_mode;
    unsigned int ui_v_self_check;      //Indicate self check in vertical
    unsigned int ui_v_start_pos;       //Indicate v motor start position upmost 
    unsigned int ui_v_total_steps;       //record how many step from upmost to
             int  i_v_will_steps;      //Record steps will run in vertical
    unsigned int ui_v_will_dir;        //Direction in vertical (vertical)

    int i_v_pos;                      //Record steps from downmost to current 
                                       // position
    int i_h_pos;                      //Record steps from leftmost to current
                                       // position
    wait_queue_head_t wqh_return;

    unsigned int ui_v_arrive;          //Vertical ha arrived
    unsigned int ui_h_arrive;          //Horizontal had arrived
    int i_h_state;                     //

 };

/*
  timer's cycle is C = 2*(TIMER_COUNT_INIT) / 3 * 1000 000 
                                       (ULN2003A + 28BYJ-48)
  C             TIMER_COUNT_INIT       horizontal_cycle  vertical_cycle
  1ms           1500
  2ms           3000
  3ms           4500                   (can't move)
  3.6ms         5500                   11.2S(can move sometimes)
  4ms           6000                   12.2S
  5ms           7500                   15.2S
  6ms           9000                   18.3S 
  10ms          l5000                  30.5S
*/
//#define TIMER2_COUNT_INIT  (13210 * 1)
//#define TIMER2_LOAD_VAL                (5000) //smallest value for hi3518c
#define TIMER2_LOAD_VAL                (9000)
//#define TIMER2_LOAD_VAL                (18000)
#define TIMER2_BGLOAD_INIT             (TIMER2_LOAD_VAL)
#define TIMER3_BGLOAD_INIT             (8000)

#define TIMER2_BGLOAD_LIMIT            (TIMER2_LOAD_VAL)    
#define TIMER3_BGLOAD_LIMIT            (50000)   //if number less than this,
                                                 // step motor can't run 
                                                 //
//Vertical 
#define V_TOTAL_STEPS                   (540)    //Vertical total steps
#define V_TOTAL_DEGREE                  (100)    //Vertical total degree
//Horizontal
#define H_TOTAL_STEPS                   (1370)   //Horizontal total steps
#define H_TOTAL_DEGREE                  (245)    //Horizontal total degree

//#define  

#define ON                             (0x0)
#define OFF                            (0x1)

#define INC_SPEED                      (0x20)   //Increase speed
#define DEC_SPEED                      (0x21)   //Decrease speed
//#define TRUE                           (0x22)
//#define FAIL                           (0x23)
#define VTML                           (0x26)   //Vertical terminal
#define VRGN                           (0x27)   //Vertical origin
#define HRGN                           (0x28)   //Horizontal origin
#define HTML                           (0x29)   //Horizontal terminal
#define OTHER_POS                      (0x2a)

#define YES                            (0x30)
#define NO                             (0x31) 


//Runing mode
#define SELF_CHECK                     (0x40)
#define DEGREE_RUN                     (0x41)
#define TRACK                          (0x42)
#define STEPS_RUN                      (0x43)
#define STAND_BY                       (0x44)  //Don't move
#define SC_ERROR                       (0x45)  //If in this state, don't move 

#define TRACK01                        (0x45)
#define TRACK02                        (0x46)
#define TRACK03                        (0x47)




#define DIR_NONE                       (0x40)  //Direction was no confirm
#define DIR_HTML                       (0x41)  //Direction of horizontalterminal
#define DIR_HRGN                       (0x42)  //Direction of horizontal origin
#define DIR_VTML                       (0x43)  //Direction of vertical terminal
#define DIR_VRGN                       (0x44)  //Direction of Vertical origin



/*
#define SM_BASE           GPIO6_BASE
#define SM_BASE_1         GPIO1_BASE

#define SM_DIR            GPIO6_DIR     
#define SM_IS             GPIO6_IS      

#define SM_DIR_1          GPIO1_DIR     
#define SM_IS_1           GPIO1_IS      



#define VA                (0)
#define VB                (1)
#define VC                (1)
#define VD                (3)


#define HA                (4)   
#define HB                (5)   
#define HC                (6)   
#define HD                (7)   


#define VA_REG            (SM_BASE + (1 << (2 + VA)))
#define VB_REG            (SM_BASE + (1 << (2 + VB)))
//#define VC_REG            (SM_BASE + (1 << (2 + VC)))   //GPIO1_1
#define VC_REG            (SM_BASE_1 + (1 << (2 + VC)))
#define VD_REG            (SM_BASE + (1 << (2 + VD)))

#define HA_REG            (SM_BASE + (1 << (2 + HA)))
#define HB_REG            (SM_BASE + (1 << (2 + HB)))
#define HC_REG            (SM_BASE + (1 << (2 + HC)))
#define HD_REG            (SM_BASE + (1 << (2 + HD)))


#define END_BASE          GPIO0_BASE
#define END_DIR           GPIO0_DIR
//CON607 for end check of horizontal
#define L_MOST           (4)
#define R_MOST           (5)
//CON608 for end check of vertical
#define U_MOST           (2) 
#define D_MOST           (3) 

#define L_MOST_REG      (END_BASE + (1 << (2 + L_MOST)))
#define R_MOST_REG      (END_BASE + (1 << (2 + R_MOST)))
#define U_MOST_REG      (END_BASE + (1 << (2 + U_MOST)))
#define D_MOST_REG      (END_BASE + (1 << (2 + D_MOST)))







//CON607 for end check of horizontal
#define  LEFTMOST          4
#define  RIGHTMOST         5
//CON608 for end check of vertical
#define  UPMOST            2 
#define  DOWNMOST          3 


#define  VA_P               GPIO6_0
#define  VB_P               GPIO6_1
//#define  VC_P               GPIO6_2   //GPIO1_1
#define  VC_P               GPIO1_1
#define  VD_P               GPIO6_3

#define  HA_P               GPIO6_4
#define  HB_P               GPIO6_5   
#define  HC_P               GPIO6_6 
#define  HD_P               GPIO6_7

#define  LEFTMOST_P         GPIO0_4
#define  RIGHTMOST_P        GPIO0_5
#define  UPMOST_P           GPIO0_2
#define  DOWNMOST_P         GPIO0_3 

#define V_PORT (SM_BASE + (((1 << VA) | (1 << VB) | (1 << VD)) << 2))
*/



#define SM_BASE           GPIO4_BASE

#define SM_DIR            GPIO4_DIR     
#define SM_IS             GPIO4_IS      

#define SM_DIR_1          GPIO1_DIR     
#define SM_IS_1           GPIO1_IS      



#define VA                (0)
#define VB                (1)
#define VC                (2)
#define VD                (3)


#define HA                (4)   
#define HB                (5)   
#define HC                (6)   
#define HD                (7)   


#define VA_REG            (SM_BASE + (1 << (2 + VA)))
#define VB_REG            (SM_BASE + (1 << (2 + VB)))
#define VC_REG            (SM_BASE + (1 << (2 + VC)))   
#define VD_REG            (SM_BASE + (1 << (2 + VD)))

#define HA_REG            (SM_BASE + (1 << (2 + HA)))
#define HB_REG            (SM_BASE + (1 << (2 + HB)))
#define HC_REG            (SM_BASE + (1 << (2 + HC)))
#define HD_REG            (SM_BASE + (1 << (2 + HD)))


#define END_BASE          GPIO0_BASE
#define END_DIR           GPIO0_DIR
//CON607 for end check of horizontal
#define L_MOST           (4)
#define R_MOST           (5)
//CON608 for end check of vertical
#define U_MOST           (2) 
#define D_MOST           (3) 

#define L_MOST_REG      (END_BASE + (1 << (2 + L_MOST)))
#define R_MOST_REG      (END_BASE + (1 << (2 + R_MOST)))
#define U_MOST_REG      (END_BASE + (1 << (2 + U_MOST)))
#define D_MOST_REG      (END_BASE + (1 << (2 + D_MOST)))







//CON607 for end check of horizontal
#define  LEFTMOST          4
#define  RIGHTMOST         5
//CON608 for end check of vertical
#define  UPMOST            2 
#define  DOWNMOST          3 


#define  VA_P               GPIO4_0
#define  VB_P               GPIO4_1
#define  VC_P               GPIO4_2
//#define  VC_P               GPIO1_1
#define  VD_P               GPIO4_3

#define  HA_P               GPIO4_4
#define  HB_P               GPIO4_5
#define  HC_P               GPIO4_6
#define  HD_P               GPIO4_7

#define  LEFTMOST_P         GPIO0_4
#define  RIGHTMOST_P        GPIO0_5
#define  UPMOST_P           GPIO0_2
#define  DOWNMOST_P         GPIO0_3 

#define V_PORT (SM_BASE + (((1 << VA) | (1 << VB) | (1 << VD)) << 2))



#define V_STEPA()\
    do{\
	    hi_writel(0x00, VC_REG);\
        hi_writel(((1 << VA) | (1 << VB)), V_PORT);\
    }while(0)


#define V_STEPB()\
    do{\
        hi_writel((1 << VB), V_PORT);\
		hi_writel((1 << VC), VC_REG);\
    }while(0)


#define V_STEPC()\
    do{\
		hi_writel((1 << VD), V_PORT);\
		hi_writel((1 << VC), VC_REG);\
	}while(0)


#define V_STEPD()\
    do{\
	    hi_writel(0x00, VC_REG);\
        hi_writel(((1 << VD) | (1 << VA)), V_PORT);\
    }while(0)




#define H_PORT (SM_BASE + (((1 << HA) | (1 << HB) | (1 << HC) | (1 << HD)) << 2))

#define H_STEPA()\
    do{\
        hi_writel(((1 << HA) | (1 << HB)), H_PORT);\
    }while(0)


#define H_STEPB()\
    do{\
        hi_writel(((1 << HB) | (1 << HC)), H_PORT);\
    }while(0)


#define H_STEPC()\
    do{\
		hi_writel(((1 << HC) | (1 << HD)), H_PORT);\
    }while(0)


#define H_STEPD()\
    do{\
        hi_writel(((1 << HD) | (1 << HA)), H_PORT);\
    }while(0)



/*
#define V_STEPA()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VB) | (1 << VC) | (1 << VD)) << 2));\
        hi_writel(0xFF, VA_P);\
    }while(0)


#define V_STEPB()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VC) | (1 << VD)) << 2));\
        hi_writel(0xFF, VB_P);\
    }while(0)


#define V_STEPC()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VB) | (1 << VD)) << 2));\
        hi_writel(0xFF, VC_P);\
    }while(0)


#define V_STEPD()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VB) | (1 << VC)) << 2));\
        hi_writel(0xFF, VD_P);\
    }while(0)
*/
/*
#define V_PORT (SM_BASE + (((1 << VA) | (1 << VB) | (1 << VC) | (1 << VD)) << 2))

#define V_STEPA()\
    do{\
        hi_writel(((1 << VA) | (1 << VB)), V_PORT);\
    }while(0)


#define V_STEPB()\
    do{\
        hi_writel(((1 << VB) | (1 << VC)), V_PORT);\
    }while(0)


#define V_STEPC()\
    do{\
        hi_writel(((1 << VC) | (1 << VD)), V_PORT);\
    }while(0)


#define V_STEPD()\
    do{\
        hi_writel(((1 << VD) | (1 << VA)), V_PORT);\
    }while(0)
*/




#define H_HALF_STEP0()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HB) | (1 << HC) | (1 << HD)) << 2));\
        hi_writel(0xFF, HA_P);\
    }while(0)

#define H_HALF_STEP1()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HC) | (1 << HD)) << 2));\
        hi_writel(0xFF, HA_P);\
        hi_writel(0xFF, HB_P);\
    }while(0)

#define H_HALF_STEP2()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HA) | (1 << HC) | (1 << HD)) << 2));\
        hi_writel(0xFF, HB_P);\
    }while(0)

#define H_HALF_STEP3()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HB) | (1 << HD)) << 2));\
        hi_writel(0xFF, HB_P);\
        hi_writel(0xFF, HC_P);\
    }while(0)

#define H_HALF_STEP4()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HA) | (1 << HB) | (1 << HD)) << 2));\
        hi_writel(0xFF, HC_P);\
    }while(0)


#define H_HALF_STEP5()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HA) | (1 << HD)) << 2));\
        hi_writel(0xFF, HC_P);\
        hi_writel(0xFF, HD_P);\
    }while(0)


#define H_HALF_STEP6()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HA) | (1 << HB) | (1 << HC)) << 2));\
        hi_writel(0xFF, HD_P);\
    }while(0)

#define H_HALF_STEP7()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << HB) | (1 << HC)) << 2));\
        hi_writel(0xFF, HD_P);\
        hi_writel(0xFF, HA_P);\
    }while(0)


#define V_HALF_STEP0()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VB) | (1 << VC) | (1 << VD)) << 2));\
        hi_writel(0xFF, VA_P);\
    }while(0)

#define V_HALF_STEP1()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VC) | (1 << VD)) << 2));\
        hi_writel(0xFF, VA_P);\
        hi_writel(0xFF, VB_P);\
    }while(0)

#define V_HALF_STEP2()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VC) | (1 << VD)) << 2));\
        hi_writel(0xFF, VB_P);\
    }while(0)

#define V_HALF_STEP3()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VB) | (1 << VD)) << 2));\
        hi_writel(0xFF, VB_P);\
        hi_writel(0xFF, VC_P);\
    }while(0)

#define V_HALF_STEP4()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VB) | (1 << VD)) << 2));\
        hi_writel(0xFF, VC_P);\
    }while(0)


#define V_HALF_STEP5()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VD)) << 2));\
        hi_writel(0xFF, VC_P);\
        hi_writel(0xFF, VD_P);\
    }while(0)


#define V_HALF_STEP6()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VA) | (1 << VB) | (1 << VC)) << 2));\
        hi_writel(0xFF, VD_P);\
    }while(0)

#define V_HALF_STEP7()\
    do{\
        hi_writel(0x0, SM_BASE + (((1 << VB) | (1 << VC)) << 2));\
        hi_writel(0xFF, VD_P);\
        hi_writel(0xFF, VA_P);\
    }while(0)

#define V_RELEASE()\
    do{\
        hi_writel(0x00, SM_BASE + ((( 1 << VA) | (1 << VB) | (1 << VC) | (1 << VD)) << 2));\
    }while(0)


#define H_RELEASE()\
    do{\
        hi_writel(0x00, SM_BASE + ((( 1 << HA) | (1 << HB) | (1 << HC) | (1 << HD)) << 2));\
    }while(0)


#endif  /* __PT_H_ */
