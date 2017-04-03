#ifndef __GlobalParameter_H
#define __GlobalParameter_H



/*
	macro
	
*/
//system
#define DRV_VERSION "13.11.28 - test"
#define SHIFT_LEN_MINOR 	20 
#define SHIFTLEN_NAME 		"SHITF_LENS"


//action
#define DC_SHIFT_NIGHT	0x0100
#define DC_SHIFT_DAY	0x0200



typedef struct {
	int aOpenTimes;
}mFilePrivdata;




#endif