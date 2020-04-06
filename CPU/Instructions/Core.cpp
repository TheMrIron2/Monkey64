//////////////////////////////////////////////////////////////////////////
// Cpu Core
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Core

#include "Instructions.h"
#include "../Registers.h"
#include "../Memory.h"
#include "../Timers.h"
#include "../Exceptions.h"
#include "../../Video_Psp.h"
#include "../../Helper_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
N64_OPCODE InstrOpcode;
DWORD JumpAddress;
DWORD BreakAddress = 0x80000000 - 0x4;
int iNextJump = 0;
int iEmulatorRunning;
int iPauseOnExit;

int iPrintDebug;
int iDispPrintDebug;
int iStepLoop;
int iWaitForInputTrig = 1;

int WaitForInput( u16 uInputButton );

// Check if Stable (Taken from PJ64)
//////////////////////////////////////////////////////////////////////////
#define TEST_COP1_USABLE_EXCEPTION							\
	if( ( N64Regs->COP0[12] & 0x20000000 ) == 0 )			\
	{														\
		PerformCop1UnusableException( iNextJump );			\
		JumpAddress = N64Regs->PC;							\
		return;												\
	}

// Instruction Set ( Core )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Core[64])(void) =
{
	Op_Extended, Op_Regimm, Op_J,    Op_JAL,	Op_BEQ,	 Op_BNE,  Op_BLEZ,  Op_BGTZ,
	Op_ADDI,	 Op_ADDIU,	Op_SLTI, Op_SLTIU,  Op_ANDI, Op_ORI,  Op_XORI,  Op_LUI,
	Op_Cop0,	 Op_Cop1,	Op_NI,   Op_NI,		Op_BEQL, Op_BNEL, Op_BLEZL, Op_BGTZL,
	Op_NI,		 Op_DADDIU,	Op_NI,   Op_NI,		Op_NI,	 Op_NI,   Op_NI,    Op_NI,
	Op_LB,		 Op_LH,		Op_LWL,  Op_LW,		Op_LBU,	 Op_LHU,  Op_LWR,   Op_LWU,
	Op_SB,		 Op_SH,		Op_SWL,  Op_SW,		Op_NI,	 Op_NI,   Op_SWR,   Op_CACHE,
	Op_NI,		 Op_LWC1,	Op_NI,   Op_NI,		Op_NI,	 Op_LDC1, Op_NI,    Op_LD,
	Op_NI,		 Op_SWC1,	Op_NI,   Op_NI,		Op_NI,	 Op_SDC1, Op_NI,    Op_SD

//* Instruction Core Reference ( Taken from Mupen64 )
//   SPECIAL, REGIMM, J   , JAL  , BEQ , BNE , BLEZ , BGTZ ,
//   ADDI   , ADDIU , SLTI, SLTIU, ANDI, ORI , XORI , LUI  ,
//   COP0   , COP1  , NI  , NI   , BEQL, BNEL, BLEZL, BGTZL,
//   DADDI  , DADDIU, LDL , LDR  , NI  , NI  , NI   , NI   ,
//   LB     , LH    , LWL , LW   , LBU , LHU , LWR  , LWU  ,
//   SB     , SH    , SWL , SW   , SDL , SDR , SWR  , CACHE,
//   LL     , LWC1  , NI  , NI   , NI  , LDC1, NI   , LD   ,
//   SC     , SWC1  , NI  , NI   , NI  , SDC1, NI   , SD
};

// Instruction Set ( Extended / Extra )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Extended[64])(void) =
{
	Op_SLL,  Op_NI,	   Op_SRL,  Op_SRA,  Op_SLLV,   Op_NI,	   Op_SRLV,   Op_SRAV,
	Op_JR,   Op_JALR,  Op_NI,   Op_NI,   Op_NI,     Op_NI,	   Op_NI,     Op_SYNC,
	Op_MFHI, Op_MTHI,  Op_MFLO, Op_MTLO, Op_DSLLV,  Op_NI,	   Op_DSRLV,  Op_DSRAV,
	Op_MULT, Op_MULTU, Op_DIV,  Op_DIVU, Op_DMULT,  Op_DMULTU, Op_DDIV,   Op_DDIVU,
	Op_ADD,  Op_ADDU,  Op_SUB,  Op_SUBU, Op_AND,    Op_OR,	   Op_XOR,    Op_NOR,
	Op_NI,   Op_NI,	   Op_SLT,  Op_SLTU, Op_DADD,   Op_DADDU,  Op_NI,     Op_NI,
	Op_NI,   Op_NI,	   Op_NI,   Op_NI,   Op_TEQ,    Op_NI,	   Op_NI,     Op_NI,
	Op_DSLL, Op_NI,	   Op_DSRL, Op_DSRA, Op_DSLL32, Op_NI,	   Op_DSRL32, Op_DSRA32

//* Instruction Extended Reference ( Taken from Mupen64 )
//   SLL , NI   , SRL , SRA , SLLV   , NI    , SRLV  , SRAV  ,
//   JR  , JALR , NI  , NI  , SYSCALL, NI    , NI    , SYNC  ,
//   MFHI, MTHI , MFLO, MTLO, DSLLV  , NI    , DSRLV , DSRAV ,
//   MULT, MULTU, DIV , DIVU, DMULT  , DMULTU, DDIV  , DDIVU ,
//   ADD , ADDU , SUB , SUBU, AND    , OR    , XOR   , NOR   ,
//   NI  , NI   , SLT , SLTU, DADD   , DADDU , DSUB  , DSUBU ,
//   NI  , NI   , NI  , NI  , TEQ    , NI    , NI    , NI    ,
//   DSLL, NI   , DSRL, DSRA, DSLL32 , NI    , DSRL32, DSRA32
};

//  Instruction Set ( Regimm )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Regimm[32])(void) =
{
	Op_BLTZ,   Op_BGEZ,	  Op_BLTZL, Op_BGEZL, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,     Op_NI,	  Op_NI,    Op_NI,	  Op_NI, Op_NI, Op_NI, Op_NI,
	Op_BLTZAL, Op_BGEZAL, Op_NI,    Op_NI,    Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,     Op_NI,	  Op_NI,    Op_NI,    Op_NI, Op_NI, Op_NI, Op_NI

//* Instruction Regimm Reference ( Taken from Mupen64 )
//   BLTZ  , BGEZ  , BLTZL  , BGEZL  , NI, NI, NI, NI,
//   NI    , NI    , NI     , NI     , NI, NI, NI, NI,
//   BLTZAL, BGEZAL, BLTZALL, BGEZALL, NI, NI, NI, NI,
//   NI    , NI    , NI     , NI     , NI, NI, NI, NI
};

// Instruction Set ( Cop0 / Co Processor 0 )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop0[32])(void) =
{
	Op_MFC0, Op_NI, Op_NI, Op_NI, Op_MTC0, Op_NI, Op_NI, Op_NI,
	Op_NI,	 Op_NI, Op_NI, Op_NI, Op_NI,   Op_NI, Op_NI, Op_NI,
	Op_Tlb,  Op_NI, Op_NI, Op_NI, Op_NI,   Op_NI, Op_NI, Op_NI,
	Op_NI,	 Op_NI, Op_NI, Op_NI, Op_NI,   Op_NI, Op_NI, Op_NI

//* Instruction Cop0 Reference ( Taken from Mupen64 )
//   MFC0, NI, NI, NI, MTC0, NI, NI, NI,
//   NI  , NI, NI, NI, NI  , NI, NI, NI,
//   TLB , NI, NI, NI, NI  , NI, NI, NI,
//   NI  , NI, NI, NI, NI  , NI, NI, NI
};

