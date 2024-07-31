/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#include "rcar-xos/osal/r_osal.h"
#include "hello_rgl.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "r_cio_api.h"

#include "r_print_api.h"
#include "r_cpg_api.h"

/*
   Update following definetion to enable R_PRINT_Log() in /vlib/drivers/print/src/r_print_api.c
   #define R_USE_LOG 0
*/

#include "r_sysc_api.h"
#include "r_sysc_regs.h"

osal_memory_manager_handle_t    g_hndl_mmngr = OSAL_MEMORY_MANAGER_HANDLE_INVALID;

static int64_t loc_HelloRglThread(void * Arg)
{
    hello_main();
    while (1) {
        R_OSAL_ThreadSleepForTimePeriod(10);
    }
    return 0;
}

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

static void* loc_CioInit(void *Arg)
{
    /* This relies on R_CIO_Init being able to run before
       the scheduler has been started by R_OSAL_StartOS.
       If this cannot be done, you have to create a thread
       for this function.
    */
    R_CIO_Init();
    return 0;
}

int start_thread(void* Arg)
{
    e_osal_return_t osal_ret;
    st_osal_thread_config_t thread_config;
    osal_thread_handle_t    cio_thread    = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    hello_handle  = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    taurus_thread = OSAL_THREAD_HANDLE_INVALID;
    st_osal_mmngr_config_t  memConfig;
    int64_t thread_ret = 0;

    osal_ret = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_MmngrGetOsalMaxConfig( &memConfig );
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("main: R_OSAL_MmngrGetOsalMaxConfig failer, ret = %d \n", osal_ret);
        return -1;
    }

    osal_ret = R_OSAL_MmngrOpen( &memConfig, &g_hndl_mmngr );
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("main: R_OSAL_MmngrOpen failer, ret = %d \n", osal_ret);
        return -1;
    }

    loc_CioInit(0);

    /* Cio Start */
    osal_ret = R_OSAL_ThreadInitializeThreadConfigSt(&thread_config);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_ThreadInitializeThreadConfigSt() for CIO Thread: Err=  %d\n", osal_ret);
        return (-1);
    }

    thread_config.priority   = LOC_CIO_SERVER_PRIO;
    thread_config.task_name  = "CioServer";
    thread_config.func       = (p_osal_thread_func_t)loc_CioServerThread;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = (size_t)LOC_THREAD_CIO_SERVER_STACK_SIZE;

    osal_ret = R_OSAL_ThreadCreate(&thread_config,
                                   (osal_thread_id_t)LOC_THREAD_ID_CIO_SERVER,
                                   &cio_thread);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("Create CIO Thread[%s], ret = %d \n", thread_config.task_name, osal_ret);
        return (-1);
    }

    /* hello rgl Start */
    osal_ret = R_OSAL_ThreadInitializeThreadConfigSt(&thread_config);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_ThreadInitializeThreadConfigSt() for App Thread: ret = %d \n", osal_ret);
        return (-1);
    }
    thread_config.priority   = LOC_HELLO_RGL_PRIO;
    thread_config.task_name  = "HelloRglStart";
    thread_config.func       = (p_osal_thread_func_t)loc_HelloRglThread;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = LOC_THREAD_HELLO_RGL_STACK_SIZE;

    osal_ret = R_OSAL_ThreadCreate(&thread_config,
                                   (osal_thread_id_t)LOC_THREAD_ID_HELLO_RGL,
                                   &hello_handle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_ThreadCreate: [%s], Err= %d \n", thread_config.task_name, osal_ret);
        return (-1);
    }

    /* Taurus Start */
    osal_ret = R_OSAL_ThreadInitializeThreadConfigSt(&thread_config);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_ThreadInitializeThreadConfigSt() for TAURUS Thread: Err= %d \n", osal_ret);
        return (-1);
    }
    thread_config.priority   = LOC_TAURUS_PRIO;
    thread_config.task_name  = "TaurusStart";
    thread_config.func       = (p_osal_thread_func_t)loc_TaurusThread;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = LOC_THREAD_TAURUS_STACK_SIZE;
    osal_ret = R_OSAL_ThreadCreate(&thread_config,
                                   (osal_thread_id_t)LOC_THREAD_ID_TAURUS,
                                   &taurus_thread);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_ThreadCreate: TAURUS Thread[%s], Err= %d \n", thread_config.task_name, osal_ret);
        return (-1);
    }

    osal_ret = R_OSAL_ThreadJoin(cio_thread, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(taurus_thread, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(hello_handle, &thread_ret);

    /* Close MMNGR */
    R_PRINT_Log("close mem handle\n\r");

    osal_ret = R_OSAL_MmngrClose( g_hndl_mmngr );
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("R_OSAL_MmngrClose() Err= %d \n", osal_ret);
    }

    R_PRINT_Log("deinitialize osal\n");

    /* Deinit OSAL */
    osal_ret = R_OSAL_Deinitialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("R_OSAL_Deinitialize() Err= %d \n", osal_ret);
        return (-1);
    }
    R_PRINT_Log("Finished\n");

    return 0;
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(start_thread);
    return(-1);
}
