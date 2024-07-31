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
 * File Name    : servcie_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot service cio and taurus.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdbool.h>
#include "rcar-xos/osal/r_osal.h"
#include "fastboot.h"
#include "cio_server.h"
#include "taurus_app.h"
#include "r_gpio_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define FASTBOOT_CFG_CIO_TSK_PRIO         (OSAL_THREAD_PRIORITY_TYPE6)
#define FASTBOOT_CFG_TAURUS_TSK_PRIO      (OSAL_THREAD_PRIORITY_TYPE5)
#define FASTBOOT_CFG_EVT_CHECKER_TSK_PRIO (OSAL_THREAD_PRIORITY_TYPE4)

#if defined(R_TARGET_BOARD_ERGUOTOU)
#define FASTBOOT_CFG_R_GEAR_INIT_STS (FASTBOOT_EVT_R_GEAR_OFF)
#else
#define FASTBOOT_CFG_R_GEAR_INIT_STS (FASTBOOT_EVT_R_GEAR_ON)
#endif

#define FASTBOOT_CFG_EVT_CHECKER_INTERVAL   (200)
#define FASTBOOT_CFG_R_GEAR_TOGGLE_INTERVAL (50)

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static void * loc_CioServerThread(void * Arg)
{
    cio_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static void * loc_TaurusThread(void * Arg)
{
    taurus_main();
    while (1) R_OSAL_ThreadSleepForTimePeriod(10);
    return 0;
}

static void * loc_EventCheckerThread(void * Arg)
{
    fastboot_Event_t evt;
    uint32_t         ms_counter      = 0;
    uint32_t         r_gear_sts      = FASTBOOT_CFG_R_GEAR_INIT_STS;
    bool             b_update_r_gear = true;
    bool             b_manual_mode   = false;

    /* Pin 4 of SW4 (GP5_23) as manual mode switch */
    /* Pin 3 of SW4 (GP5_22) as reverse gear shift switch */
    R_GPIO_SetGpio(5, 23, 0);
    R_GPIO_SetGpio(5, 22, 0);

    while (1) {
        /* Check board switch status */
        b_manual_mode = !(bool)R_GPIO_GetInput(5, 23);
        if (b_manual_mode) {
            if (FASTBOOT_EVT_R_GEAR_ON == r_gear_sts) {
                if (false == !(bool)R_GPIO_GetInput(5, 22)) {
                    b_update_r_gear = true;
                }
            }
            else {
                if (true == !(bool)R_GPIO_GetInput(5, 22)) {
                    b_update_r_gear = true;
                }
            }
        }

        /* Toggle R gear */
        if (b_update_r_gear) {
            b_update_r_gear = false;
            if (FASTBOOT_EVT_R_GEAR_OFF == r_gear_sts) {
                r_gear_sts = FASTBOOT_EVT_R_GEAR_ON;
            }
            else {
                r_gear_sts = FASTBOOT_EVT_R_GEAR_OFF;
            }
            evt.EventId = r_gear_sts;
            Fastboot_TriggerEvent(evt);
        }

        /* Update time counter */
        R_OSAL_ThreadSleepForTimePeriod(FASTBOOT_CFG_EVT_CHECKER_INTERVAL);
        ms_counter++;
        if (!b_manual_mode) {
            if (0 == (ms_counter % FASTBOOT_CFG_R_GEAR_TOGGLE_INTERVAL)) {
                b_update_r_gear = true;
            }
        }
    }

    return 0;
}

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const fastboot_SubApp_t fastboot_SubAppCio = {
    .ThreadFunc       = loc_CioServerThread,
    .ThreadPriority   = FASTBOOT_CFG_CIO_TSK_PRIO,
    .EventHandlerFunc = 0,
};

const fastboot_SubApp_t fastboot_SubAppTaurus = {
    .ThreadFunc       = loc_TaurusThread,
    .ThreadPriority   = FASTBOOT_CFG_TAURUS_TSK_PRIO,
    .EventHandlerFunc = 0,
};

const fastboot_SubApp_t fastboot_SubAppEvtChecker = {
    .ThreadFunc       = loc_EventCheckerThread,
    .ThreadPriority   = FASTBOOT_CFG_EVT_CHECKER_TSK_PRIO,
    .EventHandlerFunc = 0,
};
