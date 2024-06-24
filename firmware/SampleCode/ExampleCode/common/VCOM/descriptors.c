/**************************************************************************//**
 * @file     descriptors.c
 * @version  V3.00
 * @brief    HSUSBD descriptor.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
/*!<Includes */
#include "NuMicro.h"
#include "vcom_serial.h"



/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
uint8_t gu8DeviceDescriptor[] =
{
    LEN_DEVICE,     /* bLength */
    DESC_DEVICE,    /* bDescriptorType */
    0x00, 0x02,     /* bcdUSB:usb2.0.0 */

    0xEF,             /* bDeviceClass: miscellaneous device class */
    0x02,             /* bDeviceSubClass: common class */
    0x01,             /* bDeviceProtocol: IAD */

    CEP_MAX_PKT_SIZE,   /* bMaxPacketSize0 */
    /* idVendor */
    GET_BYTE0(USBD_VID),
    GET_BYTE1(USBD_VID),
    /* idProduct */
    GET_BYTE0(USBD_PID),
    GET_BYTE1(USBD_PID),
    0x00, 0x03,     /* bcdDevice */
    0x01,           /* iManufacture */
    0x02,           /* iProduct */
    0x03,           /* iSerialNumber  */
    0x01            /* bNumConfigurations */
};

/*!<USB Qualifier Descriptor */
uint8_t gu8QualifierDescriptor[] =
{
    LEN_QUALIFIER,  /* bLength */
    DESC_QUALIFIER, /* bDescriptorType */
    0x00, 0x02,     /* bcdUSB */

    0xEF,           /* bDeviceClass: miscellaneous device class */
    0x02,           /* bDeviceSubClass: common class */
    0x01,           /* bDeviceProtocol: IAD */

    CEP_OTHER_MAX_PKT_SIZE, /* bMaxPacketSize0 */
    0x01,           /* bNumConfigurations */
    0x00
};


