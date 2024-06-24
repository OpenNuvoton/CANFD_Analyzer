/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Use SD as back end storage media to simulate an USB pen drive.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "massstorage.h"
#include "usbd_update.h"
#include "user_boot.h"

#include "OLED4PINiic.h"

#define OLED_DISP  1

uint8_t volatile g_u8SdInitFlag = 0;
extern int32_t g_TotalSectors;
static volatile uint32_t s_u32GetSum;


/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */
/*---------------------------------------------------------*/
unsigned long get_fattime(void)
{
    unsigned long tmr;
    tmr = 0x00000;
    return tmr;
}

/*--------------------------------------------------------------------------*/

void SDH0_IRQHandler(void)
{
    unsigned int volatile isr;
    unsigned int volatile ier;

    // FMI data abort interrupt
    if(SDH0->GINTSTS & SDH_GINTSTS_DTAIF_Msk)
    {
        /* ResetAllEngine() */
        SDH0->GCTL |= SDH_GCTL_GCTLRST_Msk;
    }

    //----- SD interrupt status
    isr = SDH0->INTSTS;
    if(isr & SDH_INTSTS_BLKDIF_Msk)
    {
        // block down
        SD0.DataReadyFlag = TRUE;
        SDH0->INTSTS = SDH_INTSTS_BLKDIF_Msk;
    }

    if(isr & SDH_INTSTS_CDIF_Msk)
    {
        // card detect
        //----- SD interrupt status
        // it is work to delay 50 times for SD_CLK = 200KHz
        {
            int volatile i;         // delay 30 fail, 50 OK
            for(i = 0; i < 0x500; i++); // delay to make sure got updated value from REG_SDISR.
            isr = SDH0->INTSTS;
        }

        if(isr & SDH_INTSTS_CDSTS_Msk)
        {
            printf("\n***** card remove !\n");
            SD0.IsCardInsert = FALSE;   // SDISR_CD_Card = 1 means card remove for GPIO mode
            memset(&SD0, 0, sizeof(SDH_INFO_T));
        }
        else
        {
            printf("***** card insert !\n");
            SDH_Open(SDH0, CardDetect_From_GPIO);
            if(SDH_Probe(SDH0))
            {
                g_u8SdInitFlag = 0;
                printf("SD initial fail!!\n");
            }
            else
            {
                g_u8SdInitFlag = 1;
                g_TotalSectors = SD0.totalSectorN;
            }
        }
        SDH0->INTSTS = SDH_INTSTS_CDIF_Msk;
    }

    // CRC error interrupt
    if(isr & SDH_INTSTS_CRCIF_Msk)
    {
        if(!(isr & SDH_INTSTS_CRC16_Msk))
        {
            //printf("***** ISR sdioIntHandler(): CRC_16 error !\n");
            // handle CRC error
        }
        else if(!(isr & SDH_INTSTS_CRC7_Msk))
        {
            if(!SD0.R3Flag)
            {
                //printf("***** ISR sdioIntHandler(): CRC_7 error !\n");
                // handle CRC error
            }
        }
        SDH0->INTSTS = SDH_INTSTS_CRCIF_Msk;      // clear interrupt flag
    }

    if(isr & SDH_INTSTS_DITOIF_Msk)
    {
        printf("***** ISR: data in timeout !\n");
        SDH0->INTSTS |= SDH_INTSTS_DITOIF_Msk;
    }

    // Response in timeout interrupt
    if(isr & SDH_INTSTS_RTOIF_Msk)
    {
        printf("***** ISR: response in timeout !\n");
        SDH0->INTSTS |= SDH_INTSTS_RTOIF_Msk;
    }
}

void TIME_Init(void)
{
//100ms¶¨Ê±ÖÐ¶Ï		
		CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);
	
	  /* Open Timer3 in periodic mode, enable interrupt and 100 interrupt ticks per second */
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 10);
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);
    TIMER_Start(TIMER3);	

		CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);	
}


void SYS_Init(void)
{
    uint32_t volatile i;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable HIRC and HXT clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk | CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LIRCEN_Msk);

    /* Wait for HIRC and HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HXTSTB_Msk | CLK_PWRCTL_LIRCEN_Msk);

    /* Set PCLK0 and PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Set core clock to 200MHz */
    CLK_SetCoreClock(FREQ_200MHZ);

    /* Enable all GPIO clock */
    CLK->AHBCLK0 |= CLK_AHBCLK0_GPACKEN_Msk | CLK_AHBCLK0_GPBCKEN_Msk | CLK_AHBCLK0_GPCCKEN_Msk | CLK_AHBCLK0_GPDCKEN_Msk |
                    CLK_AHBCLK0_GPECKEN_Msk | CLK_AHBCLK0_GPFCKEN_Msk | CLK_AHBCLK0_GPGCKEN_Msk | CLK_AHBCLK0_GPHCKEN_Msk;
    CLK->AHBCLK1 |= CLK_AHBCLK1_GPICKEN_Msk | CLK_AHBCLK1_GPJCKEN_Msk;

    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Select HSUSBD */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;

    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSUSBEN_Msk;
    for(i = 0; i < 0x1000; i++);   // delay > 10 us
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    /* Enable HSUSBD module clock */
    CLK_EnableModuleClock(HSUSBD_MODULE);
    /* Enable SDH0 module clock */
    CLK_EnableModuleClock(SDH0_MODULE);
    /* Enable CRC module clock */
    CLK_EnableModuleClock(CRC_MODULE);
    /* Select SDH0 module clock source as HCLK and SDH0 module clock divider as 10 */
    CLK_SetModuleClock(SDH0_MODULE, CLK_CLKSEL0_SDH0SEL_HCLK, CLK_CLKDIV0_SDH0(10));

		TIME_Init();
		

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set multi-function pins for UART0 RXD and TXD */
#if   0
		SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();
