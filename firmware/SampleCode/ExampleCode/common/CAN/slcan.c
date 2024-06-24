/****************************************************************************
 * @file     slcan.c
 * @version  V1.00
 * @brief    slcan protocol source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "slcan.h"

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "canapp.h"
#include "user_timer.h"


/*------------------------------------------------------------------------------------------------*/
/* Global variables                                                                               */
/*------------------------------------------------------------------------------------------------*/
struct  slcan_t slcan0,slcan1,slcan2,slcan3;
UART_CFG_T def_uart_cfg =	{115200, 0, 0, 8};


/**
 * @brief       read can normal baudrate by index
 * @param[in]   can_baud_index :	can baudrate index
 * @return      Real baud rate
 * @details     define baud rate as 500kbps
 */
uint32_t  sclcan_can_baud(uint8_t can_baud_index)
{
    uint32_t baud = 500000;
    switch(can_baud_index)
    {
    case SLCAN_BAUD_10K:
        baud = 10000;
        break;
    case SLCAN_BAUD_20K:
        baud = 20000;
        break;
    case SLCAN_BAUD_50K:
        baud = 50000;
        break;
    case SLCAN_BAUD_100K:
        baud = 100000;
        break;
    case SLCAN_BAUD_125K:
        baud = 125000;
        break;
    case SLCAN_BAUD_250K:
        baud = 250000;
        break;
    case SLCAN_BAUD_500K:
        baud = 500000;
        break;
    case SLCAN_BAUD_800K:
        baud = 800000;
        break;
    case SLCAN_BAUD_1000K:
        baud = 1000000;
        break;
    }
    return baud;
}

/**
 * @brief       open one slcan
 * @param[in]   slcan_port : one can port will to be operated
 * @return
 * @details     if open debug will print debug menssage
 */
void slcan_can_open(struct slcan_t* slcan_port)
{
    sCANFD_Config[slcan0.candev_sn]  = slcan0.candev_cfg ;
    CANFD_Initial(slcan_port->candev_sn,&slcan_port->candev_cfg);
    slcan_port->candev_isopen = 1;
    DEBUG_MSG("CAN[%d] Open  [%dkbps][%dkbps]  \n",slcan_port->candev_sn,
              slcan_port->candev_cfg.sBtConfig.sNormBitRate.u32BitRate/1000,slcan_port->candev_cfg.sBtConfig.sDataBitRate.u32BitRate/1000);
}

/**
 * @brief       close one slcan
 * @param[in]   slcan_port : one can port will to be operated
 * @return
 * @details     if open debug will print debug menssage
 */
void slcan_can_close(struct slcan_t* slcan_port)
{
    CANFD_Fini(slcan_port->candev_sn);
    slcan_port->candev_isopen = 0;
    DEBUG_MSG("CAN[%d] Close \n",slcan_port->candev_sn);
}

/**
 * @brief       set can normal baud rate
 * @param[in]   slcan_port : one can port will to be operated
 * @param[in]   can baudrate index
 * @return
 * @details     if open debug will print debug menssage
 */
void slcan_can_set_nbaud(struct slcan_t* slcan_port, uint8_t baud_index)
{
    slcan_port->candev_cfg.sBtConfig.sNormBitRate.u32BitRate = sclcan_can_baud(baud_index);
    DEBUG_MSG("CAN[%d]set NormBitRate: [%dkbps]\n", slcan_port->candev_sn, slcan_port->candev_cfg.sBtConfig.sNormBitRate.u32BitRate/1000);
}

/**
 * @brief       set can data baud rate
 * @param[in]   slcan_port : one can port will to be operated
 * @param[in]   data_mbps:
														- \ref 2  =》 2Mbps
														- \ref 5  =》 5Mbps
 * @return
 * @details     if open debug will print debug menssage
 */
void slcan_can_set_dbaud(struct slcan_t* slcan_port, uint8_t data_mbps)
{
    slcan_port->candev_cfg.sBtConfig.sDataBitRate.u32BitRate = data_mbps*1000000;
    DEBUG_MSG("CAN[%d]set DataBitRate: [%dkbps]\n", slcan_port->candev_sn, slcan_port->candev_cfg.sBtConfig.sDataBitRate.u32BitRate/1000);
}


/**
 * @brief       set can work mode
 * @param[in]   slcan_port : one can port will to be operated
 * @param[in]    mode_index:	select can work mode										
											- \ref SLCAN_MODE_NORMAL 
											- \ref SLCAN_MODE_LISTEN,
											- \ref SLCAN_MODE_LOOPBACK,
											- \ref SLCAN_MODE_LOOPBACKANLISTEN,

 * @return
 * @details     if open debug will print debug menssage
 */
