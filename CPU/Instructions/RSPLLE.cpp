//////////////////////////////////////////////////////////////////////////
// Rsp LLE Core
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with the Rsp Core

#include "Instructions.h"
#include "../Registers.h"
#include "../Memory.h"
#include "../Timers.h"
#include "../Exceptions.h"
#include "../Dma.h"
#include "../Video.h"
#include "../../Video_Psp.h"

// Varibles
//////////////////////////////////////////////////////////////////////////
N64_OPCODE RspInstrOpcode;
DWORD RspJumpAddress;
int iRspRunning;

extern int iEmulatorRunning;

/* Removed from header to cut back on stupid warnings.
//* Rsp Core
void RspOp_Extended( void );//* Instruction: Performs Rsp Extended / Extra Instruction
void RspOp_Regimm ( void ); //* Instruction: Performs Rsp Regimm Instruction
void RspOp_J ( void );		//* Instruction: Jump Rsp Instruction
void RspOp_JAL ( void );	//* Instruction: Jump And Link Rsp Instruction
void RspOp_BEQ ( void );	//* Instruction: Branch if Equal Rsp Instruction
void RspOp_BNE ( void );	//* Instruction: Branch if Not Equal Rsp Instruction
void RspOp_BLEZ ( void );	//* Instruction: Branch if Less Then or Equal Zero Rsp Instruction
void RspOp_BGTZ ( void );	//* Instruction: Branch if Greater Then Zero Rsp Instruction
void RspOp_ADDI ( void );	//* Instruction: Add Immediate Rsp Instruction
void RspOp_ADDIU ( void );	//* Instruction: Add Immediate Unsigned Rsp Instruction
void RspOp_SLTI ( void );	//* Instruction: SLT Immediate Rsp Instruction
void RspOp_SLTIU ( void );	//* Instruction: SLT Immediate Unsigned Rsp Instruction
void RspOp_ANDI ( void );	//* Instruction: And Immediate Rsp Instruction
void RspOp_ORI ( void );	//* Instruction: Or Immediate Rsp Instruction
void RspOp_XORI ( void );	//* Instruction: XOr Immediate Rsp Instruction
void RspOp_LUI ( void );	//* Instruction: Load Upper Immediate Rsp Instruction
void RspOp_Cop0 ( void );	//* Instruction: Performs Rsp Cop0 Instruction
void RspOp_Cop2 ( void );	//* Instruction: Performs Rsp Cop2 Instruction
void RspOp_LB ( void );		//* Instruction: Load Byte Rsp Instruction
void RspOp_LH ( void );		//* Instruction: Load HalfWord Rsp Instruction
void RspOp_LW ( void );		//* Instruction: Load Word Rsp Instruction
void RspOp_LBU ( void );	//* Instruction: Load Byte Unsigned Rsp Instruction
void RspOp_LHU ( void );	//* Instruction: Load HalfWord Unsigned Rsp Instruction
void RspOp_SB ( void );		//* Instruction: Store Byte Rsp Instruction
void RspOp_SH ( void );		//* Instruction: Store HalfWord Rsp Instruction
void RspOp_SW ( void );		//* Instruction: Store Word Rsp Instruction
void RspOp_LC2 ( void );	//* Instruction: Performs Rsp LC2 Instruction
void RspOp_SC2 ( void );	//* Instruction: Performs Rsp SC2 Instruction

//* Rsp Extended
void RspOp_SLL ( void );	//* Instruction: Shift Left L Extended Rsp Instruction
void RspOp_SRL ( void );	//* Instruction: Shift Right L Extended Rsp Instruction
void RspOp_SRA ( void );	//* Instruction: Shift Right A Extended Rsp Instruction
void RspOp_SLLV ( void );	//* Instruction: Shift Left LV Extended Rsp Instruction
void RspOp_SRLV ( void );	//* Instruction: Shift Right LV Extended Rsp Instruction
void RspOp_SRAV ( void );	//* Instruction: Shift Right AV Extended Rsp Instruction
void RspOp_JR ( void );		//* Instruction: Jump to Register addr Extended Rsp Instruction
void RspOp_JALR ( void );	//* Instruction: Jump And Link to Register addr Extended Rsp Instruction
void RspOp_Break ( void );	//* Instruction: Break Extended Rsp Instruction
void RspOp_ADD ( void );	//* Instruction: Add Extended Rsp Instruction
void RspOp_ADDU ( void );	//* Instruction: Add Unsigned Extended Rsp Instruction
void RspOp_SUB ( void );	//* Instruction: Subtract Extended Rsp Instruction
void RspOp_SUBU ( void );	//* Instruction: Subtract Unsigned Extended Rsp Instruction
void RspOp_AND ( void );	//* Instruction: And Extended Rsp Instruction
void RspOp_OR ( void );		//* Instruction: Or Extended Rsp Instruction
void RspOp_XOR ( void );	//* Instruction: XOr Extended Rsp Instruction
void RspOp_NOR ( void );	//* Instruction: NOr Extended Rsp Instruction
void RspOp_SLT ( void );	//* Instruction: SLT Extended Rsp Instruction
void RspOp_SLTU ( void );	//* Instruction: SLT Unsigned Extended Rsp Instruction

//* Rsp Regimm
void RspOp_BLTZ ( void );	//* Instruction: Branch if Less Then Zero Regimm Rsp Instruction
void RspOp_BGEZ ( void );	//* Instruction: Branch if Greater Then or Equal Zero Regimm Rsp Instruction
void RspOp_BLTZAL ( void );	//* Instruction: Branch if Less Then Zero AL Regimm Rsp Instruction
void RspOp_BGEZAL ( void );	//* Instruction: Branch if Greater Then or Equal Zero AL Regimm Rsp Instruction

//* Rsp Cop0
void RspOp_MFC0 ( void );	//* Instruction: MF Cop0 Rsp Instruction
void RspOp_MTC0 ( void );	//* Instruction: MT Cop0 Rsp Instruction

//* Rsp Cop2
void RspOp_MFC2 ( void );	//* Instruction: MFC2 Cop2 Rsp Instruction
void RspOp_CFC2 ( void );	//* Instruction: CFC2 Cop2 Rsp Instruction
void RspOp_MTC2 ( void );	//* Instruction: MTC2 Cop2 Rsp Instruction
void RspOp_CTC2 ( void );	//* Instruction: CTC2 Cop2 Rsp Instruction
void RspOp_Vect ( void );	//* Instruction: Perform Vector Rsp Instruction

//* Rsp Vect
void RspOp_VMULF ( void );	//* Instruction: VMULF Vector Rsp Instruction
void RspOp_VMUDL ( void );	//* Instruction: VMUDL Vector Rsp Instruction
void RspOp_VMUDM ( void );	//* Instruction: VMUDH Vector Rsp Instruction
void RspOp_VMUDN ( void );	//* Instruction: VMUDN Vector Rsp Instruction
void RspOp_VMUDH ( void );	//* Instruction: VMUDH Vector Rsp Instruction
void RspOp_VMACF ( void );	//* Instruction: VMACF Vector Rsp Instruction
void RspOp_VMACU ( void );	//* Instruction: VMACU Vector Rsp Instruction
void RspOp_VMADL ( void );	//* Instruction: VMADL Vector Rsp Instruction
void RspOp_VMADM ( void );	//* Instruction: VMADM Vector Rsp Instruction
void RspOp_VMADN ( void );	//* Instruction: VMADN Vector Rsp Instruction
void RspOp_VMADH ( void );	//* Instruction: VMADH Vector Rsp Instruction
void RspOp_VADD ( void );	//* Instruction: VADD Vector Rsp Instruction
void RspOp_VSUB ( void );	//* Instruction: VSUB Vector Rsp Instruction
void RspOp_VADDC ( void );	//* Instruction: VADDC Vector Rsp Instruction
void RspOp_VSUBC ( void );	//* Instruction: VSUBC Vector Rsp Instruction
void RspOp_VSAW ( void );	//* Instruction: VSAW Vector Rsp Instruction
void RspOp_VLT ( void );	//* Instruction: VLT Vector Rsp Instruction
void RspOp_VEQ ( void );	//* Instruction: VEQ Vector Rsp Instruction
void RspOp_VNE ( void );	//* Instruction: VNE Vector Rsp Instruction
void RspOp_VGE ( void );	//* Instruction: VGE Vector Rsp Instruction
void RspOp_VCL ( void );	//* Instruction: VCL Vector Rsp Instruction
void RspOp_VCH ( void );	//* Instruction: VCH Vector Rsp Instruction
void RspOp_VCR ( void );	//* Instruction: VCR Vector Rsp Instruction
void RspOp_VMRG ( void );	//* Instruction: VMRG Vector Rsp Instruction
void RspOp_VAND ( void );	//* Instruction: VAND Vector Rsp Instruction
void RspOp_VNAND ( void );	//* Instruction: VNAND Vector Rsp Instruction
void RspOp_VOR ( void );	//* Instruction: VOR Vector Rsp Instruction
void RspOp_VNOR ( void );	//* Instruction: VNOR Vector Rsp Instruction
void RspOp_VXOR ( void );	//* Instruction: VXOR Vector Rsp Instruction
void RspOp_VNXOR ( void );	//* Instruction: VNXOR Vector Rsp Instruction
void RspOp_VRCP ( void );	//* Instruction: VRCP Vector Rsp Instruction
void RspOp_VRCPL ( void );	//* Instruction: VRCPL Vector Rsp Instruction
void RspOp_VRCPH ( void );	//* Instruction: VRCPH Vector Rsp Instruction
void RspOp_VMOV ( void );	//* Instruction: VMOV Vector Rsp Instruction

//* Rsp LC2
void RspOp_LBV ( void );	//* Instruction: LBV LC2 Rsp Instruction
void RspOp_LSV ( void );	//* Instruction: LSV LC2 Rsp Instruction
void RspOp_LLV ( void );	//* Instruction: LLV LC2 Rsp Instruction
void RspOp_LDV ( void );	//* Instruction: LDV LC2 Rsp Instruction
void RspOp_LQV ( void );	//* Instruction: LQV LC2 Rsp Instruction
void RspOp_LRV ( void );	//* Instruction: LRV LC2 Rsp Instruction
void RspOp_LPV ( void );	//* Instruction: LPV LC2 Rsp Instruction
void RspOp_LUV ( void );	//* Instruction: LUV LC2 Rsp Instruction
void RspOp_LHV ( void );	//* Instruction: LHV LC2 Rsp Instruction
void RspOp_LFV ( void );	//* Instruction: LFV LC2 Rsp Instruction
void RspOp_LTV ( void );	//* Instruction: LTV LC2 Rsp Instruction

//* Rsp SC2
void RspOp_SBV ( void );	//* Instruction: SBV SC2 Rsp Instruction
void RspOp_SSV ( void );	//* Instruction: SSV SC2 Rsp Instruction
void RspOp_SLV ( void );	//* Instruction: SLV SC2 Rsp Instruction
void RspOp_SDV ( void );	//* Instruction: SDV SC2 Rsp Instruction
void RspOp_SQV ( void );	//* Instruction: SQV SC2 Rsp Instruction
void RspOp_SFV ( void );	//* Instruction: SFV SC2 Rsp Instruction
*/

