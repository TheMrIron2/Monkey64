//////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the n64 memory

#include "Registers.h"
#include "Memory.h"
#include "Exceptions.h"
#include "Dma.h"
#include "Pif.h"
#include "Video.h"
#include "Instructions/Instructions.h"

// External Calls
//////////////////////////////////////////////////////////////////////////
extern void RunRsp ( void );
extern int WaitForInput ( u16 uInputButton );

// Varibles
//////////////////////////////////////////////////////////////////////////
BYTE* g_pRomImage = NULL;
BYTE* g_pRDRam = NULL;
BYTE* g_pDMem = NULL;
BYTE* g_pIMem = NULL;
BYTE* g_pTMemPal = NULL;
BYTE* g_pTMem = NULL;
BYTE* g_pTlbR = NULL;
BYTE* g_pTlbW = NULL;

int   g_iRomLen;
int   g_iUseFastTlb = 0;

/*
char* RDRam_Registers;
char* SP_Registers;			// 0400000h
char* DP_Cmd_Registers;		// 0410000h
char* DP_Span_Registers;	// 0420000h
char* MI_Registers;			// 0430000h
char* VI_Registers;			// 0440000h
char* AI_Registers;			// 0450000h
char* PI_Registers;			// 0460000h
char* RI_Registers;			// 0470000h
char* SI_Registers;			// 0480000h
*/

DWORD DWReadReturn;
DWORD RealAddr;

extern int iEmulatorRunning;

// Init Rom Memory
//////////////////////////////////////////////////////////////////////////
char* AllocateRomMemory( int iSize )
{
	//* Verify that the size is not larger then 16MB
	if( iSize > 16*1024*1024 )
		return NULL;

	//* Free Previous Rom
	if( g_pRomImage )
	{
		free( g_pRomImage );
		g_pRomImage = NULL;
	}

	//* Store Size
	g_iRomLen = iSize;

	//* Allocate memory
	g_pRomImage = (BYTE *)malloc( g_iRomLen );

	//* Verify if valid
	if( !g_pRomImage )
		return NULL;

	//* Clear memory
	memset( g_pRomImage, 0, g_iRomLen );

	//* Return rom memory pointer
	return (char *)g_pRomImage;
}

// Init Memory
//////////////////////////////////////////////////////////////////////////
int InitMemorySystem( void )
{
	//* Allocate space
	N64Regs = (N64_REGISTERS *)malloc( sizeof(N64_REGISTERS) );
	RdpCurState = (RdpState *)malloc( sizeof(RdpState) );
	g_pRDRam = (BYTE *)malloc( 0x400000 );
	g_pDMem = (BYTE *)malloc( 0x2000 );
	g_pIMem = g_pDMem + 0x1000; //* note, Both are alligned to make for faster reads & writes
	//g_pTMemPal = (BYTE *)malloc( 256 * 4 );
	//g_pTMem = (BYTE *)malloc( 512 * 8 );

	if( g_iUseFastTlb )
	{
		g_pTlbR = (BYTE *)malloc( 0x100000 * 4 );
		g_pTlbW = (BYTE *)malloc( 0x100000 * 4 );

		if( !g_pTlbR || !g_pTlbW )
		{
			FreeAllocatedMemory( 0 );
			return 0;
		}

		memset( g_pTlbR, 0, 0x100000 * 4 );
		memset( g_pTlbW, 0, 0x100000 * 4 );
	}
	
	//* Verify
	if( !N64Regs || !RdpCurState || !g_pRDRam || !g_pDMem || !g_pIMem /*|| !g_pTMemPal || !g_pTMem*/ )
	{
		FreeAllocatedMemory( 0 );
		return 0;
	}

	//* Clear memory
	memset( g_pRDRam, 0, 0x400000 );
	memset( g_pDMem, 0, 0x1000 );
	memset( g_pIMem, 0, 0x1000 );
	//memset( g_pTMemPal, 0, 256*4 );
	//memset( g_pTMem, 0, 512*8 );

	//* Setup Register Pointers
/*
	DP_Cmd_Registers = (char *)&N64Regs->DPC[0];
	RDRam_Registers = (char *)&N64Regs->RDRAM[0];
	MI_Registers = (char *)&N64Regs->MI[0];
	VI_Registers = (char *)&N64Regs->VI[0];
	AI_Registers = (char *)&N64Regs->AI[0];
	PI_Registers = (char *)&N64Regs->PI[0];
	RI_Registers = (char *)&N64Regs->RI[0];
	SI_Registers = (char *)&N64Regs->SI[0];
	SP_Registers = (char *)&N64Regs->SP[0];
*/
	return 1;
}