void slcan_can_set_mode(struct slcan_t* slcan_port, uint8_t mode_index)
{
    if(slcan_port->candev_isopen == 1 )	 return ;

    switch(mode_index)
    {
    case SLCAN_MODE_NORMAL:
        slcan_port->candev_cfg.sBtConfig.bEnableMonitor  = FALSE;
        slcan_port->candev_cfg.sBtConfig.bEnableLoopBack = FALSE;
        break;
    case SLCAN_MODE_LISTEN:
        slcan_port->candev_cfg.sBtConfig.bEnableMonitor  = TRUE	;
        slcan_port->candev_cfg.sBtConfig.bEnableLoopBack = FALSE;
        break;
    case SLCAN_MODE_LOOPBACK:
        slcan_port->candev_cfg.sBtConfig.bEnableMonitor  = FALSE;
        slcan_port->candev_cfg.sBtConfig.bEnableLoopBack = TRUE;
        break;
    case SLCAN_MODE_LOOPBACKANLISTEN:
        slcan_port->candev_cfg.sBtConfig.bEnableMonitor  = TRUE	;
        slcan_port->candev_cfg.sBtConfig.bEnableLoopBack = TRUE;
        break;
    }

    DEBUG_MSG("CAN[%d]mode_index:[%d] ", slcan_port->candev_sn,  mode_index);
    DEBUG_MSG("Monitor:[%d]; LoopBack:[%d]\n", slcan_port->candev_cfg.sBtConfig.bEnableMonitor,  slcan_port->candev_cfg.sBtConfig.bEnableLoopBack);
}

/**
 * @brief       read one message to slcan instance 
 * @param[in]   slcan_port : one can port will to be operated								
 * @return  		sizeof(CANFD_FD_MSG_T)
 * @details     if open debug will print debug menssage
 */
uint32_t slcan_can_read(struct slcan_t* slcan_port)
{
    slcan_port->can_rx_msg = g_sRxMsgFrame[slcan_port->candev_sn];
    return sizeof(CANFD_FD_MSG_T);
}

/**
 * @brief       write one message to slcan can port
 * @param[in]   slcan_port : one can port will to be operated								
 * @return  		sizeof(CANFD_FD_MSG_T)
 * @details     
 */
uint32_t slcan_can_write(struct slcan_t* slcan_port)
{
    CANFD_BusCheck(slcan_port->candev_sn);															//check bus error 
    CANFD_BUFSet(slcan_port->candev_sn, 0,&slcan_port->can_tx_msg);			//put message in send FIFO1 serial 0
    CANFD_BUFSend(slcan_port->candev_sn, 0);														//send the serial 0 message

    return sizeof(CANFD_FD_MSG_T);
}

/**
 * @brief       which channel uart port well open
 * @param[in]   slcan_port : one can port will to be operated								
 * @return  		
 * @details     if use CAN to UART need open initialize
								for example:
								UART_Open(UART0, slcan_port->uartdev_cfg.u32BitRate);
								UART_SetLineConfig(UART0, slcan_port->uartdev_cfg.u32BitRate, slcan_port->uartdev_cfg.u8DataBits,
															slcan_port->uartdev_cfg.u8ParityType, slcan_port->uartdev_cfg.u8CharFormat);
 */

void slcan_uart_open(struct slcan_t* slcan_port)
{
    switch(slcan_port->uartdev_sn)
    {
    case 4://add uart initialize code
    case 5:
        break;
    }
    DEBUG_MSG("UART[%d] open \n",slcan_port->uartdev_sn);
}

/**
 * @brief       which channel uart port well close
 * @param[in]   slcan_port : one can port will to be operated								
 * @return  		
 * @details    	for example :UART_Close(UART0)
 */
void slcan_uart_close(struct slcan_t* slcan_port)
{
    switch(slcan_port->uartdev_sn)
    {
    case 4://add uart close code
    case 5:
        break;
    }
    DEBUG_MSG("UART[%d] close \n",slcan_port->uartdev_sn);
}

/**
 * @brief       read slcan uart data from buffer and check valid data
 * @param[in]   slcan_port : one can port will to be operated								
 * @return  		return -1 : no data
								return 0 	: only 1 valid data
								return 1 	: There still data left to process
 * @details    	
 */

