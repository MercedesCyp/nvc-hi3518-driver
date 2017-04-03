#include "GlobalParameter.h"
#include <string.h>
#include <stdio.h>

#define MagicWord	0xCA83

uint8 *sfCreateHeader(
			uint8* iBuf,
			uint16 iCmdtype,
			uint16 iLen,
			uint8 iUnit,
			uint8 iErr){
	
	uint8 *tBuf = iBuf;
	uint16 tMagic = MagicWord;
	
	tBuf = strncpy(tBuf, (uint8*)&tMagic, 	2 );
	tBuf+=2;
	tBuf = strncpy(tBuf, (uint8*)&iCmdtype, 2 );
	tBuf+=2;
	tBuf = strncpy(tBuf, (uint8*)&iLen, 	2 );
	tBuf+=2;
	tBuf = strncpy(tBuf, (uint8*)&iUnit, 	1 );
	tBuf+=1;
	tBuf = strncpy(tBuf, (uint8*)&iErr, 	1 );
	tBuf += 5;
	return tBuf;
}


uint8 tStandChar[]="0123456789ABCDEF ";
void PrintStrHex(uint8 *imsg, uint32 len){
	
	uint8 _i;
	uint8 _Buf[3];
	_Buf[2] = '\0';
	_i = 0;
	while( len ){
		if( _i == 10 ){
			printf("\r\n");
			_i = 0;
		}
		_Buf[0] = tStandChar[(*imsg&0xF0)>>4];
		_Buf[1] = tStandChar[*imsg&0xF];
		
		printf( "%s ",_Buf );

		imsg++;
		_i++;
		len--;
	}
	
	printf("\r\n");
}
