//////////////////////////////////////////////////////////////////////////
// Video Rendering
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Rendering to the PSP

#ifndef _VIDEO_PSP_H_
#define _VIDEO_PSP_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "StdInc.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
extern int iTextureMode;

// Functions
//////////////////////////////////////////////////////////////////////////

//* Render Display
void RenderFrame ( void );

//* Render Framerate
void RenderFrameRate ( void );

//* Check Input
void CheckInput ( void );

//* Clear Display
void ClearDisplay ( void );

//* Reset Display
void ResetDisplay ( void );

//* Exception Handler
void ExceptionDisplay ( PspDebugRegBlock *regs );

//* Snag Vram
void SystemSaveVramToBitmap ( char* szImage, char* pVramAddr );
void SystemSaveImgToBitmap( char* szImageName, char* pImageAddr, unsigned int iWidth, unsigned int iHeight );

#endif
