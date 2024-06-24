
/**************************************************************************//**
 * @file			user_timer.c
 * @version  	V1.00
 * @brief    
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "user_timer.h"
#include "slcan.h"

static volatile uint32_t u32_TimeTick;
static volatile uint8_t second_flag=0;

extern unsigned long volatile can_timestamp;

extern void VCOMapp(void);



void TIME_Init(void)
{
		CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);
	
	  /* Open Timer3 in periodic mode, enable interrupt and 1000 interrupt ticks per second */
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);
    TIMER_Start(TIMER3);		
}




void TMR3_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        /* Clear Timer3 time-out interrupt flag */
      TIMER_ClearIntFlag(TIMER3);
			u32_TimeTick++;
			slcan_timestamp();
			VCOMapp();//Check whether the USB is connected
			
			if(u32_TimeTick%1000 == 0)
			{
			second_flag = 1;	
			}
    }
}


uint8_t GetSecondFlag(void)
{
	return second_flag;
}

void ClrSecondFlag(void)
{
	 second_flag = 0;
}

uint32_t GetTick(void)
{
    return u32_TimeTick;
}

//This function provides accurate delay (in milliseconds) based on variable incremented.
void Tick_Delay(uint32_t Delayms)
{
    uint32_t tickstart = GetTick();
    uint32_t wait = Delayms;

    while ((GetTick() - tickstart) < wait)
    {
    }
}

void start_timer0(void)
{
    /* Start TIMER0  */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_TMR0SEL_Msk)) | CLK_CLKSEL1_TMR0SEL_HXT;
    CLK->APBCLK0 |= CLK_APBCLK0_TMR0CKEN_Msk;    /* enable TIMER0 clock                  */
    TIMER0->CTL = 0;                   /* disable timer                                  */
    TIMER0->INTSTS = (TIMER_INTSTS_TWKF_Msk | TIMER_INTSTS_TIF_Msk);  /* clear interrupt status */
    TIMER0->CMP = 0xFFFFFE;            /* maximum time                                   */
    TIMER0->CNT = 0;                   /* clear timer counter                            */
    /* start timer */
    TIMER0->CTL = (11 << TIMER_CTL_PSC_Pos) | TIMER_ONESHOT_MODE | TIMER_CTL_CNTEN_Msk;
}

void stop_timer0(void)
{
    TIMER0->CTL = 0;                   /* disable timer   */  
}	

uint32_t  get_timer0_counter(void)
{
    return TIMER0->CNT;
}

