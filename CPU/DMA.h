//////////////////////////////////////////////////////////////////////////
// Dma / Direct Memory Access
//////////////////////////////////////////////////////////////////////////
// Everything that has to deal with Dma Transfers

#ifndef _DMA_H_
#define _DMA_H_

// Include Fies
//////////////////////////////////////////////////////////////////////////
#include "../StdInc.h"

// Functions
//////////////////////////////////////////////////////////////////////////

//* Pi Dma Transfer
void DmaPiRead ( void );
void DmaPiWrite ( void );

//* Si Dma Transfer
void DmaSiRead ( void );
void DmaSiWrite ( void );

//* Sp Dma Transfer
void DmaSpRead ( void );
void DmaSpWrite ( void );

//* Rsp Sp Dma Transfer
void DmaSpReadRsp ( void );
void DmaSpWriteRsp ( void );

#endif
