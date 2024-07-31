
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
 * File Name    : mmc_main.c
 * Version      : 1.0
 * Description  : Implementation of eMMC Sample app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <string.h>

#include "r_osal_api.h"
#include "r_print_api.h"
#include "emmc_def.h"
#include "emmc_config.h"
#include "r_osal_common.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define DATA_SIZE	(10485760)	// 10 Megabytes
#define START_SECTOR 0x0
#define EMMC_SAMPLE_ERASE
#define EMMC_SAMPLE_WRITE
#define EMMC_SAMPLE_READ

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static uint8_t wbuff[DATA_SIZE] __attribute__ ((aligned (128)));
static uint8_t rbuff[DATA_SIZE] __attribute__ ((aligned (128)));

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
	int status;
	int i;
	st_osal_time_t write_dma_start_time, write_dma_end_time;
	st_osal_time_t write_cpu_start_time, write_cpu_end_time;
	st_osal_time_t read_dma_start_time, read_dma_end_time;
	st_osal_time_t read_cpu_start_time, read_cpu_end_time;
	osal_nano_sec_t write_dma_time, write_cpu_time, read_dma_time, read_cpu_time;
	double write_dma_speed, write_cpu_speed, read_dma_speed, read_cpu_speed;
	R_PRINT_Log("[eMMC] Start...\n");
	status = rcar_emmc_init();
	if (EMMC_SUCCESS != status) {
		R_PRINT_Log("[eMMC] Failed to eMMC driver initialize.\n");
		return;
	}
	rcar_emmc_memcard_power(EMMC_POWER_ON);
	status = rcar_emmc_mount();
	if (EMMC_SUCCESS != status) {
		R_PRINT_Log("[eMMC] Failed to eMMC mount operation.\n");
		return;
	}
#ifdef EMMC_SAMPLE_ERASE
	status = emmc_erase_sector(START_SECTOR, START_SECTOR + (DATA_SIZE / EMMC_SECTOR_SIZE));
	if (status != EMMC_SUCCESS) {
		R_PRINT_Log("[eMMC] Failed to eMMC erase operation.\n");
		return;
	}
#endif
#ifdef EMMC_SAMPLE_WRITE
	memset(wbuff, 0xAA, DATA_SIZE);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &write_dma_start_time);
	status = emmc_write_sector((uint32_t *) wbuff, START_SECTOR, DATA_SIZE / EMMC_SECTOR_SIZE, LOADIMAGE_FLAGS_DMA_ENABLE);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &write_dma_end_time);
	if (status != EMMC_SUCCESS) {
		R_PRINT_Log("[eMMC] write (DMA) = NG\n");
	}
	else {
		R_PRINT_Log("[eMMC] write (DMA) = OK\n");
	}
#endif
#ifdef EMMC_SAMPLE_READ
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &read_dma_start_time);
	status = emmc_read_sector((uint32_t *) rbuff, START_SECTOR, DATA_SIZE / EMMC_SECTOR_SIZE, LOADIMAGE_FLAGS_DMA_ENABLE);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &read_dma_end_time);
	if (status != EMMC_SUCCESS || memcmp(wbuff, rbuff, DATA_SIZE) != 0) {
		R_PRINT_Log("[eMMC] read (DMA)  = NG\n");
	}
	else {
		R_PRINT_Log("[eMMC] read (DMA)  = OK\n");
	}
#endif
#ifdef EMMC_SAMPLE_WRITE
	memset(wbuff, 0x55, DATA_SIZE);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &write_cpu_start_time);
	status = emmc_write_sector((uint32_t *) wbuff, START_SECTOR, DATA_SIZE / EMMC_SECTOR_SIZE, 0);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &write_cpu_end_time);
	if (status != EMMC_SUCCESS) {
		R_PRINT_Log("[eMMC] write (CPU) = NG\n");
	}
	else {
		R_PRINT_Log("[eMMC] write (CPU) = OK\n");
	}
#endif
#ifdef EMMC_SAMPLE_READ
	memset(rbuff, 0, DATA_SIZE);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &read_cpu_start_time);
	status = emmc_read_sector((uint32_t *) rbuff, START_SECTOR, DATA_SIZE / EMMC_SECTOR_SIZE, 0);
	R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &read_cpu_end_time);
	if (status != EMMC_SUCCESS || memcmp(wbuff, rbuff, DATA_SIZE) != 0) {
		R_PRINT_Log("[eMMC] read (CPU)  = NG\n");
	}
	else {
		R_PRINT_Log("[eMMC] read (CPU)  = OK\n");
	}
#endif
	R_OSAL_ClockTimeCalculateTimeDifference(&write_dma_end_time, &write_dma_start_time, &write_dma_time);
	R_OSAL_ClockTimeCalculateTimeDifference(&write_cpu_end_time, &write_cpu_start_time, &write_cpu_time);
	R_OSAL_ClockTimeCalculateTimeDifference(&read_dma_end_time, &read_dma_start_time, &read_dma_time);
	R_OSAL_ClockTimeCalculateTimeDifference(&read_cpu_end_time, &read_cpu_start_time, &read_cpu_time);
	write_dma_speed = (((double)DATA_SIZE) / (write_dma_time / 1000000000.0f)) / 1048576.0f;
	write_cpu_speed = (((double)DATA_SIZE) / (write_cpu_time / 1000000000.0f)) / 1048576.0f;
	read_dma_speed = (((double)DATA_SIZE) / (read_dma_time / 1000000000.0f)) / 1048576.0f;
	read_cpu_speed = (((double)DATA_SIZE) / (read_cpu_time / 1000000000.0f)) / 1048576.0f;
#ifdef EMMC_SAMPLE_WRITE
	R_PRINT_Log("[eMMC] write speed (DMA) = %.2f MB/s\n", write_dma_speed);
#endif
#ifdef EMMC_SAMPLE_READ
	R_PRINT_Log("[eMMC] read speed  (DMA) = %.2f MB/s\n", read_dma_speed);
#endif
#ifdef EMMC_SAMPLE_WRITE
	R_PRINT_Log("[eMMC] write speed (CPU) = %.2f MB/s\n", write_cpu_speed);
#endif
#ifdef EMMC_SAMPLE_READ
	R_PRINT_Log("[eMMC] read speed  (CPU) = %.2f MB/s\n", read_cpu_speed);
#endif
	R_PRINT_Log("[eMMC] End.\n");
	return;
}
/***********************************************************************************************************************
 End of function mmc_main
 **********************************************************************************************************************/