// Free Allocated Memory
//////////////////////////////////////////////////////////////////////////
void FreeAllocatedMemory( int iFreeRom  )
{
	if( g_pTlbW )		{ free( g_pTlbW );		g_pTlbW = NULL; }
	if( g_pTlbR )		{ free( g_pTlbR );		g_pTlbR = NULL; }
	//if( g_pTMem )		{ free( g_pTMem );		g_pTMem = NULL; }
	//if( g_pTMemPal )	{ free( g_pTMemPal );	g_pTMemPal = NULL; }
	if( g_pDMem )		{ free( g_pDMem );		g_pDMem = NULL; }
	if( g_pRDRam )		{ free( g_pRDRam );		g_pRDRam = NULL; }
	if( RdpCurState )	{ free( RdpCurState );	RdpCurState = NULL; }
	if( N64Regs )		{ free( N64Regs );		N64Regs = NULL; }
	if( g_pRomImage && iFreeRom )	{ free( g_pRomImage );	g_pRomImage = NULL; }
}

// Copy Chunk of Data to Memory
//////////////////////////////////////////////////////////////////////////
void CopyToN64Memory( void* pData, DWORD uAddr, unsigned int iSize )
{
	if( uAddr <= 0x003fffff )
	{
		memcpy( g_pRDRam + ( uAddr & 0x003fffff ), pData, iSize );
	}
	else if( uAddr >= 0x04000000 && uAddr <  0x04002000 )
	{
		memcpy( g_pDMem + ( uAddr & 0x00001FFF ), pData, iSize );
	}
	else if( uAddr >= 0x1FC007C0 )
	{
		int iPifOffset = ( uAddr & 0x7FF ) - 0x7C0;
		
		memcpy( N64Regs->PIF_Ram + iPifOffset, pData, iSize );
	}
}

// Byteswap Memcpy
//////////////////////////////////////////////////////////////////////////
inline void bs_memcpy16( void* out, void* in )
{
	((char *)out)[0] = ((char*)in)[1];
	((char *)out)[1] = ((char*)in)[0];
}

inline void bs_memcpy32( void* out, void* in )
{
	((char *)out)[0] = ((char*)in)[3];
	((char *)out)[1] = ((char*)in)[2];
	((char *)out)[2] = ((char*)in)[1];
	((char *)out)[3] = ((char*)in)[0];
}

inline void bs_memcpy64( void* out, void* in )
{
	((char *)out)[0] = ((char*)in)[3];
	((char *)out)[1] = ((char*)in)[2];
	((char *)out)[2] = ((char*)in)[1];
	((char *)out)[3] = ((char*)in)[0];

	((char *)out)[4] = ((char*)in)[7];
	((char *)out)[5] = ((char*)in)[6];
	((char *)out)[6] = ((char*)in)[5];
	((char *)out)[7] = ((char*)in)[4];
}

// First Dma Access
//////////////////////////////////////////////////////////////////////////
void FirstDMA ( void )
{
	DWORD RDRam_Len = 0x400000;

	switch( N64Regs->iCicChipID )
	{
		case 1: *(DWORD *)&g_pRDRam[0x318] = RDRam_Len; break;
		case 2: *(DWORD *)&g_pRDRam[0x318] = RDRam_Len; break;
		case 3: *(DWORD *)&g_pRDRam[0x318] = RDRam_Len; break;
		case 5: *(DWORD *)&g_pRDRam[0x3F0] = RDRam_Len; break;
		case 6: *(DWORD *)&g_pRDRam[0x318] = RDRam_Len; break;
	}
}

// Load Byte ( 8bits )
//////////////////////////////////////////////////////////////////////////
BYTE LoadByte( DWORD uAddr )
{
	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 0 );

		if( uAddr == 0x0 )
			return 0;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		return g_pRDRam[ uAddr ^ 3 ];
	}
	else if( uAddr >= 0x10000000 && uAddr < 0x16000000 )
	{
		//if( ( uAddr & 2 ) == 0 ) { uAddr = (uAddr + 4) ^ 2; }

		if( ( uAddr - 0x10000000 ) < g_iRomLen ) 
			return g_pRomImage[ (uAddr ^ 3) - 0x10000000 ];
	}
	
	//* Print Error
//	printf( "Error: Illegal 8bit Memory Read at 0x%08x <PC:0x%08x>\n", (unsigned int)uAddr, (unsigned int)N64Regs->PC );
//	iEmulatorRunning = 0;

	return 0;
}