// Instruction Set ( Rsp Core )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspCore[64])(void) =
{
	RspOp_Extended, RspOp_Regimm, RspOp_J,    RspOp_JAL,   RspOp_BEQ,  RspOp_BNE, RspOp_BLEZ, RspOp_BGTZ,
	RspOp_ADDI,     RspOp_ADDIU,  RspOp_SLTI, RspOp_SLTIU, RspOp_ANDI, RspOp_ORI, RspOp_XORI, RspOp_LUI,
	RspOp_Cop0,     RspOp_NI,     RspOp_Cop2, RspOp_NI,    RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,
	RspOp_NI,       RspOp_NI,     RspOp_NI,   RspOp_NI,    RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,
	RspOp_LB,       RspOp_LH,     RspOp_NI,   RspOp_LW,    RspOp_LBU,  RspOp_LHU, RspOp_NI,   RspOp_NI,
	RspOp_SB,       RspOp_SH,     RspOp_NI,   RspOp_SW,    RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,
	RspOp_NI,       RspOp_NI,     RspOp_LC2,  RspOp_NI,    RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,
	RspOp_NI,       RspOp_NI,     RspOp_SC2,  RspOp_NI,    RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI

//* Instruction Rsp Core Reference ( Taken from Pj64 Rsp Interp. )
//   SPECIAL, REGIMM, J   , JAL  , BEQ , BNE , BLEZ , BGTZ ,
//   ADDI   , ADDIU , SLTI, SLTIU, ANDI, ORI , XORI , LUI  ,
//   COP0   , NI    , COP2, NI   , NI  , NI  , NI   , NI   ,
//   NI     , NI    , NI  , NI   , NI  , NI  , NI   , NI   ,
//   LB     , LH    , NI  , LW   , LBU , LHU , NI   , NI   ,
//   SB     , SH    , NI  , SW   , NI  , NI  , NI   , NI   ,
//   NI     , NI    , LC2 , NI   , NI  , NI  , NI   , NI   ,
//   NI     , NI    , SC2 , NI   , NI  , NI  , NI   , NI
};

// Instruction Set ( Rsp Extended / Extra )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspExtended[64])(void) =
{
	RspOp_SLL, RspOp_NI,   RspOp_SRL, RspOp_SRA,  RspOp_SLLV, RspOp_NI,    RspOp_SRLV, RspOp_SRAV,
	RspOp_JR,  RspOp_JALR, RspOp_NI,  RspOp_NI,   RspOp_NI,   RspOp_Break, RspOp_NI,   RspOp_NI,
	RspOp_NI,  RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,   RspOp_NI,    RspOp_NI,   RspOp_NI,
	RspOp_NI,  RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,   RspOp_NI,    RspOp_NI,   RspOp_NI,
	RspOp_ADD, RspOp_ADDU, RspOp_SUB, RspOp_SUBU, RspOp_AND,  RspOp_OR,    RspOp_XOR,  RspOp_NOR,
	RspOp_NI,  RspOp_NI,   RspOp_SLT, RspOp_SLTU, RspOp_NI,   RspOp_NI,    RspOp_NI,   RspOp_NI,
	RspOp_NI,  RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,   RspOp_NI,    RspOp_NI,   RspOp_NI,
	RspOp_NI,  RspOp_NI,   RspOp_NI,  RspOp_NI,   RspOp_NI,   RspOp_NI,    RspOp_NI,   RspOp_NI

//* Instruction Rsp Extended Reference ( Taken from Pj64 Rsp Interp. )
//   SLL, NI  , SRL, SRA , SLLV, NI   , SRLV, SRAV,
//   JR , JALR, NI , NI  , NI  , BREAK, NI  , NI  ,
//   NI , NI  , NI , NI  , NI  , NI   , NI  , NI  ,
//   NI , NI  , NI , NI  , NI  , NI   , NI  , NI  ,
//   ADD, ADDU, SUB, SUBU, AND , OR   , XOR , NOR ,
//   NI , NI  , SLT, SLTU, NI  , NI   , NI  , NI  ,
//   NI , NI  , NI , NI  , NI  , NI   , NI  , NI  ,
//   NI , NI  , NI , NI  , NI  , NI   , NI  , NI
};

// Instruction Set ( Rsp Regimm )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspRegimm[32])(void) =
{
	RspOp_BLTZ,   RspOp_BGEZ,   RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,     RspOp_NI,     RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_BLTZAL, RspOp_BGEZAL, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,     RspOp_NI,     RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI

//* Instruction Rsp Cop0 Reference ( Taken from Pj64 Rsp Interp. )
//   BLTZ  , BGEZ  , NI, NI, NI, NI, NI, NI,
//   NI    , NI    , NI, NI, NI, NI, NI, NI,
//   BLTZAL, BGEZAL, NI, NI, NI, NI, NI, NI,
//   NI    , NI    , NI, NI, NI, NI, NI, NI
};

// Instruction Set ( Rsp Cop0 / Co Processor 0 )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspCop0[32])(void) =
{
	RspOp_MFC0, RspOp_NI, RspOp_NI, RspOp_NI, RspOp_MTC0, RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,   RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI,   RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,   RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI,   RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,   RspOp_NI, RspOp_NI, RspOp_NI, RspOp_NI,   RspOp_NI, RspOp_NI, RspOp_NI

//* Instruction Rsp Cop0 Reference ( Taken from Pj64 Rsp Interp. )
//   MFC0, NI, NI, NI, MTC0, NI, NI, NI,
//   NI  , NI, NI, NI, NI  , NI, NI, NI,
//   NI  , NI, NI, NI, NI  , NI, NI, NI,
//   NI  , NI, NI, NI, NI  , NI, NI, NI
};

// Instruction Set ( Rsp Cop2 / Co Processor 2 )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspCop2[32])(void) =
{
	RspOp_MFC2, RspOp_NI,   RspOp_CFC2, RspOp_NI,   RspOp_MTC2, RspOp_NI,   RspOp_CTC2, RspOp_NI,
	RspOp_NI,   RspOp_NI,   RspOp_NI,   RspOp_NI,   RspOp_NI,   RspOp_NI,   RspOp_NI,   RspOp_NI,
	RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect,
	RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect, RspOp_Vect

//* Instruction Rsp Cop0 Reference ( Taken from Pj64 Rsp Interp. )
//   MFC2, NI , CFC2, NI , MTC2, NI , CTC2, NI ,
//   NI  , NI , NI  , NI , NI  , NI , NI  , NI ,
//   VEC , VEC, VEC , VEC, VEC , VEC, VEC , VEC,
//   VEC , VEC, VEC , VEC, VEC , VEC, VEC , VEC
};

// Instruction Set ( Rsp Vector )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspVect[64])(void) =
{
	RspOp_VMULF, RspOp_NI,    RspOp_NI,    RspOp_NI,   RspOp_VMUDL, RspOp_VMUDM, RspOp_VMUDN, RspOp_VMUDH,
	RspOp_VMACF, RspOp_VMACU, RspOp_NI,    RspOp_NI,   RspOp_VMADL, RspOp_VMADM, RspOp_VMADN, RspOp_VMADH,
	RspOp_VADD,  RspOp_VSUB,  RspOp_NI,    RspOp_NI,   RspOp_VADDC, RspOp_VSUBC, RspOp_NI,    RspOp_NI,
	RspOp_NI,    RspOp_NI,    RspOp_NI,    RspOp_NI,   RspOp_NI,    RspOp_VSAW,  RspOp_NI,    RspOp_NI,
	RspOp_VLT,   RspOp_VEQ,   RspOp_VNE,   RspOp_VGE,  RspOp_VCL,   RspOp_VCH,   RspOp_VCR,   RspOp_VMRG,
	RspOp_VAND,  RspOp_VNAND, RspOp_VOR,   RspOp_VNOR, RspOp_VXOR,  RspOp_VNXOR, RspOp_NI,    RspOp_NI,
	RspOp_VRCP,  RspOp_VRCPL, RspOp_VRCPH, RspOp_VMOV, RspOp_NI,    RspOp_NI,    RspOp_NI,    RspOp_NI,
	RspOp_NI,    RspOp_NI,    RspOp_NI,    RspOp_NI,   RspOp_NI,    RspOp_NI,    RspOp_NI,    RspOp_NI

//* Instruction Rsp Vect Reference ( Taken from Pj64 Rsp Interp. )
//   VMULF, VMULU, NI   , NI   , VMUDL, VMUDM, VMUDN, VMUDH,
//   VMACF, VMACU, NI   , VMACQ, VMADL, VMADM, VMADN, VMADH,
//   VADD , VSUB , NI   , VABS , VADDC, VSUBC, NI   , NI   ,
//   NI   , NI   , NI   , NI   , NI   , VSAW , NI   , NI   ,
//   VLT  , VEQ  , VNE  , VGE  , VCL  , VCH  , VCR  , VMRG ,
//   VAND , VNAND, VOR  , VNOR , VXOR , VNXOR, NI   , NI   ,
//   VRCP , VRCPL, VRCPH, VMOV , VRSQ , VRSQL, VRSQH, VNOOP,
//   NI   , NI   , NI   , NI   , NI   , NI   , NI   , NI
};

