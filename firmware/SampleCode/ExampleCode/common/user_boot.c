
#include "user_boot.h"
#include "usbd_update.h"
#include "OLED4PINiic.h"

uint32_t app_map[4]={SPACE_APROM_ADD0,SPACE_APROM_ADD1,SPACE_APROM_ADD2,SPACE_APROM_ADD3};

uint32_t BootSwitch(void)
{    
		uint16_t fuction=0xff; 
		uint32_t vecmap,vector;	
	
//		GPIO_SetMode(PF, BIT0|BIT1, GPIO_MODE_INPUT);
//	  fuction = (PF->PIN)&0x03;
	
	 /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function */
    FMC_Open();
		vecmap = FMC_GetVECMAP();
    printf("VECMAP = 0x%x\n", vecmap);
	
		OLED_clear();
		if(vecmap == SPACE_APROM_ADD0) 
										{OLED_ShowString(0,0,"VCOM  <--> SLCAN",1)	;	fuction = 0;}		
		else 						 OLED_ShowString(0,0,"VCOM  <--> SLCAN",0)	;	
		
		if(vecmap == SPACE_APROM_ADD1) 
										{OLED_ShowString(0,2,"VCOM  <--> UART ",1)	;	fuction = 1;}					
		else 						 OLED_ShowString(0,2,"VCOM  <--> UART ",0)	;	
		
		if(vecmap == SPACE_APROM_ADD2) 
										{OLED_ShowString(0,4,"VCOM  <--> 3in1 ",1)	;	fuction = 2;}					
		else 						 OLED_ShowString(0,4,"VCOM  <--> 3in1 ",0)	;	
				
	
	while(1)
	{
		if((PF->PIN&0x01) == 0)
		{TIMER_Delay(TIMER2,10000);
			if((PF->PIN&0x01) == 0)
			{TIMER_Delay(TIMER2,10000);
			 while((PF->PIN&0x01) == 0);
				fuction++;
				if(fuction>=3) fuction=0;
			}
		}
		
		
    switch(fuction)
    {
    case 0:
			  if(vector == SPACE_APROM_ADD0)  break;
        vector = SPACE_APROM_ADD0;
		    printf("[0] Boot 0, base = 0x00000\n");	
				OLED_ShowString(0,4,"VCOM  <--> 3in1 ",0);
				OLED_ShowString(0,0,"VCOM  <--> SLCAN",1)	;
				break;
    case 1:
        if(vector == SPACE_APROM_ADD1)  break;
        vector = SPACE_APROM_ADD1;
				printf("[1] Boot 1, base = 0x20000\n");
				OLED_ShowString(0,0,"VCOM  <--> SLCAN",0)	;		
				OLED_ShowString(0,2,"VCOM  <--> UART ",1);
        break;
    case 2:
			  if(vector == SPACE_APROM_ADD2)  break;
        vector = SPACE_APROM_ADD2;
				printf("[2] Boot 2, base = 0x40000\n");
				OLED_ShowString(0,2,"VCOM  <--> UART ",0);		
				OLED_ShowString(0,4,"VCOM  <--> 3in1 ",1);
		
        break;
    default:	printf("[x] Boot x");
				
				OLED_ShowString(0,0,"VCOM  <--> SLCAN",0)	;	
				OLED_ShowString(0,2,"VCOM  <--> UART ",0)	;
				OLED_ShowString(0,4,"VCOM  <--> 3in1 ",0)	;	
				fuction = 0;
        break;
    }
		
		if((PF->PIN&0x02) == 0)
		{TIMER_Delay(TIMER2,10000);
			if((PF->PIN&0x02) == 0)
			{
				return vector;
			}
		}

	};
	
//    /* Reset CPU only to reset to new vector page */
//    SYS_ResetCPU();
}
