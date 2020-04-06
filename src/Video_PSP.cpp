//////////////////////////////////////////////////////////////////////////
// Video Rendering
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Rendering to the PSP

#include "Main_PSP.h"
#include "Video_PSP.h"
#include "Helper_PSP.h"
#include "CPU/Registers.h"
#include "CPU/Memory.h"
#include "CPU/Exceptions.h"
#include "CPU/Video.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
extern int iEmulatorRunning;
extern char szRomFile[256];
extern int iPauseOnExit;
extern int iPrintVerts;
extern int iCullMode;
extern int iBufferReady;

unsigned int __attribute__((aligned(16))) iCopyCfbToDisp = 1;
int __attribute__((aligned(16))) iTextureMode = GU_TFX_MODULATE;
ScePspFVector3 fZoom = { 0.0f, 0.0f, 0.0f }; //-1.5f
int iScreenShotCount = 0;

// Draw Framebuffer
//////////////////////////////////////////////////////////////////////////
void DrawFramebuffer ( void )
{
	int *buf32 = (int*)( (char *)sceGeEdramGetAddr() ); //+ ( iPSPFrameBuffer * FRAME_SIZE ) );
	int i, j;

	WORD uColor = 0;
	char* pSrc = (char *)g_pRDRam + ( N64Regs->VI[1] & 0x7FFFFF ); //LoadDWord( 0x04400004 )
	char* uColorPtr = (char*)&uColor;

	u8 uR, uG, uB;
	int iOffset;

	buf32 += (16 * 512) + 80;

	//* To speed things up
	if( ViScreenSize[0] == 320 )
	{
		//* 320x240 Res
		for( j = 0; j < 240; j++ )
		{
			for( i = 0; i < 320; i++ )
			{
				iOffset = ( j * 640 ) + ( i * 2 );

				uColorPtr[0] = pSrc[ ( iOffset     ) ^ 2 ];
				uColorPtr[1] = pSrc[ ( iOffset + 1 ) ^ 2 ];

				uR = ( ( uColor >>  1 ) & 0x1f ) << 3;
				uG = ( ( uColor >>  6 ) & 0x1f ) << 3;
				uB = ( ( uColor >> 11 ) & 0x1f ) << 3;

				buf32[j*512+i] = uB + ( uG << 8 ) + ( uR << 16 );
			}
		}
	}
	else
	{
		//* Any other Res
		float fScreenWDouble = (float)ViScreenSize[0] * 2.0f;
		float fHeightInc = ViScreenSize[0] / 240;
		float fWidthInc = ViScreenSize[0] / 320;

		float fi; //* floating point i;
		float fj; //* floating point j;

		//* Unknown resolution
		for( fj = 0, j = 0; fj < (float)ViScreenSize[1]; fj += fHeightInc )
		{
			for( fi = 0, i = 0; fi < (float)ViScreenSize[0]; fi += fWidthInc )
			{
				iOffset = (int)( ( fj * fScreenWDouble ) + ( fi * 2.0f ) );

				uColorPtr[0] = pSrc[ iOffset ];
				uColorPtr[1] = pSrc[ iOffset + 1 ];

				uR = ( ( uColor >>  1 ) & 0x1f ) << 3;
				uG = ( ( uColor >>  6 ) & 0x1f ) << 3;
				uB = ( ( uColor >> 11 ) & 0x1f ) << 3;

				buf32[j*512+i] = uB + ( uG << 8 ) + ( uR << 16 );
				i++;
			}

			j++;
		}
	}

	//* Flip Surface
}

// Render Frame
//////////////////////////////////////////////////////////////////////////
void RenderFrame ( void )
{
	//sceDisplayWaitVblankStart();

	if( N64Regs->DlistCount == 0 )
		DrawFramebuffer();

	RenderFrameRate();
//	printf( "PC: 0x%08x HLE D:%d A:%d", (unsigned int)N64Regs->PC, N64Regs->DlistCount, N64Regs->AlistCount );
	
	CheckInput();
}

// Render Framerate
//////////////////////////////////////////////////////////////////////////
void RenderFrameRate ( void )
{
	static int val = 0;
	static float curr_ms = 1.0f;
	static struct timeval time_slices[16];
	float curr_fps = 1.0f / curr_ms;

	pspDebugScreenSetXY(0,256/8);
	pspDebugScreenPrintf("VSYNC: %d.%03d  \n",(int)curr_fps,(int)((curr_fps-(int)curr_fps) * 1000.0f));
	gettimeofday(&time_slices[val & 15],0);
	val++;

	if (!(val & 15))
	{
		struct timeval last_time = time_slices[0];
		unsigned int i;

		curr_ms = 0;
		for (i = 1; i < 16; ++i)
		{
			struct timeval curr_time = time_slices[i];

			if (last_time.tv_usec > curr_time.tv_usec)
			{
				curr_time.tv_sec++;
				curr_time.tv_usec-=1000000;
			}

			curr_ms += ((curr_time.tv_usec-last_time.tv_usec) + (curr_time.tv_sec-last_time.tv_sec) * 1000000) * (1.0f/1000000.0f);

			last_time = time_slices[i];
		}
		curr_ms /= 15.0f;
	}
}

