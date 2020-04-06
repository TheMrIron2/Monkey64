//////////////////////////////////////////////////////////////////////////
// Main Menu
//////////////////////////////////////////////////////////////////////////
// Main Menu

#include "Main_Psp.h"
#include "Menu_Psp.h"
#include "Rom_Psp.h"
#include "Video_Psp.h"
#include "Helper_Psp.h"
#include "CPU/Registers.h"
#include "CPU/Memory.h"

// Global Varibles
//////////////////////////////////////////////////////////////////////////
char __attribute__((aligned(16))) szRomFile[256] = "";
//float fAxisInvert[3] = { 1.0f, 1.0f, -1.0f };
int iCullMode = 2;
int iViewPortMode = 0;
extern int iPrintVerts;
extern int iRspMicrocode;
extern int iScreenShotCount;

// Menu
//////////////////////////////////////////////////////////////////////////
int MainMenu( void )
{
	static int iRomSelected = 0;
	static int iRomLoaded = 0;
	static int iCpuSpeed = 1;
	static int iBitSelect = 0;

	SceCtrlData PadInput;
	unsigned int iButtonPrev = 0;
	unsigned int iButtonTrigger = 0;

	SceIoDirent RomList[256];
	int iRomCount = 256;
	int iMenuSelect = 0;
	int i;

	//* Clear List
	memset( &RomList, 0, sizeof( SceIoDirent ) * iRomCount );

	//* Build Map List
	FindFiles( &RomList[0], "ms0:/n64/", &iRomCount, FIO_SO_IFREG );

	//* Loop
	while( 1 )
	{
		//* Clear Screen
		pspDebugScreenSetXY( 0,0 );

		//* Update Input
		iButtonPrev = PadInput.Buttons;
		sceCtrlReadBufferPositive( &PadInput, 1 );
		iButtonTrigger = PadInput.Buttons & (~iButtonPrev);

		if( ( iButtonTrigger & PSP_CTRL_UP ) && ( iMenuSelect > 0 ) )
			iMenuSelect --;

		if( (iButtonTrigger & PSP_CTRL_DOWN ) && ( iMenuSelect < enumMenuItem_Max - 1 ) )
			iMenuSelect ++;

		//* Process Input
		switch( iMenuSelect )
		{
			//////////////////////////////////////////////////////////////////////////
			//* Emulation Setup
			case enumMenuItem_LoadRom:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) && ( iRomSelected > 0 ) )
					iRomSelected --;

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) && ( iRomSelected < iRomCount - 1 ) )
					iRomSelected ++;

				if( ( PadInput.Buttons & PSP_CTRL_LTRIGGER ) && ( iRomSelected > 0 ) )
					iRomSelected --;

				if( ( PadInput.Buttons & PSP_CTRL_RTRIGGER ) && ( iRomSelected < iRomCount - 1 ) )
					iRomSelected ++;

				if( ( iButtonTrigger & PSP_CTRL_CROSS ) && ( iRomCount > 0 ) )
				{
					char szRomPath[256] = "";

					strcpy( szRomPath, "ms0:/n64/" );
					strcat( szRomPath, RomList[iRomSelected].d_name );

					//* Load Rom
					iRomLoaded = LoadRom( szRomPath );
					
					sceKernelDcacheWritebackAll();
					sceDisplayWaitVblankStart();

					if( !InitRegisters() )
					{
						iRomLoaded = 0;
						FreeAllocatedMemory( 1 );
					}

					sceKernelDcacheWritebackAll();
					sceDisplayWaitVblankStart();

					if( iRomLoaded )
					{
						ResetDisplay();
						strcpy( szRomFile, RomList[iRomSelected].d_name );
						iScreenShotCount = 0;
						return 2;
					}
					else
					{
						memset( szRomFile, 0, 256 );
					}
				}
			}
			break;

			case enumMenuItem_ResetEmulation:
			{
				if( ( iButtonTrigger & PSP_CTRL_CROSS ) && iRomLoaded )
				{
					FreeAllocatedMemory( 0 );
					ResetDisplay();
					if( InitRegisters() )
						return 2;
				}
			}
			break;

			case enumMenuItem_RunEmulation:
			{
				if( ( iButtonTrigger & PSP_CTRL_CROSS ) && iRomLoaded )
				{
					pspDebugScreenClear();
					return 2;
				}
			}
			break;

			//////////////////////////////////////////////////////////////////////////
			//* Emulation Properties

			case enumMenuItem_ClockSpeed:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) )
				{
					scePowerSetClockFrequency( 222, 222, 111 );
					iCpuSpeed = 0;
				}

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) )
				{
					scePowerSetClockFrequency( 333, 333, 166 );
					iCpuSpeed = 1;
				}
			}
			break;

			case enumMenuItem_Tlb:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) )
					g_iUseFastTlb = 0; //iTextureMode = GU_TFX_ADD;

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) )
					g_iUseFastTlb = 1; //iTextureMode = GU_TFX_MODULATE;
			}
			break;

			case enumMenuItem_MicroCode:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) && ( iRspMicrocode > 0 ) )
					iRspMicrocode--;

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) && ( iRspMicrocode < 3 ) )
					iRspMicrocode++;
			}
			break;

			case enumMenuItem_ScreenSize:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) && ( iViewPortMode > 0 ) )
					iViewPortMode--;

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) && ( iViewPortMode < 2 ) )
					iViewPortMode++;

				switch( iViewPortMode )
				{
					case 0: UpdateViewport( 320, 240 ); break;
					case 1: UpdateViewport( 362, 272 ); break;
					case 2: UpdateViewport( 480, 272 ); break;
				}
			}
			break;

