

#include "diskapp.h"
#include "CANapp.h"


#define  CH_FOLDER  		6
#define  MAX_RECORD_FILE_SIZE  1000000


char ext_name[] = ".csv";
char path_name[] = "0:/can0";
char file_path[] = "0:/can0/data";
char file_sn[CANFD_NUM] = {0,0,0,0};
char file_path_buf[64];
char dat_format[] = "序号,通道,方向,时间,帧格式,帧ID,长度,数据\n";

unsigned long volatile fattime;
/*---------------------------------------------------------------------------*/
/* Functions                                                                 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */

unsigned long get_fattime(void)
{
    unsigned long tmr;

    tmr = fattime;

    return tmr;
}


void SDH0_IRQHandler(void)
{
    uint32_t volatile u32Isr;

    // FMI data abort interrupt
    if (SDH0->GINTSTS & SDH_GINTSTS_DTAIF_Msk)
    {
        /* ResetAllEngine() */
        SDH0->GCTL |= SDH_GCTL_GCTLRST_Msk;
    }

    //----- SD interrupt status
    u32Isr = SDH0->INTSTS;

    if (u32Isr & SDH_INTSTS_BLKDIF_Msk)
    {
        // block down
        SD0.DataReadyFlag = TRUE;
        SDH0->INTSTS = SDH_INTSTS_BLKDIF_Msk;
    }

    if (u32Isr & SDH_INTSTS_CDIF_Msk)   // port 0 card detect
    {
        //----- SD interrupt status
        // it is work to delay 50 times for SD_CLK = 200KHz
        {
            int volatile i;         // delay 30 fail, 50 OK

            for (i = 0; i < 0x500; i++); // delay to make sure got updated value from REG_SDISR.

            u32Isr = SDH0->INTSTS;
        }

        if (u32Isr & SDH_INTSTS_CDSTS_Msk)
        {
            DEBUG_MSG("\n***** card remove !\n");
            SD0.IsCardInsert = FALSE;   // SDISR_CD_Card = 1 means card remove for GPIO mode
            memset(&SD0, 0, sizeof(SDH_INFO_T));
        }
        else
        {
            DEBUG_MSG("***** card insert !\n");
            SDH_Open(SDH0, CardDetect_From_GPIO);
            SDH_Probe(SDH0);
        }

        SDH0->INTSTS = SDH_INTSTS_CDIF_Msk;
    }

    // CRC error interrupt
    if (u32Isr & SDH_INTSTS_CRCIF_Msk)
    {
        if (!(u32Isr & SDH_INTSTS_CRC16_Msk))
        {
            //DEBUG_MSG("***** ISR sdioIntHandler(): CRC_16 error !\n");
            // handle CRC error
        }
        else if (!(u32Isr & SDH_INTSTS_CRC7_Msk))
        {
            if (!SD0.R3Flag)
            {
                //DEBUG_MSG("***** ISR sdioIntHandler(): CRC_7 error !\n");
                // handle CRC error
            }
        }

        SDH0->INTSTS = SDH_INTSTS_CRCIF_Msk;      // clear interrupt flag
    }

    if (u32Isr & SDH_INTSTS_DITOIF_Msk)
    {
        DEBUG_MSG("***** ISR: data in timeout !\n");
        SDH0->INTSTS |= SDH_INTSTS_DITOIF_Msk;
    }

    // Response in timeout interrupt
    if (u32Isr & SDH_INTSTS_RTOIF_Msk)
    {
        DEBUG_MSG("***** ISR: response in timeout !\n");
        SDH0->INTSTS |= SDH_INTSTS_RTOIF_Msk;
    }
}

