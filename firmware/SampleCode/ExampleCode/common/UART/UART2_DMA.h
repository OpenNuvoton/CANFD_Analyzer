
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


//���ڷ���DMAͨ����
#define UART2_PDMA_TX_CH     2

//���ڳ�ʱ
#define UART2_RX_TIMEOUT  0x10

#define UART2_BUFFER_SIZE  256

//extern volatile uint8_t U2ErrorCode ;
extern volatile uint8_t U2SendFlag ;
extern volatile uint8_t U2RecvFlag ;


void UART2_IO_Init(void);//�˿ڳ�ʼ��
void UART2_Init(void);//�����պͳ�ʱ�ж�
void UART2_PDMA_Init(void);//����DMA��ʼ��

void UART2_DMA_Send(uint8_t *sendbuf,uint16_t data_lenght);//DMA�������ݣ���������Ǿ�̬��ַ
void UART2_PDMA_CallBack(uint32_t status);//DMA��������ж�


int16_t Read_O2sensor(void);

extern int16_t O2sensor[3];

#endif

