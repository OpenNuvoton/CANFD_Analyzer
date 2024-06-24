/**************************************************************************//**
 * @file     usbd_update.c
 * @version  V3.00
 * @brief    Search and read new firmware from USB drive and update APROM flash with it.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "usbd_update.h"

//total 512K
char file_name[] = "/aprom.bin";	//address 0x00000			128K
char file_name1[] = "/aprom1.bin";//address 0x20000			128K
char file_name2[] = "/aprom2.bin";//address 0x40000			128K
char file_name3[] = "/aprom3.bin";//address 0x60000			128K

char data_name[] = "/data.bin";
char file_path[] = "0:/upgrade";
char file_path_buf[64];

uint8_t firmware_status[5];//aprom & data

uint8_t   _Buff[FMC_FLASH_PAGE_SIZE];

static FILINFO   Finfo;
static FIL       file1;

static    uint32_t   dfba;
uint32_t read_dfba(void)
{
return dfba;
}


int  program_flash_page(uint32_t page_addr, uint32_t *buff, int count)
{
    uint32_t  addr;                         /* flash address                              */
    uint32_t  *p = buff;                    /* data buffer pointer                        */

    printf("Program page 0x%x, count=%d\n", page_addr, count);

    FMC_Erase(page_addr);                   /* Erase an APROM page                        */

    for(addr = page_addr; addr < (page_addr + count); addr += 4, p++)     /* loop page    */
    {
        FMC_Write(addr, *p);                /* program flash                              */
    }

    /* Verify ... */
    p = buff;
    for(addr = page_addr; addr < (page_addr + count); addr += 4, p++)     /* loop page      */
    {
        if(FMC_Read(addr) != *p)            /* Read flash word and verify                 */
        {
            printf("Verify failed at 0x%x, read:0x%x, expect:0x%x\n", addr, FMC_Read(addr), *p);
					return -1;                      /* verify data mismatched                     */
        }
    }

    return 0;
}



//firmware_status = 0 no file
//firmware_status = 1 file exist
//firmware_status = 2 file upgrade success