/*!<USB Configure Descriptor */
uint8_t gu8ConfigDescriptor[] =
{
    LEN_CONFIG,  /* bLength              */
    DESC_CONFIG, /* bDescriptorType      */
    GET_BYTE0(LEN_CONFIGURATION_TOTAL),
    GET_BYTE1(LEN_CONFIGURATION_TOTAL), /* wTotalLength         */
    VCOM_CNT * 2,                       /* bNumInterfaces       */
    0x01,                               /* bConfigurationValue  */
    0x00,                               /* iConfiguration       */
    0xC0,                               /* bmAttributes         */
    USBD_MAX_POWER,                     /* MaxPower             */

//-----------------VCOM0-------------------//
#if (VCOM_CNT >= 1)
    // IAD
    LEN_IAD,        // bLength        : Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM0, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass     : CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,                          /* bLength              */
    DESC_INTERFACE,                         /* bDescriptorType      */
    INTF_NUM_VCOM0,                         /* bInterfaceNumber     */
    0x00,                                   /* bAlternateSetting    */
    0x01,                                   /* bNumEndpoints        */
    DESC_CDC_COMMUNICATION_INTERFACE_CLASS, /* bInterfaceClass      */
    DESC_CDC_ABSTRACT_CONTROL_MODEL,        /* bInterfaceSubClass   */
    0x01,                                   /* bInterfaceProtocol   */
    0x00,                                   /* iInterface           */
//LEN_VCOM_COMMAND_INTR
    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,           				/* Header functional descriptor subtype */
    0x10, 0x01,     									/* Communication device compliant to the communication spec. ver. 1.10 */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT,         /* Call management functional descriptor */
    0x00,           									/* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM0 + 1,           		/* Interface number of data class interface optionally used for call management */

    /* Communication Class Specified INTERFACE descriptor */
    0x04,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MODEL, 	/* Abstract control management functional descriptor subtype */
    0x00,                            	/* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  						/* bLength              */
    DESC_CDC_CS_INTERFACE, 						/* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        						/* bDescriptorSubType:Union Functional Descriptor   */
    INTF_NUM_VCOM0,        						/* bMasterInterface     */
    INTF_NUM_VCOM0 + 1,    						/* bSlaveInterface0     */
//END  ILEN_VCOM_COMMAND_INTR

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                 		/* bLength          */
    DESC_ENDPOINT,                		/* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_0), 		/* bEndpointAddress */
    EP_INT,                       		/* bmAttributes     */
    GET_BYTE0(EPC_MAX_PKT_SIZE),
    GET_BYTE1(EPC_MAX_PKT_SIZE), 			/* wMaxPacketSize   */
    0x01,                        			/* bInterval:1 ms   */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM0 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass:CDC-Data */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                  	/* bLength          */
    DESC_ENDPOINT,                 	/* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_0), 		/* bEndpointAddress */
    EP_BULK,                       	/* bmAttributes     */
    GET_BYTE0(EPA_MAX_PKT_SIZE),
    GET_BYTE1(EPA_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_0), 	/* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    GET_BYTE0(EPB_MAX_PKT_SIZE),
    GET_BYTE1(EPB_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */
#endif
//-----------------VCOM1-------------------//
#if (VCOM_CNT >= 2)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM1, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM1, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM1 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM1,        /* bMasterInterface     */
    INTF_NUM_VCOM1 + 1,    /* bSlaveInterface0     */


    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_1),   /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPF_MAX_PKT_SIZE),
    GET_BYTE1(EPF_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM1 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_1),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPD_MAX_PKT_SIZE),
    GET_BYTE1(EPD_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_1),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPE_MAX_PKT_SIZE),
    GET_BYTE1(EPE_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM2-------------------//
#if (VCOM_CNT >= 3)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM2, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM2 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM2,        /* bMasterInterface     */
    INTF_NUM_VCOM2 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_2),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPI_MAX_PKT_SIZE),
    GET_BYTE1(EPI_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_2),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPG_MAX_PKT_SIZE),
    GET_BYTE1(EPG_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_2),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPH_MAX_PKT_SIZE),
    GET_BYTE1(EPH_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM3-------------------//
#if (VCOM_CNT >= 4)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM3, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM3 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM3,        /* bMasterInterface     */
    INTF_NUM_VCOM3 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_3),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPL_MAX_PKT_SIZE),
    GET_BYTE1(EPL_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_3),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPJ_MAX_PKT_SIZE),
    GET_BYTE1(EPJ_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_3),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPK_MAX_PKT_SIZE),
    GET_BYTE1(EPK_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif

};
/*!<USB Other Speed Configure Descriptor */
uint8_t gu8OtherConfigDescriptorHS[] =
{
    LEN_CONFIG,  /* bLength              */
    DESC_OTHERSPEED, /* bDescriptorType      */
    GET_BYTE0(LEN_CONFIGURATION_TOTAL),
    GET_BYTE1(LEN_CONFIGURATION_TOTAL), /* wTotalLength         */
    VCOM_CNT * 2,                       /* bNumInterfaces       */
    0x01,                               /* bConfigurationValue  */
    0x00,                               /* iConfiguration       */
    0xC0,                               /* bmAttributes         */
    USBD_MAX_POWER,                     /* MaxPower             */

//-----------------VCOM0-------------------//
#if (VCOM_CNT >= 1)
    // IAD
    LEN_IAD,        // bLength        : Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM0, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass     : CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,                          /* bLength              */
    DESC_INTERFACE,                         /* bDescriptorType      */
    INTF_NUM_VCOM0,                         /* bInterfaceNumber     */
    0x00,                                   /* bAlternateSetting    */
    0x01,                                   /* bNumEndpoints        */
    DESC_CDC_COMMUNICATION_INTERFACE_CLASS, /* bInterfaceClass      */
    DESC_CDC_ABSTRACT_CONTROL_MODEL,        /* bInterfaceSubClass   */
    0x01,                                   /* bInterfaceProtocol   */
    0x00,                                   /* iInterface           */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,           				/* Header functional descriptor subtype */
    0x10, 0x01,     									/* Communication device compliant to the communication spec. ver. 1.10 */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT,         /* Call management functional descriptor */
    0x00,           									/* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM0 + 1,           		/* Interface number of data class interface optionally used for call management */

    /* Communication Class Specified INTERFACE descriptor */
    0x04,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MODEL, 	/* Abstract control management functional descriptor subtype */
    0x00,                            	/* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  						/* bLength              */
    DESC_CDC_CS_INTERFACE, 						/* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        						/* bDescriptorSubType   */
    INTF_NUM_VCOM0,        						/* bMasterInterface     */
    INTF_NUM_VCOM0 + 1,    						/* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                 		/* bLength          */
    DESC_ENDPOINT,                		/* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_0), 			/* bEndpointAddress */
    EP_INT,                       		/* bmAttributes     */
    GET_BYTE0(EPC_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPC_OTHER_MAX_PKT_SIZE), 			/* wMaxPacketSize   */
    0x01,                        			/* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM0 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                  	/* bLength          */
    DESC_ENDPOINT,                 	/* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_0), 		/* bEndpointAddress */
    EP_BULK,                       	/* bmAttributes     */
    GET_BYTE0(EPA_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPA_OTHER_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_0), 	/* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    GET_BYTE0(EPB_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPB_OTHER_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */
#endif
//-----------------VCOM1-------------------//
#if (VCOM_CNT >= 2)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM1, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM1, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM1 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM1,        /* bMasterInterface     */
    INTF_NUM_VCOM1 + 1,    /* bSlaveInterface0     */


    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_1),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPF_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPF_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM1 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_1),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPD_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPD_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_1),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPE_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPE_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM2-------------------//
