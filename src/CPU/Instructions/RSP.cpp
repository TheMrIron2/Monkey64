//////////////////////////////////////////////////////////////////////////
// Rsp Core
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
#include "../3DMath.h"
#include "../../Video_Psp.h"
#include "../../Main_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspHle[256])( void );
RdpState* RdpCurState = NULL;
DWORD segment[16];
int iRspMicrocode = 2;
int iBufferReady = 0;
int iRspRunning;

extern DWORD RdpInstrOpcode[4];
extern unsigned int iCopyCfbToDisp;
extern int iEmulatorRunning;
extern int iRspRunning;
extern int RdpOpSize[256];

extern int WaitForInput( u16 uInputButton );

// Init Rsp
//////////////////////////////////////////////////////////////////////////
void InitRsp ( void )
{
	//* Set Rsp PC Pointer to SP_PC_REG
	N64Regs->RspPC = &N64Regs->SP[8];

	//* Clear Segment Buffer
	memset( segment, 0, 4*16 );

	//* Clear Rdp State
	memset( RdpCurState, 0, sizeof(RdpState) );

	//* Setup UCode
	SetRspUCode( iRspMicrocode );
}

void ByteSwap16( void* out, void* in )
{
	((char *)out)[0] = ((char*)in)[1];
	((char *)out)[1] = ((char*)in)[0];
}

void ByteSwap32( void* out, void* in )
{
	((char *)out)[0] = ((char*)in)[3];
	((char *)out)[1] = ((char*)in)[2];
	((char *)out)[2] = ((char*)in)[1];
	((char *)out)[3] = ((char*)in)[0];
}

// Run Rsp Emulation
//////////////////////////////////////////////////////////////////////////
void RunRsp ( void ) 
{
	if( ( N64Regs->SP[4] & 0x001 ) == 0 )
	{
		if( ( N64Regs->SP[4] & 0x002 ) == 0 )
		{
			RunRspHLE();
		} 
	}
}

// Run Rsp HLE
//////////////////////////////////////////////////////////////////////////
void RunRspHLE ( void )
{
	DWORD TaskType;

	TaskType = *(DWORD *)&g_pDMem[0xFC0];

	if( TaskType == 1 )
	{
		ProcessDList();

		N64Regs->DPC[3] &= ~2;
		N64Regs->DlistCount += 1; 
	}
	
	if( TaskType == 2 )
	{
		//* Process Audio List
		N64Regs->AlistCount += 1; 
	}

	N64Regs->SP[4] |= 0x0203;

	if( ( N64Regs->SP[4] & 0x040 ) != 0 )
	{
		//* Set SP Interupt
		N64Regs->MI[2] |= 0x01;
		CheckInterrupts();
	}
}

// Process Rsp List
//////////////////////////////////////////////////////////////////////////
void ProcessDList ( void )
{
	N64Regs->RspHlePC[0] = *(DWORD *)&g_pDMem[0x0ff0];
	N64Regs->RspPCi = 0;

	iRspRunning = 1;

	//* Clear
	RdpCurState->RdpPrimColor[0] = 0xff;
	RdpCurState->RdpPrimColor[1] = 0xff;
	RdpCurState->RdpPrimColor[2] = 0xff;
	RdpCurState->RdpPrimColor[3] = 0xff;

	//* Start Disp List
	SetupDispList( 1 );

	//* Reset Colors
	sceGuColor( 0xFFFFFFFF );
	sceGuTexEnvColor( 0xFFFFFFFF );

	//* Hack/Clear Screen
	sceGuClearDepth( 0 );
	sceGuClearColor( *(unsigned int *)RdpCurState->RdpFillColor );
	sceGuClear( GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT );

	while( iRspRunning )
	{
		RdpInstrOpcode[0] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] ];
		RdpInstrOpcode[1] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 4 ];

		if( RdpOpSize[RdpInstrOpcode[0] >> 24] > 8 )
		{
			RdpInstrOpcode[2] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 12 ];
			RdpInstrOpcode[3] = *(DWORD *)&g_pRDRam[ N64Regs->RspHlePC[N64Regs->RspPCi] + 20 ];
		}

		OpcodeSet_RspHle[RdpInstrOpcode[0] >> 24]();
	}

	SetupDispList( 2 );
}

// Process Display List
//////////////////////////////////////////////////////////////////////////
void ProcessRDPList ( void )
{
}

// Set Rsp UCode (taken from gl plugin)
//////////////////////////////////////////////////////////////////////////
void SetRspUCode( BYTE uCode )
{
	// Initialize all commands to RSP_Unknown
	for( int i = 0x00; i < 0x100; i++ )
		OpcodeSet_RspHle[i] = RspHleOp_NI;

	switch( uCode )
	{
		case 0: //UCODE_FAST3D
			RspInitFast3D();
			break;
		case 1: //UCODE_FAST3DEXT
			RspInitFast3DEXT();
			break;
		case 2: //UCODE_F3DEX
			RspInitF3DEX();
			break;
		case 3: //UCODE_F3DEX2:
			RspInitF3DEX2();
			break;
	}

//	RSP.update = 0xFFFFFFFF;
//	RSP.perspNorm = 1.0f;
}