void Disk_IO_Init(void)
{
    /* Select multi-function pins */

#if NK_M467HJ == 1	
			SET_SD0_DAT0_PE2();
			SET_SD0_DAT1_PE3();
			SET_SD0_DAT2_PE4();
			SET_SD0_DAT3_PE5();
			SET_SD0_CLK_PE6();
			SET_SD0_CMD_PE7();
			SET_SD0_nCD_PD13();
#else
			SET_SD0_DAT0_PB2();
			SET_SD0_DAT1_PB3();
			SET_SD0_DAT2_PB4();
			SET_SD0_DAT3_PB5();
			SET_SD0_CLK_PB1();
			SET_SD0_CMD_PB0();		
#endif


    /* Select IP clock source */
    CLK_SetModuleClock(SDH0_MODULE, CLK_CLKSEL0_SDH0SEL_PLL_DIV2, CLK_CLKDIV0_SDH0(2));
    /* Enable IP clock */
    CLK_EnableModuleClock(SDH0_MODULE);

    NVIC_SetPriority(SDH0_IRQn, 3);
}


void RecordFileCheck(FIL *fil,FRESULT *res,uint8_t channel)
{
	  UINT bw;       /* Bytes written */
	
		if(channel > CANFD_NUM)	return ;
			
		file_path[CH_FOLDER] = '0' + channel;	//select record folder
    sprintf(file_path_buf,"%s%d%s",file_path,file_sn[channel],ext_name);	//make file path & file name		
		*res = f_open(fil,file_path_buf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
			
	switch (*res) 
		{
    case FR_OK:

					if(f_size(fil)>MAX_RECORD_FILE_SIZE)  //文件大于1M新建文件开始记录
					{file_sn[channel]++;
					 sprintf(file_path_buf,"%s%d%s",file_path,file_sn[channel],ext_name);		
					*res = f_open(fil,file_path_buf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//creat new file
					*res = f_write(fil, dat_format, sizeof(dat_format), &bw);//write file head
							if(*res== FR_OK)
							{
							 DEBUG_MSG("new file %s ,Size: (%d).\n",file_path_buf,bw);
							}	else 
							 DEBUG_MSG("file creat fail.\n",*res);		
					}							
        break;

		case FR_NO_PATH:/* (5) Could not find the path */	
					path_name[CH_FOLDER] = file_path[CH_FOLDER];//creat new file folder
					*res = f_mkdir(path_name);
					DEBUG_MSG("\nmake new folder %s,(%d).\n",path_name,*res);			
		case FR_NO_FILE:/* (4) Could not find the file */
		      *res = f_open(fil,file_path_buf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
					*res = f_write(fil, dat_format, sizeof(dat_format), &bw);    
							if(*res== FR_OK)
							{
							 DEBUG_MSG("new file %s ,Size: (%d).\n",file_path_buf,bw);
							}	else 
							 DEBUG_MSG("file creat fail.\n",*res);							
//				  f_close(fil);
					break;

    default:
        DEBUG_MSG("An error occured. (%d)\n", *res);
    }
}

void Disk_Init(void)
{	
		
		FIL fil;       /* File object */
		FRESULT res;   /* API result code */
		UINT bw;       /* Bytes written */
    FILINFO fno;
	  uint8_t cnt;

	/* Configure FATFS */
    SDH_Open_Disk(SDH0, CARD_DETECT);
	
	
	  for(cnt=0;cnt<CANFD_NUM;cnt++)//record file init
		{
	  file_path[CH_FOLDER] = '0' + cnt;
		sprintf(file_path_buf,"%s%d%s",file_path,file_sn[cnt],ext_name);			  
	  res = f_stat(file_path_buf, &fno);
	
    switch (res) 
		{
    case FR_OK:

		    while(fno.fsize>MAX_RECORD_FILE_SIZE)//跳过已经记录的文件
				{	 
					file_sn[cnt]++;
					sprintf(file_path_buf,"%s%d%s",file_path,file_sn[cnt],ext_name);							
					res = f_stat(file_path_buf, &fno);
					
					if(res == FR_NO_FILE)
					{ res = f_open(&fil,file_path_buf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);	//creat new file
						res = f_write(&fil, dat_format, sizeof(dat_format), &bw); 				//write file head   
							if(res== FR_OK)
							{
							 DEBUG_MSG("new file %s ,Size: (%d).\n",file_path_buf,bw);
							}				
						f_close(&fil);
						break;
					}
				}
				
				
				DEBUG_MSG("%s ,Size: %lu\n", file_path_buf,fno.fsize);//print file brief information

#if 0				
        DEBUG_MSG("Timestamp: %u/%02u/%02u, %02u:%02u\n",
               (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
               fno.ftime >> 11, fno.ftime >> 5 & 63);
        DEBUG_MSG("Attributes: %c%c%c%c%c\n",
               (fno.fattrib & AM_DIR) ? 'D' : '-',
               (fno.fattrib & AM_RDO) ? 'R' : '-',
               (fno.fattrib & AM_HID) ? 'H' : '-',
               (fno.fattrib & AM_SYS) ? 'S' : '-',
               (fno.fattrib & AM_ARC) ? 'A' : '-');
#endif				
        break;


		case FR_NO_PATH:/* (5) Could not find the path */
					path_name[CH_FOLDER] = file_path[CH_FOLDER];//creat new file folder
					res = f_mkdir(path_name);
					DEBUG_MSG("\nmake new folder %s,(%d).\n",path_name,res);			
		case FR_NO_FILE:/* (4) Could not find the file */
		      res = f_open(&fil,file_path_buf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);	//creat new file
					res = f_write(&fil, dat_format, sizeof(dat_format), &bw);    			//write file head   
							if(res== FR_OK)
							{
							 DEBUG_MSG("new file %s ,Size: (%d).\n",file_path_buf,bw);
							}	else 
							 DEBUG_MSG("file creat fail.\n",res);							
				  f_close(&fil);
					break;

    default:
        DEBUG_MSG("An error occured. (%d)\n", res);
    }
	 }
}



/**
  * @brief      store can message to TF Card
  * @param[in]  record serial number
  * @param[in]  message source channel number
  * @param[in]  message txd or rxd 
	* @param[in]  message data point
  * @return     None
  * @details    
  */
char record_buffer[FF_MAX_SS];
int32_t Record_CANmsg(uint32_t sn,uint8_t channel, uint8_t rxtx,CANFD_FD_MSG_T *psMsg)
{

		uint8_t u8Cnt;
    int16_t bytenum;
	
		FIL fil;       /* File object */
		FRESULT res;   /* API result code */
		UINT bw;       /* Bytes written */

//序号，通道，方向，时间，帧格式，帧ID，长度
		bytenum = sprintf(record_buffer," %d, %d, %d, %ld, %d, %d, %d, hex=", sn,channel,rxtx,fattime,psMsg->eIdType,psMsg->u32Id,psMsg->u32DLC);
//数据
    for(u8Cnt = 0; u8Cnt < psMsg->u32DLC; u8Cnt++)
    {
        bytenum = sprintf(record_buffer+bytenum,"%02x", psMsg->au8Data[u8Cnt]) + bytenum;
    }
		bytenum = sprintf(record_buffer+bytenum,"\r\n") + bytenum;
		

//open & check record file	

		RecordFileCheck(&fil,&res,channel);
		
//data write in record file			
    if(res== FR_OK)
    {
				f_lseek(&fil,f_size(&fil));//file point move to  end of file		
        res = f_write(&fil, record_buffer, bytenum, &bw);    
				if(res== FR_OK)
				{
					DEBUG_MSG("[SN=%d] record successfully saved.\r\n\n",sn);	
				}					
    }
		f_close(&fil);

#if 0 //写入校验		
		memset(record_buffer,0,bytenum);
		
		res = f_open(&fil,file_path,FA_READ);
		if(res== FR_OK)
    {
				f_lseek(&fil,f_size(&fil) - bytenum);	
        res = f_read(&fil, record_buffer, bytenum, &bw);    
				if(res== FR_OK)
				{
					DEBUG_MSG("文件读取成功，读到的字节数据：%d\r\n",bw);
					DEBUG_MSG("读取的文件数据为：%s\r\n",record_buffer);
				}
				else
				{
					DEBUG_MSG("：%d\r\n",res); 
				}					
    }
		f_close(&fil);
#endif	
		
		return 1 ;
}		

