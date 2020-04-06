//////////////////////////////////////////////////////////////////////////
// Rsp Fast3D Hle Core
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

// Init F3DEX UCODE
//////////////////////////////////////////////////////////////////////////
void RspInitF3DEX ( void )
{
	int i;

	//* Setup Geometry Modes
	RdpCurState->RspGeomModeZBuff = 0x00000001;
	RdpCurState->RspGeomModeCullBoth = 0x00003000;
	RdpCurState->RspGeomModeCullBack = 0x00002000;
	RdpCurState->RspGeomModeCullFront = 0x00001000;

	for( i = 0; i < 0xFF; i++ )
		OpcodeSet_RspHle[i] = RspHleOp_Fast3DNoOp;

	//* Store Fast 3D Opcodes
	OpcodeSet_RspHle[0x00] = RspHleOp_F3DEXNoOp;
	OpcodeSet_RspHle[0x01] = RspHleOp_F3DEXMtx;
	OpcodeSet_RspHle[0x03] = RspHleOp_F3DEXMoveMem;
	OpcodeSet_RspHle[0x04] = RspHleOp_F3DEXVtx;
	OpcodeSet_RspHle[0x06] = RspHleOp_F3DEXDL;

	OpcodeSet_RspHle[0xB0] = RspHleOp_F3DEXNoOp; //RspHleOp_F3DEXBranchZ;
	OpcodeSet_RspHle[0xB1] = RspHleOp_F3DEXTri2;
	OpcodeSet_RspHle[0xB2] = RspHleOp_F3DEXNoOp; // <- Not Correct, MKT Hits here but we will ignore for now
	OpcodeSet_RspHle[0xB3] = RspHleOp_F3DEXRdpHalf2;
	OpcodeSet_RspHle[0xB4] = RspHleOp_F3DEXRdpHalf1;
	OpcodeSet_RspHle[0xB5] = RspHleOp_F3DEXLine3D;
	OpcodeSet_RspHle[0xB6] = RspHleOp_F3DEXClearGeometryMode;
	OpcodeSet_RspHle[0xB7] = RspHleOp_F3DEXSetGeometryMode;
	OpcodeSet_RspHle[0xB8] = RspHleOp_F3DEXEndDL;
	OpcodeSet_RspHle[0xB9] = RspHleOp_F3DEXSetOtherModeL;
	OpcodeSet_RspHle[0xBA] = RspHleOp_F3DEXSetOtherModeH;
	OpcodeSet_RspHle[0xBB] = RspHleOp_F3DEXTexture;
	OpcodeSet_RspHle[0xBC] = RspHleOp_F3DEXMoveWord;
	OpcodeSet_RspHle[0xBD] = RspHleOp_F3DEXPopMatirx;
	OpcodeSet_RspHle[0xBE] = RspHleOp_F3DEXNoOp; // F3DEX_CullDL
	OpcodeSet_RspHle[0xBF] = RspHleOp_F3DEXTri1;

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

// Instruction: F3DEX No Op
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXNoOp ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Mtx
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXMtx ( void )
{
	BYTE command = (BYTE)((RdpInstrOpcode[0] >> 16) & 0xFF);

	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);

	float mtx[4][4];

	RspLoadMatrix( mtx, address );
	
	if( command & 0x01 ) //F3DEX_MTX_PROJECTION
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
		if( ( command & 0x04 ) && ( RdpCurState->modelViewi < 17 ) ) //F3DEX_MTX_PUSH
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

// Instruction: F3DEX Move Memory
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXMoveMem ( void )
{
	//* Load View Port or Lights

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Vtx
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXVtx ( void )
{
	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);
	BYTE num = (RdpInstrOpcode[0] >> 10) & 0x3F;
	BYTE v0 = (RdpInstrOpcode[0] >> 17) & 0x7FFF;

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

// Instruction: F3DEX Display List
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXDL ( void )
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

// Instruction: F3DEX Triangle 2
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXTri2 ( void )
{
	RSPTriangle		*tmpTriangle;
	BYTE tri[3];

	do
	{
		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[0];

		tri[0] = tmpTriangle->v0 >> 1;
		tri[1] = tmpTriangle->v1 >> 1;
		tri[2] = tmpTriangle->v2 >> 1;
		RspAddTriangle( tri );

		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[1];

		tri[0] = tmpTriangle->v0 >> 1;
		tri[1] = tmpTriangle->v1 >> 1;
		tri[2] = tmpTriangle->v2 >> 1;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xB1) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0xB5) && ((RdpInstrOpcode[0] >> 24) != 0xBF))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: F3DEX Rdp Half 2 ?
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXRdpHalf2 ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Rdp Half 1 ?
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXRdpHalf1 ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Line 3D
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXLine3D ( void )
{
	BYTE tri[3];

	// Line3D acts as Quad3D in F3DEX, but for some reason Nintendo didn't bother renaming it...
	do
	{
		tri[0] = (RdpInstrOpcode[1] >> 25) & 0x7F;
		tri[1] = (RdpInstrOpcode[1] >> 17) & 0x7F;
		tri[2] = (RdpInstrOpcode[1] >>  9) & 0x7F;
		RspAddTriangle( tri );

		tri[0] = (RdpInstrOpcode[1] >>  1) & 0x7F;
		tri[1] = (RdpInstrOpcode[1] >> 25) & 0x7F;
		tri[2] = (RdpInstrOpcode[1] >>  9) & 0x7F;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xB5) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0xB1) && ((RdpInstrOpcode[0] >> 24) != 0xBF))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: F3DEX Clear Geometry Mode
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXClearGeometryMode ( void )
{
	RdpCurState->RdpGeometryMode &= ~RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Set Geometry Mode
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXSetGeometryMode ( void )
{
	RdpCurState->RdpGeometryMode |= RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX End Display List
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXEndDL ( void )
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

// Instruction: F3DEX Set Other Mode L
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXSetOtherModeL ( void )
{
	DWORD shift = (RdpInstrOpcode[0] >> 8) & 0xFF;
	DWORD length = RdpInstrOpcode[0] & 0xFF;
	DWORD mask = ((1 << length) - 1) << shift;

	RdpCurState->RdpOtherMode[1] &= ~mask;
	RdpCurState->RdpOtherMode[1] |= RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Set Other Mode H
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXSetOtherModeH ( void )
{
	DWORD shift = (RdpInstrOpcode[0] >> 8) & 0xFF;
	DWORD length = RdpInstrOpcode[0] & 0xFF;
	DWORD mask = ((1 << length) - 1) << shift;

	RdpCurState->RdpOtherMode[0] &= ~mask;
	RdpCurState->RdpOtherMode[0] |= RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Texture
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXTexture( void )
{
	RdpCurState->RspTexture.level = (BYTE)((RdpInstrOpcode[0] >> 11) & 0x3);
	RdpCurState->RspTexture.tile = (BYTE)((RdpInstrOpcode[0] >> 8) & 0x3);
	RdpCurState->RspTexture.on = (BYTE)(RdpInstrOpcode[0] & 0xFF);
	RdpCurState->RspTexture.scaleS = ((RdpInstrOpcode[1] >> 16) & 0xFFFF) * 1.5259022e-005;
	RdpCurState->RspTexture.scaleT = (RdpInstrOpcode[1] & 0xFFFF) * 1.5259022e-005;

	RdpCurState->RdpCurrentTile = &RdpCurState->RdpLoadedTiles[RdpCurState->RspTexture.tile];
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: F3DEX Move Word
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXMoveWord ( void )
{
	WORD offset = (WORD)(RdpInstrOpcode[0] >> 8);
	BYTE index = (BYTE)RdpInstrOpcode[0];
	DWORD data = RdpInstrOpcode[1];

	switch( index )
	{
		case 0x06: //FAST3D_MOVEWORD_SEGMENT
			segment[(offset >> 2) & 0xF] = data;
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

// Instruction: F3DEX Pop Matrix
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXPopMatirx ( void )
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

// Instruction: F3DEX Triangle 1
//////////////////////////////////////////////////////////////////////////
void RspHleOp_F3DEXTri1 ( void )
{
	RSPTriangle		*tmpTriangle;
	BYTE tri[3];

	do
	{
		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[1];

		tri[0] = tmpTriangle->v0 >> 1;
		tri[1] = tmpTriangle->v1 >> 1;
		tri[2] = tmpTriangle->v2 >> 1;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xBF) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0xB1) && ((RdpInstrOpcode[0] >> 24) != 0xB5))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}