// Instruction: No Instruction ( Unemulated / Not Suported )
//////////////////////////////////////////////////////////////////////////
void RspHleOp_NI ( void )
{
	printf( "0x%08X - ", (unsigned int)RdpInstrOpcode[0] );
	printf( "0x%08X\n", (unsigned int)RdpInstrOpcode[1] );

	iEmulatorRunning = 0;
	iRspRunning = 0;
}

// Load N64 Matrix
//////////////////////////////////////////////////////////////////////////
void RspLoadMatrix( float mtx[4][4], u32 address )
{
	float recip = 1.5258789e-05f;

	struct _N64Matrix
	{
		short integer[4][4];
		WORD fraction[4][4];
	} *n64Mat = (struct _N64Matrix *)&g_pRDRam[address];

	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			mtx[i][j] = (float)(n64Mat->integer[i][j^1]) + (float)(n64Mat->fraction[i][j^1]) * recip;
}

// Combine Matricies
//////////////////////////////////////////////////////////////////////////
void RspCombineMatrices()
{
	CopyMatrix( RdpCurState->combinedMtx, RdpCurState->projectionMtx );
	MultMatrix( RdpCurState->combinedMtx, RdpCurState->modelViewStack[RdpCurState->modelViewi] );

	RdpCurState->UpdateCombineMtx = 0;
}

// Load Verticies
//////////////////////////////////////////////////////////////////////////
void RspLoadVertices( DWORD address, BYTE v0, BYTE num )
{
	Vertex *destVertex;
	RSPVertex *srcVertex;
	int i;

//	if( RdpCurState->UpdateCombineMtx )
//		RspCombineMatrices();

	//if (RSP.update & UPDATE_LIGHTS)
	//	RSP_UpdateLights();

	srcVertex = (RSPVertex*)&g_pRDRam[address];
	destVertex = &RdpCurState->RspVertices[v0];

	for (i = 0; i < num; i++)
	{
		destVertex->x = (float)srcVertex->x;
		destVertex->y = (float)srcVertex->y;
		destVertex->z = (float)srcVertex->z;
		destVertex->flag = srcVertex->flag;
		destVertex->s = (float)srcVertex->s * 0.03125f;
		destVertex->t = (float)srcVertex->t * 0.03125f;

		if (RdpCurState->RdpGeometryMode & 0x00020000) //FAST3D_GEOMETRYMODE_LIGHTING
		{
			destVertex->nx = (float)srcVertex->normal.x;
			destVertex->ny = (float)srcVertex->normal.y;
			destVertex->nz = (float)srcVertex->normal.z;

//			Normalize( &destVertex->nx );
//			RSP_LightVertex( v0+i );

//			if (RSP.geometryMode & RSP_GEOMETRYMODE_TEXTURE_GEN)
//			{
//				TransformVector( &destVertex->nx, RSP.combinedMtx );
//				Normalize( &destVertex->nx );
//				if (RSP.geometryMode & RSP_GEOMETRYMODE_TEXTURE_GEN_LINEAR)
//				{   
//					destVertex->s = acosf(destVertex->nx) * 325.94931f;
//					destVertex->t = acosf(destVertex->ny) * 325.94931f;
//				}
//				else
//				{
//					destVertex->s = (destVertex->nx + 1.0f) * 512.0f;
//					destVertex->t = (destVertex->ny + 1.0f) * 512.0f;
//				}
//			}

			destVertex->r = 1.0f;
			destVertex->g = 1.0f;
			destVertex->b = 1.0f;
			destVertex->a = 1.0f;
		}
		else
		{			       
			destVertex->r = srcVertex->color.r * 0.0039215689f;
			destVertex->g = srcVertex->color.g * 0.0039215689f;
			destVertex->b = srcVertex->color.b * 0.0039215689f;
			destVertex->a = srcVertex->color.a * 0.0039215689f;
		}

		destVertex->s *= RdpCurState->RspTexture.scaleS;
		destVertex->t *= RdpCurState->RspTexture.scaleT;

//		TransformVertex( &destVertex->x, RdpCurState->combinedMtx );

		if( RdpCurState->RdpOtherMode[1] & 0x4 )
			destVertex->z = RdpCurState->RdpPrimDepth; // * (float)destVertex->w;

		destVertex->changed = 1;
		
		srcVertex++;
		destVertex++;
	}

	//* NOTE - Disable here WTF?!?!?!
	sceKernelDcacheWritebackAll();
}

