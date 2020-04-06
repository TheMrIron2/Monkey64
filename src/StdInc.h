//////////////////////////////////////////////////////////////////////////
// Standard Include
//////////////////////////////////////////////////////////////////////////
// Standard Include Header

#ifndef _STDINC_H_
#define _STDINC_H_

// Needed Psp Include Files
//////////////////////////////////////////////////////////////////////////
#include <pspkernel.h>
//#include <pspvfpu.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>
#include <psppower.h>

#ifndef printf
#define printf	pspDebugScreenPrintf
#endif

// Macros
//////////////////////////////////////////////////////////////////////////
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

// Defines
//////////////////////////////////////////////////////////////////////////
#define BUF_WIDTH			( 512 )

#define SCR_WIDTH			( 480 )
#define SCR_HEIGHT			( 272 )

#define PIXEL_SIZE			( 4 )
#define VPORT_SIZE			( 2048 )
#define FRAME_SIZE			( BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE )
#define ZBUFF_SIZE			( BUF_WIDTH * SCR_HEIGHT * 2 )

#define EDRAM_START			( (FRAME_SIZE * 2) + ZBUFF_SIZE )

//#define DEBUGER_ENABLED

// Size Defines
//////////////////////////////////////////////////////////////////////////
#define		BYTE		unsigned char
#define		WORD		unsigned short int
#define		DWORD		unsigned long
#define		QWORD		unsigned long long

// Rom Header Structure (temp, will trash soon)
//////////////////////////////////////////////////////////////////////////
typedef struct ROM_typ
{
  WORD valid;
  unsigned short is_compressed;   // 12=uncompressed, 13 = compressed
  unsigned short unknown;                 // always 0x40
  DWORD Clockrate;
  DWORD Program_Counter;
  DWORD Release;
  DWORD CRC1;
  DWORD CRC2;
  QWORD filler1;
  BYTE Image_Name[20];
  short int filler2[7];
  BYTE Manu_ID;
  WORD Cart_ID;
  BYTE Country_Code;
  short int filler3;
  DWORD BOOTCODE;
} ROMHeader;

// Size unions (Borrowed from Pj64)
//////////////////////////////////////////////////////////////////////////
typedef union tagUWORD {
	long				W;
	float				F;
	unsigned long		UW;
	short				HW[2];
	unsigned short		UHW[2];
	char				B[4];
	unsigned char		UB[4];
} MIPS_WORD;

typedef union tagUDWORD {
	double				D;
	long long			DW;
	unsigned long long	UDW;
	long				W[2];
	float				F[2];
	unsigned long		UW[2];
	short				HW[4];
	WORD				UHW[4];
	char				B[8];
	BYTE				UB[8];
} MIPS_DWORD;

typedef union tagVect {
	double				FD[2];
	long long			DW[2];
	unsigned long long	UDW[2];
	long				W[4];
	float				FS[4];
	unsigned long		UW[4];
	short				HW[8];
	unsigned short		UHW[8];
	char				B[16];
	unsigned char		UB[16];
} MIPS_VECTOR;

#endif
