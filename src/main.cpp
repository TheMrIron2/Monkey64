/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */

#include "Main_PSP.h"
#include "Video_PSP.h"
#include "Menu_PSP.h"
#include "CPU/Instructions/Instructions.h"

PSP_MODULE_INFO("Monkey64", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU); // THREAD_ATTR_USER

static unsigned int __attribute__((aligned(16))) list[262144];
unsigned int __attribute__((aligned(16))) iPSPFrameBuffer = 0;

void DisableFPUExceptions( void );

int main(int argc, char* argv[])
{
	DisableFPUExceptions();

	SetupCallbacks();

	// setup GU
    sceGuInit();
    
    sceGuStart(GU_DIRECT,list);
    sceGuDrawBuffer(GU_PSM_8888,(void*)FRAME_SIZE,BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0,BUF_WIDTH);
    sceGuDepthBuffer((void*)(FRAME_SIZE * 2),BUF_WIDTH);
    sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
    sceGuViewport(2048,2048,320,240);
    sceGuDepthRange(65535,0);
	//sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuScissor(80,16,400,256);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	pspDebugScreenInit();

	scePowerSetClockFrequency( 333, 333, 166 );

	int iCurrentMode = 1;

	while( iCurrentMode > 0 )
	{
		//* Clear Screen
		ClearDisplay();

		//* Main Menu
		if( iCurrentMode == 1 )
			iCurrentMode = MainMenu();

		//* Emulate n64
		if( iCurrentMode == 2 )
		{
			ClearDisplay();
			//SetupDispList( 1 );
			iCurrentMode = RunCPU();
			//SetupDispList( 2 );

			if( iPSPFrameBuffer )
			{
				sceGuSwapBuffers();
				iPSPFrameBuffer ^= 1;
			}
		}
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}

// Setup Display List
//////////////////////////////////////////////////////////////////////////
void SetupDispList( int iMode )
{
	//* Start Frame
	if( iMode & 1 )
	{
		sceGuStart(GU_DIRECT,list);

		//* Set Orthogonal Matrix
		//* Note Y is inverted
		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		//sceGumOrtho( -1.0f, 1.0f,-1.0f, 1.0f, -1.0f, 1.0f );

		//* Push Identity into view & model matrix.
		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();

		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
	}

	//* End Frame
	if( iMode & 2 )
	{
		//* Fush display list
		sceGuFinish();
		sceGuSync(0,0);

		//sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
		iPSPFrameBuffer ^= 1;
	}
}

// Change Viewport & Clipping
//////////////////////////////////////////////////////////////////////////
void UpdateViewport( int iWidth, int iHeight )
{
	//* Calc Offset
	int iHalfWidth = iWidth / 2;
	int iHalfHeight = iHeight / 2;

	int iOffsetX = 240 - iHalfWidth;
	int iOffsetY = 136 - iHalfHeight;

	if( iOffsetX < 0 ) iOffsetX = 0;
	if( iOffsetY < 0 ) iOffsetY = 0;

	//* Start Disp List
	sceGuStart( GU_DIRECT, list );

	//* Setup Clipping & Viewport
	sceGuViewport( 2048, 2048, iWidth, iHeight );
	sceGuScissor( iOffsetX, iOffsetY, iOffsetX + iWidth, iOffsetY + iHeight );

	//* Finish List
	sceGuFinish();
	sceGuSync( 0, 0 );
}

// Exit Callback (Home Button)
//////////////////////////////////////////////////////////////////////////
int exit_callback( int arg1, int arg2, void *common )
{
	scePowerSetClockFrequency( 222, 222, 111 );
	sceKernelExitGame();
	return 0;
}

// Callback Thread
//////////////////////////////////////////////////////////////////////////
int CallbackThread( SceSize args, void *argp )
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

// Sets up the callback thread and returns its thread id
//////////////////////////////////////////////////////////////////////////
int SetupCallbacks( void )
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

// Disable Float Exceptions
//////////////////////////////////////////////////////////////////////////
void DisableFPUExceptions( void )
{
	__asm__ volatile ( 
		"cfc1    $2, $31\n" 
		"lui     $8, 0x80\n" 
		"and     $8, $2, $8\n"
		"ctc1    $8, $31\n"  );
}
