//////////////////////////////////////////////////////////////////////////
// Exceptions / Interupts
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Exceptions & Interupts

#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

// Functions
//////////////////////////////////////////////////////////////////////////

//* Check Interupts
void CheckInterrupts ( void );

//* Perform Interupt Exception
void PerformIntrException ( int iDelaySlot );

//* Performs Cop1 Exception
void PerformCop1UnusableException ( int DelaySlot );

//* Perform Tlb Miss Exception
void PerformTLBException ( int DelaySlot, DWORD BadAddr, int iWrite );

//* Check Tlb Addr
int TlbAddressDefined ( DWORD Addr );

#endif
