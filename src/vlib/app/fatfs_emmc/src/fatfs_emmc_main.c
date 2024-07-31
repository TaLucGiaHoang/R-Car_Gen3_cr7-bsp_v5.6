
/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : fatfs_emmc_main.c
 * Version      : 1.0
 * Description  : Implementation of FAT Filesystem using eMMC app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <string.h>

#include "r_osal_api.h"
#include "r_print_api.h"
#include "ff.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define DATA_SIZE	(5242880)			// 5 Megabytes

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static uint8_t wbuff[DATA_SIZE] __attribute__ ((aligned (128)));
static uint8_t rbuff[DATA_SIZE] __attribute__ ((aligned (128)));
FATFS FatFs;   /* Work area (filesystem object) for logical drive */

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mmc_main
 * Description  : main function of emmc sample app.
 * Arguments    : None
 * Return Value : void
 *********************************************************************************************************************/
void mmc_main(void)
{
	FIL fil;        /* File object */
	char line[100]; /* Line buffer */
	FRESULT fr;     /* FatFs return code */
	UINT size, total_size = 0;

	R_PRINT_Log("[FatFs-eMMC] Start...\n", line);

	/* Give a work area to the default drive */
	fr = f_mount(&FatFs, "0:/", 0);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_mount        : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_mount        : OK!\n");

	/* Open a text file */
	fr = f_open(&fil, "0:/file.bin", FA_READ);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_open(large)  : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_open(large)  : OK!\n");

	st_osal_time_t start_time, end_time;
	osal_nano_sec_t time;

	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &start_time);
	do {
		fr = f_read(&fil, rbuff, DATA_SIZE, &size);
		total_size += size;
	} while (!fr && size!=0);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &end_time);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_read(large)  : NG!\n");
	else {
		R_PRINT_Log("[FatFs-eMMC] f_read(large)  : OK!\n");
		R_OSAL_ClockTimeCalculateTimeDifference(&end_time, &start_time, &time);
		R_PRINT_Log("[FatFs-eMMC] f_read speed   : %.2f MB/s\n", (((double)total_size) / ((double)time / 1000000000.0f)) / 1048576.0f);
	}

	/* Close the file */
	fr = f_close(&fil);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_close(large) : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_close(large) : OK!\n");

		/* Open a text file */
	fr = f_open(&fil, "0:/test.txt", FA_READ);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_open         : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_open         : OK!\n");

	fr = f_read(&fil, line, sizeof(line), &size);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_read         : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_read         : OK!\n");

	line[size] = 0;
	R_PRINT_Log("%s\n", line);

	/* Close the file */
	fr = f_close(&fil);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_close        : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_close        : OK!\n");

	
	/* Open a text file */
	fr = f_open(&fil, "0:/test0.txt", FA_CREATE_ALWAYS|FA_WRITE);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_open         : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_open         : OK!\n");

	for (int i=0; i<DATA_SIZE; i++) {
		wbuff[i] = (i % 26) + 'A';
	}
	fr = f_write(&fil, wbuff, DATA_SIZE, &size);
	if (fr || size != DATA_SIZE)
		R_PRINT_Log("[FatFs-eMMC] f_write        : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_write        : OK!\n");

	/* Close the file */
	fr = f_close(&fil);
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_close        : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_close        : OK!\n");

	/* Unmount the drive */
	fr = f_unmount("");
	if (fr)
		R_PRINT_Log("[FatFs-eMMC] f_unmount      : NG!\n");
	else
		R_PRINT_Log("[FatFs-eMMC] f_unmount      : OK!\n");

	R_PRINT_Log("[FatFs-eMMC] End.\n", line);

	return;
}
/***********************************************************************************************************************
 End of function mmc_main
 **********************************************************************************************************************/
