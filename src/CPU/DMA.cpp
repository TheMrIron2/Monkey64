//////////////////////////////////////////////////////////////////////////
// Dma / Direct Memory Access
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Dma Transfers

#include "Dma.h"
#include "Registers.h"
#include "Memory.h"
#include "Exceptions.h"
#include "Timers.h"
#include "Pif.h"

extern int WaitForInput ( u16 uInputButton );

// Dma Pi Read
//////////////////////////////////////////////////////////////////////////
void DmaPiRead ( void ) 
{
//	printf( "Dma Pi Read <0x%08x> -> <0x%08x> : %d bytes\n", N64Regs->PI[0], N64Regs->PI[1], N64Regs->PI[2] );
//	WaitForInput( PSP_CTRL_CROSS );

	//* Remove for now since we dont do anything
/*
	if( ( N64Regs->PI[0] + N64Regs->PI[2] + 1 ) > 0x400000 ) {
		//* Error: Larger then mem
		N64Regs->PI[4] &= ~0x01;
		N64Regs->MI[2] |= 0x10;
		CheckInterrupts();
		return;
	}

	if( N64Regs->PI[1] >= 0x08000000 && N64Regs->PI[1] <= 0x08010000 ) 
	{
		//* Save Write
		N64Regs->PI[4] &= ~0x01;
		N64Regs->MI[2] |= 0x10;
		CheckInterrupts();
		return;
	}
*/
	//* Error, Unknown place
	N64Regs->PI[4] &= ~0x01;
	N64Regs->MI[2] |= 0x10;
	CheckInterrupts();
}

// Dma Pi Write
//////////////////////////////////////////////////////////////////////////
void DmaPiWrite ( void ) 
{
	DWORD i;

	N64Regs->PI[4] |= 0x01;

//	printf( "Dma Pi Write <0x%08x> -> <0x%08x> : %d bytes\n", N64Regs->PI[0], N64Regs->PI[1], N64Regs->PI[3] );
//	printf( "PC: 0x%08x\n", N64Regs->PC );
//	WaitForInput( PSP_CTRL_CROSS );

	if( ( N64Regs->PI[0] + N64Regs->PI[3] + 1 ) > 0x400000 ) {
		//* Error: Larger then mem
		printf( "Dma Pi Write Out of Bounds RDRAM\n" );
		WaitForInput( PSP_CTRL_CROSS );

		N64Regs->PI[4] &= ~0x01;
		N64Regs->MI[2] |= 0x10;
		CheckInterrupts();
		return;
	}
/*
	if( N64Regs->PI[1] >= 0x08000000 && N64Regs->PI[1] <= 0x08010000 ) 
	{
		//* Save Write
		N64Regs->PI[4] &= ~0x01;
		N64Regs->MI[2] |= 0x10;
		CheckInterrupts();
		return;
	}
*/
	if( N64Regs->PI[1] >= 0x10000000 && N64Regs->PI[1] <= 0x1FBFFFFF ) 
	{
		N64Regs->PI[1] -= 0x10000000;

		if( ( N64Regs->PI[1] + N64Regs->PI[3] + 1 ) < g_iRomLen ) 
		{
			for( i = 0; i < N64Regs->PI[3] + 1; i++ )
				g_pRDRam[ (N64Regs->PI[0] + i) ^ 3 ] = g_pRomImage[ ( N64Regs->PI[1] + i ) ^ 3 ];
		} 
		else 
		{
			DWORD Len;
			Len = g_iRomLen - N64Regs->PI[1];

			for( i = 0; i < Len; i++ )
				g_pRDRam[ (N64Regs->PI[0] + i) ^ 3 ] = g_pRomImage[ ( N64Regs->PI[1] + i ) ^ 3 ];

			for( i = Len; i < N64Regs->PI[3] + 1 - Len; i++ )
				g_pRDRam[ (N64Regs->PI[0] + i) ^ 3 ] = 0;
		}

		N64Regs->PI[1] += 0x10000000;

		if( !N64Regs->iFirstDmaWrite )
		{
			FirstDMA();
			N64Regs->iFirstDmaWrite = 1;
		}

		N64Regs->PI[4] &= ~0x01;
		N64Regs->MI[2] |= 0x10;
		CheckInterrupts();
		CheckTimers();
		return;
	}

	//* Error, Unknown Address
	N64Regs->PI[4] &= ~0x01;
	N64Regs->MI[2] |= 0x10;
	CheckInterrupts();
}

