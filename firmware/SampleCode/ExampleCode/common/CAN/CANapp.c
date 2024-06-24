/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    An example of interrupt control using CAN FD bus communication.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/


#include "CANapp.h"
#include "user_timer.h"



/*---------------------------------------------------------------------------------------------------------*/
/* Message RAM elements                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

#define SID_BUFF_SIZE  24     //24 elements  
#define XID_BUFF_SIZE  16			//16 elements  
#define TX_FIFO_SIZE   32			//32 elements  
#define RX_FIFO_SIZE	 50			//50 elements  

/*
 The maximum Message RAM size is 1536 words = 0x1800
 SID_BUFF_ADDR_OFFSET			= 0
 XID_BUFF_ADDR_OFFSET     = 24*4 							= 0x60
 TX_FIFO_ADDR_OFFSET 			= 0x60 	+ 16*8 		 	= 0xE0
 RX_FIFO_ADDR_OFFSET 			= 0xE0 	+ 32*(8+64) = 0x9E0
 END_ADD									= 0x9E0 + 50*(8+64) = 0x17F0 < 0x1800
 
 sizeof(SID elements) 		= 4    
 sizeof(XID elements) 		= 8 
 sizeof(TX FIFO elements)	= 8 + 64    
 sizeof(RX FIFO elements) = 8 + 64      
*/

#define SID_BUFF_ADDR_OFFSET			(0)
#define XID_BUFF_ADDR_OFFSET      (SID_BUFF_SIZE*4)
#define TX_FIFO_ADDR_OFFSET 			(XID_BUFF_ADDR_OFFSET + XID_BUFF_SIZE*8)
#define RX_FIFO_ADDR_OFFSET  			(TX_FIFO_ADDR_OFFSET  + TX_FIFO_SIZE*(sizeof(CANFD_BUF_T)))
	


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
CANFD_FD_T 					sCANFD_Config[CANFD_NUM];															//CANFD config information
CANFD_FD_MSG_T    	g_sRxMsgFrame[CANFD_NUM];    													//one RX message buffer
CANFD_FD_MSG_T    	g_sTxMsgFrame[CANFD_NUM];															//one TX message buffer
	
volatile uint8_t   g_u8RxFifo1CompleteFlag[CANFD_NUM] = {FALSE,FALSE,FALSE,FALSE};//when receive 1 message will be set TRUE
volatile uint32_t  msg_count[CANFD_NUM];																					//The number of messages received

static CANFD_T  *canfd[CANFD_NUM]={CANFD0,CANFD1,CANFD2,CANFD3};									//channel 0~3 map CANFD0 ~CANFD3

/**
 * @brief       CAN_Port_Init
 * @param[in]   
 * @return      
 * @details       
 */
void CAN_Port_Init(void)
{
			/* Select CAN FD clock source is HCLK */
	  CLK_SetModuleClock(CANFD0_MODULE, CLK_CLKSEL0_CANFD0SEL_HCLK, CLK_CLKDIV5_CANFD0(10));//200Mhz/10 = 20Mhz
    CLK_SetModuleClock(CANFD1_MODULE, CLK_CLKSEL0_CANFD1SEL_HCLK, CLK_CLKDIV5_CANFD1(10));
    CLK_SetModuleClock(CANFD2_MODULE, CLK_CLKSEL0_CANFD2SEL_HCLK, CLK_CLKDIV5_CANFD2(10));
    CLK_SetModuleClock(CANFD3_MODULE, CLK_CLKSEL0_CANFD3SEL_HCLK, CLK_CLKDIV5_CANFD3(10));
	
	    /* Enable CANFD module clock */	
    CLK_EnableModuleClock(CANFD0_MODULE);
    CLK_EnableModuleClock(CANFD1_MODULE);
    CLK_EnableModuleClock(CANFD2_MODULE);
    CLK_EnableModuleClock(CANFD3_MODULE);		


		#if  NK_M467HJ == 1
	    /* Set PJ multi-function pins for CAN FD0 RXD and TXD */	
			SET_CAN0_RXD_PJ11();
			SET_CAN0_TXD_PJ10();
		#else
    /* Set PB multi-function pins for CAN FD0 RXD and TXD */		
			SET_CAN0_RXD_PB10();
			SET_CAN0_TXD_PB11();
			
		#endif		
	
			/* Set PB multi-function pins for CAN FD1 RXD and TXD */
			SET_CAN1_RXD_PB6();
			SET_CAN1_TXD_PB7();

			/* Set PB multi-function pins for CAN FD2 RXD and TXD */
			SET_CAN2_RXD_PB8();
			SET_CAN2_TXD_PB9();

			/* Set PC multi-function pins for CAN FD3 RXD and TXD */
			SET_CAN3_RXD_PC6();
			SET_CAN3_TXD_PC7();

}	

