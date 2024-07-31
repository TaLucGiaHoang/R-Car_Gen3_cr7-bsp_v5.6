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
 * File Name    : splash_screen_cfg.c
 * Version      : 1.0
 * Description  : Configuration of splash screen app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "fastboot_cfg.h"

#if FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE

#include "fastboot.h"
#include "dcu_sample.h"
#include "incbin.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define FASTBOOT_CFG_SPLASH_SCREEN_TSK_PRIO (OSAL_THREAD_PRIORITY_TYPE4)

#define SPLASH_CFG_IMG_WIDTH  (900)
#define SPLASH_CFG_IMG_HEIGHT (180)
#define SPLASH_CFG_IMG_NUMBER (40)

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*
 * INCBIN generates the following symbols (X=1...40):
 * const unsigned char gFlashXData[];
 * const unsigned char *const gFlashXEnd;
 * const unsigned int gFlashXSize;
 */
INCBIN(Flash1, "..//..//fastboot_mono//src//splash_screen//flash1.dcu");
INCBIN(Flash2, "..//..//fastboot_mono//src//splash_screen//flash2.dcu");
INCBIN(Flash3, "..//..//fastboot_mono//src//splash_screen//flash3.dcu");
INCBIN(Flash4, "..//..//fastboot_mono//src//splash_screen//flash4.dcu");
INCBIN(Flash5, "..//..//fastboot_mono//src//splash_screen//flash5.dcu");
INCBIN(Flash6, "..//..//fastboot_mono//src//splash_screen//flash6.dcu");
INCBIN(Flash7, "..//..//fastboot_mono//src//splash_screen//flash7.dcu");
INCBIN(Flash8, "..//..//fastboot_mono//src//splash_screen//flash8.dcu");
INCBIN(Flash9, "..//..//fastboot_mono//src//splash_screen//flash9.dcu");
INCBIN(Flash10, "..//..//fastboot_mono//src//splash_screen//flash10.dcu");
INCBIN(Flash11, "..//..//fastboot_mono//src//splash_screen//flash11.dcu");
INCBIN(Flash12, "..//..//fastboot_mono//src//splash_screen//flash12.dcu");
INCBIN(Flash13, "..//..//fastboot_mono//src//splash_screen//flash13.dcu");
INCBIN(Flash14, "..//..//fastboot_mono//src//splash_screen//flash14.dcu");
INCBIN(Flash15, "..//..//fastboot_mono//src//splash_screen//flash15.dcu");
INCBIN(Flash16, "..//..//fastboot_mono//src//splash_screen//flash16.dcu");
INCBIN(Flash17, "..//..//fastboot_mono//src//splash_screen//flash17.dcu");
INCBIN(Flash18, "..//..//fastboot_mono//src//splash_screen//flash18.dcu");
INCBIN(Flash19, "..//..//fastboot_mono//src//splash_screen//flash19.dcu");
INCBIN(Flash20, "..//..//fastboot_mono//src//splash_screen//flash20.dcu");
INCBIN(Flash21, "..//..//fastboot_mono//src//splash_screen//flash21.dcu");
INCBIN(Flash22, "..//..//fastboot_mono//src//splash_screen//flash22.dcu");
INCBIN(Flash23, "..//..//fastboot_mono//src//splash_screen//flash23.dcu");
INCBIN(Flash24, "..//..//fastboot_mono//src//splash_screen//flash24.dcu");
INCBIN(Flash25, "..//..//fastboot_mono//src//splash_screen//flash25.dcu");
INCBIN(Flash26, "..//..//fastboot_mono//src//splash_screen//flash26.dcu");
INCBIN(Flash27, "..//..//fastboot_mono//src//splash_screen//flash27.dcu");
INCBIN(Flash28, "..//..//fastboot_mono//src//splash_screen//flash28.dcu");
INCBIN(Flash29, "..//..//fastboot_mono//src//splash_screen//flash29.dcu");
INCBIN(Flash30, "..//..//fastboot_mono//src//splash_screen//flash30.dcu");
INCBIN(Flash31, "..//..//fastboot_mono//src//splash_screen//flash31.dcu");
INCBIN(Flash32, "..//..//fastboot_mono//src//splash_screen//flash32.dcu");
INCBIN(Flash33, "..//..//fastboot_mono//src//splash_screen//flash33.dcu");
INCBIN(Flash34, "..//..//fastboot_mono//src//splash_screen//flash34.dcu");
INCBIN(Flash35, "..//..//fastboot_mono//src//splash_screen//flash35.dcu");
INCBIN(Flash36, "..//..//fastboot_mono//src//splash_screen//flash36.dcu");
INCBIN(Flash37, "..//..//fastboot_mono//src//splash_screen//flash37.dcu");
INCBIN(Flash38, "..//..//fastboot_mono//src//splash_screen//flash38.dcu");
INCBIN(Flash39, "..//..//fastboot_mono//src//splash_screen//flash39.dcu");
INCBIN(Flash40, "..//..//fastboot_mono//src//splash_screen//flash40.dcu");

