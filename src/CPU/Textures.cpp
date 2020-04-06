//////////////////////////////////////////////////////////////////////////
// Rdp Textured (Taken from Gl 0.3 n64 plugin)
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Rsp Core

#include "Instructions/Instructions.h"
#include "Registers.h"
#include "Memory.h"
#include "Video.h"
#include "Textures.h"
#include "../Helper_Psp.h"
#include "../Video_Psp.h"


// Objects
//////////////////////////////////////////////////////////////////////////
DWORD* pTextureBuffer = NULL;
CachedTexture current;
extern int iTextureMode;

// Get Texture Format
//////////////////////////////////////////////////////////////////////////
typedef DWORD (*GetFormatFunc)( BYTE *src, WORD x, WORD i, BYTE palette );

const struct
{
	GetFormatFunc	GetTexelFunc;
	WORD lineShift, maxTexels;
} imageFormat[4][5] = {
	{ // 4-bit
		{ Get4b_CI, 4, 4096 }, // RGBA (Banjo-Kazooie uses this, doesn't make sense, but it works...)
		{ GetNone, 0, 0 }, // YUV
		{ Get4b_CI, 4, 4096 }, // CI
		{ Get4b_IA, 4, 8192 }, // IA
		{ Get4b_I, 4, 8192 }, // I
	},
	{ // 8-bit
		{ GetNone, 0, 0 }, // RGBA
		{ GetNone, 0, 0 }, // YUV
		{ Get8b_CI, 3, 2048 }, // CI
		{ Get8b_IA, 3, 4096 }, // IA
		{ Get8b_I, 3, 4096 }, // I
	},
	{ // 16-bit
		{ Get16b_RGBA, 2, 2048 }, // RGBA
		{ GetNone, 2, 2048 }, // YUV
		{ GetNone, 0, 0 }, // CI
		{ Get16b_IA, 2, 2048 }, // IA
		{ GetNone, 0, 0 }, // I
	},
	{ // 32-bit
		{ Get32b_RGBA, 2, 1024 }, // RGBA
		{ GetNone, 0, 0 }, // YUV
		{ GetNone, 0, 0 }, // CI
		{ GetNone, 0, 0 }, // IA
		{ GetNone, 0, 0 }, // I
	}
};

// Init Texture Cache
//////////////////////////////////////////////////////////////////////////
void TextureCache_Init()
{
	// Nada to do atm
}

int TextureCache_Verify()
{
	return 1;
}

void TextureCache_RemoveBottom()
{
}

CachedTexture *TextureCache_AddTop()
{
	return NULL;
}

void TextureCache_MoveToTop( CachedTexture *newtop )
{
}

void TextureCache_Destroy()
{
}

