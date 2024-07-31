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
/** OSAL resource configuration */

#ifndef OSAL_USR_CFG_CONTROL_H
#define OSAL_USR_CFG_CONTROL_H

#include "rcar-xos/osal/r_osal.h"


/*******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER_AUTOSAR_USR_CFG
 * @defgroup OSAL_Public_Defines_UsrConfig_control User Configurator Public macro definitions (control)
 *
 * @{
 **********************************************************************************************************************/
/** Offset to match the hardware interrupt number */
#define OSAL_INTERRUPT_SPI_OFFSET       (32UL)

/** Max value of the thread control table. */
#define OSAL_RCFG_THREAD_MAX_NUM        (2048)

/** Max value of the mutex control table */
#define OSAL_RCFG_MUTEX_MAX_NUM         (1024)

/** Max value of the condition variable control table */
#define OSAL_RCFG_COND_MAX_NUM          (1024)

/** Max value of the message queue control table */
#define OSAL_RCFG_MQ_MAX_NUM            (1024)

/** Max value of the interrupt thread control table. */
#define OSAL_RCFG_INTERRUPT_MAX_NUM     (256)

/** @} */

/*******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER_AUTOSAR_USR_CFG
 * @defgroup OSAL_Public_Type_UsrConfig_control User Configurator Public type definitions (control)
 *
 * @{
 **********************************************************************************************************************/

/*
 * Dummy types to allow r_osal_usr_cfg.c to compile. We don't use most of the functions in this file for our Trampoline
 * port, but since it's common for all OS, we can't modify it. This is the minimum needed to compile.
 */

typedef struct st_osal_thread_control {
    osal_milli_sec_t        inner_timeout;  /*!< Waiting time of lock inner_mtx */
    const struct st_osal_rcfg_thread_config *usr_config;      /*!< User configuration for thread */
} st_osal_thread_control_t;

typedef struct st_osal_mutex_control {
    osal_milli_sec_t    inner_timeout;      /*!< Waiting time of lock mutex object */
    const struct st_osal_rcfg_mutex_config *usr_config; /*!< User configuration for mutex */
} st_osal_mutex_control_t;

typedef struct st_osal_cond_control {
    osal_milli_sec_t    inner_timeout;      /*!< Waiting time of lock inner_mtx */
    osal_milli_sec_t    inner_cond_timeout; /*!< Waiting time of relocking mutex */
    const struct st_osal_rcfg_cond_config *usr_config; /*!< User configuration for cndition variable */
}st_osal_cond_control_t;

typedef struct st_osal_interrupt_control {
    osal_milli_sec_t      inner_timeout;    /*!< Waiting time of lock inner_mtx */
    const struct st_osal_interrupt_config *usr_config; /*!< User configuration for message queue */
}st_osal_interrupt_control_t;



/*******************************************************************************************************************//**
 * structure to control message queue.
***********************************************************************************************************************/
typedef struct st_osal_mq_control {
    char                *buffer;
    const struct st_osal_rcfg_mq_config *usr_config; /*!< User configuration for message queue */

    osal_milli_sec_t    inner_timeout;      /* NOT USED BY TRAMPOLINE, ONLY R_OSAL_RCFG_GetMqInfo() */
}st_osal_mq_control_t;


/*******************************************************************************************************************//**
* @brief            Get number of Message queue Control information.
* @return           size_t  number of Message queue Control information
***********************************************************************************************************************/
size_t R_OSAL_RCFG_GetNumOfMq(void);

/*******************************************************************************************************************//**
* @brief            Get Message queue Control information.
* @param[in]        num      To set the message queue number
* @param[out]       pp_info  To set the address of the message queue control structure pointer
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK   Success
* @retval           OSAL_RETURN_PAR  Parameter error
***********************************************************************************************************************/
e_osal_return_t R_OSAL_RCFG_GetMqInfo(uint32_t num, st_osal_mq_control_t **pp_info);


/** @} */

#endif /* OSAL_USR_CFG_CONTROL_H */
