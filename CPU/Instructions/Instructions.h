//////////////////////////////////////////////////////////////////////////
// Instructions
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with instructions

#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../../StdInc.h"

// Opcode Structure (Taken from PJ64)
//////////////////////////////////////////////////////////////////////////
typedef struct {
	union {

		unsigned long Hex;
		unsigned char Ascii[4];

		struct {
			unsigned offset : 16;
			unsigned rt : 5;
			unsigned rs : 5;
			unsigned op : 6;
		};

		struct {
			unsigned immediate : 16;
			unsigned : 5;
			unsigned base : 5;
			unsigned : 6;
		};

		struct {
			unsigned target : 26;
			unsigned : 6;
		};

		struct {
			unsigned funct : 6;
			unsigned sa : 5;
			unsigned rd : 5;
			unsigned : 5;
			unsigned : 5;
			unsigned : 6;
		};

		struct {
			unsigned : 6;
			unsigned fd : 5;
			unsigned fs : 5;
			unsigned ft : 5;
			unsigned fmt : 5;
			unsigned : 6;
		};

		struct {
			signed   voffset : 7;
			unsigned del     : 4;
			unsigned : 5;
			unsigned dest    : 5;
			unsigned : 5;
			unsigned : 6;
		};
	};

} N64_OPCODE;

// Instruction Varibles
//////////////////////////////////////////////////////////////////////////
extern N64_OPCODE InstrOpcode;
extern DWORD JumpAddress;
extern int iNextJump;

// Instructions
//////////////////////////////////////////////////////////////////////////

extern void ByteSwap16( void* out, void* in );
extern void ByteSwap32( void* out, void* in );

//* Basic CPU
void PrintOpcode ( int iClose );		//* Print Executed Opcode (for debugging)
void RunCpuInstruction ( void );		//* Run A Single Cpu Instruction
int RunCpu ( void );					//* Run Cpu Emulation
void ProcPermLoop ( void );				//* Process a Permanent Loop
void Op_NI ( void );					//* Instruction: No Instruction

//* Basic RSP
void InitRsp ( void );					//* Init Rsp
void RunRsp ( void );					//* Run Rsp Emulation
//void RunRspInstruction ( void );		//* Run A Single Rsp Instruction
//void RunRspLLE ( void );				//* Run Rsp LLE
void RunRspHLE ( void );				//* Run Rsp HLE
void ProcessDList ( void );				//* Process Rsp List
void ProcessRDPList ( void );			//* Process Rdp List
void SetRspUCode( BYTE uCode );			//* Set Rsp UCode
//void RspOp_NI ( void );					//* Instruction: No Rsp Instruction
void RspHleOp_NI ( void );				//* Instruction: No Rsp Hle Instruction

//* Basic RDP
void RunRdp ( void );					//* Run Rdp Emulation
void RdpOp_NI ( void );					//* Instruction: No Rdp Instruction