/*---------------------------------------------------------------------------------------------------------*/
/*  ISR to handle CAN FD Line 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/

void CANFD00_IRQHandler(void)
{
 		msg_count[0]++;		
		/*Receive the Rx Fifo1 buffer */
    CANFD_ReadRxFifoMsg(CANFD0, 1, &g_sRxMsgFrame[0]);
    /*Clear the Interrupt flag */
    CANFD_ClearStatusFlag(CANFD0, CANFD_IR_TOO_Msk | CANFD_IR_RF1N_Msk);
    g_u8RxFifo1CompleteFlag[0] = 1;
}


void CANFD10_IRQHandler(void)
{
    msg_count[1]++;	
	  /*Receive the Rx Fifo1 buffer */
    CANFD_ReadRxFifoMsg(CANFD1, 1, &g_sRxMsgFrame[1]);
    /*Clear the Interrupt flag */
    CANFD_ClearStatusFlag(CANFD1, CANFD_IR_TOO_Msk | CANFD_IR_RF1N_Msk);
    g_u8RxFifo1CompleteFlag[1] = 1;
}


void CANFD20_IRQHandler(void)
{
 		msg_count[2]++;
  	/*Receive the Rx Fifo1 buffer */
    CANFD_ReadRxFifoMsg(CANFD2, 1, &g_sRxMsgFrame[2]);
    /*Clear the Interrupt flag */
    CANFD_ClearStatusFlag(CANFD2, CANFD_IR_TOO_Msk | CANFD_IR_RF1N_Msk);
    g_u8RxFifo1CompleteFlag[2] = 1;
}

void CANFD30_IRQHandler(void)
{
	  msg_count[3]++;	
	  /*Receive the Rx Fifo1 buffer */
    CANFD_ReadRxFifoMsg(CANFD3, 1, &g_sRxMsgFrame[3]);
    /*Clear the Interrupt flag */
    CANFD_ClearStatusFlag(CANFD3, CANFD_IR_TOO_Msk | CANFD_IR_RF1N_Msk);
    g_u8RxFifo1CompleteFlag[3] = 1;
	
}


/*---------------------------------------------------------------------------------------------------------*/
/*                                 Send CAN FD Message Function                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*Classic CAN, only support 1,2,3,4,5,6,7,8 bytes send,  
CANFD has 12, 16, 20, 24, 32, 48 and 64 bytes ,  
The important thing to note here is that CAN FD is not an arbitrary number of bytes from 1 to 64 
*/
static uint8_t CANFD_EncodeDLC(uint8_t u8NumberOfBytes)
{
    if (u8NumberOfBytes <= 8) return u8NumberOfBytes;
    else if (u8NumberOfBytes <= 12) return 9;
    else if (u8NumberOfBytes <= 16) return 10;
    else if (u8NumberOfBytes <= 20) return 11;
    else if (u8NumberOfBytes <= 24) return 12;
    else if (u8NumberOfBytes <= 32) return 13;
    else if (u8NumberOfBytes <= 48) return 14;
    else return 15;
}


