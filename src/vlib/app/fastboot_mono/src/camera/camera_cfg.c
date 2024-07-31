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
 * File Name    : camera_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot_mono sub app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if FASTBOOT_CFG_APP_CAMERA_ENABLE
#include "fastboot_cfg.h"

#include "fastboot.h"
#if defined(R_TARGET_BOARD_ERGUOTOU)
#include "surround_view.h"
#else
#include "rearview_camera.h"
#endif

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define FASTBOOT_CFG_CAMERA_TSK_PRIO (OSAL_THREAD_PRIORITY_TYPE6)

/**********************************************************************************************************************
 Exported/Imported global variables
 *********************************************************************************************************************/
#if defined(R_TARGET_BOARD_ERGUOTOU)
extern const svc_Config_t fastboot_SvcConfig;
#else
extern const rvc_Config_t rvc_FastbootConfig;
#endif

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static void * loc_CameraThread(void * Arg)
{
#if defined(R_TARGET_BOARD_ERGUOTOU)
    SVC_Main(&fastboot_SvcConfig);
#else
    RVC_Main(&rvc_FastbootConfig);
#endif
    return 0;
}

static void loc_CameraEvtHandler(fastboot_Event_t Evt)
{
    switch (Evt.EventId) {
    case FASTBOOT_EVT_R_GEAR_ON:
#if defined(R_TARGET_BOARD_ERGUOTOU)
        SVC_ToggleDisplay(true);
#else
        // RVC_ToggleDisplayLayer(RVC_ID_LAYER_CID, true);
        RVC_ToggleDisplayLayer(RVC_ID_LAYER_CLUSTER, true);
#endif
        break;

    case FASTBOOT_EVT_R_GEAR_OFF:
#if defined(R_TARGET_BOARD_ERGUOTOU)
        SVC_ToggleDisplay(false);
#else
        RVC_ToggleDisplayLayer(RVC_ID_LAYER_CLUSTER, false);
#endif
        break;

    default:
        break;
    }
}

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const fastboot_SubApp_t fastboot_SubAppCamera = {
    .ThreadFunc       = loc_CameraThread,
    .ThreadPriority   = FASTBOOT_CFG_CAMERA_TSK_PRIO,
    .EventHandlerFunc = loc_CameraEvtHandler,
};

#endif /* FASTBOOT_CFG_APP_CAMERA_ENABLE */
