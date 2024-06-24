
#include <string.h>
#include "NuMicro.h"
#include "vcom_serial.h"
#include "LINapp.h"

#define TX_FIFO_SIZE        16  /* TX Hardware FIFO size */
#define NK_M467HJ						0
#define RS485								1

UART_T * uart_port[VCOM_CNT]=
{
#if (VCOM_CNT>=1)
    UART0,
#endif
#if (VCOM_CNT>=2)
    UART2,
#endif
#if (VCOM_CNT>=3)
    UART6,
#endif
#if (VCOM_CNT>=4)
    UART4,
#endif
};

/**
 *  @brief      UART Module Init
 *  @param[in]  None
 *  @return     None
 *  @details    The function is used to:
								1.Enable UART Module
								2.Enable UART Clock
								3.Set multi-function pins
*/
void UART_Module_Init(void)
{
#if (VCOM_CNT>=1)
//--------------UART0-LIN-----------------//
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Set multi-function pins for UART RXD and TXD */

//UART0 connet to nulink VCOM	
//    SET_UART0_RXD_PB12();
//    SET_UART0_TXD_PB13();
	
    SET_UART0_RXD_PA15();
    SET_UART0_TXD_PA14();

#endif
#if (VCOM_CNT>=2)
//-----------------UART2-------------------//
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART2_MODULE);
    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART2_MODULE, CLK_CLKSEL3_UART2SEL_HIRC, CLK_CLKDIV4_UART2(1));

    /* Set multi-function pins for UART RXD and TXD */
    SET_UART2_RXD_PC4();
    SET_UART2_TXD_PC5();

#endif
#if (VCOM_CNT>=3)
//-------------UART6-RS485--------------//
    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART6_MODULE);
    /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART6_MODULE, CLK_CLKSEL3_UART6SEL_HIRC, CLK_CLKDIV4_UART6(1));

    /* Set multi-function pins for UART RXD and TXD */
    SET_UART6_TXD_PA11();
    SET_UART6_RXD_PA10();

#endif
#if (VCOM_CNT>=4)
//-----------------UART4-------------------//
    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART4_MODULE);
    /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART4_MODULE, CLK_CLKSEL3_UART4SEL_HIRC, CLK_CLKDIV4_UART4(1));

    /* Set multi-function pins for UART RXD and TXD */

#if  NK_M467HJ == 1
		SET_UART4_RXD_PF6();
		SET_UART4_TXD_PF7();
#else
    SET_UART4_RXD_PA13();
    SET_UART4_TXD_PA12();
#endif		

		
#endif

}

