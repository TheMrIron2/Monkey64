//////////////////////////////////////////////////////////////////////////
// Rdp Core
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Rsp Core

#include "Instructions.h"
#include "../Registers.h"
#include "../Memory.h"
#include "../Timers.h"
#include "../Exceptions.h"
#include "../Dma.h"
#include "../Video.h"
#include "../Textures.h"
#include "../Convert.h"
#include "../../Video_Psp.h"
#include "../../Helper_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
DWORD RdpInstrOpcode[4];
BYTE* pTextBuf = NULL;

extern RdpState* RdpCurState;
extern int iRspHleMode;
extern int iEmulatorRunning;
extern int iRspRunning;
extern unsigned int iCopyCfbToDisp;

extern int WaitForInput( u16 uInputButton );
extern void DispSync( void );
extern void RspUpdateState ( void );

extern DWORD segment[16];
#define RSP_SegmentAddress(seg) ( segment[(seg >> 24) & 0x0F] + (seg & 0x007FFFFF) )

// Instruction Set ( Rdp Core )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RdpCore[256])(void) =
{
	RdpOp_NI,  RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI,  RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI,  RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI,  RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,

	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
	RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI, RdpOp_NI,
 
	RdpOp_NOP,         RdpOp_NOP,           RdpOp_NOP,          RdpOp_NOP,         RdpOp_NOP,        RdpOp_NOP,        RdpOp_NOP,          RdpOp_NOP,
	RdpOp_NI,          RdpOp_NI,            RdpOp_NI,           RdpOp_NI,          RdpOp_NI,         RdpOp_NI,         RdpOp_NI,           RdpOp_NI,
	RdpOp_NOP,         RdpOp_NOP,           RdpOp_NOP,          RdpOp_NOP,         RdpOp_NOP,        RdpOp_NOP,        RdpOp_NOP,          RdpOp_NOP,
	RdpOp_NOP,         RdpOp_NOP,           RdpOp_NOP,          RdpOp_NOP,         RdpOp_NOP,        RdpOp_NOP,        RdpOp_NOP,          RdpOp_NOP,
	RdpOp_NOP,         RdpOp_NOP,           RdpOp_NOP,          RdpOp_NOP,         RdpOp_RecText,    RdpOp_NI,         RdpOp_LoadSync,     RdpOp_PipeSync,
	RdpOp_TileSync,    RdpOp_FullSync,      RdpOp_SetKeyGB,     RdpOp_SetKeyR,     RdpOp_SetConv,    RdpOp_SetScissor, RdpOp_SetPrimDepth, RdpOp_SetOther,
	RdpOp_LoadTLut,    RdpOp_NI,            RdpOp_SetTileSize,  RdpOp_LoadBlock,   RdpOp_LoadTile,   RdpOp_SetTile,    RdpOp_RecFill,      RdpOp_SetFillColor,
	RdpOp_SetFogColor, RdpOp_SetBlendColor, RdpOp_SetPrimColor, RdpOp_SetEvnColor, RdpOp_SetCombine, RdpOp_SetTextImg, RdpOp_SetZImage,    RdpOp_SetColorImage,

//* Instruction Rdp Reference ( Taken from Michael Tedder rdp docs, 0xC0 )
//	NOP        , 0            , 0           , 0          , 0         , 0           , 0           , 0                ,
//  TriFill    , TriFillZBuf  , TriText     , TriTextZBuf, TriShade  , TriShadeZBuf, TriShadeText, TriShadeTextZBuff, 
//	0          , 0            , 0           , 0          , RecText   , RecTextFlip , LoadSync    , PipeSync         , 
//  TileSync   , FullSync     , SetKeyGB    , SetKeyR    , SetConv   , SetScissor  , SetPrimDepth, SetOther         , 
//	LoadTLut   , 0            , SetTileSize , LoadBlock  , LoadTile  , SetTile     , RecFill     , SetFillColor     ,
//	SetFogColor, SetBlendColor, SetPrimColor, SetEvnColor, SetCombine, SetTextImg  , SetZBuf     , SetFrameBuf       
};

// Instruction Size ( Rdp Core )
//////////////////////////////////////////////////////////////////////////
int RdpOpSize[256] =
{
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,

	8, 8, 8, 8,  8, 8, 8, 8,
	8, 8, 8, 8,  8, 8, 8, 8,
	8, 8, 8, 8,  8, 8, 8, 8,
	8, 8, 8, 8,  8, 8, 8, 8,
	8, 8, 8, 8, 16, 8, 8, 8,
	8, 8, 8, 8,  8, 8, 8, 8,
	8, 8, 8, 8,  8, 8, 8, 8,
	8, 8, 8, 8,  8, 8, 8, 8,
};

// Unswap Copy to Texture Mem
//////////////////////////////////////////////////////////////////////////
inline void UnswapToTMem( int iDramOff, int iTmemOff, u32 numBytes )
{
	BYTE* pTMemByte = (BYTE *)N64Regs->TMem;
	u32 i = 0;

	while( i < numBytes )
	{
		pTMemByte[ iTmemOff + i ] = g_pRDRam[ ( iDramOff + i ) ^ 3 ];
		i++;
	}

	sceKernelDcacheWritebackAll();
}

