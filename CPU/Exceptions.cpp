//////////////////////////////////////////////////////////////////////////
// Exceptions / Interupts
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Exceptions & Interupts

#include "Registers.h"
#include "Exceptions.h"
#include "Instructions/Instructions.h"

int WaitForInput( u16 uInputButton );

// Check Interupts
//////////////////////////////////////////////////////////////////////////
void CheckInterrupts ( void ) 
{
	N64Regs->MI[2] &= ~0x04;
	N64Regs->MI[2] |= (N64Regs->AudioIntrReg & 0x04); 

	if ((N64Regs->MI[3] & N64Regs->MI[2]) != 0)
		N64Regs->COP0[32] |= 0x400;
	else
		N64Regs->COP0[32] &= ~0x400;

	if( ( N64Regs->COP0[12] & 0x00000001 ) == 0 ) { return; }
	if( ( N64Regs->COP0[12] & 0x00000002 ) != 0 ) { return; }
	if( ( N64Regs->COP0[12] & 0x00000004 ) != 0 ) { return; }

	if( ( N64Regs->COP0[12] & N64Regs->COP0[32] & 0xFF00 ) != 0 )
		N64Regs->iPerformInterupt = 1;
}

// Perform Interupt Exception
//////////////////////////////////////////////////////////////////////////
void PerformIntrException ( int iDelaySlot ) 
{
	if( ( N64Regs->COP0[12] & 0x00000001 ) == 0 ) { return; }
	if( ( N64Regs->COP0[12] & 0x00000002 ) != 0 ) { return; }
	if( ( N64Regs->COP0[12] & 0x00000004 ) != 0 ) { return; }

	N64Regs->COP0[13] = N64Regs->COP0[32]; //FAKE_CAUSE_REGISTER
	N64Regs->COP0[13] |= 0 << 2; //* I know this looks odd but needed

//	if (iDelaySlot) 
//	{
//		N64Regs->COP0[13] |= 0x80000000;
//		N64Regs->COP0[14]  = N64Regs->PC - 4;
//	} 
//	else 
//	{
//		N64Regs->COP0[13] &= 0x7FFFFFFF;
		N64Regs->COP0[14] = N64Regs->PC;
//	}

	N64Regs->COP0[12] |= 0x00000002;
	N64Regs->PC = 0x80000180;
}

// Perform Cop1 Unstable Exception
//////////////////////////////////////////////////////////////////////////
void PerformCop1UnusableException ( int DelaySlot )
{
//	if(( STATUS_REGISTER & STATUS_EXL  ) != 0 ) { 
//		DisplayError("EXL set in Break Exception");
//	}
//	if (( STATUS_REGISTER & STATUS_ERL  ) != 0 ) { 
//		DisplayError("ERL set in Break Exception");
//	}

	N64Regs->COP0[13]  = 11 << 2;
	N64Regs->COP0[13] |= 0x10000000;

	if( DelaySlot ) 
	{
		N64Regs->COP0[13] |= 0x80000000;
		N64Regs->COP0[14] = N64Regs->PC - 4;
	} 
	else
	{
		N64Regs->COP0[14] = N64Regs->PC;
	}

	N64Regs->COP0[12] |= 0x00000002;
	N64Regs->PC = 0x80000180;
}

// Perform Tlb Miss Exception
//////////////////////////////////////////////////////////////////////////
void PerformTLBException ( int DelaySlot, DWORD BadAddr, int iWrite ) 
{
	if( iWrite )
		N64Regs->COP0[13] = 3 << 2; //* Tlb Write Miss
	else
		N64Regs->COP0[13] = 2 << 2; //* Tlb Read Miss

	N64Regs->COP0[8]  = BadAddr;
	N64Regs->COP0[4] &= 0xFF80000F;
	N64Regs->COP0[4] |= (BadAddr >> 9) & 0x007FFFF0;
	N64Regs->COP0[10] = (BadAddr & 0xFFFFE000);

	if( ( N64Regs->COP0[12] & 0x00000002 ) == 0 ) 
	{
		if( DelaySlot ) 
		{
			N64Regs->COP0[13] |= 0x80000000;
			N64Regs->COP0[14] = N64Regs->PC - 4;
		} 
		else 
		{
			N64Regs->COP0[14] = N64Regs->PC;
		}

		if( TlbAddressDefined(BadAddr) )
			N64Regs->PC = 0x80000180;
		else
			N64Regs->PC = 0x80000000;

		N64Regs->COP0[12] |= 0x00000002;
	} 
	else 
	{
		N64Regs->PC = 0x80000180;
	}

	JumpAddress = N64Regs->PC;
}

// Tlb Address Defined
//////////////////////////////////////////////////////////////////////////
int TlbAddressDefined ( DWORD Addr )
{
	DWORD i;

	if( Addr >= 0x80000000 && Addr <= 0xBFFFFFFF )
		return 1;

	for( i = 0; i < 32; i++ ) 
	{
		//if( FastTlb[i].ValidEntry == 0 ) { continue; }
		if( Addr >= N64Regs->Tlb[i].start_even && Addr <= N64Regs->Tlb[i].end_even )
			return 1;

		if( Addr >= N64Regs->Tlb[i].start_odd && Addr <= N64Regs->Tlb[i].end_odd )
			return 1;
	}

	return 0;	
}
