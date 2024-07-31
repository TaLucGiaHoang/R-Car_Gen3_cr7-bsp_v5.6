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
 * File Name    : fastboot_main.c
 * Version      : 1.0
 * Description  : Main function of fastboot app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "fastboot_cfg.h"
#include "fastboot.h"
#include "display.h"
#include "r_cio_api.h"

#define LOC_RESOURCE_ID_2DG                         (0x9000U)

/**********************************************************************************************************************
 Exported/Imported global variables
 *********************************************************************************************************************/
extern const fastboot_SubApp_t fastboot_SubAppCio;
extern const fastboot_SubApp_t fastboot_SubAppTaurus;
extern const fastboot_SubApp_t fastboot_SubAppEvtChecker;

#if FASTBOOT_CFG_APP_AUDIO_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppAudio;
#endif
#if FASTBOOT_CFG_APP_CAMERA_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppCamera;
#endif
#if FASTBOOT_CFG_APP_CAN_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppCan;
#endif
#if FASTBOOT_CFG_APP_CLUSTER_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppCluster;
#endif
#if FASTBOOT_CFG_APP_GUIDELINE_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppGuideline;
#endif
#if FASTBOOT_CFG_APP_LOGO_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppLogo;
#endif
#if FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE
extern const fastboot_SubApp_t fastboot_SubAppSplashScreen;
#endif

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
osal_memory_manager_handle_t    g_hndl_mmngr = OSAL_MEMORY_MANAGER_HANDLE_INVALID;

static const fastboot_SubApp_t * loc_ServiceList[] = {
    &fastboot_SubAppCio,        /* CIO service */
    &fastboot_SubAppTaurus,     /* taurus service */
    &fastboot_SubAppEvtChecker, /* event checker service */
};

static const fastboot_SubApp_t * loc_SubAppList[] = {

#if FASTBOOT_CFG_APP_AUDIO_ENABLE
    &fastboot_SubAppAudio,
#endif
#if FASTBOOT_CFG_APP_CAMERA_ENABLE
    &fastboot_SubAppCamera,
#endif
#if FASTBOOT_CFG_APP_CAN_ENABLE
    &fastboot_SubAppCan,
#endif
#if FASTBOOT_CFG_APP_CLUSTER_ENABLE
    &fastboot_SubAppCluster,
#endif
#if FASTBOOT_CFG_APP_GUIDELINE_ENABLE
    &fastboot_SubAppGuideline,
#endif
#if FASTBOOT_CFG_APP_LOGO_ENABLE
    &fastboot_SubAppLogo,
#endif
#if FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE
    &fastboot_SubAppSplashScreen,
#endif
};

volatile int        loc_quit = 0; /* Needed for dhd_os_hw.c */
static osal_mq_handle_t loc_EvtMqHandle;

static void * loc_EvtHandlerThread(void * Arg);
/**********************************************************************************************************************
 * Function Name: loc_EvtHandlerThread
 * Description  : Event handler thread.
 * Arguments    : Arg - Thread argument.
 * Return Value : void
 *********************************************************************************************************************/
static void * loc_EvtHandlerThread(void * Arg)
{
    int32_t                   ret;
    fastboot_Event_t          rx_evt;
    uint32_t                  i;
    const fastboot_SubApp_t * p_app;
    st_osal_mq_config_t       evt_mq_conf;
    r_wm_Display_t            display[3];
    r_wm_Error_t              wm_err = R_WM_ERR_SUCCESS;
    char                      task_name[16];
    e_osal_return_t           osal_ret = OSAL_RETURN_OK;
    st_osal_thread_config_t thread_config;
    osal_thread_handle_t    thread_handle[sizeof(loc_SubAppList) / sizeof(fastboot_SubApp_t *)];

    /* Create event message queue */
    evt_mq_conf.max_num_msg = FASTBOOT_CFG_EVT_MQ_LENGTH;
    evt_mq_conf.msg_size = FASTBOOT_CFG_EVT_MQ_MSG_SIZE;
    
    osal_ret            = R_OSAL_MqCreate(&evt_mq_conf, (osal_mutex_id_t)(LOC_RESOURCE_ID_2DG + 71), &loc_EvtMqHandle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[loc_EvtHandlerThread]: R_OSAL_MqCreate failed(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                    osal_ret, evt_mq_conf.max_num_msg, evt_mq_conf.msg_size, LOC_RESOURCE_ID_2DG + 66);
        return(-1);
    }

    /* Intialize all displays here */
    wm_err |= R_WM_DisplayConfig(R_WM_DISP_PORT_HDMI0, &display[0]);
    wm_err |= R_WM_DisplayInit(&display[0]);
    
    wm_err |= R_WM_DisplayConfig(R_WM_DISP_PORT_LVDS0, &display[1]);
    display[1].BackgroundColor = 0xFFFFFFFF; /* white for splash screen */
    wm_err |= R_WM_DisplayInit(&display[1]);

    wm_err |= R_WM_DisplayConfig(R_WM_DISP_PORT_HDMI1, &display[2]);
    wm_err |= R_WM_DisplayInit(&display[2]);
    /* Enable all displays together after initialzation */
    wm_err |= R_WM_DisplayEnable(&display[0], true);
    wm_err |= R_WM_DisplayEnable(&display[1], true);
    wm_err |= R_WM_DisplayEnable(&display[2], true);

    if (R_WM_ERR_SUCCESS != wm_err) {
        R_PRINT_Log("[Fastboot] Failed in initializing display!\r\n");
    }
    else {
        /* Create and kick all sub apps */
        for (i = 0; i < sizeof(loc_SubAppList) / sizeof(fastboot_SubApp_t *); i++) {
            p_app = loc_SubAppList[i];
            snprintf(task_name, sizeof(task_name), "AppFastboot%2.2lX", i);
            thread_config.priority   = FASTBOOT_CFG_EVT_TSK_PRIO;
            thread_config.task_name  = task_name;
            thread_config.func       = (p_osal_thread_func_t)p_app->ThreadFunc;
            thread_config.userarg    = (void*)0;
            thread_config.stack_size = (size_t)0x1000;
            osal_ret = R_OSAL_ThreadCreate(&thread_config,
                                        (osal_thread_id_t)0x8000 + i + 92,
                                        &thread_handle[i]);
        }

        while (1) {
            ret = R_OSAL_MqReceiveForTimePeriod(loc_EvtMqHandle, (osal_milli_sec_t)100, (char *)&rx_evt, sizeof(fastboot_Event_t));
            if (ret >= 0) {
                for (i = 0; i < sizeof(loc_SubAppList) / sizeof(fastboot_SubApp_t *); i++) {
                    p_app = loc_SubAppList[i];
                    if (p_app->EventHandlerFunc) {
                        p_app->EventHandlerFunc(rx_evt);
                    }
                }
            }
        }
    }

    return 0;
}
/**********************************************************************************************************************
 * End of function loc_EvtHandlerThread
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: Fastboot_TriggerEvent
 * Description  : Trigger event to all fastboot sub app.
 * Arguments    : Evt - Instance of event.
 * Return Value : void
 *********************************************************************************************************************/