/**
 * @brief       Messages will put into the send buffer
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   bufsn		: buffer serial number [0-(TX_FIFO_SIZE-1)]
 * @param[in]   psMsg		: Messages point
 * @return      
 * @details    	     
 */
uint32_t CANFD_BUFSet(uint8_t channel, uint8_t bufsn,CANFD_FD_MSG_T *psMsg)
{

		uint32_t Count = 0 ;
		CANFD_T * g_pCanfd=canfd[channel];	
		CANFD_BUF_T  *TxBuff;
	
		TxBuff = (CANFD_BUF_T  *)(CANFD_SRAM_BASE_ADDR(g_pCanfd) + TX_FIFO_ADDR_OFFSET);

	  if(bufsn >= TX_FIFO_SIZE) return 0;

    /* transmission is pending in this message buffer */
    if(g_pCanfd->TXBRP & (1 << bufsn)) return 0;
	
	
		TxBuff[bufsn].u32Id = psMsg->u32Id;
		if (psMsg->eIdType == eCANFD_XID)
    {
			TxBuff[bufsn].u32Id = TX_BUFFER_T0_ELEM_XTD_Msk | (TxBuff[bufsn].u32Id & 0x1FFFFFFF);//XID bit:[28:0].
    }
    else
    {
			TxBuff[bufsn].u32Id = (TxBuff[bufsn].u32Id & 0x7FF) << 18;														//SID bit:[28-18].
    }

		TxBuff[bufsn].u32Config = (CANFD_EncodeDLC(psMsg->u32DLC) << TX_BUFFER_T1_ELEM_DLC_Pos);//Data Length bit:[19:16]

    if (psMsg->bFDFormat)  
			{TxBuff[bufsn].u32Config |= TX_BUFFER_T1_ELEM_FDF_Msk;																//set	1 = CANFD frame	bit:[21]
     if (psMsg->bBitRateSwitch)  TxBuff[bufsn].u32Config |= TX_BUFFER_T1_ELEM_BSR_Msk; 			//set	1 = CANFD BSR frame	bit:[20]
		}
		else
		{
    if (psMsg->eFrmType == eCANFD_REMOTE_FRM) TxBuff[bufsn].u32Id |= TX_BUFFER_T0_ELEM_RTR_Msk;		//set	1 = Transmit remote frame	bit:[29]
		}	


    for (Count = 0; Count < (psMsg->u32DLC + (4 - 1)) / 4; Count++)													//Message data putin TX buffer
    {
        TxBuff[bufsn].au32Data[Count] = psMsg->au32Data[Count];
    }

    return 1;

}


/**
 * @brief       make send buffer data out
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   bufsn		: buffer serial number [0-(TX_FIFO_SIZE-1)]
 * @return      
 * @details    	     
 */
void CANFD_BUFSend(uint8_t channel, uint8_t bufsn)	
{
		
	CANFD_T * g_pCanfd=canfd[channel];	
	uint32_t u32Config;

    u32Config = 200 ;
    do
    {
        if (CANFD_GET_COMMUNICATION_STATE(g_pCanfd) == eCANFD_IDLE)
        {
            g_pCanfd->TXBAR = 1 << bufsn ;       // Tx_Element_Buff 0 Request transmitting
            break ;
        }
    } while (--u32Config > 0);

}


/**
 * @brief       direct use tx buffer_sn to send one messgae
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   bufsn		: buffer serial number [0-(TX_FIFO_SIZE-1)]
 * @param[in]   psMsg		: Messages point
 * @return      
 * @details    	     
 */
void CANFD_MsgSend(uint8_t channel,uint8_t bufsn, CANFD_FD_MSG_T *psMsg)
{
		if( bufsn<TX_FIFO_SIZE )
	  {	CANFD_BUFSet(channel, bufsn,psMsg);
			CANFD_BUFSend(channel, bufsn);	
		}
}

