
/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHLWIPER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : main.c
 * Version      : 1.00
 * Description  :
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 27.10.2020 1.00     First Release
 *********************************************************************************************************************/
#if !defined(ETHER_ENABLE) || !defined(LWIP_SUPPORTED)
#error "lwip_app requires the ETHER_ENABLE and LWIP_SUPPORTED options. Please execute 'cmake -DETHER_ENABLE=ON -DLWIP_SUPPORTED=ON <build-dir>' to enable it."
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_osal_api.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "r_cio_api.h"
#include "r_lwip_lib.h"
#include "r_print_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define LOC_CIO_SERVER_PRIO 102
#define LOC_TAURUS_PRIO     101
#define LOC_LWIP_COMM_PRIO  100

#define ETHER_RESOURCE_ID 0x8000U

#define ETHER_THREAD_ID_00   (ETHER_RESOURCE_ID + 48U) /* OSAL_THREAD_ETHER_CIO_SERVER  */
#define ETHER_THREAD_ID_01   (ETHER_RESOURCE_ID + 49U) /* OSAL_THREAD_ETHER_TAURUS */
#define ETHER_THREAD_ID_02   (ETHER_RESOURCE_ID + 46U) /* loc_EtherStartThread */
#define ETHER_THREAD_ID_03   (ETHER_RESOURCE_ID + 47U) /* loc_EtherLoopThread */

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

osal_memory_manager_handle_t g_hndl_mmngr = NULL;

static void *loc_CioServerThread(void * Arg)
{
    (void)Arg;
    cio_main();
    return 0;
}

static void *loc_TaurusThread(void * Arg)
{
    (void)Arg;
    taurus_main();
    return 0;
}

static void *loc_LwipThread(void * Arg)
{
    (void)Arg;
    vLwipStart();
    return 0;
}

static void loc_CioInit(void *Arg) {
    (void)Arg;
    /* This relies on R_CIO_Init being able to run before
       the scheduler has been started.
       If this cannot be done, you have to create a thread
       for this function.
    */
    R_CIO_Init();
}

int start_thread(void* arg)
{
    (void)arg;
    e_osal_return_t osal_ret;

    osal_thread_handle_t    cio_thread_handle;
    osal_thread_handle_t    lwip_thread_handle;
    st_osal_thread_config_t thread_config;
    st_osal_mmngr_config_t  mmngr_config;
    int64_t thread_ret = 0;

    osal_ret = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_MmngrGetOsalMaxConfig(&mmngr_config);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[main]: ERROR!!! R_OSAL_MmngrGetOsalMaxConfig return value = %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_MmngrOpen(&mmngr_config, &g_hndl_mmngr);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[main]: ERROR!!! R_OSAL_MmngrOpen return value = %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_MmngrInitializeMmngrConfigSt(&mmngr_config);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! R_OSAL_MmngrInitializeMmngrConfigSt return value = %d\n", osal_ret);
        return(-1);
    }

    loc_CioInit(0);

    thread_config.func    = (p_osal_thread_func_t)loc_CioServerThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, ETHER_THREAD_ID_00, &cio_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
        return(-1);
    }

    thread_config.func    = (p_osal_thread_func_t)loc_LwipThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, ETHER_THREAD_ID_03, &lwip_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! EtherLoopThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(lwip_thread_handle, &thread_ret);
    return(0);
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(start_thread);
    return(-1);
}