//* Core
void Op_Extended ( void );	//* Instruction: Performs Cpu Extended / Extra Instruction
void Op_Regimm ( void );	//* Instruction: Performs Cpu Regimm Instruction
void Op_J ( void );			//* Instruction: Jump Instruction
void Op_JAL ( void );		//* Instruction: Jump And Link Instruction
void Op_BEQ ( void );		//* Instruction: Branch if Equal Instruction
void Op_BNE ( void );		//* Instruction: Branch if Not Equal Instruction
void Op_BGTZ ( void );		//* Instruction: Branch if Greater Than Zero
void Op_BLEZ ( void );		//* Instruction: Branch if Less than or Equal to Zero Instruction
void Op_ADDI ( void );		//* Instruction: Add Immediate Instructin
void Op_ADDIU ( void );		//* Instruction: Add Immediate Unsigned Instructin
void Op_SLTI ( void );		//* Instruction: SLT Immediate Instruction
void Op_SLTIU ( void );		//* Instruction: SLT Immediate Unsigned Instruction
void Op_ANDI ( void );		//* Instruction: And Immediate Instruction
void Op_ORI ( void );		//* Instruction: Or Immediate Instruction
void Op_XORI ( void );		//* Instruction: XOr Immediate Instruction
void Op_LUI ( void );		//* Instruction: LUI Cpu Instruction
void Op_Cop0 ( void );		//* Instruction: Perferform Cop0 Instruction
void Op_Cop1 ( void );		//* Instruction: Perferform Cop1 Instruction
void Op_BEQL ( void );		//* Instruction: Branch if Equal L Instruction
void Op_BNEL ( void );		//* Instruction: Branch if Not Equal L Instruction
void Op_BLEZL ( void );		//* Instruction: Branch if Less than or Equal to Zero L Instruction 
void Op_BGTZL ( void );		//* Instruction: Branch if Greater than Zero L Instruction
void Op_DADDIU ( void );	//* Instruction: Double Add Immediate Unsigned Instruction
void Op_LB ( void );		//* Instruction: Load Byte Cpu Instruction
void Op_LH ( void );		//* Instruction: Load Halfword Cpu Instruction
void Op_LWL ( void );		//* Instruction: Load Word Left Cpu Instruction
void Op_LW ( void );		//* Instruction: Load Word Cpu Instruction
void Op_LBU ( void );		//* Instruction: Load Byte Unsigned Cpu Instruction
void Op_LHU ( void );		//* Instruction: Load Halfword Unsigned Cpu Instruction
void Op_LWR ( void );		//* Instruction: Load Word Right Cpu Instruction
void Op_LWU ( void );		//* Instruction: Load Word Unsigned Cpu Instruction
void Op_SB ( void );		//* Instruction: Store Byte Cpu Instruction
void Op_SH ( void );		//* Instruction: Store Halfword Cpu Instruction
void Op_SWL ( void );		//* Instruction: Store Word Left Cpu Instruction
void Op_SW ( void );		//* Instruction: Store Word Cpu Instruction
void Op_SWR ( void );		//* Instruction: Store Word Right Cpu Instruction
void Op_CACHE ( void );		//* Instruction: Cache Instruction
void Op_LWC1 ( void );		//* Instruction: Load Word to Cop1 Register Instruction
void Op_LDC1 ( void );		//* Instruction: Load Double Word to Cop1 Register Instruction
void Op_LD ( void );		//* Instruction: Load Double Word Cpu Instruction
void Op_SWC1 ( void );		//* Instruction: Store Word to Cop1 Register Instruction
void Op_SDC1 ( void );		//* Instruction: Store Double Word to Cop1 Register Instruction
void Op_SD ( void );		//* Instruction: Store Double Word Cpu Instruction

//* Core Extended
void Op_SLL ( void );		//* Instruction: Shift Left L Extended Instruction
void Op_SRL ( void );		//* Instruction: Shift Right L Extended Instruction
void Op_SRA ( void );		//* Instruction: Sifht Rigth A Extended Instruction
void Op_SLLV ( void );		//* Instruction: Shift Left LV Extended Instruction
void Op_SRLV ( void );		//* Instruction: Shift Right LV Extended Instruction
void Op_SRAV ( void );		//* Instruction: Shift Right AV Extended Instruction
void Op_JR ( void );		//* Instruction: Jump to Register addr Extended Instruction
void Op_JALR ( void );		//* Instruction: Jump And Link to Register addr Extended Instruction
void Op_SYNC ( void );		//* Instruction: Sync Extended Instruction
void Op_MFHI ( void );		//* Instruction: MFHI Extended Instruction
void Op_MTHI ( void );		//* Instruction: MTHI Extended Instruction
void Op_MFLO ( void );		//* Instruction: MFLO Extended Instruction
void Op_MTLO ( void );		//* Instruction: MTLO Extended Instruction
void Op_DSLLV ( void );		//* Instruction: Double Shift Left LV Extended Instruction
void Op_DSRLV ( void );		//* Instruction: Double Shift Right LV Extended Instruction
void Op_DSRAV ( void );		//* Instruction: Double Shift Right AV Extended Instruction
void Op_MULT ( void );		//* Instruction: Multiply Extended Instruction
void Op_MULTU ( void );		//* Instruction: Multiply Unsigned Extended Instruction
void Op_DIV ( void );		//* Instruction: Divide Extended Instruction
void Op_DIVU ( void );		//* Instruction: Divide Unsigned Extended Instruction
void Op_DMULT ( void );		//* Instruction: Double Multiply Extended Instruction
void Op_DMULTU ( void );	//* Instruction: Double Multiply Unsigned Extended Instruction
void Op_DDIV ( void );		//* Instruction: Double Divide Extended Instruction
void Op_DDIVU ( void );		//* Instruction: Double Divide Unsigned Extended Instruction
void Op_ADD ( void );		//* Instruction: Add Extended Instruction
void Op_ADDU ( void );		//* Instruction: Add Unsigned Extended Instruction
void Op_SUB ( void );		//* Instruction: Subtract Extended Instruction
void Op_SUBU ( void );		//* Instruction: Subtract Unsigned Extended Instruction
void Op_AND ( void );		//* Instruction: And Extended Instruction
void Op_OR ( void );		//* Instruction: Or Extended Instruction
void Op_XOR ( void );		//* Instruction: XOr Extended Instruction
void Op_NOR ( void );		//* Instruction: NOr Extended Instruction
void Op_SLT ( void );		//* Instruction: SLT Extended Instruction
void Op_SLTU ( void );		//* Instruction: SLT Unsigned Extended Instruction
void Op_DADD ( void );		//* Instruction: Double Add Extended Instruction
void Op_DADDU ( void );		//* Instruction: Double Add Unsigned Extended Instruction
void Op_TEQ ( void );		//* Instruction: TEQ Extended Instruction
void Op_DSLL ( void );		//* Instruction: DSLL Extended Instruction
void Op_DSRL ( void );		//* Instruction: DSRL Extended Instruction
void Op_DSRA ( void );		//* Instruction: DSRA Extended Instruction
void Op_DSLL32 ( void );	//* Instruction: DSLL32 Extended Instruction
void Op_DSRL32 ( void );	//* Instruction: DSRL32 Extended Instruction
void Op_DSRA32 ( void );	//* Instruction: DSRA32 Extended Instruction

