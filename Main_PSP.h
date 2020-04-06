//////////////////////////////////////////////////////////////////////////
// Main App
//////////////////////////////////////////////////////////////////////////
// The Main App for Monkey64

#ifndef _MAIN_PSP_H_
#define _MAIN_PSP_H_

#include "StdInc.h"

// External Objects
//////////////////////////////////////////////////////////////////////////
extern DWORD BreakAddress;
extern int iWaitForInputTrig;
extern unsigned int __attribute__((aligned(16))) iPspFrameBuffer;

// Defines
//////////////////////////////////////////////////////////////////////////
#define Monkey64_Version         "2.0"
//#define DEBUGER_ENABLED

// Functions
//////////////////////////////////////////////////////////////////////////

//* Setup Callbacks
int SetupCallbacks( void );

//* Init Display
void InitDisplay( void );

//* Init Input
void InitInput( void );

//* Setup Display List
void SetupDispList( int iMode );

//* Setup Viewport
void UpdateViewport( int iWidth, int iHeight );

#endif
