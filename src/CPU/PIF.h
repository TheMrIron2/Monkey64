//////////////////////////////////////////////////////////////////////////
// Pif / Input
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Pif

#ifndef _PIF_H_
#define _PIF_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

// Command Emumeration
//////////////////////////////////////////////////////////////////////////
enum EnumPifCommands
{
	ePifCommand_GetStatus		= 0x00,
	ePifCommand_ReadController	= 0x01,
	ePifCommand_ReadMempack		= 0x02,
	ePifCommand_WriteMempack	= 0x03,
	ePifCommand_ReadEeprom		= 0x04,
	ePifCommand_WriteEeprom		= 0x05,
	ePifCommand_ResetController = 0xFF,
};

// N64 Input Buttons (Borrowed from Mupen64)
//////////////////////////////////////////////////////////////////////////
typedef union {
	DWORD Value;
	struct {
		unsigned R_DPAD       : 1;
		unsigned L_DPAD       : 1;
		unsigned D_DPAD       : 1;
		unsigned U_DPAD       : 1;
		unsigned START_BUTTON : 1;
		unsigned Z_TRIG       : 1;
		unsigned B_BUTTON     : 1;
		unsigned A_BUTTON     : 1;

		unsigned R_CBUTTON    : 1;
		unsigned L_CBUTTON    : 1;
		unsigned D_CBUTTON    : 1;
		unsigned U_CBUTTON    : 1;
		unsigned R_TRIG       : 1;
		unsigned L_TRIG       : 1;
		unsigned Reserved1    : 1;
		unsigned Reserved2    : 1;

		//* Note thies should be swapped but for some reason they are inverted if I dont do it.
		signed   X_AXIS       : 8;
		signed   Y_AXIS       : 8;
	};
} N64_BUTTONS;

// Functions
//////////////////////////////////////////////////////////////////////////

//* Controller Read/Write
void internal_ReadController( int Control, BYTE *Command );
void internal_ControllerCommand( int Control, BYTE *Command );

//* Pif Read/Write
void PifRamRead( void );
void PifRamWrite( void );

//* Byteswap Pif
void ByteSwapPif( void );

//* Eeprom
void EepromCommand( BYTE * Command );
int LoadEepromFromFile( void );
void ReadFromEeprom( BYTE * Buffer, int line );
void WriteToEeprom( BYTE * Buffer, int line );

#endif