int32_t slcan_uart_read(struct slcan_t* slcan_port)
{
    uint32_t cnt=0;
    uint8_t *p = slcan_port->uart_rx_buffer;
    int32_t udata;
    static	uint32_t timeout_cnt;
    static	uint32_t timeout_tick;
    static	uint32_t DataBytes;


    if(slcan_port->uartdev_sn < 4)//USB_VCOM used sn < 4;
    {															//Data Point to USB_VCOM
        if(vcom_dat[slcan_port->uartdev_sn].comTbytes > 0)//Data buffer not empty
        {
            if(timeout_tick != GetTick())									//Add Timeout Count
            {   timeout_cnt++;							
                timeout_tick =  GetTick();
            }

            if(DataBytes != vcom_dat[slcan_port->uartdev_sn].comTbytes)
            {   timeout_cnt	=	0;
                DataBytes 	= vcom_dat[slcan_port->uartdev_sn].comTbytes;
            }

            if(timeout_cnt > 1)														//1ms timeout to check receive data
            {
                do
                {
                    udata = usb_getchar(slcan_port->uartdev_sn);
                    if(udata != -1)
                    {   p[cnt] = (char)udata;
                        if(p[cnt] == '\r')
                        {   cnt++;
                            if(vcom_dat[slcan_port->uartdev_sn].comTbytes == 0) 
														{slcan_port->uart_tx_len = cnt;
														return 0; 	//Find 1 Command
														}
                            else return 1;								//There's still data to be processed
                        }
                        cnt++;
                    }
                    else return -1;												//Data buffer empty
                }
                while(1);
            }
        }
        else return -1;																		//Data buffer empty

    }
		else
		{		
			//user add uart port data read code
			//Data Point to UART
		}
			
		
    return -1;																						//Data buffer empty
}

/**
 * @brief       send slcan uart data 
 * @param[in]   slcan_port : one can port will to be operated		
 * @param[in]   buffer : send buffer opint 
 * @param[in]   size	 : buffer data lenght
 * @return  		The length of the sent data
 * @details    	
 */

uint32_t slcan_uart_write(struct slcan_t* slcan_port, void* buffer, uint32_t size)
{

    uint8_t *p = buffer;
    uint32_t res;

    if(slcan_port->uartdev_sn < 4)//USB_VCOM used sn < 4
    {
		res = USB_TransferData(p,size,slcan_port->uartdev_sn);//Data Point to USB_VCOM
		}
		else
		{		
			//user add uart port data read code
			//Data Point to UART
		}

    return res;
}

/**
 * @brief       ASC2 convert to decimal
 * @param[in]   c 		 ：ASC2 code
 * @return  		decimal number
 * @details    	
 */

static int asc2nibble(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';

    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;

    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;

    return 16; /* error */
}

/**
 * @brief       hex sting convert to decimal
 * @param[in]   strbuffer		 	：hex sting
 * @param[in]   strlen		 		：sting lenght
 * @return  		decimal number
 * @details    	
 */
static int hexstr2int(uint8_t* strbuffer, uint8_t strlen)
{
    uint32_t numcount = 0;
    int8_t num = 0;
    int8_t index = 0;
    while(index < strlen)
    {
        num = asc2nibble(strbuffer[index]);
        if( (0x00 <= num) && (num <= 0x0F) )
        {
            numcount = numcount << 4;
            numcount |= (num & 0x0F);
        }
        index++;
    }
    return numcount;
}

/**
 * @brief       Decode the Data Length Code.
 * @param[in]   code:   Data Length Code.
 * @return      Number of bytes in a message.
 * @details     Converts a Data Length Code into a number of message bytes.
 */
static uint8_t SLCAN_DecodeDLC(uint8_t code)
{
    if (code >= '0' && code < '9')
    {
        return  code - '0';
    }
    else
    {
        switch(code)
        {
        case '9':
            return 12;
        case 'A':
            return 16;
        case 'B':
            return 20;
        case 'C':
            return 24;
        case 'D':
            return 32;
        case 'E':
            return 48;
        case 'F':
            return 64;
        default :
            return 0xff;
        }
    }
}


/**
 * @brief       Encode the Data Length Code.
 * @param[in]   dlc:  Number of bytes in a message.
 * @return      Data Length Code.
 * @details     Converts number of bytes in a message into a Data Length Code.
 */
static uint8_t SLCAN_EncodeDLC(uint8_t dlc)
{
    if ( dlc < 9)
    {
        return  dlc + '0';
    }
    else
    {
        switch(dlc)
        {
        case 12:
            return '9';
        case 16:
            return 'A';
        case 20:
            return 'B';
        case 24:
            return 'C';
        case 32:
            return 'D';
        case 48:
            return 'E';
        case 64:
            return 'F';
        default :
            return 0xff;
        }
    }
}

/**
 * @brief       uart ascii data convert to command data 
 * @param[in]   slcan_port : one can port will to be operated		
 * @return      
 * @details     slcan port operate and send can message
 */