// Instruction Set ( Rsp LC2 )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspLC2[32])(void) =
{
	RspOp_LBV, RspOp_LSV, RspOp_LLV, RspOp_LDV, RspOp_LQV, RspOp_LRV, RspOp_LPV, RspOp_LUV,
	RspOp_LHV, RspOp_LFV, RspOp_NI,  RspOp_LTV, RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,
	RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,
	RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI

//* Instruction Rsp Cop0 Reference ( Taken from Pj64 Rsp Interp. )
//   LBV, LSV, LLV, LDV, LQV, LRV, LPV, LUV,
//   LHV, LFV, NI , LTV, NI , NI , NI , NI ,
//   NI , NI , NI , NI , NI , NI , NI , NI ,
//   NI , NI , NI , NI , NI , NI , NI , NI
};

// Instruction Set ( Rsp SC2 )
//////////////////////////////////////////////////////////////////////////
void (*OpcodeSet_RspSC2[32])(void) =
{
	RspOp_SBV, RspOp_SSV, RspOp_SLV, RspOp_SDV, RspOp_SQV, RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,  RspOp_SFV, RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI, RspOp_NI, RspOp_NI,
	RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI,  RspOp_NI, RspOp_NI, RspOp_NI

//* Instruction Rsp Cop0 Reference ( Taken from Pj64 Rsp Interp. )
//   SBV, SSV, SLV, SDV, SQV, SRV, SPV, SUV,
//   SHV, SFV, SWV, STV, NI , NI , NI , NI ,
//   NI , NI , NI , NI , NI , NI , NI , NI ,
//   NI , NI , NI , NI , NI , NI , NI , NI
};

