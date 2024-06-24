

#ifndef _DISKAPP_H_
#define _DISKAPP_H_



#include "NuMicro.h"
#include "diskio.h"     /* FatFs lower layer API */
#include "ff.h"

#include <stdio.h>
#include <string.h>

extern unsigned long volatile fattime;


//#define CARD_DETECT    CardDetect_From_DAT3
#define CARD_DETECT    CardDetect_From_GPIO

void Disk_IO_Init(void);
void Disk_Init(void);
int32_t Record_CANmsg(uint32_t sn,uint8_t channel, uint8_t rxtx,CANFD_FD_MSG_T *psMsg);
void RecordFileCheck(FIL *fil,FRESULT *res,uint8_t channel);

unsigned long get_fattime(void);

#endif