// Instruction Set ( Cop0 Tlb / Co Processor 0 Tlb )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop0_Tlb[64])(void) =
{
	Op_NI,   Op_TLBR, Op_TLBWI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_TLBP, Op_NI,	  Op_NI,	Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,   Op_NI,	  Op_NI,	Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_ERET, Op_NI,	  Op_NI,    Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,   Op_NI,	  Op_NI,	Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,   Op_NI,	  Op_NI,	Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,   Op_NI,	  Op_NI,	Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,   Op_NI,	  Op_NI,	Op_NI, Op_NI, Op_NI, Op_NI, Op_NI

//* Instruction Cop0 Tlb Reference ( Taken from Mupen64 )
//   NI  , TLBR, TLBWI, NI, NI, NI, TLBWR, NI,
//   TLBP, NI  , NI   , NI, NI, NI, NI   , NI,
//   NI  , NI  , NI   , NI, NI, NI, NI   , NI,
//   ERET, NI  , NI   , NI, NI, NI, NI   , NI,
//   NI  , NI  , NI   , NI, NI, NI, NI   , NI,
//   NI  , NI  , NI   , NI, NI, NI, NI   , NI,
//   NI  , NI  , NI   , NI, NI, NI, NI   , NI,
//   NI  , NI  , NI   , NI, NI, NI, NI   , NI
};

// Instruction Set ( Cop1 / Co Processor 1 )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop1[32])(void) =
{
	Op_MFC1, Op_DMFC1, Op_CFC1, Op_NI, Op_MTC1, Op_DMTC1, Op_CTC1, Op_NI,
	Op_BC,   Op_NI,    Op_NI,   Op_NI, Op_NI,   Op_NI,    Op_NI,   Op_NI,
	Op_S,    Op_D,     Op_NI,   Op_NI, Op_W,    Op_L,     Op_NI,   Op_NI,
	Op_NI,   Op_NI,    Op_NI,   Op_NI, Op_NI,   Op_NI,    Op_NI,   Op_NI

//* Instruction Cop1 Reference ( Taken from Mupen64 )
//   MFC1, DMFC1, CFC1, NI, MTC1, DMTC1, CTC1, NI,
//   BC  , NI   , NI  , NI, NI  , NI   , NI  , NI,
//   S   , D    , NI  , NI, W   , L    , NI  , NI,
//   NI  , NI   , NI  , NI, NI  , NI   , NI  , NI
};

// Instruction Set ( Cop1 BC / Co Processor 1 BC )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop1_BC[4])(void) =
{
	Op_BC1F, Op_BC1T, Op_BC1FL, Op_BC1TL

//* Instruction Cop1 BC Reference ( Taken from Mupen64 )
//	BC1F , BC1T, BC1FL, BC1TL
};

// Instruction Set ( Cop1 Single / Co Processor 1 Single )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop1_S[64])(void) =
{
	Op_ADD_S, Op_SUB_S,   Op_MULT_S, Op_DIV_S, Op_SQRT_S,    Op_ABS_S,     Op_MOV_S, Op_NEG_S,
	Op_NI,    Op_NI,      Op_NI,     Op_NI,    Op_ROUND_W_S, Op_TRUNC_W_S, Op_NI,    Op_NI,
	Op_NI,    Op_NI,      Op_NI,     Op_NI,    Op_NI,        Op_NI,        Op_NI,    Op_NI,
	Op_NI,    Op_NI,      Op_NI,     Op_NI,    Op_NI,        Op_NI,        Op_NI,    Op_NI,
	Op_NI,    Op_CVT_D_S, Op_NI,     Op_NI,    Op_CVT_W_S,   Op_NI,        Op_NI,    Op_NI,
	Op_NI,    Op_NI,      Op_NI,     Op_NI,    Op_NI,        Op_NI,        Op_NI,    Op_NI,
	Op_CMP_S, Op_CMP_S,   Op_CMP_S,  Op_CMP_S, Op_CMP_S,     Op_CMP_S,     Op_CMP_S, Op_CMP_S,
	Op_CMP_S, Op_CMP_S,   Op_CMP_S,  Op_CMP_S, Op_CMP_S,     Op_CMP_S,     Op_CMP_S, Op_CMP_S

//* Instruction Cop1 Single Reference ( Taken from Mupen64 )
//	ADD_S    ,SUB_S    ,MUL_S   ,DIV_S    ,SQRT_S   ,ABS_S    ,MOV_S   ,NEG_S    ,
//	ROUND_L_S,TRUNC_L_S,CEIL_L_S,FLOOR_L_S,ROUND_W_S,TRUNC_W_S,CEIL_W_S,FLOOR_W_S,
//	NI       ,NI       ,NI      ,NI       ,NI       ,NI       ,NI      ,NI       ,
//	NI       ,NI       ,NI      ,NI       ,NI       ,NI       ,NI      ,NI       ,
//	NI       ,CVT_D_S  ,NI      ,NI       ,CVT_W_S  ,CVT_L_S  ,NI      ,NI       ,
//	NI       ,NI       ,NI      ,NI       ,NI       ,NI       ,NI      ,NI       ,
//	C_F_S    ,C_UN_S   ,C_EQ_S  ,C_UEQ_S  ,C_OLT_S  ,C_ULT_S  ,C_OLE_S ,C_ULE_S  ,
//	C_SF_S   ,C_NGLE_S ,C_SEQ_S ,C_NGL_S  ,C_LT_S   ,C_NGE_S  ,C_LE_S  ,C_NGT_S
};

// Instruction Set ( Cop1 Double / Co Processor 1 Double )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop1_D[64])(void) =
{
	Op_ADD_D,   Op_SUB_D, Op_MULT_D, Op_DIV_D, Op_SQRT_D,  Op_ABS_D,     Op_MOV_D, Op_NEG_D,
	Op_NI,      Op_NI,    Op_NI,     Op_NI,    Op_NI,      Op_TRUNC_W_D, Op_NI,    Op_NI,
	Op_NI,      Op_NI,    Op_NI,     Op_NI,    Op_NI,      Op_NI,        Op_NI,    Op_NI,
	Op_NI,      Op_NI,    Op_NI,     Op_NI,    Op_NI,      Op_NI,        Op_NI,    Op_NI,
	Op_CVT_S_D, Op_NI,    Op_NI,     Op_NI,    Op_CVT_W_D, Op_NI,        Op_NI,    Op_NI,
	Op_NI,      Op_NI,    Op_NI,     Op_NI,    Op_NI,      Op_NI,        Op_NI,    Op_NI,
	Op_CMP_D,   Op_CMP_D, Op_CMP_D,  Op_CMP_D, Op_CMP_D,   Op_CMP_D,     Op_CMP_D, Op_CMP_D,
	Op_CMP_D,   Op_CMP_D, Op_CMP_D,  Op_CMP_D, Op_CMP_D,   Op_CMP_D,     Op_CMP_D, Op_CMP_D

//* Instruction Cop1 Double Reference ( Taken from Mupen64 )
//	ADD_D    ,SUB_D    ,MUL_D   ,DIV_D    ,SQRT_D   ,ABS_D    ,MOV_D   ,NEG_D    ,
//	ROUND_L_D,TRUNC_L_D,CEIL_L_D,FLOOR_L_D,ROUND_W_D,TRUNC_W_D,CEIL_W_D,FLOOR_W_D,
//	NI       ,NI       ,NI      ,NI       ,NI       ,NI       ,NI      ,NI       ,
//	NI       ,NI       ,NI      ,NI       ,NI       ,NI       ,NI      ,NI       ,
//	CVT_S_D  ,NI       ,NI      ,NI       ,CVT_W_D  ,CVT_L_D  ,NI      ,NI       ,
//	NI       ,NI       ,NI      ,NI       ,NI       ,NI       ,NI      ,NI       ,
//	C_F_D    ,C_UN_D   ,C_EQ_D  ,C_UEQ_D  ,C_OLT_D  ,C_ULT_D  ,C_OLE_D ,C_ULE_D  ,
//	C_SF_D   ,C_NGLE_D ,C_SEQ_D ,C_NGL_D  ,C_LT_D   ,C_NGE_D  ,C_LE_D  ,C_NGT_D
};

