#include <stdio.h>
#include <string.h>
#include "GlobalParameter.h"
#include "sys/fcntl.h"
#include "MsgProtocal.h"

#define Test_str "Test, your success!"

extern uint8 *sfCreateHeader(
			uint8* iBuf,
			uint16 iCmdtype,
			uint16 iLen,
			uint8 iUnit,
			uint8 iErr);
extern void PrintStrHex(uint8 *imsg, uint32 len);




int main(void){
	int tDevhand;
	uint8 tSendBuf[512];
	uint8 tRcvBuf[512];
	
	tDevhand = open(DC_DriverName,O_RDWR);
	
	// write( tDevhand, Test_str, strlen(Test_str)+1 );
	sleep(1);
	
	sfCreateHeader( tSendBuf, 0x0101, 0,0xAA,0xBB );
	
	PrintStrHex(tSendBuf,100  );
	
	
	
	sleep(5);
	close(tDevhand);
	return 0;
}