void slcan_parse_ascii(struct slcan_t* slcan_port)
{

    /* for answers to received commands */
    int rx_out_len = 0;														//uart ack data lenght
    char replybuf[10] = {0}; 											//uart ack data buffer
    uint8_t uart_status;													//uart cmd ack status
    uint8_t *buf = slcan_port->uart_rx_buffer;//cmd data buffer

    uint8_t cmd_bytes ;														//uart cmd lenght :	cmd_bytes = 1 has no parameter
																									//									cmd_bytes = 2 has 1 parameter

    int tmp = 0;

    uint8_t cmd = buf[0];													//slcan uart cmd

    switch(cmd)    																//command analyse
    {

    /* 	Open the CAN channel in normal mode (sending & receiving).
    			This command is only active if the CAN channel is closed and
    			has been set up prior with either the S or s command (i.e. initiated)*/
    case 'O':
        slcan_can_close(slcan_port);
        if(slcan_port->candev_cfg.sBtConfig.sNormBitRate.u32BitRate == 0)
            slcan_port->candev_cfg.sBtConfig.sNormBitRate.u32BitRate = 500000;

        slcan_can_open(slcan_port);
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_ACK;
        break;


    /* 	Close the CAN channel.
    			This command is only active if the CAN channel is open */
    case 'C':
        slcan_can_close(slcan_port);
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_ACK;
        break;

    /* Setup with standard CAN FD Data bit-rates where n is 2 or 5.
    	2: Data bit rates is 2Mbps ;  5: Data bit rates is 5Mbps ； */
    case 'Y'://only Y2 or Y5
        cmd_bytes 	= 2;
        if(buf[1] == '2' || buf[1] == '5')
        {   slcan_can_set_dbaud(slcan_port, buf[1]-'0');
            uart_status = SLCAN_UART_ACK;
        } else uart_status = SLCAN_UART_NACK;
        break;

    /* Setup with standard CAN bit-rates where n is 0-8.
    	This command is only active if the CAN channel is close */
    case 'S'://S0~S8
        cmd_bytes 	= 2;
        if((buf[1] - '0' ) < 9)
        {   slcan_can_set_nbaud(slcan_port, buf[1]-'0');
            uart_status = SLCAN_UART_ACK;
        } else uart_status = SLCAN_UART_NACK;

        break;

//---------
//canableV2.0 CANFD mode,	FD mode	none remote frame
    case 	'r'://Transmit an standard RTR (11bit) CAN frame
    case	't'://Transmit an standard (11bit) CAN frame.
    case 	'd'://Transmit an standard CAN FD frame
    case 	'b'://Transmit an standard CAN FD BRS frame

        tmp = SLCAN_DecodeDLC(buf[4]);
        if(tmp == 0xff)
        {
            uart_status = SLCAN_UART_NACK;
            break;
        }
        else slcan_port->can_tx_msg.u32DLC	= tmp;
        tmp =  hexstr2int(buf+1,3);
        slcan_port->can_tx_msg.u32Id 		= tmp;//Frame ID
        slcan_port->can_tx_msg.eIdType 	= eCANFD_SID; //11bit id  Standard frame format
        cmd_bytes =  5;  // "Riiin********\a"
        if(cmd == 'r')
        {   //tiiil[CR]
            slcan_port->can_tx_msg.eFrmType = eCANFD_REMOTE_FRM;//Remote frame
        } else
        {
            switch(cmd)
            {
            case 'b':
                slcan_port->can_tx_msg.bBitRateSwitch = 1;
                slcan_port->can_tx_msg.bFDFormat 			= 1;
                break;
            case 'd':
                slcan_port->can_tx_msg.bBitRateSwitch = 0;
                slcan_port->can_tx_msg.bFDFormat 			= 1;
                break;
            default :
                slcan_port->can_tx_msg.bFDFormat 			= 0;
                slcan_port->can_tx_msg.bBitRateSwitch = 0;
                break;
            }

            //tiiildd...[CR]
            slcan_port->can_tx_msg.eFrmType = eCANFD_DATA_FRM;//Data frame
            if(slcan_port->can_tx_msg.u32DLC > 0)
            {
                cmd_bytes += (slcan_port->can_tx_msg.u32DLC* 2); //add dlc
                for (tmp = 0; tmp < slcan_port->can_tx_msg.u32DLC; tmp++)
                {
                    slcan_port->can_tx_msg.au8Data[tmp] = hexstr2int( buf +5 + (tmp *2), 2);
                }
            }
        }
        rx_out_len = 1;
        uart_status = SLCAN_UART_SEND;
        break;

    case 'R'://Transmit an extended RTR (29bit) CAN frame.
    case 'T'://Transmit an extended (29bit) CAN frame.
    case 'D'://Transmit an extended CAN FD frame
    case 'B'://Transmit an extended CAN FD BRS frame
        tmp = SLCAN_DecodeDLC(buf[9]);
        if(tmp == 0xff)
        {
            uart_status = SLCAN_UART_NACK;
            break;
        }
        else slcan_port->can_tx_msg.u32DLC	= tmp;
        tmp =  hexstr2int( buf + 1,8);
        slcan_port->can_tx_msg.u32Id 		= tmp;//Frame ID
        slcan_port->can_tx_msg.eIdType 	= eCANFD_XID; //29bit id Extend frame format
        cmd_bytes =  10;; // "Riiiiiiii********\a"
        if(cmd == 'R')
        {   //Riiiiiiiil[CR]
            slcan_port->can_tx_msg.eFrmType = eCANFD_REMOTE_FRM;//Remote frame
        } else
        {
            switch(cmd)
            {
            case 'B':
                slcan_port->can_tx_msg.bBitRateSwitch 	= 1;
                slcan_port->can_tx_msg.bFDFormat 				= 1;
                break;
            case 'D':
                slcan_port->can_tx_msg.bBitRateSwitch 	= 0;
                slcan_port->can_tx_msg.bFDFormat 				= 1;
                break;
            default :
                slcan_port->can_tx_msg.bFDFormat 				= 0;
                slcan_port->can_tx_msg.bBitRateSwitch 	= 0;
                break;
            }

            slcan_port->can_tx_msg.eFrmType = eCANFD_DATA_FRM;//Data frame
            if(slcan_port->can_tx_msg.u32DLC > 0)
            {   //Tiiiiiiiildd...[CR]
                cmd_bytes += (slcan_port->can_tx_msg.u32DLC* 2); //add dlc
                for (tmp = 0; tmp < slcan_port->can_tx_msg.u32DLC; tmp++)
                {
                    slcan_port->can_tx_msg.au8Data[tmp] = hexstr2int( buf + 10 + (tmp *2), 2);
                }
            }
        }
        rx_out_len = 1;
        uart_status = SLCAN_UART_SEND;
        break;
				
    /* Get Version number of both CAN232 hardware and software
    		This command is only active always */
    case 'V'://software version
        rx_out_len 	= sprintf(replybuf, "V2401\r");
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_REPLY;
        break;

    /* check for 'v'ersion command */
    case 'v'://Hardware version
        rx_out_len 	= sprintf(replybuf, "v2401\r");
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_REPLY;
        break;

    /* Get Serial number of the CAN232.
    		This command is only active always */
    case 'N':
        rx_out_len = sprintf(replybuf, "N0001\r");
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_REPLY;
        break;
		
//---------------------------------------------------------------------------//		
//The following functions need to be added,	release it in the future version //
//---------------------------------------------------------------------------//		
		
    /* 	Open the CAN channel in listen only mode (receiving).
    			This command is only active if the CAN channel is closed and
    			has been set up prior with either the S or s command (i.e. initiated) */
    case 'L':
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_ACK;
        break;
    /* 	Read Status Flags.
    			This command is only active if the CAN channel is open
    	Bit 0 CAN receive FIFO queue full
    	Bit 1 CAN transmit FIFO queue full
    	Bit 2 Error warning (EI), see SJA1000 datasheet
    	Bit 3 Data Overrun (DOI), see SJA1000 datasheet
    	Bit 4 Not used.
    	Bit 5 Error Passive (EPI), see SJA1000 datasheet
    	Bit 6 Arbitration Lost (ALI), see SJA1000 datasheet *
    	Bit 7 Bus Error (BEI), see SJA1000 datasheet **
    	*/
    case 'F':
        rx_out_len  = sprintf(replybuf, "F00\r");
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_REPLY;
        break;				
				
    /* Setup UART with a new baud rate where n is 0-6.
    		This command is only active if the CAN channel is closed.
    		The value is saved in EEPROM and is remembered next time
    		the CAN232 is powered up.
    		U0 • Setup 230400 baud (not guaranteed to work)
    		U1 •• Setup 115200 baud
    		U2 ••• Setup 57600 baud (default when delivered)
    		U3 •••• Setup 38400 baud
    		U4 ••••• Setup 19200 baud
    		U5 •••••• Setup 9600 baud
    		U6 ••••••• Setup 2400 bau */
    case 'U':
        cmd_bytes 	= 2;
        break;

    /*Setup with BTR0/BTR1 CAN bit-rates where xx and yy is a hex
    value. This command is only active if the CAN channel is closed.*/
    case 's'://sxxyy
        cmd_bytes 	= 5;
        uart_status = SLCAN_UART_NACK;
        break;

//poll functio set  ---------
	/*Poll incomming FIFO for CAN frames (single poll)
		This command is only active if the CAN channel is open.
		NOTE: This command is disabled in the new AUTO POLL/SEND
		feature from version V1220. It will then reply BELL if used.
		Example 1: P[CR]
		Poll one CAN frame from the FIFO queue.
		Returns: A CAN frame with same formatting as when sending
		frames and ends with a CR (Ascii 13) for OK. If there
		are no pendant frames it returns only CR. If CAN
		channel isn’t open it returns BELL (Ascii 7). If the
		TIME STAMP is enabled, it will reply back the time
		in milliseconds as well after the last data byte (before
		the CR). For more information, see the Z command.*/
    case 'P':
  /*Polls incomming FIFO for CAN frames (all pending frames)
    This command is only active if the CAN channel is open.
    NOTE: This command is disabled in the new AUTO POLL/SEND
    feature from version V1220. It will then reply BELL if used.
    Example 1: A[CR]
    Polls all CAN frame from the FIFO queue.
    Returns: CAN frames with same formatting as when sending
    frames seperated with a CR (Ascii 13). When all
    frames are polled it ends with an A and
    a CR (Ascii 13) for OK. If there are no pending
    frames it returns only an A and CR. If CAN
    channel isn’t open it returns BELL (Ascii 7). If the
    TIME STAMP is enabled, it will reply back the time
    in milliseconds as well after the last data byte (before
    the CR). For more information, see the Z command*/
    case 'A':
        cmd_bytes 	= 1;
        uart_status = SLCAN_UART_NACK;
        break;
    /*Sets Auto Poll/Send ON/OFF for received frames.*/
    case 'X':
        cmd_bytes 	= 2;
        if (buf[1] & 0x01)
            uart_status = SLCAN_UART_ACK;
        else
            uart_status = SLCAN_UART_NACK;
        break;				
				
			
    /* Setting Acceptance Code and Mask registers*/
    case 	'W'	://Wn[CR] Filter mode setting.
        cmd_bytes 	= 2;
        uart_status = SLCAN_UART_ACK;
        break;
    case  'M' ://Mxxxxxxxx[CR] Sets Acceptance Mask Register
    case 	'm'	://mxxxxxxxx[CR] Sets Acceptance Code Register
        buf[9] 			= 0; /* terminate filter string */
        cmd_bytes 	= 9;
        uart_status = SLCAN_UART_ACK;
        break;

    /* Sets Time Stamp ON/OFF for received frames only */
    case  'Z':
        if((buf[1] & 0x01))
        {
            slcan_port->set_flag |= SLCAN_SFLAG_TIMESTAMP;
        } else {
            slcan_port->set_flag &= ~SLCAN_SFLAG_TIMESTAMP;
        }
        cmd_bytes 	= 2;
        uart_status = SLCAN_UART_ACK;
        break;
				
    case '\r' :
    /*Auto Startup feature (from power on).*/
    case 	'Q':
    default:
        uart_status = SLCAN_UART_NACK;
    }


    switch(uart_status)//State process
    {
    case SLCAN_UART_EXIT	:
        rx_out_len = 0;
        break;
    case SLCAN_UART_NACK	:
        replybuf[0] = '\a';
        rx_out_len = 1;
        break;
    case SLCAN_UART_REPLY:
        break;
    case SLCAN_UART_SEND	:
        slcan_can_write(slcan_port);
    case SLCAN_UART_ACK 	:
        replybuf[0] = '\r';
        rx_out_len = 0;//cangroo don't need ack
    default:
        break;
    }


    if(rx_out_len > 0)//uart ack
    {
        slcan_uart_write(slcan_port, replybuf, rx_out_len);
        DEBUG_MSG("COM[%d]Reply:[%d][%s]\n",slcan_port->uartdev_sn, rx_out_len,replybuf);
    }
}