// Instruction Set ( Cop1 Word / Co Processor 1 Word )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop1_W[64])(void) =
{
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_CVT_S_W, Op_CVT_D_W, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI

//* Instruction Cop1 Word Reference ( Taken from Mupen64 )
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	CVT_S_W, CVT_D_W, NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI
};

// Instruction Set ( Cop1 Long / Co Processor 1 Long )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_Cop1_L[64])(void) =
{
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_CVT_S_L, Op_CVT_D_L, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI,
	Op_NI,      Op_NI,      Op_NI, Op_NI, Op_NI, Op_NI, Op_NI, Op_NI

//* Instruction Cop1 Long Reference ( Taken from Mupen64 )
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	CVT_S_L, CVT_D_L, NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI,
//	NI     , NI     , NI, NI, NI, NI, NI, NI
};

// Instruction: No Instruction ( Unemulated / Not Suported )
//////////////////////////////////////////////////////////////////////////
void Op_NI ( void )
{
	//* Clear screen so we can read the print
	ResetDisplay();

	switch (InstrOpcode.op)
	{
	case 0: // Extra
		printf( "0x%08X: hit unknown Extended opcode %08X (op: %02d extra: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.funct);
		break;
	case 1: // Regimm
		printf( "0x%08X: hit unknown Regimm opcode %08X (op: %02d regimm: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.rt);
		break;
	case 16: // COP0
		if( InstrOpcode.rs == 16 )
			printf( "0x%08X: hit unknown Cop0 Tlb opcode %08X (op: %02d Tlb: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.funct);
		else
			printf( "0x%08X: hit unknown Cop0 opcode %08X (op: %02d cop0: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.rs);
		break;
	case 17: // COP1
		if( InstrOpcode.rs == 8 )
			printf( "0x%08X: hit unknown Cop1 BC opcode %08X (op: %02d cop1: %02d-%02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.fmt, (unsigned int)InstrOpcode.ft );
		else
			printf( "0x%08X: hit unknown Cop1 opcode %08X (op: %02d cop1: %02d-%02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.fmt, (unsigned int)InstrOpcode.funct );
		break;
	default:
		printf( "0x%08X: hit unknown opcode %08X (op: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op);
		break;
	}

	//* Dump Memory
	//DumpMemoryToFile( "rdram.bin", (char *)g_pRDRam, 0x300 );
	//PrintOpcode( 1 );

	iEmulatorRunning = 0;
}

// Instruction: Extended ( Performs Cpu Extended / Extra Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_Extended ( void )
{
	OpcodeSet_Extended[InstrOpcode.funct]();
}

// Instruction: Regimm ( Performs Cpu Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_Regimm ( void )
{
	OpcodeSet_Regimm[InstrOpcode.rt]();
}

// Instruction: J ( Jump Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_J ( void )
{
	JumpAddress = ( N64Regs->PC & 0xF0000000 ) + ( InstrOpcode.target << 2 );

	if( JumpAddress == N64Regs->PC )
		ProcPermLoop();

	N64Regs->PC += 4;
	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: JAL ( Jump And Link Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_JAL ( void )
{
	JumpAddress = ( N64Regs->PC & 0xF0000000 ) + ( InstrOpcode.target << 2 );

	N64Regs->CPU[31].DW = (long)( N64Regs->PC + 8 );

	if( JumpAddress == N64Regs->PC )
		ProcPermLoop();

	N64Regs->PC += 4;
	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: BEQ ( Branch if Equal Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BEQ ( void )
{
	N64Regs->PC += 4;

	if( N64Regs->CPU[InstrOpcode.rs].DW == N64Regs->CPU[InstrOpcode.rt].DW )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	if( JumpAddress == N64Regs->PC - 4 )
		ProcPermLoop();

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: BNE ( Branch if Not Equal Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BNE ( void )
{
	N64Regs->PC += 4;

	if( N64Regs->CPU[InstrOpcode.rs].DW != N64Regs->CPU[InstrOpcode.rt].DW )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	if( JumpAddress == N64Regs->PC - 4 )
		ProcPermLoop();

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: BLEZ ( Branch if Less than or Equal to Zero Instruction)
//////////////////////////////////////////////////////////////////////////
void Op_BLEZ ( void )
{
	N64Regs->PC += 4;

	if( N64Regs->CPU[InstrOpcode.rs].DW <= 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	if( JumpAddress == N64Regs->PC - 4 )
		ProcPermLoop();

	RunCpuInstruction();
	
	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: BGTZ ( Branch if Greater Than Zero Instruction)
//////////////////////////////////////////////////////////////////////////
void Op_BGTZ ( void )
{
	N64Regs->PC += 4;

	if( N64Regs->CPU[InstrOpcode.rs].DW > 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	if( JumpAddress == N64Regs->PC - 4 )
		ProcPermLoop();

	RunCpuInstruction();
	
	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: ADDI ( Add Immediate Instructin )
//////////////////////////////////////////////////////////////////////////
void Op_ADDI ( void )
{
	//if( InstrOpcode.rt != 0 )
	N64Regs->CPU[InstrOpcode.rt].DW = (N64Regs->CPU[InstrOpcode.rs].W[0] + ((short)InstrOpcode.immediate));
	N64Regs->PC += 4;
}

// Instruction: ADDIU ( Add Immediate Unsigned Instructin )
//////////////////////////////////////////////////////////////////////////
void Op_ADDIU ( void )
{
#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug )
	{
		printf( "ADDIU: RT[0x%08x] ", N64Regs->CPU[InstrOpcode.rs].W[0] );
		printf( "Imm[%d] ", (short)InstrOpcode.immediate );
		printf( "Val[0x%08x]\n", N64Regs->CPU[InstrOpcode.rs].W[0] + ((short)InstrOpcode.immediate) );
	}
#endif

	N64Regs->CPU[InstrOpcode.rt].DW = ( N64Regs->CPU[InstrOpcode.rs].W[0] + ((short)InstrOpcode.immediate) );
	N64Regs->PC += 4;
}

// Instruction: SLTI ( SLT Immediate Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SLTI ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW < (long long)( (short)InstrOpcode.immediate ) )
		N64Regs->CPU[InstrOpcode.rt].DW = 1;
	else
		N64Regs->CPU[InstrOpcode.rt].DW = 0;

	N64Regs->PC += 4;
}

// Instruction: SLTIU ( SLT Immediate Unsigned Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SLTIU ( void )
{
	int imm32 = (short)InstrOpcode.immediate;
	long long imm64;

	imm64 = imm32;

	if( N64Regs->CPU[InstrOpcode.rs].UDW < (QWORD)imm64 )
		N64Regs->CPU[InstrOpcode.rt].DW = 1;
	else
		N64Regs->CPU[InstrOpcode.rt].DW = 0;

	N64Regs->PC += 4;
}

// Instruction: ANDI ( And Immediate Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ANDI ( void )
{
#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug )
	{
		printf( "ANDI: 0x%08x & ", N64Regs->CPU[InstrOpcode.rs].W[0] );
		printf( "0x%08x = ", InstrOpcode.immediate );
	}
#endif

	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rs].DW & InstrOpcode.immediate;
	N64Regs->PC += 4;

#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug )
	{
		printf( "0x%08x\n", N64Regs->CPU[InstrOpcode.rt].W[0] );
	}
#endif
}

// Instruction: ORI ( Or Immediate Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ORI ( void )
{
	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rs].DW | InstrOpcode.immediate;
	N64Regs->PC += 4;
}


// Instruction: XORI ( XOr Immediate Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_XORI ( void )
{
	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rs].DW ^ InstrOpcode.immediate;
	N64Regs->PC += 4;
}

// Instruction: LUI ( LUI Cpu Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LUI ( void )
{
	//if( InstrOpcode.rt != 0 )
	N64Regs->CPU[InstrOpcode.rt].DW = (long)( (short)InstrOpcode.offset << 16 );

#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug )
	{
		printf( "LUI: RT[0x%016x] ", N64Regs->CPU[InstrOpcode.rt].DW );
		printf( "Offset[0x%08x]\n", (long)( (short)InstrOpcode.offset << 16 ) );
	}
#endif
	N64Regs->PC += 4;
}

// Instruction: BEQL ( Branch if Equal L Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BEQL ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW == N64Regs->CPU[InstrOpcode.rt].DW )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		if( JumpAddress == N64Regs->PC - 4 )
			ProcPermLoop();

		RunCpuInstruction();
		N64Regs->PC = JumpAddress;
	}
	else
	{
		N64Regs->PC += 8;
	}

	N64Regs->iCheckTimers = 1;
}

// Instruction: BNEL ( Branch if Not Equal L Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BNEL ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW != N64Regs->CPU[InstrOpcode.rt].DW )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		if( JumpAddress == N64Regs->PC - 4 )
			ProcPermLoop();

		RunCpuInstruction();
		N64Regs->PC = JumpAddress;
	}
	else
	{
		N64Regs->PC += 8;
	}

	N64Regs->iCheckTimers = 1;
}

// Instruction: BLEZL ( Branch if Less than or Equal to Zero L Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BLEZL ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW <= 0 )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		if( JumpAddress == N64Regs->PC - 4 )
			ProcPermLoop();

		RunCpuInstruction();
		N64Regs->PC = JumpAddress;
	}
	else
	{
		N64Regs->PC += 8;
	}

	N64Regs->iCheckTimers = 1;
}

// Instruction: BGTZL ( Branch if Greater than Zero L Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BGTZL ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW > 0 )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		if( JumpAddress == N64Regs->PC - 4 )
			ProcPermLoop();

		RunCpuInstruction();
		N64Regs->PC = JumpAddress;
	}
	else
	{
		N64Regs->PC += 8;
	}

	N64Regs->iCheckTimers = 1;
}

// Instruction: DADDIU ( Double Add Immediate Unsigned Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DADDIU ( void )
{
	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rs].DW + (long long)( (short)InstrOpcode.immediate );
	N64Regs->PC += 4;
}

// Instruction: LB ( Load Byte Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LB ( void )
{
	//if( InstrOpcode.rt != 0 )
	N64Regs->CPU[InstrOpcode.rt].UB[0] = LoadByte( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rt].B[0];
	
	N64Regs->PC += 4;
}

// Instruction: LH ( Load Halfword Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LH ( void )
{
	N64Regs->CPU[InstrOpcode.rt].UHW[0] = LoadWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rt].HW[0];
	
	N64Regs->PC += 4;
}

// Instruction: LWL ( Load Word Left Instruction )
//////////////////////////////////////////////////////////////////////////
DWORD LWL_MASK[4] = { 0,0xFF,0xFFFF,0xFFFFFF };
int LWL_SHIFT[4] = { 0, 8, 16, 24};

void Op_LWL ( void )
{
	DWORD Value;
	DWORD Addr = N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset;

	Value = LoadDWord( Addr & ~3 );

	N64Regs->CPU[InstrOpcode.rt].DW = (int)( N64Regs->CPU[InstrOpcode.rt].W[0] & LWL_MASK[Addr & 3] );
	N64Regs->CPU[InstrOpcode.rt].DW += (int)( Value << LWL_SHIFT[Addr & 3] );

	N64Regs->PC += 4;
}

// Instruction: LW ( Load Word Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LW ( void )
{
	//if( InstrOpcode.rt != 0 )
#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug ) //iDispPrintDebug
		printf( "LW: Addr[0x%08x] ", (DWORD)( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset ) );
#endif

	N64Regs->CPU[InstrOpcode.rt].UW[0] = LoadDWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->CPU[InstrOpcode.rt].DW = N64Regs->CPU[InstrOpcode.rt].W[0];

#ifdef DEBUGER_ENABLED	
	if( iDispPrintDebug )
		printf( "Val[0x%08x]\n", N64Regs->CPU[InstrOpcode.rt].UW[0] );
#endif

	N64Regs->PC += 4;
}

// Instruction: LBU ( Load Byte Unsigned Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LBU ( void )
{
	N64Regs->CPU[InstrOpcode.rt].UB[0] = LoadByte( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->CPU[InstrOpcode.rt].UB[1] = 0;
	N64Regs->CPU[InstrOpcode.rt].UB[2] = 0;
	N64Regs->CPU[InstrOpcode.rt].UB[3] = 0;
	N64Regs->CPU[InstrOpcode.rt].UB[4] = 0;
	N64Regs->CPU[InstrOpcode.rt].UB[5] = 0;
	N64Regs->CPU[InstrOpcode.rt].UB[6] = 0;
	N64Regs->CPU[InstrOpcode.rt].UB[7] = 0;
	//N64Regs->CPU[InstrOpcode.rt].UDW = N64Regs->CPU[InstrOpcode.rt].UB[0];
	N64Regs->PC += 4;
}

// Instruction: LHU ( Load Halfword Unsigned Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LHU ( void )
{
	N64Regs->CPU[InstrOpcode.rt].UHW[0] = LoadWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->CPU[InstrOpcode.rt].UHW[1] = 0;
	N64Regs->CPU[InstrOpcode.rt].UHW[2] = 0;
	N64Regs->CPU[InstrOpcode.rt].UHW[3] = 0;
	//N64Regs->CPU[InstrOpcode.rt].UDW = N64Regs->CPU[InstrOpcode.rt].UHW[0];
	N64Regs->PC += 4;
}

// Instruction: LWR ( Load Word Right Instruction )
//////////////////////////////////////////////////////////////////////////
DWORD LWR_MASK[4] = { 0xFFFFFF00, 0xFFFF0000, 0xFF000000, 0 };
int LWR_SHIFT[4] = { 24, 16 ,8, 0 };

void Op_LWR ( void )
{
	DWORD Value;
	DWORD Addr = N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset;

	Value = LoadDWord( Addr & ~3 );

	N64Regs->CPU[InstrOpcode.rt].DW = (int)( N64Regs->CPU[InstrOpcode.rt].W[0] & LWR_MASK[Addr & 3] );
	N64Regs->CPU[InstrOpcode.rt].DW += (int)( Value >> LWR_SHIFT[Addr & 3] );

	N64Regs->PC += 4;
}

// Instruction: LWU ( Load Word Unsigned Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LWU ( void )
{
	N64Regs->CPU[InstrOpcode.rt].UW[0] = LoadDWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->CPU[InstrOpcode.rt].UDW = N64Regs->CPU[InstrOpcode.rt].UW[0];
	N64Regs->PC += 4;
}

// Instruction: SB ( Store Byte Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SB ( void )
{
	StoreByte( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, N64Regs->CPU[InstrOpcode.rt].UB[0] ); 
	N64Regs->PC += 4;
}

// Instruction: SH ( Store Halfword Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SH ( void )
{
	StoreWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, N64Regs->CPU[InstrOpcode.rt].UHW[0] ); 
	N64Regs->PC += 4;
}

// Instruction: SWL ( Store Word Left Instruction )
//////////////////////////////////////////////////////////////////////////
DWORD SWL_MASK[4] = { 0,0xFF000000,0xFFFF0000,0xFFFFFF00 };
int SWL_SHIFT[4] = { 0, 8, 16, 24 };

void Op_SWL ( void )
{
	DWORD Value;
	DWORD Addr = N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset;

	Value = LoadDWord( Addr & ~3 );

	Value &= SWL_MASK[Addr & 3];
	Value += N64Regs->CPU[InstrOpcode.rt].UW[0] >> SWL_SHIFT[Addr & 3];

	StoreDWord( Addr & ~3, Value );

	N64Regs->PC += 4;
}

// Instruction: SW ( Store Word Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SW ( void )
{
//	if( N64Regs->CPU[InstrOpcode.rt].UW[0] == 0x11010 ) //iDispPrintDebug
//	{
//		printf( "SW: PC[0x%08x] ", N64Regs->PC );
//		printf( "Addr[0x%08x] ", N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
//		printf( "Val[0x%08x]\n", N64Regs->CPU[InstrOpcode.rt].UW[0] );
//	}

	StoreDWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, N64Regs->CPU[InstrOpcode.rt].UW[0] ); 
	N64Regs->PC += 4;
}

// Instruction: SWR ( Store Word Right Instruction )
//////////////////////////////////////////////////////////////////////////
DWORD SWR_MASK[4] = { 0x00FFFFFF,0x0000FFFF,0x000000FF,0x00000000 };
int SWR_SHIFT[4] = { 24, 16 , 8, 0  };

void Op_SWR ( void )
{
	DWORD Value;
	DWORD Addr = N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset;

	Value = LoadDWord( Addr & ~3 );

	Value &= SWR_MASK[Addr & 3];
	Value += N64Regs->CPU[InstrOpcode.rt].UW[0] << SWR_SHIFT[Addr & 3];

	StoreDWord( Addr & ~3, Value );

	N64Regs->PC += 4;
}

// Instruction: Cache ( Cache Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_CACHE ( void )
{
	//* We dont emulate the Cache instruction
	N64Regs->PC += 4;
}

// Instruction: LWC1 ( Load Word to Cop1 Register Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LWC1 ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	*(DWORD *)N64Regs->COP1Float[InstrOpcode.ft] = LoadDWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (DWORD)(short)InstrOpcode.offset );
	N64Regs->PC += 4;
}

// Instruction: LDC1 ( Load Double Word to Cop1 Register Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LDC1 ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	*(QWORD *)N64Regs->COP1Double[InstrOpcode.ft] = LoadQWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (DWORD)(short)InstrOpcode.offset );
	N64Regs->PC += 4;
}

// Instruction: LD ( Load Double Word Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_LD ( void )
{
	N64Regs->CPU[InstrOpcode.rt].UDW = LoadQWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset );
	N64Regs->PC += 4;
}

