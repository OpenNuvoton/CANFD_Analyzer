
/***************************************************************************//**
 * @file     vcomapp.h
 * @version  V1.0
 * @brief    vcom function header file.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef _VCOMAPP_H_
#define _VCOMAPP_H_



#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "vcom_serial.h"


extern uint8_t usbactive;

void USB_Port_Init(void);
void VCOMapp(void);
void VCOM_Start(void);
//Transfer  Data  immediate
uint16_t USB_TransferData(uint8_t *p,uint16_t u16Len,uint8_t vcom);
//check transfer buffer data and push out at scheduled times.
void VCOM_TransferData(void);

//Data flow : mcu -> usb -> pc_vcom
int32_t com_getchar(uint8_t vcom);
//Data flow : pc_vcom -> usb -> mcu
int32_t usb_getchar(uint8_t vcom);


//Data flow : mcu -> usb -> pc_vcom
int32_t usb_putchar(char bInChar,uint8_t vcom);
//Data flow : pc_vcom -> usb -> mcu
int32_t com_putchar(char bInChar,uint8_t vcom);


int multi_printf(const char *str, ...);


#define DEBUG_MSG    multi_printf

#endif  /*_VCOMAPP_H_*/


