//////////////////////////////////////////////////////////////////////////
// CoProcessor 1 Core
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Cop1 opcode set

#include "Instructions.h"
#include "../Registers.h"
#include "../Memory.h"
#include "../Timers.h"
#include "../Exceptions.h"

// External Objects
//////////////////////////////////////////////////////////////////////////
extern int WaitForInput( u16 uInputButton );

// Check if Stable (Taken from PJ64)
//////////////////////////////////////////////////////////////////////////
#define TEST_COP1_USABLE_EXCEPTION							\
	if( ( N64Regs->COP0[12] & 0x20000000 ) == 0 )			\
	{														\
		PerformCop1UnusableException( iNextJump );			\
		JumpAddress = N64Regs->PC;							\
		return;												\
	}

// Instruction: MFC1 ( MF Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MFC1 (void) 
{
	TEST_COP1_USABLE_EXCEPTION

	N64Regs->CPU[InstrOpcode.rt].DW = *(int *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: DMFC1 ( DMF Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DMFC1 (void) 
{
	TEST_COP1_USABLE_EXCEPTION

	N64Regs->CPU[InstrOpcode.rt].DW = *(long long *)N64Regs->COP1Double[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CFC1 ( CF Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CFC1 ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	if( InstrOpcode.fs == 31 || InstrOpcode.fs == 0 ) 
	{
		N64Regs->CPU[InstrOpcode.rt].DW = (int)N64Regs->COP1CR[InstrOpcode.fs];
	}

	N64Regs->PC += 4;
}

// Instruction: MTC1 ( MT Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MTC1 ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(int *)N64Regs->COP1Float[InstrOpcode.fs] = N64Regs->CPU[InstrOpcode.rt].W[0];
	N64Regs->PC += 4;
}

// Instruction: DMTC1 ( DMT Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DMTC1 ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(long long *)N64Regs->COP1Double[InstrOpcode.fs] = N64Regs->CPU[InstrOpcode.rt].DW;
	N64Regs->PC += 4;
}

// Instruction: CTC1 ( CT Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CTC1 ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	if( InstrOpcode.fs == 31 )
	{
		N64Regs->COP1CR[InstrOpcode.fs] = N64Regs->CPU[InstrOpcode.rt].W[0];
		
		switch( N64Regs->COP1CR[InstrOpcode.fs] & 3 )
		{
			case 0:
				N64Regs->iRoundingModel = 0x00000000; //* RC_NEAR
				break;
			case 1:
				N64Regs->iRoundingModel = 0x00000300; //* RC_CHOP
				break;
			case 2:
				N64Regs->iRoundingModel = 0x00000200; //* RC_UP
				break;
			case 3:
				N64Regs->iRoundingModel = 0x00000100; //* RC_DOWN
				break;
		}
	}

	N64Regs->PC += 4;
}

// Instruction: BC1F ( BC1F BC Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BC1F ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	N64Regs->PC += 4;

	if( ( N64Regs->COP1CR[31] & 0x00800000 ) == 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: BC1T ( BC1T BC Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BC1T ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	N64Regs->PC += 4;

	if( ( N64Regs->COP1CR[31] & 0x00800000 ) != 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: BC1FL ( BC1FL BC Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BC1FL ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	if( ( N64Regs->COP1CR[31] & 0x00800000 ) == 0 )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		RunCpuInstruction();

		N64Regs->PC = JumpAddress;
	}
	else
		N64Regs->PC += 8;

	N64Regs->iCheckTimers = 1;
}

// Instruction: BC1TL ( BC1TL BC Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BC1TL ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	if( ( N64Regs->COP1CR[31] & 0x00800000 ) != 0 )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		RunCpuInstruction();

		N64Regs->PC = JumpAddress;
	}
	else
		N64Regs->PC += 8;

	N64Regs->iCheckTimers = 1;
}

// Instruction: ADD_S ( Add Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ADD_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = ( *(float *)N64Regs->COP1Float[InstrOpcode.fs] + *(float *)N64Regs->COP1Float[InstrOpcode.ft] ); 
	N64Regs->PC += 4;
}

// Instruction: ADD_D ( Add Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ADD_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = *(double *)N64Regs->COP1Double[InstrOpcode.fs] + *(double *)N64Regs->COP1Double[InstrOpcode.ft]; 
	N64Regs->PC += 4;
}

