//////////////////////////////////////////////////////////////////////////
// Registers
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the n64 regisers

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

// Tlb Registers (Borrowed from Mupen64)
//////////////////////////////////////////////////////////////////////////
//* Taken from Mupen64
typedef struct
{
	short mask;
	long vpn2;
	char g;
	unsigned char asid;
	long pfn_even;
	char c_even;
	char d_even;
	char v_even;
	long pfn_odd;
	char c_odd;
	char d_odd;
	char v_odd;
	char r;
	//long check_parity_mask;

	unsigned long start_even;
	unsigned long end_even;
	unsigned long phys_even;
	unsigned long start_odd;
	unsigned long end_odd;
	unsigned long phys_odd;
} N64_TLB;

typedef struct
{
	DWORD*		R[0x100000];
	DWORD*		W[0x100000];
} N64_TLBFAST;

// Register Types (Borrowed from Pj64)
//////////////////////////////////////////////////////////////////////////
typedef struct 
{
	//* Program Counter
	DWORD		PC;
	DWORD*		RspPC;
	DWORD		RspHlePC[10];
	DWORD		RspPCi;
	DWORD		PrevPC;

	//* Cpu & Cop Registers
	MIPS_DWORD	CPU[32];
	DWORD		COP0[33];
	MIPS_DWORD	COP1[32];
	DWORD		COP1CR[32];
	DWORD		AudioIntrReg;

	//* Cop1 Float Pointers
	void*		COP1Float[32];
	void*		COP1Double[32];

	//* Rsp Registers
//	MIPS_WORD   RSP[32];
//	MIPS_WORD   RSPFlags[4];
//	MIPS_DWORD  RSPAccum[8];
//	MIPS_VECTOR RSPVect[32];
//	MIPS_DWORD	RSPEleSpec[32];
//	MIPS_DWORD	RSPIndx[32];
//	MIPS_WORD	RSPRecp;
//	MIPS_WORD	RSPRecpResult;

//	DWORD		RSPCycleCount;

	//* Hi & Lo Registers
	MIPS_DWORD	HI;
	MIPS_DWORD	LO;

	//* Memory & Other Registers
	DWORD		RDRAM[10];
	DWORD		SP[10];
	DWORD		DPC[10];
	DWORD		MI[4];
	DWORD		VI[14];
	DWORD		AI[6];
	DWORD		PI[13];
	DWORD		RI[8];
	DWORD		SI[4];

	//* Tlb Registers
	N64_TLB		Tlb[32];
	//N64_TLBFAST	Tlb_Lut;
	DWORD*		Tlb_Lut_R; //[0x100000];
	DWORD*		Tlb_Lut_W; //[0x100000];

	//* Pif Ram
	BYTE		PIF_Mem[0x7FF];
	BYTE*		PIF_Ram;

	//* Diplay & Audio List Count
	int DlistCount;
	int AlistCount;

	//* Rounding Model
	int			iRoundingModel;

	//* Cpu States
	int			iCheckTimers;
	int			iPerformInterupt;
	int			iCheckInterupt;

	//* Rom State
	int			iCicChipID;
	int			iCountryID;
	int			iFirstDmaWrite;

	//* Texture Memory
	DWORD*		TMemPal;
	QWORD*		TMem;
	DWORD*		TCache;

} N64_REGISTERS;

// Global Varibles
//////////////////////////////////////////////////////////////////////////
extern N64_REGISTERS* N64Regs;

// Functions
//////////////////////////////////////////////////////////////////////////

//* Set Fpu Locations
void SetFpuLocations( void );

//* Init Registers
int InitRegisters( void );

#endif