void Fastboot_TriggerEvent(fastboot_Event_t Evt)
{
    e_osal_return_t osal_ret;
    if(NULL != loc_EvtMqHandle) {
        osal_ret = R_OSAL_MqSendForTimePeriod(loc_EvtMqHandle, (osal_milli_sec_t)1000, (void *)&Evt, sizeof(fastboot_Event_t));
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("ERROR: R_OSAL_MqSendForTimePeriod[%s], ret = %d \n", osal_ret);
            return (-1);
        }
    }
}
/**********************************************************************************************************************
 * End of function Fastboot_TriggerEvent
 *********************************************************************************************************************/


int start_thread(void* Arg)
{
    e_osal_return_t osal_ret;
    st_osal_thread_config_t thread_config;
    // osal_thread_handle_t    cio_thread    = OSAL_THREAD_HANDLE_INVALID;
    // osal_thread_handle_t    hello_handle  = OSAL_THREAD_HANDLE_INVALID;
    // osal_thread_handle_t    taurus_thread = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    evt_thread    = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    thread_handle[sizeof(loc_ServiceList) / sizeof(fastboot_SubApp_t *)];
    st_osal_mmngr_config_t  memConfig;
    int64_t thread_ret = 0;
    const fastboot_SubApp_t * p_app;

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

    // loc_CioInit(0);
    R_CIO_Init();

    /* Start fastboot event handler */
    osal_ret = R_OSAL_ThreadInitializeThreadConfigSt(&thread_config);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_ThreadInitializeThreadConfigSt() for CIO Thread: Err=  %d\n", osal_ret);
        return (-1);
    }

    thread_config.priority   = FASTBOOT_CFG_EVT_TSK_PRIO;
    thread_config.task_name  = "AppFastbootEvtHdlr";
    thread_config.func       = (p_osal_thread_func_t)loc_EvtHandlerThread;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = (size_t)0x1000;

    osal_ret = R_OSAL_ThreadCreate(&thread_config,
                                    /* TODO IDs */
                                   (osal_thread_id_t)0x8000 + 91,
                                   &evt_thread);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("Create Thread[%s], ret = %d \n", thread_config.task_name, osal_ret);
        return (-1);
    }

    /* Create all services */
    for (int i = 0; i < (int)(sizeof(loc_ServiceList) / sizeof(fastboot_SubApp_t *)); i++)
    {
        p_app = loc_ServiceList[i];
        thread_config.priority   = p_app->ThreadPriority;
        thread_config.task_name  = "AppFastbootSrvr";
        thread_config.func       = (p_osal_thread_func_t)p_app->ThreadFunc;
        thread_config.userarg    = (void*)0;
        thread_config.stack_size = (size_t)0x1000;

        // OSAL IDs
        // 0x9002 = CIO
        // 0x9003 = Taurus
        // 0x9004 = EvtChecker
        osal_ret = R_OSAL_ThreadCreate(&thread_config, 
                            /* TODO fix IDs */
                            (osal_thread_id_t)0x8000 + i + 5, 
                            &thread_handle[i]);
        if (OSAL_RETURN_OK != osal_ret) {
           R_PRINT_Log("Create Thread[%s], ret = %d \n", thread_config.task_name, osal_ret);
            return (-1);
        }
    }

    // while (1) R_OSAL_ThreadSleepForTimePeriod(10);

    for (int i = 0; i < (int)(sizeof(loc_ServiceList) / sizeof(fastboot_SubApp_t *)); i++)
    {
        osal_ret |= R_OSAL_ThreadJoin(thread_handle[i], &thread_ret);
    }

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

/**********************************************************************************************************************
 * Function Name: main
 * Description  : main function of fastboot.
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
void main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(start_thread);
    return(-1);
}
/**********************************************************************************************************************
 * End of function main
 *********************************************************************************************************************/
