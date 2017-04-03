#include "NTC.h"


char TempBuf[156]={
    -30,-29,-28,-27,-26,-25,-24,-23,-22,-21,
    -20,-19,-18,-17,-16,-15,-14,-13,-12,-11,
    -10,-9,-8,-7,-6,-5,-4,-3,-2,-1,
    0,
    1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,
    31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,
    51,52,53,54,55,56,57,58,59,60,
    61,62,63,64,65,66,67,68,69,70,
    71,72,73,74,75,76,77,78,79,80,
    81,82,83,84,85,86,87,88,89,90,
    91,92,93,94,95,96,97,98,99,100,
    101,102,103,104,105,106,107,108,109,110,
    111,112,113,114,115,116,117,118,119,120, 
	121,122,123,124,125
};
// int ADCCodebuf[151]=
// {
    // 54, 57, 61, 64, 68, 72, 76, 80, 85, 90,
    // 94,100,105,110,116,111,128,134,141,148,
    // 155,162,169,177,185,193,201,210,219,228,
    // 237,
    // 246,256,266,276,286,296,307,317,328,339,
    // 350,361,372,384,395,406,418,429,441,452,
    // 464,476,487,498,512,521,533,544,555,566,
    // 577,588,599,609,620,630,640,650,660,670,
    // 679,688,698,707,715,724,736,741,749,757,
    // 765,772,779,787,794,800,807,813,820,826,
    // 832,838,843,849,854,859,864,869,873,878,
    // 883,887,891,895,899,903,907,910,914,917,
    // 920,924,927,930,931,935,938,941,943,946,
    // 948,950,952,955,957,959,961,962,964,966,
    // 968,969,971,973,974,976,977,978,980,981,
    // 982,983,985,986,987,988,989,990,991,992, 
// };

int ADCCodebuf[156]=
{
    50, 53, 57, 60, 64, 68, 72, 76, 80, 85,	   //-30 -21
    90,95,100,105,111,117,123,129,136,143,     //-20 -11
    150,157,165,172,181,189,197,206,215,224,   //-10  -1
    234,                                       //0
    243,253,263,273,284,294,305,316,327,338,   //1
    349,361,372,384,395,407,419,430,442,454,   //10
    465,477,489,500,512,523,535,546,557,569,   //20
    580,590,601,612,622,632,642,652,662,672,   //30
    681,690,700,708,717,726,734,742,750,758,   //40
    765,773,780,787,794,801,807,814,820,826,   //50
    832,837,843,848,854,859,864,868,873,877,   //60
    882,886,890,894,898,902,905,909,912,916,   //70
    919,922,925,928,931,934,936,939,941,944,   //80
    946,948,951,953,955,957,959,961,963,966,   //90
    966,968,969,971,972,974,975,977,978,979,   //100
    980,982,983,984,985,986,987,988,989,990,   //110
	991,992,993,994,995
};


static int DigitalInsertValue (int bDMin,int bDMax,int NowValue)//线性一维插值法
{
	int bRange,bWeight,InsertValue;
	bRange=bDMax-bDMin;
	bWeight=NowValue-bDMin;
	InsertValue=(int)((bWeight*100)/bRange/10);
	return InsertValue;
}

static int SeekMaxMinFromArray(int *pMax,int *pMin,int *pNowValue, int bArray[],unsigned int bCount)
{ 
	int bCountHead=bCount,bCountTail=0;

	if(*pNowValue>bArray[bCount-1])
		return 	200;
	else if(*pNowValue<bArray[0])
		return 	-100;

	bCount=((bCountHead+bCountTail)>>1);
LoopIntSMMFA_Start:
	if(bArray[bCount]> *pNowValue)/////////////////////////////
	{
		if(bArray[bCount-1]< *pNowValue)
		{
			*pMax=bCount;
			*pMin=bCount-1;
			goto LoopIntSMMFA_Stop;
		}else if(bArray[bCount-1]== *pNowValue)
		{
			*pMax=bCount-1;
			*pMin=bCount-1;
			goto LoopIntSMMFA_Stop;
		}else
		{
			bCountHead=bCount;
			bCount=((bCountHead+bCountTail)>>1);
			goto LoopIntSMMFA_Start;
		}
	}else if(bArray[bCount]< *pNowValue)//////////////////////////
	{
		if(bArray[bCount+1]> *pNowValue)
		{
			*pMax=bCount+1;
			*pMin=bCount;
			goto LoopIntSMMFA_Stop;
		}else if(bArray[bCount+1]== *pNowValue)
		{
			*pMax=bCount+1;
			*pMin=bCount+1;
			goto LoopIntSMMFA_Stop;
		}else
		{
			bCountTail=bCount;
			bCount=((bCountHead+bCountTail)>>1);
			goto LoopIntSMMFA_Start;
		}
	}else if(bArray[bCount]== *pNowValue)/////////////////////////
	{
		*pMax=bCount;
		*pMin=bCount;
		goto LoopIntSMMFA_Stop;
	}
LoopIntSMMFA_Stop:
	return 1;
}


int ConvertTempFromVoltage(uint16 _Voltage)
{
	int mADValue;
	int mMax=0,mMin=0;
	int Temp=0;
	mADValue = _Voltage;
	if(SeekMaxMinFromArray(&mMin,&mMax,&mADValue,ADCCodebuf,156)) 
	{
		if(mMin==mMax)
		{
			Temp=TempBuf[mMin]*10;
		}else
		{
			Temp=TempBuf[mMin]*10+DigitalInsertValue(ADCCodebuf[mMin],ADCCodebuf[mMax],mADValue);
		}   
	}
	
	Temp-=33;
   
	return Temp;
}



