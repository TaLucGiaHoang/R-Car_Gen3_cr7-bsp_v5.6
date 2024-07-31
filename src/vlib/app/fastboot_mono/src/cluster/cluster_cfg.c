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
 * File Name    : cluster_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot_mono sub app.
 *********************************************************************************************************************/
#if FASTBOOT_CFG_APP_CLUSTER_ENABLE

#include "fastboot_cfg.h"

#include "fastboot.h"
#include "2dwow.h"
#include "display.h"

#define FASTBOOT_CFG_CLUSTER_TSK_PRIO (108)
#define CLUSTER_CFG_IMG_WIDTH  (1920)
#define CLUSTER_CFG_IMG_HEIGHT (720)

const cluster_Config_t cluster_FastbootConfig = {
#if defined(R_TARGET_BOARD_SALVATORXS)
    // .Port = R_WM_DISP_PORT_HDMI0,
    .Port = R_WM_DISP_PORT_LVDS0,
#elif defined(R_TARGET_BOARD_ERGUOTOU)
    .Port = R_WM_DISP_PORT_LVDS0,
#else
#error "Specified board type is not supported!"
#endif
    .PosX   = (1920 - CLUSTER_CFG_IMG_WIDTH) / 2,
    .PosY   = 0,
    .PosZ   = 1,
    .Width  = CLUSTER_CFG_IMG_WIDTH,
    .Height = CLUSTER_CFG_IMG_HEIGHT,
};


static void * loc_ClusterThread(void * Arg)
{
    x2dwow_main(&cluster_FastbootConfig);
    return 0;
}

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const fastboot_SubApp_t fastboot_SubAppCluster = {
    .ThreadFunc       = loc_ClusterThread,
    .ThreadPriority   = FASTBOOT_CFG_CLUSTER_TSK_PRIO,
    .EventHandlerFunc = 0,
};

#endif /* FASTBOOT_CFG_APP_CLUSTER_ENABLE */
