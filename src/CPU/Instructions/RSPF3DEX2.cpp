//////////////////////////////////////////////////////////////////////////
// Rsp F3DEX2 Hle Core
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Rsp Core

#include "Instructions.h"
#include "../Registers.h"
#include "../Memory.h"
#include "../Timers.h"
#include "../Exceptions.h"
#include "../Dma.h"
#include "../Video.h"
#include "../3DMath.h"
#include "../../Video_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
extern void (*OpcodeSet_RspHle[256])( void );
extern DWORD RdpInstrOpcode[4];
extern DWORD segment[16];
extern int iRspRunning;

extern int WaitForInput ( u16 uInputButton );

#define RSP_SegmentAddress(seg) ( segment[(seg >> 24) & 0x0F] + (seg & 0x007FFFFF) )

// Init F3DEX2 UCODE
//////////////////////////////////////////////////////////////////////////
void RspInitF3DEX2 ( void )
{
	int i;

	for( i = 0; i < 0xFF; i++ )
		OpcodeSet_RspHle[i] = RspHleOp_F3DEX2NoOp;

	//* Setup Geometry Modes
	RdpCurState->RspGeomModeZBuff = 0x00000001;
	RdpCurState->RspGeomModeCullBoth = 0x00000600;
	RdpCurState->RspGeomModeCullBack = 0x00000400;
	RdpCurState->RspGeomModeCullFront = 0x00000200;

	//* Store Fast 3D Opcodes
	OpcodeSet_RspHle[0x00] = RspHleOp_F3DEX2NoOp;
	OpcodeSet_RspHle[0x01] = RspHleOp_F3DEX2Vtx;
	OpcodeSet_RspHle[0x02] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2ModifyVtx;
	OpcodeSet_RspHle[0x03] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2CullDL;
	OpcodeSet_RspHle[0x04] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2BranchZ;
	OpcodeSet_RspHle[0x05] = RspHleOp_F3DEX2Tri1;
	OpcodeSet_RspHle[0x06] = RspHleOp_F3DEX2Tri2;
	OpcodeSet_RspHle[0x07] = RspHleOp_F3DEX2Quad;
	OpcodeSet_RspHle[0x08] = RspHleOp_F3DEX2Line3D;

	OpcodeSet_RspHle[0x0A] = RspHleOp_F3DEX2NoOp; //RspHleOp_S2DEXBgCopy;

	OpcodeSet_RspHle[0xD3] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2Special3;
	OpcodeSet_RspHle[0xD4] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2Special2;
	OpcodeSet_RspHle[0xD5] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2Special1;
	OpcodeSet_RspHle[0xD6] = RspHleOp_F3DEX2NoOp; //RspHleOp_F3DEX2DmaIO;
	OpcodeSet_RspHle[0xD7] = RspHleOp_F3DEX2Texture;
	OpcodeSet_RspHle[0xD8] = RspHleOp_F3DEX2PopMatirx;
	OpcodeSet_RspHle[0xD9] = RspHleOp_F3DEX2GeometryMode;
	OpcodeSet_RspHle[0xDA] = RspHleOp_F3DEX2Mtx;
	OpcodeSet_RspHle[0xDB] = RspHleOp_F3DEX2MoveWord;
	OpcodeSet_RspHle[0xDC] = RspHleOp_F3DEX2MoveMem;
	OpcodeSet_RspHle[0xDD] = RspHleOp_F3DEX2NoOp; // <- Load uCode
	OpcodeSet_RspHle[0xDE] = RspHleOp_F3DEX2DL;
	OpcodeSet_RspHle[0xDF] = RspHleOp_F3DEX2EndDL;

	OpcodeSet_RspHle[0xE0] = RspHleOp_F3DEX2NoOp; // <- SP Nop
	OpcodeSet_RspHle[0xE1] = RspHleOp_F3DEX2RdpHalf1;
	OpcodeSet_RspHle[0xE2] = RspHleOp_F3DEX2SetOtherModeL;
	OpcodeSet_RspHle[0xE3] = RspHleOp_F3DEX2SetOtherModeH;

	OpcodeSet_RspHle[0xF1] = RspHleOp_F3DEX2RdpHalf2;

	//* Store RDP Opcodes
	// Set known RDP commands
	OpcodeSet_RspHle[0xC0] = RdpOp_NOP;

	OpcodeSet_RspHle[0xE4] = RdpOp_RecText;
	OpcodeSet_RspHle[0xE5] = RdpOp_NOP; //RDP_TexRectFlip
	OpcodeSet_RspHle[0xE6] = RdpOp_LoadSync;
	OpcodeSet_RspHle[0xE7] = RdpOp_PipeSync;
	OpcodeSet_RspHle[0xE8] = RdpOp_TileSync;
	OpcodeSet_RspHle[0xE9] = RdpOp_FullSync;
	OpcodeSet_RspHle[0xEA] = RdpOp_SetKeyGB;
	OpcodeSet_RspHle[0xEB] = RdpOp_SetKeyR;
	OpcodeSet_RspHle[0xEC] = RdpOp_SetConv;
	OpcodeSet_RspHle[0xED] = RdpOp_SetScissor;
	OpcodeSet_RspHle[0xEE] = RdpOp_SetPrimDepth;
	OpcodeSet_RspHle[0xEF] = RdpOp_SetOther;

	OpcodeSet_RspHle[0xF0] = RdpOp_LoadTLut;
	OpcodeSet_RspHle[0xF2] = RdpOp_SetTileSize;
	OpcodeSet_RspHle[0xF3] = RdpOp_LoadBlock;
	OpcodeSet_RspHle[0xF4] = RdpOp_LoadTile;
	OpcodeSet_RspHle[0xF5] = RdpOp_SetTile;
	OpcodeSet_RspHle[0xF6] = RdpOp_RecFill;
	OpcodeSet_RspHle[0xF7] = RdpOp_SetFillColor;
	OpcodeSet_RspHle[0xF8] = RdpOp_SetFogColor;
	OpcodeSet_RspHle[0xF9] = RdpOp_SetBlendColor;
	OpcodeSet_RspHle[0xFA] = RdpOp_SetPrimColor;
	OpcodeSet_RspHle[0xFB] = RdpOp_SetEvnColor;
	OpcodeSet_RspHle[0xFC] = RdpOp_SetCombine;
	OpcodeSet_RspHle[0xFD] = RdpOp_SetTextImg;
	OpcodeSet_RspHle[0xFE] = RdpOp_SetZImage;
	OpcodeSet_RspHle[0xFF] = RdpOp_SetColorImage;
}

