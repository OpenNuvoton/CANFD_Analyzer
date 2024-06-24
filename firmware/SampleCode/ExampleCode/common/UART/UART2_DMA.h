
/**************************************************************************//**
 * @file     
 * @version  V1.00
 * @brief    NuMicro peripheral access layer header file.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __UART2_DMA_H__
#define __UART2_DMA_H__



#include "NuMicro.h"
#include <string.h>


//串口发送DMA通道号
#define UART2_PDMA_TX_CH     2

//串口超时
#define UART2_RX_TIMEOUT  0x10

#define UART2_BUFFER_SIZE  256

//extern volatile uint8_t U2ErrorCode ;
extern volatile uint8_t U2SendFlag ;
extern volatile uint8_t U2RecvFlag ;


void UART2_IO_Init(void);//端口初始化
void UART2_Init(void);//开接收和超时中断
void UART2_PDMA_Init(void);//发送DMA初始化

void UART2_DMA_Send(uint8_t *sendbuf,uint16_t data_lenght);//DMA发送数据，数组必须是静态地址
void UART2_PDMA_CallBack(uint32_t status);//DMA发送完成中断


int16_t Read_O2sensor(void);

extern int16_t O2sensor[3];

#endif

