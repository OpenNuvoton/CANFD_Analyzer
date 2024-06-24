/**************************************************************************//**
 * @file			dma_iarq.c
 * @version  	V1.00
 * @brief
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "uart0_dma.h"


volatile uint8_t DMAErrorCode = 0;//error code


/**
 * @brief       PDMA Interrupt Handler
 * @param[in]
 * @return
 * @details
 */
void PDMA0_IRQHandler(void)
{
    uint32_t u32Status 	= PDMA0->TDSTS;			//Each channel completes the status register
    uint32_t status 		= PDMA0->INTSTS;		//DMA Controller state

    if (status & PDMA_INTSTS_TDIF_Msk)     /* pdam done */
    {   DMAErrorCode  = 0;
//-------------UART0 PDMA------------------//
        UART0_PDMA_CallBack(u32Status);

    }
    else  if (status & PDMA_INTSTS_ABTIF_Msk)   //target abort interrupt
    {
        if (PDMA_GET_ABORT_STS(PDMA0) & 0x4)
            DMAErrorCode = 2;
        PDMA_CLR_ABORT_FLAG(PDMA0,PDMA_GET_ABORT_STS(PDMA0));
    }
    else
    {
        DMAErrorCode  = 3;												//Other interrupt
    }
}