// Instruction: F3DEX2 No Op
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2NoOp ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Mtx
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Mtx ( void )
{
	BYTE command = (BYTE)((RdpInstrOpcode[0] & 0xFF) ^ 1);

	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);

	float mtx[4][4];

	RspLoadMatrix( mtx, address );
	
	if( command & 0x04 ) //F3DEX_MTX_PROJECTION
	{
		sceGumMatrixMode(GU_PROJECTION);

        if( command & 0x02 ) //F3DEX_MTX_LOAD
			sceGumLoadMatrix( (ScePspFMatrix4 *)mtx ); //CopyMatrix( RdpCurState->projectionMtx, mtx );
		else
			sceGumMultMatrix( (ScePspFMatrix4 *)mtx ); //MultMatrix( RdpCurState->projectionMtx, mtx );

		sceGumMatrixMode(GU_MODEL);
	}
	else
	{
		if( ( command & 0x01 ) && ( RdpCurState->modelViewi < 9 ) ) //F3DEX_MTX_PUSH
		{
			sceGumPushMatrix();
			//CopyMatrix( RdpCurState->modelViewStack[RdpCurState->modelViewi+1], RdpCurState->modelViewStack[RdpCurState->modelViewi] );
			RdpCurState->modelViewi++;
		}

		if( command & 0x02 ) //F3DEX_MTX_LOAD
			sceGumLoadMatrix( (ScePspFMatrix4 *)mtx ); //CopyMatrix( RdpCurState->modelViewStack[RdpCurState->modelViewi], mtx );
		else
			sceGumMultMatrix( (ScePspFMatrix4 *)mtx ); //MultMatrix( RdpCurState->modelViewStack[RdpCurState->modelViewi], mtx );
	}

	//RdpCurState->UpdateCombineMtx = 1;

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Move Memory
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2MoveMem ( void )
{
	BYTE type = (RdpInstrOpcode[0] & 0xFF );
	WORD size = ((RdpInstrOpcode[0] >> 16) & 0xFF) + 8;
	WORD offset = ((RdpInstrOpcode[0] >> 5) & 0x7F8);
	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);

	switch( type )
	{
		case 14: //F3DEX2_MOVEMEM_MATRIX
			break;
	}

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Vtx
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Vtx ( void )
{
	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);
	BYTE num = (RdpInstrOpcode[0] >> 12) & 0xFF;
	BYTE v0 = ((RdpInstrOpcode[0] >> 1) & 0x7F) - num;

	// Make sure it's valid
