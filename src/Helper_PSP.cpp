//////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////
// Helper Functions

#include "Helper_Psp.h"

// Wait for Input
//////////////////////////////////////////////////////////////////////////
int WaitForInput( u16 uInputButton )
{
	SceCtrlData PadInput;
	unsigned int iButtonPrev = 0;
	unsigned int iButtonTrigger = 0;

	//* Turn all buttons on
	PadInput.Buttons = 0xffffff;

	while( 1 )
	{
		sceDisplayWaitVblankStart();

		iButtonPrev = PadInput.Buttons;
		sceCtrlReadBufferPositive( &PadInput, 1 );
		iButtonTrigger = PadInput.Buttons & (~iButtonPrev);

		if( ( iButtonTrigger & uInputButton ) )
			break;

		//* Special Case
		if( ( iButtonTrigger & PSP_CTRL_SELECT ) )
			return 1;
	}

	return 0;
}

// Pause if there is input
//////////////////////////////////////////////////////////////////////////
void PauseOnInput( u16 uInputButton )
{
	static SceCtrlData PadInput;
	static unsigned int iButtonPrev = 0;
	static unsigned int iButtonTrigger = 0;

	//* Read Input
	sceCtrlReadBufferPositive( &PadInput, 1 );

	//* Pause if Button Pressed
	if( PadInput.Buttons & uInputButton )
	{
		while( 1 )
		{
			sceDisplayWaitVblankStart();

			iButtonPrev = PadInput.Buttons;
			sceCtrlReadBufferPositive( &PadInput, 1 );
			iButtonTrigger = PadInput.Buttons & (~iButtonPrev);

			if( ( iButtonTrigger & uInputButton ) )
				break;
		}
	}
}

// Find Files
//////////////////////////////////////////////////////////////////////////
void FindFiles( SceIoDirent* dlist, char* szSubDir, int* iFoundItems, int iSearchType )
{
	int iCount = 0;
	int ret = 1;
	int fileID;
	int iStrLen;
	int iCountLimit = *iFoundItems;

	//* Open Directory Path
	fileID = sceIoDopen( szSubDir );

	if( fileID <= 0 )
		return;

	//* Read Files
	while( 1 )
	{
		//* Read Entry
		ret = sceIoDread( fileID, dlist + iCount );

		if( ret <= 0 )
			break;

		//* Ignore . & ..
		if( dlist[iCount].d_name[0] == '.' )
			continue;

		//* Ignore Folders
		if( !( dlist[iCount].d_stat.st_attr & iSearchType ) ) //FIO_SO_IFDIR
			continue;

		//* Ignore EBOOT
		if( strcasecmp( "eboot.pbp", dlist[iCount].d_name) == 0 )
			continue;

		//* Ignore eeprom files
		iStrLen = strlen( dlist[iCount].d_name );

		if( strcasecmp( "epr", dlist[iCount].d_name + iStrLen - 3 ) == 0 )
			continue;

		if( strcasecmp( "zip", dlist[iCount].d_name + iStrLen - 3 ) == 0 )
			continue;

		iCount++;

		if( iCount >= iCountLimit )
			break;
	}

	//* Close Handler
	sceIoDclose( fileID );

	//* StoreCount
	*iFoundItems = iCount;
}

// Dump to File
//////////////////////////////////////////////////////////////////////////
void DumpToFile( void* pData, int iSize, char* szFile )
{
	int iFileID;

	//* Open File
	iFileID = sceIoOpen( szFile, PSP_O_WRONLY|PSP_O_CREAT, 0777 );

	//* Validate
	if( iFileID < 0 )
		return;

	//* Write Data
	sceIoWrite( iFileID, pData, iSize );

	//* Close File
	sceIoClose( iFileID );
}
