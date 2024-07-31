/*************************************************************************************************************
* logo_standalone_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_osal_common.h"
#include "r_osal_api.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "logo.h"
#include "r_cio_api.h"
#include "r_print_api.h"
#include "display.h"
#include "logo_image.h"

#define LOGO_THREAD_ID      0x8000U               /* RESOURCE_ID_BSP     */
#define LOGO_THREAD_ID_00   (LOGO_THREAD_ID + 0U) /* OSAL_THREAD_LOGO_00 */
#define LOGO_THREAD_ID_01   (LOGO_THREAD_ID + 1U) /* OSAL_THREAD_LOGO_01 */
#define LOGO_THREAD_ID_02   (LOGO_THREAD_ID + 2U) /* OSAL_THREAD_LOGO_02 */
#define LOGO_THREAD_ID_03   (LOGO_THREAD_ID + 3U) /* OSAL_THREAD_LOGO_03 */

#define LOGO_CFG_IMG_WIDTH  (1920)
#define LOGO_CFG_IMG_HEIGHT (360)

#ifdef USE_DPAD
#define RGB_IMAGE(member)     (rgb_image_dpad.member)
#elif USE_LVDS
#define RGB_IMAGE(member)     (rgb_image_lvds.member)
#else  /* USE_HDMI0 / USE_HDMI1 */
#define RGB_IMAGE(member)     (rgb_image_hdmi.member)
#endif /* USE_DPAD */

logo_Config_t loc_LogoConfig;

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

static int64_t loc_LogoStartThread(void * Arg)
{
    loc_LogoConfig.DisplayPort = R_WM_DISP_PORT_HDMI0;
    loc_LogoConfig.LayerZIndex  = 1;
    loc_LogoConfig.LayerPosX    = 0;
    loc_LogoConfig.LayerPosY    = 1080 - LOGO_CFG_IMG_HEIGHT;
    loc_LogoConfig.ImageWidth   = LOGO_CFG_IMG_WIDTH;
    loc_LogoConfig.ImageHeight  = LOGO_CFG_IMG_HEIGHT;
    loc_LogoConfig.FrameBuffer = (uintptr_t)RGB_IMAGE(pbuffer);

    logo_main(&loc_LogoConfig);
    while (1) {
        // Disp_PrintDiscomError();
        R_OSAL_ThreadSleepForTimePeriod(10);
    }
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

int loc_MainThread(void* arg)
{
    e_osal_return_t osal_ret;

    osal_thread_handle_t    cio_thread_handle    = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    logo_thread_handle  = OSAL_THREAD_HANDLE_INVALID;
    osal_thread_handle_t    taurus_thread_handle = OSAL_THREAD_HANDLE_INVALID;
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

    thread_config.priority = 110;
    thread_config.task_name  = "CioServer";
    thread_config.func    = (p_osal_thread_func_t)loc_CioServerThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, LOGO_THREAD_ID_00, &cio_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    thread_config.task_name  = "TaurusStart";
    thread_config.func    = (p_osal_thread_func_t)loc_TaurusThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, LOGO_THREAD_ID_01, &taurus_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! TaurusThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    thread_config.priority = 0;
    thread_config.task_name  = "LogoStart";
    thread_config.func    = (p_osal_thread_func_t)loc_LogoStartThread;
    thread_config.userarg = NULL;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, LOGO_THREAD_ID_02, &logo_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[main]: ERROR!!! LogoStartThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }

    osal_ret = R_OSAL_ThreadJoin(cio_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(taurus_thread_handle, &thread_ret);
    osal_ret = R_OSAL_ThreadJoin(logo_thread_handle, &thread_ret);

    /* Close MMNGR */
    R_PRINT_Log("close mem handle\n\r");

    osal_ret = R_OSAL_MmngrClose( g_hndl_mmngr );
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("R_OSAL_MmngrClose() Err= %d \n", osal_ret);
    }

    return 0;
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(loc_MainThread);
    return(-1);
}
