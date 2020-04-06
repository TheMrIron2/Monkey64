//////////////////////////////////////////////////////////////////////////
// CoProcessor 0 Core
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Cop0 opcode set

#include "Instructions.h"
#include "../Registers.h"
#include "../Memory.h"
#include "../Timers.h"
#include "../Exceptions.h"


// Instruction: MFC0 ( MF Cop0 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MFC0 ( void )
{
	N64Regs->CPU[InstrOpcode.rt].DW = (int)N64Regs->COP0[InstrOpcode.rd];
	N64Regs->PC += 4;
}

// Instruction: MTC0 ( MT Cop0 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MTC0 ( void )
{
	switch( InstrOpcode.rd ) 
	{	
	case 0: //Index
	case 2: //EntryLo0
	case 3: //EntryLo1
	case 5: //PageMask
	case 6: //Wired
	case 10: //Entry Hi
	case 14: //EPC
	case 16: //Config
	case 18: //WatchLo
	case 19: //WatchHi
	case 28: //Tag lo
	case 29: //Tag Hi
		N64Regs->COP0[InstrOpcode.rd] = N64Regs->CPU[InstrOpcode.rt].UW[0];
		break;

	case 4: //Context
		N64Regs->COP0[InstrOpcode.rd] = N64Regs->CPU[InstrOpcode.rt].UW[0] & 0xFF800000;
		break;

	case 9: //Count
		N64Regs->COP0[InstrOpcode.rd] = N64Regs->CPU[InstrOpcode.rt].UW[0];
		ChangeCompareTimer();
		break;

	case 11: //Compare
		N64Regs->COP0[InstrOpcode.rd] = N64Regs->CPU[InstrOpcode.rt].UW[0];
		N64Regs->COP0[32] &= ~0x8000;
		ChangeCompareTimer();
		break;

	case 12: //Status
		if( ( N64Regs->COP0[InstrOpcode.rd] ^ N64Regs->CPU[InstrOpcode.rt].UW[0] ) != 0 )
		{
			N64Regs->COP0[InstrOpcode.rd] = N64Regs->CPU[InstrOpcode.rt].UW[0];
			SetFpuLocations();
		} else 
		{
			N64Regs->COP0[InstrOpcode.rd] = N64Regs->CPU[InstrOpcode.rt].UW[0];
		}
		CheckInterrupts();
		break;

	case 13: //Cause
		N64Regs->COP0[InstrOpcode.rd] &= 0xFFFFCFF;
		break;

	default:
		Op_NI();
	}

	N64Regs->PC += 4;
}

// Instruction: TLBR ( TLBR Cop0 Tlb Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_TLBR ( void )
{
	DWORD Index = N64Regs->COP0[0] & 0x1F;

	N64Regs->COP0[5] = N64Regs->Tlb[Index].mask << 13;
	N64Regs->COP0[10] = ( ( N64Regs->Tlb[Index].vpn2 << 13 ) | N64Regs->Tlb[Index].asid );
	N64Regs->COP0[2] = ( N64Regs->Tlb[Index].pfn_even << 6) | ( N64Regs->Tlb[Index].c_even << 3)
					| ( N64Regs->Tlb[Index].d_even << 2) | ( N64Regs->Tlb[Index].v_even << 1)
					| N64Regs->Tlb[Index].g;
	N64Regs->COP0[3] = ( N64Regs->Tlb[Index].pfn_odd << 6 ) | ( N64Regs->Tlb[Index].c_odd << 3 )
					| ( N64Regs->Tlb[Index].d_odd << 2) | ( N64Regs->Tlb[Index].v_odd << 1 )
					| N64Regs->Tlb[Index].g;
	
	N64Regs->PC += 4;
}

