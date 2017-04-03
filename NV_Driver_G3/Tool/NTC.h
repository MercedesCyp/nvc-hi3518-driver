#ifndef __NTC_H
#define __NTC_H
// =============================================================================
// INCLUDE
// C
// Linux
// local
// remote
#include "../GlobalParameter.h"

//=============================================================================
// MACRO
#define DC_TEMPERATURE_fAbsoluteZero    (-273.15)
#define DC_TEMPERATURE_iAbsoluteZero    (-273)
// CONSTANT

//==============================================================================
//extern
extern int ConvertTempFromVoltage(uint16 _Voltage);

#endif
