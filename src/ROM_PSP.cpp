//////////////////////////////////////////////////////////////////////////
// Rom Loading
//////////////////////////////////////////////////////////////////////////

#include "CPU/Memory.h"
#include "CPU/Registers.h"
#include "CPU/Convert.h"
#include "ROM_PSP.h"

// Check Rom byte format
//////////////////////////////////////////////////////////////////////////
int CheckForSwap( WORD var )
{
	if( var == 0x8037 )
		return 1;

	return 0;
}

// Swap Memory by bytes
//////////////////////////////////////////////////////////////////////////
void SwapTwoBytes(WORD *var)
{
    WORD temp;
    temp = (*var >> 8);
    temp |= (*var << 8);
    *var = temp;
}

// Swap Memory by word
//////////////////////////////////////////////////////////////////////////
void SwapTwoWords(DWORD *var)
{
    DWORD temp;
    temp = (*var >> 16);
    temp |= (*var << 16);
    *var = temp;
}

// Load Rom
//////////////////////////////////////////////////////////////////////////
int LoadRom( char* szFile )
{
	unsigned int filesize;
	unsigned int i;
	char* g_n64rom = NULL;
	int fileID;

	//* Read Rom
	fileID = sceIoOpen( szFile, PSP_O_RDONLY, 0777 );

	if( !( fileID > 0 ) )
		return 0;

	// Read Rom Header
	//////////////////////////////////////////////////////////////////////////

	//* Leave for now incase I wish to keep info from the header like rom name

	// Read Rom into Memory
	//////////////////////////////////////////////////////////////////////////
	
	//* Make sure Space is Free
	FreeAllocatedMemory( 1 );

	//* Get Rom Size
	filesize = sceIoLseek( fileID, 0, SEEK_END );

	//* Allocate Space
	g_n64rom = AllocateRomMemory( filesize );

	if( !g_n64rom )
		return 0;

	//* Reset file position
	sceIoLseek( fileID, 0, SEEK_SET );

	//* Read Rom
	sceIoRead( fileID, &g_n64rom[0], filesize );

	//* Close File IO
	sceIoClose( fileID );

	//* Byteswap Rom
	if( CheckForSwap( ( (WORD *)g_n64rom )[0] ) )
	{
		//* Header[2b] == 3780 - V64 (byteswap)
		//* Header[2b] == 8037 - Z64 (leave)

		for( i = 0; i < filesize; i += 2 )
			SwapTwoBytes( (WORD *)&g_n64rom[i] );
	}

	//* Swap Data from 1 2 3 4 -> 4 3 2 1
	for( i = 0; i < filesize; i += 4 )
		*(DWORD *)&g_n64rom[i] = swapdword( *(DWORD *)&g_n64rom[i] );

	//* Rom Loaded
	return 1;
}
