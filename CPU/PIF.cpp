//////////////////////////////////////////////////////////////////////////
// Pif / Input
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Pif

#include "Registers.h"
#include "Exceptions.h"
#include "Pif.h"

// External varibles
//////////////////////////////////////////////////////////////////////////
extern char szRomFile[256];

inline void memcpy32p( void* out, void* in )
{
	((char *)out)[0] = ((char*)in)[0];
	((char *)out)[1] = ((char*)in)[1];
	((char *)out)[2] = ((char*)in)[2];
	((char *)out)[3] = ((char*)in)[3];
}

// Process Psp Input
//////////////////////////////////////////////////////////////////////////
void ProcessInput( N64_BUTTONS* pInput )
{
	SceCtrlData PadInput;

	//* Read PSP Input
	sceCtrlPeekBufferPositive( &PadInput, 1 );

	//* Clear
	pInput->Value = 0;

	//* Translate Input
	pInput->L_DPAD |= PadInput.Buttons & PSP_CTRL_LEFT	? 1 : 0;
	pInput->R_DPAD |= PadInput.Buttons & PSP_CTRL_RIGHT	? 1 : 0;
	pInput->U_DPAD |= PadInput.Buttons & PSP_CTRL_UP	? 1 : 0;
	pInput->D_DPAD |= PadInput.Buttons & PSP_CTRL_DOWN	? 1 : 0;

	if( PadInput.Buttons & PSP_CTRL_LTRIGGER )
	{
		pInput->L_CBUTTON |= PadInput.Buttons & PSP_CTRL_SQUARE		? 1 : 0;
		pInput->R_CBUTTON |= PadInput.Buttons & PSP_CTRL_CIRCLE		? 1 : 0;
		pInput->U_CBUTTON |= PadInput.Buttons & PSP_CTRL_TRIANGLE	? 1 : 0;
		pInput->D_CBUTTON |= PadInput.Buttons & PSP_CTRL_CROSS		? 1 : 0;
	}
	else
	{
		pInput->B_BUTTON |= PadInput.Buttons & PSP_CTRL_CROSS	? 1 : 0;
		pInput->A_BUTTON |= PadInput.Buttons & PSP_CTRL_CIRCLE	? 1 : 0;
		pInput->L_TRIG |= PadInput.Buttons & PSP_CTRL_SQUARE	? 1 : 0;
		pInput->R_TRIG |= PadInput.Buttons & PSP_CTRL_TRIANGLE	? 1 : 0;
	}

	pInput->START_BUTTON |= PadInput.Buttons & PSP_CTRL_START	? 1 : 0;
	pInput->Z_TRIG |= PadInput.Buttons & PSP_CTRL_RTRIGGER		? 1 : 0;

	pInput->X_AXIS = PadInput.Lx - 128;
	pInput->Y_AXIS = (255 - PadInput.Ly) - 128;

	if( ( pInput->X_AXIS < 64 ) && ( pInput->X_AXIS > -64 ) )
		pInput->X_AXIS = 0;

	if( ( pInput->Y_AXIS < 64 ) && ( pInput->Y_AXIS > -64 ) )
		pInput->Y_AXIS = 0;
}

// Mempak Crc (taken from Mupen64)
//////////////////////////////////////////////////////////////////////////
unsigned char mempack_crc(unsigned char *data)
{
	int i;
	unsigned char CRC = 0;

	for( i = 0; i <= 0x20; i++ )
	{
		int mask;

		for( mask = 0x80; mask >= 1; mask >>= 1 )
		{
			int xor_tap = (CRC & 0x80) ? 0x85 : 0x00;
			CRC <<= 1;
			if (i != 0x20 && (data[i] & mask)) CRC |= 1;
			CRC ^= xor_tap;
		}
	}

	return CRC;
}

// internal_ReadController (mupen64)
//////////////////////////////////////////////////////////////////////////
void internal_ReadController( int Control, BYTE *Command )
{
	switch( Command[2] )
	{
	case 1:
		//if (Controls[Control].Present)
		if( Control == 0 )
		{
			N64_BUTTONS Input;

			ProcessInput( &Input );
			
			Command[4] = 0;
			Command[5] = 0;
			Command[6] = Input.Y_AXIS;
			Command[7] = Input.X_AXIS;
			//memcpy32p( &Command[3], &Input.Value );
			//printf("3");
		}
		break;

	case 2: // read controller pack
		//* We ignore the controller pack for now
		break;

	case 3: // write controller pack
		//* We ignore the controller pack for now
		break;
	}
}

