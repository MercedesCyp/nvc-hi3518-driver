#ifndef __String_H
#define __String_H
//include file
#include "GlobalParameter.h"


extern int16 gfMatchStringFromArray(uint8 **pTar,uint8 *pSou,uint16 iTarLim);

extern uint8 *gfStringCopy(uint8 *iDest,uint8 *iSrc,uint16 ilen);

extern uint8 *gfMemset(uint8 *ioptStr, uint8 ival, uint16 isize);

extern int16 gfStringLen(uint8 *iSrc);

#endif