/**
 * @brief       uart process
 * @param[in]   slcan_port : one can port will to be operated		
 * @return      
 * @details     serial  data ---> can message
 */
void slcan_process_uart(struct slcan_t* slcan_port)
{
    int32_t rc = 1;

    while(rc)																	//Buffer still has command. Keep checking
    {
        rc = slcan_uart_read(slcan_port);	//check uart data correct

        if( rc < 0 )													//No valid data
        {
            memset(slcan_port->uart_rx_buffer, 0, SLCAN_MTU);
            return;
        }

        DEBUG_MSG("\nSLCAN%d :COM[%d] >> CAN[%d] len:[%d] data:[%s]\n\n",
                  slcan_port->candev_sn,slcan_port->uartdev_sn, slcan_port->candev_sn, slcan_port->uart_tx_len, slcan_port->uart_rx_buffer);

        slcan_parse_ascii(slcan_port);  	//uart data conversion
        memset(slcan_port->uart_rx_buffer, 0, SLCAN_MTU);
    }
}

/**
 * @brief       can message convert to uart data
 * @param[in]   slcan_port : one can port will to be operated		
 * @return      
 * @details     can data ---> serial  data
 */
static int slcan_can2ascii(struct slcan_t* slcan_port)
{
    int pos = 0;
    uint32_t temp = 0;
    memset(slcan_port->uart_tx_buffer, 0, SLCAN_MTU);

    /* RTR frame */
    if(slcan_port->can_rx_msg.eFrmType ==  eCANFD_REMOTE_FRM)
    {
        if(slcan_port->can_rx_msg.eIdType == eCANFD_XID)
        {
            slcan_port->uart_tx_buffer[pos] = 'R';
        } else
        {
            slcan_port->uart_tx_buffer[pos] = 'r';
        }
    } else /* data frame */
    {
        if(slcan_port->can_rx_msg.eIdType == eCANFD_XID)
        {
            if(slcan_port->can_rx_msg.bBitRateSwitch)  	slcan_port->uart_tx_buffer[pos] = 'B';
            else if(slcan_port->can_rx_msg.bFDFormat ) 	slcan_port->uart_tx_buffer[pos] = 'D';
            else 																						slcan_port->uart_tx_buffer[pos] = 'T';
        } else
        {
            if(slcan_port->can_rx_msg.bBitRateSwitch)  	slcan_port->uart_tx_buffer[pos] = 'b';
            else if(slcan_port->can_rx_msg.bFDFormat ) 	slcan_port->uart_tx_buffer[pos] = 'd';
            else 																						slcan_port->uart_tx_buffer[pos] = 't';
        }
    }


    pos++;
    /* id */
    temp =  slcan_port->can_rx_msg.u32Id;
    if(slcan_port->can_rx_msg.eIdType == eCANFD_XID)
    {
        snprintf((char *)(slcan_port->uart_tx_buffer+pos), 9, "%08X", temp);
        pos += 8;
    } else
    {
        snprintf((char *)(slcan_port->uart_tx_buffer+pos), 4, "%03X", temp);
        pos += 3;
    }
    /* len */
    slcan_port->uart_tx_buffer[pos] = SLCAN_EncodeDLC(slcan_port->can_rx_msg.u32DLC);
    pos++;
    /* data */
    if(slcan_port->can_rx_msg.eFrmType !=  eCANFD_REMOTE_FRM)
    {
        for(int i=0; i < slcan_port->can_rx_msg.u32DLC; i++)
        {
            snprintf((char *)(slcan_port->uart_tx_buffer+pos), 3, "%02X", slcan_port->can_rx_msg.au8Data[i]);
            pos+=2;
        }
    }
    /* timestamp */
		
    if(slcan_port->set_flag & SLCAN_SFLAG_TIMESTAMP)
    {
        uint32_t tick = slcan_port->timestamp;
        sprintf((char *)(slcan_port->uart_tx_buffer+pos), "%04X", (tick & 0x0000FFFF) );
        pos += 4;
    }
    /* end */
    slcan_port->uart_tx_buffer[pos] = '\r';
    pos++;
    slcan_port->uart_tx_buffer[pos] = 0;
    return pos;
}


