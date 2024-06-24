
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


//���ڷ���DMAͨ����
#define UART0_PDMA_TX_CH     10

//���ڳ�ʱ,16����ǰ������λ�ź�
#define UART0_RX_TIMEOUT  0x10

#define UART0_BUFFER_SIZE  512

extern volatile uint8_t U0SendFlag ;
extern volatile uint8_t U0RecvFlag ;


void UART0_IO_Init(void);//�˿ڳ�ʼ��
void UART0_Init(void);//�����պͳ�ʱ�ж�
void UART0_PDMA_Init(void);//����DMA��ʼ��

void UART0_DMA_Send(uint8_t *sendbuf,uint16_t data_length);//DMA�������ݣ���������Ǿ�̬��ַ
void UART0_PDMA_CallBack(uint32_t status);//DMA��������ж�


#endif

