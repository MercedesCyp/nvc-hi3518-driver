#ifndef __STATE_LED_H_
#define __STATE_LED_H_


typedef struct flash_time_
{
    unsigned int u32_on;                 // the time that turn on the led, the unit is 10 milisecond.
	unsigned int u32_off;                // the time that turn off the led, the unit is 10 milisecond.
} flash_time;




struct state_led_data
{
    unsigned int u32_led01_on;            // the time that turn on the led01, the unit is 10 milisecond.
    unsigned int u32_led01_off;           // the time that turn off the led01, the unit is 10 milisecond.

    unsigned int u32_led02_on;      
    unsigned int u32_led02_off;    

    unsigned int u32_led03_on;       
    unsigned int u32_led03_off;    
};


//define command of ioctl
#define SL_IOC_MAGIC  's'

//turn off the led01 and the led will off  from now on, unless you chan the state
#define SET_LED01_OFF              _IO(SL_IOC_MAGIC, 21) 

//turn on the led01 and the led will on  from now on, unless you chan the state
#define SET_LED01_ON               _IO(SL_IOC_MAGIC, 22) 

// set led01 to twinkle
#define SET_LED01_FLASH           _IOW(SL_IOC_MAGIC, 40,  flash_time)

//turn off the led02 and the led will off  from now on, unless you chan the state
#define SET_LED02_OFF              _IO(SL_IOC_MAGIC, 24) 

//turn on the led02 and the led will on  from now on, unless you chan the state
#define SET_LED02_ON               _IO(SL_IOC_MAGIC, 25) 

// set led02 to twinkle
#define SET_LED02_FLASH           _IOW(SL_IOC_MAGIC, 26,  flash_time)


//turn off the led03 and the led will off  from now on, unless you chan the state
#define SET_LED03_OFF              _IO(SL_IOC_MAGIC, 27) 

//turn on the led03 and the led will on  from now on, unless you chan the state
#define SET_LED03_ON               _IO(SL_IOC_MAGIC, 28) 

// set led03 to twinkle
#define SET_LED03_FLASH           _IOW(SL_IOC_MAGIC, 29,  flash_time)


#define SL_IOC_MAXNR   50

#endif  //__STATE_LED_H_