/*---------------------------------------------------------------------------------------------------------*/
/*                             Show the CAN FD Message Function                                            */
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief       use tx bufsn direct send one messgae
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   bufsn		: buffer serial number [0-(TX_FIFO_SIZE-1)]
 * @param[in]   psMsg		: Messages point
 * @return      
 * @details    	     
 */
void CANFD_ShowMessage(uint8_t channel,uint8_t txrx,CANFD_FD_MSG_T *psMsg)
{
    uint8_t u8Cnt,res;
		static char printbuf[512];

#if  1	
		if(txrx)
		{
			res =sprintf(printbuf,"\n--->--->\n");
			res =res+sprintf(printbuf+res,"Tx CAN = [%d]\n", channel);	
		}else
		{		
			res =sprintf(printbuf,"\n<---<---\n");
			res =res+sprintf(printbuf+res,"Rx CAN = [%d][%d]\n", channel,	msg_count[channel]);
		}
	
	  if (psMsg->eIdType == eCANFD_SID)
        res =res+sprintf(printbuf+res,"STD_ID = 0x%03X\n", psMsg->u32Id);
    else
        res =res+sprintf(printbuf+res,"EXT_ID = 0x%08X\n",  psMsg->u32Id);
		
		if(psMsg->eFrmType ==   eCANFD_DATA_FRM) //eFrmType
		{    
				if(psMsg->bBitRateSwitch) 
				{												res =res+sprintf(printbuf+res,"BRS_");}
				else
				{
				  if(psMsg->bFDFormat)  res =res+sprintf(printbuf+res,"FD_");
					else  								res =res+sprintf(printbuf+res,"STD_");
				}
				
				res =res+sprintf(printbuf+res,"DAT = (%02d bytes) : ",  psMsg->u32DLC);			
				for (u8Cnt = 0; u8Cnt <   psMsg->u32DLC; u8Cnt++)
				{
						res =res+sprintf(printbuf+res,"%02x",  psMsg->au8Data[u8Cnt]);
				}
		}		
    else res =res+sprintf(printbuf+res,"REMOTE_FRAME");//remote frame no data
#else
		res =sprintf(printbuf,"Rx CAN = [%d][%d]", channel,	msg_count[channel]);
#endif
		res =res+sprintf(printbuf+res,"\n\n");
		DEBUG_MSG("%s",printbuf);
}


/*---------------------------------------------------------------------------------------------------------*/
/*                                     CAN FD Config                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief       Receive Disable 
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @return      
 * @details    	only disable reveive interrupt     
 */
void CANFD_Fini(uint8_t channel)
{
	switch(channel)
	{
		case CAN0:
			NVIC_DisableIRQ(CANFD00_IRQn);break;
		case CAN1:
			NVIC_DisableIRQ(CANFD10_IRQn);break;
		case CAN2:
			NVIC_DisableIRQ(CANFD20_IRQn);break;
		case CAN3:
			NVIC_DisableIRQ(CANFD30_IRQn);break;
	}
}


void CAN_GetDefaultConfig(CANFD_FD_T *psConfig, uint8_t u8OpMode)
{
    memset(psConfig, 0, sizeof(CANFD_FD_T));

    psConfig->sBtConfig.sNormBitRate.u32BitRate = DEF_NormBitRate;			//Normal bit rate

    if (u8OpMode == CANFD_OP_CAN_MODE)																	
    {
        psConfig->sBtConfig.sDataBitRate.u32BitRate = 0;
        psConfig->sBtConfig.bFDEn = FALSE;															//Classical CAN mode 
        psConfig->sBtConfig.bBitRateSwitch = FALSE;
    }
    else																																
    {
        psConfig->sBtConfig.sDataBitRate.u32BitRate = DEF_DataBitRate;	//Data bit rate. 
        psConfig->sBtConfig.bFDEn = TRUE;																//CAN FD mode 
        psConfig->sBtConfig.bBitRateSwitch = FALSE;
    }

    /*Disable the Internal Loopback mode */
    psConfig->sBtConfig.bEnableLoopBack = FALSE;
		/*Disable the Monitor mode */
    psConfig->sBtConfig.bEnableMonitor= FALSE;
    /*Get the CAN FD memory address*/
    psConfig->u32MRamSize  = CANFD_SRAM_SIZE;
		
}		