//* Core Regimm
void Op_BLTZ ( void );		//* Instruction: Branch if Less Than Zero Regimm Instruction
void Op_BGEZ ( void );		//* Instruction: Branch if Greater Than or Equal to Zero Regimm Instruction
void Op_BLTZL ( void );		//* Instruction: Branch if Less Than Zero L Regimm Instruction
void Op_BGEZL ( void );		//* Instruction: Branch if Greater Than or Equal to Zero L Regimm Instruction
void Op_BLTZAL ( void );	//* Instruction: Branch if Less Than Zero AL Regimm Regimm Instruction
void Op_BGEZAL ( void );	//* Instruction: Branch if Greater Than or Equal to Zero AL Regimm Instruction

//* Cop0
void Op_MFC0 ( void );		//* Instruction: MF Cop0 Instruction
void Op_MTC0 ( void );		//* Instruction: MT Cop0 Instruction
void Op_Tlb ( void );		//* Instruction: Perferform Cop0 Tlb Instruction

//* Cop0 Tlb
void Op_TLBR ( void );		//* Instruction: TLBR Cop0 Tlb Instruction
void Op_TLBWI ( void );		//* Instruction: TLBWI Cop0 Tlb Instruction
void Op_TLBP ( void );		//* Instruction: TLBP Cop0 Tlb Instruction
void Op_ERET ( void );		//* Instruction: ERET Cop0 Tlb Instruction

//* Cop1
void Op_MFC1 ( void );		//* Instruction: MF Cop1 Instruction
void Op_DMFC1 ( void );		//* Instruction: DMF Cop1 Instruction
void Op_CFC1 ( void );		//* Instruction: CF Cop1 Instruction
void Op_MTC1 ( void );		//* Instruction: MT Cop1 Instruction
void Op_DMTC1 ( void );		//* Instruction: DMT Cop1 Instruction
void Op_CTC1 ( void );		//* Instruction: CT Cop1 Instruction
void Op_BC ( void );		//* Instruction: Perferform Cop1 BC Instruction
void Op_S ( void );			//* Instruction: Perferform Cop1 Single Instruction
void Op_D ( void );			//* Instruction: Perferform Cop1 Double Instruction
void Op_W ( void );			//* Instruction: Perferform Cop1 Word Instruction
void Op_L ( void );			//* Instruction: Perferform Cop1 Long Instruction

//* Cop1 BC
void Op_BC1F ( void );		//* Instruction: BC1F BC Cop1 Instruction
void Op_BC1T ( void );		//* Instruction: BC1T BC Cop1 Instruction
void Op_BC1FL ( void );		//* Instruction: BC1FL BC Cop1 Instruction
void Op_BC1TL ( void );		//* Instruction: BC1TL BC Cop1 Instruction

//* Cop1 S
void Op_ADD_S ( void );		//* Instruction: Add Single Cop1 Instruction
void Op_SUB_S ( void );		//* Instruction: Subtract Single Cop1 Instruction
void Op_MULT_S ( void );	//* Instruction: Multiply Single Cop1 Instruction
void Op_DIV_S ( void );		//* Instruction: Divide Single Cop1 Instruction
void Op_SQRT_S ( void );	//* Instruction: Square Root Single Cop1 Instruction
void Op_ABS_S ( void );		//* Instruction: Abs Single Cop1 Instruction
void Op_MOV_S ( void );		//* Instruction: Move Single Cop1 Instruction
void Op_NEG_S ( void );		//* Instruction: Negate Single Cop1 Instruction
void Op_ROUND_W_S ( void );	//* Instruction: Round Single to Word Cop1 Instruction
void Op_TRUNC_W_S ( void );	//* Instruction: Truncate Single to Word Cop1 Instruction
void Op_CVT_D_S ( void );	//* Instruction: Convert Single to Double Cop1 Instruction
void Op_CVT_W_S ( void );	//* Instruction: Convert Single to Word Cop1 Instruction
void Op_CMP_S ( void );		//* Instruction: CMP_S Cop1 Instruction