// Add Triangle
//////////////////////////////////////////////////////////////////////////
void RspAddTriangle ( BYTE tri[3] )
{
	int i;

	if( RdpCurState->RspTriangleCount < 80 )
	{
		RdpCurState->RspTriangles[RdpCurState->RspTriangleCount][0] = tri[0];
		RdpCurState->RspTriangles[RdpCurState->RspTriangleCount][1] = tri[1];
		RdpCurState->RspTriangles[RdpCurState->RspTriangleCount][2] = tri[2];
		RdpCurState->RspTriangleCount++;
	}
}

// Update State
//////////////////////////////////////////////////////////////////////////
void RspUpdateState ( void )
{
	//* Culling
 	if( RdpCurState->RdpGeometryMode & RdpCurState->RspGeomModeCullBoth )
	{
		sceGuEnable( GU_CULL_FACE );

		if( RdpCurState->RdpGeometryMode & RdpCurState->RspGeomModeCullBack )
			sceGuFrontFace( GU_CCW );
		else
			sceGuFrontFace( GU_CW );
	}
	else
		sceGuDisable( GU_CULL_FACE );

	// Z buffer
	if( RdpCurState->RdpGeometryMode & RdpCurState->RspGeomModeZBuff )
		sceGuEnable( GU_DEPTH_TEST );
	else
		sceGuDisable( GU_DEPTH_TEST );

	// Z compare
	if ( RdpCurState->RdpOtherMode[1] & 0x0010 )
		sceGuDepthFunc( GU_GEQUAL ); //GL_LESS
	else
		sceGuDepthFunc( GU_ALWAYS );

	// Z update
	if( RdpCurState->RdpOtherMode[1] & 0x0020 )
		sceGuDepthMask( 0 );
	else
		sceGuDepthMask( 1 );

	// Enable alpha test for threshold mode
	if( ( ( RdpCurState->RdpOtherMode[1] & 0x03 ) == 0x01 ) && !( RdpCurState->RdpOtherMode[1] & 0x2000 ) )
	{
		sceGuEnable( GU_ALPHA_TEST );
		sceGuAlphaFunc( GU_GEQUAL, (unsigned int)RdpCurState->RdpBlendColor[3], 0xff );
	}
	// Used in TEX_EDGE and similar render modes
 	else if( RdpCurState->RdpOtherMode[1] & 0x1000 )
	{
		sceGuEnable( GU_ALPHA_TEST );

		// Arbitrary number -- gives nice results though
		sceGuAlphaFunc( GU_GEQUAL, 1, 0xff );
	}
	else
		sceGuDisable( GU_ALPHA_TEST );
}
// Draw Triangles
//////////////////////////////////////////////////////////////////////////
void RspDrawTriangles ( void )
{
	static int fileID;
	int i,j;

	if( RdpCurState->RspTriangleCount == 0 )
		return;

	//* Get Verticies
	RspPspVert *vertices = (RspPspVert *)sceGuGetMemory( sizeof(RspPspVert) * ( RdpCurState->RspTriangleCount * 3 ) );
	//RspPspVert *vertices = &RdpCurState->PspTriangles[0];

	if( vertices == NULL )
		return;

	//* Activate Texture
	TextureCache_ActivateTexture( RdpCurState->RspTexture.tile );

	for( i = 0; i < RdpCurState->RspTriangleCount; i++ )
	{
		for( j = 0; j < 3; j++ )
		{
			vertices[(i*3)+j].x = RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].x;
			vertices[(i*3)+j].y = RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].y;
			vertices[(i*3)+j].z = RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].z;
			vertices[(i*3)+j].u = RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].s * current.scaleS - RdpCurState->RdpLoadedTiles[RdpCurState->RspTexture.tile].fulS * current.offsetScaleS;
			vertices[(i*3)+j].v = RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].t * current.scaleT - RdpCurState->RdpLoadedTiles[RdpCurState->RspTexture.tile].fulT * current.offsetScaleT;

			vertices[(i*3)+j].color[0] = (BYTE)(RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].r * 255.0f);
			vertices[(i*3)+j].color[1] = (BYTE)(RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].g * 255.0f);
			vertices[(i*3)+j].color[2] = (BYTE)(RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].b * 255.0f);
			vertices[(i*3)+j].color[3] = (BYTE)(RdpCurState->RspVertices[ RdpCurState->RspTriangles[i][j] ].a * 255.0f);
		}
	}

	//* NOTE - Disable here WTF?!?!?!
	sceKernelDcacheWritebackAll();

	//* Update State
	RspUpdateState();

	//* Draw Triangles
	sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_COLOR_8888|GU_TRANSFORM_3D, RdpCurState->RspTriangleCount*3, 0, vertices );

	//* Clear Triangle Count
	RdpCurState->RspTriangleCount = 0;
}
