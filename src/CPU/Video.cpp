//////////////////////////////////////////////////////////////////////////
// Video
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Video Emulation

#include "Registers.h"
#include "Memory.h"
#include "Timers.h"
#include "Exceptions.h"
#include "Video.h"
#include "../Video_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
unsigned int ViScreenSize[2] = { 320, 240 };
float ViScreenHalf[2] = { 320.0f/2.0f, 240.0f/2.0f };
int ViFieldNumber = 0;
int iViewportChanged = 0;
DWORD ViHalfLine;

extern void RunRdp ( void );

// VI Refresh Display
//////////////////////////////////////////////////////////////////////////
void ViRefreshScreen (void )
{ 
	static DWORD OLD_VI_V_SYNC_REG = 0, VI_INTR_TIME = 500000;
	int iNewViTimer;

	if( OLD_VI_V_SYNC_REG != N64Regs->VI[6] ) 
	{
		if( N64Regs->VI[6] == 0 ) 
		{
			VI_INTR_TIME = 500000;
		} 
		else 
		{
			VI_INTR_TIME = ( N64Regs->VI[6] + 1 ) * 1500;

			if( ( N64Regs->VI[6] % 1 ) != 0 )
				VI_INTR_TIME -= 38;
		}
	}

	RenderFrame();

	iNewViTimer = N64SysTimers.iCurrentTime + N64SysTimers.iTimerTime[enumN64TimerVi] + VI_INTR_TIME;

	SetTimer( enumN64TimerVi, iNewViTimer );
	
	if( ( N64Regs->VI[0] & 0x10 ) != 0 ) 
	{
		if( ViFieldNumber == 0 )
			ViFieldNumber = 1;
		else
			ViFieldNumber = 0;		
	}
	else 
	{
		ViFieldNumber = 0;
	}
}

// Vi Status Change
//////////////////////////////////////////////////////////////////////////
void ViStatusChange ( void )
{
	//* Call Update Video Satus
}

// Vi Width Change
//////////////////////////////////////////////////////////////////////////
void ViWidthChanged ( void )
{
	//* Store Width
	ViScreenSize[0] = N64Regs->VI[2];
	ViScreenSize[1] = ( ViScreenSize[0] >> 2 ) * 3;

	ViScreenHalf[0] = (float)( ViScreenSize[0] >> 1 );
	ViScreenHalf[1] = (float)( ViScreenSize[1] >> 1 );

	//* Call Update Video Status
	iViewportChanged = 1;
}

// UpdateHalfLine
//////////////////////////////////////////////////////////////////////////
void ViUpdateHalfLine ( void )
{
	if( N64SysTimers.iCurrentTime < 0 ) 
	{ 
		ViHalfLine = 0;
		return;
	}

	ViHalfLine = (N64SysTimers.iCurrentTime / 1500);
	ViHalfLine &= ~1;
	ViHalfLine += ViFieldNumber;
}

// Update Size
//////////////////////////////////////////////////////////////////////////
void ViUpdateSize ( void )
{
//	float xScale = _FIXED2FLOAT( _SHIFTR( N64Regs->VI[12], 0, 12 ), 10 );
//	float xOffset = _FIXED2FLOAT( _SHIFTR( N64Regs->VI[12], 16, 12 ), 10 );
//
//	float yScale = _FIXED2FLOAT( _SHIFTR( N64Regs->VI[13], 0, 12 ), 10 );
//	float yOffset = _FIXED2FLOAT( _SHIFTR( N64Regs->VI[13], 16, 12 ), 10 );
//
//	unsigned int hEnd = _SHIFTR( N64Regs->VI[9], 0, 10 );
//	unsigned int hStart = _SHIFTR( N64Regs->VI[9], 16, 10 );
//
//	unsigned int vEnd = _SHIFTR( N64Regs->VI[10], 1, 9 );
//	unsigned int vStart = _SHIFTR( N64Regs->VI[10], 17, 9 );
//
//	ViScreenSize[0] = (hEnd - hStart) * xScale;
//	ViScreenSize[1] = (vEnd - vStart) * yScale * 1.0126582f;
//
//	if( ViScreenSize[0] == 0.0f ) ViScreenSize[0] = 320.0f;
//	if( ViScreenSize[1] == 0.0f ) ViScreenSize[1] = 240.0f;
}
