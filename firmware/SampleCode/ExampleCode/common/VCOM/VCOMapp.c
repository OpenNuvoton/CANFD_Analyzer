

/***************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Demonstrate how to implement an USB virtual COM port device.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "VCOMapp.h"
#include "canapp.h"



uint8_t usbactive=0;

/**
 * @brief       HSUSB port initialize
 * @param[in]   
 * @return      
 * @details     
 */
void USB_Port_Init(void)
{
    uint32_t volatile i;

    /* Select HSUSBD */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;

    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSUSBEN_Msk;
    for(i = 0; i < 0x1000; i++);   // delay > 10 us
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    /* Enable HSUSBD module clock */
    CLK_EnableModuleClock(HSUSBD_MODULE);

}

/**
 * @brief       HSUSB VCOM start to run
 * @param[in]   
 * @return      
 * @details     
 */
void VCOM_Start(void)
{

    HSUSBD_Open(&gsHSInfo, VCOM_ClassRequest, NULL);

    /* Endpoint configuration */
    VCOM_Init();

    /* Enable HSUSBD interrupt */
    NVIC_EnableIRQ(USBD20_IRQn);

}



/**
 * @brief       Transfer  Data  immediate
 * @param[in]   *p			: data point
 * @param[in]   u16Len	: data lenght
 * @return      data number send out
 * @details
 */
uint16_t USB_TransferData(uint8_t *p,uint16_t u16Len,uint8_t vcom)
{
    uint16_t i, MaxLen;

    /* Check if any data to send to USB & USB is ready to send them out */
    if(usbactive == 0) return 0;

    if(u16Len)
    {
        MaxLen = u16Len;
        if(MaxLen > EP_MAX_DATA_PKT_SIZE)
            MaxLen = EP_MAX_DATA_PKT_SIZE;

        switch(vcom)
        {

#if (VCOM_CNT>=1)
        case IDX_VCOM0:
            for(i = 0; i < u16Len; i++)
                HSUSBD->EP[EPA].EPDAT_BYTE 	= p[i];//data putin usb buffer
            HSUSBD->EP[EPA].EPRSPCTL 		= HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
            HSUSBD->EP[EPA].EPTXCNT 		= MaxLen;
            HSUSBD_ENABLE_EP_INT(EPA, HSUSBD_EPINTEN_INTKIEN_Msk);
            break;
#endif
#if (VCOM_CNT>=2)
        case IDX_VCOM1:
            for(i = 0; i < u16Len; i++)
                HSUSBD->EP[EPD].EPDAT_BYTE 	= p[i];//data putin usb buffer
            HSUSBD->EP[EPD].EPRSPCTL 		= HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
            HSUSBD->EP[EPD].EPTXCNT 		= u16Len;
            HSUSBD_ENABLE_EP_INT(EPD, HSUSBD_EPINTEN_INTKIEN_Msk);
            break;
#endif
#if (VCOM_CNT>=3)
        case IDX_VCOM2 :
            for(i = 0; i < u16Len; i++)
                HSUSBD->EP[EPG].EPDAT_BYTE 	= p[i];//data putin usb buffer
            HSUSBD->EP[EPG].EPRSPCTL 		= HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
            HSUSBD->EP[EPG].EPTXCNT 		= u16Len;
            HSUSBD_ENABLE_EP_INT(EPG, HSUSBD_EPINTEN_INTKIEN_Msk);
            break;
#endif
#if (VCOM_CNT>=4)
        case IDX_VCOM3 :
            for(i = 0; i < u16Len; i++)
                HSUSBD->EP[EPJ].EPDAT_BYTE 	= p[i];//data putin usb buffer
            HSUSBD->EP[EPJ].EPRSPCTL 		= HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
            HSUSBD->EP[EPJ].EPTXCNT 		= u16Len;
            HSUSBD_ENABLE_EP_INT(EPJ, HSUSBD_EPINTEN_INTKIEN_Msk);
            break;
#endif
        }
        return i;
    }
    else return 0;
}


/**
 * @brief       check transfer buffer data and push out at scheduled times.
 * @param[in]
 * @return
 * @details
 */
void VCOM_TransferData(void)
{
    int32_t i, i32Len;
    uint8_t cnt;

    /* Check if any data to send to USB & USB is ready to send them out */
    volatile struct vcom_var_t *vtp;
    if(usbactive == 0) return ;

    for(cnt=0; cnt<VCOM_CNT; cnt++)
    {   vtp=&vcom_dat[cnt];
        if(vtp->comRbytes)	//check data buffer not empty &&  usb atcive
        {
            i32Len = vtp->comRbytes;
            if(i32Len > EP_MAX_DATA_PKT_SIZE)
                i32Len = EP_MAX_DATA_PKT_SIZE;

            switch(cnt)
            {
#if (VCOM_CNT>=1)
            case IDX_VCOM0:
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPA].EPDAT_BYTE = 	com_getchar(cnt);					//data putin usb buffer
                HSUSBD->EP[EPA].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPA].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPA, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
#if (VCOM_CNT>=2)
            case IDX_VCOM1:
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPD].EPDAT_BYTE = 	com_getchar(cnt);					//data putin usb buffer
                HSUSBD->EP[EPD].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPD].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPD, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
