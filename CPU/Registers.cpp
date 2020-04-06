//////////////////////////////////////////////////////////////////////////
// Registers
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the n64 regisers

#include "Registers.h"
#include "Memory.h"
#include "Timers.h"
#include "Textures.h"
#include "Instructions/Instructions.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
N64_REGISTERS* N64Regs = NULL;

extern int WaitForInput( u16 uInputButton );

// Get Cic Chip ID (taken from Pj64)
//////////////////////////////////////////////////////////////////////////
int GetCicChipID( void ) 
{
	unsigned long long uCRC = 0;
	int iCount;

	for( iCount = 0x40; iCount < 0x1000; iCount += 4 )
		uCRC += ( *(DWORD *)( g_pRomImage+iCount ) );

	//printf( "Crc: %08x", ( uCRC & 0xFFFFFFFF00000000ULL ) >> 32 );
	//printf( "-%08x\n", uCRC & 0x00000000FFFFFFFFULL );

	switch( uCRC ) 
	{
		case 0x000000D0027FDF31ULL: return 1;
		case 0x000000CFFB631223ULL: return 1;
		case 0x000000D057C85244ULL: return 2;
		case 0x000000D6497E414BULL: return 3;
		case 0x0000011A49F60E96ULL: return 5;
		case 0x000000D6D5BE5580ULL: return 6;
		default:					return -1;
	}
}

// Setup Fpu Locations (Taken from PJ64)
//////////////////////////////////////////////////////////////////////////
void SetFpuLocations( void )
{
	int iCount;

	if( ( N64Regs->COP0[12] & 0x04000000 ) == 0 ) 
	{
		for( iCount = 0; iCount < 32; iCount++ ) 
		{
			N64Regs->COP1Float[iCount] = (void *)( &N64Regs->COP1[iCount >> 1].W[iCount & 1] );
			N64Regs->COP1Double[iCount] = (void *)( &N64Regs->COP1[iCount >> 1].DW );
		}
	} 
	else 
	{
		for( iCount = 0; iCount < 32; iCount++ )
		{
			N64Regs->COP1Float[iCount] = (void *)( &N64Regs->COP1[iCount].W[0] );
			N64Regs->COP1Double[iCount] = (void *)( &N64Regs->COP1[iCount].DW );
		}
	}
}