char* TextureCache_Load( CachedTexture *texInfo )
{
	DWORD *dest;

	//QWORD *src;
	BYTE *src;
	DWORD x, y, i, j, tx, ty, s, t, line;
	WORD mirrorSBit, maskSMask, clampSClamp;
	WORD mirrorTBit, maskTMask, clampTClamp;
	GetFormatFunc	GetTexel;

 	texInfo->textureBytes = (texInfo->realWidth * texInfo->realHeight) << 2;

	//if( pTextureBuffer )
	//	free( pTextureBuffer );

	//sceKernelDcacheWritebackAll();

	//pTextureBuffer = (DWORD*)malloc( texInfo->textureBytes );
	pTextureBuffer = N64Regs->TCache;

	GetTexel = imageFormat[texInfo->size][texInfo->format].GetTexelFunc;

	line = texInfo->line;

	if (texInfo->size == 3) //IMAGE_SIZE_32b
		line <<= 1;

	if (texInfo->maskS)
	{
		maskSMask = (1 << texInfo->maskS) - 1;
		if (texInfo->mirrorS)
			mirrorSBit = maskSMask + 1;
		else
			mirrorSBit = 0;
	}
	else
	{
		mirrorSBit = 0x0000;
		maskSMask = 0xFFFF;
	}

	if (texInfo->maskT)
	{
		maskTMask = (1 << texInfo->maskT) - 1;
		if (texInfo->mirrorT)
			mirrorTBit = maskTMask + 1;
		else
			mirrorTBit = 0;
	}
	else
	{
		mirrorTBit = 0x0000;
		maskTMask = 0xFFFF;
	}

	if (texInfo->clampS)
		clampSClamp = texInfo->lrS - texInfo->ulS;
	else
		clampSClamp = 0xFFFF;

	if (texInfo->clampT)
		clampTClamp = texInfo->lrT - texInfo->ulT;
	else
		clampTClamp = 0xFFFF;

	j = 0;
	for (y = 0; y < texInfo->realHeight; y++)
	{
		ty = min(y, clampTClamp) & maskTMask;

		if (y & mirrorTBit)
			ty ^= maskTMask;

		src = ( (BYTE *)&N64Regs->TMem[texInfo->tMem] ) + ( line * ty * 8 );

		i = (ty & 1) << 1;
		for (x = 0; x < texInfo->realWidth; x++)
		{
			tx = min(x, clampSClamp) & maskSMask;

			if (x & mirrorSBit)
				tx ^= maskSMask;

			pTextureBuffer[j++] = GetTexel( src, tx, i, texInfo->palette );
		}
	}

	//* NOTE - Disable here WTF?!?!?!
	sceKernelDcacheWritebackAll();
	sceGuTexFlush();

	return (char *)pTextureBuffer;
}
/*
DWORD TextureCache_CalculateCRC( BYTE t, WORD width, WORD height )
{
	DWORD crc;
	DWORD y, i, bpl, lineBytes, line;
	DWORD64 *src;

	src = (DWORD64*)&RDP.tMem[RSP.textureTile[t]->tMem];
	bpl = width << RSP.textureTile[t]->size >> 1;
	lineBytes = RSP.textureTile[t]->line << 3;

	line = RSP.textureTile[t]->line;
 	if (RSP.textureTile[t]->size == IMAGE_SIZE_32b)
		line <<= 1;

	crc = 0xFFFFFFFF;
 	for (y = 0; y < height; y++)
	{
		crc = CRC_Calculate( crc, src, bpl );

		src += line;
	}

   	if (RSP.textureTile[t]->format == IMAGE_FORMAT_CI)
	{
		if (RSP.textureTile[t]->size == IMAGE_SIZE_4b)
			crc = CRC_Calculate( crc, &RDP.palette[RSP.textureTile[t]->palette << 4], 64 );
		else if (RSP.textureTile[t]->size == IMAGE_SIZE_8b)
			crc = CRC_Calculate( crc, &RDP.palette[0], 1024 );
	}
	return crc;
}
*/