// internal_ControllerCommand (mupen64)
//////////////////////////////////////////////////////////////////////////
void internal_ControllerCommand( int Control, BYTE *Command )
{
	switch( Command[2] )
	{
		case 0x00: // check
		case 0xFF:
			if( Command[1] & 0x80 )
				break;

			//* Controller 1 always present
			if( Control == 0 ) //if( Controls[Control].Present )
			{
				Command[3] = 0x05;
				Command[4] = 0x00;
				Command[5] = 0x02;
			}
			else
				Command[1] |= 0x80;
			break;

		case 0x01:
			if( Control != 0 ) //if (!Controls[Control].Present)
				Command[1] |= 0x80;
			break;

		case 0x02: // read controller pack
			//* For the time being, we do not suport mempak
			if( Control == 0 )
			{
				memset(&Command[5], 0, 0x20);
				Command[0x25] = 0;
			}
			else
				Command[1] |= 0x80;
			break;

		case 0x03: // write controller pack
			if( Control == 0 )
			{
				Command[0x25] = mempack_crc( &Command[5] );
			}
			else
				Command[1] |= 0x80;
			break;
	}
}
// Write to Pif Ram (taken from mupen64)
//////////////////////////////////////////////////////////////////////////
void PifRamWrite( void )
{
	int i = 0;
	int iChannel = 0;

	if( N64Regs->PIF_Ram[0x3F] > 1 )
	{
		switch( N64Regs->PIF_Ram[0x3F] )
		{
			case 0x08:
				N64Regs->PIF_Ram[0x3F] = 0;
				N64Regs->MI[2] |= 0x02;
				N64Regs->SI[3] |= 0x1000;
				CheckInterrupts();
				break;
			case 0x10:
				memset( N64Regs->PIF_Mem, 0, 0x7C0 );
				break;
			case 0x30:
				N64Regs->PIF_Ram[0x3F] = 0x80;		
				break;
			case 0xC0:
				memset( N64Regs->PIF_Ram, 0, 0x40 );
				break;
			default:
				printf("error in update_pif_write : %x\n", N64Regs->PIF_Ram[0x3F]);
		}

		return;
	}

	while( i < 0x40 )
	{
		switch( N64Regs->PIF_Ram[i] )
		{
		case 0x00:
			if( ++iChannel > 6 ) 
				i = 0x40;
			break;

		case 0xFE:
			i = 0x40;
			break;

		case 0xFF:
			break;

		//* Taken from PJ64, ???
		case 0xB4: 
		case 0x56: 
		case 0xB8: 
			break; 

		default:
			if( !(N64Regs->PIF_Ram[i] & 0xC0) )
			{
				if (iChannel < 4)
					internal_ControllerCommand(iChannel, &N64Regs->PIF_Ram[i] );
				else if( iChannel == 4 )
					EepromCommand( &N64Regs->PIF_Ram[i] );

				i += N64Regs->PIF_Ram[i] + ( N64Regs->PIF_Ram[i + 1] & 0x3F ) + 1;

				iChannel++;
			}
			else
				i = 0x40;
		}

		i++;
	}

	//* Finish Write
	N64Regs->PIF_Ram[0x3F] = 0;
}

// Read Pif Ram
//////////////////////////////////////////////////////////////////////////
void PifRamRead( void )
{
	int iController = 0;
	int iCommandBlock = 0;

	while( iCommandBlock < 64 )
	{
		if( N64Regs->PIF_Ram[iCommandBlock] != 0xFF )
		{
			//* Check if End command
			if( N64Regs->PIF_Ram[iCommandBlock] == 0xFE )
				iCommandBlock = 64;
			
			//* Process next block
			iCommandBlock += 8;
			iController ++;
			continue;
		}

		if( ( N64Regs->PIF_Ram[iCommandBlock + 2] & 0xC0 ) == 0 )
		{
			switch( N64Regs->PIF_Ram[iCommandBlock + 3] )
			{
				case ePifCommand_ReadController:
				{
					//* Check Length
					if( N64Regs->PIF_Ram[iCommandBlock + 2] != 0x04 )
						N64Regs->PIF_Ram[iCommandBlock + 2] |= 0x40;

					//* We only care about controller 1
					if( iController == 0 )
					{
						N64_BUTTONS Input;

						ProcessInput( &Input );

						memcpy32p( &N64Regs->PIF_Ram[iCommandBlock + 4], &Input );
					}
					else
						N64Regs->PIF_Ram[iCommandBlock + 2] |= 0x80;

				}
				break;

				case ePifCommand_ReadMempack:
					break;

				case ePifCommand_WriteMempack:
					break;
			}
		}

		//* Process next block
		iCommandBlock += 8;
		iController ++;
	}
}

// Byteswap Pif
//////////////////////////////////////////////////////////////////////////
void ByteSwapPif( void )
{
	BYTE PifTemp[64];
	int i;

	memcpy( PifTemp, N64Regs->PIF_Ram, 64 );

	for( i = 0; i < 64; i += 4 )
	{
		N64Regs->PIF_Ram[i + 0] = PifTemp[i + 3];
		N64Regs->PIF_Ram[i + 1] = PifTemp[i + 2];
		N64Regs->PIF_Ram[i + 2] = PifTemp[i + 1];
		N64Regs->PIF_Ram[i + 3] = PifTemp[i + 0];
	}
}