//	if (((RSP.cmd0 & 0xFFFF) >> 4) != num)
//		if (RSP_DetectUCode())
//			return; // Found a matching uCode

	if (v0 + num > 80)
	{
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
		return;
	}

	RspLoadVertices( address, v0, num );

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Display List
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2DL ( void )
{
	BYTE push = (BYTE)((RdpInstrOpcode[0] >> 16) & 0xFF);
	DWORD address = RSP_SegmentAddress( RdpInstrOpcode[1] );

	if( push == 0x00 ) //F3DEX_DL_PUSH
	{
		if( N64Regs->RspPCi < 7 )
		{
			N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
			N64Regs->RspPCi++;
			N64Regs->RspHlePC[N64Regs->RspPCi] = address;
		}
		else
		{
			printf( "\n\nERROR!! RspHleOp_Fast3DDL\n\n" );
			iRspRunning = 0;
		}
	}
	else
	{
		N64Regs->RspHlePC[N64Regs->RspPCi] = address;
	}
}

// Instruction: F3DEX2 Triangle 2
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Tri2 ( void )
{
	BYTE tri[3];

	do
	{
		tri[0] = (RdpInstrOpcode[0] >> 17) & 0x7F;
		tri[1] = (RdpInstrOpcode[0] >>  9) & 0x7F;
		tri[2] = (RdpInstrOpcode[0] >>  1) & 0x7F;
		RspAddTriangle( tri );

		tri[0] = (RdpInstrOpcode[1] >> 17) & 0x7F;
		tri[1] = (RdpInstrOpcode[1] >>  9) & 0x7F;
		tri[2] = (RdpInstrOpcode[1] >>  1) & 0x7F;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0x06) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0x05) && ((RdpInstrOpcode[0] >> 24) != 0x07))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: F3DEX2 Rdp Half 2 ?
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2RdpHalf2 ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Rdp Half 1 ?
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2RdpHalf1 ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Line 3D
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Line3D ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Quad
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Quad ( void )
{
	BYTE tri[3];

	// Line3D acts as Quad3D in F3DEX, but for some reason Nintendo didn't bother renaming it...
	do
	{
		tri[0] = (RdpInstrOpcode[0] >> 17) & 0x7F;
		tri[1] = (RdpInstrOpcode[0] >>  9) & 0x7F;
		tri[2] = (RdpInstrOpcode[0] >>  1) & 0x7F;
		RspAddTriangle( tri );

		tri[0] = (RdpInstrOpcode[1] >> 17) & 0x7F;
		tri[1] = (RdpInstrOpcode[1] >>  9) & 0x7F;
		tri[2] = (RdpInstrOpcode[1] >>  1) & 0x7F;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0x07) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0x05) && ((RdpInstrOpcode[0] >> 24) != 0x06))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: F3DEX2 Geometry Mode
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2GeometryMode ( void )
{
	RdpCurState->RdpGeometryMode = (RdpCurState->RdpGeometryMode & (RdpInstrOpcode[0] & 0x00FFFFFF)) | RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 End Display List
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2EndDL ( void )
{
	if( N64Regs->RspPCi > 0 )
	{
		N64Regs->RspPCi--;
	}
	else
	{
		iRspRunning = 0;
	}
}

// Instruction: F3DEX2 Set Other Mode L
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2SetOtherModeL ( void )
{
	BYTE length = (RdpInstrOpcode[0] & 0xFF) + 1;
	BYTE shift = 32 - ((RdpInstrOpcode[0] >> 8) & 0xFF) - length;
	DWORD mask = (((1 << length) - 1) << shift);

	RdpCurState->RdpOtherMode[1] &= ~mask;
	RdpCurState->RdpOtherMode[1] |= RdpInstrOpcode[1] & mask;

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Set Other Mode H
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2SetOtherModeH ( void )
{
	BYTE length = (RdpInstrOpcode[0] & 0xFF) + 1;
	BYTE shift = 32 - ((RdpInstrOpcode[0] >> 8) & 0xFF) - length;
	DWORD mask = (((1 << length) - 1) << shift);

	RdpCurState->RdpOtherMode[0] &= ~mask;
	RdpCurState->RdpOtherMode[0] |= RdpInstrOpcode[1] & mask;

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Texture
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Texture( void )
{
	RdpCurState->RspTexture.level = (BYTE)((RdpInstrOpcode[0] >> 11) & 0x3);
	RdpCurState->RspTexture.tile = (BYTE)((RdpInstrOpcode[0] >> 8) & 0x3);
	RdpCurState->RspTexture.on = (BYTE)(RdpInstrOpcode[0] & 0xFF);
	RdpCurState->RspTexture.scaleS = ((RdpInstrOpcode[1] >> 16) & 0xFFFF) * 1.5259022e-005;
	RdpCurState->RspTexture.scaleT = (RdpInstrOpcode[1] & 0xFFFF) * 1.5259022e-005;

	RdpCurState->RdpCurrentTile = &RdpCurState->RdpLoadedTiles[RdpCurState->RspTexture.tile];
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Move Word
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2MoveWord ( void )
{
	WORD offset = (WORD)(RdpInstrOpcode[0] & 0xFFFF);
	BYTE index = (RdpInstrOpcode[0] >> 16) & 0xFF;
	DWORD data = RdpInstrOpcode[1];

	switch( index )
	{
		case 0x06: //FAST3D_MOVEWORD_SEGMENT
			segment[(offset >> 2) & 0xF] = data & 0x00FFFFFF;;
			break;

		case 0x08: //FAST3D_MOVEWORD_FOG
//			RSP.fogMultiplier = (float)(SHORT)(data >> 16);
//			RSP.fogOffset = (float)(SHORT)(data & 0xFFFF);
			break;

		case 0x02: //FAST3D_MOVEWORD_NUMLIGHT
//			RSP.numLights = (BYTE)(((data >> 5) & 0x1F) - 1);
			break;

		case 0x0e: //FAST3D_MOVEWORD_PERSPNORM
//			RSP.perspNorm = (float)(data & 0xFFFF) / 65535.0f;
			break;

		default:
			break;
	}

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Pop Matrix
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2PopMatirx ( void )
{
	if( RdpCurState->modelViewi > 0 )
	{
		sceGumPopMatrix();
		RdpCurState->modelViewi--;
		//RdpCurState->UpdateCombineMtx = 1;
	}
	//* Add matrix code here
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX2 Triangle 1
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEX2Tri1 ( void )
{
	BYTE tri[3];

	do
	{
		tri[0] = (RdpInstrOpcode[0] >> 17) & 0x7F;
		tri[1] = (RdpInstrOpcode[0] >>  9) & 0x7F;
		tri[2] = (RdpInstrOpcode[0] >>  1) & 0x7F;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0x05) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0x06) && ((RdpInstrOpcode[0] >> 24) != 0x07))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}
