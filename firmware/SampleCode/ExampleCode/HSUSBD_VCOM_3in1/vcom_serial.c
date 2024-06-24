/***************************************************************************//**
 * @file     vcom_serial.c
 * @version  V3.00
 * @brief    HSUSBD virtual COM sample file.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include "NuMicro.h"
#include "vcom_serial.h"
#include "LINapp.h"

/*--------------------------------------------------------------------------*/
/* Stop bit     */
/* parity       */
/* data bits    */
STR_VCOM_LINE_CODING gLineCoding[VCOM_CNT] =
#if (VCOM_CNT>=1)
{   {19200, 0, 0, 8},   /* Baud rate : 115200    */
#endif
#if (VCOM_CNT>=2)
    {115200, 0, 0, 8},   /* Baud rate : 115200    */
#endif
#if (VCOM_CNT>=3)
    {115200, 0, 0, 8},   /* Baud rate : 115200    */
#endif
#if (VCOM_CNT>=4)
    {115200, 0, 0, 8},   /* Baud rate : 115200    */
#endif
};


uint16_t gCtrlSignal[VCOM_CNT] =      /* BIT0: DTR(Data Terminal Ready) , BIT1: RTS(Request To Send) */
#if (VCOM_CNT>=1)
{   0,
#endif
#if (VCOM_CNT>=2)
    0,
#endif
#if (VCOM_CNT>=3)
    0,
#endif
#if (VCOM_CNT>=4)
    0,
#endif
};


volatile struct vcom_var_t  vcom_dat[VCOM_CNT];

/*--------------------------------------------------------------------------*/
void USBD20_IRQHandler(void)
{
    __IO uint32_t IrqStL, IrqSt;

    IrqStL = HSUSBD->GINTSTS & HSUSBD->GINTEN;    /* get interrupt status */

    if(!IrqStL)    return;

    /* USB interrupt */
    if(IrqStL & HSUSBD_GINTSTS_USBIF_Msk)
    {
        IrqSt = HSUSBD->BUSINTSTS & HSUSBD->BUSINTEN;

        if(IrqSt & HSUSBD_BUSINTSTS_SOFIF_Msk)
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SOFIF_Msk);

        if(IrqSt & HSUSBD_BUSINTSTS_RSTIF_Msk)
        {
            HSUSBD_SwReset();
            HSUSBD_ResetDMA();

#if (VCOM_CNT>=1)
            HSUSBD->EP[EPA].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
            HSUSBD->EP[EPB].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
#endif
#if (VCOM_CNT>=2)
            HSUSBD->EP[EPD].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
            HSUSBD->EP[EPE].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
#endif
#if (VCOM_CNT>=3)
            HSUSBD->EP[EPG].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
            HSUSBD->EP[EPH].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
#endif
#if (VCOM_CNT>=4)
            HSUSBD->EP[EPJ].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
            HSUSBD->EP[EPK].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
#endif


            if(HSUSBD->OPER & 0x04)   /* high speed */
                VCOM_InitForHighSpeed();
            else                    	/* full speed */
                VCOM_InitForFullSpeed();

            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            HSUSBD_SET_ADDR(0);
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RSTIF_Msk);
            HSUSBD_CLR_CEP_INT_FLAG(0x1ffc);
        }

        if(IrqSt & HSUSBD_BUSINTSTS_RESUMEIF_Msk)
        {
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RESUMEIF_Msk);
        }

        if(IrqSt & HSUSBD_BUSINTSTS_SUSPENDIF_Msk)
        {
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SUSPENDIF_Msk);
        }

        if(IrqSt & HSUSBD_BUSINTSTS_HISPDIF_Msk)
        {
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_HISPDIF_Msk);
        }
//--------------//
//        if(IrqSt & HSUSBD_BUSINTSTS_DMADONEIF_Msk)
//        {
//            g_hsusbd_DmaDone = 1;
//            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_DMADONEIF_Msk);

//            if(!(HSUSBD->DMACTL & HSUSBD_DMACTL_DMARD_Msk))
//            {
//                HSUSBD_ENABLE_EP_INT(EPB, HSUSBD_EPINTEN_RXPKIEN_Msk);
//            }