#if (VCOM_CNT >= 3)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM2, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM2 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM2,        /* bMasterInterface     */
    INTF_NUM_VCOM2 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_2),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPI_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPI_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_2),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPG_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPG_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_2),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPH_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPH_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM3-------------------//
#if (VCOM_CNT >= 4)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM3, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM3 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM3,        /* bMasterInterface     */
    INTF_NUM_VCOM3 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_3),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPL_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPL_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_3),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPJ_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPJ_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_3),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPK_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPK_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
};

uint8_t gu8ConfigDescriptorFS[] =
{
    LEN_CONFIG,  /* bLength              */
    DESC_CONFIG, /* bDescriptorType      */
    GET_BYTE0(LEN_CONFIGURATION_TOTAL),
    GET_BYTE1(LEN_CONFIGURATION_TOTAL), /* wTotalLength         */
    VCOM_CNT * 2,                       /* bNumInterfaces       */
    0x01,                               /* bConfigurationValue  */
    0x00,                               /* iConfiguration       */
    0xC0,                               /* bmAttributes         */
    USBD_MAX_POWER,                     /* MaxPower             */

//-----------------VCOM0-------------------//
#if (VCOM_CNT >= 1)
    //IAD:Interface Association Descriptor
    LEN_IAD,        // bLength        : Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM0, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass     : CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,                          /* bLength              */
    DESC_INTERFACE,                         /* bDescriptorType      */
    INTF_NUM_VCOM0,                         /* bInterfaceNumber     */
    0x00,                                   /* bAlternateSetting    */
    0x01,                                   /* bNumEndpoints        */
    DESC_CDC_COMMUNICATION_INTERFACE_CLASS, /* bInterfaceClass      */
    DESC_CDC_ABSTRACT_CONTROL_MODEL,        /* bInterfaceSubClass   */
    0x01,                                   /* bInterfaceProtocol   */
    0x00,                                   /* iInterface           */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,           				/* Header functional descriptor subtype */
    0x10, 0x01,     									/* Communication device compliant to the communication spec. ver. 1.10 */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT,         /* Call management functional descriptor */
    0x00,           									/* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM0 + 1,           		/* Interface number of data class interface optionally used for call management */

    /* Communication Class Specified INTERFACE descriptor */
    0x04,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MODEL, 	/* Abstract control management functional descriptor subtype */
    0x00,                            	/* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  						/* bLength              */
    DESC_CDC_CS_INTERFACE, 						/* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        						/* bDescriptorSubType   */
    INTF_NUM_VCOM0,        						/* bMasterInterface     */
    INTF_NUM_VCOM0 + 1,    						/* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                 		/* bLength          */
    DESC_ENDPOINT,                		/* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_0), 			/* bEndpointAddress */
    EP_INT,                       		/* bmAttributes     */
    GET_BYTE0(EPC_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPC_OTHER_MAX_PKT_SIZE), 			/* wMaxPacketSize   */
    0x01,                        			/* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM0 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                  	/* bLength          */
    DESC_ENDPOINT,                 	/* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_0), 		/* bEndpointAddress */
    EP_BULK,                       	/* bmAttributes     */
    GET_BYTE0(EPA_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPA_OTHER_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_0), 	/* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    GET_BYTE0(EPB_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPB_OTHER_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */
#endif
//-----------------VCOM1-------------------//
#if (VCOM_CNT >= 2)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM1, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM1, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM1 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM1,        /* bMasterInterface     */
    INTF_NUM_VCOM1 + 1,    /* bSlaveInterface0     */


    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_1),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPF_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPF_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM1 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_1),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPD_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPD_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_1),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPE_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPE_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM2-------------------//