// Instruction: No Instruction ( Unemulated / Not Suported )
//////////////////////////////////////////////////////////////////////////
void RspOp_NI ( void )
{
	//* Clear screen so we can read the print
	ResetDisplay();

	switch( RspInstrOpcode.op )
	{
		case 0: // Extra
			printf( "0x%08X: hit unknown Extended Rsp opcode %08X (op: %02d extra: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.funct);
			break;
		case 1: // Regimm
			printf( "0x%08X: hit unknown Regimm Rsp opcode %08X (op: %02d regimm: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.rt);
			break;
		case 16: // COP0
			printf( "0x%08X: hit unknown Cop0 Rsp opcode %08X (op: %02d cop0: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.rs);
			break;
		case 18: // COP2
			if( RspInstrOpcode.rs < 16 )
				printf( "0x%08X: hit unknown Cop2 Rsp opcode %08X (op: %02d cop2: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.rs);
			else
				printf( "0x%08X: hit unknown Vect Rsp opcode %08X (op: %02d Vect: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.funct);
			break;
		case 50: // LC2
			printf( "0x%08X: hit unknown LC2 Rsp opcode %08X (op: %02d lc2: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.rd);
			break;
		case 58: // SC2
			printf( "0x%08X: hit unknown SC2 Rsp opcode %08X (op: %02d sc2: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op, (unsigned int)RspInstrOpcode.rd);
			break;
		default:
			printf( "0x%08X: hit unknown Rsp opcode %08X (op: %02d)\n", (unsigned int)*N64Regs->RspPC, (unsigned int)RspInstrOpcode.Hex, (unsigned int)RspInstrOpcode.op);
			break;
	}

	iEmulatorRunning = 0;
	iRspRunning = 0;
}

// Instruction: Extended ( Performs Cpu Extended / Extra Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_Extended ( void )
{
	OpcodeSet_RspExtended[RspInstrOpcode.funct]();
}

// Instruction: Regimm ( Performs Cpu Regimm Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_Regimm ( void )
{
	OpcodeSet_RspRegimm[RspInstrOpcode.rt]();
}

// Instruction: J ( Jump Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_J ( void )
{
	RspJumpAddress = ( RspInstrOpcode.target << 2 ) & 0xFFC;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: JAL ( Jump And Link Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_JAL ( void )
{
	RspJumpAddress = ( RspInstrOpcode.target << 2 ) & 0xFFC;
	N64Regs->RSP[31].UW = ( *N64Regs->RspPC + 8 ) & 0xFFC;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BEQ ( Branch if Equal Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BEQ ( void )
{
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W == N64Regs->RSP[RspInstrOpcode.rt].W )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;
	
	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BNE ( Branch if Not Equal Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BNE ( void )
{
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W != N64Regs->RSP[RspInstrOpcode.rt].W )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BLEZ ( Branch if Less Then or Equal to Zero Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BLEZ ( void )
{
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W <= 0 )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BGTZ ( Branch if Greater Then Zero Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BGTZ ( void )
{
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W > 0 )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: ADDI ( Add Immediate Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_ADDI ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSP[RspInstrOpcode.rs].W + (short)RspInstrOpcode.immediate;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: ADDIU ( Add Immediate Unsigned Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_ADDIU ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->RSP[RspInstrOpcode.rs].UW + (DWORD)( (short)RspInstrOpcode.immediate );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SLTI ( SLT Immediate Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLTI ( void )
{
	if( N64Regs->RSP[RspInstrOpcode.rs].W < (short)RspInstrOpcode.immediate )
		N64Regs->RSP[RspInstrOpcode.rt].W = 1;
	else
		N64Regs->RSP[RspInstrOpcode.rt].W = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SLTIU ( SLT Immediate Unsigned Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLTIU ( void )
{
	if( N64Regs->RSP[RspInstrOpcode.rs].UW < (DWORD)( (short)RspInstrOpcode.immediate ) )
		N64Regs->RSP[RspInstrOpcode.rt].W = 1;
	else
		N64Regs->RSP[RspInstrOpcode.rt].W = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: ANDI ( And Immediate Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_ANDI ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSP[RspInstrOpcode.rs].W & RspInstrOpcode.immediate;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: ORI ( Or Immediate Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_ORI ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSP[RspInstrOpcode.rs].W | RspInstrOpcode.immediate;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: XORI ( XOr Immediate Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_XORI ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSP[RspInstrOpcode.rs].W ^ RspInstrOpcode.immediate;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LUI ( Load Upper Immediate Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LUI ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].W = (short)RspInstrOpcode.offset << 16;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Cop0 ( Perferform Cop0 Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_Cop0 ( void )
{
	OpcodeSet_RspCop0[RspInstrOpcode.rs]();
}

// Instruction: Cop2 ( Perferform Cop2 Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_Cop2 ( void )
{
	OpcodeSet_RspCop2[RspInstrOpcode.rs]();
}

// Instruction: LB ( Load Byte Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LB ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].UB[0] = LoadByteDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset );
	N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSP[RspInstrOpcode.rt].B[0];  
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LH ( Load HalfWord Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LH ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].UHW[0] = LoadWordDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset );
	N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSP[RspInstrOpcode.rt].HW[0];
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LW ( Load Word Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LW ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].UW = LoadDWordDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LBU ( Load Byte Unsigned Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LBU ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].UB[0] = LoadByteDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset );
	N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->RSP[RspInstrOpcode.rt].UB[0];  
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LHU ( Load HalfWord Unsigned Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LHU ( void )
{
	N64Regs->RSP[RspInstrOpcode.rt].UHW[0] = LoadWordDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset );
	N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->RSP[RspInstrOpcode.rt].UHW[0];
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SB ( Store Byte Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SB ( void )
{
	StoreByteDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset, N64Regs->RSP[RspInstrOpcode.rt].UB[0] );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SH ( Store HalfWord Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SH ( void )
{
	StoreWordDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset, N64Regs->RSP[RspInstrOpcode.rt].UHW[0] );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SW ( Store Word Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SW ( void )
{
	StoreDWordDMem( N64Regs->RSP[RspInstrOpcode.base].UW + (short)RspInstrOpcode.offset, N64Regs->RSP[RspInstrOpcode.rt].UW );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 ( Performs LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LC2 ( void )
{
	OpcodeSet_RspLC2[RspInstrOpcode.rd]();
}

// Instruction: SC2 ( Performs SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SC2 ( void )
{
	OpcodeSet_RspSC2[RspInstrOpcode.rd]();
}

// Instruction: Extended SLL ( Shift Left L Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLL ( void )
{
	if( RspInstrOpcode.Hex != 0 ) //* Incase of NOP
	{
		N64Regs->RSP[RspInstrOpcode.rd].W = N64Regs->RSP[RspInstrOpcode.rt].W << RspInstrOpcode.sa;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SRL ( Shift Right L Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SRL ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rt].UW >> RspInstrOpcode.sa;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SRA ( Shift Right A Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SRA ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].W = N64Regs->RSP[RspInstrOpcode.rt].W >> RspInstrOpcode.sa;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SLLV ( Shift Left LV Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLLV ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].W = N64Regs->RSP[RspInstrOpcode.rt].W << ( N64Regs->RSP[RspInstrOpcode.rs].W & 0x1F );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SRLV ( Shift Right LV Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SRLV ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rt].UW >> ( N64Regs->RSP[RspInstrOpcode.rs].W & 0x1F );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SRAV ( Shift Right AV Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SRAV ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].W = N64Regs->RSP[RspInstrOpcode.rt].W >> ( N64Regs->RSP[RspInstrOpcode.rs].W & 0x1F );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: JR ( Jump to Register addr Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_JR ( void )
{
	RspJumpAddress = N64Regs->RSP[RspInstrOpcode.rs].W & 0xFFC;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: JALR ( Jump And Link to Register addr Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_JALR ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].W = (*N64Regs->RspPC + 8) & 0xFFC;
	RspJumpAddress = N64Regs->RSP[RspInstrOpcode.rs].W & 0xFFC;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: Break ( Break Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_Break ( void )
{
	//* End / Pause Rsp
	iRspRunning = 0;

	//* Set SP Hault & Broke to Status Reg
	N64Regs->SP[4] |= ( 0x01 | 0x02 );

	if( ( N64Regs->SP[4] & 0x040 ) != 0 )
	{
		//* Set SP Interupt
		N64Regs->MI[2] |= 0x01;
		CheckInterrupts();
	}

	//*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended ADD ( Add Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_ADD ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].W = N64Regs->RSP[RspInstrOpcode.rs].W + N64Regs->RSP[RspInstrOpcode.rt].W;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended ADDU ( Add Unsigned Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_ADDU ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rs].UW + N64Regs->RSP[RspInstrOpcode.rt].UW;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SUB ( Subtract Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SUB ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].W = N64Regs->RSP[RspInstrOpcode.rs].W - N64Regs->RSP[RspInstrOpcode.rt].W;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SUBU ( Subtract Unsigned Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SUBU ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rs].UW - N64Regs->RSP[RspInstrOpcode.rt].UW;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended AND ( And Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_AND ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rs].UW & N64Regs->RSP[RspInstrOpcode.rt].UW;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended OR ( Or Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_OR ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rs].UW | N64Regs->RSP[RspInstrOpcode.rt].UW;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended XOR ( XOr Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_XOR ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = N64Regs->RSP[RspInstrOpcode.rs].UW ^ N64Regs->RSP[RspInstrOpcode.rt].UW;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended NOR ( NOr Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_NOR ( void )
{
	N64Regs->RSP[RspInstrOpcode.rd].UW = ~( N64Regs->RSP[RspInstrOpcode.rs].UW | N64Regs->RSP[RspInstrOpcode.rt].UW );
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SLT ( SLT Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLT ( void )
{
	if( N64Regs->RSP[RspInstrOpcode.rs].W < N64Regs->RSP[RspInstrOpcode.rt].W )
		N64Regs->RSP[RspInstrOpcode.rd].UW = 1;
	else
		N64Regs->RSP[RspInstrOpcode.rd].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: Extended SLTU ( SLT Unsigned Extended Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLTU ( void )
{
	if( N64Regs->RSP[RspInstrOpcode.rs].UW < N64Regs->RSP[RspInstrOpcode.rt].UW )
		N64Regs->RSP[RspInstrOpcode.rd].UW = 1;
	else
		N64Regs->RSP[RspInstrOpcode.rd].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: BLTZ ( Branch if Less Then Zero Regimm Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BLTZ ( void )
{
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W < 0 )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BGEZ ( Branch if Greater Then or Equal Zero Regimm Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BGEZ ( void )
{
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W >= 0 )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BLTZ ( Branch if Less Then Zero AL Regimm Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BLTZAL ( void )
{
	N64Regs->RSP[31].UW = (*N64Regs->RspPC + 8) & 0xFFC;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W < 0 )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: BGEZAL ( Branch if Greater Then or Equal Zero AL Regimm Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_BGEZAL ( void )
{
	N64Regs->RSP[31].UW = (*N64Regs->RspPC + 8) & 0xFFC;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	if( N64Regs->RSP[RspInstrOpcode.rs].W >= 0 )
		RspJumpAddress = ( *N64Regs->RspPC + ( (short)RspInstrOpcode.offset << 2 ) ) & 0xFFC;
	else
		RspJumpAddress = ( *N64Regs->RspPC + 4 ) & 0xFFC;

	RunRspInstruction();

	*N64Regs->RspPC = RspJumpAddress;
}

// Instruction: MFC0 ( MF Cop0 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_MFC0 ( void )
{
	switch( RspInstrOpcode.rd ) 
	{
		case 4:
			N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->SP[4]; //SP_STATUS_REG
			break;

		case 5:
			N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->SP[5]; //SP_DMA_FULL_REG
			break;

		case 6:
			N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->SP[6]; //SP_DMA_BUSY_REG
			break;

		case 7: 
			N64Regs->RSP[RspInstrOpcode.rt].W = 0;
			break;

		case 8:
			N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->DPC[0]; //DPC_START_REG
			break;

		case 9: 
			N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->DPC[1]; //DPC_END_REG
			break;

		case 10:
			N64Regs->RSP[RspInstrOpcode.rt].UW = N64Regs->DPC[2]; //DPC_CURRENT_REG/[2]
			break;

		case 11:
			N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->DPC[3]; //DPC_STATUS_REG
			break;

		case 12:
			N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->DPC[4]; //DPC_CLOCK_REG
			break;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: MTC0 ( MT Cop0 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_MTC0 ( void )
{
	switch( RspInstrOpcode.rd )
	{
		case 0:
			N64Regs->SP[0] = N64Regs->RSP[RspInstrOpcode.rt].UW; //SP_MEM_ADDR_REG
			break;

		case 1:
			N64Regs->SP[1] = N64Regs->RSP[RspInstrOpcode.rt].UW; //SP_DRAM_ADDR_REG
			break;

		case 2: 
			N64Regs->SP[2] = N64Regs->RSP[RspInstrOpcode.rt].UW; //SP_RD_LEN_REG
			DmaSpReadRsp();
			break;

		case 3: 
			N64Regs->SP[3] = N64Regs->RSP[RspInstrOpcode.rt].UW; //SP_WR_LEN_REG
			DmaSpWriteRsp();
			break;

		case 4: 
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000001 ) != 0) { N64Regs->SP[4] &= ~0x001; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000002 ) != 0) { N64Regs->SP[4] |= 0x001;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000004 ) != 0) { N64Regs->SP[4] &= ~0x002; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000008 ) != 0) { N64Regs->MI[2] &= ~0x01; }
			//if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & SP_SET_INTR ) != 0) { DisplayError("SP_SET_INTR");  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000020 ) != 0) { N64Regs->SP[4] &= ~0x020; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000040 ) != 0) { N64Regs->SP[4] |= 0x020;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000080 ) != 0) { N64Regs->SP[4] &= ~0x040; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000100 ) != 0) { N64Regs->SP[4] |= 0x040;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000200 ) != 0) { N64Regs->SP[4] &= ~0x080; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000400 ) != 0) { N64Regs->SP[4] |= 0x080;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0000800 ) != 0) { N64Regs->SP[4] &= ~0x100; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0001000 ) != 0) { N64Regs->SP[4] |= 0x100;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0002000 ) != 0) { N64Regs->SP[4] &= ~0x200; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0004000 ) != 0) { N64Regs->SP[4] |= 0x200;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0008000 ) != 0) { N64Regs->SP[4] &= ~0x400; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0010000 ) != 0) { N64Regs->SP[4] |= 0x400;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0020000 ) != 0) { N64Regs->SP[4] &= ~0x800; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0040000 ) != 0) { N64Regs->SP[4] |= 0x800;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0080000 ) != 0) { N64Regs->SP[4] &= ~0x1000; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0100000 ) != 0) { N64Regs->SP[4] |= 0x1000;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0200000 ) != 0) { N64Regs->SP[4] &= ~0x2000; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0400000 ) != 0) { N64Regs->SP[4] |= 0x2000;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0800000 ) != 0) { N64Regs->SP[4] &= ~0x4000; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x1000000 ) != 0) { N64Regs->SP[4] |= 0x4000;  }
			break;

		case 7:
			N64Regs->SP[7] = 0;
			break;

		case 8: 
			N64Regs->DPC[0] = N64Regs->RSP[RspInstrOpcode.rt].UW; //DPC_START_REG
			N64Regs->DPC[2] = N64Regs->RSP[RspInstrOpcode.rt].UW; //DPC_CURRENT_REG
			break;

		case 9: 
			N64Regs->DPC[1] = N64Regs->RSP[RspInstrOpcode.rt].UW; //DPC_END_REG
			ProcessRDPList();
			break;

		case 10:
			N64Regs->DPC[2] = N64Regs->RSP[RspInstrOpcode.rt].UW; //DPC_CURRENT_REG
			break;

		case 11: 
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0001 ) != 0) { N64Regs->DPC[3] &= ~0x001; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0002 ) != 0) { N64Regs->DPC[3] |= 0x001;  }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0004 ) != 0) { N64Regs->DPC[3] &= ~0x002; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0008 ) != 0) { N64Regs->DPC[3] |= 0x002;  }		
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0010 ) != 0) { N64Regs->DPC[3] &= ~0x004; }
			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & 0x0020 ) != 0) { N64Regs->DPC[3] |= 0x004;  }
//			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & DPC_CLR_TMEM_CTR ) != 0) { DisplayError("RSP: DPC_STATUS_REG: DPC_CLR_TMEM_CTR"); }
//			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & DPC_CLR_PIPE_CTR ) != 0) { DisplayError("RSP: DPC_STATUS_REG: DPC_CLR_PIPE_CTR"); }
//			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & DPC_CLR_CMD_CTR ) != 0) { DisplayError("RSP: DPC_STATUS_REG: DPC_CLR_CMD_CTR"); }
//			if ( ( N64Regs->RSP[RspInstrOpcode.rt].W & DPC_CLR_CLOCK_CTR ) != 0) { DisplayError("RSP: DPC_STATUS_REG: DPC_CLR_CLOCK_CTR"); }
			break;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: MFC2 ( MFC2 Cop2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_MFC2 ( void )
{
	int element = (RspInstrOpcode.sa >> 1);

	N64Regs->RSP[RspInstrOpcode.rt].B[1] = N64Regs->RSPVect[RspInstrOpcode.rd].B[15 - element];
	N64Regs->RSP[RspInstrOpcode.rt].B[0] = N64Regs->RSPVect[RspInstrOpcode.rd].B[15 - ( ( element + 1 ) % 16 )];
	N64Regs->RSP[RspInstrOpcode.rt].W    = N64Regs->RSP[RspInstrOpcode.rt].HW[0];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: CFC2 ( CFC2 Cop2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_CFC2 ( void )
{
	switch( RspInstrOpcode.rd & 0x03 ) 
	{
		case 0: 
			N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSPFlags[0].HW[0]; 
			break;

		case 1: 
			N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSPFlags[1].HW[0];
			break;

		case 2:
			N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSPFlags[2].HW[0];
			break;

		case 3: 
			N64Regs->RSP[RspInstrOpcode.rt].W = N64Regs->RSPFlags[2].HW[0]; 
			break;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: MTC2 ( MTC2 Cop2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_MTC2 ( void )
{
	int element = 15 - (RspInstrOpcode.sa >> 1);

	N64Regs->RSPVect[RspInstrOpcode.rd].B[element] = N64Regs->RSP[RspInstrOpcode.rt].B[1];

	if (element != 0)
		N64Regs->RSPVect[RspInstrOpcode.rd].B[element - 1] = N64Regs->RSP[RspInstrOpcode.rt].B[0];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: CTC2 ( CTC2 Cop2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_CTC2 ( void )
{
	switch( RspInstrOpcode.rd & 0x03 ) 
	{
		case 0:
			N64Regs->RSPFlags[0].HW[0] = N64Regs->RSP[RspInstrOpcode.rt].HW[0];
			break;

		case 1:
			N64Regs->RSPFlags[1].HW[0] = N64Regs->RSP[RspInstrOpcode.rt].HW[0];
			break;

		case 2:
			N64Regs->RSPFlags[2].B[0] = N64Regs->RSP[RspInstrOpcode.rt].B[0];
			break;

		case 3:
			N64Regs->RSPFlags[2].B[0] = N64Regs->RSP[RspInstrOpcode.rt].B[0];
			break;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VECT ( Performs Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_Vect ( void )
{
	OpcodeSet_RspVect[RspInstrOpcode.funct]();
}

// Instruction: VMULF ( VMULF Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMULF ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		if( N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] != 0x8000 || N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL] != 0x8000 ) 
		{
			temp.W = ( (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] * (long)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) << 1; 
			temp.UW += 0x8000;
			N64Regs->RSPAccum[iEL].HW[2] = temp.HW[1];
			N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];

			if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
				N64Regs->RSPAccum[iEL].HW[3] = -1;
			else
				N64Regs->RSPAccum[iEL].HW[3] = 0;

			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
		} 
		else 
		{
			temp.W = 0x80000000; 
			N64Regs->RSPAccum[iEL].UHW[3] = 0;
			N64Regs->RSPAccum[iEL].UHW[2] = 0x8000;
			N64Regs->RSPAccum[iEL].UHW[1] = 0x8000;
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMUDL ( VMUDL Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMUDL ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.UW = (DWORD)N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] * (DWORD)N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
		N64Regs->RSPAccum[iEL].W[1] = 0;
		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[1];
		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMUDM ( VMUDM Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMUDM ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.UW = (DWORD)( (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] ) * (DWORD)N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];

		if (temp.W < 0)
			N64Regs->RSPAccum[iEL].HW[3] = -1;
		else
			N64Regs->RSPAccum[iEL].HW[3] = 0;

		N64Regs->RSPAccum[iEL].HW[2] = temp.HW[1];
		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];
		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMUDN ( VMUDN Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMUDN ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.UW = (DWORD)N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] * (DWORD)(long)( N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] );

		if( temp.W < 0 )
			N64Regs->RSPAccum[iEL].HW[3] = -1;
		else
			N64Regs->RSPAccum[iEL].HW[3] = 0;

		N64Regs->RSPAccum[iEL].HW[2] = temp.HW[1];
		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];
		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMUDH ( VMUDH Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMUDH ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPAccum[iEL].W[1] = (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] * (long)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL]; 
		N64Regs->RSPAccum[iEL].HW[1] = 0;

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 ) 
		{
			if (N64Regs->RSPAccum[iEL].UHW[3] != 0xFFFF) 
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
			else
			{
				if( N64Regs->RSPAccum[iEL].HW[2] >= 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		} 
		else 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
			else 
			{
				if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMACF ( VMACF Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMACF ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.W = (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] * (long)(DWORD)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
		N64Regs->RSPAccum[iEL].DW += ( (long long)temp.W ) << 17;

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 ) 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0xFFFF )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
			else 
			{
				if( N64Regs->RSPAccum[iEL].HW[2] >= 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		} 
		else 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 ) 
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
			else
			{
				if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMACU ( VMACU Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMACU ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp, temp2;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.W = (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] * (long)(DWORD)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
		N64Regs->RSPAccum[iEL].UHW[3] += (WORD)(temp.W >> 31);
		temp.UW = temp.UW << 1;
		temp2.UW = temp.UHW[0] + N64Regs->RSPAccum[iEL].UHW[1];
		N64Regs->RSPAccum[iEL].HW[1] = temp2.HW[0];
		temp2.UW = temp.UHW[1] + N64Regs->RSPAccum[iEL].UHW[2] + temp2.UHW[1];
		N64Regs->RSPAccum[iEL].HW[2] = temp2.HW[0];
		N64Regs->RSPAccum[iEL].HW[3] += temp2.HW[1];

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 )
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0;
		else
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 )
				N64Regs->RSPVect[RspInstrOpcode.sa].UHW[iEL] = 0xFFFF; 
			else
			{
				if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].UHW[iEL] = 0xFFFF; 
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMADL( VMADL Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMADL ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD uTemp, uTemp2;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		uTemp.UW = (DWORD)N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] * (DWORD)N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
		uTemp2.UW = uTemp.UHW[1] + N64Regs->RSPAccum[iEL].UHW[1];

		N64Regs->RSPAccum[iEL].HW[1] = uTemp2.HW[0];
		uTemp2.UW = N64Regs->RSPAccum[iEL].UHW[2] + uTemp2.UHW[1];

		N64Regs->RSPAccum[iEL].HW[2] = uTemp2.HW[0];
		N64Regs->RSPAccum[iEL].HW[3] += uTemp2.HW[1];

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 ) 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0xFFFF )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0;
			else
			{
				if( N64Regs->RSPAccum[iEL].HW[2] >= 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
			}
		} 
		else 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 ) 
				N64Regs->RSPVect[RspInstrOpcode.sa].UHW[iEL] = 0xFFFF; 
			else 
			{
				if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].UHW[iEL] = 0xFFFF; 
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMADM( VMADM Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMADM ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp, temp2;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.UW = (DWORD)( (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] ) * (DWORD)N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
		temp2.UW = temp.UHW[0] + N64Regs->RSPAccum[iEL].UHW[1];

		N64Regs->RSPAccum[iEL].HW[1] = temp2.HW[0];

		temp2.UW = temp.UHW[1] + N64Regs->RSPAccum[iEL].UHW[2] + temp2.UHW[1];

		N64Regs->RSPAccum[iEL].HW[2] = temp2.HW[0];
		N64Regs->RSPAccum[iEL].HW[3] += temp2.HW[1];

		if( temp.W < 0 )
			N64Regs->RSPAccum[iEL].HW[3] -= 1; 

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 ) 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0xFFFF )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
			else 
			{
				if( N64Regs->RSPAccum[iEL].HW[2] >= 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		} 
		else 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
			else 
			{
				if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMADN ( VMADN Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMADN ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp, temp2;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.UW = (DWORD)N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] * (DWORD)( (long)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] );
		temp2.UW = temp.UHW[0] + N64Regs->RSPAccum[iEL].UHW[1];

		N64Regs->RSPAccum[iEL].HW[1] = temp2.HW[0];

		temp2.UW = temp.UHW[1] + N64Regs->RSPAccum[iEL].UHW[2] + temp2.UHW[1];

		N64Regs->RSPAccum[iEL].HW[2] = temp2.HW[0];
		N64Regs->RSPAccum[iEL].HW[3] += temp2.HW[1];

		if( temp.W < 0 )
			N64Regs->RSPAccum[iEL].HW[3] -= 1; 

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 ) 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0xFFFF )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0;
			else
			{
				if( N64Regs->RSPAccum[iEL].HW[2] >= 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
			}
		} 
		else 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 )
				N64Regs->RSPVect[RspInstrOpcode.sa].UHW[iEL] = 0xFFFF;
			else 
			{
				if( N64Regs->RSPAccum[iEL].HW[2] < 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].UHW[iEL] = 0xFFFF;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMADH ( VMADH Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMADH ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPAccum[iEL].W[1] += (long)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] * (long)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL]; 

		if( N64Regs->RSPAccum[iEL].HW[3] < 0 ) 
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0xFFFF )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
			else
			{
				if( N64Regs->RSPAccum[iEL].HW[2] >= 0 )
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		} 
		else
		{
			if( N64Regs->RSPAccum[iEL].UHW[3] != 0 ) 
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
			else
			{
				if (N64Regs->RSPAccum[iEL].HW[2] < 0)
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
				else
					N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[2];
			}
		}
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VADD ( VADD Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VADD ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.W = (int)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] + (int)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] +
			( ( N64Regs->RSPFlags[0].UW >> ( 7 - iEL ) ) & 0x1 );

		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];

		if( ( temp.HW[0] & 0x8000 ) == 0 ) 
		{
			if( temp.HW[1] != 0 )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
			else
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = temp.HW[0];
		} 
		else 
		{
			if( temp.HW[1] != -1 )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
			else
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = temp.HW[0];
		}
	}

	N64Regs->RSPFlags[0].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VSUB ( VSUB Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VSUB ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.W = (int)N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] - (int)N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] +
			( ( N64Regs->RSPFlags[0].UW >> ( 7 - iEL ) ) & 0x1 );

		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];

		if( ( temp.HW[0] & 0x8000 ) == 0 )
		{
			if( temp.HW[1] != 0 )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = (WORD)0x8000;
			else
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = temp.HW[0];
		}
		else 
		{
			if( temp.HW[1] != -1 )
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = 0x7FFF;
			else
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = temp.HW[0];
		}
	}

	N64Regs->RSPFlags[0].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VADDC ( VADDC Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VADDC ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;

	N64Regs->RSPFlags[0].UW = 0;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		temp.UW = (int)N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] + (int)N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];
		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = temp.HW[0];

		if( temp.UW & 0xffff0000 )
			N64Regs->RSPFlags[0].UW |= ( 1 << ( 7 - iEL ) );
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VSUBC ( VSUBC Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VSUBC ( void )
{
	int iCount, iEL, iDEL;
	MIPS_WORD temp;
	
	N64Regs->RSPFlags[0].UW = 0x0;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];
        
		temp.UW = (int)N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] - (int)N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];

		N64Regs->RSPAccum[iEL].HW[1] = temp.HW[0];
		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = temp.HW[0];

		if( temp.HW[0] != 0 )
			N64Regs->RSPFlags[0].UW |= ( 0x1 << (15 - iEL) );
		
		if( temp.UW & 0xffff0000 )
			N64Regs->RSPFlags[0].UW |= ( 0x1 << (7 - iEL) );
	}
	
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VSAW ( VSAW Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VSAW ( void )
{
	switch( RspInstrOpcode.rs & 0xF ) 
	{
		case 8:
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[0] = N64Regs->RSPAccum[0].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[1] = N64Regs->RSPAccum[1].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[2] = N64Regs->RSPAccum[2].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[3] = N64Regs->RSPAccum[3].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[4] = N64Regs->RSPAccum[4].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[5] = N64Regs->RSPAccum[5].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[6] = N64Regs->RSPAccum[6].HW[3];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[7] = N64Regs->RSPAccum[7].HW[3];
			break;

		case 9:
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[0] = N64Regs->RSPAccum[0].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[1] = N64Regs->RSPAccum[1].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[2] = N64Regs->RSPAccum[2].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[3] = N64Regs->RSPAccum[3].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[4] = N64Regs->RSPAccum[4].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[5] = N64Regs->RSPAccum[5].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[6] = N64Regs->RSPAccum[6].HW[2];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[7] = N64Regs->RSPAccum[7].HW[2];
			break;

		case 10:
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[0] = N64Regs->RSPAccum[0].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[1] = N64Regs->RSPAccum[1].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[2] = N64Regs->RSPAccum[2].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[3] = N64Regs->RSPAccum[3].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[4] = N64Regs->RSPAccum[4].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[5] = N64Regs->RSPAccum[5].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[6] = N64Regs->RSPAccum[6].HW[1];
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[7] = N64Regs->RSPAccum[7].HW[1];
			break;

		default:
			N64Regs->RSPVect[RspInstrOpcode.sa].DW[1] = 0;
			N64Regs->RSPVect[RspInstrOpcode.sa].DW[0] = 0;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VLT ( VLT Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VLT ( void )
{
	int iCount, iEL, iDEL;
	
	N64Regs->RSPFlags[1].UW = 0;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] < N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) 
		{
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL];
			N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL) );
		} 
		else if ( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] != N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) 
		{
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
			N64Regs->RSPFlags[1].UW &= ~( 1 << (7 - iEL) );
		} 
		else 
		{
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL];

			if( ( N64Regs->RSPFlags[0].UW & ( 0x101 << ( 7 - iEL ) ) ) == (WORD)( 0x101 << ( 7 - iEL ) ) )
				N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL) );
			else
				N64Regs->RSPFlags[1].UW &= ~( 1 << (7 - iEL) );
		}

		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];			
	}

	N64Regs->RSPFlags[0].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VEQ ( VEQ Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VEQ ( void )
{
	int iCount, iEL, iDEL;
	
	N64Regs->RSPFlags[1].UW = 0;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

        N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];

		if( N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] == N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL] ) 
		{
			if( ( N64Regs->RSPFlags[0].UW & ( 1 << ( 15 - iEL ) ) ) == 0 )
				N64Regs->RSPFlags[1].UW |= ( 1 << ( 7 - iEL ) );
		}
	}

	N64Regs->RSPFlags[0].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VNE ( VEQ Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VNE ( void )
{
	int iCount, iEL, iDEL;
	
	N64Regs->RSPFlags[1].UW = 0;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

        N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];

		if( N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] != N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL] ) 
		{
			N64Regs->RSPFlags[1].UW |= ( 1 << ( 7 - iEL ) );
		}
		else
		{
			if( ( N64Regs->RSPFlags[0].UW & ( 1 << ( 15 - iEL ) ) ) != 0 )
				N64Regs->RSPFlags[1].UW |= ( 1 << ( 7 - iEL ) );
		}
	}

	N64Regs->RSPFlags[0].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VGE ( VGE Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VGE ( void )
{
	int iCount, iEL, iDEL;

	N64Regs->RSPFlags[1].UW = 0;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] == N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) 
		{
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL];

			if( ( N64Regs->RSPFlags[0].UW & ( 0x101 << ( 7 - iEL ) ) ) == (WORD)( 0x101 << ( 7 - iEL ) ) )
				N64Regs->RSPFlags[1].UW &= ~( 1 << (7 - iEL) );
			else
				N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL) );

		} 
		else if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] > N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) 
		{
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL];
			N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL) );
		} 
		else 
		{
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
			N64Regs->RSPFlags[1].UW &= ~( 1 << (7 - iEL) );
		}

		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];			
	}

	N64Regs->RSPFlags[0].UW = 0;
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VCL ( VCL Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VCL ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0;iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		if ((N64Regs->RSPFlags[0].UW & ( 1 << (7 - iEL))) != 0 ) 
		{
			if ((N64Regs->RSPFlags[0].UW & ( 1 << (15 - iEL))) != 0 ) 
			{
				if ((N64Regs->RSPFlags[1].UW & ( 1 << (7 - iEL))) != 0 ) {
					N64Regs->RSPAccum[iEL].HW[1] = -N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
				} else {
					N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
				}
			} 
			else 
			{
				if ((N64Regs->RSPFlags[2].UW & ( 1 << (7 - iEL)))) 
				{
					if ( N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] + N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL] > 0x10000) 
					{
						N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
						N64Regs->RSPFlags[1].UW &= ~(1 << (7 - iEL));
					} 
					else 
					{
						N64Regs->RSPAccum[iEL].HW[1] = -N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
						N64Regs->RSPFlags[1].UW |= (1 << (7 - iEL));
					}
				} 
				else 
				{
					if (N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL] + N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] != 0) 
					{
						N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
						N64Regs->RSPFlags[1].UW &= ~(1 << (7 - iEL));
					} 
					else 
					{
						N64Regs->RSPAccum[iEL].HW[1] = -N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
						N64Regs->RSPFlags[1].UW |= (1 << (7 - iEL));
					}
				}
			}
		}
		else 
		{
			if ((N64Regs->RSPFlags[0].UW & ( 1 << (15 - iEL))) != 0 ) 
			{
				if ((N64Regs->RSPFlags[1].UW & ( 1 << (15 - iEL))) != 0 )
					N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
				else
					N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
			} 
			else 
			{			
				if ( N64Regs->RSPVect[RspInstrOpcode.rd].UHW[iEL] - N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL] >= 0) 
				{
					N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
					N64Regs->RSPFlags[1].UW |= (1 << (15 - iEL));
				} 
				else 
				{
					N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
					N64Regs->RSPFlags[1].UW &= ~(1 << (15 - iEL));
				}				
			}
		}

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
	}

	N64Regs->RSPFlags[0].UW = 0;
	N64Regs->RSPFlags[2].UW = 0;

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VCH ( VCH Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VCH ( void )
{
	int iCount, iEL, iDEL;

	N64Regs->RSPFlags[0].UW = 0;
	N64Regs->RSPFlags[1].UW = 0;
	N64Regs->RSPFlags[2].UW = 0;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];
						
		if( ( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] ^ N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) < 0 ) 
		{
			N64Regs->RSPFlags[0].UW |= ( 1 << (7 - iEL));

			if( N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] < 0 )
				N64Regs->RSPFlags[1].UW |= ( 1 << (15 - iEL));

			if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] + N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] <= 0 )
			{
				if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] + N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] == -1 )
					N64Regs->RSPFlags[2].UW |= ( 1 << (7 - iEL));

				N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL));
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = -N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
			} 
			else 
			{
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
			}

			if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] + N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] != 0 ) 
			{
				if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] != ~N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] )
					N64Regs->RSPFlags[0].UW |= ( 1 << (15 - iEL));
			}
		} 
		else 
		{
			if( N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] < 0 )
				N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL));

			if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] - N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] >= 0 )
			{
				N64Regs->RSPFlags[1].UW |= ( 1 << (15 - iEL));
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
			} 
			else 
			{
				N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
			}

			if (N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] - N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] != 0)
			{
				if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] != ~N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] )
					N64Regs->RSPFlags[0].UW |= ( 1 << (15 - iEL));
			}
		}

		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VCR ( VCR Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VCR ( void )
{
	int iCount, iEL, iDEL;

	N64Regs->RSPFlags[0].UW = 0;
	N64Regs->RSPFlags[1].UW = 0;
	N64Regs->RSPFlags[2].UW = 0;

	for( iCount = 0;iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];
		
		if( ( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] ^ N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] ) < 0 ) 
		{
			if( N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] < 0 )
				N64Regs->RSPFlags[1].UW |= ( 1 << ( 15 - iEL ) );

			if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] + N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] <= 0 )
			{
				N64Regs->RSPAccum[iEL].HW[1] = ~N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
				N64Regs->RSPFlags[1].UW |= ( 1 << ( 7 - iEL ) );
			} 
			else 
			{
				N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
			}
		} 
		else 
		{
			if( N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] < 0 )
				N64Regs->RSPFlags[1].UW |= ( 1 << (7 - iEL));

			if( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] - N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] >= 0 ) 
			{
				N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[iDEL];
				N64Regs->RSPFlags[1].UW |= ( 1 << (15 - iEL));
			} 
			else 
			{
				N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
			}
		}

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPAccum[iEL].HW[1];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMRG ( VMRG Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMRG ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		if( ( N64Regs->RSPFlags[1].UW & ( 1 << ( 7 - iEL ) ) ) != 0 )
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL];
		else
			N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VAND ( VAND Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VAND ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] & N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VNAND ( VNAND Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VNAND ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = 7 - iCount;
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = ~( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] & N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] );
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VOR ( VOR Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VOR ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] | N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VNOR ( VNOR Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VNOR ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ )
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = ~( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] | N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] );
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VXOR ( VXOR Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VXOR ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] ^ N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL];
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VNXOR ( VNXOR Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VNXOR ( void )
{
	int iCount, iEL, iDEL;

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		iEL = N64Regs->RSPIndx[RspInstrOpcode.rs].B[iCount];
		iDEL = N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iEL];

		N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL] = ~( N64Regs->RSPVect[RspInstrOpcode.rd].HW[iEL] ^ N64Regs->RSPVect[RspInstrOpcode.rt].HW[iDEL] );
		N64Regs->RSPAccum[iEL].HW[1] = N64Regs->RSPVect[RspInstrOpcode.sa].HW[iEL];
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VRCP ( VRCP Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VRCP ( void )
{
	int iCount, iNeg;

	N64Regs->RSPRecpResult.W = N64Regs->RSPVect[RspInstrOpcode.rt].HW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[RspInstrOpcode.rd & 0x7] ];

	if( N64Regs->RSPRecpResult.UW == 0 )
		N64Regs->RSPRecpResult.UW = 0x7FFFFFFF;
	else
	{
		if( N64Regs->RSPRecpResult.W < 0 ) 
		{
			iNeg = 1;
			N64Regs->RSPRecpResult.W = ~N64Regs->RSPRecpResult.W + 1;
		} else
			iNeg = 0;

		for( iCount = 15; iCount > 0; iCount-- ) 
		{
			if( N64Regs->RSPRecpResult.W & ( 1 << iCount ) ) 
			{
				N64Regs->RSPRecpResult.W &= ( 0xFFC0 >> ( 15 - iCount ) );
				iCount = 0;
			}
		}

		N64Regs->RSPRecpResult.W = (long)( 0x7FFFFFFF / (double)N64Regs->RSPRecpResult.W );

		for( iCount = 31; iCount > 0; iCount-- ) 
		{
			if( N64Regs->RSPRecpResult.W & ( 1 << iCount ) ) 
			{
				N64Regs->RSPRecpResult.W &= ( 0xFFFF8000 >> ( 31 - iCount ) );
				iCount = 0;
			}
		}

		if( iNeg == 1 )
			N64Regs->RSPRecpResult.W = ~N64Regs->RSPRecpResult.W;
	}

	for( iCount = 0; iCount < 8; iCount++ )
		N64Regs->RSPAccum[iCount].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iCount] ];

	N64Regs->RSPVect[RspInstrOpcode.sa].HW[7 - ( RspInstrOpcode.rd & 0x7 )] = N64Regs->RSPRecpResult.UHW[0];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VRCPL ( VRCPL Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VRCPL ( void )
{
	int iCount, iNeg;

	N64Regs->RSPRecpResult.UW = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[RspInstrOpcode.rd & 0x7] ] | N64Regs->RSPRecp.W;

	if( N64Regs->RSPRecpResult.UW == 0 )
		N64Regs->RSPRecpResult.UW = 0x7FFFFFFF;
	else
	{
		if( N64Regs->RSPRecpResult.W < 0 ) 
		{
			iNeg = 1;
			if( ( N64Regs->RSPRecpResult.UHW[1] == 0xFFFF ) && ( N64Regs->RSPRecpResult.HW[0] < 0 ) )
				N64Regs->RSPRecpResult.W = ~N64Regs->RSPRecpResult.W + 1;
			else
				N64Regs->RSPRecpResult.W = ~N64Regs->RSPRecpResult.W;
		} 
		else
			iNeg = 0;

		for( iCount = 31; iCount > 0; iCount-- )
		{
			if( ( N64Regs->RSPRecpResult.W & ( 1 << iCount ) ) ) 
			{
				N64Regs->RSPRecpResult.W &= ( 0xFFC00000 >> ( 31 - iCount ) );
				iCount = 0;
			}
		}

		N64Regs->RSPRecpResult.W = 0x7FFFFFFF / N64Regs->RSPRecpResult.W;

		for( iCount = 31; iCount > 0; iCount-- )
		{
			if( ( N64Regs->RSPRecpResult.W & ( 1 << iCount ) ) ) 
			{
				N64Regs->RSPRecpResult.W &= ( 0xFFFF8000 >> ( 31 - iCount ) );
				iCount = 0;
			}
		}

		if( iNeg == 1 )
			N64Regs->RSPRecpResult.W = ~N64Regs->RSPRecpResult.W;
	}

	for( iCount = 0; iCount < 8; iCount++ )
	{
		N64Regs->RSPAccum[iCount].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iCount] ];
	}

	N64Regs->RSPVect[RspInstrOpcode.sa].HW[7 - (RspInstrOpcode.rd & 0x7)] = N64Regs->RSPRecpResult.UHW[0];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VRCPH ( VRCPH Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VRCPH ( void )
{
	int iCount;

	N64Regs->RSPRecp.UHW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[ RspInstrOpcode.rd & 0x7 ] ];

	for( iCount = 0; iCount < 8; iCount++ ) 
	{
		N64Regs->RSPAccum[iCount].HW[1] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[iCount] ];
	}

	N64Regs->RSPVect[RspInstrOpcode.sa].UHW[7 - ( RspInstrOpcode.rd & 0x7 )] = N64Regs->RSPRecpResult.UHW[1];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: VMOV ( VMOV Vector Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_VMOV ( void )
{
	N64Regs->RSPVect[RspInstrOpcode.sa].UHW[7 - ( RspInstrOpcode.rd & 0x7 )] = N64Regs->RSPVect[RspInstrOpcode.rt].UHW[ N64Regs->RSPEleSpec[RspInstrOpcode.rs].B[RspInstrOpcode.rd & 0x7] ];
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LBV ( LBV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LBV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset ) ) & 0xFFF );

	N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - RspInstrOpcode.del] = *( g_pDMem + ( ( Address ) & 0xFFF ) );

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LSV ( LSV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LSV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 1 ) ) & 0xFFF );
	int	iLength, iCount;

	iLength = 2;
	if (iLength > 16 - RspInstrOpcode.del)
		iLength = 16 - RspInstrOpcode.del;

	for (iCount = RspInstrOpcode.del; iCount < ( iLength + RspInstrOpcode.del ); iCount++ )
	{
		N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - iCount] = *( g_pDMem + ( ( Address ) & 0xFFF ) );
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LLV ( LLV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LLV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 2 ) ) & 0xFFF );
	int	iLength, iCount;

	iLength = 4;
	if (iLength > 16 - RspInstrOpcode.del)
		iLength = 16 - RspInstrOpcode.del;

	for (iCount = RspInstrOpcode.del; iCount < ( iLength + RspInstrOpcode.del ); iCount++ )
	{
		N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - iCount] = *( g_pDMem + ( ( Address ) & 0xFFF ) );
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LDV ( LDV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LDV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 3 ) ) & 0xFFF );
	int	iLength, iCount;

	iLength = 8;
	if (iLength > 16 - RspInstrOpcode.del)
		iLength = 16 - RspInstrOpcode.del;

	for (iCount = RspInstrOpcode.del; iCount < ( iLength + RspInstrOpcode.del ); iCount++ )
	{
		N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - iCount] = *( g_pDMem + ( ( Address ) & 0xFFF ) );
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LQV ( LQV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LQV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );
	int	iLength, iCount;

	iLength = ( ( Address + 0x10 ) & ~0xF ) - Address;

	if( iLength > 16 - RspInstrOpcode.del )
		iLength = 16 - RspInstrOpcode.del;

	for( iCount = RspInstrOpcode.del; iCount < ( iLength + RspInstrOpcode.del ); iCount++ )
	{
		N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - iCount] = *( g_pDMem + ( ( Address ) & 0xFFF ) );
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LRV ( LRV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LRV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );
	int	iLength, iCount, iOffset;

	iOffset = (Address & 0xF) - 1;
	iLength = (Address & 0xF) - RspInstrOpcode.del;
	Address &= 0xFF0;

	for( iCount = RspInstrOpcode.del; iCount < (iLength + RspInstrOpcode.del); iCount++ )
	{
		N64Regs->RSPVect[RspInstrOpcode.rt].B[iOffset - iCount] = *( g_pDMem + ( ( Address ) & 0xFFF ) );
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}
short ByteSwap16Ret( short in )
{
	short uRet;

	((char *)&uRet)[0] = ((char*)&in)[1];
	((char *)&uRet)[1] = ((char*)&in)[0];

	return uRet;
}