//            if(HSUSBD->DMACTL & HSUSBD_DMACTL_DMARD_Msk)
//            {
//                if(g_hsusbd_ShortPacket == 1)
//                {
//                    HSUSBD->EP[EPA].EPRSPCTL = (HSUSBD->EP[EPA].EPRSPCTL & 0x10) | HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
//                    g_hsusbd_ShortPacket = 0;
//                }
//            }
//        }
//--------------//

        if(IrqSt & HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk)
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk);

        if(IrqSt & HSUSBD_BUSINTSTS_VBUSDETIF_Msk)
        {
            if(HSUSBD_IS_ATTACHED())
            {
                /* USB Plug In */
                HSUSBD_ENABLE_USB();
            }
            else
            {
                /* USB Un-plug */
                HSUSBD_DISABLE_USB();
            }
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_VBUSDETIF_Msk);
        }
    }

    if(IrqStL & HSUSBD_GINTSTS_CEPIF_Msk)
    {
        IrqSt = HSUSBD->CEPINTSTS & HSUSBD->CEPINTEN;

        if(IrqSt & HSUSBD_CEPINTSTS_SETUPTKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPTKIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_SETUPPKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPPKIF_Msk);
            HSUSBD_ProcessSetupPacket();
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_OUTTKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_OUTTKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_INTKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
            if(!(IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk))
            {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk);
                HSUSBD_CtrlIn();
            }
            else
            {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            }
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_PINGIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_PINGIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_TXPKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            if(g_hsusbd_CtrlInSize)
            {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
            }
            else
            {
                if(g_hsusbd_CtrlZero == 1)
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_ZEROLEN);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            }
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_RXPKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_RXPKIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_NAKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_NAKIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_STALLIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STALLIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_ERRIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_ERRIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk)
        {
            HSUSBD_UpdateDeviceState();
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_BUFFULLIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFFULLIF_Msk);
            return;
        }

        if(IrqSt & HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk);
            return;
        }
    }
//-----------------VCOM0-------------------//
    /* bulk in */
    if(IrqStL & HSUSBD_GINTSTS_EPAIF_Msk)
    {
        EPA_Handle();
    }
    /* bulk out */
    if(IrqStL & HSUSBD_GINTSTS_EPBIF_Msk)
    {
        EPB_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPCIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPC].EPINTSTS & HSUSBD->EP[EPC].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPC, IrqSt);
    }
//-----------------VCOM1-------------------//
    if(IrqStL & HSUSBD_GINTSTS_EPDIF_Msk)
    {
        EPD_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPEIF_Msk)
    {
        EPE_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPFIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPF].EPINTSTS & HSUSBD->EP[EPF].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPF, IrqSt);
    }
//-----------------VCOM2-------------------//
    if(IrqStL & HSUSBD_GINTSTS_EPGIF_Msk)
    {
        EPG_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPHIF_Msk)
    {
        EPH_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPIIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPI].EPINTSTS & HSUSBD->EP[EPI].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPI, IrqSt);
    }
//-----------------VCOM3-------------------//
    if(IrqStL & HSUSBD_GINTSTS_EPJIF_Msk)
    {
        EPJ_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPKIF_Msk)
    {
        EPK_Handle();
    }

    if(IrqStL & HSUSBD_GINTSTS_EPLIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPL].EPINTSTS & HSUSBD->EP[EPL].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPL, IrqSt);
    }
}


void VCOM_InitForHighSpeed(void)
{

#if (VCOM_CNT>=1)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPA, EPA_BUF_BASE, EPA_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPA, EPA_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPA, BULK_IN_EP_NUM_0, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPB, EPB_BUF_BASE, EPB_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPB, EPB_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPB, BULK_OUT_EP_NUM_0, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPB, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPC, EPC_BUF_BASE, EPC_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPC, EPC_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPC, INT_IN_EP_NUM_0, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
#if (VCOM_CNT>=2)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPD, EPD_BUF_BASE, EPD_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPD, EPD_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPD, BULK_IN_EP_NUM_1, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPE, EPE_BUF_BASE, EPE_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPE, EPE_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPE, BULK_OUT_EP_NUM_1, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPE, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPF, EPF_BUF_BASE, EPF_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPF, EPF_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPF, INT_IN_EP_NUM_1, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
#if (VCOM_CNT>=3)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPG, EPG_BUF_BASE, EPG_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPG, EPG_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPG, BULK_IN_EP_NUM_2, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPH, EPH_BUF_BASE, EPH_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPH, EPH_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPH, BULK_OUT_EP_NUM_2, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPH, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPI, EPI_BUF_BASE, EPI_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPI, EPI_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPI, INT_IN_EP_NUM_2, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
#if (VCOM_CNT>=4)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPJ, EPJ_BUF_BASE, EPJ_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPJ, EPJ_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPJ, BULK_IN_EP_NUM_3, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPK, EPK_BUF_BASE, EPK_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPK, EPK_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPK, BULK_OUT_EP_NUM_3, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPK, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPL, EPL_BUF_BASE, EPL_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPL, EPL_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPL, INT_IN_EP_NUM_3, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
}

