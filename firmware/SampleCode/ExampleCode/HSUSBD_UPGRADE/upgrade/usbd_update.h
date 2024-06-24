/**************************************************************************//**
 * @file     usbd_update.h
 * @version  V3.00
 * @brief    Search and read new firmware from USB drive and update APROM flash with it.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
 
 
#ifndef __USBD_UPDATE_H__
#define __USBD_UPDATE_H__
 
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"


#include "ff.h"
#include "diskio.h"

#define LOADER_BASE 0x0
#define LOADER_SIZE 0x8000
#define APP_BASE    0x8000
#define APP_SIZE    0x8000

#define FW_CRC_BASE        0x7F800
#define NEW_FW_CRC_BASE    0x7F804
#define BACKUP_FW_CRC_BASE 0x7F808



#define USBH_DRIVE          3               /* Assigned USBH drive number in FATFS        */


#define SPACE_APROM_INDEX0   0
#define SPACE_APROM_INDEX1   1
#define SPACE_APROM_INDEX2   2
#define SPACE_APROM_INDEX3   3
#define SPACE_DATA 				0xF0

#define SPACE_APROM_ADD0   0x00000
#define SPACE_APROM_ADD1   0x20000
#define SPACE_APROM_ADD2   0x40000
#define SPACE_APROM_ADD3   0x60000



//#define CARD_DETECT    CardDetect_From_DAT3
#define CARD_DETECT    CardDetect_From_GPIO


int  program_flash_page(uint32_t page_addr, uint32_t *buff, int count);

uint32_t firmware_check(uint32_t space);
void firmware_update(uint32_t base_addr,uint32_t space);

int32_t SDH_Open_Disk(SDH_T *sdh, uint32_t u32CardDetSrc);
void SDH_Close_Disk(SDH_T *sdh);

void JumptoAPROM(uint32_t vector);
extern uint8_t firmware_status[5];//aprom & data
uint32_t read_dfba(void);



#endif