// Instruction: SUB_S ( Subtract Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SUB_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = ( *(float *)N64Regs->COP1Float[InstrOpcode.fs] - *(float *)N64Regs->COP1Float[InstrOpcode.ft] ); 
	N64Regs->PC += 4;
}

// Instruction: SUB_D ( Subtract Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SUB_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = *(double *)N64Regs->COP1Double[InstrOpcode.fs] - *(double *)N64Regs->COP1Double[InstrOpcode.ft];
	N64Regs->PC += 4;
}

// Instruction: MULT_S ( Multiply Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MULT_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = ( *(float *)N64Regs->COP1Float[InstrOpcode.fs] * *(float *)N64Regs->COP1Float[InstrOpcode.ft] ); 
	N64Regs->PC += 4;
}

// Instruction: MULT_D ( Multiply Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MULT_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = *(double *)N64Regs->COP1Double[InstrOpcode.fs] * *(double *)N64Regs->COP1Double[InstrOpcode.ft]; 
	N64Regs->PC += 4;
}

// Instruction: DIV_S ( Divide Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DIV_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = ( *(float *)N64Regs->COP1Float[InstrOpcode.fs] / *(float *)N64Regs->COP1Float[InstrOpcode.ft] ); 
	N64Regs->PC += 4;
}

// Instruction: DIV_D ( Divide Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DIV_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = *(double *)N64Regs->COP1Double[InstrOpcode.fs] / *(double *)N64Regs->COP1Double[InstrOpcode.ft]; 
	N64Regs->PC += 4;
}

// Instruction: SQRT_S ( Square Root Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SQRT_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = (float)sqrt( *(float *)N64Regs->COP1Float[InstrOpcode.fs] );
	N64Regs->PC += 4;
}

// Instruction: SQRT_D ( Square Root Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SQRT_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = (double)sqrt( *(double *)N64Regs->COP1Double[InstrOpcode.fs] );
	N64Regs->PC += 4;
}

// Instruction: ABS_S ( Abs Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ABS_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = (float)fabs( *(float *)N64Regs->COP1Float[InstrOpcode.fs] );
	N64Regs->PC += 4;
}

// Instruction: ABS_D ( Abs Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ABS_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = (double)fabs( *(double *)N64Regs->COP1Double[InstrOpcode.fs] );
	N64Regs->PC += 4;
}

// Instruction: MOV_S ( Move Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MOV_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = *(float *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: MOV_D ( Move Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MOV_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(long long *)N64Regs->COP1Double[InstrOpcode.fd] = *(long long *)N64Regs->COP1Double[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: NEG_S ( Negate Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_NEG_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = ( *(float *)N64Regs->COP1Float[InstrOpcode.fs] * -1.0f );
	N64Regs->PC += 4;
}

// Instruction: NEG_D ( Negate Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_NEG_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = ( *(double *)N64Regs->COP1Double[InstrOpcode.fs] * -1.0f );
	N64Regs->PC += 4;
}

