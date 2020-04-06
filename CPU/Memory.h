//////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the n64 memory

#ifndef _MEMORY_H_
#define _MEMORY_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

// Global Objects
//////////////////////////////////////////////////////////////////////////
extern BYTE* g_pRomImage;
extern BYTE* g_pRDRam;
extern BYTE* g_pDMem;
extern BYTE* g_pIMem;
extern int   g_iRomLen;
extern BYTE* g_pTMemPal;
extern BYTE* g_pTMem;
extern BYTE* g_pTlbR;
extern BYTE* g_pTlbW;
extern int   g_iUseFastTlb;

/*
extern char* SP_Registers;			// 0400000h
extern char* DP_Cmd_Registers;		// 0410000h
extern char* DP_Span_Registers;		// 0420000h
extern char* MI_Registers;			// 0430000h
extern char* VI_Registers;			// 0440000h
extern char* AI_Registers;			// 0450000h
extern char* PI_Registers;			// 0460000h
extern char* RI_Registers;			// 0470000h
extern char* SI_Registers;			// 0480000h
*/
// Functions
//////////////////////////////////////////////////////////////////////////

//* Allocate space for rom image, Function returns pointer addr
char* AllocateRomMemory( int iSize );

//* Init memory system
int InitMemorySystem( void );

//* Free all allocated memory
void FreeAllocatedMemory( int iFreeRom );

//* Store RDRam Size
void FirstDMA ( void );

//* To copy chunks of data to memory (unbyteswaped)
void CopyToN64Memory( void* pData, DWORD uAddr, unsigned int iSize );

//* Load Memory
BYTE LoadByte( DWORD uAddr );
WORD LoadWord( DWORD uAddr );
DWORD LoadDWord( DWORD uAddr );
QWORD LoadQWord( DWORD uAddr );

//* Store Memory
void StoreByte( DWORD uAddr, BYTE WriteData );
void StoreWord( DWORD uAddr, WORD WriteData );
void StoreDWord( DWORD uAddr, DWORD WriteData );
void StoreQWord( DWORD uAddr, QWORD WriteData );

//* Translate Tlb Addr
DWORD TlbTranslateAddr( DWORD addresse, int iWrite );

#endif