#if (VCOM_CNT >= 3)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM2, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM2 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM2,        /* bMasterInterface     */
    INTF_NUM_VCOM2 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_2),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPI_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPI_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_2),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPG_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPG_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_2),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPH_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPH_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM3-------------------//
#if (VCOM_CNT >= 4)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM3, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM3 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM3,        /* bMasterInterface     */
    INTF_NUM_VCOM3 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_3),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPL_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPL_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_3),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPJ_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPJ_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_3),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPK_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPK_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
};

/*!<USB Other Speed Configure Descriptor */
uint8_t gu8OtherConfigDescriptorFS[] =
{
    LEN_CONFIG,  /* bLength              */
    DESC_OTHERSPEED, /* bDescriptorType      */
    GET_BYTE0(LEN_CONFIGURATION_TOTAL),
    GET_BYTE1(LEN_CONFIGURATION_TOTAL), /* wTotalLength         */
    VCOM_CNT * 2,                       /* bNumInterfaces       */
    0x01,                               /* bConfigurationValue  */
    0x00,                               /* iConfiguration       */
    0xC0,                               /* bmAttributes         */
    USBD_MAX_POWER,                     /* MaxPower             */

//-----------------VCOM0-------------------//
#if (VCOM_CNT >= 1)
    // IAD
    LEN_IAD,        // bLength        : Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM0, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass     : CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,                          /* bLength              */
    DESC_INTERFACE,                         /* bDescriptorType      */
    INTF_NUM_VCOM0,                         /* bInterfaceNumber     */
    0x00,                                   /* bAlternateSetting    */
    0x01,                                   /* bNumEndpoints        */
    DESC_CDC_COMMUNICATION_INTERFACE_CLASS, /* bInterfaceClass      */
    DESC_CDC_ABSTRACT_CONTROL_MODEL,        /* bInterfaceSubClass   */
    0x01,                                   /* bInterfaceProtocol   */
    0x00,                                   /* iInterface           */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,           				/* Header functional descriptor subtype */
    0x10, 0x01,     									/* Communication device compliant to the communication spec. ver. 1.10 */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT,         /* Call management functional descriptor */
    0x00,           									/* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM0 + 1,           		/* Interface number of data class interface optionally used for call management */

    /* Communication Class Specified INTERFACE descriptor */
    0x04,           									/* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,           	/* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MODEL, 	/* Abstract control management functional descriptor subtype */
    0x00,                            	/* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  						/* bLength              */
    DESC_CDC_CS_INTERFACE, 						/* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        						/* bDescriptorSubType   */
    INTF_NUM_VCOM0,        						/* bMasterInterface     */
    INTF_NUM_VCOM0 + 1,    						/* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                 		/* bLength          */
    DESC_ENDPOINT,                		/* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_0), 		/* bEndpointAddress */
    EP_INT,                       		/* bmAttributes     */
    GET_BYTE0(EPC_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPC_OTHER_MAX_PKT_SIZE), 			/* wMaxPacketSize   */
    0x01,                        			/* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM0 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                  	/* bLength          */
    DESC_ENDPOINT,                 	/* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_0), 		/* bEndpointAddress */
    EP_BULK,                       	/* bmAttributes     */
    GET_BYTE0(EPA_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPA_OTHER_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_0), 	/* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    GET_BYTE0(EPB_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPB_OTHER_MAX_PKT_SIZE), 		/* wMaxPacketSize   */
    0x00,                        		/* bInterval        */
#endif
//-----------------VCOM1-------------------//
#if (VCOM_CNT >= 2)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM1, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM1, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM1 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM1,        /* bMasterInterface     */
    INTF_NUM_VCOM1 + 1,    /* bSlaveInterface0     */


    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_1),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPF_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPF_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,      /* bLength              */
    DESC_INTERFACE,     /* bDescriptorType      */
    INTF_NUM_VCOM1 + 1, /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x0A,               /* bInterfaceClass      */
    0x00,               /* bInterfaceSubClass   */
    0x00,               /* bInterfaceProtocol   */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_1),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPD_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPD_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_1),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPE_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPE_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM2-------------------//