// Activate Texture
//////////////////////////////////////////////////////////////////////////
char* TextureCache_ActivateTexture( BYTE uTile )
{
	int i, j, k;
	DWORD crc;
//	BYTE *source = &g_pRDRam[RDP.textureImage.address];
	QWORD *src;
	WORD bpl, cacheNum, maxTexels;
	WORD tileWidth, maskWidth, loadWidth, lineWidth, clampWidth, height;
	WORD tileHeight, maskHeight, loadHeight, lineHeight, clampHeight, width;

	RdpTile *CurrentTile = &RdpCurState->RdpLoadedTiles[uTile];

	maxTexels = imageFormat[CurrentTile->size][CurrentTile->format].maxTexels;

	tileWidth = CurrentTile->lrS - CurrentTile->ulS + 1;
	tileHeight = CurrentTile->lrT - CurrentTile->ulT + 1;

	maskWidth = 1 << CurrentTile->maskS;
	maskHeight = 1 << CurrentTile->maskT;

	loadWidth = RdpCurState->RdpCurrentTile->lrS - RdpCurState->RdpCurrentTile->ulS + 1;
	loadHeight = RdpCurState->RdpCurrentTile->lrT - RdpCurState->RdpCurrentTile->ulT + 1;

	lineWidth = CurrentTile->line << imageFormat[CurrentTile->size][CurrentTile->format].lineShift;

	if (lineWidth) // Don't allow division by zero
		lineHeight = min( min( maxTexels / lineWidth, tileHeight ), maskHeight );
	else
		lineHeight = 0;

	if (CurrentTile->maskS && ((maskWidth * maskHeight) <= maxTexels))
		width = maskWidth; // Use mask width if set and valid
	else if ((tileWidth * tileHeight) <= maxTexels)
		width = tileWidth; // else use tile width if valid
	else if (RdpCurState->RdpCurrentTile->loadType == 1 ) //LOADTYPE_TILE
		width = loadWidth; // else use load width if load done with LoadTile
	else
		width = lineWidth; // else use line-based width

	if (CurrentTile->maskT && ((maskWidth * maskHeight) <= maxTexels))
		height = maskHeight;
	else if ((tileWidth * tileHeight) <= maxTexels)
		height = tileHeight;
	else if (RdpCurState->RdpCurrentTile->loadType == 1 ) //LOADTYPE_TILE
		height = loadHeight;
	else
		height = lineHeight;

	clampWidth = CurrentTile->clampS ? tileWidth : width;
	clampHeight = CurrentTile->clampT ? tileHeight : height;

	current.address = RdpCurState->RdpTImg.address;

	current.format = CurrentTile->format;
	current.size = CurrentTile->size;

	current.width = width;
	current.height = height;

	current.clampWidth = clampWidth;
	current.clampHeight = clampHeight;

	current.palette = CurrentTile->palette;
	current.fulS = CurrentTile->fulS;
	current.fulT = CurrentTile->fulT;
	current.ulS = CurrentTile->ulS;
	current.ulT = CurrentTile->ulT;
	current.lrS = CurrentTile->lrS;
	current.lrT = CurrentTile->lrT;
	current.maskS = CurrentTile->maskS;
	current.maskT = CurrentTile->maskT;
	current.mirrorS = CurrentTile->mirrorS;
	current.mirrorT = CurrentTile->mirrorT;
 	current.clampS = CurrentTile->clampS;
	current.clampT = CurrentTile->clampT;
	current.line = CurrentTile->line;
	current.tMem = CurrentTile->tMem;

	if (current.clampS)
		current.realWidth = pow2( clampWidth );
	else if (current.mirrorS)
		current.realWidth = maskWidth << 1;
	else
		current.realWidth = pow2( width );

	if (current.clampT)
		current.realHeight = pow2( clampHeight );
	else if (current.mirrorT)
		current.realHeight = maskHeight << 1;
	else
		current.realHeight = pow2( height );

	current.scaleS = 1.0f / (float)current.realWidth;
	current.scaleT = 1.0f / (float)current.realHeight;

	current.offsetScaleS = current.scaleS;
	current.offsetScaleT = current.scaleT;

	if (CurrentTile->shiftS > 10)
		current.scaleS *= (float)(1 << (16 - CurrentTile->shiftS));
	else if (CurrentTile->shiftS > 0)
		current.scaleS /= (float)(1 << CurrentTile->shiftS);

	if (CurrentTile->shiftT > 10)
		current.scaleT *= (float)(1 << (16 - CurrentTile->shiftT));
	else if (CurrentTile->shiftT > 0)
		current.scaleT /= (float)(1 << CurrentTile->shiftT);

	TextureCache_Load( &current );

	sceGuEnable( GU_TEXTURE_2D );
	sceGuTexMode( GU_PSM_8888, 0, 0, 0 );
	sceGuTexImage( 0, current.realWidth, current.realHeight, current.realWidth, (char *)pTextureBuffer );
	sceGuTexFunc( iTextureMode, GU_TCC_RGBA );
	sceGuTexScale( 1.0f, 1.0f );
	sceGuTexOffset( 0.0f, 0.0f );
	sceGuTexWrap( current.clampS ? GU_CLAMP : GU_REPEAT, current.clampT ? GU_CLAMP : GU_REPEAT );
	sceGuTexFilter( GU_LINEAR, GU_LINEAR );

	return NULL;
}

/*
void TextureCache_ActivateNoise( BYTE t )
{
	if (OGL.ARB_multitexture)
		glActiveTextureARB( GL_TEXTURE0_ARB + t );

	glBindTexture( GL_TEXTURE_2D, cache.glNoiseNames[RSP.DList & 0x1F] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}
*/
