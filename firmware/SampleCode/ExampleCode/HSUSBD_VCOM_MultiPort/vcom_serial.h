/***************************************************************************//**
 * @file     vcom_serial.h
 * @version  V3.00
 * @brief    HSUSBD virtual COM header file.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __VCOM_SERIAL_H__
#define __VCOM_SERIAL_H__



#include "NuMicro.h"


#define VCOM_CNT 4

#if ((VCOM_CNT>=5) || (VCOM_CNT<=0))
#error VCOM number error, VCOM_CNT must be 1 ~ 4
#endif


#if (VCOM_CNT>=1)
#define IDX_VCOM0 0x0
#define INTF_NUM_VCOM0 (IDX_VCOM0*2)
#endif
#if (VCOM_CNT>=2)
#define IDX_VCOM1 0x1
#define INTF_NUM_VCOM1 (IDX_VCOM1*2)
#endif
#if (VCOM_CNT>=3)
#define IDX_VCOM2 0x2
#define INTF_NUM_VCOM2 (IDX_VCOM2*2)
#endif
#if (VCOM_CNT>=4)
#define IDX_VCOM3 0x3
#define INTF_NUM_VCOM3 (IDX_VCOM3*2)
#endif


//------------------device descriptor define---------------------//
/* Define the vendor id and product id */
#define USBD_VID        0x0416
#define USBD_PID        0x50A1


/* Define Descriptor information */
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  200  /* The unit is in 2mA. ex: 200 * 2mA = 400mA */


//------------------config descriptor define---------------------//

//VCOM_LEN = 8+9*2+(5+5+4+5)+7*3 = 66
//LEN_CONFIGURATION_TOTAL = 9 + 66*4 = 273
#define LEN_VCOM_COMMAND_INTR (0x5UL + 0x5UL + 0x4UL + 0x5UL)
#define VCOM_LEN ( LEN_IAD + (LEN_INTERFACE * 2) + LEN_VCOM_COMMAND_INTR + (LEN_ENDPOINT * 3))
#define LEN_CONFIGURATION_TOTAL (LEN_CONFIG+ (VCOM_LEN * VCOM_CNT))


/*!<USB Descriptor Length */
#define LEN_IAD             0x08ul
/*!<USB Descriptor Type */
#define DESC_IAD            0x0Bul
/*!<Define CDC Class Specific Descriptor */
#define DESC_CDC_COMMUNICATION_INTERFACE_CLASS       0x02
#define DESC_CDC_DATA_INTERFACE_CLASS                0x0A
#define DESC_CDC_ABSTRACT_CONTROL_MODEL              0x02
#define DESC_CDC_CS_INTERFACE                        0x24
#define DESC_CDC_HEADER                              0x00
#define DESC_CDC_CALL_MANAGEMENT                     0x01
#define DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define DESC_CDC_UNION                               0x06

//------------------cdc command define---------------------//
/*!<Define CDC Class Specific Request */
#define SET_LINE_CODE           0x20
#define GET_LINE_CODE           0x21
#define SET_CONTROL_LINE_STATE  0x22


//------------------ram buffer define---------------------//
/* Define DMA Maximum Transfer length */
#define USBD_MAX_DMA_LEN    0x1000
#define EP_MAX_DATA_PKT_SIZE   256

/* Define EP maximum packet size */
/*4096 Bytes Configurable RAM used as endpoint buffer*/
#define CEP_MAX_PKT_SIZE        64
#define CEP_OTHER_MAX_PKT_SIZE  64
//-----------------VCOM1-------------------//
#define EPA_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPA_OTHER_MAX_PKT_SIZE  64
#define EPB_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPB_OTHER_MAX_PKT_SIZE  64
#define EPC_MAX_PKT_SIZE        64
#define EPC_OTHER_MAX_PKT_SIZE  64
//-----------------VCOM2-------------------//
#define EPD_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPD_OTHER_MAX_PKT_SIZE  64
#define EPE_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPE_OTHER_MAX_PKT_SIZE  64
#define EPF_MAX_PKT_SIZE        64
#define EPF_OTHER_MAX_PKT_SIZE  64
//-----------------VCOM3-------------------//
#define EPG_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPG_OTHER_MAX_PKT_SIZE  64
#define EPH_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPH_OTHER_MAX_PKT_SIZE  64
#define EPI_MAX_PKT_SIZE        64
#define EPI_OTHER_MAX_PKT_SIZE  64
//-----------------VCOM4-------------------//
#define EPJ_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPJ_OTHER_MAX_PKT_SIZE  64
#define EPK_MAX_PKT_SIZE        EP_MAX_DATA_PKT_SIZE
#define EPK_OTHER_MAX_PKT_SIZE  64
#define EPL_MAX_PKT_SIZE        64
#define EPL_OTHER_MAX_PKT_SIZE  64