// Instruction: SWC1 ( Store Word to Cop1 Register Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SWC1 ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	StoreDWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, *(DWORD *)N64Regs->COP1Float[InstrOpcode.ft] );
	N64Regs->PC += 4;
}

// Instruction: SDC1 ( Store Double Word to Cop1 Register Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SDC1 ( void )
{
	TEST_COP1_USABLE_EXCEPTION

	StoreQWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, *(QWORD *)N64Regs->COP1Double[InstrOpcode.ft] );
	N64Regs->PC += 4;
}

// Instruction: SD ( Store Double Word Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SD ( void )
{
	//if( iDispPrintDebug )
	//	printf( "SW: Addr[0x%08x] Val[0x%08x]\n", N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, N64Regs->CPU[InstrOpcode.rt].UW[0] );
	StoreQWord( N64Regs->CPU[InstrOpcode.base].UW[0] + (short)InstrOpcode.offset, N64Regs->CPU[InstrOpcode.rt].UDW ); 
	N64Regs->PC += 4;
}

// Instruction: Extended SLL ( Shift Left L Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SLL ( void )
{
	if( InstrOpcode.Hex != 0 ) //* Incase of NOP
		N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rt].W[0] << InstrOpcode.sa;
	
	N64Regs->PC += 4;
}

// Instruction: Extended SRL ( Shift Right L Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SRL ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = (int)( N64Regs->CPU[InstrOpcode.rt].UW[0] >> InstrOpcode.sa );
	N64Regs->PC += 4;
}