/**
 *  @brief      Open used UART
 *  @param[in]  None
 *  @return     None
 *  @details    The function is used to:
								Enable UART function and set baud-rate.
								Enable UART specified interrupt.
*/
void UART_Init(void)
{   uint8_t cnt;
	
		LIN_Init(0); //UART0 use as LIN_port0
	
    for(cnt=1; cnt<VCOM_CNT; cnt++)
    {		
				/* Init UART to 115200-8n1 */
        UART_Open(uart_port[cnt], 115200);      
        /* Enable Interrupt and install the call back function */
        UART_ENABLE_INT(uart_port[cnt], (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/**
 *  @brief      UART Callback function
 *  @param[in]  UART COM Index：
  *             - \ref IDX_VCOM0
  *             - \ref IDX_VCOM1
  *             - \ref IDX_VCOM2
  *             - \ref IDX_VCOM3
 *  @return     None
 *  @details    Put it in the interrupt function corresponding to the UART
*/
void UART_IRQHandler(uint8_t com_sn)
{
    uint8_t bInChar;
    int32_t size;
    uint32_t u32IntStatus;
    volatile struct vcom_var_t *vtp=&vcom_dat[com_sn];

    u32IntStatus = uart_port[com_sn]->INTSTS;

    if((u32IntStatus & UART_INTSTS_RDAINT_Msk) || (u32IntStatus & UART_INTSTS_RXTOINT_Msk))
    {
        /* Receiver FIFO threshold level is reached or Rx time out */

        /* Get all the input characters */
        while((!UART_GET_RX_EMPTY(uart_port[com_sn])))
        {
            /* Get the character from UART Buffer */
            bInChar = UART_READ(uart_port[com_sn]);    /* Rx trigger level is 1 byte*/

            /* Check if buffer full */
            if(vtp->comRbytes < RXBUFSIZE)
            {
                /* Enqueue the character */
                vtp->comRbuf[vtp->comRtail++] = bInChar;
                if(vtp->comRtail >= RXBUFSIZE)
                    vtp->comRtail = 0;
                vtp->comRbytes++;
            }
            else
            {
                /* FIFO over run */
            }
        }
    }

    if(u32IntStatus & UART_INTSTS_THREINT_Msk)
    {

        if(vtp->comTbytes && (UART0->INTEN & UART_INTEN_THREIEN_Msk))
        {
            /* Fill the Tx FIFO */
            size = vtp->comTbytes;
            if(size >= TX_FIFO_SIZE)
            {
                size = TX_FIFO_SIZE;
            }

            while(size)
            {
                bInChar = vtp->comTbuf[vtp->comThead++];
                UART_WRITE(uart_port[com_sn], bInChar);
                if(vtp->comThead >= TXBUFSIZE)
                    vtp->comThead = 0;
                vtp->comTbytes--;
                size--;
            }
        }
        else
        {
            /* No more data, just stop Tx (Stop work) */
            uart_port[com_sn]->INTEN &= ~UART_INTEN_THREIEN_Msk;
        }
    }
}

/**
 *  @brief      UART & USB Receive Buffer Transmit  
 *  @param[in]  None
 *  @return     None
 *  @details    Put it in main Loop to receive and transmit uart data
*/
void VCOM_TransferData(void)
{
    int32_t i, i32Len,cnt;
    volatile struct vcom_var_t *vtp;
	


    for(cnt=0; cnt<VCOM_CNT; cnt++)
    {   
				vtp=&vcom_dat[cnt];

        /* Check if any data to send to USB & USB is ready to send them out */
        if(vtp->comRbytes && (vtp->gu32TxSize == 0))
        {
            i32Len = vtp->comRbytes;
            if(i32Len > EP_MAX_DATA_PKT_SIZE)
                i32Len = EP_MAX_DATA_PKT_SIZE;

            for(i = 0; i < i32Len; i++)
            {
                vtp->gRxBuf[i] = vtp->comRbuf[vtp->comRhead++];
                if(vtp->comRhead >= RXBUFSIZE)
                    vtp->comRhead = 0;
            }


            switch(cnt)
            {
#if (VCOM_CNT>=1)
            case IDX_VCOM0 :
                NVIC_DisableIRQ(UART0_IRQn);
                vtp->comRbytes -= i32Len;
                NVIC_EnableIRQ(UART0_IRQn);
                break;
#endif
#if (VCOM_CNT>=2)
            case IDX_VCOM1 :
                NVIC_DisableIRQ(UART2_IRQn);
                vtp->comRbytes -= i32Len;
                NVIC_EnableIRQ(UART2_IRQn);
                break;
#endif
#if (VCOM_CNT>=3)
            case IDX_VCOM2 :
                NVIC_DisableIRQ(UART6_IRQn);
                vtp->comRbytes -= i32Len;
                NVIC_EnableIRQ(UART6_IRQn);
                break;
#endif
#if (VCOM_CNT>=4)
            case IDX_VCOM3 :
                NVIC_DisableIRQ(UART4_IRQn);
                vtp->comRbytes -= i32Len;
                NVIC_EnableIRQ(UART4_IRQn);
                break;
#endif
            }

            vtp->gu32TxSize = i32Len;

            switch(cnt)
            {
#if (VCOM_CNT>=1)
            case IDX_VCOM0 :
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPA].EPDAT_BYTE = vtp->gRxBuf[i];
                HSUSBD->EP[EPA].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPA].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPA, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
#if (VCOM_CNT>=2)
            case IDX_VCOM1 :
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPD].EPDAT_BYTE = vtp->gRxBuf[i];
                HSUSBD->EP[EPD].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPD].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPD, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
#if (VCOM_CNT>=3)
            case IDX_VCOM2 :
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPG].EPDAT_BYTE = vtp->gRxBuf[i];
                HSUSBD->EP[EPG].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPG].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPG, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
#if (VCOM_CNT>=4)
            case IDX_VCOM3 :
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPJ].EPDAT_BYTE = vtp->gRxBuf[i];
                HSUSBD->EP[EPJ].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPJ].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPJ, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
            }
        }

        /* Process the Bulk out data when bulk out data is ready. */
        if(vtp->gi8BulkOutReady && (vtp->gu32RxSize <= TXBUFSIZE - vtp->comTbytes))
        {
            
					/* Process the software Tx FIFO */
					if(cnt == 0 )    //LIN 总线使用不同发送机制		
					{	
						if(vtp->gu32RxSize >1)	
						{						
							lin_var.bytes = vtp->gu32RxSize-1;//减去ID
							lin_var.id = vtp->gUsbRxBuf[0];//id
							
							/* Compute checksum without ID and fill checksum value to au8TestPattern[8] */
							lin_var.buf[lin_var.bytes] = ComputeChksumValue(vtp->gUsbRxBuf+1, lin_var.bytes);
							
							for(i = 0; i < lin_var.bytes+1; i++)
							{
									if(i<lin_var.bytes)  lin_var.buf[i] = vtp->gUsbRxBuf[i+1];//跳过ID和校验
									vtp->comTbuf[vtp->comTtail++] = lin_var.buf[i];//放入发送缓冲
									if(vtp->comTtail >= TXBUFSIZE)
											vtp->comTtail = 0;
							}						
							
							LIN_Write_Header(0, lin_var.id);	//发送LIN header
						}
		
					}
					else //非LIN口
						{
							for(i = 0; i < vtp->gu32RxSize; i++)
							{
									vtp->comTbuf[vtp->comTtail++] = vtp->gUsbRxBuf[i];
									if(vtp->comTtail >= TXBUFSIZE)
											vtp->comTtail = 0;
							}
						}


            switch(cnt)
            {
#if (VCOM_CNT>=1)
            case IDX_VCOM0 :
                NVIC_DisableIRQ(UART0_IRQn);
                vtp->comTbytes += vtp->gu32RxSize;
                NVIC_EnableIRQ(UART0_IRQn);
                break;
#endif
#if (VCOM_CNT>=2)
            case IDX_VCOM1 :
                NVIC_DisableIRQ(UART2_IRQn);
                vtp->comTbytes += vtp->gu32RxSize;
                NVIC_EnableIRQ(UART2_IRQn);
                break;
#endif
#if (VCOM_CNT>=3)
            case IDX_VCOM2 :
                NVIC_DisableIRQ(UART6_IRQn);
                vtp->comTbytes += vtp->gu32RxSize;
                NVIC_EnableIRQ(UART6_IRQn);
                break;
#endif
#if (VCOM_CNT>=4)
            case IDX_VCOM3 :
                NVIC_DisableIRQ(UART4_IRQn);
                vtp->comTbytes += vtp->gu32RxSize;
                NVIC_EnableIRQ(UART4_IRQn);
                break;
#endif
            }

            vtp->gu32RxSize = 0;
            vtp->gi8BulkOutReady = 0; /* Clear bulk out ready flag */
        }

        if(vtp->comTbytes)
        {
            /* Check if Tx is working */
            if((uart_port[cnt]->INTEN & UART_INTEN_THREIEN_Msk) == 0)
            {
                /* Send one bytes out */
                UART_WRITE(uart_port[cnt], vtp->comTbuf[vtp->comThead++]);
                if(vtp->comThead >= TXBUFSIZE)
                    vtp->comThead = 0;

                vtp->comTbytes--;

                /* Enable Tx Empty Interrupt. (Trigger first one) */
                uart_port[cnt]->INTEN |= UART_INTEN_THREIEN_Msk;
            }
        }
    }
}

