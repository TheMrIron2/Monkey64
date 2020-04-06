//////////////////////////////////////////////////////////////////////////
// Main Menu
//////////////////////////////////////////////////////////////////////////
// Main Menu

#ifndef _MENU_PSP_H_
#define _MENU_PSP_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "StdInc.h"

// Menu Enumerations
//////////////////////////////////////////////////////////////////////////
enum EnumMenuItems
{
	enumMenuItem_LoadRom = 0,
	enumMenuItem_ResetEmulation,
	enumMenuItem_RunEmulation,

	enumMenuItem_ClockSpeed,
	enumMenuItem_Tlb,
	enumMenuItem_MicroCode,
	enumMenuItem_ScreenSize,
//	enumMenuItem_AxisDir,

	enumMenuItem_About,
	enumMenuItem_Exit,

#ifdef DEBUGER_ENABLED
	enumMenuItem_DebugBreakAddr,
	enumMenuItem_DebugWaitInput,
#endif

	enumMenuItem_Max
};

// Functions
//////////////////////////////////////////////////////////////////////////

//* Main Menu
int MainMenu( void );

//* Print Credits
void PrintCredits( void );

#endif
