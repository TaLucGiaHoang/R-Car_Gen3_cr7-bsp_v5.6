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
 * Description  : main function for standalone dcu sample app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_osal_common.h"
#include "r_osal_api.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "dcu_sample.h"
#include "r_cio_api.h"
#include "r_print_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define DCU_THREAD_ID      0x8000U               /* RESOURCE_ID_BSP     */
#define DCU_THREAD_ID_00   (DCU_THREAD_ID + 0U) /* OSAL_THREAD_LOGO_00 */
#define DCU_THREAD_ID_01   (DCU_THREAD_ID + 1U) /* OSAL_THREAD_LOGO_01 */
#define DCU_THREAD_ID_02   (DCU_THREAD_ID + 2U) /* OSAL_THREAD_LOGO_02 */
#define DCU_THREAD_ID_03   (DCU_THREAD_ID + 3U) /* OSAL_THREAD_LOGO_03 */

/**********************************************************************************************************************
 Exported/Imported global variables
 *********************************************************************************************************************/
extern const dcu_Config_t dcu_DefaultConfig;

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

static void * loc_DcuStartThread(void * Arg)
{
    Dcu_Main(&dcu_DefaultConfig);
    return 0;
}

int loc_MainThread(void* arg)
{
    e_osal_return_t osal_ret;
    int64_t thread_ret = 0;

    osal_thread_handle_t    cio_thread_handle    = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    logo_thread_handle  = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    taurus_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    st_osal_thread_config_t thread_config;

    osal_ret  = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    R_CIO_Init();

    thread_config.priority = 110;
    thread_config.task_name  = "CioServer";
    thread_config.func    = (p_osal_thread_func_t)loc_CioServerThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, DCU_THREAD_ID_00, &cio_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    thread_config.task_name  = "TaurusStart";
    thread_config.func    = (p_osal_thread_func_t)loc_TaurusThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, DCU_THREAD_ID_01, &taurus_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! TaurusThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }
    
    thread_config.priority = 0;
    thread_config.task_name  = "AppDcuStart";
    thread_config.func    = (p_osal_thread_func_t)loc_DcuStartThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, DCU_THREAD_ID_02, &logo_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! LogoStartThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(taurus_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(logo_thread_handle, &thread_ret);

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