#if (VCOM_CNT>=1)
//UART0 use as LIN  function in LINapp.c
//void UART0_IRQHandler(void)
//{
//    UART_IRQHandler(IDX_VCOM0);
//}
#endif
#if (VCOM_CNT>=2)
void UART2_IRQHandler(void)
{
    UART_IRQHandler(IDX_VCOM1);
}
#endif
#if (VCOM_CNT>=3)
void UART6_IRQHandler(void)
{
    UART_IRQHandler(IDX_VCOM2);
}
#endif
#if (VCOM_CNT>=4)
void UART4_IRQHandler(void)
{
    UART_IRQHandler(IDX_VCOM3);
}
#endif

/**
 *  @brief      USB VCOM set uart config
 *  @param[in]  UART COM Index：
  *             - \ref IDX_VCOM0
  *             - \ref IDX_VCOM1
  *             - \ref IDX_VCOM2
  *             - \ref IDX_VCOM3
 *  @return     None
 *  @details
*/
void VCOM_LineCoding(uint8_t port)
{
    uint32_t u32Reg;
    uint32_t u32Baud_Div;
    volatile struct vcom_var_t *vtp=&vcom_dat[port];

    switch(port)
    {
#if (VCOM_CNT>=1)
    case IDX_VCOM0 :
        NVIC_DisableIRQ(UART0_IRQn);
        break;
#endif
#if (VCOM_CNT>=2)
    case IDX_VCOM1 :
        NVIC_DisableIRQ(UART2_IRQn);
        break;
#endif
#if (VCOM_CNT>=3)
    case IDX_VCOM2 :
        NVIC_DisableIRQ(UART6_IRQn);
        break;
#endif
#if (VCOM_CNT>=4)
    case IDX_VCOM3 :
        NVIC_DisableIRQ(UART4_IRQn);
        break;
#endif
    }

    // Reset software fifo
    vtp->comRbytes = 0;
    vtp->comRhead = 0;
    vtp->comRtail = 0;

    vtp->comTbytes = 0;
    vtp->comThead = 0;
    vtp->comTtail = 0;

    // Reset hardware fifo
    uart_port[port]->FIFO = 0x3;

    // Set baudrate
    u32Baud_Div = UART_BAUD_MODE2_DIVIDER(__HIRC, gLineCoding[port].u32DTERate);

    if(u32Baud_Div > 0xFFFF)
        uart_port[port]->BAUD = (UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER(__HIRC, gLineCoding[port].u32DTERate));
    else
        uart_port[port]->BAUD = (UART_BAUD_MODE2 | u32Baud_Div);

    // Set parity
    if(gLineCoding[port].u8ParityType == 0)
        u32Reg = 0; 		// none parity
    else if(gLineCoding[port].u8ParityType == 1)
        u32Reg = 0x08; 	// odd parity
    else if(gLineCoding[port].u8ParityType == 2)
        u32Reg = 0x18; 	// even parity
    else
        u32Reg = 0;

    // bit width
    switch(gLineCoding[port].u8DataBits)
    {
    case 5:
        u32Reg |= 0;
        break;
    case 6:
        u32Reg |= 1;
        break;
    case 7:
        u32Reg |= 2;
        break;
    case 8:
        u32Reg |= 3;
        break;
    default:
        break;
    }

    // stop bit
    if(gLineCoding[port].u8CharFormat > 0)
        u32Reg |= 0x4; // 2 or 1.5 bits

    uart_port[port]->LINE = u32Reg;

    // Re-enable UART interrupt
    switch(port)
    {
#if (VCOM_CNT>=1)
    case IDX_VCOM0 :
        NVIC_EnableIRQ(UART0_IRQn);
        break;
#endif
#if (VCOM_CNT>=2)
    case IDX_VCOM1 :
        NVIC_EnableIRQ(UART2_IRQn);
        break;
#endif
#if (VCOM_CNT>=3)
    case IDX_VCOM2 :
        NVIC_EnableIRQ(UART6_IRQn);
        break;
#endif
#if (VCOM_CNT>=4)
    case IDX_VCOM3 :
        NVIC_EnableIRQ(UART4_IRQn);
        break;
#endif
    }
}