/**
 * @brief       can process
 * @param[in]   slcan_port : one can port will to be operated		
 * @return      
 * @details     can data ---> serial  data
 */
void slcan_process_can(struct slcan_t* slcan_port)
{
    int read_rc = 0;
    int tx_len = 0;

    if(g_u8RxFifo1CompleteFlag[slcan_port->candev_sn])
    {
        CANFD_LED(slcan_port->candev_sn,0);	//open LED
        g_u8RxFifo1CompleteFlag[slcan_port->candev_sn] = 0;
        read_rc = slcan_can_read(slcan_port);

        if(read_rc != sizeof(CANFD_FD_MSG_T) )
        {
            return;
        }
        tx_len = slcan_can2ascii(slcan_port);
        if(tx_len <= 0)
        {
            return;
        }
        slcan_uart_write(slcan_port, slcan_port->uart_tx_buffer, tx_len);

        //record receive data
#if CAN_RECV_RECORD == 1
        Record_CANmsg(data_sn[slcan_port->candev_sn],slcan_port->candev_sn,0,&slcan_port->can_rx_msg);
#endif

        memset(&slcan_port->can_rx_msg, 0, sizeof(CANFD_FD_MSG_T));

        DEBUG_MSG("SLCAN%d :COM[%d] << CAN[%d] len:[%d] data:[%s]\n\n",
                  slcan_port->candev_sn,slcan_port->uartdev_sn, slcan_port->candev_sn, tx_len, slcan_port->uart_tx_buffer);

        CANFD_LED(slcan_port->candev_sn,1);//close LED
    }
}