// Instruction: No Instruction ( Unemulated / Not Suported )
//////////////////////////////////////////////////////////////////////////
void RdpOp_NI ( void )
{
	//* Clear screen so we can read the print
	//ResetDisplay();

	int i;

	N64Regs->DPC[2] -= 16;

	printf( "hit unknown Rdp opcode\n" );

	for( i = 0; i < 3; i++ )
	{
		if( ( N64Regs->DPC[3] & 0x001 ) )
		{
			RdpInstrOpcode[0] = LoadDWord( N64Regs->DPC[2] );
			RdpInstrOpcode[1] = LoadDWord( N64Regs->DPC[2] + 4 );
		}
		else
		{
			RdpInstrOpcode[0] = LoadDWord( N64Regs->DPC[2] );
			RdpInstrOpcode[1] = LoadDWord( N64Regs->DPC[2] + 4 );
		}

		printf( "0x%08X - ", (unsigned int)RdpInstrOpcode[0] );
		printf( "0x%08X\n", (unsigned int)RdpInstrOpcode[1] );

		N64Regs->DPC[2] += 8;
	}

	//N64Regs->DPC[2] -= 8;

	//printf( "0x%08X: hit unknown Rdp opcode %08X (op: %02X)\n", (unsigned int)N64Regs->DPC[2], (unsigned long long)RdpInstrOpcode, (unsigned int)( ( RdpInstrOpcode >> 56 ) & 0x3f ) );
	//printf( "0x%02X\n", (unsigned int)( ( RdpInstrOpcode >> 56 ) & 0x3f ) );
	printf( "0x%08x-0x%08x-0x%08x-0x%08x\n", (unsigned int)N64Regs->DPC[0], (unsigned int)N64Regs->DPC[1], (unsigned int)N64Regs->DPC[2], (unsigned int)N64Regs->DPC[3] );

	//* Exit
	N64Regs->DPC[2] = N64Regs->DPC[1];
	iEmulatorRunning = 0;
	iRspRunning = 0;
}

