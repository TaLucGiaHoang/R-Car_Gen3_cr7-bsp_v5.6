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
 * File Name    : logo_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot_mono sub app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if FASTBOOT_CFG_APP_LOGO_ENABLE

#include "fastboot_cfg.h"

#include "fastboot.h"
#include "logo.h"
#include "incbin.h"
#include "display.h"
#include "logo_image.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define LOGO_CFG_IMG_WIDTH  (1920)
#define LOGO_CFG_IMG_HEIGHT (360)

#define FASTBOOT_CFG_LOGO_TSK_PRIO (OSAL_THREAD_PRIORITY_TYPE4)

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
#define RGB_IMAGE(member)     (rgb_image_hdmi.member)
logo_Config_t loc_LogoConfig;

static void * loc_LogoThread(void * Arg)
{
    loc_LogoConfig.DisplayPort = R_WM_DISP_PORT_HDMI0;
    loc_LogoConfig.LayerZIndex  = 1;
    loc_LogoConfig.LayerPosX    = 0;
    loc_LogoConfig.LayerPosY    = 1080 - LOGO_CFG_IMG_HEIGHT;
    loc_LogoConfig.ImageWidth   = LOGO_CFG_IMG_WIDTH;
    loc_LogoConfig.ImageHeight  = LOGO_CFG_IMG_HEIGHT;
    loc_LogoConfig.FrameBuffer = (uintptr_t)RGB_IMAGE(pbuffer);
    
    logo_main(&loc_LogoConfig);
    return 0;
}

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const fastboot_SubApp_t fastboot_SubAppLogo = {
    .ThreadFunc       = loc_LogoThread,
    .ThreadPriority   = FASTBOOT_CFG_LOGO_TSK_PRIO,
    .EventHandlerFunc = 0,
};

#endif /* FASTBOOT_CFG_APP_LOGO_ENABLE */