/**
 * @brief       both can and process
 * @param[in]   slcan_port : one can port will to be operated		
 * @return      
 * @details     
 */
void slcan_process_task(struct slcan_t* slcan_port)
{
    slcan_process_can(slcan_port);	//data from CANFD to VCOM
    slcan_process_uart(slcan_port);	//data from VCOM  to CANFD
}


/**
 * @brief       slcan instance initialization
 * @param[in]   
 * @return      
 * @details     
 */
void slcan_port0_init(void)
{
struct slcan_t* slcan= &slcan0;
	
	/*slcan port 0 initialization */
    slcan->candev_sn 			= 0;//CANFD Port0
    slcan->candev_cfg  		= sCANFD_Config[slcan->candev_sn];
    slcan->candev_oflag 	= 0;//can hardware switch flag

    slcan->uartdev_sn 		= 0;//USB VCOM1
    slcan->uartdev_oflag 	= 0;//uart hardware switch flag
    slcan->uart_rx_len 		= 0;

    slcan->run_state 			= 0;
    slcan->run_flag 			= 0;
    slcan->set_flag 			= 0;
    slcan->timestamp_isopen = 0;
    slcan->candev_isopen 	= 0;//cangaroo app switch flag

    DEBUG_MSG("SLCAN0 Port Init: CAN[%d]<=>VCOM[%d]\n",slcan->candev_sn, slcan->uartdev_sn);
}
 