uint32_t firmware_check(uint32_t space)
{
    FRESULT res;                            /* FATFS operation return code                */

	 switch(space)
	 {
		 case SPACE_APROM_INDEX0:		sprintf(file_path_buf,"%s%s",file_path,file_name);printf("\n+---------APROM0---------+\n");	break;
		 case SPACE_APROM_INDEX1:		sprintf(file_path_buf,"%s%s",file_path,file_name1);printf("\n+---------APROM1---------+\n");	break;		
		 case SPACE_APROM_INDEX2:		sprintf(file_path_buf,"%s%s",file_path,file_name2);printf("\n+---------APROM2---------+\n");	break;		
		 case SPACE_APROM_INDEX3:		sprintf(file_path_buf,"%s%s",file_path,file_name3);printf("\n+---------APROM3---------+\n");	break;				 
	   case SPACE_DATA:					sprintf(file_path_buf,"%s%s",file_path,data_name);printf("\n+---------DATA-----------+\n");	break;
	   default :break;	 
	 }
    /* Try APROM firmware image file      */	  
	  res = f_stat(file_path_buf, &Finfo);				  

    switch (res) 
		{
    case FR_OK: 
			printf("image exist.\nInfo:size = %d \nDT:%u/%02u/%02u %02u:%02u\n",
				(uint32_t)Finfo.fsize,
				(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31, 
				(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63		);  
				 firmware_status[space] = 1;  
			break;
		case FR_NO_PATH:										/* (5) Could not find the path */
					sprintf(file_path_buf,"%s",file_path);
					f_mkdir(file_path_buf);//creat new file folder
					printf("\nmake new folder %s\n",file_path_buf);	break;	
		case FR_NO_FILE:										/* (4) Could not find the file */		
        printf("image not found.\n");   /* File not found    */
				firmware_status[space] = 0;
    default: printf("file has an error occured. (%d)\n", res);break;
    }
		
    /*------------------------------------------------------------------------------------*/
    /*  Try to open Data Flash data image. If found, read and update Data Flash.          */
    /*------------------------------------------------------------------------------------*/
		FMC_ENABLE_ISP();
    if(FMC_Read(FMC_USER_CONFIG_0) & 0x1)    /* Data Flash is enabled?                     */
    {
        printf("Data Flash is not enabled.\n");  /* Data Flash is not enabled             */
      	dfba = 0;
				firmware_status[4] = 0;  
    }
		else
    {dfba = FMC_ReadDataFlashBaseAddr();     /* get Data Flash base address                */
		}			
		
		FMC_DISABLE_ISP();
		
		return firmware_status[space];

}

/**
  * @brief Run CRC32 checksum calculation and get result.
  * @param[in] u32addr   Starting flash address. It must be a page aligned address.
  * @param[in] u32count  Byte count of flash to be calculated. It must be multiple of 4k bytes.
  * @return Success or not.
  * @retval   0           Success.
  * @retval   0xFFFFFFFF  Invalid parameter or command failed.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Run/Read check sum time-out failed
  *           -2  u32addr or u32count must be aligned with 4k
  */
static uint32_t  FMC_GetCrc(uint32_t u32addr, uint32_t u32count)
{
    static uint32_t   ret;
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    if ((u32addr % 0x1000) || (u32count % 0x1000))
    {
        g_FMC_i32ErrCode = -2;
        ret = 0xFFFFFFFF;
    }
    else
    {
        FMC->ISPCMD  = FMC_ISPCMD_RUN_CKS;
        FMC->ISPADDR = u32addr;
        FMC->ISPDAT  = u32count;
        FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;

        i32TimeOutCnt = FMC_TIMEOUT_CHKSUM;
        while (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)
        {
            if( i32TimeOutCnt-- <= 0)
            {
                g_FMC_i32ErrCode = -1;
                return 0xFFFFFFFF;
            }
        }

        FMC->ISPCMD = FMC_ISPCMD_READ_CKS;
        FMC->ISPADDR    = u32addr;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

        i32TimeOutCnt = FMC_TIMEOUT_CHKSUM;
        while (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)
        {
            if( i32TimeOutCnt-- <= 0)
            {
                g_FMC_i32ErrCode = -1;
                return 0xFFFFFFFF;
            }
        }
        ret = FMC->ISPDAT;
    }
    return ret;
}


//CRC_CHECKSUM_COM   /*!<CRC Checksum Complement \hideinitializer */
//CRC_CHECKSUM_RVS    /*!<CRC Checksum Reverse \hideinitializer */
//CRC_WDATA_COM        /*!<CRC Write Data Complement \hideinitializer */
//CRC_WDATA_RVS


void firmware_update(uint32_t base_addr,uint32_t space)
{
  FRESULT    res;
	uint32_t FileAlign4kAddr,addr;
	int32_t result = 0;
	UINT   i,br;
	uint32_t *int32p,u32CalChecksum,u32ChkSum;

		int32p = (uint32_t *)_Buff;

		printf("+-----%s update-------+\n",(space == 0) ? "APROM" : "DATA ");
	 switch(space)
	 {
		 case SPACE_APROM_INDEX0:sprintf(file_path_buf,"%s%s",file_path,file_name);break;	
		 case SPACE_APROM_INDEX1:sprintf(file_path_buf,"%s%s",file_path,file_name1);break;		 
		 case SPACE_APROM_INDEX2:sprintf(file_path_buf,"%s%s",file_path,file_name2);break;		 
		 case SPACE_APROM_INDEX3:sprintf(file_path_buf,"%s%s",file_path,file_name3);break;		 	 
		 case SPACE_DATA: 		 sprintf(file_path_buf,"%s%s",file_path,data_name);break;		 
	 }
 		res = f_stat(file_path_buf, &Finfo);//read file information
	 
	 if(res == FR_OK)
    {		
				FileAlign4kAddr = (((uint32_t)Finfo.fsize>>12)+1)<<12; //aligned with 4k
			  res = f_open(&file1, file_path_buf, FA_OPEN_EXISTING | FA_READ);
				/* Configure CRC controller for CRC32 CPU mode */
				CRC_Open(CRC_32, CRC_CHECKSUM_RVS| CRC_WDATA_RVS | CRC_CHECKSUM_COM, 0xFFFFFFFF, CRC_CPU_WDATA_32);
				FMC_ENABLE_ISP();
			
			  for(addr = 0; addr < FileAlign4kAddr; addr += FMC_FLASH_PAGE_SIZE)
        {    /* read a flash page size data from file      */
            res = f_read(&file1, _Buff, FMC_FLASH_PAGE_SIZE, &br); 	
						/* read operation success?                    */
            if((res == FR_OK) && br)       													
            { 
							for(i = 0; i < (br>>2); i++)
							{CRC_WRITE_DATA(int32p[i]);	}
							
							if(br%4)//add last WORD,aligned with WORD
							{ 
								switch(br%4)
								{case 1:int32p[i] |= 0xffffff00;break;
								 case 2:int32p[i] |= 0xffff0000;break;
								 case 3:int32p[i] |= 0xff000000;break;
								}
								CRC_WRITE_DATA(int32p[i]);
								br = (br>>2)+1;
							}else br = br>>2;
            }
            else
						{  
							result =-1;
						}
        }

				for(i = br; i < FMC_FLASH_PAGE_SIZE>>2; i++) //last page unused word fill 0xffffffff
				{
						CRC_WRITE_DATA(0xffffffff);
				}	
				
				u32CalChecksum 		= CRC_GetChecksum();
				u32ChkSum = FMC_GetCrc(base_addr, FileAlign4kAddr);//
				
				if(u32CalChecksum != u32ChkSum && result == 0 )	//aprom file upgrade is successful and no further upgrade is required
				{
					printf("Start update firmware[0x%x][%d][0x%x]...\n",base_addr,(uint32_t)Finfo.fsize,FileAlign4kAddr);					
					FMC_ENABLE_AP_UPDATE();             /* enable APROM update                        */
					f_lseek(&file1, 0);  
					for(addr = 0; addr < FileAlign4kAddr; addr += FMC_FLASH_PAGE_SIZE)
					{    
            res = f_read(&file1, _Buff, FMC_FLASH_PAGE_SIZE, &br); /* read a flash page size data from file      */
							if((res == FR_OK) && br)       /* read operation success?                    */
							{
									/* update APROM firmware page                 */
									result = program_flash_page(addr+base_addr, (uint32_t *)_Buff, br);	
							}
							else
							{  
								result =-1;
							}
					}
					FMC_DISABLE_AP_UPDATE();

					/* Get CRC checksum value */
					u32ChkSum = FMC_GetCrc(base_addr, FileAlign4kAddr);
					FMC_DISABLE_ISP();
					
					printf("CRC checksum is 0x%X ? 0x%X ... %s.\n", u32CalChecksum,u32ChkSum, (u32CalChecksum == u32ChkSum) ? "PASS" : "FAIL");				
					result  =  (u32CalChecksum == u32ChkSum) ? 0 : -1;

					
					if(result == 0 )                   /* reach end-of-file?                         */
					{ 		printf("update success.\n");     /* firmware update success             */
								firmware_status[space] = 2;
					}
					else 	printf("update failed!\n");      /* firmware update failed              */					
					printf("\n");      /* firmware update failed              */			
					f_close(&file1);                    /* close file                                 */			
				}		
				else 		printf("+-----not new image------+\n\n");
    }
}
    
void JumptoAPROM(uint32_t vector)
{
		uint32_t s_u32ExecBank;
	
    if(firmware_status[0] == 2)
		{
		firmware_status[0] = 0;
			
		s_u32ExecBank = (uint32_t)((FMC->ISPSTS & FMC_ISPSTS_FBS_Msk) >> FMC_ISPSTS_FBS_Pos);
		printf("\nBANK%d APP processing \n", s_u32ExecBank);	
		}


    FMC_ENABLE_ISP();
		FMC_ENABLE_CFG_UPDATE();			
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   /* disable SYSTICK (prevent interrupt)   */

    /* Switch HCLK clock source to HIRC. Restore HCLK to default setting. */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    FMC_SetVectorPageAddr(vector);               /* Set vector remap to APROM address 0x0      */

    SYS->IPRST0 = SYS_IPRST0_CPURST_Msk;    /* Let CPU reset. Will boot from APROM.       */
    /* This reset can bring up user application   */
    /* in APROM address 0x0.                      */
    /* Please make sure user application has been */
    /* programmed to APROM 0x0 at this time.      */
		FMC_DISABLE_ISP();
    while(1);

}

