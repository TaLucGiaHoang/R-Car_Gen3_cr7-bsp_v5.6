/*************************************************************************************************************
* rearview_camera_standalone_main
* Copyright (c) [2020-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : main.c
 * Version      : 1.0
 * Description  : main function for standalone rearview camera app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
// #include "FreeRTOS.h"
// #include "task.h"
// #include "interrupts.h"
#include "rcar-xos/osal/r_osal.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "rearview_camera.h"
#include "r_cio_api.h"
#include "r_print_api.h"

extern const rvc_Config_t rvc_DefaultConfig;

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define REARVIEW_THREAD_ID              0x8000U                   /* RESOURCE_ID_BSP                 */
#define REARVIEW_THREAD_CIO_SERVER_ID   (REARVIEW_THREAD_ID + 5U) /* OSAL_THREAD_REARVIEW_CIO_SERVER */
#define REARVIEW_THREAD_TAURUS_ID       (REARVIEW_THREAD_ID + 6U) /* OSAL_THREAD_REARVIEW_TAURUS     */
#define REARVIEW_THREAD_RVC_MAIN_ID     (REARVIEW_THREAD_ID + 7U) /* OSAL_THREAD_RVC_MAIN            */

osal_memory_manager_handle_t g_hndl_mmngr = NULL;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static int64_t loc_CioServerThread(void * Arg)
{
    cio_main();
    while (1) {
        R_OSAL_ThreadSleepForTimePeriod(10);
    }
    return 0;
}

static int64_t loc_TaurusThread(void * Arg)
{
    taurus_main();
    while (1) {
        R_OSAL_ThreadSleepForTimePeriod(10);
    }
    return 0;
}

static int64_t loc_RvcStartThread(void * Arg)
{
    RVC_Main(&rvc_DefaultConfig);
    while (1) {
        R_OSAL_ThreadSleepForTimePeriod(10);
    }
    return 0;
}

static int64_t loc_CioInit(void *Arg) {
    /* This relies on R_CIO_Init being able to run before
       the scheduler has been started.
       If this cannot be done, you have to create a thread
       for this function.
    */
    R_CIO_Init();
    return 0;
}

void loc_MainThread(void* arg)
{
    e_osal_return_t osal_ret;
    st_osal_thread_config_t thrd1_cfg;
    st_osal_thread_config_t thrd2_cfg;
    st_osal_thread_config_t thrd3_cfg;
    osal_thread_handle_t cio_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t taurus_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t rvcmain_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    st_osal_mmngr_config_t mmngr_config;
    int64_t thread_ret = 0;

    osal_ret  = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_MmngrGetOsalMaxConfig(&mmngr_config);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[main]: ERROR!!! R_OSAL_MmngrGetOsalMaxConfig return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_MmngrOpen(&mmngr_config, &g_hndl_mmngr);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[main]: ERROR!!! R_OSAL_MmngrOpen return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_MmngrInitializeMmngrConfigSt(&mmngr_config);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! R_OSAL_MmngrInitializeMmngrConfigSt return value = %d\n", osal_ret);
    }

    loc_CioInit(0);

    thrd1_cfg.func       = (p_osal_thread_func_t)loc_CioServerThread;
    thrd1_cfg.userarg    = NULL;
    thrd2_cfg.func       = (p_osal_thread_func_t)loc_TaurusThread;
    thrd2_cfg.userarg    = NULL;
    thrd3_cfg.func       = (p_osal_thread_func_t)loc_RvcStartThread;
    thrd3_cfg.userarg    = NULL;

    osal_ret = R_OSAL_ThreadCreate(&thrd1_cfg, REARVIEW_THREAD_CIO_SERVER_ID, &cio_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_ThreadCreate(&thrd2_cfg, REARVIEW_THREAD_TAURUS_ID, &taurus_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! TaurusThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_ThreadCreate(&thrd3_cfg, REARVIEW_THREAD_RVC_MAIN_ID, &rvcmain_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! RvcStartThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(taurus_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(rvcmain_thread_handle, &thread_ret);
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(loc_MainThread);
    return(-1);
}