//* Cop1 D
void Op_ADD_D ( void );		//* Instruction: Add Double Cop1 Instruction
void Op_SUB_D ( void );		//* Instruction: Subtract Double Cop1 Instruction
void Op_MULT_D ( void );	//* Instruction: Multiply Double Cop1 Instruction
void Op_DIV_D ( void );		//* Instruction: Divide Double Cop1 Instruction
void Op_SQRT_D ( void );	//* Instruction: Square Root Double Cop1 Instruction
void Op_ABS_D ( void );		//* Instruction: Abs Double Cop1 Instruction
void Op_MOV_D ( void );		//* Instruction: Move Double Cop1 Instruction
void Op_NEG_D ( void );		//* Instruction: Negate Double Cop1 Instruction
void Op_TRUNC_W_D ( void );	//* Instruction: Truncate Double to Word Cop1 Instruction
void Op_CVT_S_D ( void );	//* Instruction: Convert Double to Single Cop1 Instruction
void Op_CVT_W_D ( void );	//* Instruction: Convert Double to Word Cop1 Instruction
void Op_CMP_D ( void );		//* Instruction: CMP_D Cop1 Instruction

//* Cop1 W
void Op_CVT_S_W ( void );	//* Instruction: Convert Word to Single Cop1 Instruction
void Op_CVT_D_W ( void );	//* Instruction: Convert Word to Double Cop1 Instruction

//* Cop1 L
void Op_CVT_S_L ( void );	//* Instruction: Convert Long to Single Cop1 Instruction
void Op_CVT_D_L ( void );	//* Instruction: Convert Long to Double Cop1 Instruction

//* Rdp
void RdpOp_NOP ( void );			//* Instruction: No Operation Rdp Instruction
void RdpOp_RecText ( void );		//* Instruction: Draw Textured Rectangle Rdp Instruction
void RdpOp_LoadSync ( void );		//* Instruction: Load Sync Rdp Instruction
void RdpOp_PipeSync ( void );		//* Instruction: Pipe Sync Rdp Instruction
void RdpOp_TileSync ( void );		//* Instruction: Tile Sync Rdp Instruction
void RdpOp_FullSync ( void );		//* Instruction: Full Sync Rdp Instruction
void RdpOp_SetKeyGB ( void );		//* Instruction: Set Key Green Blue Rdp Instruction
void RdpOp_SetKeyR ( void );		//* Instruction: Set Key Red Rdp Instruction
void RdpOp_SetConv ( void );		//* Instruction: Set Conv? Rdp Instruction
void RdpOp_SetScissor ( void );		//* Instruction: Set Scissor Rectangle Rdp Instruction
void RdpOp_SetPrimDepth ( void );	//* Instruction: Set Primary Z Depth Rdp Instruction
void RdpOp_SetOther ( void );		//* Instruction: Set Other Modes Rdp Instruction
void RdpOp_LoadTLut ( void );		//* Instruction: Load Texture Look Up Table Rdp Instruction
void RdpOp_SetTileSize ( void );	//* Instruction: Set Tile Size Rdp Instruction
void RdpOp_LoadBlock ( void );		//* Instruction: Load Block Rdp Instruction
void RdpOp_LoadTile ( void );		//* Instruction: Load Tile Rdp Instruction
void RdpOp_SetTile ( void );		//* Instruction: Set Tile Mode Rdp Instruction
void RdpOp_RecFill ( void );		//* Instruction: Draw Filled Rectangle Rdp Instruction
void RdpOp_SetFillColor ( void );	//* Instruction: Set Fill Color Rdp Instruction
void RdpOp_SetFogColor ( void );	//* Instruction: Set Fog Color Rdp Instruction
void RdpOp_SetBlendColor ( void );	//* Instruction: Set Blend Color Rdp Instruction
void RdpOp_SetPrimColor ( void );	//* Instruction: Set Primitive Color Rdp Instruction
void RdpOp_SetEvnColor ( void );	//* Instruction: Set Evn Color Rdp Instruction
void RdpOp_SetCombine ( void );		//* Instruction: Set Combine Mode Rdp Instruction
void RdpOp_SetTextImg ( void );		//* Instruction: Set Texture Image Rdp Instruction
void RdpOp_SetZImage ( void );		//* Instruction: Set Z Image Rdp Instruction
void RdpOp_SetColorImage ( void );	//* Instruction: Set Color Image Rdp Instruction

