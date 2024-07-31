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
 * File Name    : fastboot.h
 * Version      : 1.0
 * Description  : List API functions of fastboot demo app.
 *********************************************************************************************************************/
#ifndef FASTBOOT_H
#define FASTBOOT_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>

#include "rcar-xos/osal/r_osal.h"
#include "r_wm_api.h"
#include "r_print_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* fastboot event */
#define FASTBOOT_EVT_R_GEAR_ON  (1)
#define FASTBOOT_EVT_R_GEAR_OFF (2)

/* guideline event */
#define GUIDELINE_EVT_UPDATE_ANGLE (3)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct {
    uint32_t EventId;
    uint32_t Para1;
    uint32_t Para2;
    uint32_t Para3;
} fastboot_Event_t;

typedef struct {
    void * (*ThreadFunc)(void * Arg);
    uint32_t ThreadPriority;
    void (*EventHandlerFunc)(fastboot_Event_t Evt);
} fastboot_SubApp_t;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void Fastboot_TriggerEvent(fastboot_Event_t Evt);

#ifdef __cplusplus
}
#endif

#endif /* FASTBOOT_H */
