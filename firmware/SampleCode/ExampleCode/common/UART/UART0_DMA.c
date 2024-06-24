/**************************************************************************//**
 * @file     uart0_dma.c
 * @version  V1.00
 * @brief    This uart used for print debug information.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/


#include "uart0_dma.h"
#include "CANapp.h"



/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

static uint8_t g_u8Tx_Buffer[UART0_BUFFER_SIZE];
static uint8_t g_u8Rx_Buffer[UART0_BUFFER_SIZE];


volatile uint8_t U0ErrorCode 	= 0;		//Error Code 
volatile uint8_t U0SendFlag 	= 0;		//1 frame data send complete flag
volatile uint8_t U0RecvFlag 	= 0;		//1 frame data receive complete flag
volatile uint16_t u0recv_bytes;				//receive data length

/**
 * @brief       UART0 IO Initialize 
 * @param[in]   
 * @return      
 * @details     
 */
void UART0_IO_Init(void)
{
 
		CLK_EnableModuleClock(UART0_MODULE);
	
    /* Select UART module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

	
#if  NK_M467HJ == 1
		SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();
#else		
    SET_UART0_RXD_PA15();
    SET_UART0_TXD_PA14();		
#endif
	
}

/**
 * @brief       UART0 Initialize 
 * @param[in]   
 * @return      
 * @details     
 */
void UART0_Init(void)
{
	
	UART_Open(UART0, 912600);											//Open uart and set baudrate
	
	UART0->FIFO &= ~UART_FIFO_RFITL_Msk;
	UART0->FIFO |= UART_FIFO_RFITL_14BYTES;				// Set Rx FIFO lenght as 14bytes
  UART_SetTimeoutCnt(UART0, UART0_RX_TIMEOUT); 	// Set Rx Time-out counter
	
	UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk|UART_INTEN_RXTOIEN_Msk));//open reveive & receive timeout interrupt
	
  NVIC_EnableIRQ(UART0_IRQn);
	
	UART0_PDMA_Init();
	U0SendFlag = 1;																//send complete ,can send  next data

}

/**
 * @brief       UART0_DMA_IRQHandler 
 * @param[in]   
 * @return      
 * @details     
 */
void UART0_PDMA_CallBack(uint32_t status)
{
		if (status & (1 << UART0_PDMA_TX_CH)) 
		{
				U0SendFlag = 1;													//send complete
				UART0->INTEN &= ~UART_INTEN_TXPDMAEN_Msk;
				PDMA_CLR_TD_FLAG(PDMA0,(1 << UART0_PDMA_TX_CH));
		}
}


/**
 * @brief       UART0_IRQHandler 
 * @param[in]   
 * @return      
 * @details     
 */
void UART0_IRQHandler(void)
{
    uint32_t u32IntSts = UART0->INTSTS;
		static uint8_t recv_count;

    if(u32IntSts & UART_INTSTS_HWRLSIF_Msk)
    {
        if(UART0->FIFOSTS & UART_FIFOSTS_BIF_Msk)  U0ErrorCode |= 0x01; //Break Interrupt Flag;
        if(UART0->FIFOSTS & UART_FIFOSTS_FEF_Msk)  U0ErrorCode |= 0x02; //Framing Error Flag
        if(UART0->FIFOSTS & UART_FIFOSTS_PEF_Msk)  U0ErrorCode |= 0x04; //Parity Error Flag 

        UART0->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk);
    }
		
		if((u32IntSts & UART_INTSTS_RDAINT_Msk)) 		//RX FIFO Receive Full Available Interrupt. 
    {
        /* Get all the input characters */
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
            /* Get the character from UART Buffer */
            g_u8Rx_Buffer[recv_count++] = UART_READ(UART0);
        }
    }
		
		if( (u32IntSts & UART_INTSTS_RXTOINT_Msk))	//Receiver Buffer Time-out Interrupt.
		{
			        /* Get all the input characters */
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {   
					/* Get the character from UART Buffer */
            g_u8Rx_Buffer[recv_count++] = UART_READ(UART0);				
        }
			  U0RecvFlag = 1;
				u0recv_bytes = recv_count;
				recv_count = 0;
		}
}



/**
 * @brief       UART0 PDMA Initialize 
 * @param[in]   
 * @return      
 * @details     
 */
void UART0_PDMA_Init(void)
{
		//Open DMA Channel for UART TX
    PDMA_Open(PDMA0,1 << UART0_PDMA_TX_CH); 
		//Set PDMA Transfer Address
    PDMA_SetTransferAddr(PDMA0,UART0_PDMA_TX_CH, ((uint32_t) (&g_u8Tx_Buffer[0])), PDMA_SAR_INC, UART0_BASE, PDMA_DAR_FIX);
    //Select Single Request
    PDMA_SetBurstType(PDMA0,UART0_PDMA_TX_CH, PDMA_REQ_SINGLE, 0);

		//Open DMA Interrupt
    PDMA_EnableInt(PDMA0,UART0_PDMA_TX_CH, PDMA_INT_TRANS_DONE);
    NVIC_EnableIRQ(PDMA0_IRQn);	
}



/**
 * @brief       Set CAN bus Status LED
 * @param[in]   sendbuf 		: send buf data point
 * @param[in]   data_length : how length data will be send
 * @return      
 * @details     Before Send start need check TX FIFO is empty
 */
void UART0_DMA_Send(uint8_t *sendbuf,uint16_t data_length)
{
	
		UART_WAIT_TX_EMPTY(UART0);						//Wait TX FIFO data empty
	  while(U0SendFlag == 0 );							//Wait DMA interrupt finish
	
	  if(data_length > UART0_BUFFER_SIZE) return ;
	
		memcpy(g_u8Tx_Buffer,sendbuf,data_length);

    // Select basic mode
    PDMA_SetTransferMode(PDMA0,UART0_PDMA_TX_CH, PDMA_UART0_TX, 0, 0);
    // Set data width and transfer count
    PDMA_SetTransferCnt(PDMA0,UART0_PDMA_TX_CH, PDMA_WIDTH_8, data_length);
		//Start DMA Transfer
	  UART_EnableInt(UART0, (UART_INTEN_TXPDMAEN_Msk));
		
		U0SendFlag=0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* uart0 dma printf                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdarg.h>
static char printf_buf[UART0_BUFFER_SIZE];
/**
 * @brief       Redefine printf 
 * @param[in]   
 * @return      The amount of data transferred
 * @details   	Same usage as printf
 */
int multi_printf(const char *str, ...)
{
    va_list aptr;
    int ret;


    va_start(aptr, str);
    ret = vsprintf(printf_buf, str, aptr);
    va_end(aptr);

    if(ret>0)
    {
        UART0_DMA_Send((uint8_t *)printf_buf,ret);
    }

    return ret;
}

