#include "String.h"



//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> Local Function                             //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
// 	---------->	Linux Interface                                                 //
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		sfMatchString	                                                        //
//	                                                                            //
// 	Param:                                                                      //
// 		pTar    The target String you want compare                              //
//      pSou	that you not clear too much                                     //
//	                                                                            //
// 	Return:                                                                     //
// 		0		The contents of two string is no different/                     //
//		-1		diff                                                            //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
int32 sfMatchString(uint8 *pTar,uint8 *pSou){
	while((*pTar!='\0')&&(*pSou!='\0')){
		if(*pTar!=*pSou)
			break;
		pTar++;
		pSou++;
	}
	
	if((*pTar=='\0')&&(*pSou=='\0'))
		return 0;
	else 
		return -1;
	
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		sfMatchStringFromArray                                                  //
//	                                                                            //
// 	Param:                                                                      //
// 		pTar    The target String you want compare                              //
//      pSou	that you not clear too much                                     //
//		iTarLim	The Array size                                                  //
//	                                                                            //
// 	Return:                                                                     //
//		>=0 	The source string in target position                            //
// 		-1		The target String Array not exist target string                 //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
int32 sfMatchStringFromArray(uint8 **pTar,uint8 *pSou,uint16 iTarLim)
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


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		sfStringCopy		                                                    //
//	                                                                            //
// 	Param:                                                                      //
// 		iDest                                                                   //
//      iSrc                                                                    //
//		ilen                                                                    //
//	                                                                            //
// 	Return:                                                                     //
//		point	return the point to destination address of copy ending          //
// 		                                                                        //
//////////////////////////////////////////////////////////////////////////////////
uint8 *sfStringCopy(uint8 *iDest,uint8 *iSrc,uint16 ilen)
{
	uint8 *ptSrcEnd=iSrc+ilen;
	while( iSrc<ptSrcEnd )
		*iDest++ = *iSrc++;
	return iDest;
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		sfMemset                                                                //
//	                                                                            //
// 	Param:                                                                      //
// 		ioptStr                                                                 //
//      ival	                                                                //
//		isize	                                                                //
//	                                                                            //
// 	Return:                                                                     //
//		point	return the point to destination address of format ending        //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
uint8 *sfMemset(uint8 *ioptStr, uint8 ival, uint16 isize)
{
	uint8 *ioptend = ioptStr+isize;
	while( ioptStr<ioptend )
		*ioptStr++ = ival;
	
	return ioptStr;
}


//////////////////////////////////////////////////////////////////////////////////
// 	Function NAME:                                                              //
// 		sfStringLen                                                             //
//	                                                                            //
// 	Param:                                                                      //
// 		iSrc	                                                                //
//	                                                                            //
// 	Return:                                                                     //
//		>0		string lenght                                                   //
//	                                                                            //
//////////////////////////////////////////////////////////////////////////////////
int32 sfStringLen(uint8 *iSrc)
{
	int32 _Count = 0;
	while(*iSrc++ != '\0')
		_Count++;
	return _Count;
}


//////////////////////////////////////////////////////////////////////////////////
//  -------------------------------> declare global variable                    //
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--              //
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
mSTRING_Fun const gClassStr = {        	                                        //
	sfMatchString,                                                              //
	sfMatchStringFromArray,                                                     //
	sfStringCopy,                                                               //
	sfMemset,                                                                   //
	sfStringLen                                                                 //
};                                                                              //
//////////////////////////////////////////////////////////////////////////////////