// Dma Si Read
//////////////////////////////////////////////////////////////////////////
void DmaSiRead ( void ) 
{
	int i;

	N64Regs->SI[0] &= 0xFFFFFFF8;

	//* Check Address Bounds
	if( (int)N64Regs->SI[0] > (int)0x400000 ) 
	{
//		printf( "Error Si Read: SI_DRAM_ADDR_REG is outside RDRam Space!!\n" );
		return;
	}

	//* Read Pif
	PifRamRead();

	if( (int)N64Regs->SI[0] < 0 ) 
	{
		int count, RdramPos;

		RdramPos = (int)N64Regs->SI[0];

		for( count = 0; count < 0x40; count++, RdramPos++ ) 
		{
			if (RdramPos < 0)
				continue;

//			g_pRDRam[RdramPos] = N64Regs->PIF_Ram[count];
		}
	} 
	else 
	{
		//* Copy Data (byteswapped)
		for( i = 0; i < 64; i += 4 )
		{
			g_pRDRam[N64Regs->SI[0] + i + 0] = N64Regs->PIF_Ram[i + 3];
			g_pRDRam[N64Regs->SI[0] + i + 1] = N64Regs->PIF_Ram[i + 2];
			g_pRDRam[N64Regs->SI[0] + i + 2] = N64Regs->PIF_Ram[i + 1];
			g_pRDRam[N64Regs->SI[0] + i + 3] = N64Regs->PIF_Ram[i + 0];
		}
	}

	//* Setup Interupts
	N64Regs->MI[2] |= 0x02;
	N64Regs->SI[3] |= 0x1000;
	CheckInterrupts();
}

// Dma Si Write
//////////////////////////////////////////////////////////////////////////
void DmaSiWrite ( void )
{
	int i;

	N64Regs->SI[0] &= 0xFFFFFFF8;

	//* Check Address Bounds
	if( (int)N64Regs->SI[0] > (int)0x400000 ) 
	{
//		printf( "Error Si Write: SI_DRAM_ADDR_REG is outside RDRam Space!! (0x%08x)\n", (unsigned int)N64Regs->SI[0] );
		return;
	}
	
	if( (int)N64Regs->SI[0] < 0 )
	{
		int count, RdramPos;

		RdramPos = (int)N64Regs->SI[0];

		for( count = 0; count < 0x40; count++, RdramPos++ ) 
		{
			if( RdramPos < 0 ) 
			{
				N64Regs->PIF_Ram[count] = 0; 
				continue;
			}

//			N64Regs->PIF_Ram[count] = g_pRDRam[RdramPos];
		}
	} 
	else 
	{
		//* Copy Data (byteswapped)
		for( i = 0; i < 64; i += 4 )
		{
			N64Regs->PIF_Ram[i + 0] = g_pRDRam[N64Regs->SI[0] + i + 3];
			N64Regs->PIF_Ram[i + 1] = g_pRDRam[N64Regs->SI[0] + i + 2];
			N64Regs->PIF_Ram[i + 2] = g_pRDRam[N64Regs->SI[0] + i + 1];
			N64Regs->PIF_Ram[i + 3] = g_pRDRam[N64Regs->SI[0] + i + 0];
		}
	}

	//* Write to Pif Ram
	PifRamWrite();

	//* Setup Interupts
	N64Regs->MI[2] |= 0x02;
	N64Regs->SI[3] |= 0x1000;
	CheckInterrupts();
}

// Dma Sp Read
//////////////////////////////////////////////////////////////////////////
void DmaSpRead ( void )
{
//	printf( "Dma Sp Read DM<0x%08x> RDM<0x%08x> : %d bytes\n", N64Regs->SP[0], N64Regs->SP[1], N64Regs->SP[2] );
//	WaitForInput( PSP_CTRL_CROSS );

	if( N64Regs->SP[1] > 0x400000 )
	{
		N64Regs->SP[6]  = 0;
		N64Regs->SP[4] &= ~0x004;
		return;
	}
	
	if( N64Regs->SP[2] + 1 + ( N64Regs->SP[0] & 0xFFF ) > 0x1000 )
		return;		

	memcpy( g_pDMem + ( N64Regs->SP[0] & 0x1FFF ), g_pRDRam + N64Regs->SP[1], N64Regs->SP[2] + 1 );

	N64Regs->SP[6]  = 0;
	N64Regs->SP[4] &= ~0x004;
}

// Dma Sp Write
//////////////////////////////////////////////////////////////////////////
void DmaSpWrite ( void ) 
{
	int i;

//	printf( "Dma Sp Write DM<0x%08x> RDM<0x%08x> : %d bytes\n", N64Regs->SP[0], N64Regs->SP[1], N64Regs->SP[3] );
//	WaitForInput( PSP_CTRL_CROSS );

	if( N64Regs->SP[1] > 0x400000 ) 
		return;
	
	if( N64Regs->SP[3] + 1 + ( N64Regs->SP[0] & 0xFFF ) > 0x1000 ) 
		return;

	memcpy( g_pRDRam + N64Regs->SP[1], g_pDMem + ( N64Regs->SP[0] & 0x1FFF ), N64Regs->SP[3] + 1 );
		
	N64Regs->SP[6]  = 0;
	N64Regs->SP[4] &= ~0x004;
}