void VCOM_InitForFullSpeed(void)
{

#if (VCOM_CNT>=1)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPA, EPA_BUF_BASE, EPA_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPA, EPA_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPA, BULK_IN_EP_NUM_0, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPB, EPB_BUF_BASE, EPB_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPB, EPB_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPB, BULK_OUT_EP_NUM_0, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPB, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPC, EPC_BUF_BASE, EPC_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPC, EPC_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPC, INT_IN_EP_NUM_0, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
#if (VCOM_CNT>=2)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPD, EPD_BUF_BASE, EPD_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPD, EPD_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPD, BULK_IN_EP_NUM_1, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPE, EPE_BUF_BASE, EPE_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPE, EPE_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPE, BULK_OUT_EP_NUM_1, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPE, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPF, EPF_BUF_BASE, EPF_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPF, EPF_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPF, INT_IN_EP_NUM_1, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
#if (VCOM_CNT>=3)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPG, EPG_BUF_BASE, EPG_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPG, EPG_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPG, BULK_IN_EP_NUM_2, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPH, EPH_BUF_BASE, EPH_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPH, EPH_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPH, BULK_OUT_EP_NUM_2, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPH, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPI, EPI_BUF_BASE, EPI_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPI, EPI_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPI, INT_IN_EP_NUM_2, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
#if (VCOM_CNT>=4)
    /*****************************************************/
    /* EPA ==> Bulk IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPJ, EPJ_BUF_BASE, EPJ_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPJ, EPJ_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPJ, BULK_IN_EP_NUM_3, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_IN);

    /* EPB ==> Bulk OUT endpoint, address 2 */
    HSUSBD_SetEpBufAddr(EPK, EPK_BUF_BASE, EPK_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPK, EPK_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPK, BULK_OUT_EP_NUM_3, HSUSBD_EP_CFG_TYPE_BULK, HSUSBD_EP_CFG_DIR_OUT);
    HSUSBD_ENABLE_EP_INT(EPK, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPL, EPL_BUF_BASE, EPL_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPL, EPL_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPL, INT_IN_EP_NUM_3, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
#endif
}

/*--------------------------------------------------------------------------*/
/**
  * @brief  USBD Endpoint Config.
  * @param  None.
  * @retval None.
  */
void VCOM_Init(void)
{
    /* Configure USB controller */
    /* Enable USB BUS, CEP and EPA , EPB global interrupt */
    HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk | HSUSBD_GINTEN_CEPIEN_Msk |
#if (VCOM_CNT>=1)
                          HSUSBD_GINTEN_EPAIEN_Msk | HSUSBD_GINTEN_EPBIEN_Msk | HSUSBD_GINTEN_EPCIEN_Msk 		//VCOM0
#endif
#if (VCOM_CNT>=2)
                          |HSUSBD_GINTEN_EPDIEN_Msk | HSUSBD_GINTEN_EPEIEN_Msk | HSUSBD_GINTEN_EPFIEN_Msk 	//VCOM1
#endif
#if (VCOM_CNT>=3)
                          |HSUSBD_GINTEN_EPGIEN_Msk | HSUSBD_GINTEN_EPHIEN_Msk | HSUSBD_GINTEN_EPIIEN_Msk 	//VCOM2
#endif
#if (VCOM_CNT>=4)
                          |HSUSBD_GINTEN_EPJIEN_Msk | HSUSBD_GINTEN_EPKIEN_Msk | HSUSBD_GINTEN_EPLIEN_Msk 	//VCOM3
#endif
                         );
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_VBUSDETIEN_Msk);
    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);

    /*****************************************************/
    /* Control endpoint */
    HSUSBD_SetEpBufAddr(CEP, CEP_BUF_BASE, CEP_BUF_LEN);
    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);

    VCOM_InitForHighSpeed();
}

/**
 *  @brief      USB Control endpoint to Config UART port
 *  @param[in]  None
 *  @return     None
 *  @details    The function is used to:
								Read UART Config information
								Write UART Config information
*/
void VCOM_ClassRequest(void)
{   static uint8_t intface;
    if(gUsbCmd.bmRequestType & 0x80)    /* request data transfer direction */
    {
        // Device to host
        switch(gUsbCmd.bRequest)
        {
        case GET_LINE_CODE:
        {
            intface = gUsbCmd.wIndex & 0xff;
            HSUSBD_PrepareCtrlIn((uint8_t *)&gLineCoding[intface>>1], 7);

            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
            break;
        }
        default:
        {
            /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
            break;
        }
        }
    }
    else
    {
        // Host to device
        switch(gUsbCmd.bRequest)
        {
        case SET_CONTROL_LINE_STATE:
        {
            intface = gUsbCmd.wIndex & 0xff;
            gCtrlSignal[intface>>1] = gUsbCmd.wValue;

            // DATA IN for end of setup
            /* Status stage */
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            break;
        }
        case SET_LINE_CODE:
        {
            intface = gUsbCmd.wIndex & 0xff;
            HSUSBD_CtrlOut((uint8_t *)&gLineCoding[intface>>1], 7);

            /* Status stage */
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);

            /* UART setting */
            VCOM_LineCoding(intface>>1);
            break;
        }
        default:
        {
            /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
            break;
        }
        }
    }
}



