/*************************************************************************************************************
* audio_standalone_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "audio.h"
#include "r_cio_api.h"
#include "r_print_api.h"

#define AUDIO_THREAD_ID_00   0x8000u
#define AUDIO_THREAD_ID_01   0x8001u
#define AUDIO_THREAD_ID_02   0x8002u

extern const audio_Config_t audio_DefaultConfig;

static void *loc_CioServerThread(void * Arg) 
{
    cio_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static void *loc_TaurusThread(void * Arg) 
{
    taurus_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static void *loc_AudioStartThread(void * Arg) 
{
    /* Allow CIO to init AUDMAC */
    R_OSAL_ThreadSleepForTimePeriod(10);
    audio_main(&audio_DefaultConfig);
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static void* loc_CioInit(void *Arg) {
    /* This relies on R_CIO_Init being able to run before
       the scheduler has been started.
       If this cannot be done, you have to create a thread
       for this function.
    */
    R_CIO_Init();
    return 0;
}

int start_thread(void* arg)
{
    e_osal_return_t osal_ret;

    osal_thread_handle_t cio_thread_handle;
    osal_thread_handle_t taurus_thread_handle;
    osal_thread_handle_t audio_thread_handle;
    st_osal_thread_config_t thread_config;

    int64_t thread_ret = 0;

    osal_ret  = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    loc_CioInit(0);

    thread_config.func = (p_osal_thread_func_t)loc_CioServerThread;
    thread_config.userarg = NULL;
    R_OSAL_ThreadCreate(&thread_config, AUDIO_THREAD_ID_00, &cio_thread_handle);

    thread_config.func = (p_osal_thread_func_t)loc_TaurusThread;
    thread_config.userarg = NULL;
    R_OSAL_ThreadCreate(&thread_config, AUDIO_THREAD_ID_01, &taurus_thread_handle);

    thread_config.func = (p_osal_thread_func_t)loc_AudioStartThread;
    thread_config.userarg = NULL;
    R_OSAL_ThreadCreate(&thread_config, AUDIO_THREAD_ID_02, &audio_thread_handle);

    osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(taurus_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(audio_thread_handle, &thread_ret);

    return 0;
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(start_thread);
    return(-1);
}