#if (VCOM_CNT >= 3)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM2, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM2 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM2,        /* bMasterInterface     */
    INTF_NUM_VCOM2 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_2),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPI_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPI_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM2+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_2),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPG_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPG_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_2),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPH_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPH_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
//-----------------VCOM3-------------------//
#if (VCOM_CNT >= 4)
    // IAD
    LEN_IAD,        // bLength: Interface Descriptor size
    DESC_IAD,       // bDescriptorType: IAD
    INTF_NUM_VCOM3, // bFirstInterface
    0x02,           // bInterfaceCount
    0x02,           // bFunctionClass: CDC
    0x02,           // bFunctionSubClass
    0x01,           // bFunctionProtocol
    0x02,           // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3, /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Header Functional descriptor */
    0x05,                  /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE, /* CS_INTERFACE descriptor type */
    DESC_CDC_HEADER,       /* Header functional descriptor subtype */
    0x10, 0x01, 					 /* Communication device compliant to the communication spec.ver. 1.10 */

    /* Call Management Functional descriptor */
    0x05,                     /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,    /* CS_INTERFACE descriptor type */
    DESC_CDC_CALL_MANAGEMENT, /* Call management functional descriptor */
    0x00, /* BIT0: Whether device handle call management itself. */
    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    INTF_NUM_VCOM3 + 1, /* Interface number of data class interface optionally used for call management */

    /* Abstract Control Management Functional Descriptor  */
    0x04,                                 /* Size of the descriptor, in bytes */
    DESC_CDC_CS_INTERFACE,                /* CS_INTERFACE descriptor type */
    DESC_CDC_ABSTRACT_CONTROL_MANAGEMENT, /* Abstract control management
                                             functional descriptor subtype */
    0x00,                                 /* bmCapabilities       */

    /* Union Functional descriptor */
    0x05,                  /* bLength              */
    DESC_CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE descriptor type */
    DESC_CDC_UNION,        /* bDescriptorSubType   */
    INTF_NUM_VCOM3,        /* bMasterInterface     */
    INTF_NUM_VCOM3 + 1,    /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM_3),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPL_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPL_OTHER_MAX_PKT_SIZE),
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    INTF_NUM_VCOM3+1,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM_3),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPJ_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPJ_OTHER_MAX_PKT_SIZE),
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM_3),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    /* wMaxPacketSize */
    GET_BYTE0(EPK_OTHER_MAX_PKT_SIZE),
    GET_BYTE1(EPK_OTHER_MAX_PKT_SIZE),
    0x00,                            /* bInterval        */
#endif
};



//=============String Descriptor=============//

/*!<USB Language String Descriptor */
uint8_t gu8StringLang[4] =
{
    4,           /* bLength */
    DESC_STRING, /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
uint8_t gu8VendorStringDesc[] = {
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
uint8_t gu8ProductStringDesc[] = {
    32,          /* bLength          */
    DESC_STRING, /* bDescriptorType  */
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'V', 0, 'i', 0, 'r', 0, 't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0, 'M', 0
};

uint8_t gu8StringSerial[26] = {
    26,          // bLength
    DESC_STRING, // bDescriptorType
    'A', 0, '0', 0, '2', 0, '0', 0, '2', 0, '4', 0, '0', 0, '3', 0, '0', 0, '8', 0, '0', 0, '1', 0
};

uint8_t *gpu8UsbString[4] = {
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8StringSerial
};



uint8_t *gu8UsbHidReport[3] =
{
    NULL,
    NULL,
    NULL
};

uint32_t gu32UsbHidReportLen[3] =
{
    0,
    0,
    0
};

uint32_t gu32ConfigHidDescIdx[3] =
{
    0,
    0,
    0
};

S_HSUSBD_INFO_T gsHSInfo =
{
    gu8DeviceDescriptor,
    gu8ConfigDescriptor,
    gpu8UsbString,
    gu8QualifierDescriptor,
    gu8ConfigDescriptorFS,
    gu8OtherConfigDescriptorHS,
    gu8OtherConfigDescriptorFS,
    NULL,											//bos Descriptor
    gu8UsbHidReport,
    gu32UsbHidReportLen,
    gu32ConfigHidDescIdx
};