// Load Word ( 16bits )
//////////////////////////////////////////////////////////////////////////
WORD LoadWord( DWORD uAddr )
{
	WORD ReadReturn = 0;

	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 0 );

		if( uAddr == 0x0 )
			return 0;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		return *(WORD *)&g_pRDRam[ uAddr ^ 2 ];
	}
	else if( uAddr >= 0x10000000 && uAddr < 0x16000000 )
	{
		if( ( uAddr - 0x10000000 ) < g_iRomLen ) 
			return *(WORD *)&g_pRomImage[ (uAddr ^ 2) - 0x10000000 ];
	}

	//* Print Error
//	printf( "Error: Illegal 16bit Memory Read at 0x%08x <PC:0x%08x>\n", (unsigned int)uAddr, (unsigned int)N64Regs->PC );
//	iEmulatorRunning = 0;

	return 0;
}

// Load Double Word ( 32bits )
//////////////////////////////////////////////////////////////////////////
DWORD LoadDWord( DWORD uAddr )
{
	DWReadReturn = 0;
	RealAddr = uAddr;

	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 0 );

		if( uAddr == 0x0 )
			return 0;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		return *(DWORD *)&g_pRDRam[uAddr];
	}
	else if( uAddr >= 0x10000000 )
	{
		if (uAddr >= 0x1FC00000 && uAddr <= 0x1FC007FF)
		{
			bs_memcpy32( &DWReadReturn, &N64Regs->PIF_Mem[ uAddr - 0x1FC00000 ] );
			return DWReadReturn;
		}
		
		if( ( uAddr - 0x10000000 ) < g_iRomLen ) 
			return *(DWORD *)&g_pRomImage[ uAddr - 0x10000000 ];

		return 0;
	}
	
	switch( uAddr & 0xFFF00000 )
	{
		case 0x03f00000:
		{
			switch( uAddr ) 
			{
				case 0x03F00000: DWReadReturn = N64Regs->RDRAM[0]; break;
				case 0x03F00004: DWReadReturn = N64Regs->RDRAM[1]; break;
				case 0x03F00008: DWReadReturn = N64Regs->RDRAM[2]; break;
				case 0x03F0000C: DWReadReturn = N64Regs->RDRAM[3]; break;
				case 0x03F00010: DWReadReturn = N64Regs->RDRAM[4]; break;
				case 0x03F00014: DWReadReturn = N64Regs->RDRAM[5]; break;
				case 0x03F00018: DWReadReturn = N64Regs->RDRAM[6]; break;
				case 0x03F0001C: DWReadReturn = N64Regs->RDRAM[7]; break;
				case 0x03F00020: DWReadReturn = N64Regs->RDRAM[8]; break;
				case 0x03F00024: DWReadReturn = N64Regs->RDRAM[9]; break;
			}

			return DWReadReturn;
		}
		break;

		case 0x04000000:
		{
			if( uAddr < 0x04002000 )
			{
				return *(DWORD *)&g_pDMem[ uAddr & 0x1FFF ];
			}

			switch( uAddr )
			{
				case 0x04040010: return N64Regs->SP[4]; break;
				case 0x04040014: return N64Regs->SP[5]; break;
				case 0x04040018: return N64Regs->SP[6]; break;
				case 0x04080000: return N64Regs->SP[8]; break;
			}

			return 0;
		}
		break;

		case 0x04100000:
		{
			switch( uAddr )
			{
				case 0x0410000C: DWReadReturn = N64Regs->DPC[3]; break;
				case 0x04100010: DWReadReturn = N64Regs->DPC[4]; break;
				case 0x04100014: DWReadReturn = N64Regs->DPC[5]; break;
				case 0x04100018: DWReadReturn = N64Regs->DPC[6]; break;
				case 0x0410001C: DWReadReturn = N64Regs->DPC[7]; break;
			}

			return DWReadReturn;
		}
		break;

		case 0x04300000:
		{
			switch( uAddr )
			{ 
				case 0x04300000: return N64Regs->MI[0]; break;
				case 0x04300004: return N64Regs->MI[1]; break;
				case 0x04300008: return N64Regs->MI[2]; break;
				case 0x0430000C: return N64Regs->MI[3]; break;
			}

			return 0;
		}
		break;

		case 0x04400000:
		{
			switch( uAddr )
			{
				case 0x04400000: return N64Regs->VI[0]; break;
				case 0x04400004: return N64Regs->VI[1]; break;
				case 0x04400008: return N64Regs->VI[2]; break;
				case 0x0440000C: return N64Regs->VI[3]; break;
				case 0x04400014: return N64Regs->VI[5]; break;
				case 0x04400018: return N64Regs->VI[6]; break;
				case 0x0440001C: return N64Regs->VI[7]; break;
				case 0x04400020: return N64Regs->VI[8]; break;
				case 0x04400024: return N64Regs->VI[9]; break;
				case 0x04400028: return N64Regs->VI[10]; break;
				case 0x0440002C: return N64Regs->VI[11]; break;
				case 0x04400030: return N64Regs->VI[12]; break;
				case 0x04400034: return N64Regs->VI[13]; break;

				case 0x04400010: 
					ViUpdateHalfLine();
					return ViHalfLine;
					break;
			}

			return 0;
		}
		break;

		case 0x04500000:
		{
			switch( uAddr )
			{
				case 0x04500004: return 0; break;
				case 0x0450000C: return N64Regs->AI[3]; break;
			}
			
			return 0;
		}
		break;

		case 0x04600000:
		{
			switch( uAddr )
			{
				case 0x04600010: return N64Regs->PI[4]; break;
				case 0x04600014: return N64Regs->PI[5]; break;
				case 0x04600018: return N64Regs->PI[6]; break;
				case 0x0460001C: return N64Regs->PI[7]; break;
				case 0x04600020: return N64Regs->PI[8]; break;
				case 0x04600024: return N64Regs->PI[9]; break;
				case 0x04600028: return N64Regs->PI[10]; break;
				case 0x0460002C: return N64Regs->PI[11]; break;
				case 0x04600030: return N64Regs->PI[12]; break;
			}

			return 0;
		}
		break;

		case 0x04700000:
		{
			switch( uAddr )
			{
				case 0x04700000: return N64Regs->RI[0]; break;
				case 0x04700004: return N64Regs->RI[1]; break;
				case 0x04700008: return N64Regs->RI[2]; break;
				case 0x0470000C: return N64Regs->RI[3]; break;
				case 0x04700010: return N64Regs->RI[4]; break;
				case 0x04700014: return N64Regs->RI[5]; break;
				case 0x04700018: return N64Regs->RI[6]; break;
				case 0x0470001C: return N64Regs->RI[7]; break;
			}

			return 0;
		}
		break;
		
		case 0x04800000:
		{
			switch( uAddr )
			{
				case 0x04800018: return N64Regs->SI[3]; break;
			}

			return 0;
		}
		break;
	}

	if( uAddr >= 0x05000000 && uAddr <= 0x05FFFFFF )
	{
		DWReadReturn = uAddr & 0xFFFF;
		DWReadReturn = (DWReadReturn << 16) | DWReadReturn;

		return DWReadReturn;
	}

	if( uAddr >= 0x08000000 && uAddr <= 0x08FFFFFF )
	{
		DWReadReturn = uAddr & 0xFFFF;
		DWReadReturn = (DWReadReturn << 16) | DWReadReturn;

		return DWReadReturn;
	}

	//* Print Error
	printf( "Error: Illegal 32bit Memory Read at 0x%08x <PC:0x%08x>\n", (unsigned int)RealAddr, (unsigned int)N64Regs->PC );
	WaitForInput( PSP_CTRL_CROSS );
