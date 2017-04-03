
#ifndef __IRC_TEST_H_
#define __IRC_TEST_H_


//IRC switch to day
#define SET_IRC_DAY_              "set_irc_day"
//IRC switch to night
#define SET_IRC_NIGHT_            "set_irc_night"
//read mode of IRC, DAY or night
#define READ_IRC_STATE_           "read_irc_state"

//turn IR led on
#define SET_IRLED_ON_             "set_irled_on"
//turn IR led off
#define SET_IRLED_OFF_            "set_irled_off"
//read state of IRLED, ON or OFF
#define READ_IRLED_STATE_         "read_irled_state"

//read state of CDS, DAY or NIGHT
#define READ_CDS_STATE_           "read_cds_state"

// set delay time, IRC switch after CDS level change(second)
#define SET_IRC_DELAY_            "set_irc_delay"
// read delay time, IRC switch after CDS level change(second)
#define READ_IRC_DELAY_           "read_irc_delay"

// set 
#define SET_SWITCH_BEFOR_TIME_    "set_switch_befor_time"
// read 
#define READ_SWITCH_BEFOR_TIME_   "read_switch_befor_time"
// read in a certain period of time if IRC switch had happen
// DAY_2_NIGHT, NIGHT_2_DAY, NO_SWITCH
#define READ_SWITCH_BEFOR_        "read_switch_befor"

// set IRC mode to AUTO
#define SET_IRC_MODE_AUTO_        "set_irc_mode_auto"
#define SET_IRC_MODE_DAY_         "set_irc_mode_day"
#define SET_IRC_MODE_NIGHT_       "set_irc_mode_night"
#define SET_IRC_MODE_RESET_       "set_irc_mode_reset"
#define READ_IRC_MODE_            "read_irc_mode"

#define QUIT_                     "quit"
#define Q_                        "q"

#endif
