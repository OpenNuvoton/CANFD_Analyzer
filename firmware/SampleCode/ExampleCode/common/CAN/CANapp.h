/**************************************************************************//**
 * @file     
 * @version  
 * @brief    An example of interrupt control using CAN FD bus communication.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef _CANAPP_H_
#define _CANAPP_H_

#include "stdio.h"
#include "string.h"
#include "NuMicro.h"
#include "vcomapp.h"

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/



#define NK_M467HJ    	(0)

#define CANFD_NUM   	(4)
#define CANABLE  			(0)
#define CAN_HW_FILTER (0) 

#if NK_M467HJ == 1
#define CAN0LED  		PH4
#else
#define CAN0LED  		PA8
#endif

#define  CAN0					0
#define  CAN1   		 	1
#define  CAN2   		 	2
#define  CAN3    		 	3

#define CAN1LED  	PF5
#define CAN2LED  	PF6
#define CAN3LED  	PF4


#define CAN_RECV_ACK  		1
#define CAN_RECV_RECORD   0

#define DEF_NormBitRate	  500000
#define DEF_DataBitRate  5000000

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
extern  CANFD_FD_MSG_T    g_sRxMsgFrame[CANFD_NUM];    	//one RX message buffer, 
extern  CANFD_FD_MSG_T    g_sTxMsgFrame[CANFD_NUM];			//one TX message buffer
extern 	CANFD_FD_T 				sCANFD_Config[CANFD_NUM];			//CANFD config information
extern volatile uint8_t   g_u8RxFifo1CompleteFlag[CANFD_NUM];		//when receive 1 message will be set TRUE
/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/* 
	@param[in] "channel" 		: which CANFD port will be operation
	@param[in] "psCanfdStr" : CANFD port configuration information
	@param[in] "psMsg"			:	CANFD Message information
	@param[in] "bufsn"			: which fifo buffer serial number will be operation
	@param[in] "txrx"				: the message is receive in or send out
*/
/*---------------------------------------------------------------------------------------------------------*/
void CANFD_Start(void);
void CANFD_Task(void);
void CANFD_Fini(uint8_t channel);
void CAN_Port_Init(void);
void CANFD_LED(uint8_t channel,uint8_t status);

//config CANFD
void CANFD_Initial(uint8_t channel,CANFD_FD_T *psCanfdStr);//config message RAM
void CANFD_BitRate_Init(CANFD_T *psCanfd, CANFD_FD_T *psCanfdStr);//config bitrate
void CANFD_Set_Mode(uint8_t channel,CANFD_FD_T *psCanfdStr);//config work mode 
//check CANFD bus for errors
void CANFD_BusCheck(uint8_t channel);
//printf in or out message
void CANFD_ShowMessage(uint8_t channel,uint8_t txrx,CANFD_FD_MSG_T *psMsg);

//put message into the "bufsn" TX fifo buffer
uint32_t CANFD_BUFSet(uint8_t channel, uint8_t bufsn,CANFD_FD_MSG_T *psMsg);
//send out the message in "bufsn"  fifo buffer
void CANFD_BUFSend(uint8_t channel, uint8_t bufsn);
//use "bufsn"  fifo buffer to direct transmission
void CANFD_MsgSend(uint8_t channel,uint8_t bufsn, CANFD_FD_MSG_T *psMsg);


#endif
