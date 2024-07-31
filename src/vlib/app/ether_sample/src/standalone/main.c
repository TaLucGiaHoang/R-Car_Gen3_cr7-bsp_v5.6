/*************************************************************************************************************
* ether_sample_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "ether_sample.h"
#include "app_eth_common_sample.h"
#include "r_cio_api.h"
#include "r_print_api.h"

#if !defined(ETHER_ENABLE)
#error "ether_sample requires the ETHER_ENABLE option. Please execute 'cmake -DETHER_ENABLE=ON <build-dir>' to enable it."
#endif

#define ETHER_RESOURCE_ID 0x8000U

#define ETHER_THREAD_ID_00   (ETHER_RESOURCE_ID + 48U) /* OSAL_THREAD_ETHER_CIO_SERVER  */
#define ETHER_THREAD_ID_01   (ETHER_RESOURCE_ID + 49U) /* OSAL_THREAD_ETHER_TAURUS */
#define ETHER_THREAD_ID_02   (ETHER_RESOURCE_ID + 46U) /* loc_EtherStartThread */
#define ETHER_THREAD_ID_03   (ETHER_RESOURCE_ID + 47U) /* loc_EtherLoopThread */

osal_memory_manager_handle_t g_hndl_mmngr = NULL;

static int64_t loc_CioServerThread(void * Arg)
{
    cio_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static int64_t loc_TaurusThread(void * Arg)
{
    taurus_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static int64_t loc_EtherStartThread(void * Arg)
{
    ether_sample_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static int64_t loc_EtherLoopThread(void * Arg)
{
    loc_EtherMainFunctionLoop();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static void loc_CioInit(void *Arg) {
    /* This relies on R_CIO_Init being able to run before
       the scheduler has been started.
       If this cannot be done, you have to create a thread
       for this function.
    */
    R_CIO_Init();
}

void loc_MainThread(void* arg)
{
    e_osal_return_t osal_ret;

    osal_thread_handle_t    cio_thread_handle;
    osal_thread_handle_t    taurus_thread_handle;
    osal_thread_handle_t    ether_sample_thread_handle;
    osal_thread_handle_t    ether_loop_thread_handle;
    st_osal_thread_config_t thread_config;
    st_osal_mmngr_config_t  mmngr_config;
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

    thread_config.func    = (p_osal_thread_func_t)loc_CioServerThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, ETHER_THREAD_ID_00, &cio_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    thread_config.func    = (p_osal_thread_func_t)loc_TaurusThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, ETHER_THREAD_ID_01, &taurus_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! TaurusThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    thread_config.func    = (p_osal_thread_func_t)loc_EtherStartThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, ETHER_THREAD_ID_02, &ether_sample_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! EtherStartThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    thread_config.func    = (p_osal_thread_func_t)loc_EtherLoopThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, ETHER_THREAD_ID_03, &ether_loop_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! EtherLoopThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(taurus_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(ether_sample_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(ether_loop_thread_handle, &thread_ret);
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(loc_MainThread);
    return(-1);
}