//	iEmulatorRunning = 0;

	return 0;
}

// Load Quad Word ( 64 bits )
//////////////////////////////////////////////////////////////////////////
QWORD LoadQWord( DWORD uAddr )
{
	DWORD ReadReturn[2];

	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 0 );

		if( uAddr == 0x0 )
			return 0;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		ReadReturn[1] = *(DWORD *)&g_pRDRam[uAddr];
		ReadReturn[0] = *(DWORD *)&g_pRDRam[uAddr+4];

		return *(QWORD *)&ReadReturn;
	}
	else if( uAddr >= 0x10000000 && uAddr < 0x16000000 )
	{
		if( ( uAddr - 0x10000000 ) < g_iRomLen ) 
		{
			uAddr -= 0x10000000;
			ReadReturn[1] = *(DWORD *)&g_pRomImage[uAddr];
			ReadReturn[0] = *(DWORD *)&g_pRomImage[uAddr + 4];
			
			return *(QWORD *)&ReadReturn;
		}
	}

	//* Print Error
//	printf( "Error: Illegal 64bit Memory Read at 0x%08x <PC:0x%08x>\n", (unsigned int)uAddr, (unsigned int)N64Regs->PC );
//	iEmulatorRunning = 0;

	return 0;
}

// Store Byte ( 8bits )
//////////////////////////////////////////////////////////////////////////
void StoreByte( DWORD uAddr, BYTE WriteData )
{
	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 1 );

		if( uAddr == 0x0 )
			return;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		g_pRDRam[ uAddr ^ 3 ] = WriteData;
		return;
	}

	//* Print Error