/*
			case enumMenuItem_AxisDir:
			{
				if( ( iButtonTrigger & PSP_CTRL_SQUARE ) )
					fAxisInvert[0] = -1.0f;

				if( ( iButtonTrigger & PSP_CTRL_CIRCLE ) )
					fAxisInvert[0] = 1.0f;

				if( ( iButtonTrigger & PSP_CTRL_CROSS ) )
					fAxisInvert[1] = -1.0f;

				if( ( iButtonTrigger & PSP_CTRL_TRIANGLE ) )
					fAxisInvert[1] = 1.0f;

				if( ( iButtonTrigger & PSP_CTRL_LTRIGGER ) )
					fAxisInvert[2] = -1.0f;

				if( ( iButtonTrigger & PSP_CTRL_RTRIGGER ) )
					fAxisInvert[2] = 1.0f;
				
			}
			break;
*/
			//////////////////////////////////////////////////////////////////////////
			//* Info / Exit

			case enumMenuItem_About:
			{
				if( iButtonTrigger & PSP_CTRL_CROSS )
				{
					pspDebugScreenClear();
					PrintCredits();
				}
			}
			break;

			case enumMenuItem_Exit:
			{
				if( ( iButtonTrigger & PSP_CTRL_CROSS ) )
				{
//					scePowerSetClockFrequency( 222, 222, 111 );
//					sceKernelExitGame();
					return 0;
				}
			}
			break;

			//////////////////////////////////////////////////////////////////////////
			//* Debug Parts
#ifdef DEBUGER_ENABLED
			case enumMenuItem_DebugBreakAddr:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) )
				{
					if( iBitSelect == 0 )
						BreakAddress -= 4;
					else
					{
						int iMult = 16;
						int k;

						for( k = 1; k < iBitSelect; k++ )
							iMult = iMult * 16;

						BreakAddress -= iMult;
					}
				}

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) )
				{
					if( iBitSelect == 0 )
						BreakAddress += 4;
					else
					{
						int iMult = 16;
						int k;

						for( k = 1; k < iBitSelect; k++ )
							iMult = iMult * 16;

						BreakAddress += iMult;
					}
				}

				if( ( iButtonTrigger & PSP_CTRL_LTRIGGER ) && ( iBitSelect < 7 ) )
					iBitSelect += 1;

				if( ( iButtonTrigger & PSP_CTRL_RTRIGGER ) && ( iBitSelect > 0 ) )
					iBitSelect -= 1;
			}
			break;

			case enumMenuItem_DebugWaitInput:
			{
				if( ( iButtonTrigger & PSP_CTRL_LEFT ) )
				{
					//uRenderWidthFixed = 0;
					iWaitForInputTrig = 0;
				}

				if( ( iButtonTrigger & PSP_CTRL_RIGHT ) )
				{
					//uRenderWidthFixed = 1;
					iWaitForInputTrig = 1;
				}
			}
			break;