static const void * loc_ImageDataList[SPLASH_CFG_IMG_NUMBER] = {
    gFlash1Data,  gFlash2Data,  gFlash3Data,  gFlash4Data,  gFlash5Data,  gFlash6Data,  gFlash7Data,  gFlash8Data,
    gFlash9Data,  gFlash10Data, gFlash11Data, gFlash12Data, gFlash13Data, gFlash14Data, gFlash15Data, gFlash16Data,
    gFlash17Data, gFlash18Data, gFlash19Data, gFlash20Data, gFlash21Data, gFlash22Data, gFlash23Data, gFlash24Data,
    gFlash25Data, gFlash26Data, gFlash27Data, gFlash28Data, gFlash29Data, gFlash30Data, gFlash31Data, gFlash32Data,
    gFlash33Data, gFlash34Data, gFlash35Data, gFlash36Data, gFlash37Data, gFlash38Data, gFlash39Data, gFlash40Data,
};

static const uint32_t * loc_ImageSizeList[SPLASH_CFG_IMG_NUMBER] = {
    (uint32_t *)&gFlash1Size,  (uint32_t *)&gFlash2Size,  (uint32_t *)&gFlash3Size,  (uint32_t *)&gFlash4Size,
    (uint32_t *)&gFlash5Size,  (uint32_t *)&gFlash6Size,  (uint32_t *)&gFlash7Size,  (uint32_t *)&gFlash8Size,
    (uint32_t *)&gFlash9Size,  (uint32_t *)&gFlash10Size, (uint32_t *)&gFlash11Size, (uint32_t *)&gFlash12Size,
    (uint32_t *)&gFlash13Size, (uint32_t *)&gFlash14Size, (uint32_t *)&gFlash15Size, (uint32_t *)&gFlash16Size,
    (uint32_t *)&gFlash17Size, (uint32_t *)&gFlash18Size, (uint32_t *)&gFlash19Size, (uint32_t *)&gFlash20Size,
    (uint32_t *)&gFlash21Size, (uint32_t *)&gFlash22Size, (uint32_t *)&gFlash23Size, (uint32_t *)&gFlash24Size,
    (uint32_t *)&gFlash25Size, (uint32_t *)&gFlash26Size, (uint32_t *)&gFlash27Size, (uint32_t *)&gFlash28Size,
    (uint32_t *)&gFlash29Size, (uint32_t *)&gFlash30Size, (uint32_t *)&gFlash31Size, (uint32_t *)&gFlash32Size,
    (uint32_t *)&gFlash33Size, (uint32_t *)&gFlash34Size, (uint32_t *)&gFlash35Size, (uint32_t *)&gFlash36Size,
    (uint32_t *)&gFlash37Size, (uint32_t *)&gFlash38Size, (uint32_t *)&gFlash39Size, (uint32_t *)&gFlash40Size,
};

// TODO why add 2?
static uint32_t loc_FrameBuf[SPLASH_CFG_IMG_WIDTH * SPLASH_CFG_IMG_HEIGHT + 2] __attribute__((aligned(256)));

static const dcu_Config_t loc_SplashScreenConfig = {
    .DisplayPort      = R_WM_DISP_PORT_HDMI1,
    .LayerZIndex      = 3,
    .LayerPosX        = (1920 - SPLASH_CFG_IMG_WIDTH) / 2,
    .LayerPosY        = (1080 - SPLASH_CFG_IMG_HEIGHT) / 2,
    .ImageWidth       = SPLASH_CFG_IMG_WIDTH,
    .ImageHeight      = SPLASH_CFG_IMG_HEIGHT,
    .ImageNumber      = SPLASH_CFG_IMG_NUMBER,
    .ImageDataListRef = loc_ImageDataList,
    .ImageSizeListRef = loc_ImageSizeList,
    .FrameBuffer      = loc_FrameBuf,
    .RepeatCount      = 0xffffffff,
    .RepeatIntervalMs = 1000,
};

static void * loc_SplashScreenThread(void * Arg)
{
    Dcu_Main(&loc_SplashScreenConfig);
    return 0;
}

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const fastboot_SubApp_t fastboot_SubAppSplashScreen = {
    .ThreadFunc       = loc_SplashScreenThread,
    .ThreadPriority   = FASTBOOT_CFG_SPLASH_SCREEN_TSK_PRIO,
    .EventHandlerFunc = 0,
};

#endif /* FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE */
