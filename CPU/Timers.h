//////////////////////////////////////////////////////////////////////////
// System Timers
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the System Timers (based on PJ64 setup)

#ifndef _TIMERS_H_
#define _TIMERS_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

// Enumerations
//////////////////////////////////////////////////////////////////////////
enum EnumN64Timers
{
	enumN64TimerCompair = 0,
	enumN64TimerSi,
	enumN64TimerPi,
	enumN64TimerVi,
	enumN64MaxTimers
};

// Timer Structure
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int iTimerTime[enumN64MaxTimers];
	int iActive[enumN64MaxTimers];
	int iCurrentTime;
	int iSelectedTimer;
} N64_TIMERS;

// Global Varibles
//////////////////////////////////////////////////////////////////////////
extern N64_TIMERS N64SysTimers;

// Functions
//////////////////////////////////////////////////////////////////////////

//* Init Timer
void InitTimer( void );

//* Set Timer Values & Activate
void SetTimer( int iTimer, int iValue );

//* Change Comapare Timer
void ChangeCompareTimer( void );

//* Update Timer
void UpdateTimer( void );

//* Check Timer
void CheckTimers( void );

#endif