#else		
    SET_UART0_RXD_PA15();
    SET_UART0_TXD_PA14();		
#endif

    /* Select multi-function pins for SD0 */
//    SET_SD0_DAT0_PE2();
//    SET_SD0_DAT1_PE3();
//    SET_SD0_DAT2_PE4();
//    SET_SD0_DAT3_PE5();
//    SET_SD0_CLK_PE6();
//    SET_SD0_CMD_PE7();
//    SET_SD0_nCD_PD13();

			SET_SD0_DAT0_PB2();
			SET_SD0_DAT1_PB3();
			SET_SD0_DAT2_PB4();
			SET_SD0_DAT3_PB5();
			SET_SD0_CLK_PB1();
			SET_SD0_CMD_PB0();		
	
#if 1
    SYS->GPF_MFP0 = 0;
    GPIO_SetMode(PF, BIT0|BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PF, BIT0|BIT1, GPIO_MODE_INPUT);
		GPIO_SetPullCtl(PF, BIT0|BIT1, GPIO_PUSEL_PULL_UP);
		GPIO_SET_DEBOUNCE_TIME(PF, GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_4);
		GPIO_ENABLE_DEBOUNCE(PF, BIT0|BIT1);
#endif

	  GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);	
    /* Lock protected registers */
    SYS_LockReg();
}

//make sd card as usb disk
void Run_USB_Disk(void)
{

    HSUSBD_Open(&gsHSInfo, MSC_ClassRequest, NULL);

    /* Endpoint configuration */
    MSC_Init();

    /* Enable HSUSBD interrupt */
    NVIC_EnableIRQ(USBD20_IRQn);

    /* Start transaction */
    while(1)
    {
        if(HSUSBD_IS_ATTACHED())
        {
            HSUSBD_Start();
            break;
        }
    }

    while(1)
    {
        if(g_hsusbd_Configured)
            MSC_ProcessCmd();
    }
}



void TMR3_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        /* Clear Timer3 time-out interrupt flag */
      TIMER_ClearIntFlag(TIMER3);
			PC14 = ~PC14;//when run usb disk ,the LED flash fast
    }
}

//#define KEY_upgrade  (PF0 == 0)

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function 
keil need compiler vesion 5
*/
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
  uint16_t KEY_upgrade; 
	uint32_t boot_vector;
	
	/* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 912600);
	
		printf("\n\n");
		printf("+------------------------+\n");
		printf("|  Boot from 0x%08X  |\n", FMC_GetVECMAP());
		printf("+------------------------+\n");

    /* initial SD card */
		g_u8SdInitFlag = SDH_Open_Disk(SDH0, CARD_DETECT);
	
	
		GPIO_SetMode(PF, BIT0|BIT1, GPIO_MODE_INPUT);
	  KEY_upgrade = (PF->PIN)&0x03;
	
	#if OLED_DISP  == 1		
			OLED_IO_init();
			OLED_init();
			OLED_clear();		
	#endif	

	  switch(KEY_upgrade)
		{case 1:
			if(g_u8SdInitFlag)
			{		
				
			printf("\n+------------------------+\n");
			printf("|       USB  DISK        |\n");
			printf("+------------------------+\n");
				
	#if OLED_DISP  == 1	
				
			Picture_display(nuvoton_disk);   // delay_ms(1000);					
	#endif			
					
				Run_USB_Disk();	
			}	
			break;
			
			
		case 2:
			TIMER_Stop(TIMER3);	
			printf("\n+------------------------+\n");
			printf("|       Boot Swtich      |\n");
			printf("+------------------------+\n");
			boot_vector = BootSwitch();
				break;
		default:boot_vector=0;
		}
			

    SYS_UnlockReg();                   /* Unlock register lock protect */	
		
		
		//aprom
	
		if(firmware_check(SPACE_APROM_INDEX0) && g_u8SdInitFlag)
		{ firmware_update(SPACE_APROM_ADD0,SPACE_APROM_INDEX0);
		}
		if(firmware_check(SPACE_APROM_INDEX1) && g_u8SdInitFlag)
		{ firmware_update(SPACE_APROM_ADD1,SPACE_APROM_INDEX1);
		}
		if(firmware_check(SPACE_APROM_INDEX2) && g_u8SdInitFlag)
		{ firmware_update(SPACE_APROM_ADD2,SPACE_APROM_INDEX2);
		}		
		//data flash
		if(firmware_check(SPACE_DATA) && g_u8SdInitFlag)	
		{ firmware_update(read_dfba(),SPACE_DATA);
		}
		
			
		printf("\n\n+------------------------+\n");
		printf("|     Jump to APROM      |\n");
		printf("+------------------------+\n");	
		JumptoAPROM(boot_vector);//aprom size 0~256k
    SYS_LockReg();

}
