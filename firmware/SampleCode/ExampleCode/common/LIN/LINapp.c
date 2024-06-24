

#include "LINapp.h"

#define TX_FIFO_SIZE        16  /* TX Hardware FIFO size */
struct LIN_var_t lin_var;

volatile int32_t g_i32RxCounter[2];
static uint8_t LIN_mode[2] = {1,0};

//Only UART0 & UART1 Support Hardware LIN
UART_T * LIN_port[2]=
{
    UART0, UART1,
};

STR_VCOM_LINE_CODING gLINCoding[2] =
{   
		{19200, 0, 0, 8},   /* Baud rate : 115200    */
    {19200, 0, 0, 8},   /* Baud rate : 115200    */
};

/*---------------------------------------------------------------------------------------------------------*/
/* Compute Parity Value                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t GetParityValue(uint32_t u32id)
{
    uint32_t u32Res = 0, ID[6], p_Bit[2], mask = 0;

    for(mask = 0; mask < 6; mask++)
        ID[mask] = (u32id & (1 << mask)) >> mask;

    p_Bit[0] = (ID[0] + ID[1] + ID[2] + ID[4]) % 2;
    p_Bit[1] = (!((ID[1] + ID[3] + ID[4] + ID[5]) % 2));

    u32Res = u32id + (p_Bit[0] << 6) + (p_Bit[1] << 7);
    return (uint8_t)u32Res;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Compute CheckSum Value                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t ComputeChksumValue(uint8_t *pu8Buf, uint32_t u32ByteCnt)
{
    uint32_t i, CheckSum = 0;

    for(i = 0 ; i < u32ByteCnt; i++)
    {
        CheckSum += pu8Buf[i];
        if(CheckSum >= 256)
            CheckSum -= 255;
    }
    return (uint8_t)(255 - CheckSum);
}



/**
 *    @brief        Write LIN data
 *
 *    @param[in]    uart            The pointer of the specified UART module.
 *    @param[in]    pu8TxBuf        The buffer to send the data to UART transmission FIFO.
 *    @param[out]   u32WriteBytes   The byte number of data.
 *
 *    @return       u32Count transfer byte count
 *
 *    @details      The function is to write data into TX buffer to transmit data by UART.
 */
uint32_t LIN_Write(uint8_t ch, uint8_t id,uint8_t pu8TxBuf[], uint32_t u32WriteBytes)
{
    uint32_t u32TimeOutCnt;

    //The sample code will send a LIN header with ID is 0x30 and response field.
    //The response field with 8 data bytes and checksum without including ID.
	

    LIN_port[ch]->LINCTL &= ~UART_LINCTL_PID_Msk;
		LIN_port[ch]->LINCTL |= UART_LINCTL_PID(id) ;					//LIN_ID
	
//		LIN_port[ch]->LINCTL |= UART_LINCTL_MUTE_Msk;		
	
    /* LIN TX Send Header Enable */
    LIN_port[ch]->LINCTL |= UART_LINCTL_SENDH_Msk;

	
    /* Wait until break field, sync field and ID field transfer completed */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while((LIN_port[ch]->LINCTL & UART_LINCTL_SENDH_Msk) == UART_LINCTL_SENDH_Msk)
    {
        if(--u32TimeOutCnt == 0)
        {
//            printf("Wait for UART LIN header transfer completed time-out!\n");
            return 0;
        }
    }

    return UART_Write(LIN_port[ch], lin_var.buf, u32WriteBytes);
		
}

uint32_t LIN_Write_Header(uint8_t ch, uint8_t id)
{
    uint32_t u32TimeOutCnt;

    //The sample code will send a LIN header with ID is 0x30 and response field.
    //The response field with 8 data bytes and checksum without including ID.
	

    LIN_port[ch]->LINCTL &= ~UART_LINCTL_PID_Msk;
		LIN_port[ch]->LINCTL |= UART_LINCTL_PID(id) ;					//LIN_ID
	
//		LIN_port[ch]->LINCTL |= UART_LINCTL_MUTE_Msk;		
	
    /* LIN TX Send Header Enable */
    LIN_port[ch]->LINCTL |= UART_LINCTL_SENDH_Msk;

	
    /* Wait until break field, sync field and ID field transfer completed */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while((LIN_port[ch]->LINCTL & UART_LINCTL_SENDH_Msk) == UART_LINCTL_SENDH_Msk)
    {
        if(--u32TimeOutCnt == 0)
        {
//            printf("Wait for UART LIN header transfer completed time-out!\n");
            return 0;
        }
    }

    return 1;
		
}