//-----------------VCOM0-------------------//
void EPA_Handle(void) //Send complete endpoint
{   //data out	mcu
    volatile uint32_t	IrqSt = HSUSBD->EP[EPA].EPINTSTS & HSUSBD->EP[EPA].EPINTEN;
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM0];

    vtp->gu32TxSize = 0;
    HSUSBD_ENABLE_EP_INT(EPA, 0);
    HSUSBD_CLR_EP_INT_FLAG(EPA, IrqSt);
}

void EPB_Handle(void) //Receive complete endpoint
{   //data in mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPB].EPINTSTS & HSUSBD->EP[EPB].EPINTEN;
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM0];
    int volatile i;

    vtp->gu32RxSize = HSUSBD->EP[EPB].EPDATCNT & 0xffff;
    for(i = 0; i < vtp->gu32RxSize; i++)
        vtp->gUsbRxBuf[i] = HSUSBD->EP[EPB].EPDAT_BYTE;

    /* Set a flag to indicate bulk out ready */
    vtp->gi8BulkOutReady = 1;
    HSUSBD_CLR_EP_INT_FLAG(EPB, IrqSt);

}

//-----------------VCOM1-------------------//
void EPD_Handle(void)
{   //data out mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPD].EPINTSTS & HSUSBD->EP[EPD].EPINTEN;

#if (VCOM_CNT>=2)
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM1];
    vtp->gu32TxSize = 0;
#endif
    HSUSBD_ENABLE_EP_INT(EPD, 0);
    HSUSBD_CLR_EP_INT_FLAG(EPD, IrqSt);
}

void EPE_Handle(void)
{   //data in mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPE].EPINTSTS & HSUSBD->EP[EPE].EPINTEN;

#if (VCOM_CNT>=2)
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM1];
    int volatile i;

    vtp->gu32RxSize = HSUSBD->EP[EPE].EPDATCNT & 0xffff;
    for(i = 0; i < vtp->gu32RxSize; i++)
        vtp->gUsbRxBuf[i] = HSUSBD->EP[EPE].EPDAT_BYTE;

    /* Set a flag to indicate bulk out ready */
    vtp->gi8BulkOutReady = 1;
#endif
    HSUSBD_CLR_EP_INT_FLAG(EPE, IrqSt);
}
//-----------------VCOM2-------------------//
void EPG_Handle(void)
{   //data out mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPG].EPINTSTS & HSUSBD->EP[EPG].EPINTEN;

#if (VCOM_CNT>=3)
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM2];
    vtp->gu32TxSize = 0;
#endif

    HSUSBD_ENABLE_EP_INT(EPG, 0);
    HSUSBD_CLR_EP_INT_FLAG(EPG, IrqSt);
}

void EPH_Handle(void)
{   //data in mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPH].EPINTSTS & HSUSBD->EP[EPH].EPINTEN;
#if (VCOM_CNT>=3)
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM2];
    int volatile i;

    vtp->gu32RxSize = HSUSBD->EP[EPH].EPDATCNT & 0xffff;
    for(i = 0; i < vtp->gu32RxSize; i++)
        vtp->gUsbRxBuf[i] = HSUSBD->EP[EPH].EPDAT_BYTE;

    /* Set a flag to indicate bulk out ready */
    vtp->gi8BulkOutReady = 1;
#endif
    HSUSBD_CLR_EP_INT_FLAG(EPH, IrqSt);
}
//-----------------VCOM3-------------------//
void EPJ_Handle(void)
{   //data out mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPJ].EPINTSTS & HSUSBD->EP[EPJ].EPINTEN;
#if (VCOM_CNT>=4)
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM3];
    vtp->gu32TxSize = 0;
#endif
    HSUSBD_ENABLE_EP_INT(EPJ, 0);
    HSUSBD_CLR_EP_INT_FLAG(EPJ, IrqSt);
}

void EPK_Handle(void)
{   //data in mcu
    volatile uint32_t IrqSt = HSUSBD->EP[EPK].EPINTSTS & HSUSBD->EP[EPK].EPINTEN;

#if (VCOM_CNT>=4)
    volatile struct vcom_var_t *vtp=&vcom_dat[IDX_VCOM3];
    int volatile i;

    vtp->gu32RxSize = HSUSBD->EP[EPK].EPDATCNT & 0xffff;
    for(i = 0; i < vtp->gu32RxSize; i++)
        vtp->gUsbRxBuf[i] = HSUSBD->EP[EPK].EPDAT_BYTE;

    /* Set a flag to indicate bulk out ready */
    vtp->gi8BulkOutReady = 1;
#endif

    HSUSBD_CLR_EP_INT_FLAG(EPK, IrqSt);
}

