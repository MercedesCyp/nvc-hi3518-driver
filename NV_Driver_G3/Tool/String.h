#ifndef __String_H
#define __String_H
// =============================================================================
// INCLUDE
// C
// Linux
// local
// remote
#include "../GlobalParameter.h"

//=============================================================================
// DATA TYPE
typedef struct {
	int32(*afMatch)(uint8 *pTar, uint8 *pSou);
	int32(*afMatchArr)(uint8 **pTar, uint8 *pSou, uint16 iTarLim);
	uint8 *(*afCopy)(uint8 *iDest, uint8 *iSrc, uint16 ilen);
	uint8 *(*afMemset)(uint8 *ioptStr, uint8 ival, uint16 isize);
	int32(*afLen)(uint8 *iSrc);
} mSTRING_Fun;

//==============================================================================
//extern
extern mSTRING_Fun const gClassStr;

#endif