// Instruction: NOP ( No Operation Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_NOP ( void )
{
	// Invalid instruction
	// Maybe junk from prev instruction?

	//* Ignoring
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: RectText ( Draw Textured Rectangle Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_RecText ( void )
{
//	E4) Draw textured rectangle: draws a tile to the current frame buffer.
//  - Upper X 44 - 55
//  - Upper Y 32 - 43
//	- Lower X 12 - 23
//	- Lower Y  0 - 11

	//* Get Temp Verts
	RspPspVert *vertices = (RspPspVert *)sceGuGetMemory( sizeof(RspPspVert) * 2 );
	//RspPspVert *vertices = &RdpCurState->PspTriangles[0];
	float fTemp[4];

	BYTE uTile = ( RdpInstrOpcode[1] >> 24 ) & 0x7;

	//* Setup Textures
	TextureCache_ActivateTexture( uTile );

	if( vertices )
	{
		//* Setup Quad xyz
		vertices[0].x = ( ( RdpInstrOpcode[1] & 0x00FFF000 ) >> 12 ) * 0.25f;
		vertices[0].y = ( ( RdpInstrOpcode[1] & 0x00000FFF )       ) * 0.25f;
		vertices[0].z = RdpCurState->RdpOtherMode[1] & 0x4 ? RdpCurState->RdpPrimDepth : 0.0f;

		vertices[1].x = ( ( RdpInstrOpcode[0] & 0x00FFF000 ) >> 12 ) * 0.25f;
		vertices[1].y = ( ( RdpInstrOpcode[0] & 0x00000FFF )       ) * 0.25f;
		vertices[1].z = RdpCurState->RdpOtherMode[1] & 0x4 ? RdpCurState->RdpPrimDepth : 0.0f;

		//* Calc UV Slope
		fTemp[2] = (short)( ( RdpInstrOpcode[3] >> 16 ) & 0xFFFF ) * 9.765625e-4f;
		fTemp[3] = (short)( ( RdpInstrOpcode[3]       ) & 0xFFFF ) * 9.765625e-4f;

		if( RdpCurState->RdpOtherMode[0] & 0x00200000 )
		{
			fTemp[2] /= 4.0f;
			vertices[1].x += 1.0f;
			vertices[1].y += 1.0f;
		} 

		//* Setup UV
		fTemp[0] = (short)( ( RdpInstrOpcode[2] & 0xFFFF0000 ) >> 16 ) / 32.0f;
		fTemp[1] = (short)( ( RdpInstrOpcode[2] & 0x0000FFFF )       ) / 32.0f;

		fTemp[2] = fTemp[0] + ( ( vertices[1].x - vertices[0].x - 1) * fTemp[2] );
		fTemp[3] = fTemp[1] + ( ( vertices[1].y - vertices[0].y - 1) * fTemp[3] );

		if (fTemp[0] < fTemp[2]) fTemp[2] += 1.0f; else fTemp[0] += 1.0f;
		if (fTemp[1] < fTemp[3]) fTemp[3] += 1.0f; else fTemp[1] += 1.0f;

		vertices[0].u = fTemp[0] * current.scaleS - RdpCurState->RdpLoadedTiles[uTile].fulS * current.offsetScaleS;
		vertices[0].v = fTemp[1] * current.scaleT - RdpCurState->RdpLoadedTiles[uTile].fulT * current.offsetScaleT;
		vertices[1].u = fTemp[2] * current.scaleS - RdpCurState->RdpLoadedTiles[uTile].fulS * current.offsetScaleS;
		vertices[1].v = fTemp[3] * current.scaleT - RdpCurState->RdpLoadedTiles[uTile].fulT * current.offsetScaleT;

		vertices[0].x = ( vertices[0].x - ViScreenHalf[0] ) / ViScreenHalf[0];
		vertices[1].x = ( vertices[1].x - ViScreenHalf[0] ) / ViScreenHalf[0];

		vertices[0].y = ( ( vertices[0].y - ViScreenHalf[1] ) / ViScreenHalf[1] ) * -1.0f;
		vertices[1].y = ( ( vertices[1].y - ViScreenHalf[1] ) / ViScreenHalf[1] ) * -1.0f;

		*(unsigned int *)vertices[0].color = *(unsigned int *)RdpCurState->RdpPrimColor;
		*(unsigned int *)vertices[1].color = *(unsigned int *)RdpCurState->RdpPrimColor;

		//* Reset Projection
		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();

		RspUpdateState();

		//* Render Quad
		sceGumDrawArray( GU_SPRITES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_COLOR_8888|GU_TRANSFORM_3D, 2, 0, vertices );
	}

	//* Note, ignoring the ST/UV cords
	//* Note, This instruction is 128bits long.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 24;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 16 );
}

// Instruction: LoadSync ( Load Sync Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_LoadSync ( void )
{
//	E6) RDP load sync: synchronizes the RDP's texture loader, ensuring any
//		current texture load op has been completed.
	
	//* Ignoring
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: PipeSync ( Pipe Sync Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_PipeSync ( void )
{
//	E7) RDP pipe sync: synchronizes the RDP's pipeline, ensuring any rendering
//		or load TLUT (texture lookup) has been completed.

	//DispSync();

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: TileSync ( Tile Sync Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_TileSync ( void )
{
//	E8) RDP tile sync: synchronizes the RDP's tile cache, ensuring a previous
//	    set tile command has been completed.
	
	//* Ignoring
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: FullSync ( Full Sync Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_FullSync ( void )
{
//	E9) RDP full sync: flushes and immediately completes all pending operations
//		and generates a DP interrupt.

	//DispSync();

	N64Regs->MI[2] |= 0x20;
	CheckInterrupts();

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetKeyGB ( Set Key Green Blue Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetKeyGB ( void )
{
	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetKeyR ( Set Key Red Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetKeyR ( void )
{
	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetConv ( Set Conv? Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetConv ( void )
{
	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetScissor ( Set Scissor Rectangle Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetScissor ( void )
{
//	ED) Set scissor: sets a 2D clipping plane for all rendering operations.  all
//		renders will be placed within this area of memory.  (extremely useful for
//		split screens)
//  - Upper X 44 - 55
//  - Upper Y 32 - 43
//	- Lower X 12 - 23
//	- Lower Y  0 - 11

/* Will Finish Later
	float ScissorUL[2];
	float ScissorBR[2];

	ScissorUL[0] = (float)( ( RdpInstrOpcode[0] >> 14 ) & 0x3FF );
	ScissorUL[1] = (float)( ( RdpInstrOpcode[0] >>  2 ) & 0x3FF );

	ScissorBR[0] = (float)( ( RdpInstrOpcode[1] >> 14 ) & 0x3FF );
	ScissorBR[1] = (float)( ( RdpInstrOpcode[1] >>  2 ) & 0x3FF );

	ScissorUL[0] = ( ScissorUL[0] - ViScreenHalf[0] ) / ViScreenHalf[0];
	ScissorBR[0] = ( ScissorBR[0] - ViScreenHalf[0] ) / ViScreenHalf[0];

	ScissorUL[1] = ( ( ScissorUL[1] - ViScreenHalf[1] ) / ViScreenHalf[1];
	ScissorBR[1] = ( ( ScissorBR[1] - ViScreenHalf[1] ) / ViScreenHalf[1];

	//* Set Scissor Mode
	sceGuScissor(80,16,400,256);
*/
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetPrimDepth ( Set Primary Z Depth Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetPrimDepth ( void )
{
	RdpCurState->RdpPrimDepth = ( ( RdpInstrOpcode[1] >> 16) & 0x7FFF ) * 3.0518509e-5f;

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetOther ( Set Other Modes Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetOther ( void )
{
//	EF) Set other mode: ???

	//* Save other mode settings
	RdpCurState->RdpOtherMode[0] = RdpInstrOpcode[0] & 0x00FFFFFF;
	RdpCurState->RdpOtherMode[1] = RdpInstrOpcode[1];

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: LoadTLut ( Load Texture Look Up Table Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_LoadTLut ( void )
{
//	F0) Load TLUT: loads a 16 or 256 color index lookup table for palettized
//		textures.  a previous set texture image is used as the raw data for the
//		palette.

	BYTE uTile = ( RdpInstrOpcode[1] >> 24 ) & 0x07;
	WORD i, iCount = (WORD)( ( RdpInstrOpcode[1]>> 14 ) & 0x3FF ) + 1;

	DWORD *destPalette = &N64Regs->TMemPal[RdpCurState->RdpLoadedTiles[uTile].tMem - 256];
	WORD *srcPalette = (WORD * )( g_pRDRam + RdpCurState->RdpTImg.address );

	if( ( RdpCurState->RdpOtherMode[0] & 0x0000C000 ) == 0x0000C000 )
	{
		for( i = 0; i < iCount; i++ )
			destPalette[i] = IA88_RGBA8888( swapword(srcPalette[i^1]) );
	}
	else
	{
		for( i = 0; i < iCount; i++ )
			destPalette[i] = RGBA5551_RGBA8888( swapword(srcPalette[i^1]) );
	}

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetTileSize ( Set Tile Size Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetTileSize ( void )
{
//	F2) Set tile size: sets the X and Y size of a tile through parameters SH and
//		TH.

	BYTE uTile = ( RdpInstrOpcode[1] >> 24 ) & 0x7;

	RdpCurState->RdpLoadedTiles[uTile].fulS = ( ( RdpInstrOpcode[0] >> 12 ) & 0xFFF ) / 4.0f;
	RdpCurState->RdpLoadedTiles[uTile].fulT = ( RdpInstrOpcode[0] & 0xFFF ) / 4.0f;

	RdpCurState->RdpLoadedTiles[uTile].ulS = ( RdpInstrOpcode[0] >> 14 ) & 0x3FF;
	RdpCurState->RdpLoadedTiles[uTile].ulT = ( RdpInstrOpcode[0] >>  2 ) & 0x3FF;
	RdpCurState->RdpLoadedTiles[uTile].lrS = ( RdpInstrOpcode[1] >> 14 ) & 0x3FF;
	RdpCurState->RdpLoadedTiles[uTile].lrT = ( RdpInstrOpcode[1] >>  2 ) & 0x3FF;

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: LoadBlock ( Load Block Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_LoadBlock ( void )
{
//	F3) Load block: loads a block into the RDP's texture memory.  I'm not sure
//		how this differs from load tile...

	// 32-bit textures are strange on the N64. They are actually arranged much like
	// matrices.

	// On the real N64, it gets loaded with low and high words split, every four
	// pixels (qword alignment). Like this:

	// |RG0|RG1|RG2|RG3|BA0|BA1|BA2|BA3|

	// Instead of putting them in order, like this:

	// |RG0|BA0|RG1|BA1|RG2|BA2|RG3|BA3|

	// This is why 32-bit texture's line value seems to be half what it should be,
	// and why they're interleaved on qwords instead of dwords.

	// Fortunately, games depend on the RDP to do this, and since we're doing the
	// loading here, we can fudge the emulation. We do have to do the interleaving
	// correctly though. We can load them normally, we just have to do special handling
	// with 32-bit textures to make sure they're interleaved/deinterleaved correctly.

	static int iCounter = 0;

	//QWORD *src, *dest;
	DWORD srcOff, destOff;
	BYTE *src, *dest;
	DWORD y, bytes, line, bpl, height;

	BYTE uTile = ( RdpInstrOpcode[1] >> 24 ) & 0x7;
	RdpCurState->RdpCurrentTile = &RdpCurState->RdpLoadedTiles[uTile];

	// ulS - ignored
	// ulT - ignored
	// lrS = ((width * height) >> size_shift) - 1. So, number of texels to load = (lrS + 1) << size_shift;
	// dxT = reciprocal of number of 64-bit words in a line, 1.11 fixed point

	RdpCurState->RdpCurrentTile->ulS = (WORD)( RdpInstrOpcode[0] >> 14 ) & 0x3FF;
	RdpCurState->RdpCurrentTile->ulT = (WORD)( RdpInstrOpcode[0] >>  2 ) & 0x3FF;

	RdpCurState->RdpCurrentTile->lrS = (WORD)( RdpInstrOpcode[1] >> 12 ) & 0xFFF;
	RdpCurState->RdpCurrentTile->dxT = (WORD)RdpInstrOpcode[1] & 0xFFF;

	bytes = (RdpCurState->RdpCurrentTile->lrS + 1) << max(1, RdpCurState->RdpCurrentTile->size - 1);

	// Stop it if it tries to load outside RDRAM, not at all, or bigger than TMem
	if( ( ( RdpCurState->RdpTImg.address + bytes ) > 0x007FFFFF ) || ( bytes == 0 ) || ( ( ( RdpCurState->RdpCurrentTile->tMem << 3 ) + bytes ) > 4096 ) )
	{
		N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
		return;
	}

	src = (BYTE *)( g_pRDRam + RdpCurState->RdpTImg.address );
	dest = (BYTE *)( N64Regs->TMem + RdpCurState->RdpCurrentTile->tMem );

	srcOff = RdpCurState->RdpTImg.address;
	destOff = RdpCurState->RdpCurrentTile->tMem * 8;

	if( RdpCurState->RdpCurrentTile->dxT )
	{
		line = ( 2047 + RdpCurState->RdpCurrentTile->dxT ) / RdpCurState->RdpCurrentTile->dxT;
		bpl = line << 3;
		height = bytes / bpl;

		if( RdpCurState->RdpCurrentTile->size == 3 )
		{
			for (y = 0; y < height; y++)
			{
				UnswapToTMem( srcOff, destOff, bpl );
				if( y & 1 ) QWordInterleave( dest, line );

				src += line * 8;
				dest += line * 8;
				srcOff += line * 8;
				destOff += line * 8; 
			}
		}
		else
		{
			for (y = 0; y < height; y++)
			{
				UnswapToTMem( srcOff, destOff, bpl );
				if( y & 1 ) DWordInterleave( dest, line );

				src += line * 8;
				dest += line * 8;
				srcOff += line * 8;
				destOff += line * 8; 
			}
		}
	}
	else
	{
		UnswapToTMem( srcOff, destOff, bytes );
	}

	RdpCurState->RdpCurrentTile->loadType = 0;

	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: LoadTile ( Load Tile Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_LoadTile ( void )
{
//	F4) Load tile: loads a tile into the RDP's texture memory.  SH and TH are the
//		X and Y size of the tile to load.  note that the RDP only has 4k of texture
//		memory and it can only load 2k at once.  (see Important Things above)

//	void (*Interleave)( void *mem, WORD numDWords );
	DWORD srcOff, destOff;
	DWORD uAddr;
	WORD uLine, uBPL, uY, uHeight;

	BYTE *DramSrc;
	BYTE *TmemDest;
	//QWORD *TmemDest;

	BYTE uTile = ( RdpInstrOpcode[1] >> 24 ) & 0x7;
	RdpCurState->RdpCurrentTile = &RdpCurState->RdpLoadedTiles[uTile];

	RdpCurState->RdpCurrentTile->ulS = (WORD)( RdpInstrOpcode[0] >> 14 ) & 0x3FF;
	RdpCurState->RdpCurrentTile->ulT = (WORD)( RdpInstrOpcode[0] >>  2 ) & 0x3FF;
	RdpCurState->RdpCurrentTile->lrS = (WORD)( RdpInstrOpcode[1] >> 14 ) & 0x3FF;
	RdpCurState->RdpCurrentTile->lrT = (WORD)( RdpInstrOpcode[1] >>  2 ) & 0x3FF;

	// If line is 0, what's the point in continuing?
	if( RdpCurState->RdpCurrentTile->line == 0 )
	{
		N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
		return;
	}

	uHeight = RdpCurState->RdpCurrentTile->lrT - RdpCurState->RdpCurrentTile->ulT + 1;

	TmemDest = (BYTE *)( N64Regs->TMem + RdpCurState->RdpCurrentTile->tMem );
	uAddr = RdpCurState->RdpTImg.address + RdpCurState->RdpCurrentTile->ulT * RdpCurState->RdpTImg.bpl + (RdpCurState->RdpCurrentTile->ulS << RdpCurState->RdpTImg.size >> 1);
	uBPL = ( RdpCurState->RdpCurrentTile->lrS - RdpCurState->RdpCurrentTile->ulS + 1 ) << RdpCurState->RdpCurrentTile->size >> 1;

	// Don't let it go outside RDRAM
	if( ( ( uAddr + uHeight * uBPL ) > 0x007FFFFF ) ||
		( ( ( RdpCurState->RdpCurrentTile->tMem << 3 ) + uBPL * uHeight ) > 4096 ) ) // Stay within TMEM
	{
		N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
		return;
	}

	DramSrc = &g_pRDRam[uAddr];

	srcOff = uAddr;
	destOff = RdpCurState->RdpCurrentTile->tMem * 8;

	// Line given for 32-bit is half what it seems it should since they split the
	// high and low words...it's strange.
	if( RdpCurState->RdpCurrentTile->size == 3 )
	{
		uLine = RdpCurState->RdpCurrentTile->line << 1;

		for( uY = 0; uY < uHeight; uY++ )
		{
			UnswapToTMem( srcOff, destOff, uBPL );
			if (uY & 1) QWordInterleave( TmemDest, uLine );

			DramSrc += RdpCurState->RdpTImg.bpl;
			TmemDest += uLine * 8;

			srcOff += RdpCurState->RdpTImg.bpl;
			destOff += uLine * 8; 
		}
	}
	else
	{
		uLine = RdpCurState->RdpCurrentTile->line;

		for( uY = 0; uY < uHeight; uY++ )
		{
			UnswapToTMem( srcOff, destOff, uBPL );
			if (uY & 1) DWordInterleave( TmemDest, uLine );

			DramSrc += RdpCurState->RdpTImg.bpl;
			TmemDest += uLine * 8;

			srcOff += RdpCurState->RdpTImg.bpl;
			destOff += uLine * 8; 
		}
	}

	RdpCurState->RdpCurrentTile->loadType = 1;

	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetTile ( Set Tile Modes Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetTile ( void )
{
//	F5) Set tile: sets up flags for a tile to-be-loaded or already-loaded.  this
//		specifies the type of the tile (RGBA, YUV, CI [color index via a palette]),
//		the size (also known as bitdepth, which is usually G_IM_SIZ_16b for RGBA
//		and G_IM_SIZ_8b for CI), the offset of this tile into texture memory (tmem),
//		and other bitflags that control texture clamping and/or mirroring.

	BYTE uTile = ( RdpInstrOpcode[1] >> 24 ) & 0x7;

	RdpCurState->RdpLoadedTiles[uTile].format  = ( ( RdpInstrOpcode[0] >> 21 ) & 0x7 );
	RdpCurState->RdpLoadedTiles[uTile].size    = ( ( RdpInstrOpcode[0] >> 19 ) & 0x3 );
	RdpCurState->RdpLoadedTiles[uTile].line    = ( ( RdpInstrOpcode[0] >>  9 ) & 0x1FF );
	RdpCurState->RdpLoadedTiles[uTile].tMem    = (   RdpInstrOpcode[0]         & 0x1FF );
	RdpCurState->RdpLoadedTiles[uTile].palette = ( ( RdpInstrOpcode[1] >> 20 ) & 0xF );
	RdpCurState->RdpLoadedTiles[uTile].clampT  = ( ( RdpInstrOpcode[1] >> 19 ) & 0x1 );
	RdpCurState->RdpLoadedTiles[uTile].mirrorT = ( ( RdpInstrOpcode[1] >> 18 ) & 0x1 );
	RdpCurState->RdpLoadedTiles[uTile].maskT   = ( ( RdpInstrOpcode[1] >> 14 ) & 0xF );
	RdpCurState->RdpLoadedTiles[uTile].shiftT  = ( ( RdpInstrOpcode[1] >> 10 ) & 0xF );
	RdpCurState->RdpLoadedTiles[uTile].clampS  = ( ( RdpInstrOpcode[1] >>  9 ) & 0x1 );
	RdpCurState->RdpLoadedTiles[uTile].mirrorS = ( ( RdpInstrOpcode[1] >>  8 ) & 0x1 );
	RdpCurState->RdpLoadedTiles[uTile].maskS   = ( ( RdpInstrOpcode[1] >>  4 ) & 0xF );
	RdpCurState->RdpLoadedTiles[uTile].shiftS  = (   RdpInstrOpcode[1]         & 0xF );

	//* Force clamping if not masking
	if( RdpCurState->RdpLoadedTiles[uTile].maskS == 0 )
		RdpCurState->RdpLoadedTiles[uTile].clampS = 1;

	if( RdpCurState->RdpLoadedTiles[uTile].maskT == 0 )
		RdpCurState->RdpLoadedTiles[uTile].clampT = 1;

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: Rect Fill ( Draw Filled Rectangle Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_RecFill ( void )
{
//	F6) Fill rectangle: draws a filled rectangle to the current frame buffer, using
//		the current fill color.
//  - Lower X 44 - 55
//  - Lower Y 32 - 43
//	- Upper X 12 - 23
//	- Upper Y  0 - 11

#if 0
	if( RdpCurState->RdpCImg.address == RdpCurState->RdpZImgAddr )
	{	
		sceGuClearDepth( 0 );
		sceGuClear( GU_DEPTH_BUFFER_BIT ); //GU_COLOR_BUFFER_BIT
	}
	else
	{	
//		sceGuClearDepth( 0 );
//		sceGuClearColor( *(unsigned int *)RdpCurState->RdpFillColor );
//		sceGuClear( GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT );
//	}

//
		sceGuColor( *(unsigned int *)RdpCurState->RdpFillColor );

		//* Get Temp Verts
		float *vertices = (float *)sceGuGetMemory( sizeof(float) * 6 );

		if( vertices )
		{
			//* Setup Quad
			vertices[0] = ( ( RdpInstrOpcode[0] >> 14 ) & 0x3FF ) * 1.0f;
			vertices[1] = ( ( RdpInstrOpcode[0] >>  2 ) & 0x3FF ) * 1.0f;
			vertices[2] = 0.0f;

			vertices[3] = ( ( ( RdpInstrOpcode[1] >> 14 ) & 0x3FF ) * 1.0f ) + 1;
			vertices[4] = ( ( ( RdpInstrOpcode[1] >>  2 ) & 0x3FF ) * 1.0f ) + 1;
			vertices[5] = 0.0f;

			vertices[0] = ( vertices[0] - ViScreenHalf[0] ) / ViScreenHalf[0];
			vertices[3] = ( vertices[3] - ViScreenHalf[0] ) / ViScreenHalf[0];

			vertices[1] = ( ( vertices[1] - ViScreenHalf[1] ) / ViScreenHalf[1] ) * -1.0f;
			vertices[4] = ( ( vertices[4] - ViScreenHalf[1] ) / ViScreenHalf[1] ) * -1.0f;

			//* Reset Projection
			sceGumMatrixMode(GU_PROJECTION);
			sceGumLoadIdentity();
			sceGumMatrixMode(GU_MODEL);
			sceGumLoadIdentity();

			//* Disable Textures & Z
			sceGuDisable( GU_TEXTURE_2D );
			sceGuDepthMask( 1 );

			//* Render Quad
			sceGumDrawArray( GU_SPRITES, GU_VERTEX_32BITF|GU_TRANSFORM_3D, 2, 0, vertices );

			sceGuEnable( GU_TEXTURE_2D );
			sceGuDepthMask( 0 );
		}
	}
#endif

	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetFillColor ( Set Fill Color Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetFillColor ( void )
{
//	F7) Set fill color: sets the color to be used for fill operations (like F6).
//	-- Note, Can be used to flush display or draw small rect.

	//* Set Fill color
	RdpCurState->RdpFillColor[0] = ( ( RdpInstrOpcode[1] >> 11 ) & 0x1F ) << 3; // Red
	RdpCurState->RdpFillColor[1] = ( ( RdpInstrOpcode[1] >>  6 ) & 0x1F ) << 3; // Green
	RdpCurState->RdpFillColor[2] = ( ( RdpInstrOpcode[1] >>  1 ) & 0x1F ) << 3; // Blue
	RdpCurState->RdpFillColor[3] = ( RdpInstrOpcode[1] & 0x1 ) ? 0xFF : 0x00;   // Alpha

	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetFogColor ( Set Fog Color Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetFogColor ( void )
{
//	F8) Set fog color: sets the fog color?

	//* Currently ignoring.
	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetBlendColor ( Set Blend Color Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetBlendColor ( void )
{
//	F9) Set blend color: sets the texture blender color?

	RdpCurState->RdpBlendColor[0] = ( RdpInstrOpcode[1] >> 24 ) & 0xFF; // Red
	RdpCurState->RdpBlendColor[1] = ( RdpInstrOpcode[1] >> 16 ) & 0xFF; // Green
	RdpCurState->RdpBlendColor[2] = ( RdpInstrOpcode[1] >>  8 ) & 0xFF; // Blue
	RdpCurState->RdpBlendColor[3] = ( RdpInstrOpcode[1]       ) & 0xFF; // Alpha

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetPrimColor ( Set Primitive Color Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetPrimColor ( void )
{
//	FA) Set prim color: sets the primative color

	//* Set Primative color
	RdpCurState->RdpPrimColor[0] = ( RdpInstrOpcode[1] >> 24 ) & 0xFF; // Red
	RdpCurState->RdpPrimColor[1] = ( RdpInstrOpcode[1] >> 16 ) & 0xFF; // Green
	RdpCurState->RdpPrimColor[2] = ( RdpInstrOpcode[1] >>  8 ) & 0xFF; // Blue
	RdpCurState->RdpPrimColor[3] = ( RdpInstrOpcode[1]       ) & 0xFF; // Alpha

	//RdpPrimColor = 0xff554433; //RdpInstrOpcode[1];

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetEvnColor ( Set Evn Color Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetEvnColor ( void )
{
//	FB) Set evn color: sets the evn color
	BYTE EnvColor[4];

	//* Set Env Color
	EnvColor[0] = ( RdpInstrOpcode[1] >> 24 ) & 0xFF; // Red
	EnvColor[1] = ( RdpInstrOpcode[1] >> 16 ) & 0xFF; // Green
	EnvColor[2] = ( RdpInstrOpcode[1] >>  8 ) & 0xFF; // Blue
	EnvColor[3] = ( RdpInstrOpcode[1]       ) & 0xFF; // Alpha

	sceGuTexEnvColor( *(unsigned int *)EnvColor );

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetCombine ( Set Combine Mode Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetCombine ( void )
{
//	FC) Set combine mode: ??? messes with the RDP's color combiner ???

	//* Store Combine Mode
	RdpCurState->RdpCombindMode[0] = RdpInstrOpcode[0];
	RdpCurState->RdpCombindMode[1] = RdpInstrOpcode[1];

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetTextImg ( Set Texture Image Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetTextImg ( void )
{
//	FD) Set texture image: sets the pointer, format, and size of a texture map,
//		most commonly used before a load operation.  this appears to be changed
//		by a set tile command later which is incorrect.  the valid parameters for
//		format and size are passed into to this command.

	RdpCurState->RdpTImg.format  = ( RdpInstrOpcode[0] >> 21 ) & 0x7;
	RdpCurState->RdpTImg.size    = ( RdpInstrOpcode[0] >> 19 ) & 0x3;
	RdpCurState->RdpTImg.width   = ( RdpInstrOpcode[0] & 0xFFF ) + 1;
	RdpCurState->RdpTImg.address = RSP_SegmentAddress( RdpInstrOpcode[1] );
	// bpl doesn't actually exist on the N64, but it helps here
	RdpCurState->RdpTImg.bpl = RdpCurState->RdpTImg.width << RdpCurState->RdpTImg.size >> 1;

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetTextImg ( Set Texture Image Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetZImage ( void )
{
//	FE) Set z-buffer image: sets the pointer, format, and size of the Z-buffer
//		to use for triangle drawing.

	// We just need the Address
	RdpCurState->RdpZImgAddr = RSP_SegmentAddress( RdpInstrOpcode[1] );

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Instruction: SetColorImage ( Set Color Image Rdp Instruction )
//////////////////////////////////////////////////////////////////////////
void RdpOp_SetColorImage ( void )
{
//	FF) Set frame buffer: sets the pointer, format, and size of the frame buffer
//		for the RDP to use.

	RdpCurState->RdpCImg.format  = ( RdpInstrOpcode[0] >> 21 ) & 0x7;
	RdpCurState->RdpCImg.size    = ( RdpInstrOpcode[0] >> 19 ) & 0x3;
	RdpCurState->RdpCImg.width   = ( RdpInstrOpcode[0] & 0xFFF ) + 1;
	RdpCurState->RdpCImg.address = RSP_SegmentAddress( RdpInstrOpcode[1] );
	// bpl doesn't actually exist on the N64, but it helps here
	RdpCurState->RdpCImg.bpl = RdpCurState->RdpCImg.width << RdpCurState->RdpCImg.size >> 1;

	//if( iRspHleMode )
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
	//else
	//	N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
}

// Run Rdp
//////////////////////////////////////////////////////////////////////////
void RunRdp( void )
{
/*
	int iReadDMEM = 1;
	unsigned int uOp;

	if( ( N64Regs->DPC[3] & 0x001 ) == 0 ) // DPC_STATUS_XBUS_DMEM_DMA
		iReadDMEM = 0;

//	if( ( N64Regs->DPC[3] & 0x002 ) == 0 ) // DPC_STATUS_XBUS_DMEM_DMA
//		return;

	iCopyCfbToDisp = 0;

	RdpCurState->RdpPrimColor[0] = 0xff;
	RdpCurState->RdpPrimColor[1] = 0xff;
	RdpCurState->RdpPrimColor[2] = 0xff;
	RdpCurState->RdpPrimColor[3] = 0xff;

	if( iReadDMEM && ( N64Regs->DPC[2] >= 0x1000 ) )
		return;

	while( 1 ) 
	{
		//* Fetch Opcode
		if( iReadDMEM )
		{
			N64Regs->DPC[2] &= 0xFFF;
			RdpInstrOpcode[0] = LoadDWordDMem( N64Regs->DPC[2] );
			RdpInstrOpcode[1] = LoadDWordDMem( N64Regs->DPC[2] + 4 );

			uOp = ( RdpInstrOpcode[0] >> 24 ) & 0xFF;

			if( RdpOpSize[uOp] > 8 )
			{
				RdpInstrOpcode[2] = LoadDWordDMem( N64Regs->DPC[2] + 8 );
				RdpInstrOpcode[3] = LoadDWordDMem( N64Regs->DPC[2] + 12 );
			}
		}
		else
		{
			RdpInstrOpcode[0] = LoadDWord( N64Regs->DPC[2] );
			RdpInstrOpcode[1] = LoadDWord( N64Regs->DPC[2] + 4 );

			uOp = ( RdpInstrOpcode[0] >> 24 ) & 0xFF;

			if( RdpOpSize[uOp] > 8 )
			{
				RdpInstrOpcode[2] = LoadDWord( N64Regs->DPC[2] + 8 );
				RdpInstrOpcode[3] = LoadDWord( N64Regs->DPC[2] + 12 );
			}
		}

//		pspDebugScreenSetXY(0,0);
//		printf( "RDP: 0x%08x  ", (unsigned int)N64Regs->DPC[2] );
//		printf( "OP: %d  \n", (unsigned int)uOp );

//		printf( "0x%08X - ", (unsigned int)RdpInstrOpcode[0] );
//		printf( "0x%08X\n", (unsigned int)RdpInstrOpcode[1] );

//		WaitForInput( PSP_CTRL_CROSS );

		if(  N64Regs->DPC[2] + RdpOpSize[uOp] > N64Regs->DPC[1] )
			return;

		if( uOp < 0xC0 )
		{
			N64Regs->DPC[2] = ( N64Regs->DPC[2] + 8 );
			continue;
		}

		//* Execute Opcode
		OpcodeSet_RdpCore[uOp]();
	}
*/
}