//* Rsp Hle
//void CopyRspVertex ( RSPVertex* destVert, RSPVertex* srcVert );
void RspLoadMatrix( float mtx[4][4], u32 address );
void RspLoadVertices ( DWORD address, BYTE v0, BYTE num );
void RspAddTriangle ( BYTE tri[3] );
void RspDrawTriangles ( void );

//* Rsp Hle Fast 3D
void RspInitFast3D ( void );		//* Init Fast 3D UCODE
void RspHleOp_Fast3DNoOp ( void );
void RspHleOp_Fast3DMtx ( void );
void RspHleOp_Fast3DMoveMem ( void );
void RspHleOp_Fast3DVtx ( void );
void RspHleOp_Fast3DDL ( void );
void RspHleOp_Fast3DRdpHalf2 ( void );
void RspHleOp_Fast3DRdpHalf1 ( void );
void RspHleOp_Fast3DClearGeometryMode ( void );
void RspHleOp_Fast3DSetGeometryMode ( void );
void RspHleOp_Fast3DEndDL ( void );
void RspHleOp_Fast3DSetOtherModeL ( void );
void RspHleOp_Fast3DSetOtherModeH ( void );
void RspHleOp_Fast3DTexture( void );
void RspHleOp_Fast3DMoveWord ( void );
void RspHleOp_Fast3DPopMatrix ( void );
void RspHleOp_Fast3DTri1 ( void );

//* Rsp Hle Fast 3D Ext
void RspInitFast3DEXT ( void );
void RspHleOp_Fast3DEXTVtx ( void );
void RspHleOp_Fast3DEXTTri1 ( void );
void RspHleOp_Fast3DEXTTri2 ( void );
void RspHleOp_Fast3DEXTQuad3D ( void );

//* Rsp Hle F3DEX
void RspInitF3DEX ( void );		//* Init F3DEX UCODE
void RspHleOp_F3DEXNoOp ( void );
void RspHleOp_F3DEXMtx ( void );
void RspHleOp_F3DEXMoveMem ( void );
void RspHleOp_F3DEXVtx ( void );
void RspHleOp_F3DEXDL ( void );
void RspHleOp_F3DEXTri2 ( void );
void RspHleOp_F3DEXRdpHalf2 ( void );
void RspHleOp_F3DEXRdpHalf1 ( void );
void RspHleOp_F3DEXLine3D ( void );
void RspHleOp_F3DEXClearGeometryMode ( void );
void RspHleOp_F3DEXSetGeometryMode ( void );
void RspHleOp_F3DEXEndDL ( void );
void RspHleOp_F3DEXSetOtherModeL ( void );
void RspHleOp_F3DEXSetOtherModeH ( void );
void RspHleOp_F3DEXTexture( void );
void RspHleOp_F3DEXMoveWord ( void );
void RspHleOp_F3DEXPopMatirx ( void );
void RspHleOp_F3DEXTri1 ( void );

//* Rsp Hle F3DEX2
void RspInitF3DEX2 ( void );		//* Init F3DEX2 UCODE
void RspHleOp_F3DEX2NoOp ( void );
void RspHleOp_F3DEX2Mtx ( void );
void RspHleOp_F3DEX2MoveMem ( void );
void RspHleOp_F3DEX2Vtx ( void );
void RspHleOp_F3DEX2DL ( void );
void RspHleOp_F3DEX2Tri2 ( void );
void RspHleOp_F3DEX2RdpHalf2 ( void );
void RspHleOp_F3DEX2RdpHalf1 ( void );
void RspHleOp_F3DEX2Line3D ( void );
void RspHleOp_F3DEX2Quad ( void );
void RspHleOp_F3DEX2GeometryMode ( void );
void RspHleOp_F3DEX2EndDL ( void );
void RspHleOp_F3DEX2SetOtherModeL ( void );
void RspHleOp_F3DEX2SetOtherModeH ( void );
void RspHleOp_F3DEX2Texture( void );
void RspHleOp_F3DEX2MoveWord ( void );
void RspHleOp_F3DEX2PopMatirx ( void );
void RspHleOp_F3DEX2Tri1 ( void );

#endif
