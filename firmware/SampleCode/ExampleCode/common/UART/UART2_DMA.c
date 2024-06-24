/**************************************************************************//**
 * @file     
 * @version  V1.00
 * @brief    
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/


#include "uart2_dma.h"
#include "VCOMapp.h"

#define PDMA   PDMA0


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint8_t g_u8Tx_Buffer[UART2_BUFFER_SIZE];
static uint8_t g_u8Rx_Buffer[UART2_BUFFER_SIZE];


volatile uint8_t U2ErrorCode 	= 0;				//Error Code 
volatile uint8_t U2SendFlag 	= 0;				//1 frame data send complete flag
volatile uint8_t U2RecvFlag 	= 0;				//1 frame data receive complete flag
volatile uint16_t u2recv_bytes;						//receive data length



void UART2_IO_Init(void)
{
    /* Select IP clock source */
    CLK_EnableModuleClock(UART2_MODULE);
	
    CLK_SetModuleClock(UART2_MODULE, CLK_CLKSEL3_UART2SEL_HXT, CLK_CLKDIV4_UART2(1));
	
	  SET_UART2_RXD_PC4();
    SET_UART2_TXD_PC5(); 	
}


void UART2_Init(void)
{
	
	UART_Open(UART2, 9600);												//Open uart and set baudrate
	
	UART2->FIFO &= ~UART_FIFO_RFITL_Msk;					
	UART2->FIFO |= UART_FIFO_RFITL_14BYTES;				// Set Rx FIFO lenght as 14bytes
  UART_SetTimeoutCnt(UART2, UART2_RX_TIMEOUT); 	// Set Rx Time-out counter
	
	UART_EnableInt(UART2, (UART_INTEN_RDAIEN_Msk|UART_INTEN_RXTOIEN_Msk));//open reveive & receive timeout interrupt
	
  NVIC_EnableIRQ(UART2_IRQn);
	
	UART2_PDMA_Init();
	U2SendFlag = 1;																//send complete ,can send  next data

}


void UART2_PDMA_CallBack(uint32_t status)
{
		if (status & (1 << UART2_PDMA_TX_CH)) 
		{
				U2SendFlag = 1;												 	//send complete
				UART2->INTEN &= ~UART_INTEN_TXPDMAEN_Msk;
				PDMA_CLR_TD_FLAG(PDMA,(1 << UART2_PDMA_TX_CH));
		}
}



void UART2_IRQHandler(void)
{
    uint32_t u32IntSts = UART2->INTSTS;
		static uint8_t recv_count;

    if(u32IntSts & UART_INTSTS_HWRLSIF_Msk)
    {
        if(UART2->FIFOSTS & UART_FIFOSTS_BIF_Msk)  U2ErrorCode |= 0x01; //Break Interrupt Flag;
        if(UART2->FIFOSTS & UART_FIFOSTS_FEF_Msk)  U2ErrorCode |= 0x02; //Framing Error Flag
        if(UART2->FIFOSTS & UART_FIFOSTS_PEF_Msk)  U2ErrorCode |= 0x04; //Parity Error Flag 


        UART2->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk);
    }
		
		if((u32IntSts & UART_INTSTS_RDAINT_Msk))
    {
        /* Get all the input characters */
        while(UART_GET_RX_EMPTY(UART2) == 0)		//RX FIFO Receive Full Available Interrupt. 
        {
            /* Get the character from UART Buffer */
            g_u8Rx_Buffer[recv_count++] = UART_READ(UART2);
        }
    }
		
		if( (u32IntSts & UART_INTSTS_RXTOINT_Msk))	//Receiver Buffer Time-out Interrupt.
		{
			  /* Get all the input characters */
        while(UART_GET_RX_EMPTY(UART2) == 0)
        {   
					/* Get the character from UART Buffer */
            g_u8Rx_Buffer[recv_count++] = UART_READ(UART2);				
        }
				
			  U2RecvFlag = 1;
				u2recv_bytes = recv_count;
				recv_count = 0;
				Read_O2sensor();
		}
}




void UART2_PDMA_Init(void)
{
	
		//Open DMA Channel for UART TX
    PDMA_Open(PDMA,1 << UART2_PDMA_TX_CH); 
		//Set PDMA Transfer Address
    PDMA_SetTransferAddr(PDMA,UART2_PDMA_TX_CH, ((uint32_t) (&g_u8Tx_Buffer[0])), PDMA_SAR_INC, UART2_BASE, PDMA_DAR_FIX);
    //Select Single Request
    PDMA_SetBurstType(PDMA,UART2_PDMA_TX_CH, PDMA_REQ_SINGLE, 0);
	
		//Open DMA Interrupt
    PDMA_EnableInt(PDMA,UART2_PDMA_TX_CH, PDMA_INT_TRANS_DONE);
    NVIC_EnableIRQ(PDMA0_IRQn);
	
}




void UART2_DMA_Send(uint8_t *sendbuf,uint16_t data_lenght)
{
		UART_WAIT_TX_EMPTY(UART2);						//Wait TX FIFO data empty
	  while(U2SendFlag == 0 );							//Wait DMA interrupt finish
	
	  if(data_lenght > UART2_BUFFER_SIZE) return ;	
	
		memcpy(g_u8Tx_Buffer,sendbuf,data_lenght);

    // Select basic mode
    PDMA_SetTransferMode(PDMA,UART2_PDMA_TX_CH, PDMA_UART2_TX, 0, 0);
    // Set data width and transfer count
    PDMA_SetTransferCnt(PDMA,UART2_PDMA_TX_CH, PDMA_WIDTH_8, data_lenght);
		//Start DMA Transfer
	  UART_EnableInt(UART2, (UART_INTEN_TXPDMAEN_Msk));
}


int16_t O2sensor[3];
int16_t Read_O2sensor(void)
{
//0x16 0x09 0x01 0x01 0xF4 0x00 0x64 0x00 0xD2 0x00 0x00 0xB5
	static	uint8_t *RxBuffer = g_u8Rx_Buffer;	

		if(U2RecvFlag)
		{ U2RecvFlag=0;
			
			O2sensor[0] = RxBuffer[3];
			O2sensor[0] = (O2sensor[0]<<8 )| RxBuffer[4];
			
			O2sensor[1] = RxBuffer[5];
			O2sensor[1] = (O2sensor[1]<<8)|RxBuffer[6];
			
			O2sensor[2] = RxBuffer[7];
			O2sensor[2] = (O2sensor[2]<<8)|RxBuffer[8];

			return 1;
		}
		return 0;
}




