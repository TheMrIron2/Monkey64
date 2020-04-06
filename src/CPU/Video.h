//////////////////////////////////////////////////////////////////////////
// Video
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Video Emulation

#ifndef _VIDEO_H_
#define _VIDEO_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

typedef struct 
{
	//short u, v;
	float u, v;
	unsigned char color[4];
	float x,y,z;
} RspPspVert;

// Rdp Tile (dont have a header yet for rdp only stuff
// - Taken from Gln64
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE format;	// e.g. RGBA, YUV etc
	BYTE size;		// e.g 4/8/16/32bpp
	WORD line;		// Line size
	WORD tMem;		// Location in tmem

	BYTE palette;	// 0..15
	BYTE clampS, clampT;
	BYTE mirrorS, mirrorT;
	BYTE maskS, maskT;
	BYTE shiftS, shiftT;

	float fulS, fulT; // For texture coordinates

	WORD ulS;		// Upper left S
	WORD ulT;		// Upper Left T
	WORD lrS;		// Lower Right S
	WORD lrT;		// Lower Right T
	WORD dxT;		// Reciprocol of number of QWords in a line

	BYTE loadType;
} RdpTile;

// Rdp Vertex Setup
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	short y;
	short x;

	unsigned short flag;
	short z;

	short t;
	short s;

	union {
		struct
		{
			BYTE a;
			BYTE b;
			BYTE g;
			BYTE r;
		} color;
		struct
		{
			char a;
			char z;	// b
			char y;	//g
			char x;	//r
		} normal;
	};
} RSPVertex;

typedef struct
{
	float x, y, z, w;
	float nx, ny, nz;
	float s, t;
	float r, g, b, a;
	WORD flag;
	int changed;
} Vertex;

typedef struct
{
	BYTE v2, v1, v0, flag;
} RSPTriangle;

typedef struct
{
	BYTE v0, v1, v2;
} Triangle;

// Rdp State
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE format, size;
	WORD width;
	DWORD address;
	WORD bpl;
} RdpTImageMode;

typedef struct
{
	BYTE	on, tile, level;
	float	scaleS, scaleT;
} RspSetTexture;

typedef struct
{
	//* Texture Tiles
	RdpTile RdpLoadedTiles[8];
	RdpTile *RdpCurrentTile;

	RspSetTexture RspTexture;

	Vertex		RspVertices[80];
	BYTE		RspTriangles[80][3];
	int			RspTriangleCount;

	//* Render Modes
	DWORD	RdpGeometryMode;
	DWORD	RdpOtherMode[2];
	DWORD	RdpCombindMode[2];
	float   RdpPrimDepth;

	//* Colors
	BYTE	RdpPrimColor[4];
	BYTE	RdpFillColor[4];
	BYTE	RdpBlendColor[4];

	//* Culling States
	DWORD	RspGeomModeZBuff;
	DWORD	RspGeomModeCullBoth;
	DWORD	RspGeomModeCullBack;
	DWORD	RspGeomModeCullFront;

	//* Texture Image Mode
	RdpTImageMode RdpTImg;
	RdpTImageMode RdpCImg;
	DWORD RdpZImgAddr;

	//* Matricies
	float	perspNorm;
	float	combinedMtx[4][4];
	float	projectionMtx[4][4];
	float	modelViewStack[18][4][4];
	BYTE	modelViewi;
	BYTE	UpdateCombineMtx;
} RdpState;

// Varibles
//////////////////////////////////////////////////////////////////////////
extern unsigned int ViScreenSize[2];
extern float ViScreenHalf[2];
extern DWORD ViHalfLine;
extern int iViewportChanged;
extern RdpState* RdpCurState;

// Functions
//////////////////////////////////////////////////////////////////////////

//* Refresh Screen
void ViRefreshScreen ( void );

//* Status Change
void ViStatusChange ( void );

//* Screen Width Change
void ViWidthChanged ( void );

//* Update Halfline
void ViUpdateHalfLine ( void );

//* Update Size
void ViUpdateSize ( void );

#endif