#define CEP_BUF_BASE    0
#define CEP_BUF_LEN     CEP_MAX_PKT_SIZE
//-----------------VCOM1-------------------//
#define EPA_BUF_BASE    0x100
#define EPA_BUF_LEN     EPA_MAX_PKT_SIZE
#define EPB_BUF_BASE    0x200
#define EPB_BUF_LEN     EPB_MAX_PKT_SIZE
#define EPC_BUF_BASE    0x300
#define EPC_BUF_LEN     EPC_MAX_PKT_SIZE
//-----------------VCOM2-------------------//
#define EPD_BUF_BASE    0x400
#define EPD_BUF_LEN     EPD_MAX_PKT_SIZE
#define EPE_BUF_BASE    0x500
#define EPE_BUF_LEN     EPE_MAX_PKT_SIZE
#define EPF_BUF_BASE    0x600
#define EPF_BUF_LEN     EPF_MAX_PKT_SIZE
//-----------------VCOM3-------------------//
#define EPG_BUF_BASE    0x700
#define EPG_BUF_LEN     EPG_MAX_PKT_SIZE
#define EPH_BUF_BASE    0x800
#define EPH_BUF_LEN     EPH_MAX_PKT_SIZE
#define EPI_BUF_BASE    0x900
#define EPI_BUF_LEN     EPI_MAX_PKT_SIZE
//-----------------VCOM4-------------------//
#define EPJ_BUF_BASE    0xA00
#define EPJ_BUF_LEN     EPJ_MAX_PKT_SIZE
#define EPK_BUF_BASE    0xB00
#define EPK_BUF_LEN     EPK_MAX_PKT_SIZE
#define EPL_BUF_BASE    0xC00
#define EPL_BUF_LEN     EPL_MAX_PKT_SIZE


/* Define the interrupt In EP number */

#if 0
#if (VCOM_CNT>=1)
#define BULK_IN_EP_NUM_0    0x01
#define BULK_OUT_EP_NUM_0   0x01
#define INT_IN_EP_NUM_0     0x02
#endif

#if (VCOM_CNT>=2)
#define BULK_IN_EP_NUM_1    0x03
#define BULK_OUT_EP_NUM_1   0x03
#define INT_IN_EP_NUM_1     0x04
#endif
#if (VCOM_CNT>=3)
#define BULK_IN_EP_NUM_2    0x05
#define BULK_OUT_EP_NUM_2   0x05
#define INT_IN_EP_NUM_2     0x06
#endif
#if (VCOM_CNT>=4)

#define BULK_IN_EP_NUM_3    0x07
#define BULK_OUT_EP_NUM_3   0x07
#define INT_IN_EP_NUM_3     0x08
#endif


#else

#if (VCOM_CNT>=1)
#define BULK_IN_EP_NUM_0    0x01
#define BULK_OUT_EP_NUM_0   0x02
#define INT_IN_EP_NUM_0     0x03
#endif

#if (VCOM_CNT>=2)
#define BULK_IN_EP_NUM_1    0x04
#define BULK_OUT_EP_NUM_1   0x05
#define INT_IN_EP_NUM_1     0x06
#endif
#if (VCOM_CNT>=3)
#define BULK_IN_EP_NUM_2    0x07
#define BULK_OUT_EP_NUM_2   0x08
#define INT_IN_EP_NUM_2     0x09
#endif
#if (VCOM_CNT>=4)

#define BULK_IN_EP_NUM_3    0x0A
#define BULK_OUT_EP_NUM_3   0x0B
#define INT_IN_EP_NUM_3     0x0C
#endif

#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define RXBUFSIZE           512 /* RX buffer size */
#define TXBUFSIZE           512 /* RX buffer size */

/************************************************/
/* for CDC class */
/* Line coding structure
  0-3 dwDTERate    Data terminal rate (baudrate), in bits per second
  4   bCharFormat  Stop bits: 0 - 1 Stop bit, 1 - 1.5 Stop bits, 2 - 2 Stop bits
  5   bParityType  Parity:    0 - None, 1 - Odd, 2 - Even, 3 - Mark, 4 - Space
  6   bDataBits    Data bits: 5, 6, 7, 8, 16  */

typedef struct
{
    uint32_t  u32DTERate;     /* Baud rate    */
    uint8_t   u8CharFormat;   /* stop bit     */
    uint8_t   u8ParityType;   /* parity       */
    uint8_t   u8DataBits;     /* data bits    */
} STR_VCOM_LINE_CODING;

extern STR_VCOM_LINE_CODING gLineCoding[VCOM_CNT];
extern uint16_t gCtrlSignal[VCOM_CNT];

struct vcom_var_t
{

#ifdef __ICCARM__
#pragma data_alignment=4
    uint8_t comRbuf[RXBUFSIZE];
    uint8_t comTbuf[TXBUFSIZE];
    uint8_t gRxBuf[64] = {0};
    uint8_t gUsbRxBuf[64] = {0};
#else
    uint8_t comRbuf[RXBUFSIZE] 										__attribute__((aligned(4)));
    uint8_t comTbuf[TXBUFSIZE]										__attribute__((aligned(4)));
    uint8_t gRxBuf[EP_MAX_DATA_PKT_SIZE] 					__attribute__((aligned(4)));
    uint8_t gUsbRxBuf[EP_MAX_DATA_PKT_SIZE] 			__attribute__((aligned(4)));
#endif

    uint16_t comRbytes;
    uint16_t comRhead;
    uint16_t comRtail;

    uint16_t comTbytes;
    uint16_t comThead;
    uint16_t comTtail;

    uint32_t gu32RxSize;
    uint32_t gu32TxSize;

    int8_t gi8BulkOutReady;
};

extern volatile struct vcom_var_t  vcom_dat[VCOM_CNT];


/*-------------------------------------------------------------*/
void VCOM_Init(void);
void VCOM_InitForHighSpeed(void);
void VCOM_InitForFullSpeed(void);
void VCOM_ClassRequest(void);

//VCOM0
void EPA_Handle(void);
void EPB_Handle(void);
//VCOM1
void EPD_Handle(void);
void EPE_Handle(void);
//VCOM2
void EPG_Handle(void);
void EPH_Handle(void);
//VCOM3
void EPJ_Handle(void);
void EPK_Handle(void);

extern void VCOM_LineCoding(uint8_t port);
extern void VCOM_TransferData(void);

#endif  /* __USBD_CDC_H_ */