// Instruction: Extended SRA ( Shift Right A Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SRA ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rt].W[0] >> InstrOpcode.sa;
	N64Regs->PC += 4;
}

// Instruction: Extended SLLV ( Shift Left LV Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SLLV ( void )
{
	//if( InstrOpcode.rd != 0 )
	N64Regs->CPU[InstrOpcode.rd].DW = ( N64Regs->CPU[InstrOpcode.rt].W[0] << ( N64Regs->CPU[InstrOpcode.rs].UW[0] & 0x1F ) );
	N64Regs->PC += 4;
}

// Instruction: Extended SRLV ( Shift Right LV Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SRLV ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = (int)( N64Regs->CPU[InstrOpcode.rt].UW[0] >> ( N64Regs->CPU[InstrOpcode.rs].UW[0] & 0x1F ) );
	N64Regs->PC += 4;
}

// Instruction: Extended SRLV ( Shift Right AV Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SRAV ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rt].W[0] >> ( N64Regs->CPU[InstrOpcode.rs].UW[0] & 0x1F );
	N64Regs->PC += 4;
}

// Instruction: JR ( Jump to Register addr Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_JR ( void )
{
	JumpAddress = N64Regs->CPU[InstrOpcode.rs].UW[0];

	if( JumpAddress == N64Regs->PC )
		ProcPermLoop();

	N64Regs->PC += 4;
	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: JALR ( Jump And Link to Register addr Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_JALR ( void )
{
	JumpAddress = N64Regs->CPU[InstrOpcode.rs].UW[0];
	N64Regs->CPU[InstrOpcode.rd].DW = (long)( N64Regs->PC + 8 );

	if( JumpAddress == N64Regs->PC )
		ProcPermLoop();

	N64Regs->PC += 4;
	RunCpuInstruction();

	N64Regs->PC = JumpAddress;
	N64Regs->iCheckTimers = 1;
}

// Instruction: SYNC ( Sync Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SYNC ( void )
{
	//* No implementation for Sync
	N64Regs->PC += 4;
}

// Instruction: Extended MFHI ( MFHI Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MFHI ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->HI.DW;
	N64Regs->PC += 4;
}

// Instruction: Extended MTHI ( MTHI Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MTHI ( void )
{
	N64Regs->HI.DW = N64Regs->CPU[InstrOpcode.rs].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended MFLO ( MFLO Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MFLO ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->LO.DW;
	N64Regs->PC += 4;
}

// Instruction: Extended MTLO ( MTLO Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MTLO ( void )
{
	N64Regs->LO.DW = N64Regs->CPU[InstrOpcode.rs].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended DSLLV ( Double Shift Left LV Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSLLV ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rt].DW << ( N64Regs->CPU[InstrOpcode.rs].UW[0] & 0x3F );
	N64Regs->PC += 4;
}

// Instruction: Extended DSRLV ( Double Shift Right LV Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSRLV ( void )
{
	N64Regs->CPU[InstrOpcode.rd].UDW = N64Regs->CPU[InstrOpcode.rt].UDW >> ( N64Regs->CPU[InstrOpcode.rs].UW[0] & 0x3F );
	N64Regs->PC += 4;
}

// Instruction: Extended DSRAV ( Double Shift Right AV Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSRAV ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rt].DW >> ( N64Regs->CPU[InstrOpcode.rs].UW[0] & 0x3F );
	N64Regs->PC += 4;
}

// Instruction: Extended MULT ( Multiply Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MULT ( void )
{
	N64Regs->HI.DW = (long long)(N64Regs->CPU[InstrOpcode.rs].W[0]) * (long long)(N64Regs->CPU[InstrOpcode.rt].W[0]);
	N64Regs->LO.DW = N64Regs->HI.W[0];
	N64Regs->HI.DW = N64Regs->HI.W[1];
	N64Regs->PC += 4;
}

// Instruction: Extended MULTU ( Multiply Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_MULTU ( void )
{
	N64Regs->HI.DW = (QWORD)N64Regs->CPU[InstrOpcode.rs].UW[0] * (QWORD)N64Regs->CPU[InstrOpcode.rt].UW[0];
	N64Regs->LO.DW = N64Regs->HI.W[0];
	N64Regs->HI.DW = N64Regs->HI.W[1];
	N64Regs->PC += 4;
}

// Instruction: Extended DIV ( Divide Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DIV ( void )
{
	if( N64Regs->CPU[InstrOpcode.rt].UDW != 0 )
	{
		N64Regs->LO.DW = N64Regs->CPU[InstrOpcode.rs].W[0] / N64Regs->CPU[InstrOpcode.rt].W[0];
		N64Regs->HI.DW = N64Regs->CPU[InstrOpcode.rs].W[0] % N64Regs->CPU[InstrOpcode.rt].W[0];
	}

	N64Regs->PC += 4;
}

// Instruction: Extended DIVU ( Divide Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DIVU ( void )
{
	if( N64Regs->CPU[InstrOpcode.rt].UDW != 0 )
	{
		N64Regs->LO.DW = (int)( N64Regs->CPU[InstrOpcode.rs].UW[0] / N64Regs->CPU[InstrOpcode.rt].UW[0] );
		N64Regs->HI.DW = (int)( N64Regs->CPU[InstrOpcode.rs].UW[0] % N64Regs->CPU[InstrOpcode.rt].UW[0] );
	}

	N64Regs->PC += 4;
}

// Instruction: Extended DMULT ( Double Multiply Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DMULT ( void )
{
	MIPS_DWORD Tmp[3];

	//* Note, JC. There has to be some way to optimise this

	N64Regs->LO.UDW = (QWORD)N64Regs->CPU[InstrOpcode.rs].UW[0] * (QWORD)N64Regs->CPU[InstrOpcode.rt].UW[0];
	Tmp[0].UDW = (long long)N64Regs->CPU[InstrOpcode.rs].W[1] * (long long)(QWORD)N64Regs->CPU[InstrOpcode.rt].UW[0];
	Tmp[1].UDW = (long long)(QWORD)N64Regs->CPU[InstrOpcode.rs].UW[0] * (long long)N64Regs->CPU[InstrOpcode.rt].W[1];
	N64Regs->HI.UDW = (long long)N64Regs->CPU[InstrOpcode.rs].W[1] * (long long)N64Regs->CPU[InstrOpcode.rt].W[1];

	Tmp[2].UDW = (QWORD)N64Regs->LO.UW[1] + (QWORD)Tmp[0].UW[0] + (QWORD)Tmp[1].UW[0];
	N64Regs->LO.UDW += ( (QWORD)Tmp[0].UW[0] + (QWORD)Tmp[1].UW[0] ) << 32;
	N64Regs->HI.UDW += (QWORD)Tmp[0].W[1] + (QWORD)Tmp[1].W[1] + Tmp[2].UW[1];

	N64Regs->PC += 4;
}

// Instruction: Extended DMULTU ( Double Multiply Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DMULTU ( void )
{
	MIPS_DWORD Tmp[3];

	//* Note, JC. There has to be some way to optimise this

	N64Regs->LO.UDW = (QWORD)N64Regs->CPU[InstrOpcode.rs].UW[0] * (QWORD)N64Regs->CPU[InstrOpcode.rt].UW[0];
	Tmp[0].UDW = (QWORD)N64Regs->CPU[InstrOpcode.rs].UW[1] * (QWORD)N64Regs->CPU[InstrOpcode.rt].UW[0];
	Tmp[1].UDW = (QWORD)N64Regs->CPU[InstrOpcode.rs].UW[0] * (QWORD)N64Regs->CPU[InstrOpcode.rt].UW[1];
	N64Regs->HI.UDW = (QWORD)N64Regs->CPU[InstrOpcode.rs].UW[1] * (QWORD)N64Regs->CPU[InstrOpcode.rt].UW[1];

	Tmp[2].UDW = (QWORD)N64Regs->LO.UW[1] + (QWORD)Tmp[0].UW[0] + (QWORD)Tmp[1].UW[0];
	N64Regs->LO.UDW += ( (QWORD)Tmp[0].UW[0] + (QWORD)Tmp[1].UW[0] ) << 32;
	N64Regs->HI.UDW += (QWORD)Tmp[0].UW[1] + (QWORD)Tmp[1].UW[1] + Tmp[2].UW[1];

	N64Regs->PC += 4;
}

// Instruction: Extended DDIV ( Double Divide Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DDIV ( void ) 
{
	if( N64Regs->CPU[InstrOpcode.rt].UDW != 0 )
	{
		N64Regs->LO.DW = N64Regs->CPU[InstrOpcode.rs].DW / N64Regs->CPU[InstrOpcode.rt].DW;
		N64Regs->HI.DW = N64Regs->CPU[InstrOpcode.rs].DW % N64Regs->CPU[InstrOpcode.rt].DW;
	}

	N64Regs->PC += 4;
}

// Instruction: Extended DDIVU ( Double Divide Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DDIVU ( void ) 
{
	if( N64Regs->CPU[InstrOpcode.rt].UDW != 0 )
	{
		N64Regs->LO.UDW = N64Regs->CPU[InstrOpcode.rs].UDW / N64Regs->CPU[InstrOpcode.rt].UDW;
		N64Regs->HI.UDW = N64Regs->CPU[InstrOpcode.rs].UDW % N64Regs->CPU[InstrOpcode.rt].UDW;
	}


	N64Regs->PC += 4;
}

// Instruction: Extended ADD ( Add Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ADD ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].W[0] + N64Regs->CPU[InstrOpcode.rt].W[0];
	N64Regs->PC += 4;
}