//	printf( "Error: Illegal 8bit Memory Write at 0x%08x <PC:0x%08x>\n", (unsigned int)uAddr, (unsigned int)N64Regs->PC );
//	iEmulatorRunning = 0;

	return;
}

// Store Word ( 16bits )
//////////////////////////////////////////////////////////////////////////
void StoreWord( DWORD uAddr, WORD WriteData )
{
	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 1 );

		if( uAddr == 0x0 )
			return;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		*(WORD *)&g_pRDRam[uAddr ^ 2] = WriteData;
		return;
	}

	//* Print Error
//	printf( "Error: Illegal 16bit Memory Write at 0x%08x <PC:0x%08x>\n", (unsigned int)uAddr, (unsigned int)N64Regs->PC );

	return;
}

// Store Double Word ( 32bits )
//////////////////////////////////////////////////////////////////////////
void StoreDWord( DWORD uAddr, DWORD WriteData )
{
	RealAddr = uAddr;

	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 1 );

		if( uAddr == 0x0 )
			return;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		*(DWORD *)&g_pRDRam[uAddr] = WriteData;
		return;
	}
	else if( uAddr >= 0x10000000 )
	{
		if (uAddr >= 0x1FC007C0 && uAddr <= 0x1FC007FF)
		{
			bs_memcpy32( &N64Regs->PIF_Ram[ uAddr - 0x1FC007C0 ], &WriteData );

			if( uAddr == 0x1FC007FC )
				PifRamWrite();
		}

		//* Ignore rom & pif_rom writes
		return;
	}
	
	switch( uAddr & 0xFFF00000 )
	{
		case 0x03f00000:
		{
			switch( uAddr )
			{
				case 0x03F00000: N64Regs->RDRAM[0] = WriteData; break;
				case 0x03F00004: N64Regs->RDRAM[1] = WriteData; break;
				case 0x03F00008: N64Regs->RDRAM[2] = WriteData; break;
				case 0x03F0000C: N64Regs->RDRAM[3] = WriteData; break;
				case 0x03F00010: N64Regs->RDRAM[4] = WriteData; break;
				case 0x03F00014: N64Regs->RDRAM[5] = WriteData; break;
				case 0x03F00018: N64Regs->RDRAM[6] = WriteData; break;
				case 0x03F0001C: N64Regs->RDRAM[7] = WriteData; break;
				case 0x03F00020: N64Regs->RDRAM[8] = WriteData; break;
				case 0x03F00024: N64Regs->RDRAM[9] = WriteData; break;

				case 0x03F04004: 
				case 0x03F08004:
				case 0x03F80004:
				case 0x03F80008:
				case 0x03F8000C:
				case 0x03F80014:
					return;
					break; 
			}

			return;
		}
		break;

		case 0x04000000:
		{
			if( uAddr < 0x04002000 )
			{
				*(DWORD *)&g_pDMem[ uAddr & 0x1FFF ] = WriteData;
				return;
			}

			switch( uAddr )
			{
				case 0x04040000: N64Regs->SP[0] = WriteData; break;
				case 0x04040004: N64Regs->SP[1] = WriteData; break;
				case 0x04040008: N64Regs->SP[2] = WriteData; DmaSpRead();  break;
				case 0x0404000C: N64Regs->SP[3] = WriteData; DmaSpWrite(); break;

				case 0x04040010: 
					if ( ( WriteData & 0x0000001 ) ) { N64Regs->SP[4] &= ~0x0001; }
					if ( ( WriteData & 0x0000002 ) ) { N64Regs->SP[4] |= 0x0001;  }
					if ( ( WriteData & 0x0000004 ) ) { N64Regs->SP[4] &= ~0x0002; }
					if ( ( WriteData & 0x0000008 ) ) { N64Regs->MI[2] &= ~0x01; CheckInterrupts(); }
					//if ( ( WriteData & 0x0000010 ) != 0) { DisplayError("SP_SET_INTR"); }
					if ( ( WriteData & 0x0000020 ) ) { N64Regs->SP[4] &= ~0x0020; }
					if ( ( WriteData & 0x0000040 ) ) { N64Regs->SP[4] |= 0x0020;  }
					if ( ( WriteData & 0x0000080 ) ) { N64Regs->SP[4] &= ~0x0040; }
					if ( ( WriteData & 0x0000100 ) ) { N64Regs->SP[4] |= 0x0040;  }
					if ( ( WriteData & 0x0000200 ) ) { N64Regs->SP[4] &= ~0x0080; }
					if ( ( WriteData & 0x0000400 ) ) { N64Regs->SP[4] |= 0x0080;  }
					if ( ( WriteData & 0x0000800 ) ) { N64Regs->SP[4] &= ~0x0100; }
					if ( ( WriteData & 0x0001000 ) ) { N64Regs->SP[4] |= 0x0100;  }
					if ( ( WriteData & 0x0002000 ) ) { N64Regs->SP[4] &= ~0x0200; }
					if ( ( WriteData & 0x0004000 ) ) { N64Regs->SP[4] |= 0x0200;  }
					if ( ( WriteData & 0x0008000 ) ) { N64Regs->SP[4] &= ~0x0400; }
					if ( ( WriteData & 0x0010000 ) ) { N64Regs->SP[4] |= 0x0400;  }
					if ( ( WriteData & 0x0020000 ) ) { N64Regs->SP[4] &= ~0x0800; }
					if ( ( WriteData & 0x0040000 ) ) { N64Regs->SP[4] |= 0x0800;  }
					if ( ( WriteData & 0x0080000 ) ) { N64Regs->SP[4] &= ~0x1000; }
					if ( ( WriteData & 0x0100000 ) ) { N64Regs->SP[4] |= 0x1000;  }
					if ( ( WriteData & 0x0200000 ) ) { N64Regs->SP[4] &= ~0x2000; }
					if ( ( WriteData & 0x0400000 ) ) { N64Regs->SP[4] |= 0x2000;  }
					if ( ( WriteData & 0x0800000 ) ) { N64Regs->SP[4] &= ~0x4000; }
					if ( ( WriteData & 0x1000000 ) ) { N64Regs->SP[4] |= 0x4000;  }

					RunRsp();
					break;

				case 0x0404001C: N64Regs->SP[7] = 0; break;
				case 0x04080000: N64Regs->SP[8] = WriteData & 0xFFC; break;
			}

			return;
		}
		break;

		case 0x04100000:
		{
			switch( uAddr ) 
			{
				case 0x04100000: N64Regs->DPC[0] = WriteData; N64Regs->DPC[2] = WriteData; break;
				case 0x04100004: N64Regs->DPC[1] = WriteData; ProcessRDPList(); break;

				case 0x0410000C:
					if ( ( WriteData & 0x0001 ) ) { N64Regs->DPC[3] &= ~0x001; }
					if ( ( WriteData & 0x0002 ) ) { N64Regs->DPC[3] |= 0x001;  }
					if ( ( WriteData & 0x0004 ) ) { N64Regs->DPC[3] &= ~0x002; }
					if ( ( WriteData & 0x0008 ) ) { N64Regs->DPC[3] |= 0x002;  }		
					if ( ( WriteData & 0x0010 ) ) { N64Regs->DPC[3] &= ~0x004; }
					if ( ( WriteData & 0x0020 ) ) { N64Regs->DPC[3] |= 0x004;  }
					break;
			}

			return;
		}
		break;

		case 0x04300000:
		{
			switch( uAddr )
			{
				case 0x04300000:
					N64Regs->MI[0] &= ~0x7F;
					N64Regs->MI[0] |= (WriteData & 0x7F);

					if ( ( WriteData & 0x0080 ) ) { N64Regs->MI[0] &= ~0x0080; }
					if ( ( WriteData & 0x0100 ) ) { N64Regs->MI[0] |= 0x0080; }
					if ( ( WriteData & 0x0200 ) ) { N64Regs->MI[0] &= ~0x0100; }
					if ( ( WriteData & 0x0400 ) ) { N64Regs->MI[0] |= 0x0100; }
					if ( ( WriteData & 0x0800 ) ) { N64Regs->MI[2] &= ~0x20; CheckInterrupts(); }
					if ( ( WriteData & 0x1000 ) ) { N64Regs->MI[0] &= ~0x0200; }
					if ( ( WriteData & 0x2000 ) ) { N64Regs->MI[0] |= 0x0200; }
					break;

				case 0x0430000C:
					if ( ( WriteData & 0x0001 ) ) { N64Regs->MI[3] &= ~0x01; }
					if ( ( WriteData & 0x0002 ) ) { N64Regs->MI[3] |= 0x01; }
					if ( ( WriteData & 0x0004 ) ) { N64Regs->MI[3] &= ~0x02; }
					if ( ( WriteData & 0x0008 ) ) { N64Regs->MI[3] |= 0x02; }
					if ( ( WriteData & 0x0010 ) ) { N64Regs->MI[3] &= ~0x04; }
					if ( ( WriteData & 0x0020 ) ) { N64Regs->MI[3] |= 0x04; }
					if ( ( WriteData & 0x0040 ) ) { N64Regs->MI[3] &= ~0x08; }
					if ( ( WriteData & 0x0080 ) ) { N64Regs->MI[3] |= 0x08; }
					if ( ( WriteData & 0x0100 ) ) { N64Regs->MI[3] &= ~0x10; }
					if ( ( WriteData & 0x0200 ) ) { N64Regs->MI[3] |= 0x10; }
					if ( ( WriteData & 0x0400 ) ) { N64Regs->MI[3] &= ~0x20; }
					if ( ( WriteData & 0x0800 ) ) { N64Regs->MI[3] |= 0x20; }
					break;
			}

			return;
		}
		break;

		case 0x04400000:
		{
			switch( uAddr )
			{
				case 0x04400000:
					if( N64Regs->VI[0] != WriteData )
					{
						N64Regs->VI[0] = WriteData;
						ViStatusChange();
					}
					break;

				case 0x04400004: 
					N64Regs->VI[1] = (WriteData & 0xFFFFFF); 
					break;

				case 0x04400008:
					if( N64Regs->VI[2] != WriteData )
					{
						N64Regs->VI[2] = WriteData;
						ViWidthChanged();
					}
					break;

				case 0x0440000C: N64Regs->VI[3] = WriteData; break;
				case 0x04400010: N64Regs->MI[2] &= ~0x08; CheckInterrupts(); break;

				case 0x04400014: N64Regs->VI[5] = WriteData; break;
				case 0x04400018: N64Regs->VI[6] = WriteData; break;
				case 0x0440001C: N64Regs->VI[7] = WriteData; break;
				case 0x04400020: N64Regs->VI[8] = WriteData; break;
				case 0x04400024: N64Regs->VI[9] = WriteData; break;
				case 0x04400028: N64Regs->VI[10] = WriteData; break;
				case 0x0440002C: N64Regs->VI[11] = WriteData; break;
				case 0x04400030: N64Regs->VI[12] = WriteData; break;
				case 0x04400034: N64Regs->VI[13] = WriteData; break;
			}

			return;
		}
		break;

		case 0x04500000:
		{
			switch( uAddr ) 
			{
				case 0x04500004: 	
					N64Regs->AI[1] = WriteData; 
					if( N64Regs->AI[1] != 0 )
					{
						N64Regs->MI[2] |= 0x04; 
						N64Regs->AudioIntrReg |= 0x04;
						//N64Regs->iCheckInterupt = 1;
						CheckInterrupts();
					}
					break;

				case 0x0450000C: 
					N64Regs->MI[2] &= ~0x04; 
					N64Regs->AudioIntrReg &= ~0x04;
					CheckInterrupts(); 
					break;

				case 0x04500000: N64Regs->AI[0] = WriteData; break;
				case 0x04500008: N64Regs->AI[2] = ( WriteData & 1 ); break;
				case 0x04500010: N64Regs->AI[4] = WriteData; break;
				case 0x04500014: N64Regs->AI[5] = WriteData; break;
			}
			
			return;
		}
		break;

		case 0x04600000:
		{
			switch( uAddr) 
			{
				case 0x04600000: N64Regs->PI[0] = WriteData; break;
				case 0x04600004: N64Regs->PI[1] = WriteData; break;
				case 0x04600008: N64Regs->PI[2] = WriteData; DmaPiRead(); break;
				case 0x0460000C: N64Regs->PI[3] = WriteData; DmaPiWrite(); break;
				case 0x04600010:
					if ((WriteData & 0x02) != 0 ) {
						N64Regs->MI[2] &= ~0x10;
						CheckInterrupts();
					}
					break;
				case 0x04600014: N64Regs->PI[5] = (WriteData & 0xFF); break; 
				case 0x04600018: N64Regs->PI[6] = (WriteData & 0xFF); break; 
				case 0x0460001C: N64Regs->PI[7] = (WriteData & 0xFF); break; 
				case 0x04600020: N64Regs->PI[8] = (WriteData & 0xFF); break;
			}

			return;
		}
		break;

		case 0x04700000:
		{
			switch( uAddr )
			{
				case 0x04700000: N64Regs->RI[0] = WriteData; break;
				case 0x04700004: N64Regs->RI[1] = WriteData; break;
				case 0x04700008: N64Regs->RI[2] = WriteData; break;
				case 0x0470000C: N64Regs->RI[3] = WriteData; break;
				case 0x04700010: N64Regs->RI[4] = WriteData; break;
				case 0x04700014: N64Regs->RI[5] = WriteData; break;
				case 0x04700018: N64Regs->RI[6] = WriteData; break;
				case 0x0470001C: N64Regs->RI[7] = WriteData; break;
			}

			return;
		}
		break;

		case 0x04800000:
		{
			switch( uAddr ) 
			{
				case 0x04800000: N64Regs->SI[0] = WriteData; break;
				case 0x04800004: N64Regs->SI[1] = WriteData; DmaSiRead(); break;
				case 0x04800010: N64Regs->SI[2] = WriteData; DmaSiWrite(); break;
				case 0x04800018: 
					N64Regs->MI[2] &= ~0x02;
					N64Regs->SI[3] &= ~0x1000;
					CheckInterrupts();
					break;
			}

			return;
		}
		break;
	}

	//* Print Error
	printf( "Error: Illegal 32bit Memory Write at 0x%08x <PC:0x%08x>\n", (unsigned int)RealAddr, (unsigned int)N64Regs->PC );
	if( WaitForInput( PSP_CTRL_CROSS ) )
		iEmulatorRunning = 0;

	return;
}