extern 	uint32_t CANFD_CalculateTimingValues(CANFD_T *psCanfd, uint32_t u32NominalBaudRate, uint32_t u32DataBaudRate, uint32_t u32SourceClock_Hz, CANFD_TIMEING_CONFIG_T *psConfig);
extern	void CANFD_SetTimingConfig(CANFD_T *psCanfd, const CANFD_TIMEING_CONFIG_T *psConfig);

/**
 * @brief       Init CAN FD bit Rate
 * @param[in]   psCanfd 		: can port 
  *             - \ref CANFD0
  *             - \ref CANFD1
  *             - \ref CANFD2
  *             - \ref CANFD3
 * @param[in]   *psCanfdStr 	: can port configuration information
 * @return      
 * @details    	
 */
void CANFD_BitRate_Init(CANFD_T *psCanfd, CANFD_FD_T *psCanfdStr)
{

		uint8_t tdco;//TDCO = DTSEG1 
	  //Secondary sampling point  = SSP = TDCV = Delay + TDCO;

	    /* calculate and apply timing */
    if (CANFD_CalculateTimingValues(psCanfd,psCanfdStr->sBtConfig.sNormBitRate.u32BitRate, psCanfdStr->sBtConfig.sDataBitRate.u32BitRate,
                                    SystemCoreClock, &psCanfdStr->sBtConfig.sConfigBitTing))
    {
        CANFD_SetTimingConfig(psCanfd, &psCanfdStr->sBtConfig.sConfigBitTing);
    }
		else while(1);

//		/*If data rate is higher than 1Mbps, need enable the send/receive compensation function.
//		This can adjust secondary sampling point*/		
	 
   if(psCanfdStr->sBtConfig.sDataBitRate.u32BitRate> 1000000)		
	 {tdco = (((psCanfd->DBTP) & CANFD_NBTP_NTSEG1_Msk)>>CANFD_NBTP_NTSEG1_Pos);											//Get minimum tdco
		psCanfd->TDCR  = (psCanfd->TDCR & (~CANFD_TDCR_TDCO_Msk)) | (tdco << CANFD_TDCR_TDCO_Pos);			//TDCO need to measure reference DBTP
		psCanfd->TDCR  = (psCanfd->TDCR & (~CANFD_TDCR_TDCF_Msk)) | ((tdco + 1) << CANFD_TDCR_TDCF_Pos);//TDCF mast greater than TDCO,so TDCF= TDCO + 1
	  psCanfd->DBTP  |= CANFD_DBTP_TDC_Msk; 																													//open compensation function
	 }
	 	
}

/**
 * @brief       Set CANFD work mode 
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   *psCanfdStr 	: can port configuration information
 * @return  		
*/
void CANFD_Set_Mode(uint8_t channel,CANFD_FD_T *psCanfdStr)
{

		CANFD_T * g_pCanfd=canfd[channel];	

		//---------------	LoopBack-----------------//
		if(psCanfdStr->sBtConfig.bEnableLoopBack)    																		
			{ g_pCanfd->CCCR |= CANFD_CCCR_TEST_Msk;
        g_pCanfd->TEST |= CANFD_TEST_LBCK_Msk;	
			}		
		else																			
		 { g_pCanfd->CCCR &=  ~CANFD_CCCR_TEST_Msk; 
		   g_pCanfd->TEST &=  ~CANFD_TEST_LBCK_Msk;	
		 }
		//---------------	Monitor -----------------//			
		if(psCanfdStr->sBtConfig.bEnableMonitor)  g_pCanfd->CCCR |=  CANFD_CCCR_MON_Msk;  
		else 																			g_pCanfd->CCCR &=  ~CANFD_CCCR_MON_Msk; 
			
		//-------	CAN FD Bit Rate Switch ---------//					 
		if(psCanfdStr->sBtConfig.bBitRateSwitch) 	
		{		g_pCanfd->CCCR |=  (CANFD_CCCR_BRSE_Msk | CANFD_CCCR_FDOE_Msk);}
		else 
		{ 
		//--------------- CAN FD ----------------//					
			if(psCanfdStr->sBtConfig.bFDEn) 
			{
			 g_pCanfd->CCCR |=  CANFD_CCCR_FDOE_Msk;
			}
			else	g_pCanfd->CCCR &=  ~(CANFD_CCCR_BRSE_Msk | CANFD_CCCR_FDOE_Msk);
		}
		
		g_pCanfd->CCCR &= ~(CANFD_CCCR_CCE_Msk | CANFD_CCCR_INIT_Msk)  ; 
}



