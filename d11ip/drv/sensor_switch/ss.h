#ifndef  __SS_H_
#define  __SS_H_

#include "type.h"

// For SOC HI3518C and ir-cut IC is AE1511B

#define DAY_2_NIGHT     (0x10)  // SS switch from day to night   
#define NIGHT_2_DAY     (0x11)  // SS switch form night to day 
#define NIGHT_NO_SWITCH (0x12)  // SS switch in the state of night and no switch
#define DAY_NO_SWITCH   (0x13)  // SS switch int the state of day and no switch
#define NO_SWITCH       (0x14)  // SS didn't switch
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
/*
//define each function pin serial number in the GPIO group
#define SS_CTR_SN             (3) // IR-CUT control pin serial number
//#define  DAY_PIN_SN             (2)
//#define  NIGHT_PIN_SN           (0)
//#define  CDS_PIN_SN             (1)
#define LED_CTR_SN             (0)
//#define  POW_PIN_SN             (5)
*/


#define SS_BASE                   GPIO3_BASE
#define SEN_DAY_SN                (1)
#define SEN_NIGHT_SN              (0)

#define SEN_DAY_REG               (SS_BASE + ((1 << SEN_DAY_SN) << 2))
#define SEN_NIGHT_REG             (SS_BASE + ((1 << SEN_NIGHT_SN) << 2))
#define SS_DIR                    (SS_BASE  + 0x400)
#define SS_IS                     (SS_BASE  + 0x404)
#define SS_IBE                    (SS_BASE  + 0x408)
#define SS_IEV                    (SS_BASE  + 0x40C)
#define SS_IE                     (SS_BASE  + 0x410)
#define SS_RIS                    (SS_BASE  + 0x414)
#define SS_MIS                    (SS_BASE  + 0x418)
#define SS_IC                     (SS_BASE  + 0x41C)


#define LED_BASE                 GPIO0_BASE
#define LED_CTR_SN               (0)
#define LED_DIR_REG              (LED_BASE + 0x400)
#define LED_CTR_REG              (LED_BASE + ((1 << LED_CTR_SN) << 2))


/*

#define SS_BASE                 GPIO5_BASE
#define LED_BASE                 GPIO0_BASE

#define SS_DIR_REG              (SS_BASE + 0x400)
#define SS_CTR_REG              (SS_BASE + ((1 << SS_CTR_SN) << 2))

#define LED_DIR_REG              (LED_BASE + 0x400)
#define LED_CTR_REG              (LED_BASE + ((1 << LED_CTR_SN) << 2))
*/

/*
#define  SS_DAY                  (1 << IDAY)
#define  SS_NIGHT                (1 <<INIGHT)
#define  SS_CDS                  (1 << ICDS)
#define  SS_LED                  (1 << ILED)
#define  SS_POW                  (1 << IPOW)
#define  SS_DAYNIGHT_REG         (SS_BASE + ((SS_DAY << 2)|(SS_NIGHT << 2)))
#define  SS_CDS_REG              (SS_BASE + (SS_CDS << 2))
#define  SS_LED_REG              (SS_BASE + (SS_LED << 2))
#define  SS_POW_REG              (SS_BASE + (SS_POW << 2))
#define  SS_FEEDBACK_REG         (SS_BASE + (SS_FEEDBACK << 2))

#define  REC_BUF                  (20)

#define CDS_NIGHT                (IO_REG(SS_CDS_REG) == 0)
#define CDS_DAY                  (IO_REG(SS_CDS_REG) == SS_CDS)
*/

typedef enum{
    PASS_2_END    = 0x1001,
    END_2_PASS    = 0x1002,
} SS_SWITCH;


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



 struct ss_data{
    U32 u32_cds_keep;            // the time CDS change from one state(day or night)
                                 // to another and keep
                                 // after the keep time SS and IR led will change state
                                 // the unit is second
								 
    U16 u16_cds_val;             // the value of CDS, after ADC, 0--1023 

    U32 u32_led_state;           // the state of IR LED: ON or OFF 

    U32 u32_ss_state;           // the model of IR-cut, infrared pass or end

	U32 u32_state;               // state of SS-CUT system now is in, STATE_DAY, STATE_NIGHT

    U32 u32_model;               // the mode SS system is in : MOD_AUTO, 
	                             // MOD_DAY, MOD_NIGHT, MOD_USER
								 
    U32 u32_befor_times;         // this parameter was caculate base one timer and 
	                             // keep time
								 // u32_befor_time =  u32_cds_keep*100 / CHECK_TIME
	U32 u32_md_time;             // this parameter is motion detect
	                             // application will check if there an ir-cut 
								 // switch or IR-led switch
} ;

//define command of ioctl
#define SS_IOC_MAGIC  'i'

//SS switch to day
#define SET_SS_DAY              _IO(SS_IOC_MAGIC, 1) 

//SS switch to night
#define SET_SS_NIGHT            _IO(SS_IOC_MAGIC, 2)

//read state of SS, DAY or night
#define READ_SS_STATE           _IOR(SS_IOC_MAGIC, 3, unsigned int)

//turn IR led on
#define SET_IRLED_ON             _IO(SS_IOC_MAGIC, 4) 

//turn IR led off
#define SET_IRLED_OFF            _IO(SS_IOC_MAGIC, 5)

//read state of IRLED, ON or OFF
#define READ_IRLED_STATE         _IOR(SS_IOC_MAGIC, 6, unsigned int)

//read value of PT
#define READ_CDS_VALUE           _IOR(SS_IOC_MAGIC, 7, unsigned int)

// set delay time, SS switch after CDS level change(second)
#define SET_CDS_KEEP             _IOW(SS_IOC_MAGIC, 8, unsigned int)

// read delay time, SS switch after CDS level change(second)
#define READ_CDS_KEEP            _IOR(SS_IOC_MAGIC, 9, unsigned int) 

// this parameter is for motion detect
// current time is when ioctl command happen
// befor time is base on current
#define SET_MD_TIME              _IOW(SS_IOC_MAGIC, 10, unsigned int)

#define READ_MD_TIME             _IOR(SS_IOC_MAGIC, 11, unsigned int)

// read in a certain period of time if SS switch had happen
// DAY_2_NIGHT, NIGHT_2_DAY, NO_SWITCH
#define READ_MD                  _IOR(SS_IOC_MAGIC, 12, int)

// set SS mode to AUTO
#define SET_SS_MODE_AUTO         _IO(SS_IOC_MAGIC, 13)
#define SET_SS_MODE_DAY          _IO(SS_IOC_MAGIC, 14)
#define SET_SS_MODE_NIGHT        _IO(SS_IOC_MAGIC, 15)
#define READ_SS_MODE             _IOR(SS_IOC_MAGIC,16, unsigned int)
#define SET_SS_MODE_USER         _IO(SS_IOC_MAGIC, 17)

//reset mode of ss, ss delay time, switch befor time
#define SET_SS_MODE_RESET        _IO(SS_IOC_MAGIC, 18) 


#define SET_SEN_DAY_EN           _IO(DRV_IOC_MAGIC, 1)
#define SET_SEN_NIGHT_EN         _IO(DRV_IOC_MAGIC, 2)



#define SS_IOC_MAXNR   19

#endif  /* __SS_H_ */
