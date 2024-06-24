
/**************************************************************************//**
 * @file     
 * @version  V1.00
 * @brief    NuMicro peripheral access layer header file.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __UART0_DMA_H__
#define __UART0_DMA_H__



#include "NuMicro.h"
#include <string.h>


//串口发送DMA通道号
#define UART0_PDMA_TX_CH     10

//串口超时,16个当前波特率位信号
#define UART0_RX_TIMEOUT  0x10

#define UART0_BUFFER_SIZE  512

extern volatile uint8_t U0SendFlag ;
extern volatile uint8_t U0RecvFlag ;


void UART0_IO_Init(void);//端口初始化
void UART0_Init(void);//开接收和超时中断
void UART0_PDMA_Init(void);//发送DMA初始化

void UART0_DMA_Send(uint8_t *sendbuf,uint16_t data_length);//DMA发送数据，数组必须是静态地址
void UART0_PDMA_CallBack(uint32_t status);//DMA发送完成中断


#endif