// Store Quad Word ( 64bits )
//////////////////////////////////////////////////////////////////////////
void StoreQWord( DWORD uAddr, QWORD WriteData )
{
	DWORD WriteDataDW[2];

	*(QWORD *)&WriteDataDW[0] = WriteData;

	//* Check if Virtual Addr
	if( uAddr < 0x80000000 || uAddr >= 0xC0000000 )
	{
		uAddr = TlbTranslateAddr( uAddr, 1 );

		if( uAddr == 0x0 )
			return;
	}

	//* Wrap Address
	uAddr &= 0x1FFFFFFF;

	if( uAddr <= 0x003fffff )
	{
		*(DWORD *)&g_pRDRam[uAddr] = WriteDataDW[1];
		*(DWORD *)&g_pRDRam[uAddr+4] = WriteDataDW[0];

		return;
	}

	//* Print Error
//	printf( "Error: Illegal 64bit Memory Write at 0x%08x <PC:0x%08x>\n", (unsigned int)uAddr, (unsigned int)N64Regs->PC );
//	iEmulatorRunning = 0;

	return;
}

// Tlb Translate
//////////////////////////////////////////////////////////////////////////
DWORD TlbTranslateAddr( DWORD addresse, int iWrite )
{
	if (addresse >= 0x7f000000 && addresse < 0x80000000) // golden eye hack
	{
		if( *(DWORD *)&g_pRomImage[0x10] == 0xDCBC50D1 ) // US
			return 0xb0034b30 + ( addresse & 0xFFFFFF );
		if( *(DWORD *)&g_pRomImage[0x10] == 0x0414CA61 ) // E
			return 0xb00329f0 + ( addresse & 0xFFFFFF );
		if( *(DWORD *)&g_pRomImage[0x10] == 0xA24F4CF1 ) // J
			return 0xb0034b70 + ( addresse & 0xFFFFFF );
	}

	if( !g_iUseFastTlb )
	{
		int i;

		for( i = 0; i < 32; i++ )
		{
			if( addresse >= N64Regs->Tlb[i].start_even && addresse <= N64Regs->Tlb[i].end_even )
				return ( 0x80000000 | ( N64Regs->Tlb[i].phys_even + ( addresse - N64Regs->Tlb[i].start_even ) ) );

			if( addresse >= N64Regs->Tlb[i].start_odd && addresse <= N64Regs->Tlb[i].end_odd )
				return ( 0x80000000 | ( N64Regs->Tlb[i].phys_odd + ( addresse - N64Regs->Tlb[i].start_odd ) ) );
		}
	}
	else
	{
		if( iWrite == 1 )
		{
			if( N64Regs->Tlb_Lut_W[addresse>>12] )
				return	( ( N64Regs->Tlb_Lut_W[addresse>>12] & 0xFFFFF000 ) | ( addresse & 0xFFF ) );
		}
		else
		{
			if( N64Regs->Tlb_Lut_R[addresse>>12] )
				return ( ( N64Regs->Tlb_Lut_R[addresse>>12] & 0xFFFFF000 ) | ( addresse & 0xFFF ) );
		}
	}

	PerformTLBException( iNextJump, addresse, iWrite );

	return 0x00000000;
}