/*----------------------------------------------------------------------------------------------*/
/*                              CANFD_Initial                                                   */
/*----------------------------------------------------------------------------------------------*/
/**
 * @brief       CANFD_Initial  
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   *psCanfdStr 	: can port configuration information
 * @return  		
*/
void CANFD_Initial(uint8_t channel,CANFD_FD_T *psCanfdStr)
{
	CANFD_T * g_pCanfd=canfd[channel];	
	CANFD_STD_FILTER_T  *pSID_Buff ;
	CANFD_EXT_FILTER_T 	*pXID_Buff ;
	uint8_t cnt;
	
	SYS_UnlockReg();

	g_pCanfd->CCCR = CANFD_CCCR_CCE_Msk | CANFD_CCCR_INIT_Msk ;																	//Start config
	g_pCanfd->CCCR |= (CANFD_CCCR_BRSE_Msk | CANFD_CCCR_FDOE_Msk);															//Open CAN FD & Bit Rate Switch
	CANFD_BitRate_Init(g_pCanfd,psCanfdStr);																										//Config bit rate
	
/*The configurable start addresses are 32-bit word addresses i.e. only bits 15 to 2 are evaluated,the two least significant bits are ignored.*/
//--------------------Standard ID message filter element quantity SID_BUFF_SIZE = 24
		g_pCanfd->SIDFC = (SID_BUFF_SIZE << CANFD_SIDFC_LSS_Pos) | SID_BUFF_ADDR_OFFSET ; //24-SID  start address offset:0
//--------------------Extended ID message filter element quantity XID_BUFF_SIZE = 16
    g_pCanfd->XIDFC = (XID_BUFF_SIZE << CANFD_XIDFC_LSE_Pos) | XID_BUFF_ADDR_OFFSET ; // XID_BUFF_ADDR_OFFSET = SID_BUFF_SIZE*4
//--------------------TX FIFO element quantity TX_FIFO_SIZE = 32
    g_pCanfd->TXBC  = (TX_FIFO_SIZE  << CANFD_TXBC_NDTB_Pos) | TX_FIFO_ADDR_OFFSET ;  //TX_FIFO_ADDR_OFFSET =	(XID_BUFF_ADDR_OFFSET + XID_BUFF_SIZE*8)
    g_pCanfd->TXESC = 7 << CANFD_TXESC_TBDS_Pos;                    									//7 :Default TX maximum data length	

//--------------------RX FIFO element quantity RX_FIFO_SIZE = 50
		g_pCanfd->RXF1C = CANFD_RXF1C_F1OM_Msk																						//1= FIFO 1 overwrite mode
										|	(0 << CANFD_RXF1C_F1WM_Pos)         														//watermark not used
										| (RX_FIFO_SIZE << CANFD_RXF1C_F1S_Pos)         									//FIFO1 can hold (RX_FIFO_SIZE = 50) messages
										|  RX_FIFO_ADDR_OFFSET ;                        									//RX_FIFO_ADDR_OFFSET =	(TX_FIFO_ADDR_OFFSET  + TX_FIFO_SIZE*(sizeof(CANFD_BUF_T)))	

// 0=>8Byte, 1=>12Byte, 2=>16Byte, 3=>20Byte, 4=>24Byte, 5=>32Byte, 6=>48Byte, 7=>64Byte
    g_pCanfd->RXESC = (g_pCanfd->RXESC & (~CANFD_RXESC_F1DS_Msk)) | (7 << CANFD_RXESC_F1DS_Pos);//7=>Default RX maximum data length	


//------Config ID message filter element RAM address ---------//		
	pSID_Buff = (CANFD_STD_FILTER_T  *)(CANFD_SRAM_BASE_ADDR(g_pCanfd));												//
	pXID_Buff	=	(CANFD_EXT_FILTER_T  *)(CANFD_SRAM_BASE_ADDR(g_pCanfd) + XID_BUFF_ADDR_OFFSET);	//


#if CAN_HW_FILTER  == 0
		//SID_Buff &&  XID_Buff  receive all messages

    for(cnt=0;cnt<SID_BUFF_SIZE;cnt++)
     pSID_Buff[0].VALUE 			= CANFD_RX_FIFO1_STD_MASK(0, 0) ;        									// receive all SID messages
    
		for(cnt=0;cnt<XID_BUFF_SIZE;cnt++)		
    {pXID_Buff[0].LOWVALUE  	= CANFD_RX_FIFO1_EXT_MASK_LOW(0) ;
     pXID_Buff[0].HIGHVALUE 	= CANFD_RX_FIFO1_EXT_MASK_HIGH(0) ;  											// receive all XID messages
		}
  
    g_pCanfd->GFC = 0 ;                                         											// receive remote frames

#else
    pSID_Buff[0].VALUE = CANFD_RX_FIFO1_STD_MASK(0x110, 0x7F0) ;   // SID, Mask
    pSID_Buff[1].VALUE = CANFD_RX_FIFO1_STD_MASK(0x22F, 0x7FF) ;   // SID, Mask
    pSID_Buff[2].VALUE = CANFD_RX_FIFO1_STD_MASK(0x333, 0x7FF) ;   // SID, Mask

    pXID_Buff[0].LOWVALUE  = CANFD_RX_FIFO1_EXT_MASK_LOW(0x220) ;       // XID
    pXID_Buff[0].HIGHVALUE = CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFF0) ; // MASK

    pXID_Buff[1].LOWVALUE  = CANFD_RX_FIFO1_EXT_MASK_LOW(0x3333) ;      // XID
    pXID_Buff[1].HIGHVALUE = CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF) ; // MASK

    pXID_Buff[2].LOWVALUE  = CANFD_RX_FIFO1_EXT_MASK_LOW(0x44444) ;     // XID
    pXID_Buff[2].HIGHVALUE = CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF) ; // MASK

    psCanfd->GFC = 0x20 + 0x08 + 3 ;         // reject all no-match SID &XID, reject remote frames

    //  psCanfd->GFC = 0x10 + 0x04 + 3 ; //  no-match SID &XID messages into FIFO1. reject remote frames

