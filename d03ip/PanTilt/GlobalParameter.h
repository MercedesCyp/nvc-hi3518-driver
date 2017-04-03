#ifndef __GlobalParameter_H
#define __GlobalParameter_H



typedef  	char					int8;
typedef  	unsigned char			uint8;
typedef 	short 					int16;
typedef  	unsigned short 			uint16;
typedef  	long 					int32;
typedef  	unsigned long 			uint32;
typedef  	long long 				int64;
typedef 	unsigned long long 		uint64;
/*
	macro
*/
#define SHIFT_LEN_MINOR 	20 
#define PWM_MINOR 	21
#define NTC_MINOR 	22
#define PanTilt_MINOR	23



typedef struct {
	int aOpenTimes;
}mFilePrivdata;


#endif