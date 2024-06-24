
/**************************************************************************//**
 * @file     
 * @version  V1.00
 * @brief    
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2016-2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __USER_TIMER_H__
#define __USER_TIMER_H__



#include <stdio.h>
#include <string.h>

#include "NuMicro.h"

void TIME_Init(void);

uint8_t GetSecondFlag(void);
void ClrSecondFlag(void);
uint32_t GetTick(void);

//This function provides accurate delay (in milliseconds) based on variable incremented.
void Tick_Delay(uint32_t Delayms);



#endif


