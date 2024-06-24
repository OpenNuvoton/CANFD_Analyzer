/**************************************************************************//**
 * @file     sdglue.c
 * @version  V3.00
 * @brief    SD glue functions for FATFS.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NuMicro.h"
#include "diskio.h"     /* FatFs lower layer API */
#include "ff.h"     /* FatFs lower layer API */

FATFS  _FatfsVolSd0;
FATFS  _FatfsVolSd1;

static TCHAR  _Path[3];

char sd_path[] = { "0:"};    /* SD drive started from 0 */


/*---------------------------------------------------------------------------*/
/* Functions                                                                 */
/*---------------------------------------------------------------------------*/
int32_t SDH_Open_Disk(SDH_T *sdh, uint32_t u32CardDetSrc)
{
    SDH_Open(sdh, u32CardDetSrc);

    if (SDH_Probe(sdh))
    {
        printf("SD initial fail!!\n");
        return 0;
    }

    _Path[1] = ':';
    _Path[2] = 0;

    if (sdh == SDH0)
    {
       _Path[0] = '0';
       f_mount(&_FatfsVolSd0, _Path, 1);
			 printf("SDisk size = %d Mbyte\n",SD0.diskSize/1024);
			 f_chdrive(sd_path);          /* Set default path */
		
			FIL fil;       /* File object */
			FRESULT res;   /* API result code */
			UINT bw;       /* Bytes written */
			BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
		


				if(_FatfsVolSd0.fs_type == 0 || _FatfsVolSd0.fs_type > 4 ) //格式不读重新格式化
				{ /* Format the default drive with default parameters */
				
					printf("make new filesystem\n\n");
					
					res = f_mkfs("", FM_ANY, 0, work, sizeof (work));

				/* Create a file as new */
				res = f_open(&fil, "readme.txt", FA_CREATE_NEW | FA_WRITE);
					printf(" 0x%x !\n\n",res);

				/* Write a message */
				f_write(&fil, "Hello, World!\r\n", 15, &bw);
					printf(" 0x%x !\n\n",res);

				/* Close the file */
				f_close(&fil);

				/* Unregister work area */
				f_mount(0, "", 0);
				}	
    }
//    else
//    {
//        _Path[0] = '1';
//        f_mount(&_FatfsVolSd1, _Path, 1);
//    }
		
		return 1;
}

void SDH_Close_Disk(SDH_T *sdh)
{
    if (sdh == SDH0)
    {
        memset(&SD0, 0, sizeof(SDH_INFO_T));
        f_mount(NULL, _Path, 1);
        memset(&_FatfsVolSd0, 0, sizeof(FATFS));
    }
    else
    {
        memset(&SD1, 0, sizeof(SDH_INFO_T));
        f_mount(NULL, _Path, 1);
        memset(&_FatfsVolSd1, 0, sizeof(FATFS));
    }
}