#endif

		}

		//* Print Header
		printf( "Monkey 64: v%s by PSmonkey\n", Monkey64_Version );

		if( iRomLoaded )
			printf( "Rom Loaded: %s\n\n", szRomFile );
		else
			printf( "Rom Loaded: No Rom Loaded\n\n" );


		//* Display Menu
		for( i = 0; i < enumMenuItem_Max; i++ )
		{
			//* Print Selection
			if( i == iMenuSelect )
				printf( "(O) ");
			else
				printf( "( ) ");

			//* Display Individual Items
			switch( i )
			{
				//////////////////////////////////////////////////////////////////////////
				//* Emulation Setup

				case enumMenuItem_LoadRom:
					printf( "Load Rom -> ");
					if( iRomCount > 0 )
						printf( "%s\n", RomList[iRomSelected].d_name );
					else
						printf( "No Roms Found in ms0:/n64/\n" );
					break;

				case enumMenuItem_ResetEmulation:
					printf( "Reset Emulation\n" );
					break;

				case enumMenuItem_RunEmulation:
					printf( "Run Emulation\n" );
					break;

				//////////////////////////////////////////////////////////////////////////
				//* Emulation Properties

				case enumMenuItem_ClockSpeed:
					if( iCpuSpeed == 0 )
						printf( "Clock Speed: 222/222/111\n" );
					else
						printf( "Clock Speed: 333/333/166\n" );
					break;

				case enumMenuItem_Tlb:
					printf( "Tlb Fast Emulation: %s\n", g_iUseFastTlb ? "Enabled" : "Disabled" );
					break;

				case enumMenuItem_MicroCode:
					printf( "Microcode: " );
					switch( iRspMicrocode )
					{
						case 0: printf( "Fast3D\n" );		break;
						case 1: printf( "Fast3DExt\n" );	break;
						case 2: printf( "F3D3X\n" );		break;
						case 3: printf( "F3D3X2\n" );		break;
						default: printf( "Unsuported\n" );	break;
					}
					break;

				case enumMenuItem_ScreenSize:
					printf( "ScreenSize: " );
					switch( iViewPortMode )
					{
						case 0: printf( "320x240\n" ); break;
						case 1: printf( "362x272\n" ); break;
						case 2: printf( "480x272\n" ); break;
					}
					break;

//				case enumMenuItem_AxisDir:
//					printf( "Axis: %s,%s,%s\n",
//						fAxisInvert[0] == 1.0f ? " x" : "-x",
//						fAxisInvert[1] == 1.0f ? " y" : "-y",
//						fAxisInvert[2] == 1.0f ? " z" : "-z" );
//					break;
					

				//////////////////////////////////////////////////////////////////////////
				//* Info / Exit

				case enumMenuItem_About:
					printf( "About / Info\n" );
					break;

				case enumMenuItem_Exit:
					printf( "Exit Emu\n" );
					break;

				//////////////////////////////////////////////////////////////////////////
				//* Debug Parts
#ifdef DEBUGER_ENABLED
				case enumMenuItem_DebugBreakAddr:
					printf( "Break Addr: 0x%08X\n", (unsigned int)BreakAddress );
					break;

				case enumMenuItem_DebugWaitInput:
					if( iWaitForInputTrig == 1 )
						printf( "Break Wait: Wait for Input\n" );
					else
						printf( "Break Wait: No Wait\n" );
					break;
#endif
			}
		}

		//* Print Input
		pspDebugScreenSetXY( 0, 260/8 );
		printf( "Left/Right - Change Value, Up/Down - Change Selection, X - Select" );

		sceDisplayWaitVblankStart();
	}

	return 0;
}

// Print Credits
//////////////////////////////////////////////////////////////////////////
void PrintCredits( void )
{
	pspDebugScreenClear();
	printf( "// Monkey 64: v%s by PSmonkey\n", Monkey64_Version );
	printf( "////////////////////////////////////////////////////////////////////\n" );
	printf( " - N64 emulator for PSP -\n" );
	printf( "http://nemo.dcemu.co.uk\n" );
	printf( "\n" );
	printf( "Programmer\n PSMonkey\n\n" );
	printf( "Testers\n $n!pR, Wally, Evab3va & Psyco\n\n" );
	printf( "Special Thanks\n Zilmar & PJ64 Team, Lac, Strmnnrmn, GPF, Wraggster & Dcemu crew,\n Pochi & Clessy\n\n" );
	printf( "Extra Special thanks to my Girlfriend who lets me stay up at\n night to work on this emulatior\n\n\n" );

	printf( "Want to join a bunch of crazy people? Come join us at the PSMonkey's pen forum at http://nemo.dcemu.co.uk\n" );

	printf( "\n\nPress X to Continue\n" );
	WaitForInput( PSP_CTRL_CROSS );
	ResetDisplay();
}