#if (VCOM_CNT>=3)
            case IDX_VCOM2 :
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPG].EPDAT_BYTE = com_getchar(cnt);					//data putin usb buffer
                HSUSBD->EP[EPG].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPG].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPG, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
#if (VCOM_CNT>=3)
            case IDX_VCOM3 :
                for(i = 0; i < i32Len; i++)
                    HSUSBD->EP[EPJ].EPDAT_BYTE = com_getchar(cnt);					//data putin usb buffer
                HSUSBD->EP[EPJ].EPRSPCTL = HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                HSUSBD->EP[EPJ].EPTXCNT = i32Len;
                HSUSBD_ENABLE_EP_INT(EPJ, HSUSBD_EPINTEN_INTKIEN_Msk);
                break;
#endif
            }
        }
    }
}

/**
 * @brief       check usb bus is connected
 * @param[in]   
 * @return      
 * @details     
 */
void VCOMapp(void)
{
    if(usbactive == 1)
    {

        if(HSUSBD_IS_ATTACHED() == 0)		 	usbactive =0;
//				else 															VCOM_TransferData();
    }
    else
    {
        if(HSUSBD_IS_ATTACHED() )
        {
            HSUSBD_Start();
            usbactive = 1;
        }
    }
}

/**
 * @brief       get 1 byte data from mcu send buffer to vcom
 * @param[in]		vcom :com index
 * @return      1 byte data,if error return -1
 * @details   	mcu -> usb -> pc_vcom
 */
int32_t  com_getchar(uint8_t vcom)
{
    uint8_t bInChar;
    volatile struct vcom_var_t *vtp;
    vtp=&vcom_dat[vcom];
    if (vtp->comRbytes)
    {
        bInChar = vtp->comRbuf[vtp->comRhead++];
        if(vtp->comRhead >= RXBUFSIZE)
            vtp->comRhead = 0;
        vtp->comRbytes--;
    }
    else return -1;

    return bInChar;
}



/**
 * @brief       get 1 byte data from usb vcom buffer to mcu
 * @param[in]		vcom :com index
 * @return      1 byte data,if error return -1
 * @details   	pc_vcom -> usb -> mcu
 */
int32_t  usb_getchar(uint8_t vcom)
{
    uint8_t bInChar;
    volatile struct vcom_var_t *vtp;
    vtp=&vcom_dat[vcom];
    if (vtp->comTbytes)
    {
        bInChar = vtp->comTbuf[vtp->comThead++];
        if(vtp->comThead >= TXBUFSIZE)
            vtp->comThead = 0;
        vtp->comTbytes--;
    }
    else return -1;

    return bInChar;
}


/**
 * @brief       usb vcom send 1 byte data
 * @param[in]   bInChar : the data will be send
								vcom 		:	com index
 * @return      1 byte send data,if error return -1
 * @details   	mcu -> usb -> pc_vcom
 */
int32_t usb_putchar(char bInChar,uint8_t vcom)
{
    volatile struct vcom_var_t *vtp;
    vtp=&vcom_dat[vcom];
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
        return -1;
    }
    return bInChar;
}


/**
 * @brief       pc com send to usb recv in mcu
 * @param[in]   bInChar : the data will be send
								vcom 		:	com index
 * @return      1 byte send data,if error return -1
 * @details   	pc_vcom -> usb -> mcu
 */
int32_t com_putchar(char bInChar,uint8_t vcom)
{
    volatile struct vcom_var_t *vtp;
    vtp=&vcom_dat[vcom];
    if(vtp->comTbytes < TXBUFSIZE)
    {
        /* Enqueue the character */
        vtp->comTbuf[vtp->comTtail++] = bInChar;
        if(vtp->comTtail >= TXBUFSIZE)
            vtp->comTtail = 0;
        vtp->comTbytes++;
    }
    else
    {
        return -1;
    }

    return bInChar;
}

/**
 *  @brief      USB VCOM set uart config
 *  @param[in]  UART COM Index£º
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
//    uint32_t u32Baud_Div;
    volatile struct vcom_var_t *vtp=&vcom_dat[port];

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

//		uart_port[port]->LINE = u32Reg;

}