#endif

    CANFD_Set_Mode(channel,psCanfdStr);

    CANFD_EnableInt(g_pCanfd, (CANFD_IE_TOOE_Msk | CANFD_IE_RF1NE_Msk), 0, 0, 0);//Open CAN interrupt
		
		SYS_LockReg();
		
		switch(channel)																															//Open CAN interrupt vector
		{
			case CAN0:NVIC_EnableIRQ(CANFD00_IRQn);break;
			case CAN1:NVIC_EnableIRQ(CANFD10_IRQn);break;
			case CAN2:NVIC_EnableIRQ(CANFD20_IRQn);break;
			case CAN3:NVIC_EnableIRQ(CANFD30_IRQn);break;
		}
}

/**
 * @brief       CANFD Start Define Config
 * @param[in]   
 * @return  		
*/
void CANFD_Start(void)
{
		uint8_t u8Cnt;
    DEBUG_MSG("\nStart CAN FD bus function.\n");	

	
		SYS_ResetModule(CANFD0_RST);
    SYS_ResetModule(CANFD1_RST);
    SYS_ResetModule(CANFD2_RST);
    SYS_ResetModule(CANFD3_RST);	
	
	for(u8Cnt=0;u8Cnt<CANFD_NUM;u8Cnt++)
		{sCANFD_Config[u8Cnt].sBtConfig.sNormBitRate.u32BitRate = DEF_NormBitRate;
	   sCANFD_Config[u8Cnt].sBtConfig.sDataBitRate.u32BitRate = DEF_DataBitRate;//1M;2M;5M
		 sCANFD_Config[u8Cnt].sBtConfig.bFDEn = TRUE;
     sCANFD_Config[u8Cnt].sBtConfig.bBitRateSwitch = TRUE;
    /*Disable the Internal Loopback mode */
		 sCANFD_Config[u8Cnt].sBtConfig.bEnableLoopBack = FALSE;
		/*Disable the Monitor mode */
     sCANFD_Config[u8Cnt].sBtConfig.bEnableMonitor= FALSE;

		 CANFD_Initial(u8Cnt,&sCANFD_Config[u8Cnt]);
		}
}