// Instruction: ROUND_W_S ( Round Single To Word Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ROUND_W_S()
{
	TEST_COP1_USABLE_EXCEPTION

	//* TODO - ACTUALY ROUND DAMN IT !!
	*( (int *)N64Regs->COP1Float[InstrOpcode.fd] ) = (int)*(float *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: TRUNC_W_S ( Truncate Single To Word Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_TRUNC_W_S()
{
	TEST_COP1_USABLE_EXCEPTION

	*( (int *)N64Regs->COP1Float[InstrOpcode.fd] ) = (int)*(float *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: TRUNC_W_D ( Truncate Double To Word Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_TRUNC_W_D()
{
	TEST_COP1_USABLE_EXCEPTION

	*( (DWORD *)N64Regs->COP1Float[InstrOpcode.fd] ) = (DWORD)*(double *)N64Regs->COP1Double[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_S_D ( Convert Double to Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_S_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = (float) *(double *)N64Regs->COP1Double[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_S_W ( Convert Word to Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_S_W ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = (float)*(int *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_S_L ( Convert Long to Single Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_S_L ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(float *)N64Regs->COP1Float[InstrOpcode.fd] = (float)*(long long *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_D_W ( Convert Word to Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_D_W ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = (double)*(int *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_D_L ( Convert Long to Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_D_L ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = (double)*(long long *)N64Regs->COP1Double[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_D_S ( Convert Single to Double Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_D_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(double *)N64Regs->COP1Double[InstrOpcode.fd] = (double)( *(float *)N64Regs->COP1Float[InstrOpcode.fs] );
	N64Regs->PC += 4;
}

// Instruction: CVT_W_S ( Convert Single to Word Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_W_S ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*(long *)N64Regs->COP1Float[InstrOpcode.fd] = (long)*(float *)N64Regs->COP1Float[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CVT_W_D ( Convert Double to Word Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CVT_W_D ( void ) 
{
	TEST_COP1_USABLE_EXCEPTION

	*( (DWORD *)N64Regs->COP1Float[InstrOpcode.fd] ) = (DWORD)*(double *)N64Regs->COP1Double[InstrOpcode.fs];
	N64Regs->PC += 4;
}

// Instruction: CMP_S ( CMP_S Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CMP_S ( void )
{
	int less, equal, unorded, condition;
	float Temp0, Temp1;

	TEST_COP1_USABLE_EXCEPTION

	Temp0 = *(float *)N64Regs->COP1Float[InstrOpcode.fs];
	Temp1 = *(float *)N64Regs->COP1Float[InstrOpcode.ft];

	if( isnan( Temp0 ) || isnan( Temp1 ) ) 
	{
		less = 0;
		equal = 0;
		unorded = 1;
	} 
	else 
	{
		less = Temp0 < Temp1;
		equal = Temp0 == Temp1;
		unorded = 0;
	}
	
	condition = ( ( InstrOpcode.funct & 4 ) && less ) | ( ( InstrOpcode.funct & 2) && equal ) | ( ( InstrOpcode.funct & 1 ) && unorded );

	if( condition )
		N64Regs->COP1CR[31] |= 0x00800000;
	else
		N64Regs->COP1CR[31] &= ~0x00800000;

	N64Regs->PC += 4;
}

// Instruction: CMP_D ( CMP_D Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CMP_D ( void )
{
	int less, equal, unorded, condition;
	MIPS_DWORD Temp0, Temp1;

	TEST_COP1_USABLE_EXCEPTION

	Temp0.DW = *(long long *)N64Regs->COP1Double[InstrOpcode.fs];
	Temp1.DW = *(long long *)N64Regs->COP1Double[InstrOpcode.ft];

	if( isnan( Temp0.D ) || isnan( Temp1.D ) ) 
	{
		less = 0;
		equal = 0;
		unorded = 1;
	} 
	else 
	{
		less = Temp0.D < Temp1.D;
		equal = Temp0.D == Temp1.D;
		unorded = 0;
	}
	
	condition = ( ( InstrOpcode.funct & 4 ) && less ) | ( ( InstrOpcode.funct & 2) && equal ) | ( ( InstrOpcode.funct & 1 ) && unorded );

	if( condition )
		N64Regs->COP1CR[31] |= 0x00800000;
	else
		N64Regs->COP1CR[31] &= ~0x00800000;

	N64Regs->PC += 4;
}
