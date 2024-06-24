/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    
 *           
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"

#include "CANapp.h"
#include "VCOMapp.h"
#include "slcan.h"
#include "OLED4PINiic.h"

#include "user_timer.h"
#include "uart0_dma.h"

#if  NK_M467HJ == 1
	#define BOARDLED  PH4
#else
  #define BOARDLED  PC14          
#endif


volatile const char makeData[] = __DATE__; //12bytes,build data
volatile const char makeTime[] = __TIME__; //9bytes,build time


void SYS_Init(void)
{
    /* Set PF multi-function pins for XT1_OUT(PF.2) and XT1_IN(PF.3) */
    SET_XT1_OUT_PF2();
    SET_XT1_IN_PF3();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable HIRC and HXT clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk | CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HIRC and HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HXTSTB_Msk);

    /* Set PCLK0 and PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Set core clock to 192MHz */
    CLK_SetCoreClock(FREQ_200MHZ);

    /* Enable all GPIO clock */
    CLK->AHBCLK0 |= CLK_AHBCLK0_GPACKEN_Msk | CLK_AHBCLK0_GPBCKEN_Msk | CLK_AHBCLK0_GPCCKEN_Msk | CLK_AHBCLK0_GPDCKEN_Msk |
                    CLK_AHBCLK0_GPECKEN_Msk | CLK_AHBCLK0_GPFCKEN_Msk | CLK_AHBCLK0_GPGCKEN_Msk | CLK_AHBCLK0_GPHCKEN_Msk;
    CLK->AHBCLK1 |= CLK_AHBCLK1_GPICKEN_Msk | CLK_AHBCLK1_GPJCKEN_Msk;


    CLK_SetSysTickClockSrc(CLK_CLKSEL0_STCLKSEL_HXT);

    /* Enable PDMA0 module clock */
    CLK_EnableModuleClock(PDMA0_MODULE);

}



int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    UART0_IO_Init();
    USB_Port_Init();
    CAN_Port_Init();

    /* Init UART to 115200-8n1 for print message */
    UART0_Init();

    DEBUG_MSG("\n\n+------------------------+\n");
    DEBUG_MSG("|  Boot from 0x%08X  |\n", FMC_GetVECMAP());
    DEBUG_MSG("+------------------------+\n");
    DEBUG_MSG("	CPU @ %d MHz\n", SystemCoreClock/1000000);
    DEBUG_MSG("+------------------------+\n");
    DEBUG_MSG("|      CAN to SLCAN      |\n");
    DEBUG_MSG("+------------------------+\n");
    DEBUG_MSG("Data: %s  \nTime:%s\n",makeData, makeTime);

    /* Configure Status LED PIN Output mode */
    GPIO_SetMode(PF, BIT4|BIT5|BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT8|BIT9, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);

    CANFD_Start();			//CANFD Start Define Config
    slcan_init();				


    VCOM_Start();
    TIME_Init();

    SYS_LockReg();
		
		OLED_IO_init();
		OLED_init();
		
		OLED_clear();
		OLED_ShowString(0,0,"VCOM0 <-> SLCAN0",0)	;			


#if  NK_M467HJ == 1
    GPIO_SetMode(PH, BIT4|BIT5|BIT6, GPIO_MODE_OUTPUT);
#else
		OLED_ShowString(0,2,"VCOM1 <-> SLCAN1",0)	;			
		OLED_ShowString(0,4,"VCOM2 <-> SLCAN2",0)	;					
		OLED_ShowString(0,6,"VCOM3 <-> SLCAN3",0)	;			
#endif


    while(1)
    {

        if(GetSecondFlag())
        {   ClrSecondFlag();
						BOARDLED = !BOARDLED;
        }

#if  NK_M467HJ == 1
        slcan_process_task(&slcan0);
#else
        slcan_process_task(&slcan0);
        slcan_process_task(&slcan1);
        slcan_process_task(&slcan2);
        slcan_process_task(&slcan3);
#endif

    }
}