#if  (SCLAN_NUM >=2 )
/**
 * @brief       slcan instance initialization
 * @param[in]   
 * @return      
 * @details     
 */
void slcan_port1_init(void)
{
struct slcan_t* slcan= &slcan1;
	
	/*slcan port 0 initialization */
    slcan->candev_sn 			= 1;//CANFD Port0
    slcan->candev_cfg  		= sCANFD_Config[slcan->candev_sn];
    slcan->candev_oflag 	= 0;//can hardware switch flag

    slcan->uartdev_sn 		= 1;//USB VCOM2
    slcan->uartdev_oflag 	= 0;//uart hardware switch flag
    slcan->uart_rx_len 		= 0;

    slcan->run_state 			= 0;
    slcan->run_flag 			= 0;
    slcan->set_flag 			= 0;
    slcan->timestamp_isopen = 0;
    slcan->candev_isopen 	= 0;//cangaroo app switch flag

    DEBUG_MSG("SLCAN1 Port Init: CAN[%d]<=>VCOM[%d]\n",slcan->candev_sn, slcan->uartdev_sn);
}
#endif

#if  (SCLAN_NUM >=3 )
/**
 * @brief       slcan instance initialization
 * @param[in]   
 * @return      
 * @details     
 */
void slcan_port2_init(void)
{
struct slcan_t* slcan= &slcan2;
	
	/*slcan port 0 initialization */
    slcan->candev_sn 			= 2;//CANFD Port0
    slcan->candev_cfg  		= sCANFD_Config[slcan->candev_sn];
    slcan->candev_oflag 	= 0;//can hardware switch flag

    slcan->uartdev_sn 		= 2;//USB VCOM3
    slcan->uartdev_oflag 	= 0;//uart hardware switch flag
    slcan->uart_rx_len 		= 0;

    slcan->run_state 			= 0;
    slcan->run_flag 			= 0;
    slcan->set_flag 			= 0;
    slcan->timestamp_isopen = 0;
    slcan->candev_isopen 	= 0;//cangaroo app switch flag

    DEBUG_MSG("SLCAN2 Port Init: CAN[%d]<=>VCOM[%d]\n",slcan->candev_sn, slcan->uartdev_sn);
}
#endif

#if  (SCLAN_NUM >=4)
/**
 * @brief       slcan instance initialization
 * @param[in]   
 * @return      
 * @details     
 */
void slcan_port3_init(void)
{
struct slcan_t* slcan= &slcan3;
	
	/*slcan port 0 initialization */
    slcan->candev_sn 			= 3;//CANFD Port0
    slcan->candev_cfg  		= sCANFD_Config[slcan->candev_sn];
    slcan->candev_oflag 	= 0;//can hardware switch flag

    slcan->uartdev_sn 		= 3;//USB VCOM4
    slcan->uartdev_oflag 	= 0;//uart hardware switch flag
    slcan->uart_rx_len 		= 0;

    slcan->run_state 			= 0;
    slcan->run_flag 			= 0;
    slcan->set_flag 			= 0;
    slcan->timestamp_isopen = 0;
    slcan->candev_isopen 	= 0;//cangaroo app switch flag

    DEBUG_MSG("SLCAN3 Port Init: CAN[%d]<=>VCOM[%d]\n",slcan->candev_sn, slcan->uartdev_sn);
}

#endif


/**
 * @brief       all slcan instance initialization one time
 * @param[in]   
 * @return      
 * @details     
 */
void slcan_init(void)
{
#if  (SCLAN_NUM >=1)
	slcan_port0_init();
#endif
#if  (SCLAN_NUM >=2)
	slcan_port1_init();
#endif
#if  (SCLAN_NUM >=3)
	slcan_port2_init();
#endif
#if  (SCLAN_NUM >=4)
	slcan_port3_init();
#endif
}


/**
 * @brief      slcan  timestamp  increase
 * @param[in]   
 * @return      
 * @details     
 */
void slcan_timestamp(void)
{

#if  (SCLAN_NUM >=1)
	if(slcan0.timestamp_isopen)   slcan0.timestamp++;
#endif
#if  (SCLAN_NUM >=2)
	if(slcan1.timestamp_isopen)   slcan1.timestamp++;
#endif
#if  (SCLAN_NUM >=3)
	if(slcan2.timestamp_isopen)   slcan2.timestamp++;
#endif
#if  (SCLAN_NUM >=4)
	if(slcan3.timestamp_isopen)   slcan3.timestamp++;
#endif

}