// Instruction: TLBWI ( TLBWI Cop0 Tlb Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_TLBWI ( void )
{
	DWORD Index = N64Regs->COP0[0] & 0x3F;
	int i;

	if( g_iUseFastTlb )
	{
		if( N64Regs->Tlb[Index].v_even )
		{
			for( i = N64Regs->Tlb[Index].start_even; i < N64Regs->Tlb[Index].end_even; i++ )
				N64Regs->Tlb_Lut_R[i >> 12] = 0;

			if( N64Regs->Tlb[Index].d_even )
				for( i = N64Regs->Tlb[Index].start_even; i < N64Regs->Tlb[Index].end_even; i++ )
					N64Regs->Tlb_Lut_W[i >> 12] = 0;
		}

		if( N64Regs->Tlb[Index].v_odd )
		{
			for( i = N64Regs->Tlb[Index].start_odd; i < N64Regs->Tlb[Index].end_odd; i++ )
				N64Regs->Tlb_Lut_R[i >> 12] = 0;

			if( N64Regs->Tlb[Index].d_odd )
				for( i = N64Regs->Tlb[Index].start_odd; i < N64Regs->Tlb[Index].end_odd; i++ )
					N64Regs->Tlb_Lut_W[i >> 12] = 0;
		}
	}

	N64Regs->Tlb[Index].g = ( N64Regs->COP0[2] & N64Regs->COP0[3] & 1 );
	N64Regs->Tlb[Index].pfn_even = ( N64Regs->COP0[2] & 0x3FFFFFC0 ) >> 6;
	N64Regs->Tlb[Index].pfn_odd = ( N64Regs->COP0[3] & 0x3FFFFFC0 ) >> 6;
	N64Regs->Tlb[Index].c_even = ( N64Regs->COP0[2] & 0x38 ) >> 3;
	N64Regs->Tlb[Index].c_odd = ( N64Regs->COP0[3] & 0x38 ) >> 3;
	N64Regs->Tlb[Index].d_even = ( N64Regs->COP0[2] & 0x4 ) >> 2;
	N64Regs->Tlb[Index].d_odd = ( N64Regs->COP0[3] & 0x4 ) >> 2;
	N64Regs->Tlb[Index].v_even = ( N64Regs->COP0[2] & 0x2 ) >> 1;
	N64Regs->Tlb[Index].v_odd = ( N64Regs->COP0[3] & 0x2 ) >> 1;
	N64Regs->Tlb[Index].asid = ( N64Regs->COP0[10] & 0xFF );
	N64Regs->Tlb[Index].vpn2 = ( N64Regs->COP0[10] & 0xFFFFE000 ) >> 13;
	N64Regs->Tlb[Index].r = ( N64Regs->COP0[10] & 0xC000000000000000ULL ) >> 62;
	N64Regs->Tlb[Index].mask = ( N64Regs->COP0[5] & 0x1FFE000) >> 13;

	N64Regs->Tlb[Index].start_even = N64Regs->Tlb[Index].vpn2 << 13;
	N64Regs->Tlb[Index].end_even   = N64Regs->Tlb[Index].start_even + ( N64Regs->Tlb[Index].mask << 12 ) + 0xFFF;
	N64Regs->Tlb[Index].phys_even  = N64Regs->Tlb[Index].pfn_even << 12;

	if( g_iUseFastTlb )
	{
		if( N64Regs->Tlb[Index].v_even )
		{
			if( ( N64Regs->Tlb[Index].start_even < N64Regs->Tlb[Index].end_even ) && 
			!( ( N64Regs->Tlb[Index].start_even >= 0x80000000 ) && ( N64Regs->Tlb[Index].end_even < 0xC0000000 ) ) && 
				N64Regs->Tlb[Index].phys_even < 0x20000000 )
			{
				for( i = N64Regs->Tlb[Index].start_even; i < N64Regs->Tlb[Index].end_even; i++ )
					N64Regs->Tlb_Lut_R[i>>12] = 0x80000000 | ( N64Regs->Tlb[Index].phys_even + ( i - N64Regs->Tlb[Index].start_even ) );

				if( N64Regs->Tlb[Index].d_even )
					for( i = N64Regs->Tlb[Index].start_even; i < N64Regs->Tlb[Index].end_even; i++ )
						N64Regs->Tlb_Lut_W[i>>12] = 0x80000000 | ( N64Regs->Tlb[Index].phys_even + ( i - N64Regs->Tlb[Index].start_even ) );
			}
		}
	}

	N64Regs->Tlb[Index].start_odd = N64Regs->Tlb[Index].end_even + 1;
	N64Regs->Tlb[Index].end_odd   = N64Regs->Tlb[Index].start_odd + ( N64Regs->Tlb[Index].mask << 12 ) + 0xFFF;
	N64Regs->Tlb[Index].phys_odd  = N64Regs->Tlb[Index].pfn_odd << 12;

	if( g_iUseFastTlb )
	{
		if( N64Regs->Tlb[Index].v_odd )
		{
			if( ( N64Regs->Tlb[Index].start_odd < N64Regs->Tlb[Index].end_odd ) &&
			!( ( N64Regs->Tlb[Index].start_odd >= 0x80000000 ) && ( N64Regs->Tlb[Index].end_odd < 0xC0000000 ) ) &&
				( N64Regs->Tlb[Index].phys_odd < 0x20000000 ) )
			{
				for( i = N64Regs->Tlb[Index].start_odd; i < N64Regs->Tlb[Index].end_odd; i++ )
					N64Regs->Tlb_Lut_R[i>>12] = 0x80000000 | ( N64Regs->Tlb[Index].phys_odd + ( i - N64Regs->Tlb[Index].start_odd ) );

				if( N64Regs->Tlb[Index].d_odd )
					for( i = N64Regs->Tlb[Index].start_odd; i < N64Regs->Tlb[Index].end_odd; i++ )
					N64Regs->Tlb_Lut_W[i>>12] = 0x80000000 | ( N64Regs->Tlb[Index].phys_odd + ( i - N64Regs->Tlb[Index].start_odd ) );
			}
		}
	}

	N64Regs->PC += 4;
}

// Instruction: TLBP ( TLBP Cop0 Tlb Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_TLBP( void )
{
	int i;

	N64Regs->PC += 4;

	N64Regs->COP0[0] |= 0x80000000;
	N64Regs->PC += 4;

	for( i = 0; i < 32; i++ )
	{
		if( ( ( N64Regs->Tlb[i].vpn2 & ( ~N64Regs->Tlb[i].mask ) ) ==
			( ( ( N64Regs->COP0[10] & 0xFFFFE000 ) >> 13) & ( ~N64Regs->Tlb[i].mask ) ) ) &&
			( ( N64Regs->Tlb[i].g ) || ( N64Regs->Tlb[i].asid == ( N64Regs->COP0[10] & 0xFF ) ) ) )
		{
			N64Regs->COP0[0] = i;
			return;
		}
	}
}

// Instruction: ERET ( ERET Cop0 Tlb Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ERET ( void ) 
{
	if( ( N64Regs->COP0[12] & 0x00000004 ) != 0 )
	{
		N64Regs->PC = N64Regs->COP0[30];
		N64Regs->COP0[12] &= ~0x00000004;
	}
	else
	{
		N64Regs->PC = N64Regs->COP0[14];
		N64Regs->COP0[12] &= ~0x00000002;
	}
	
	//* Will leave in for now
	//LLBit = 0;

	CheckInterrupts();
	N64Regs->iCheckTimers = 1;
}
