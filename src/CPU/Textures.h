//////////////////////////////////////////////////////////////////////////
// Rdp Textured (Taken from Gl 0.3 n64 plugin)
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Rsp Core

#ifndef TEXTURES_H
#define TEXTURES_H

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"
#include "Convert.h"

struct CachedTexture
{
	DWORD	address;
	float	fulS, fulT;
	WORD	ulS, ulT, lrS, lrT;
	BYTE	maskS, maskT;
	int		clampS, clampT;
	int		mirrorS, mirrorT;
	WORD	line;
	WORD	size;
	WORD	format;
	WORD	tMem;
	BYTE	palette;
	WORD	width, height;			// N64 width and height
	WORD	clampWidth, clampHeight;// Size to clamp to
	WORD	realWidth, realHeight;	// Actual texture size
	float	scaleS, scaleT;			// Scale to map to 0.0-1.0 and scale from shift
	float	offsetScaleS, offsetScaleT;// Scale to map to 0.0-1.0
	DWORD	textureBytes;
};

// 
//////////////////////////////////////////////////////////////////////////
extern CachedTexture current;
extern WORD SwapWord( WORD Value );

inline DWORD GetNone( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return 0x00000000;
}

inline DWORD Get4b_CI( BYTE *src, WORD x, WORD i, BYTE palette )
{
	BYTE color4B;

	color4B = ((BYTE*)src)[(x>>1)^(i<<1)];

	if (x & 1)
		return N64Regs->TMemPal[(palette << 4) + (color4B & 0x0F)];
	else
		return N64Regs->TMemPal[(palette << 4) + ((color4B >> 4) & 0x0F)];
}

inline DWORD Get4b_IA( BYTE *src, WORD x, WORD i, BYTE palette )
{
	BYTE color4B;

	color4B = ((BYTE*)src)[(x>>1)^(i<<1)];

	return IA31_RGBA8888( (x & 1) ? (color4B & 0x0F) : (color4B >> 4) );
}

inline DWORD Get4b_I( BYTE *src, WORD x, WORD i, BYTE palette )
{
	BYTE color4B;

	color4B = ((BYTE*)src)[(x>>1)^(i<<1)];

	return I4_RGBA8888( (x & 1) ? (color4B & 0x0F) : (color4B >> 4) );
}

inline DWORD Get8b_CI( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return N64Regs->TMemPal[((BYTE*)src)[x^(i<<1)]];
}

inline DWORD Get8b_IA( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return IA44_RGBA8888(((BYTE*)src)[x^(i<<1)]);
}

inline DWORD Get8b_I( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return I8_RGBA8888(((BYTE*)src)[x^(i<<1)]);
}

inline DWORD Get16b_RGBA( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return RGBA5551_RGBA8888(((WORD*)src)[x^i]);
}

inline DWORD Get16b_IA( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return IA88_RGBA8888(((WORD*)src)[x^i]);
}

inline DWORD Get32b_RGBA( BYTE *src, WORD x, WORD i, BYTE palette )
{
	return ((DWORD*)src)[x^i];
}

inline WORD pow2( WORD dim )
{
	WORD i = 1;

	while (i < dim) i <<= 1;

	return i;
}

void TextureCache_Init();
void TextureCache_Destroy();
char* TextureCache_ActivateTexture( BYTE uTile );
int TextureCache_Verify();

#endif
