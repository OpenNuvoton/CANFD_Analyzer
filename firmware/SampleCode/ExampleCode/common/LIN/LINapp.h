

#ifndef __LINAPP_H__
#define __LINAPP_H__


#include "NuMicro.h"
#include "vcom_serial.h"

#define LIN_ID      0x30
#define LIN_MODE    1  //1:master;0:slave
#define LINBUFSIZE  32

struct LIN_var_t
{
		uint8_t id;
    uint8_t buf[LINBUFSIZE];
		uint8_t bytes;
};

extern struct LIN_var_t lin_var;

uint8_t ComputeChksumValue(uint8_t *pu8Buf, uint32_t u32ByteCnt);
uint8_t GetParityValue(uint32_t u32id);
void LIN_Init(uint8_t ch);
uint32_t LIN_Write(uint8_t ch, uint8_t id,uint8_t pu8TxBuf[], uint32_t u32WriteBytes);
uint32_t LIN_Write_Header(uint8_t ch, uint8_t id);

#endif
