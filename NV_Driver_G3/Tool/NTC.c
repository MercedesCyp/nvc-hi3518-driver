/*
Coder:      aojie.meng
Date:       2015-9-14

Abstract:
    该 NTC 文件主要提供 10K NTC 电阻的电压转温度算法
    限定 ADC 的检测精度 10bit

*/
//==============================================================================
// C
// Linux
// local
#include "NTC.h"
// remote

//=============================================================================
// MACRO
// CONSTANT
#define DC_NTC_CoeffcientTempOffset -33
// FUNCTION

//==============================================================================
//extern
//local
static int DigitalInsertValue(int bDMin, int bDMax, int NowValue);
static int SeekMaxMinFromArray(int *pMax, int *pMin, int *pNowValue,
                               int bArray[], unsigned int bCount);
//global

//==============================================================================
//extern
//local
static char TempBuf[156] = {
	-30, -29, -28, -27, -26, -25, -24, -23, -22, -21,
	-20, -19, -18, -17, -16, -15, -14, -13, -12, -11,
	-10, -9, -8, -7, -6, -5, -4, -3, -2, -1,
	0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
	61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
	71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
	91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
	101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
	111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
	121, 122, 123, 124, 125
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

static int ADCCodebuf[156] = {
	50, 53, 57, 60, 64, 68, 72, 76, 80, 85,	//-30 -21
	90, 95, 100, 105, 111, 117, 123, 129, 136, 143,	//-20 -11
	150, 157, 165, 172, 181, 189, 197, 206, 215, 224,	//-10  -1
	234,			//0
	243, 253, 263, 273, 284, 294, 305, 316, 327, 338,	//1
	349, 361, 372, 384, 395, 407, 419, 430, 442, 454,	//10
	465, 477, 489, 500, 512, 523, 535, 546, 557, 569,	//20
	580, 590, 601, 612, 622, 632, 642, 652, 662, 672,	//30
	681, 690, 700, 708, 717, 726, 734, 742, 750, 758,	//40
	765, 773, 780, 787, 794, 801, 807, 814, 820, 826,	//50
	832, 837, 843, 848, 854, 859, 864, 868, 873, 877,	//60
	882, 886, 890, 894, 898, 902, 905, 909, 912, 916,	//70
	919, 922, 925, 928, 931, 934, 936, 939, 941, 944,	//80
	946, 948, 951, 953, 955, 957, 959, 961, 963, 966,	//90
	966, 968, 969, 971, 972, 974, 975, 977, 978, 979,	//100
	980, 982, 983, 984, 985, 986, 987, 988, 989, 990,	//110
	991, 992, 993, 994, 995
};

//global

//==============================================================================
//Global
//---------- ---------- ---------- ----------
/*  int ConvertTempFromVoltage(uint16 _Voltage)
@introduction:
    向外部程序提供电压值转换至温度值的接口

@parameter:
    _Voltage
        输入 ADC 采集到得数值，未经转换的电压值

@return:
    返回温度值

*/
int ConvertTempFromVoltage(uint16 _Voltage)
{
	int mADValue;
	int mMax = 0, mMin = 0;
	int Temp = 0;
	int tRet;

	mADValue = _Voltage;
	tRet = SeekMaxMinFromArray(&mMin, &mMax, &mADValue, ADCCodebuf, 156);

	if (tRet) {
		return -274;
	} else {

		if (mMin == mMax) {
			Temp = TempBuf[mMin] * 10;
		} else {
			Temp =
			    TempBuf[mMin] * 10 +
			    DigitalInsertValue(ADCCodebuf[mMin],
			                       ADCCodebuf[mMax], mADValue);
		}

		// Temp-=33;

		Temp += DC_NTC_CoeffcientTempOffset;
		return Temp;
	}
}

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static int DigitalInsertValue (int bDMin,int bDMax,int NowValue)
@introduction:
    一维线性差值

@parameter:
    bDMin
        输入最小值
    bDMax
        输入最大值
    NowValue
        当前想插入的数值

@return:
    返回当前数值在最大最小值之间的权重
    精度为 0.1 （返回得数值为 1）

*/
static int DigitalInsertValue(int bDMin, int bDMax,
                              int NowValue)	//线性一维插值法
{
	int bRange, bWeight, InsertValue;
	bRange = bDMax - bDMin;
	bWeight = NowValue - bDMin;
	InsertValue = (int)((bWeight * 100) / bRange / 10);
	return InsertValue;
}

//---------- ---------- ---------- ----------
/*  static int SeekMaxMinFromArray(int *,int *,int *, int [],unsigned int )
@introduction:
    获取当前电压值在目标数组中的渐进上下界

@parameter:
    pMax
        返回目标数值上届下标
    pMin
        返回目标数值下届下标
    pNowValue
        目标数值
    bArray
        检索线性数组
    bCount
        线性数组大小

@return:
    0       正常返回正确检索值
    -1      目标值超检索数组的下界
    -2      目标值超检索数组的上界
*/
static int SeekMaxMinFromArray(int *pMax, int *pMin, int *pNowValue,
                               int bArray[], unsigned int bCount)
{
	int bCountHead = bCount, bCountTail = 0;
	int tNowValue = *pNowValue;
	if (tNowValue > bArray[bCount - 1]) {
		return -2;
	} else if (tNowValue < bArray[0]) {
		return -1;
	}

	bCount = ((bCountHead + bCountTail) >> 1);
LoopIntSMMFA_Start:
	if (bArray[bCount] > tNowValue) {	/////////////////////////////
		if (bArray[bCount - 1] < tNowValue) {
			*pMax = bCount;
			*pMin = bCount - 1;
			goto LoopIntSMMFA_Stop;
		} else if (bArray[bCount - 1] == tNowValue) {
			*pMax = bCount - 1;
			*pMin = bCount - 1;
			goto LoopIntSMMFA_Stop;
		} else {
			bCountHead = bCount;
			bCount = ((bCountHead + bCountTail) >> 1);
			goto LoopIntSMMFA_Start;
		}
	} else if (bArray[bCount] < tNowValue) {	//////////////////////////
		if (bArray[bCount + 1] > tNowValue) {
			*pMax = bCount + 1;
			*pMin = bCount;
			goto LoopIntSMMFA_Stop;
		} else if (bArray[bCount + 1] == tNowValue) {
			*pMax = bCount + 1;
			*pMin = bCount + 1;
			goto LoopIntSMMFA_Stop;
		} else {
			bCountTail = bCount;
			bCount = ((bCountHead + bCountTail) >> 1);
			goto LoopIntSMMFA_Start;
		}
	} else if (bArray[bCount] == tNowValue) {	/////////////////////////
		*pMax = bCount;
		*pMin = bCount;
		goto LoopIntSMMFA_Stop;
	}
LoopIntSMMFA_Stop:
	return 0;
}