// Instruction: LC2 LPV ( LPV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LPV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 3 ) ) & 0xFFF );

	N64Regs->RSPVect[RspInstrOpcode.rt].HW[7] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[6] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 1) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[5] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 2) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[4] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 3) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[3] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 4) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[2] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 5) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[1] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 6) & 0xF) ) & 0xFFF)) << 8 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[0] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 7) & 0xF) ) & 0xFFF)) << 8 );

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LUV ( LUV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LUV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 3 ) ) & 0xFFF );

	N64Regs->RSPVect[RspInstrOpcode.rt].HW[7] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[6] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 1) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[5] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 2) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[4] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 3) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[3] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 4) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[2] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 5) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[1] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 6) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[0] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 7) & 0xF)) & 0xFFF)) << 7 );

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LHV ( LHV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LHV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );

	N64Regs->RSPVect[RspInstrOpcode.rt].HW[7] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[6] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 2) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[5] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 4) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[4] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 6) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[3] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 8) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[2] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 10) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[1] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 12) & 0xF)) & 0xFFF)) << 7 );
	N64Regs->RSPVect[RspInstrOpcode.rt].HW[0] = ByteSwap16Ret( *(g_pDMem + ((Address + ((0x10 - RspInstrOpcode.del + 14) & 0xF)) & 0xFFF)) << 7 );

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LFV ( LFV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LFV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );
	MIPS_VECTOR Temp;
	int iCount, iLength = 8;
	
	if( iLength > 16 - RspInstrOpcode.del )
		iLength = 16 - RspInstrOpcode.del;
	
	Temp.HW[7] = ByteSwap16Ret( *(g_pDMem + (((Address + RspInstrOpcode.del)) & 0xFFF)) << 7 );
	Temp.HW[6] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x04 - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	Temp.HW[5] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x08 - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	Temp.HW[4] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x0C - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	Temp.HW[3] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x08 - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	Temp.HW[2] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x0C - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	Temp.HW[1] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x10 - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	Temp.HW[0] = ByteSwap16Ret( *(g_pDMem + (((Address + ((0x04 - RspInstrOpcode.del)) & 0xf)) & 0xFFF)) << 7 );
	
	for( iCount = RspInstrOpcode.del; iCount < (iLength + RspInstrOpcode.del); iCount++ )
		N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - iCount] = Temp.B[15 - iCount];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: LC2 LTV ( LTV LC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_LTV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );
	int iDel, iCount, iLength;
	
	iLength = 8;
	if (iLength > 32 - RspInstrOpcode.rt) {
		iLength = 32 - RspInstrOpcode.rt;
	}
	
	Address = ((Address + 8) & 0xFF0) + (RspInstrOpcode.del & 0x1);	

	for( iCount = 0; iCount < iLength; iCount++ ) 
	{
		iDel = ((8 - (RspInstrOpcode.del >> 1) + iCount) << 1) & 0xF;
		N64Regs->RSPVect[RspInstrOpcode.rt + iCount].B[15 - iDel] = *(g_pDMem + (Address));
		N64Regs->RSPVect[RspInstrOpcode.rt + iCount].B[14 - iDel] = *(g_pDMem + ((Address + 1)));
		Address += 2;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SC2 SBV ( SBV SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SBV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset ) ) & 0xFFF );
	
	*(g_pDMem + ( ( Address ) & 0xFFF ) ) = N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - RspInstrOpcode.del];

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SC2 SSV ( SSV SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SSV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 1 ) ) & 0xFFF );
	int iCount;

	for( iCount = RspInstrOpcode.del; iCount < (2 + RspInstrOpcode.del); iCount++ )
	{
		*(g_pDMem + ( ( Address ) & 0xFFF ) ) = N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - (iCount & 0xF)];
		Address += 1;
	}
	
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SC2 SLV ( SLV SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SLV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 2 ) ) & 0xFFF );
	int iCount;

	for( iCount = RspInstrOpcode.del; iCount < (4 + RspInstrOpcode.del); iCount++ )
	{
		*(g_pDMem + ( ( Address ) & 0xFFF ) ) = N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - (iCount & 0xF)];
		Address += 1;
	}
	
	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SC2 SDV ( SDV SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SDV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 3 ) ) & 0xFFF );
	int	iCount;

	for( iCount = RspInstrOpcode.del; iCount < (8 + RspInstrOpcode.del); iCount ++ )
	{
		*(g_pDMem + ( ( Address ) & 0xFFF ) ) = N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - (iCount & 0xF)];
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SC2 SQV ( SQV SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SQV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );
	int	iLength, iCount;
	
	iLength = ( ( Address + 0x10 ) & ~0xF ) - Address;

	for( iCount = RspInstrOpcode.del; iCount < (iLength + RspInstrOpcode.del); iCount++ )
	{
		*(g_pDMem + ( ( Address ) & 0xFFF ) ) = N64Regs->RSPVect[RspInstrOpcode.rt].B[15 - (iCount & 0xF)];
		Address += 1;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Instruction: SC2 SFV ( SFV SC2 Rsp Instruction )
//////////////////////////////////////////////////////////////////////////
void RspOp_SFV ( void )
{
	DWORD Address = ( ( N64Regs->RSP[RspInstrOpcode.base].UW + (DWORD)( RspInstrOpcode.voffset << 4 ) ) & 0xFFF );
	int offset = Address & 0xF;
	Address &= 0xFF0;

	switch( RspInstrOpcode.del ) 
	{
	case 0:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[7] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[6] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[5] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[4] >> 7 );
		break;
	case 1:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[1] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[0] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[3] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[2] >> 7 );
		break;
	case 2:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 3:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 4:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[6] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[5] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[4] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[7] >> 7 );
		break;
	case 5:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[0] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[3] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[2] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[1] >> 7 );
		break;
	case 6:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 7:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 8:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[3] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[2] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[1] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[0] >> 7 );
		break;
	case 9:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 10:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 11:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[4] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[7] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[6] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[5] >> 7 );
		break;
	case 12:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[2] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[1] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[0] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[3] >> 7 );
		break;
	case 13:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 14:
		*(g_pDMem + ((Address + offset))) = 0;
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = 0;
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = 0;
		break;
	case 15:
		*(g_pDMem + ((Address + offset))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[7] >> 7 );
		*(g_pDMem + ((Address + ((offset + 4) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[6] >> 7 );
		*(g_pDMem + ((Address + ((offset + 8) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[5] >> 7 );
		*(g_pDMem + ((Address + ((offset + 12) & 0xF)))) = ByteSwap16Ret( N64Regs->RSPVect[RspInstrOpcode.rt].UHW[4] >> 7 );
		break;
	}

	*N64Regs->RspPC = ( *N64Regs->RspPC + 4 ) & 0xFFC;
}

// Run Rsp Cycles
//////////////////////////////////////////////////////////////////////////
void RunRspInstruction( void )
{
	//* Fetch Opcode
	RspInstrOpcode.Hex = LoadDWordIMem( *N64Regs->RspPC );

	//* Execute Opcode
	OpcodeSet_RspCore[RspInstrOpcode.op]();

	//* Clear Reg 0
	N64Regs->RSP[0].UW = 0;
}

// Run Rsp LLE
//////////////////////////////////////////////////////////////////////////
void RunRspLLE( void )
{
	iRspRunning = 1;

	while( iRspRunning ) 
	{
		//* Fetch Opcode
		RspInstrOpcode.Hex = LoadDWordIMem( *N64Regs->RspPC );

//		pspDebugScreenSetXY(0,0);
//		printf( "RSP: 0x%08x  ", (unsigned int)*N64Regs->RspPC );
//		if( RspInstrOpcode.op == 0 )
//			printf( "EO: %d     ", (unsigned int)RspInstrOpcode.funct );
//		else if( RspInstrOpcode.op == 16 )
//			printf( "C0: %d %d  ", (unsigned int)RspInstrOpcode.rs, (unsigned int)RspInstrOpcode.rd );	
//		else 
//			printf( "OP: %d     ", (unsigned int)RspInstrOpcode.op );

		//* Execute Opcode
		OpcodeSet_RspCore[RspInstrOpcode.op]();

		//* Clear Reg 0
		N64Regs->RSP[0].UW = 0;
	}

	//* Currently Ignore due to Break not incrementing
	//*N64Regs->RspPC -= 4;
}