// Check input for break
//////////////////////////////////////////////////////////////////////////
void CheckInput( void )
{
	static SceCtrlData PadInput;
	static unsigned int iButtonPrev = 0;
	static unsigned int iButtonTrigger = 0;

	iButtonPrev = PadInput.Buttons;
	sceCtrlPeekBufferPositive( &PadInput, 1 );
	iButtonTrigger = PadInput.Buttons & (~iButtonPrev);

//	if( iButtonTrigger & PSP_CTRL_UP )
//	if( iButtonTrigger & PSP_CTRL_DOWN )


	if( PadInput.Buttons & PSP_CTRL_LTRIGGER )
	{
		if( iButtonTrigger & PSP_CTRL_SELECT )
		{
			char szScreenShot[280] = "";

			sprintf( szScreenShot, "./%s_%02d.bmp", szRomFile, iScreenShotCount );

			iScreenShotCount++;
			
			SystemSaveVramToBitmap( szScreenShot, (char *)sceGeEdramGetAddr() );
		}
	}
	else
	{
		if( iButtonTrigger & PSP_CTRL_SELECT )
		{
			iEmulatorRunning = 0;
			iPauseOnExit = 0;
		}
	}
}

// Clear Display
//////////////////////////////////////////////////////////////////////////
void ClearDisplay ( void )
{
	memset( sceGeEdramGetAddr(), 0, 512*272*4 );
	pspDebugScreenClear();
	pspDebugScreenSetXY(0,0);
}

// Reset Display
//////////////////////////////////////////////////////////////////////////
void ResetDisplay ( void )
{
	ClearDisplay();

	ViScreenSize[0] = 320;
	ViScreenSize[1] = 240;
	iViewportChanged = 0;

	iCopyCfbToDisp = 1;

	//if( iPSPFrameBuffer == 0 )
	//{
	//	sceGuSwapBuffers();
	//	iPSPFrameBuffer = 0;
	//}
}

// Exception Handler
//////////////////////////////////////////////////////////////////////////
void ExceptionDisplay(PspDebugRegBlock *regs)
{
#if 0
	/* Do normal initial dump, setup screen etc */
	pspDebugScreenInit();

	/* I always felt BSODs were more interesting that white on black */
	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	pspDebugScreenPrintf("Exception Details:\n");
	pspDebugDumpException(regs);
#endif
}

// Save Vram to Bitmap
//////////////////////////////////////////////////////////////////////////
// originally char -iron
int pBmpHeader[0x36] = {
    0x42, 0x4D, 0x38, 0xFA, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
    0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC3, 0x0E, 0x00, 0x00, 0xC3, 0x0E, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

void SystemSaveVramToBitmap( char* szImage, char* pVramAddr )
{
	char* uBGBuffer;
	int y, x;
	int iFileID;

	//* Allocate Buffer
	uBGBuffer = (char *)malloc( 480*272*3 );

	//* Flip for Bmp
	for( y = 0; y < 272; y++ )
	{
		for( x = 0; x < 480; x++ )
		{
			unsigned char uR = pVramAddr[(((y * 512) + x) * 4) + 0];
			unsigned char uG = pVramAddr[(((y * 512) + x) * 4) + 1];
			unsigned char uB = pVramAddr[(((y * 512) + x) * 4) + 2];

			uBGBuffer[((((271 - y) * 480) + x) * 3) + 2] = uR;
			uBGBuffer[((((271 - y) * 480) + x) * 3) + 1] = uG;
			uBGBuffer[((((271 - y) * 480) + x) * 3) + 0] = uB;
		}
	}

	//* Open File
	iFileID = sceIoOpen( szImage, PSP_O_WRONLY|PSP_O_CREAT, 0777 );

	//* Validate
	if( iFileID < 0 )
		return;
	
	//* Write Header
	sceIoWrite( iFileID, pBmpHeader, 0x36 );

	//* Write Bmp
	sceIoWrite( iFileID, uBGBuffer, 480*272*3 );

	//* Close File
	sceIoClose( iFileID );

	//* Free Buffer
	free( uBGBuffer );
}

void SystemSaveImgToBitmap( char* szImageName, char* pImageAddr, unsigned int iWidth, unsigned int iHeight )
{
	char* uBGBuffer;
	int y, x;
	int iFileID;
	unsigned int uGetSize;

	//* Allocate Buffer
	uBGBuffer = (char *)malloc( iWidth*iHeight*3 );

	//* Flip for Bmp
	for( y = 0; y < iHeight; y++ )
	{
		for( x = 0; x < iWidth; x++ )
		{
			unsigned char uR = pImageAddr[(((y * iWidth) + x) * 4) + 0];
			unsigned char uG = pImageAddr[(((y * iWidth) + x) * 4) + 1];
			unsigned char uB = pImageAddr[(((y * iWidth) + x) * 4) + 2];

			uBGBuffer[((((271 - y) * iWidth) + x) * 3) + 2] = uR;
			uBGBuffer[((((271 - y) * iWidth) + x) * 3) + 1] = uG;
			uBGBuffer[((((271 - y) * iWidth) + x) * 3) + 0] = uB;
		}
	}

	//* Open File
	iFileID = sceIoOpen( szImageName, PSP_O_WRONLY|PSP_O_CREAT, 0777 );

	//* Validate
	if( iFileID < 0 )
		return;
	
	//* Write Header
	sceIoWrite( iFileID, pBmpHeader, 0x36 );

	//* Write Bmp
	sceIoWrite( iFileID, uBGBuffer, iWidth*iHeight*3 );

	//* Get Size
	uGetSize = 0x36 + (iWidth*iHeight*3);

	//* Seek Size Location (in header)
	sceIoLseek( iFileID, 2, SEEK_SET );

	//* Write Size
	sceIoWrite( iFileID, &uGetSize, 4 );

	//* Seek width location (in header)
	sceIoLseek( iFileID, 12, SEEK_CUR );

	//* Write Width & Height
	sceIoWrite( iFileID, &iWidth, 4 );
	sceIoWrite( iFileID, &iHeight, 4 );

	//* Close File
	sceIoClose( iFileID );

	//* Free Buffer
	free( uBGBuffer );
}
