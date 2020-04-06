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
#include "../../Video_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
extern void (*OpcodeSet_RspHle[256])( void );
extern DWORD RdpInstrOpcode[4];
extern DWORD segment[16];
extern int iRspRunning;

#define RSP_SegmentAddress(seg) ( segment[(seg >> 24) & 0x0F] + (seg & 0x007FFFFF) )

// Init Fast 3D UCODE
//////////////////////////////////////////////////////////////////////////
void RspInitFast3D ( void )
{
	int i;

	for( i = 0; i < 0xFF; i++ )
		OpcodeSet_RspHle[i] = RspHleOp_Fast3DNoOp;

	//* Setup Geometry Modes
	RdpCurState->RspGeomModeZBuff = 0x00000001;
	RdpCurState->RspGeomModeCullBoth = 0x00003000;
	RdpCurState->RspGeomModeCullBack = 0x00002000;
	RdpCurState->RspGeomModeCullFront = 0x00001000;

	//* Store Fast 3D Opcodes
	OpcodeSet_RspHle[0x00] = RspHleOp_Fast3DNoOp;
	OpcodeSet_RspHle[0x01] = RspHleOp_Fast3DMtx;
	OpcodeSet_RspHle[0x02] = RspHleOp_Fast3DNoOp; // < Reserved 0
	OpcodeSet_RspHle[0x03] = RspHleOp_Fast3DMoveMem;
	OpcodeSet_RspHle[0x04] = RspHleOp_Fast3DVtx;
	OpcodeSet_RspHle[0x05] = RspHleOp_Fast3DNoOp; // < Reserved 1
	OpcodeSet_RspHle[0x06] = RspHleOp_Fast3DDL;
	OpcodeSet_RspHle[0x07] = RspHleOp_Fast3DNoOp; // < Reserved 2
	OpcodeSet_RspHle[0x08] = RspHleOp_Fast3DNoOp; // < Reserved 3
	OpcodeSet_RspHle[0x09] = RspHleOp_Fast3DNoOp; // < Sprite2D Base


	OpcodeSet_RspHle[0xB2] = RspHleOp_Fast3DNoOp; // <- Not Correct, MKT Hits here but we will ignore for now
	OpcodeSet_RspHle[0xB3] = RspHleOp_Fast3DRdpHalf2;
	OpcodeSet_RspHle[0xB4] = RspHleOp_Fast3DRdpHalf1;
	OpcodeSet_RspHle[0xB5] = RspHleOp_Fast3DNoOp; // < Line 3D
	OpcodeSet_RspHle[0xB6] = RspHleOp_Fast3DClearGeometryMode;
	OpcodeSet_RspHle[0xB7] = RspHleOp_Fast3DSetGeometryMode;
	OpcodeSet_RspHle[0xB8] = RspHleOp_Fast3DEndDL;
	OpcodeSet_RspHle[0xB9] = RspHleOp_Fast3DSetOtherModeL;
	OpcodeSet_RspHle[0xBA] = RspHleOp_Fast3DSetOtherModeH;
	OpcodeSet_RspHle[0xBB] = RspHleOp_Fast3DTexture;
	OpcodeSet_RspHle[0xBC] = RspHleOp_Fast3DMoveWord;
	OpcodeSet_RspHle[0xBD] = RspHleOp_Fast3DPopMatrix;
	OpcodeSet_RspHle[0xBE] = RspHleOp_Fast3DNoOp; //Fast3D_CullDL;
	OpcodeSet_RspHle[0xBF] = RspHleOp_Fast3DTri1;

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

// Init Fast 3D UCODE
//////////////////////////////////////////////////////////////////////////
void RspInitFast3DEXT ( void )
{
	RspInitFast3D();

	OpcodeSet_RspHle[0x04] = RspHleOp_Fast3DEXTVtx;
	OpcodeSet_RspHle[0xB1] = RspHleOp_Fast3DEXTTri2;
	OpcodeSet_RspHle[0xB5] = RspHleOp_Fast3DEXTQuad3D;
	OpcodeSet_RspHle[0xBF] = RspHleOp_Fast3DEXTTri1;
}

// Instruction: Fast 3D No Op
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DNoOp ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Mtx
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DMtx ( void )
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

// Instruction: Fast 3D Move Memory
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DMoveMem ( void )
{
#if 0
	BYTE type = (BYTE)(RdpInstrOpcode[0] >> 16) & 0xFF;
	WORD size = (WORD)RdpInstrOpcode[0] & 0xFFFF;
	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);

	switch (type)
	{
		case 0x80: //FAST3D_MOVEMEM_VIEWPORT:
			break;

		case FAST3D_MOVEMEM_LIGHT0:
		case FAST3D_MOVEMEM_L1GHT1:
		case FAST3D_MOVEMEM_LIGHT2:
		case FAST3D_MOVEMEM_LIGHT3:
		case FAST3D_MOVEMEM_LIGHT4:
		case FAST3D_MOVEMEM_LIGHT5:
		case FAST3D_MOVEMEM_LIGHT6:
		case FAST3D_MOVEMEM_LIGHT7:
			break;

		default:
			break;
	}
#endif
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Vtx
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DVtx ( void )
{
	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);
	BYTE num = (BYTE)((RdpInstrOpcode[0] >> 20) & 0x0F) + 1;
	BYTE v0 = (BYTE)((RdpInstrOpcode[0] >> 16) & 0x0F);

	// Make sure it's valid
//	if (((RSP.cmd0 & 0xFFFF) >> 4) != num)
//		if (RSP_DetectUCode())
//			return; // Found a matching uCode

	if (v0 + num > 16)
	{
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
		return;
	}

	RspLoadVertices( address, v0, num );

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Display List
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DDL ( void )
{
	BYTE push = (BYTE)((RdpInstrOpcode[0] >> 16) & 0xFF);
	DWORD address = RSP_SegmentAddress( RdpInstrOpcode[1] );

	if( push == 0x00 ) //FAST3D_DL_PUSH
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

// Instruction: Fast 3D Rdp Half 2 ?
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DRdpHalf2 ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Rdp Half 1 ?
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DRdpHalf1 ( void )
{
	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Clear Geometry Mode
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DClearGeometryMode ( void )
{
	RdpCurState->RdpGeometryMode &= ~RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Set Geometry Mode
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DSetGeometryMode ( void )
{
	RdpCurState->RdpGeometryMode |= RdpInstrOpcode[1];

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D End Display List
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DEndDL ( void )
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

// Instruction: Fast 3D Set Other Mode L
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DSetOtherModeL ( void )
{
	DWORD shift = (RdpInstrOpcode[0] >> 8) & 0xFF;
	DWORD length = RdpInstrOpcode[0] & 0xFF;
	DWORD mask = ((1 << length) - 1) << shift;

	RdpCurState->RdpOtherMode[1] &= ~mask;
	RdpCurState->RdpOtherMode[1] |= RdpInstrOpcode[1] & mask;

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Set Other Mode H
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DSetOtherModeH ( void )
{
	DWORD shift = (RdpInstrOpcode[0] >> 8) & 0xFF;
	DWORD length = RdpInstrOpcode[0] & 0xFF;
	DWORD mask = ((1 << length) - 1) << shift;

	RdpCurState->RdpOtherMode[0] &= ~mask;
	RdpCurState->RdpOtherMode[0] |= RdpInstrOpcode[1] & mask;

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Texture
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DTexture( void )
{
	RdpCurState->RspTexture.level = (BYTE)((RdpInstrOpcode[0] >> 11) & 0x3);
	RdpCurState->RspTexture.tile = (BYTE)((RdpInstrOpcode[0] >> 8) & 0x3);
	RdpCurState->RspTexture.on = (BYTE)(RdpInstrOpcode[0] & 0xFF);
	RdpCurState->RspTexture.scaleS = ((RdpInstrOpcode[1] >> 16) & 0xFFFF) * 1.5259022e-005;
	RdpCurState->RspTexture.scaleT = (RdpInstrOpcode[1] & 0xFFFF) * 1.5259022e-005;

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Move Word
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DMoveWord ( void )
{
	WORD offset = (WORD)((RdpInstrOpcode[0] >> 8) & 0xFFFF);
	BYTE index = (BYTE)(RdpInstrOpcode[0] & 0xFF);
	DWORD data = RdpInstrOpcode[1];

	switch( index )
	{
		case 0x06: //FAST3D_MOVEWORD_SEGMENT
			segment[offset >> 2] = data & 0x00FFFFFF;
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

// Instruction: Fast 3D Move Word
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DPopMatrix ( void )
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

// Instruction: Fast 3D Triangle 1
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DTri1 ( void )
{
	RSPTriangle		*srcTriangle;
	BYTE tri[3];

	RdpCurState->RspTriangleCount = 0;

	do
	{
		srcTriangle = (RSPTriangle*)&RdpInstrOpcode[1];

		tri[0] = srcTriangle->v0 / 10;
		tri[1] = srcTriangle->v1 / 10;
		tri[2] = srcTriangle->v2 / 10;
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xBF) && (RdpCurState->RspTriangleCount < 32));

	RspDrawTriangles();

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: Fast 3D Ext Vertex
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DEXTVtx ( void )
{
	DWORD address = RSP_SegmentAddress(RdpInstrOpcode[1]);
	BYTE num = (RdpInstrOpcode[0] >> 9) & 0x7f;
	BYTE v0 = ((RdpInstrOpcode[0] >> 16) & 0xff) / 5;

	if (v0 + num > 32)
	{
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
		return;
	}

	RspLoadVertices( address, v0, num );

	N64Regs->RspHlePC[N64Regs->RspPCi] += 8;
}

// Instruction: Fast 3D Ext Triange 1
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DEXTTri1 ( void )
{
	RSPTriangle		*tmpTriangle;
	BYTE tri[3];

	do
	{
		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[1];

//		RSP.triangleFlags[RSP.numTriangles] = srcTriangle->flag;
		tri[0] = tmpTriangle->v0 / 5;
		tri[1] = tmpTriangle->v1 / 5;
		tri[2] = tmpTriangle->v2 / 5;
		
		RspAddTriangle( tri );

		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xBF) && (RdpCurState->RspTriangleCount < 32));

	if (((RdpInstrOpcode[0] >> 24) != 0xB5) && ((RdpInstrOpcode[0] >> 24) != 0xB1))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: Fast 3D Ext Triangle 2
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DEXTTri2 ( void )
{
	RSPTriangle		*tmpTriangle;
	BYTE tri[3];

	do
	{
		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[0];

		//RSP.triangleFlags[RSP.numTriangles] = 0;		
		tri[0] = tmpTriangle->v0 / 5;
		tri[1] = tmpTriangle->v1 / 5;
		tri[2] = tmpTriangle->v2 / 5;
		RspAddTriangle( tri );

		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[1];

//		RSP.triangleFlags[RSP.numTriangles] = 0;
		tri[0] = tmpTriangle->v0 / 5;
		tri[1] = tmpTriangle->v1 / 5;
		tri[2] = tmpTriangle->v2 / 5;
		RspAddTriangle( tri );
		
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xB1) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0xBF) && ((RdpInstrOpcode[0] >> 24) != 0xB5))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}

// Instruction: Fast 3D Ext Quad3D
//////////////////////////////////////////////////////////////////////////
void RspHleOp_Fast3DEXTQuad3D ( void )
{
//	RSPTriangle		*tmpTriangle;
	BYTE tri[3];

	do
	{
//		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[0];
//		RSP.triangleFlags[RSP.numTriangles] = 0;
		tri[0] = ((RdpInstrOpcode[1] >> 24) & 0xFF) / 5;
		tri[1] = ((RdpInstrOpcode[1] >> 16) & 0xFF) / 5;
		tri[2] = ((RdpInstrOpcode[1] >> 8 ) & 0xFF) / 5;
		RspAddTriangle( tri );
		

//		tmpTriangle = (RSPTriangle*)&RdpInstrOpcode[1];
//		RSP.triangleFlags[RSP.numTriangles] = 0;
		tri[0] = ( RdpInstrOpcode[1]        & 0xFF) / 5;
		tri[1] = ((RdpInstrOpcode[1] >> 24) & 0xFF) / 5;
		tri[2] = ((RdpInstrOpcode[1] >> 8 ) & 0xFF) / 5;
		RspAddTriangle( tri );
		
		N64Regs->RspHlePC[N64Regs->RspPCi] += 8;

		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];
	}
	while (((RdpInstrOpcode[0] >> 24) == 0xB5) && (RdpCurState->RspTriangleCount < 80));

	if (((RdpInstrOpcode[0] >> 24) != 0xBF) && ((RdpInstrOpcode[0] >> 24) != 0xB1))
	{
		RspDrawTriangles();
	}

	OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
}
