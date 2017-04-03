#ifndef  __IRC_H_
#define  __IRC_H_

#include "type.h"

// For SOC HI3518C and ir-cut IC is AE1511B

#define DAY_2_NIGHT     (0x10)  // IRC switch from day to night   
#define NIGHT_2_DAY     (0x11)  // IRC switch form night to day 
#define NIGHT_NO_SWITCH (0x12)  // IRC switch in the state of night and no switch
#define DAY_NO_SWITCH   (0x13)  // IRC switch int the state of day and no switch
#define NO_SWITCH       (0x14)  // IRC didn't switch
#define SW_BF_TM        (0x15)  // between this second if there is a 
                                // switch happen. 
/*
#define MOD_NIGHT       (0x20)  // IR-CUT switch to infrared pass, IR LED ON
#define MOD_DAY         (0x21)  // IR-CUT switch to infrared cut-off,IR-LED OFF
#define MOD_AUTO        (0x22)  // IR-CUT's and IR-LED's state depend on
                                // photosensitive diode
#define MOD_USER        (0x23)  // IR-CUT's and IR-LED's state was control by
                                // user
*/

//define each function pin serial number in the GPIO group
#define IRC_CTR_SN             (3) // IR-CUT control pin serial number
//#define  DAY_PIN_SN             (2)
//#define  NIGHT_PIN_SN           (0)
//#define  CDS_PIN_SN             (1)
#define LED_CTR_SN             (0)
//#define  POW_PIN_SN             (5)

#define IRC_BASE                 GPIO5_BASE
#define LED_BASE                 GPIO0_BASE

#define IRC_DIR_REG              (IRC_BASE + 0x400)
#define IRC_CTR_REG              (IRC_BASE + ((1 << IRC_CTR_SN) << 2))

#define LED_DIR_REG              (LED_BASE + 0x400)
#define LED_CTR_REG              (LED_BASE + ((1 << LED_CTR_SN) << 2))

/*
#define  IRC_DAY                  (1 << IDAY)
#define  IRC_NIGHT                (1 <<INIGHT)
#define  IRC_CDS                  (1 << ICDS)
#define  IRC_LED                  (1 << ILED)
#define  IRC_POW                  (1 << IPOW)
#define  IRC_DAYNIGHT_REG         (IRC_BASE + ((IRC_DAY << 2)|(IRC_NIGHT << 2)))
#define  IRC_CDS_REG              (IRC_BASE + (IRC_CDS << 2))
#define  IRC_LED_REG              (IRC_BASE + (IRC_LED << 2))
#define  IRC_POW_REG              (IRC_BASE + (IRC_POW << 2))
#define  IRC_FEEDBACK_REG         (IRC_BASE + (IRC_FEEDBACK << 2))

#define  REC_BUF                  (20)

#define CDS_NIGHT                (IO_REG(IRC_CDS_REG) == 0)
#define CDS_DAY                  (IO_REG(IRC_CDS_REG) == IRC_CDS)
*/

typedef enum{
    PASS_2_END    = 0x1001,
    END_2_PASS    = 0x1002,
} IRC_SWITCH;


typedef enum{
    ON_2_OFF      = 0x1003,
    OFF_2_ON      = 0x1002,
} LED_SWITCH;


typedef enum{
    STATE_DAY     = 0x1004,
    STATE_NIGHT   = 0x1005,
	STATE_USER    = 0x1006,
} STATE_DAY_NIGHT;


typedef enum{
    MOD_DAY       = 0x1010,
    MOD_NIGHT     = 0x1011,
    MOD_AUTO      = 0x1012,
    MOD_USER      = 0x1013,
} MODEL;


typedef enum{
    IMG_MONO      = 0x1020,
    IMG_COLOR     = 0x1021,
} COLOR_MODEL;



 struct irc_data{
    U32 u32_cds_keep;            // the time CDS change from one state(day or night)
                                 // to another and keep
                                 // after the keep time IRC and IR led will change state
                                 // the unit is second
								 
    U16 u16_cds_val;             // the value of CDS, after ADC, 0--1023 

    U32 u32_led_state;           // the state of IR LED: ON or OFF 

    U32 u32_irc_state;           // the model of IR-cut, infrared pass or end

	U32 u32_state;               // state of IRC-CUT system now is in, STATE_DAY, STATE_NIGHT

    U32 u32_model;               // the mode IRC system is in : MOD_AUTO, 
	                             // MOD_DAY, MOD_NIGHT, MOD_USER
								 
    U32 u32_befor_times;         // this parameter was caculate base one timer and 
	                             // keep time
								 // u32_befor_time =  u32_cds_keep*100 / CHECK_TIME
	U32 u32_md_time;             // this parameter is motion detect
	                             // application will check if there an ir-cut 
								 // switch or IR-led switch
} ;

//define command of ioctl
#define IRC_IOC_MAGIC  'i'

//IRC switch to day
#define SET_IRC_DAY              _IO(IRC_IOC_MAGIC, 1) 

//IRC switch to night
#define SET_IRC_NIGHT            _IO(IRC_IOC_MAGIC, 2)

//read state of IRC, DAY or night
#define READ_IRC_STATE           _IOR(IRC_IOC_MAGIC, 3, unsigned int)

//turn IR led on
#define SET_IRLED_ON             _IO(IRC_IOC_MAGIC, 4) 

//turn IR led off
#define SET_IRLED_OFF            _IO(IRC_IOC_MAGIC, 5)

//read state of IRLED, ON or OFF
#define READ_IRLED_STATE         _IOR(IRC_IOC_MAGIC, 6, unsigned int)

//read value of PT
#define READ_CDS_VALUE           _IOR(IRC_IOC_MAGIC, 7, unsigned int)

// set delay time, IRC switch after CDS level change(second)
#define SET_CDS_KEEP             _IOW(IRC_IOC_MAGIC, 8, unsigned int)

// read delay time, IRC switch after CDS level change(second)
#define READ_CDS_KEEP            _IOR(IRC_IOC_MAGIC, 9, unsigned int) 

// this parameter is for motion detect
// current time is when ioctl command happen
// befor time is base on current
#define SET_MD_TIME              _IOW(IRC_IOC_MAGIC, 10, unsigned int)

#define READ_MD_TIME             _IOR(IRC_IOC_MAGIC, 11, unsigned int)

// read in a certain period of time if IRC switch had happen
// DAY_2_NIGHT, NIGHT_2_DAY, NO_SWITCH
#define READ_MD                  _IOR(IRC_IOC_MAGIC, 12, int)

// set IRC mode to AUTO
#define SET_IRC_MODE_AUTO        _IO(IRC_IOC_MAGIC, 13)
#define SET_IRC_MODE_DAY         _IO(IRC_IOC_MAGIC, 14)
#define SET_IRC_MODE_NIGHT       _IO(IRC_IOC_MAGIC, 15)
#define READ_IRC_MODE            _IOR(IRC_IOC_MAGIC,16, unsigned int)
#define SET_IRC_MODE_USER        _IO(IRC_IOC_MAGIC, 17)

//reset mode of irc, irc delay time, switch befor time
#define SET_IRC_MODE_RESET       _IO(IRC_IOC_MAGIC, 18) 

#define IRC_IOC_MAXNR   19

#endif  /* __IRC_H_ */