// Init Registers
//////////////////////////////////////////////////////////////////////////
int InitRegisters( void )
{
	int i;

	//* Init Memory System
	if( !InitMemorySystem() )
		return 0;

	//* Clear Registers
	memset( N64Regs, 0, sizeof( N64_REGISTERS ) );

	//* Set Pif Ram Pointer
	N64Regs->PIF_Ram = N64Regs->PIF_Mem + 0x7C0;

	//* Set TMem Location
	//N64Regs->TMemPal = (DWORD *)g_pTMemPal;
	//N64Regs->TMem = (QWORD *)g_pTMem;
	N64Regs->TMemPal = (DWORD *)( (char *)sceGeEdramGetAddr() + EDRAM_START  );
	N64Regs->TMem = (QWORD *)( (char *)sceGeEdramGetAddr() + EDRAM_START + (256*4) );
	N64Regs->TCache = (DWORD *)( (char *)sceGeEdramGetAddr() + EDRAM_START + (256*4) + (512*8) );

	//* Set Tlb Pointers
	if( g_iUseFastTlb && g_pTlbR && g_pTlbW )
	{
		N64Regs->Tlb_Lut_R = (DWORD *)g_pTlbR;
		N64Regs->Tlb_Lut_W = (DWORD *)g_pTlbW;
	}
	else
	{
		g_iUseFastTlb = 0;
	}

	//for( i = 0; i < 256; i++ )
	//	N64Regs->TMemPal[i] = 0;

	//for( i = 0; i < 512; i++ )
	//	N64Regs->TMem[i] = 0;

	//* Setup Default Register Values
	//* Default Values taken from PJ64
	N64Regs->iCicChipID = GetCicChipID();
	N64Regs->iCountryID = (int)*(g_pRomImage + 0x3D);

	CopyToN64Memory( g_pRomImage + 0x40, 0x04000040, 0xFBC );

	if( N64Regs->iCicChipID < 0 )
		N64Regs->iCicChipID = 2;

	N64Regs->PC			= 0xA4000040;

	//* Default Start
	N64Regs->CPU[0].UDW  = 0x0000000000000000ULL;
	N64Regs->CPU[6].UDW  = 0xFFFFFFFFA4001F0CULL;
	N64Regs->CPU[7].UDW  = 0xFFFFFFFFA4001F08ULL;
	N64Regs->CPU[8].UDW  = 0x00000000000000C0ULL;
	N64Regs->CPU[9].UDW  = 0x0000000000000000ULL;
	N64Regs->CPU[10].UDW = 0x0000000000000040ULL;
	N64Regs->CPU[11].UDW = 0xFFFFFFFFA4000040ULL;
	N64Regs->CPU[16].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[17].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[18].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[19].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[21].UDW = 0x0000000000000000ULL; 
	N64Regs->CPU[26].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[27].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[28].UDW = 0x0000000000000000ULL;
	N64Regs->CPU[29].UDW = 0xFFFFFFFFA4001FF0ULL;
	N64Regs->CPU[30].UDW = 0x0000000000000000ULL;
	
	//* Region Specific
	switch( N64Regs->iCountryID )
	{
		case 0x44: //Germany
		case 0x46: //french
		case 0x49: //Italian
		case 0x50: //Europe
		case 0x53: //Spanish
		case 0x55: //Australia
		case 0x58: // ????
		case 0x59: // X (PAL)
			switch( N64Regs->iCicChipID ) 
			{
				case 2:
					N64Regs->CPU[5].UDW  = 0xFFFFFFFFC0F1D859ULL;
					N64Regs->CPU[14].UDW = 0x000000002DE108EAULL;
					N64Regs->CPU[24].UDW = 0x0000000000000000ULL;
					break;
				case 3:
					N64Regs->CPU[5].UDW  = 0xFFFFFFFFD4646273ULL;
					N64Regs->CPU[14].UDW = 0x000000001AF99984ULL;
					N64Regs->CPU[24].UDW = 0x0000000000000000ULL;
					break;
				case 5:
					*(DWORD *)&g_pIMem[0x04] = 0xBDA807FC;
					N64Regs->CPU[5].UDW  = 0xFFFFFFFFDECAAAD1ULL;
					N64Regs->CPU[14].UDW = 0x000000000CF85C13ULL;
					N64Regs->CPU[24].UDW = 0x0000000000000002ULL;
					break;
				case 6:
					N64Regs->CPU[5].UDW  = 0xFFFFFFFFB04DC903ULL;
					N64Regs->CPU[14].UDW = 0x000000001AF99984ULL;
					N64Regs->CPU[24].UDW = 0x0000000000000002ULL;
					break;
			}

			N64Regs->CPU[20].UDW = 0x0000000000000000ULL;
			N64Regs->CPU[23].UDW = 0x0000000000000006ULL;
			N64Regs->CPU[31].UDW = 0xFFFFFFFFA4001554ULL;
			break;

		case 0x37: // 7 (Beta)
		case 0x41: // ????
		case 0x45: //USA
		case 0x4A: //Japan
		default:
			switch ( N64Regs->iCicChipID ) 
			{
				case 2:
					N64Regs->CPU[5].UDW  = 0xFFFFFFFFC95973D5ULL;
					N64Regs->CPU[14].UDW = 0x000000002449A366ULL;
					break;
				case 3:
					N64Regs->CPU[5].UDW  = 0xFFFFFFFF95315A28ULL;
					N64Regs->CPU[14].UDW = 0x000000005BACA1DFULL;
					break;
				case 5:
					*(DWORD *)&g_pIMem[0x04] = 0x8DA807FC;
					N64Regs->CPU[5].UDW  = 0x000000005493FB9AULL;
					N64Regs->CPU[14].UDW = 0xFFFFFFFFC2C20384ULL;
				case 6:
					N64Regs->CPU[5].UDW  = 0xFFFFFFFFE067221FULL;
					N64Regs->CPU[14].UDW = 0x000000005CD2B70FULL;
					break;
			}

			N64Regs->CPU[20].UDW = 0x0000000000000001ULL;
			N64Regs->CPU[23].UDW = 0x0000000000000000ULL;
			N64Regs->CPU[24].UDW = 0x0000000000000003ULL;
			N64Regs->CPU[31].UDW = 0xFFFFFFFFA4001550ULL;
	}

	//* CiC Specific
	switch( N64Regs->iCicChipID ) 
	{
		case 1: 
			N64Regs->CPU[22].UDW = 0x000000000000003FULL; 
			break;
		case 2: 
			N64Regs->CPU[1].UDW  = 0x0000000000000001ULL;
			N64Regs->CPU[2].UDW  = 0x000000000EBDA536ULL;
			N64Regs->CPU[3].UDW  = 0x000000000EBDA536ULL;
			N64Regs->CPU[4].UDW  = 0x000000000000A536ULL;
			N64Regs->CPU[12].UDW = 0xFFFFFFFFED10D0B3ULL;
			N64Regs->CPU[13].UDW = 0x000000001402A4CCULL;
			N64Regs->CPU[15].UDW = 0x000000003103E121ULL;
			N64Regs->CPU[22].UDW = 0x000000000000003FULL; 
			N64Regs->CPU[25].UDW = 0xFFFFFFFF9DEBB54FULL;
			break;
		case 3: 
			N64Regs->CPU[1].UDW  = 0x0000000000000001ULL;
			N64Regs->CPU[2].UDW  = 0x0000000049A5EE96ULL;
			N64Regs->CPU[3].UDW  = 0x0000000049A5EE96ULL;
			N64Regs->CPU[4].UDW  = 0x000000000000EE96ULL;
			N64Regs->CPU[12].UDW = 0xFFFFFFFFCE9DFBF7ULL;
			N64Regs->CPU[13].UDW = 0xFFFFFFFFCE9DFBF7ULL;
			N64Regs->CPU[15].UDW = 0x0000000018B63D28ULL;
			N64Regs->CPU[22].UDW = 0x0000000000000078ULL; 
			N64Regs->CPU[25].UDW = 0xFFFFFFFF825B21C9ULL;
			break;
		case 5: 
			*(DWORD *)&g_pIMem[0x00] = 0x3C0DBFC0;
			*(DWORD *)&g_pIMem[0x08] = 0x25AD07C0;
			*(DWORD *)&g_pIMem[0x0C] = 0x31080080;
			*(DWORD *)&g_pIMem[0x10] = 0x5500FFFC;
			*(DWORD *)&g_pIMem[0x14] = 0x3C0DBFC0;
			*(DWORD *)&g_pIMem[0x18] = 0x8DA80024;
			*(DWORD *)&g_pIMem[0x1C] = 0x3C0BB000;
			N64Regs->CPU[1].UDW  = 0x0000000000000000ULL;
			N64Regs->CPU[2].UDW  = 0xFFFFFFFFF58B0FBFULL;
			N64Regs->CPU[3].UDW  = 0xFFFFFFFFF58B0FBFULL;
			N64Regs->CPU[4].UDW  = 0x0000000000000FBFULL;
			N64Regs->CPU[12].UDW = 0xFFFFFFFF9651F81EULL;
			N64Regs->CPU[13].UDW = 0x000000002D42AAC5ULL;
			N64Regs->CPU[15].UDW = 0x0000000056584D60ULL;
			N64Regs->CPU[22].UDW = 0x0000000000000091ULL; 
			N64Regs->CPU[25].UDW = 0xFFFFFFFFCDCE565FULL;
			break;
		case 6: 
			N64Regs->CPU[1].UDW  = 0x0000000000000000ULL;
			N64Regs->CPU[2].UDW  = 0xFFFFFFFFA95930A4ULL;
			N64Regs->CPU[3].UDW  = 0xFFFFFFFFA95930A4ULL;
			N64Regs->CPU[4].UDW  = 0x00000000000030A4ULL;
			N64Regs->CPU[12].UDW = 0xFFFFFFFFBCB59510ULL;
			N64Regs->CPU[13].UDW = 0xFFFFFFFFBCB59510ULL;
			N64Regs->CPU[15].UDW = 0x000000007A3C07F4ULL;
			N64Regs->CPU[22].UDW = 0x0000000000000085ULL; 
			N64Regs->CPU[25].UDW = 0x00000000465E3F72ULL;
			break;
	}

	N64Regs->COP0[1]	= 0x1F;
	N64Regs->COP0[4]	= 0x007FFFF0;
	N64Regs->COP0[8]	= 0xFFFFFFFF;
	N64Regs->COP0[9]	= 0x5000;
	N64Regs->COP0[12]	= 0x34000000;
	N64Regs->COP0[13]	= 0x0000005C;
	N64Regs->COP0[14]	= 0xFFFFFFFF;
	N64Regs->COP0[16]	= 0x0006E463;
	N64Regs->COP0[30]	= 0xFFFFFFFF;

	N64Regs->HI.DW		= 0x0;
	N64Regs->LO.DW		= 0x0;

	N64Regs->MI[1]		= 0x02020102;
	
	N64Regs->SP[4]		= 0x00000001;

	N64Regs->COP1CR[0]	= 0x00000511;

	//* Setup Cop1 Float / Double Pointers
	SetFpuLocations();

	//* Setup RSP Registers
	//N64Regs->RSPEleSpec[ 0].DW = 0;
	//N64Regs->RSPEleSpec[ 1].DW = 0;
	//N64Regs->RSPEleSpec[ 2].DW = 0;
	//N64Regs->RSPEleSpec[ 3].DW = 0;
	//N64Regs->RSPEleSpec[ 4].DW = 0;
	//N64Regs->RSPEleSpec[ 5].DW = 0;
	//N64Regs->RSPEleSpec[ 6].DW = 0;
	//N64Regs->RSPEleSpec[ 7].DW = 0;
	//N64Regs->RSPEleSpec[ 8].DW = 0;
	//N64Regs->RSPEleSpec[ 9].DW = 0;
	//N64Regs->RSPEleSpec[10].DW = 0;
	//N64Regs->RSPEleSpec[11].DW = 0;
	//N64Regs->RSPEleSpec[12].DW = 0;
	//N64Regs->RSPEleSpec[13].DW = 0;
	//N64Regs->RSPEleSpec[14].DW = 0;
	//N64Regs->RSPEleSpec[15].DW = 0;
	//N64Regs->RSPEleSpec[16].DW = 0x0001020304050607ULL; /* None */
	//N64Regs->RSPEleSpec[17].DW = 0x0001020304050607ULL; /* None */
	//N64Regs->RSPEleSpec[18].DW = 0x0000020204040606ULL; /* 0q */
	//N64Regs->RSPEleSpec[19].DW = 0x0101030305050707ULL; /* 1q */
	//N64Regs->RSPEleSpec[20].DW = 0x0000000004040404ULL; /* 0h */
	//N64Regs->RSPEleSpec[21].DW = 0x0101010105050505ULL; /* 1h */
	//N64Regs->RSPEleSpec[22].DW = 0x0202020206060606ULL; /* 2h */
	//N64Regs->RSPEleSpec[23].DW = 0x0303030307070707ULL; /* 3h */
	//N64Regs->RSPEleSpec[24].DW = 0x0000000000000000ULL; /* 0 */
	//N64Regs->RSPEleSpec[25].DW = 0x0101010101010101ULL; /* 1 */
	//N64Regs->RSPEleSpec[26].DW = 0x0202020202020202ULL; /* 2 */
	//N64Regs->RSPEleSpec[27].DW = 0x0303030303030303ULL; /* 3 */
	//N64Regs->RSPEleSpec[28].DW = 0x0404040404040404ULL; /* 4 */
	//N64Regs->RSPEleSpec[29].DW = 0x0505050505050505ULL; /* 5 */
	//N64Regs->RSPEleSpec[30].DW = 0x0606060606060606ULL; /* 6 */
	//N64Regs->RSPEleSpec[31].DW = 0x0707070707070707ULL; /* 7 */

	//N64Regs->RSPIndx[ 0].DW = 0;
	//N64Regs->RSPIndx[ 1].DW = 0;
	//N64Regs->RSPIndx[ 2].DW = 0;
	//N64Regs->RSPIndx[ 3].DW = 0;
	//N64Regs->RSPIndx[ 4].DW = 0;
	//N64Regs->RSPIndx[ 5].DW = 0;
	//N64Regs->RSPIndx[ 6].DW = 0;
	//N64Regs->RSPIndx[ 7].DW = 0;
	//N64Regs->RSPIndx[ 8].DW = 0;
	//N64Regs->RSPIndx[ 9].DW = 0;
	//N64Regs->RSPIndx[10].DW = 0;
	//N64Regs->RSPIndx[11].DW = 0;
	//N64Regs->RSPIndx[12].DW = 0;
	//N64Regs->RSPIndx[13].DW = 0;
	//N64Regs->RSPIndx[14].DW = 0;
	//N64Regs->RSPIndx[15].DW = 0;
	//N64Regs->RSPIndx[16].DW = 0x0001020304050607ULL; /* None */
	//N64Regs->RSPIndx[17].DW = 0x0001020304050607ULL; /* None */
	//N64Regs->RSPIndx[18].DW = 0x0103050700020406ULL; /* 0q */
	//N64Regs->RSPIndx[19].DW = 0x0002040601030507ULL; /* 1q */
	//N64Regs->RSPIndx[20].DW = 0x0102030506070004ULL; /* 0h */
	//N64Regs->RSPIndx[21].DW = 0x0002030406070105ULL; /* 1h */
	//N64Regs->RSPIndx[22].DW = 0x0001030405070206ULL; /* 2h */
	//N64Regs->RSPIndx[23].DW = 0x0001020405060307ULL; /* 3h */
	//N64Regs->RSPIndx[24].DW = 0x0102030405060700ULL; /* 0 */
	//N64Regs->RSPIndx[25].DW = 0x0002030405060701ULL; /* 1 */
	//N64Regs->RSPIndx[26].DW = 0x0001030405060702ULL; /* 2 */
	//N64Regs->RSPIndx[27].DW = 0x0001020405060703ULL; /* 3 */
	//N64Regs->RSPIndx[28].DW = 0x0001020305060704ULL; /* 4 */
	//N64Regs->RSPIndx[29].DW = 0x0001020304060705ULL; /* 5 */
	//N64Regs->RSPIndx[30].DW = 0x0001020304050706ULL; /* 6 */
	//N64Regs->RSPIndx[31].DW = 0x0001020304050607ULL; /* 7 */

	//for( i = 16; i < 32; i++ ) 
	//{
	//	int count;

	//	for( count = 0; count < 8; count++ ) 
	//	{
	//		N64Regs->RSPIndx[i].B[count] = 7 - N64Regs->RSPIndx[i].B[count];
	//		N64Regs->RSPEleSpec[i].B[count] = 7 - N64Regs->RSPEleSpec[i].B[count];
	//	}

	//	for( count = 0; count < 4; count++ )
	//	{
	//		BYTE Temp;

	//		Temp = N64Regs->RSPIndx[i].B[count];
	//		N64Regs->RSPIndx[i].B[count] = N64Regs->RSPIndx[i].B[7 - count]; 
	//		N64Regs->RSPIndx[i].B[7 - count] = Temp;
	//	}
	//}

	//* Init Rsp
	InitRsp();

	//* Init Timers
	InitTimer();

	return 1;
}