// Instruction: Extended ADDU ( Add Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_ADDU ( void )
{
#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug )
	{
		printf( "ADDU: 0x%08x + ", N64Regs->CPU[InstrOpcode.rs].W[0] );
		printf( "0x%08x = ", N64Regs->CPU[InstrOpcode.rt].W[0] );
	}
#endif

	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].W[0] + N64Regs->CPU[InstrOpcode.rt].W[0];
	N64Regs->PC += 4;

#ifdef DEBUGER_ENABLED
	if( iDispPrintDebug )
	{
		printf( "0x%08x\n", N64Regs->CPU[InstrOpcode.rd].W[0] );
	}
#endif
}

// Instruction: Extended SUB ( Subtract Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SUB ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].W[0] - N64Regs->CPU[InstrOpcode.rt].W[0];
	N64Regs->PC += 4;
}

// Instruction: Extended SUBU ( Subtract Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SUBU ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].W[0] - N64Regs->CPU[InstrOpcode.rt].W[0];
	N64Regs->PC += 4;
}

// Instruction: Extended AND ( And Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_AND ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].DW & N64Regs->CPU[InstrOpcode.rt].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended OR ( Or Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_OR ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].DW | N64Regs->CPU[InstrOpcode.rt].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended XOR ( XOr Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_XOR ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].DW ^ N64Regs->CPU[InstrOpcode.rt].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended NOR ( NOr Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_NOR ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = ~( N64Regs->CPU[InstrOpcode.rs].DW | N64Regs->CPU[InstrOpcode.rt].DW );
	N64Regs->PC += 4;
}

