/***********************************************************************************************************************
* Copyright [2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name :    r_osal_usr_cfg.h
* Version :      0.1.0
* Product Name : OSAL
* Device(s) :    R-Car
* Description :  OSAL resource configuration file
***********************************************************************************************************************/

#ifndef OSAL_USR_CFG_H
#define OSAL_USR_CFG_H

/*******************************************************************************************************************//**
 * includes
***********************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "rcar-xos/osal_usr_cfg/trampoline/r_osal_usr_cfg_info.h"
#include "rcar-xos/osal_usr_cfg/trampoline/r_osal_usr_cfg_control.h"


/*******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER_FREERTOS_USR_CFG
 * @defgroup OSAL_Private_Defines_UsrConfig User Configurator Private macro definitions
 *
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @def OSAL_RCFG_INNER_TIMEOUT
 * osal internal timeout value.
***********************************************************************************************************************/
#define OSAL_RCFG_INNER_TIMEOUT         (500L)

/*******************************************************************************************************************//**
 * @def OSAL_COND_TIMEOUT
 * Timeout value when relocking mutex with CondWait.
***********************************************************************************************************************/
#define OSAL_COND_TIMEOUT (600000L)

/** @} */

/*******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER_FREERTOS_USR_CFG
 * @defgroup OSAL_Private_Defines_Static_Variables_UsrConfig User Condifurator Private static variables
 *
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * osal user configuration : thread control structure - Only used by common code, that is not used by Trampoline OSAL
***********************************************************************************************************************/
static st_osal_thread_control_t gs_osal_thread_control[1];

/*******************************************************************************************************************//**
 * osal user configuration : mutex control structure - Only used by common code, that is not used by Trampoline OSAL
***********************************************************************************************************************/
static st_osal_mutex_control_t gs_osal_mutex_control[1];

/*******************************************************************************************************************//**
 * osal user configuration : cond control structure - Only used by common code, that is not used by Trampoline OSAL
***********************************************************************************************************************/
static st_osal_cond_control_t gs_osal_cond_control[1];

/*******************************************************************************************************************//**
 * osal user configuration : message queue control structure
***********************************************************************************************************************/
static st_osal_mq_control_t gs_osal_mq_control[OSAL_RCFG_MQ_MAX_NUM];

/*******************************************************************************************************************//**
 * osal user configuration : interrupt thread control structure - Only used by common code, that is not used by Trampoline OSAL
***********************************************************************************************************************/
static st_osal_interrupt_control_t gs_osal_interrupt_thread_control[1];

/** @} */

#endif /* OSAL_USR_CFG_H */
