//////////////////////////////////////////////////////////////////////////
// System Timers
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the System Timers (based on PJ64 setup)

#include "Registers.h"
#include "Timers.h"
#include "Exceptions.h"
#include "Video.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
N64_TIMERS N64SysTimers;

// Init Timer
//////////////////////////////////////////////////////////////////////////
void InitTimer( void )
{
	memset( &N64SysTimers, 0, sizeof(N64SysTimers) );

	N64SysTimers.iSelectedTimer = -1;
	N64SysTimers.iCurrentTime = 0;

	SetTimer( enumN64TimerVi, 5000 ); 
	ChangeCompareTimer();
}

// Set Timer
//////////////////////////////////////////////////////////////////////////
void SetTimer( int iTimer, int iValue )
{
	if( iValue )
	{
		N64SysTimers.iTimerTime[iTimer] = iValue - N64SysTimers.iCurrentTime;
		N64SysTimers.iActive[iTimer] = 1;
		UpdateTimer();
	}
	else
	{
		N64SysTimers.iTimerTime[iTimer] = 0;
		N64SysTimers.iActive[iTimer] = 0; 
	}
}

// Change Compair Timer
//////////////////////////////////////////////////////////////////////////
void ChangeCompareTimer( void )
{
	DWORD NextCompare = N64Regs->COP0[11] - N64Regs->COP0[9];

	if( ( NextCompare & 0x80000000 ) != 0 )
		NextCompare = 0x7FFFFFFF;

	if( NextCompare == 0 )
		NextCompare = 0x1;

	SetTimer( enumN64TimerCompair, NextCompare );	
}

// Update Timers
//////////////////////////////////////////////////////////////////////////
void UpdateTimer( void )
{
	int i;

	for( i = 0; i < enumN64MaxTimers; i++ )
	{
		if( !N64SysTimers.iActive[i] )
			continue;

		if( !( i == enumN64TimerCompair && N64SysTimers.iTimerTime[i] == 0x7FFFFFFF ) )
			N64SysTimers.iTimerTime[i] += N64SysTimers.iCurrentTime;
	}

	N64SysTimers.iSelectedTimer = -1;
	N64SysTimers.iCurrentTime = 0x7FFFFFFF;

	for( i = 0; i < enumN64MaxTimers; i++ )
	{
		if( !N64SysTimers.iActive[i] )
			continue;

		if( N64SysTimers.iTimerTime[i] >= N64SysTimers.iCurrentTime )
			continue;

		N64SysTimers.iCurrentTime = N64SysTimers.iTimerTime[i];
		N64SysTimers.iSelectedTimer = i;
	}

	for( i = 0; i < enumN64MaxTimers; i++ )
	{
		if( !N64SysTimers.iActive[i] )
			continue;

		if( !( i == enumN64TimerCompair && N64SysTimers.iTimerTime[i] == 0x7FFFFFFF ) )
			N64SysTimers.iTimerTime[i] -= N64SysTimers.iCurrentTime;
	}
	
	if( N64SysTimers.iTimerTime[enumN64TimerCompair] == 0x7FFFFFFF )
	{
		DWORD NextCompare = N64Regs->COP0[11] - N64Regs->COP0[9];

		if( ( NextCompare & 0x80000000 ) == 0 && NextCompare != 0x7FFFFFFF ) 
			ChangeCompareTimer();
	}
}

// Check Timers
//////////////////////////////////////////////////////////////////////////
void CheckTimers( void )
{
	if( N64SysTimers.iCurrentTime >= 0 )
		return;

	switch( N64SysTimers.iSelectedTimer )
	{
		case enumN64TimerCompair:
			N64Regs->COP0[32] |= 0x8000;
			CheckInterrupts();
			ChangeCompareTimer();
			break;

		case enumN64TimerSi:
			SetTimer( enumN64TimerSi, 0 );
			N64Regs->MI[2] |= 0x02;
			N64Regs->SI[3] |= 0x1000;
			CheckInterrupts();
			break;

		case enumN64TimerPi:
			SetTimer( enumN64TimerPi, 0 );
			N64Regs->PI[4] &= ~0x01;
			N64Regs->MI[2] |= 0x10;
			CheckInterrupts();
			break;

		case enumN64TimerVi:
			ViRefreshScreen();
			N64Regs->MI[2] |= 0x08;
			CheckInterrupts();
			break;
	}

	UpdateTimer();
}