// Instruction: Extended SLT ( SLT Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SLT ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW < N64Regs->CPU[InstrOpcode.rt].DW )
		N64Regs->CPU[InstrOpcode.rd].DW = 1;
	else
		N64Regs->CPU[InstrOpcode.rd].DW = 0;

	N64Regs->PC += 4;
}

// Instruction: Extended SLTU ( SLTU Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_SLTU ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].UDW < N64Regs->CPU[InstrOpcode.rt].UDW )
		N64Regs->CPU[InstrOpcode.rd].DW = 1;
	else
		N64Regs->CPU[InstrOpcode.rd].DW = 0;

	N64Regs->PC += 4;
}

// Instruction: Extended DADD ( Double Add Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DADD ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].DW + N64Regs->CPU[InstrOpcode.rt].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended DADDU ( Double Add Unsigned Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DADDU ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = N64Regs->CPU[InstrOpcode.rs].DW + N64Regs->CPU[InstrOpcode.rt].DW;
	N64Regs->PC += 4;
}

// Instruction: Extended TEQ ( Trap if Equal Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_TEQ ( void )
{
	//* No implementation for TEQ
	N64Regs->PC += 4;
}

// Instruction: Extended DSLL ( DSLL Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSLL ( void ) 
{
	N64Regs->CPU[InstrOpcode.rd].DW = ( N64Regs->CPU[InstrOpcode.rt].DW << InstrOpcode.sa );
	N64Regs->PC += 4;
}

// Instruction: Extended DSRL ( DSRL Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSRL ( void )
{
	N64Regs->CPU[InstrOpcode.rd].UDW = ( N64Regs->CPU[InstrOpcode.rt].UDW >> InstrOpcode.sa );
	N64Regs->PC += 4;
}

// Instruction: Extended DSRA ( DSRA Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSRA ( void ) 
{
	N64Regs->CPU[InstrOpcode.rd].DW = ( N64Regs->CPU[InstrOpcode.rt].DW >> InstrOpcode.sa );
	N64Regs->PC += 4;
}

// Instruction: Extended DSLL32 ( DSLL32 Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSLL32 ( void )
{
	N64Regs->CPU[InstrOpcode.rd].DW = ( N64Regs->CPU[InstrOpcode.rt].DW << ( InstrOpcode.sa + 32 ) );
	N64Regs->PC += 4;
}

// Instruction: Extended DSRL32 ( DSRL32 Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSRL32 ( void ) 
{
	N64Regs->CPU[InstrOpcode.rd].UDW = ( N64Regs->CPU[InstrOpcode.rt].UDW >> ( InstrOpcode.sa + 32 ) );
	N64Regs->PC += 4;
}

// Instruction: Extended DSRA32 ( DSRA32 Extended Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_DSRA32 ( void ) 
{
	N64Regs->CPU[InstrOpcode.rd].DW = ( N64Regs->CPU[InstrOpcode.rt].DW >> ( InstrOpcode.sa + 32 ) );
	N64Regs->PC += 4;
}

// Instruction: Regimm BLTZ ( Branch if Less Than Zero Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BLTZ ( void )
{
	N64Regs->PC += 4;

	if( N64Regs->CPU[InstrOpcode.rs].DW < 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	if( JumpAddress == N64Regs->PC - 4 )
		ProcPermLoop();

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;

	N64Regs->iCheckTimers = 1;
}

// Instruction: Regimm BGEZ ( Branch if Greater Than or Equal to Zero Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BGEZ ( void )
{
	N64Regs->PC += 4;

	if( N64Regs->CPU[InstrOpcode.rs].DW >= 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	if( JumpAddress == N64Regs->PC - 4 )
		ProcPermLoop();

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;

	N64Regs->iCheckTimers = 1;
}

// Instruction: Regimm BLTZL ( Branch if Less Than Zero L Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BLTZL ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW < 0 )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		if( JumpAddress == N64Regs->PC - 4 )
			ProcPermLoop();

		RunCpuInstruction();
		N64Regs->PC = JumpAddress;
	}
	else
	{
		N64Regs->PC += 8;
	}

	N64Regs->iCheckTimers = 1;
}

// Instruction: Regimm BGEZL ( Branch if Greater Than or Equal to Zero L Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BGEZL ( void )
{
	if( N64Regs->CPU[InstrOpcode.rs].DW >= 0 )
	{
		N64Regs->PC += 4;
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );

		if( JumpAddress == N64Regs->PC - 4 )
			ProcPermLoop();

		RunCpuInstruction();
		N64Regs->PC = JumpAddress;
	}
	else
	{
		N64Regs->PC += 8;
	}

	N64Regs->iCheckTimers = 1;
}