// Eeprom Commands
//////////////////////////////////////////////////////////////////////////
void EepromCommand ( BYTE * Command )
{
//	if (SaveUsing == Auto) { SaveUsing = Eeprom_4K; }

	switch( Command[2] )
	{
		case 0: // check
			//if (SaveUsing != Eeprom_4K &&  SaveUsing != Eeprom_16K) {
			//	Command[1] |= 0x80;
			//	break;
			//}

			if( Command[1] != 3 ) 
			{ 
				Command[1] |= 0x40;

				if( ( Command[1] & 3 ) > 0 )
					Command[3] = 0x00;

				//if( SaveUsing == Eeprom_4K ) 
				//{
					if( ( Command[1] & 3 ) > 1 )
						Command[4] = 0x80;
				//} 
				//else 
				//{
				//	if( ( Command[1] & 3 ) > 1 )
				//		Command[4] = 0xC0;
				//}

				if( ( Command[1] & 3 ) > 2 )
					Command[5] = 0x00;
			} 
			else 
			{
				Command[3] = 0x00;
				Command[4] = 0x80; ////SaveUsing == Eeprom_4K?0x80:0xC0;
				Command[5] = 0x00;
			}
			break;

		case 4: // Read from Eeprom
			if( Command[0] != 2 ) { printf( "What am I meant to do with this Eeprom Command\n" ); }
			if( Command[1] != 8 ) { printf( "What am I meant to do with this Eeprom Command\n" ); }
			ReadFromEeprom(&Command[4],Command[3]);
			break;

		case 5: // Write to Eeprom
			if( Command[0] != 10) { printf( "What am I meant to do with this Eeprom Command\n" ); }
			if( Command[1] != 1 ) { printf( "What am I meant to do with this Eeprom Command\n" ); }
			WriteToEeprom(&Command[4],Command[3]);
			break;

		default:
			printf("Unkown EepromCommand %d",Command[2]);
			break;
	}
}

// Load Eeprom From File
//////////////////////////////////////////////////////////////////////////
int LoadEepromFromFile ( void )
{
	char szEprFile[256] = "";
	int iStrLen;
	int iFileID;
	int iFileLen;

	//* Copy Name
	strcpy( szEprFile, "ms0:/n64/" );
	strcat( szEprFile, szRomFile );

	//* Get Length
	iStrLen = strlen( szEprFile );

	//* Modify Extension
	szEprFile[iStrLen - 3] = 'e';
	szEprFile[iStrLen - 2] = 'p';
	szEprFile[iStrLen - 1] = 'r';

	//* OpenFile
	iFileID = sceIoOpen( szEprFile, PSP_O_RDWR, 0777 );

	//* If fail, try create
	if( !( iFileID > 0 ) )
		iFileID = sceIoOpen( szEprFile, PSP_O_RDWR|PSP_O_CREAT, 0777 );

	//* Get File Length
	iFileLen = sceIoLseek( iFileID, 0, SEEK_END );
	sceIoLseek( iFileID, 0, SEEK_SET );

	//* Incase Error, Create Eeprom
	if( iFileLen < 512 )
	{
		BYTE* uBuffer;
		
		//* Make Empty Buffer
		uBuffer = (BYTE *)malloc( 512 );

		if( uBuffer )
		{
			memset( uBuffer, 0, 512 );

			//* Write Blank
			sceIoWrite( iFileID, uBuffer, 512 );

			//* Seek Start
			sceIoLseek( iFileID, 0, SEEK_SET );

			//* Free Buffer
			free( uBuffer );
		}
	}

	//* Return Handle
	return iFileID;
}

// Read From Eeprom
//////////////////////////////////////////////////////////////////////////
void ReadFromEeprom ( BYTE* Buffer, int line )
{
	int i;
	int iFileID = LoadEepromFromFile();
	
	if( !( iFileID > 0 ) )
	{
		//* Failed, Dummy mem
		for( i = 0; i < 8; i++ )
			Buffer[i]= 0x00; //EEPROM[line*8+i];
	}
	else
	{
		//* Seek & Read
		sceIoLseek( iFileID, line * 8, SEEK_SET );
		sceIoRead( iFileID, Buffer, 8 );

		//* Close Handle
		sceIoClose( iFileID );
	}
}

// Write To Eeprom
//////////////////////////////////////////////////////////////////////////
void WriteToEeprom ( BYTE* Buffer, int line )
{
	int iFileID = LoadEepromFromFile();

	if( !( iFileID > 0 ) )
		return;

	//* Seek & Write
	sceIoLseek( iFileID, line * 8, SEEK_SET );
	sceIoWrite( iFileID, Buffer, 8 );

	//* Close Handle
	sceIoClose( iFileID );
}