/**
 * @brief       Check CAN bus Status
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @return      
 * @details     If there have some errors, restart	
 */
void CANFD_BusCheck(uint8_t channel)
{

	CANFD_T * g_pCanfd=canfd[channel];	

	if(g_pCanfd->PSR & CANFD_PSR_EP_Msk)				DEBUG_MSG("CAN[%d]EP offline.\n",channel);		
	if(g_pCanfd->PSR & CANFD_PSR_EW_Msk)				DEBUG_MSG("CAN[%d]EW ErrorLimit.\n",channel);
	if(g_pCanfd->PSR & CANFD_PSR_BO_Msk)    		DEBUG_MSG("CAN[%d]BO Busoff.\n",channel);
	
	//If there have some errors, restart
	if(g_pCanfd->CCCR & CANFD_CCCR_INIT_Msk)   	
	{ g_pCanfd->CCCR |= (CANFD_CCCR_CCE_Msk | CANFD_CCCR_INIT_Msk)  ; 	
		g_pCanfd->CCCR &= ~(CANFD_CCCR_CCE_Msk | CANFD_CCCR_INIT_Msk) ; 
		DEBUG_MSG("CAN[%d]ReStart \n",channel);
	}

}


/**
 * @brief       user CAN FD function
 * @param[in]   
 * @return      
 * @details     
 */
void CANFD_Task(void)
{
    uint8_t u8Cnt;

			for(u8Cnt=0;u8Cnt<CANFD_NUM;u8Cnt++)
				 { if(g_u8RxFifo1CompleteFlag[u8Cnt])
						{
						CANFD_LED(u8Cnt,0);//LED ON	
						//display receive data
						CANFD_ShowMessage(u8Cnt,0,&g_sRxMsgFrame[u8Cnt]);
						g_u8RxFifo1CompleteFlag[u8Cnt] = 0;
				
						CANFD_LED(u8Cnt,1);	//LED OFF
						}
					}	
}


/**
 * @brief       Set CAN bus Status LED
 * @param[in]   channel : can channel [0-3] map [CANFD0 - CANFD3]
 * @param[in]   status : LED on or off
 *													- \ref 0  = ON
 *                        	- \ref 1  = OFF
 * @return      
 * @details     If there have some errors, restart	
 */
void CANFD_LED(uint8_t channel,uint8_t status)
{
	switch(channel)
	{
	 case 0:CAN0LED = status ? 1 :  0;break;	
	 case 1:CAN1LED = status ? 1 :  0;break;
	 case 2:CAN2LED = status ? 1 :  0;break;
	 case 3:CAN3LED = status ? 1 :  0;break;
	 default:CAN0LED=1;CAN1LED=1;CAN2LED=1;CAN3LED=1;
	}
}