void LIN_Init(uint8_t ch)
{
		if(ch == 0)
				SYS_ResetModule(UART0_RST);
		else
				SYS_ResetModule(UART1_RST);	

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(LIN_port[ch], gLINCoding[ch].u32DTERate);
		
    /* Set UART Configuration, LIN Max Speed is 20K */
    UART_SetLineConfig(LIN_port[ch], gLINCoding[ch].u32DTERate, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);

	    /* Switch back to LIN Function */
    LIN_port[ch]->FUNCSEL = UART_FUNCSEL_LIN;
	
    //master
    LIN_port[ch]->LINCTL |= UART_LINCTL_PID(LIN_ID) ;					//LIN_ID
		LIN_port[ch]->LINCTL |= UART_LINCTL_HSEL_BREAK_SYNC_ID ;	//break+sync+ID  
		LIN_port[ch]->LINCTL |= UART_LINCTL_BSL(1);								//LIN 间隔域/同步域分隔符长度
		LIN_port[ch]->LINCTL |= UART_LINCTL_BRKFL(12);						//LIN 间隔域长度
		LIN_port[ch]->LINCTL |= UART_LINCTL_IDPEN_Msk;						//LIN ID 校验使能位
//		LIN_port[ch]->LINCTL |= UART_LINCTL_BRKDETEN_Msk;				 //LIN 间隔域检测使能位

		//salve		
		if(LIN_mode[ch]==0)	
		{
		LIN_port[ch]->LINCTL |= UART_LINCTL_MUTE_Msk;							//LIN Mute 模式使能位
//		LIN_port[ch]->LINCTL |= UART_LINCTL_SLVDUEN_Msk；					//LIN 从机分频器更新方式使能位			
//		LIN_port[ch]->LINCTL |= UART_LINCTL_SLVAREN_Msk;						//LIN 从机自动重同步模式使能位
		LIN_port[ch]->LINCTL |= UART_LINCTL_SLVHDEN_Msk;						//LIN LIN 从机报头侦测使能位
		LIN_port[ch]->LINCTL |= UART_LINCTL_SLVEN_Msk;							//LIN 从模式使能位		
		}


			
	//设置FIFO触发级别,FIFO存满14个字节产生中断
		LIN_port[ch]->FIFO &= ~UART_FIFO_RFITL_Msk;
		LIN_port[ch]->FIFO |= UART_FIFO_RFITL_14BYTES;
	
	/* Set Timeout time RX_TIMEOUT bit-time */
  UART_SetTimeoutCnt(LIN_port[ch], 0x10); // Set Rx Time-out counter
			
		/* Enable RDA\Time-out\LIN Interrupt */
    UART_ENABLE_INT(LIN_port[ch], (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_LINIEN_Msk));
		
		if(ch == 0)
				NVIC_EnableIRQ(UART0_IRQn);
		else
				NVIC_EnableIRQ(UART1_IRQn);   
}



void UART0_IRQHandler(void)
{
		volatile uint32_t u32IntSts = UART0->INTSTS;
		uint8_t bInChar;
	  int32_t size;
		volatile struct vcom_var_t *vtp=&vcom_dat[0];

    if(u32IntSts & UART_INTSTS_LININT_Msk)
    {
        if(UART0->LINSTS & UART_LINSTS_SLVHDETF_Msk)
        {
            // Clear LIN slave header detection flag
            UART0->LINSTS = UART_LINSTS_SLVHDETF_Msk;
//            printf("\n LIN Slave Header detected ");
        }

        if(UART0->LINSTS & (UART_LINSTS_SLVHEF_Msk | UART_LINSTS_SLVIDPEF_Msk | UART_LINSTS_BITEF_Msk))
        {
            // Clear LIN error flag
            UART0->LINSTS = (UART_LINSTS_SLVHEF_Msk | UART_LINSTS_SLVIDPEF_Msk | UART_LINSTS_BITEF_Msk);
//            printf("\n LIN error detected ");
        }
    }

		
    if((u32IntSts & UART_INTSTS_RDAINT_Msk) || (u32IntSts & UART_INTSTS_RXTOINT_Msk))	
		{	
		        /* Get all the input characters */
        while((!UART_GET_RX_EMPTY(LIN_port[0])))
        {
            /* Get the character from UART Buffer */
            bInChar = UART_READ(LIN_port[0]);    /* Rx trigger level is 1 byte*/

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
		
		
		if(u32IntSts & UART_INTSTS_THREINT_Msk)
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
                UART_WRITE(LIN_port[0], bInChar);
                if(vtp->comThead >= TXBUFSIZE)
                    vtp->comThead = 0;
                vtp->comTbytes--;
                size--;
            } 
        }
        else
        {
            /* No more data, just stop Tx (Stop work) */
            LIN_port[0]->INTEN &= ~UART_INTEN_THREIEN_Msk;
        }
    }
}


void UART1_IRQHandler(void)
{
    volatile uint32_t u32IntSts = UART1->INTSTS;
		uint8_t bInChar;
		volatile struct vcom_var_t *vtp=&vcom_dat[0];

    if(u32IntSts & UART_INTSTS_LININT_Msk)
    {
        if(UART1->LINSTS & UART_LINSTS_SLVHDETF_Msk)
        {
            // Clear LIN slave header detection flag
            UART1->LINSTS = UART_LINSTS_SLVHDETF_Msk;
//            printf("\n LIN Slave Header detected ");
        }

        if(UART1->LINSTS & (UART_LINSTS_SLVHEF_Msk | UART_LINSTS_SLVIDPEF_Msk | UART_LINSTS_BITEF_Msk))
        {
            // Clear LIN error flag
            UART1->LINSTS = (UART_LINSTS_SLVHEF_Msk | UART_LINSTS_SLVIDPEF_Msk | UART_LINSTS_BITEF_Msk);
//            printf("\n LIN error detected ");
        }
    }

		        /* Get all the input characters */
        while((!UART_GET_RX_EMPTY(LIN_port[1])))
        {
            /* Get the character from UART Buffer */
            bInChar = UART_READ(LIN_port[1]);    /* Rx trigger level is 1 byte*/

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
