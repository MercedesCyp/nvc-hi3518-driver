#ifndef    __PT_IOCTL_H_
#define    __PT_IOCTL_H_

//define command of ioctl
#define PT_IOC_MAGIC  'p'

//Run one step to left
#define HRGN_ONE_STEP               _IO(PT_IOC_MAGIC, 10) 

//Run one step to  right
#define HTML_ONE_STEP               _IO(PT_IOC_MAGIC, 11) 

//Run one step to up
#define VTML_ONE_STEP               _IO(PT_IOC_MAGIC, 12) 

//Run one step to down
#define VRGN_ONE_STEP               _IO(PT_IOC_MAGIC, 13) 

//Run 30 degree to left
#define HRGN_30_DEG                 _IO(PT_IOC_MAGIC, 55) 
#define HTML_30_DEG                 _IO(PT_IOC_MAGIC, 56) 
#define VTML_15_DEG                 _IO(PT_IOC_MAGIC, 57) 
#define VRGN_15_DEG                 _IO(PT_IOC_MAGIC, 58) 



#define TRACK_01                    _IO(PT_IOC_MAGIC, 59) 
#define TRACK_02                    _IO(PT_IOC_MAGIC, 60) 
#define TRACK_03                    _IO(PT_IOC_MAGIC, 61) 

//Run  some steps
#define RUN_HRGN_STEPS              _IOW(PT_IOC_MAGIC, 62, int ) 
#define RUN_HTML_STEPS              _IOW(PT_IOC_MAGIC, 63, int ) 
#define RUN_VTML_STEPS              _IOW(PT_IOC_MAGIC, 64, int ) 
#define RUN_VRGN_STEPS              _IOW(PT_IOC_MAGIC, 65, int ) 

//Run  some degree
#define RUN_HRGN_DEGREE             _IOW(PT_IOC_MAGIC, 66, int ) 
#define RUN_HTML_DEGREE             _IOW(PT_IOC_MAGIC, 67, int ) 
#define RUN_VTML_DEGREE             _IOW(PT_IOC_MAGIC, 68, int ) 
#define RUN_VRGN_DEGREE             _IOW(PT_IOC_MAGIC, 69, int ) 

#define PT_IOC_MAXNR   70



//ioctl return 
typedef enum {
    PT_FAULT    = -1,   //Fault for reason unkonw
    PT_SUCCESS  = 0,    //Success
    PT_DEGREE   = 1,    //Drgree is wrong (> 275, < 0)
    PT_STEPS    = 3,    //Steps is wrong  (> 1500, < 0)
    PT_CMD      = 4,    //No such command 
    PT_HRGN     = 5,    //Horizontal had run to origin
    PT_HTML     = 6,    //Horizontal had run to terminal
    PT_VRGN     = 7,    //Vertical had run to origin
    PT_VTML     = 8,    //Vertical had run to terminal
} PT_IOCTL_RETURN;



#define IOCTL_SUCCESS              0
#define IOCTL_H_BROKEN             1

#endif     //__PT_IOCTL_H_
