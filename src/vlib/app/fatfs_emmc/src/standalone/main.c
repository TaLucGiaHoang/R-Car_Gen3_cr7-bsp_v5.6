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
 * File Name    : main.c
 * Version      : 1.0
 * Description  : main function for standalone fatfs app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_osal_common.h"
#include "r_osal_api.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "r_cio_api.h"
#include "r_print_api.h"

#include "string.h"
#include "r_cpg_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define LOC_CIO_SERVER_PRIO             (102)
#define LOC_TAURUS_PRIO                 (101)
#define LOC_FATFS_PRIO                  (100)

#define STPnCK							0x00000100
#define STPnHCK							0x00000200

/* SD-IF clock frequency control register */
#define SD0CKCR							0xE6150074
#define SD1CKCR							0xE6150078
#define SD2CKCR							0xE6150268
#define SD3CKCR							0xE615026C
#define CLOCK_200MHZ					0x0BEBC200

#define FATFS_THREAD_ID      0x8000U                /* RESOURCE_ID_BSP     */
#define FATFS_THREAD_ID_00   (FATFS_THREAD_ID + 0U) /* OSAL_THREAD_FATFS_00 */
#define FATFS_THREAD_ID_01   (FATFS_THREAD_ID + 1U) /* OSAL_THREAD_FATFS_01 */
#define FATFS_THREAD_ID_02   (FATFS_THREAD_ID + 2U) /* OSAL_THREAD_FATFS_02 */
#define FATFS_MUTEX_ID_01    (FATFS_THREAD_ID + 4U) /* OSAL_MUTEX_FATFS_01  */
/**********************************************************************************************************************
 Exported/Imported global variables
 *********************************************************************************************************************/
osal_memory_manager_handle_t g_hndl_mmngr = OSAL_MEMORY_MANAGER_HANDLE_INVALID;
extern void mmc_main(void);

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static void * loc_CioServerThread(void * Arg)
{
	cio_main();
	return 0;
}

static void * loc_TaurusThread(void * Arg)
{
	taurus_main();
	return 0;
}

static void * loc_MmcSampleStartThread(void * Arg)
{
	/* Initialise the SDHI. */
	R_PMA_CPG_SetClockStopStat(R_CPG_SDIF0_CLOCK, false);
	R_PMA_CPG_SetClockStopStat(R_CPG_SDIF1_CLOCK, false);
	R_PMA_CPG_SetClockStopStat(R_CPG_SDIF2_CLOCK, false);
	R_PMA_CPG_SetClockStopStat(R_CPG_SDIF3_CLOCK, false);

	mmc_main();
	return 0;
}
int loc_MainThread(void* arg)
{
	e_osal_return_t osal_ret;
	int64_t thread_ret = 0;

	osal_thread_handle_t    cio_thread_handle    = OSAL_THREAD_HANDLE_INVALID;
	osal_thread_handle_t    taurus_thread_handle = OSAL_THREAD_HANDLE_INVALID;
	osal_thread_handle_t    mmc_sample_thread_handle  = OSAL_THREAD_HANDLE_INVALID;
	st_osal_thread_config_t thread_config;
	st_osal_mmngr_config_t  memConfig;

	osal_ret  = R_OSAL_Initialize();
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
		return(-1);
	}

	osal_ret = R_OSAL_MmngrGetOsalMaxConfig( &memConfig );
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("main: R_OSAL_MmngrGetOsalMaxConfig failer, ret = %d \n", osal_ret);
		return 0;
	}

	osal_ret = R_OSAL_MmngrOpen( &memConfig, &g_hndl_mmngr );
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("main: R_OSAL_MmngrOpen failer, ret = %d \n", osal_ret);
		return 0;
	}

	R_CIO_Init();

	thread_config.priority = LOC_CIO_SERVER_PRIO;
	thread_config.task_name  = "CioServer";
	thread_config.func    = (p_osal_thread_func_t)loc_CioServerThread;
	thread_config.userarg = NULL;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, FATFS_THREAD_ID_00, &cio_thread_handle);
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
	}

	thread_config.priority = LOC_TAURUS_PRIO;
	thread_config.task_name  = "TaurusStart";
	thread_config.func    = (p_osal_thread_func_t)loc_TaurusThread;
	thread_config.userarg = NULL;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, FATFS_THREAD_ID_01, &taurus_thread_handle);
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("[main]: ERROR!!! TaurusThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
	}

	thread_config.priority = LOC_FATFS_PRIO;
	thread_config.task_name  = "AppFatFsThread";
	thread_config.func    = (p_osal_thread_func_t)loc_MmcSampleStartThread;
	thread_config.userarg = NULL;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, FATFS_THREAD_ID_02, &mmc_sample_thread_handle);
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("[main]: ERROR!!! FatfsStartThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
	}

	osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
	osal_ret = R_OSAL_ThreadJoin(taurus_thread_handle, &thread_ret);
	osal_ret = R_OSAL_ThreadJoin(mmc_sample_thread_handle, &thread_ret);

	/* Close MMNGR */
	R_PRINT_Log("close mem handle\n\r");

	osal_ret = R_OSAL_MmngrClose( g_hndl_mmngr );
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("R_OSAL_MmngrClose() Err= %d \n", osal_ret);
	}

	R_PRINT_Log("[W] Deinit...\n");

	/* Deinit OSAL */
	osal_ret = R_OSAL_Deinitialize();
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("R_OSAL_Deinitialize() Err= %d \n", osal_ret);
		return 0;
	}
	R_PRINT_Log("Finished\n");

	return 0;
}

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
int main(void)
{
	/* Start OS and initial thread */
	/* Note: parameter unused by AutoSAR, see TASK(maintask) */
	R_OSAL_StartOS(loc_MainThread);
	return 0;
}
