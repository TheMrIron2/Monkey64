//////////////////////////////////////////////////////////////////////////
// Main App
//////////////////////////////////////////////////////////////////////////
// The Main App for Monkey64
// For Map File "LDFLAGS = -Wl,-Map,Monkey.map"

#include "Main_Psp.h"
//#include "Video_Psp.h"
//#include "Menu_Psp.h"
//#include "Cpu/Instructions/Instructions.h"

PSP_MODULE_INFO("Monkey 64", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

// Static Varibles
//////////////////////////////////////////////////////////////////////////
static unsigned int __attribute__((aligned(16))) list[262144];

int iTextureMode = GU_TFX_ADD;

typedef struct 
{
	short u, v;
	unsigned int color;
	float x, y, z;
} PspVertex;

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

// Main Loop
//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	int iCurrentMode = 1;

	//* Init System
	//SetupCallbacks();
	
	InitDisplay();
	InitInput();

	pspDebugScreenInit();
//	pspDebugInstallErrorHandler( ExceptionDisplay );

	//* Test
	while( 1 )
	{
		sceDisplayWaitVblankStart();

		//* Start
		SetupDispList( 1 );

		//* Finish
		SetupDispList( 2 );
	}

	//* Start Main Loop
/*
	while( iCurrentMode > 0 )
	{
		//* Clear Screen
		ResetDisplay();

		//* Main Menu
		if( iCurrentMode == 1 )
			iCurrentMode = MainMenu();

		//* Emulate n64
		if( iCurrentMode == 2 )
			iCurrentMode = RunCpu();
	}
*/
	scePowerSetClockFrequency( 222, 222, 111 );
	sceKernelExitGame();

	return 0;
}

// Init Display
//////////////////////////////////////////////////////////////////////////
void InitDisplay( void )
{
	sceGuInit();

	sceGuStart( GU_DIRECT, list );

	sceGuDrawBuffer( GU_PSM_8888, (void*)0, BUF_WIDTH );
	sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, (void*)0, BUF_WIDTH );
	sceGuDepthBuffer( (void*)(FRAME_SIZE*2), BUF_WIDTH );

	sceGuViewport( VPORT_SIZE, VPORT_SIZE, SCR_WIDTH, SCR_HEIGHT );
	sceGuOffset( VPORT_SIZE - (SCR_WIDTH/2), VPORT_SIZE - (SCR_HEIGHT/2) );

	sceGuScissor( 0, 0, SCR_WIDTH, SCR_HEIGHT );
	sceGuEnable(GU_SCISSOR_TEST);

	sceGuDepthFunc( GU_GEQUAL );
	sceGuDepthRange( 0xc350, 0x2710 );
	sceGuEnable( GU_DEPTH_TEST );

	//sceGuEnable(GU_CULL_FACE);
	//sceGuFrontFace(GU_CW);

	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);

	sceGuAmbientColor( 0xffffffff );
	sceGuTexEnvColor( 0xffffffff );

	sceGuFinish();
	sceGuSync(0,0);

	sceGuDisplay(GU_TRUE);
	sceDisplayWaitVblankStart();

	//sceGuSwapBuffers();

//	sceGuStart( GU_DIRECT, list );

//	sceGumMatrixMode(GU_PROJECTION);
//	sceGumLoadIdentity();

//	sceGumMatrixMode(GU_MODEL);
//	sceGumLoadIdentity();
}

// Setup Display List
//////////////////////////////////////////////////////////////////////////
void SetupDispList( int iMode )
{
	//* Start Frame
	if( iMode & 1 )
	{
		sceGuStart( GU_DIRECT, list );

		sceGuClearDepth( 0 );		
		sceGuClearColor( 0xff554433 );
		sceGuClear( GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT );

		sceGumMatrixMode(GU_PROJECTION);
		sceGumPerspective(45.0f,16.0f/9.0f,1.0f, 2048.0f);
		sceGumLoadIdentity();

		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();

		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();

		{
			PspVertex* pVerts = (PspVertex *)malloc( sizeof(PspVertex) * 3 );

			pVerts[0].color = 0xff0000ff;
			pVerts[0].u = 0; pVerts[0].v = 0;
			pVerts[0].x =-2; pVerts[0].y = 0; pVerts[0].z = 2.0f;

			pVerts[1].color = 0xff0000ff;
			pVerts[1].u = 0; pVerts[1].v = 0;
			pVerts[1].x = 0; pVerts[1].y = 2; pVerts[1].z = 2.0f;

			pVerts[2].color = 0xff0000ff;
			pVerts[2].u = 0; pVerts[2].v = 0;
			pVerts[2].x = 2; pVerts[2].y = 0; pVerts[2].z = 2.0f;

			sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 3, 0, pVerts );
		}
	}

	//* End Frame
	if( iMode & 2 )
	{
		sceGuFinish();
		sceGuSync(0,0);
		//sceGuSwapBuffers();
	}
}

// Init Input
//////////////////////////////////////////////////////////////////////////
void InitInput( void )
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}