// Instruction: Regimm BLTZAL ( Branch if Less Than Zero AL Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BLTZAL ( void )
{
	N64Regs->PC += 4;

	N64Regs->CPU[31].DW = (long)(N64Regs->PC + 4);

	if( N64Regs->CPU[InstrOpcode.rs].DW < 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;

	N64Regs->iCheckTimers = 1;
}

// Instruction: Regimm BGEZAL ( Branch if Greater Than or Equal to Zero AL Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BGEZAL ( void )
{
	N64Regs->PC += 4;

	N64Regs->CPU[31].DW = (long)(N64Regs->PC + 4);

	if( N64Regs->CPU[InstrOpcode.rs].DW >= 0 )
		JumpAddress = N64Regs->PC + ( (short)InstrOpcode.offset << 2 );
	else
		JumpAddress = N64Regs->PC + 4;

	RunCpuInstruction();

	N64Regs->PC = JumpAddress;

//	if( N64Regs->PC = 0x800001C4 )
//		N64Regs->PC += 8;

	N64Regs->iCheckTimers = 1;
}

// Instruction: Cop0 ( Perferform Cop0 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_Cop0 ( void )
{
	OpcodeSet_Cop0[InstrOpcode.rs]();
}

// Instruction: Cop0 Tlb ( Perferform Cop0 Tlb Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_Tlb ( void )
{
	OpcodeSet_Cop0_Tlb[InstrOpcode.funct]();
}

// Instruction: Cop1 ( Perferform Cop1 Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_Cop1 ( void )
{
	OpcodeSet_Cop1[InstrOpcode.fmt]();
}

// Instruction: Cop1 BC ( Perferform Cop1 BC Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_BC()
{
   OpcodeSet_Cop1_BC[InstrOpcode.ft]();
}

// Instruction: Cop1 Signle ( Perferform Cop1 Single Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_S()
{
	OpcodeSet_Cop1_S[InstrOpcode.funct]();
}

// Instruction: Cop1 Double ( Perferform Cop1 Double Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_D()
{
	OpcodeSet_Cop1_D[InstrOpcode.funct]();
}

// Instruction: Cop1 Word ( Perferform Cop1 Word Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_W()
{
	OpcodeSet_Cop1_W[InstrOpcode.funct]();
}

// Instruction: Cop1 Long ( Perferform Cop1 Long Instruction )
//////////////////////////////////////////////////////////////////////////
void Op_L()
{
	OpcodeSet_Cop1_L[InstrOpcode.funct]();
}

// Run Cpu Instruction
//////////////////////////////////////////////////////////////////////////
void RunCpuInstruction( void )
{
	//* Update Count Register
	N64Regs->COP0[9] += 2; //CountPerOp;
	N64SysTimers.iCurrentTime -= 2; //CountPerOp;

	//* ?? 
	N64Regs->COP0[0] -= 1;
	if( (int)N64Regs->COP0[0] < (int)N64Regs->COP0[6] )
		N64Regs->COP0[0] = 31;

	//* Clear R0 incase writen to
	N64Regs->CPU[0].DW = 0;

	//* Set Jump Flag
	iNextJump = 1;

	//* Fetch Opcode
	InstrOpcode.Hex = LoadDWord( N64Regs->PC );

//	PrintOpcode();

	//* Execute Opcode
	OpcodeSet_Core[InstrOpcode.op]();

	//* Clear Jump Flag
	iNextJump = 0;
}

// Run Cpu Emulation
//////////////////////////////////////////////////////////////////////////
int RunCpu( void )
{
	DWORD PrevPCC = N64Regs->PC;
	SceCtrlData PadInput;
	int iCycleTick = 0;

	iStepLoop = 0;
	//* Set Loop state to true
	iEmulatorRunning = 1;
	iPauseOnExit = 1;
	iPrintDebug = 0;
	iDispPrintDebug = 0;

	while( iEmulatorRunning )
	{
		//* Update Count Register
		N64Regs->COP0[9] += 2; //CountPerOp;
		N64SysTimers.iCurrentTime -= 2; //CountPerOp;

		//* ?? 
		N64Regs->COP0[0] -= 1;
		if( (int)N64Regs->COP0[0] < (int)N64Regs->COP0[6] )
			N64Regs->COP0[0] = 31;

#ifdef DEBUGER_ENABLED
		if( ( N64Regs->PC == BreakAddress ) )
		{
			iStepLoop = 1;
			//iDispPrintDebug = 1;
		}
#endif
		//N64Regs->PrevPC = PrevPCC;
		//PrevPCC = N64Regs->PC;

		//* Fetch Opcode
		InstrOpcode.Hex = LoadDWord( N64Regs->PC );
#ifdef DEBUGER_ENABLED
		if( iStepLoop )
		{
			PrintOpcode( 0 );

			//* Execute Opcode
			OpcodeSet_Core[InstrOpcode.op]();

			printf( "New PC: 0x%08x - Prv PC: 0x%08x\n\n", N64Regs->PC, N64Regs->PrevPC );

			if( iWaitForInputTrig )
			{
				if( WaitForInput( PSP_CTRL_CROSS ) )
					iEmulatorRunning = 0;
			}
		}
		else
		{
#endif
			//* Execute Opcode
			OpcodeSet_Core[InstrOpcode.op]();
#ifdef DEBUGER_ENABLED
		}
#endif

		//* Clear R0 incase writen to
		N64Regs->CPU[0].DW = 0;

		if( N64Regs->iCheckTimers )
		{
			N64Regs->iCheckTimers = 0;
			CheckTimers();
		}

		if( N64Regs->iPerformInterupt )
		{
			N64Regs->iPerformInterupt = 0;
			PerformIntrException( 0 );
		}
	}

//	printf( "Status Register: 0x%08x\n", N64Regs->COP0[12] );
//	printf( "MI Register: 0x%08x\n", N64Regs->MI[2] );
	printf( "Emulation stoped at <0x%08x>\n", (int)N64Regs->PC );

	if( iPauseOnExit )
		WaitForInput( PSP_CTRL_CROSS );

	return 1;
}

// Process Permanate Loop
//////////////////////////////////////////////////////////////////////////
void ProcPermLoop ( void ) 
{
	if( N64Regs->iPerformInterupt ) 
		return;

	N64SysTimers.iCurrentTime -= 5; //CountPerOp;
	N64Regs->COP0[9] += 5; //CountPerOp;

	//* Interrupts enabled ?
	if (( N64Regs->COP0[12] & 0x00000001 ) == 0 ) { goto InterruptsDisabled; }
	if (( N64Regs->COP0[12] & 0x00000002 ) != 0 ) { goto InterruptsDisabled; }
	if (( N64Regs->COP0[12] & 0x00000004 ) != 0 ) { goto InterruptsDisabled; }
	if (( N64Regs->COP0[12] & 0xFF00) == 0) { goto InterruptsDisabled; }

	//* Speed up timer
	if( N64SysTimers.iCurrentTime > 0 ) 
	{
		N64Regs->COP0[9] += N64SysTimers.iCurrentTime + 1;
		N64SysTimers.iCurrentTime = -1;
	}

	//* Exit Function
	return;

InterruptsDisabled:
	printf( "Emulator is stuck in a permanent loop it can not get out of!\n" );
	printf( "Status Register: 0x%08x\n", N64Regs->COP0[12] );
	iEmulatorRunning = 0;
}

// Print Executed Opcode
//////////////////////////////////////////////////////////////////////////
void PrintOpcode ( int iClose )
{
	switch( InstrOpcode.op )
	{
		case 0: // Extra
			printf( "0x%08X: hit Extended opcode %08X (op: %02d extra: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.funct);
			break;
		case 1: // Regimm
			printf( "0x%08X: hit Regimm opcode %08X (op: %02d regimm: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.rt);
			break;
		case 16: // COP0
			if( InstrOpcode.rs == 16 )
				printf( "0x%08X: hit Cop0 Tlb opcode %08X (op: %02d Tlb: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.funct);
			else
				printf( "0x%08X: hit Cop0 opcode %08X (op: %02d cop0: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.rs);
			break;
		case 17: // COP1
			if( InstrOpcode.rs == 8 )
				printf( "0x%08X: hit Cop1 BC opcode %08X (op: %02d cop1: %02d-%02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.fmt, (unsigned int)InstrOpcode.ft );
			else
				printf( "0x%08X: hit Cop1 opcode %08X (op: %02d cop1: %02d-%02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op, (unsigned int)InstrOpcode.fmt, (unsigned int)InstrOpcode.funct );
			break;
		default:
			printf( "0x%08X: hit opcode %08X (op: %02d)\n", (unsigned int)N64Regs->PC, (unsigned int)InstrOpcode.Hex, (unsigned int)InstrOpcode.op);
			break;
	}
}
