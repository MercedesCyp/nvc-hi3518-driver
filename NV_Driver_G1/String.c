#include "String.h"

int16 gfMatchStringFromArray(uint8 **pTar,uint8 *pSou,uint16 iTarLim)
{
	uint16 theTarLim=0;
	uint8 *ptheTar;
	uint8 *ptheSou;
	
	while(theTarLim<iTarLim){
		ptheSou = pSou;
		ptheTar = pTar[theTarLim];
		while((*ptheTar!='\0')&&(*ptheSou!='\0')){
			if(*ptheTar!=*ptheSou)
				break;
			ptheTar++;
			ptheSou++;
		}
		if((*ptheTar=='\0')&&(*ptheSou=='\0'))
			return theTarLim;
		theTarLim++;
	}
	return -1;
}

uint8 *gfStringCopy(uint8 *iDest,uint8 *iSrc,uint16 ilen)
{
	uint8 *ptSrcEnd=iSrc+ilen;
	while( iSrc<ptSrcEnd )
		*iDest++ = *iSrc++;
	return iDest;
}

uint8 *gfMemset(uint8 *ioptStr, uint8 ival, uint16 isize)
{
	uint8 *ioptend = ioptStr+isize;
	while( ioptStr<ioptend )
		*ioptStr++ = ival;
	
	return ioptStr;
}


int16 gfStringLen(uint8 *iSrc)
{
	int16 _Count = 0;
	while(*iSrc++ != '\0')
		_Count++;
	return _Count;
}

