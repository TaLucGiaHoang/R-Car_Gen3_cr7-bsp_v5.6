/***********************************************************************************************************************
* Copyright [2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
* Description :  OSAL wrapper common for Trampoline (AUTOSAR) header file
***********************************************************************************************************************/

#ifndef OSAL_COMMON_H
#define OSAL_COMMON_H

/* Trampoline (AUTOSAR) OS */
#include "tpl_os.h"

#include "rcar-xos/osal/r_osal.h"
#include "rcar-xos/osal_usr_cfg/trampoline/r_osal_usr_cfg_info.h"
#include "rcar-xos/osal_usr_cfg/trampoline/r_osal_usr_cfg_control.h"
#include "rcar-xos/osal_dev_cfg/trampoline/r_osal_dev_cfg_info.h"
#include "rcar-xos/osal_dev_cfg/trampoline/r_osal_dev_cfg_control.h"

/*******************************************************************************************************************//**
 * @defgroup OSAL_WRAPPER_TRAMPOLINE OSAL Wrapper For Trampoline
 * @ingroup osal_wrapper
 * OSAL Wrapper for Trampoline
 * @{
 **********************************************************************************************************************/
/******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER
 * @defgroup OSAL_Private_Defines_Common Common macro definitions
 *
 * @{
 *********************************************************************************************************************/
 
/*******************************************************************************************************************//**
 * @def OSAL_DEBUG
 * debug print.
***********************************************************************************************************************/
void printf_raw(const char *format, ...);
void log_printf(const char *format, ...);
void log_flush(void);
void dbg_show_tasks(void);

#define OSAL_DEBUG(...)
// #define OSAL_DEBUG log_printf
#define OSAL_DEBUG_WARNING printf_raw
#define OSAL_DEBUG_ERROR printf_raw
#define OSAL_DEBUG_INFO printf

/*******************************************************************************************************************//**
 * @def OSAL_STATIC
 * STATIC for Unit test
***********************************************************************************************************************/
#ifdef UNIT_TEST
#define OSAL_STATIC
#else
#define OSAL_STATIC static
#endif

/*******************************************************************************************************************//**
 * @def OSAL_MAX_SEC
 * Max value of second
***********************************************************************************************************************/
#define OSAL_MAX_SEC   (0x7FFFFFFFL)

/*******************************************************************************************************************//**
 * @def OSAL_MAX_TV_NSEC
 * Max value of nano second
***********************************************************************************************************************/
#define OSAL_MAX_TV_NSEC (1000000000L - 1L)

/*******************************************************************************************************************//**
 * @def OSAL_SECTOMSEC
 * Mask value for converting second to miili second
***********************************************************************************************************************/
#define OSAL_SECTOMSEC (1000L)

/*******************************************************************************************************************//**
 * @def OSAL_SECTONSEC
 * Mask value for converting second to nano second
***********************************************************************************************************************/
#define OSAL_SECTONSEC (1000000000L)

/*******************************************************************************************************************//**
 * @def OSAL_MSECTONSEC
 * Mask value for converting milli second to nano second
***********************************************************************************************************************/
#define OSAL_MSECTONSEC (1000000L)



/*******************************************************************************************************************//**
 * @def OSAL_READ_CPUID
 * OSAL_READ_CPUID macro definition for unit test
***********************************************************************************************************************/
#define OSAL_READ_CPUID(reg)  __asm volatile ("mrs %0, MPIDR_EL1" : "=r"((reg)))

/*******************************************************************************************************************//**
 * @def OSAL_CONV_PRIORITY
 * Convert priority level (30 steps -> 256 steps)
***********************************************************************************************************************/
#define OSAL_CONV_PRIORITY(prio)  ((uint16_t)((prio * 255) / 29))

/** @} */

/*******************************************************************************************************************//**
 * @typedef osal_inner_thread_attr_t
 * @brief   thread attribute. \n
 *          This type is a typedef definition for a pointer to a struct st_osal_inner_thread_attr.
 **********************************************************************************************************************/
typedef struct st_osal_inner_thread_attr *osal_inner_thread_attr_t;

/*******************************************************************************************************************//**
 * @typedef osal_inner_thread_handle_t
 * @brief   Handle of Thread. \n
 *          This type is a typedef definition for a pointer to a st_osal_inner_thread_control.
 **********************************************************************************************************************/
typedef struct st_osal_inner_thread_control *osal_inner_thread_handle_t;

/*******************************************************************************************************************//**
 * @typedef osal_inner_mutexattr_t
 * @brief   mutex attribute. \n
 *          This type is a typedef definition for a pointer to a struct st_osal_inner_mutexattr.
 **********************************************************************************************************************/
typedef struct st_osal_inner_mutexattr *osal_inner_mutexattr_t;

/*******************************************************************************************************************//**
 * @typedef osal_inner_mutex_handle_t
 * @brief   Handle of Mutex. \n
 *          This type is a typedef definition for a pointer to a struct st_osal_inner_mutex_control.
 **********************************************************************************************************************/
typedef struct st_osal_inner_mutex_control *osal_inner_mutex_handle_t;

/*******************************************************************************************************************//**
 * @typedef osal_inner_cond_handle_t
 * @brief   Handle of condition. \n
 *          This type is a typedef definition for a pointer to a struct st_osal_inner_cond_control.
 **********************************************************************************************************************/
typedef struct st_osal_inner_cond_control *osal_inner_cond_handle_t;

/*******************************************************************************************************************//**
 * @typedef osal_inner_mq_handle_t
 * @brief   Message information. \n
 *          This type is a typedef definition for a pointer to a struct st_osal_inner_mq_control.
 **********************************************************************************************************************/
typedef struct R_OS_Internal_MqData_t *osal_inner_mq_handle_t;

/*******************************************************************************************************************//**
 * @typedef p_osal_inner_thread_func_t
 * @brief   inner thread worker function.
 **********************************************************************************************************************/
typedef void (*p_osal_inner_thread_func_t)(void* user_arg);

/*******************************************************************************************************************//**
 * @enum e_osal_inner_thread_policy_t
 * Thread Policy definitions.
***********************************************************************************************************************/
typedef enum e_osal_inner_thread_policy
{
    OSAL_SCHED_INVALID = 0,
    OSAL_SCHED_FIFO,
    OSAL_SCHED_RR
}e_osal_inner_thread_policy_t;

/*******************************************************************************************************************//**
 * @st_osal_inner_thread_attrinfo_t
 * structure to thread attribute information.
***********************************************************************************************************************/
/* PRQA S 3630 1 # This definition must be published because the another module reference the member after calling. */
typedef struct st_osal_inner_thread_attrinfo
{
    e_osal_inner_thread_policy_t policy;
    e_osal_thread_priority_t     priority;
    size_t                       stack_size;
} st_osal_inner_thread_attrinfo_t;

/*******************************************************************************************************************//**
 * @struct st_osal_inner_thread_config_t
 * structure to thread configuration.
***********************************************************************************************************************/
/* PRQA S 3630 1 # This definition must be published because the another module reference the member after calling. */
typedef struct st_osal_inner_thread_config
{
    osal_inner_thread_attr_t    *attr;
    p_osal_inner_thread_func_t  func;      /*!< Worker function */
    void*                       userarg;   /*!< Pointer of Argument for worker function */
} st_osal_inner_thread_config_t;

/*******************************************************************************************************************//**
 * @struct st_osal_inner_mq_config_t
 * structure to Message queue configuration.
***********************************************************************************************************************/
/* PRQA S 3630 1 # This definition must be published because the another module reference the member after calling. */
typedef struct st_osal_inner_mq_config
{
    uint32_t max_num_msg;
    uint32_t msg_size;
} st_osal_inner_mq_config_t;

/*******************************************************************************
  Mq structure for internal use (Power Management Agent)
*******************************************************************************/
typedef struct {
    osal_mq_id_t               Id;
    unsigned int               Open;
    st_osal_inner_mq_config_t  p_config;
    base_mutex_t               Lock;
    base_sema_t                Signal;
    base_sema_t                Space;
    char                       *Buffer;
    char                       *Start;
    char                       *End;
    int                        ValidByte;
} R_OS_Internal_MqData_t;

/******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER
 * @defgroup OSAL_Private_Functions_Common Common function definitions
 *
 * @{
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
* @brief            Initialize Mutex Synchronization Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_MEM
* @retval           OSAL_ERROR_CONF
* @retval           OSAL_ERROR_FAIL
* @retval           OSAL_ERROR_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_mutex_initialize(void);

/*******************************************************************************************************************//**
* @brief            Initialize Cond Synchronization Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_MEM
* @retval           OSAL_ERROR_CONF
* @retval           OSAL_ERROR_FAIL
* @retval           OSAL_ERROR_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_cond_initialize(void);

/*******************************************************************************************************************//**
* @brief            Deinitialize Message Queue Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_FAIL
***********************************************************************************************************************/
extern e_osal_return_t osal_mq_deinitialize(void);

/*******************************************************************************************************************//**
* @brief            Initialize Message Queue Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_MEM
* @retval           OSAL_ERROR_CONF
* @retval           OSAL_ERROR_FAIL
* @retval           OSAL_ERROR_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_mq_initialize(void);

/*******************************************************************************************************************//**
* @brief            Resolve all SoC specific dependencies (choose correct device and resource settings).
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
***********************************************************************************************************************/
extern e_osal_return_t osal_resolve_soc_dependencies(void);

/*******************************************************************************************************************//**
* @brief            Initialize Clock Time Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
***********************************************************************************************************************/
extern e_osal_return_t osal_clock_time_initialize(void);

/*******************************************************************************************************************//**
* @brief            Deinitialize Clock Time Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_FAIL
***********************************************************************************************************************/
extern e_osal_return_t osal_clock_time_deinitialize(void);

/*******************************************************************************************************************//**
* @brief            Initialize interrupt Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_MEM
* @retval           OSAL_ERROR_CONF
* @retval           OSAL_ERROR_FAIL
* @retval           OSAL_ERROR_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_interrupt_initialize(void);

/*******************************************************************************************************************//**
* @brief            Initialize IO Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_MEM
* @retval           OSAL_ERROR_CONF
* @retval           OSAL_ERROR_FAIL
* @retval           OSAL_ERROR_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_io_initialize(void);

/*******************************************************************************************************************//**
* @brief            Deinitialize IO Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_FAIL
***********************************************************************************************************************/
extern e_osal_return_t osal_io_deinitialize(void);

/*******************************************************************************************************************//**
* @brief            Initialize Power Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_MEM
* @retval           OSAL_ERROR_CONF
* @retval           OSAL_ERROR_FAIL
* @retval           OSAL_ERROR_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_pm_initialize(void);

/*******************************************************************************************************************//**
* @brief            Deinitialize Power Manager.
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_FAIL
***********************************************************************************************************************/
extern e_osal_return_t osal_pm_deinitialize(void);

/*******************************************************************************************************************//**
* @brief            Initialize Memory Manager.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_STATE
* @retval           OSAL_RETURN_MEM
***********************************************************************************************************************/
extern e_osal_return_t osal_memory_initialize(void);

/*******************************************************************************************************************//**
* @brief            Deinitialize Memory Manager.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_STATE
***********************************************************************************************************************/
extern e_osal_return_t osal_memory_deinitialize(void);

/*******************************************************************************************************************//**
* @brief            Get ISR Context
* @param[out]       is_rsr  To set the address of isr status
* @return           none
***********************************************************************************************************************/
extern void R_OSAL_Internal_GetISRContext(bool *is_rsr);

/*******************************************************************************************************************//**
* @brief            Check Timestamp
* @param[in]        p_time_stamp  To set the time stamp
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_PAR
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_CheckTimestamp(const st_osal_time_t *const p_time_stamp);

/*******************************************************************************************************************//**
* @brief            Get Timestamp
* @param[out]       p_time_stamp  To set the address of time stamp
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_PAR
* @retval           OSAL_ERROR_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_GetTimestamp(st_osal_time_t *const p_time_stamp);

/*******************************************************************************************************************//**
* @brief            Convert tick time to millisec
* @param[out]       time_period  To set the time period
* @param[in]        tick  To set the address of tick time
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_PAR
***********************************************************************************************************************/
extern void R_OSAL_Internal_TickToMillisec(osal_milli_sec_t *time_period, TickType tick);

/*******************************************************************************************************************//**
* @brief            Convert millisec to tick time
* @param[in]        time_period  To set the time period
* @param[out]       tick  To set the address of tick time
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_PAR
***********************************************************************************************************************/
extern void R_OSAL_Internal_MillisecToTick(osal_milli_sec_t time_period, TickType *tick);

/*******************************************************************************************************************//**
* @brief            Get difference time
* @param[in]        p_time2  To set the time stamp
* @param[in]        p_time1  To set the time stamp
* @param[out]       p_time_difference  To set the address of difference time
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_PAR
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_GetDiffTime(const st_osal_time_t *const p_time2,
                                                   const st_osal_time_t *const p_time1,
                                                   osal_nano_sec_t *const p_time_difference);

/*******************************************************************************************************************//**
* @brief            Calcurate difference time
* @param[in]        p_time2  To set the time stamp
* @param[in]        p_time1  To set the time stamp
* @param[out]       p_time_difference  To set the address of difference time
* @return           e_osal_return_t
* @retval           OSAL_ERROR_OK
* @retval           OSAL_ERROR_PAR
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_CalcDiffTime(const st_osal_time_t *const p_time2,
                                                    const st_osal_time_t *const p_time1,
                                                    osal_nano_sec_t *const p_time_difference);

/*******************************************************************************************************************//**
* @brief            Sleep in milli second.
* @param[in]        milliseconds  Specify the sleep time in milliseconds.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_PAR
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MsSleep(osal_milli_sec_t milliseconds);

/*******************************************************************************************************************//**
* @brief            Set the attribute.
* @param[in]        p_attr_info    A pointer to thread attribute information.
* @param[in,out]    p_attr         A pointer to osal_inner_thread_attr_t.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_MEM
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_SetThreadAttr(const st_osal_inner_thread_attrinfo_t *const p_attr_info,
                                                     osal_inner_thread_attr_t *const p_attr);

/*******************************************************************************************************************//**
* @brief            Create a new thread in the calling process as the configuration specified in p_config.
* @param[in]        p_config       A pointer to thread configuration.
* @param[in,out]    p_handle       A pointer to Handle of Thread.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_MEM
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_ThreadCreate(const st_osal_inner_thread_config_t *const p_config,
                                                    osal_inner_thread_handle_t *const p_handle);

/*******************************************************************************************************************//**
* @brief            Join a thread, with a time limit.
* @param[in]        handle         Handle of Thread.
* @param[out]       p_return_value A pointer to NULL, or a pointer to a location where the function can store the value.
* @param[in]        time_period    Specify the timeout time in milliseconds.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_ThreadTimedJoin(osal_inner_thread_handle_t const handle,
                                                       int32_t *const p_return_value,
                                                       const osal_milli_sec_t time_period);

/*******************************************************************************************************************//**
* @brief            Set the mutex attribute.
* @param[in,out]    p_mutex_attr   A pointer to mutex attribute.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_MEM
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_SetMutexAttr(osal_inner_mutexattr_t *const p_mutex_attr);

/*******************************************************************************************************************//**
* @brief            Initialize mutex assigned and set acquired mutex handle to p_handle.
* @param[in]        p_mutex_attr   A pointer to mutex attribute.
* @param[in,out]    p_handle       The address of osal_inner_mutex_handle_t.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_MEM
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MutexInit(const osal_inner_mutexattr_t *const p_mutex_attr,
                                                 osal_inner_mutex_handle_t *const p_handle);

/*******************************************************************************************************************//**
* @brief            Lock a mutex(timed lock).
* @param[in]        handle        The address of osal_inner_mutex_handle_t.
* @param[in]        time_period   Specify the timeout time in milliseconds.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_TIME
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MutexTimedLock(osal_inner_mutex_handle_t handle,
                                                      osal_milli_sec_t time_period);

/*******************************************************************************************************************//**
* @brief            Unlock mutex assigned to handle.
* @param[in]        handle         The address of osal_inner_mutex_handle_t.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Internal_MutexUnlock(osal_inner_mutex_handle_t handle);

/*******************************************************************************************************************//**
* @brief            Destroy mutex assigned to handle.
* @param[in]        handle         The address of osal_inner_mutex_handle_t.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MutexDestroy(osal_inner_mutex_handle_t handle);

/*******************************************************************************************************************//**
* @brief            Create the message queue in the configuration specified in p_config.
* @param[in]        p_config       A pointer to Message queue configuration.
* @param[in,out]    p_handle       The address of osal_inner_mq_handle_t. 
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_CONF
* @retval           OSAL_RETURN_FAIL
* @retval           OSAL_RETURN_MEM
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MqCreate(const st_osal_inner_mq_config_t *const p_config,
                                                osal_inner_mq_handle_t *const p_handle);

/*******************************************************************************************************************//**
* @brief            Send a message to message queue with timeout until time stamp.
* @param[in]        handle         The address of osal_inner_mq_handle_t.
* @param[in]        time_period    Specify the timeout time in milliseconds.
* @param[in]        p_buffer       The address of void* for the send data.
* @param[in]        buffer_size    The p_buffer size by Byte. 
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_BUSY
* @retval           OSAL_RETURN_TIME
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MqTimedSend(osal_inner_mq_handle_t handle,
                                                   osal_milli_sec_t time_period,
                                                   const void * p_buffer,
                                                   size_t buffer_size);

/*******************************************************************************************************************//**
* @brief            Receive a message to message queue with timeout until time stamp.
* @param[in]        handle         The address of osal_inner_mq_handle_t.
* @param[in]        time_period    Specify the timeout time in milliseconds.
* @param[out]       p_buffer       The address of void* for the receive data.
* @param[in]        buffer_size    The p_buffer size by Byte. 
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_BUSY
* @retval           OSAL_RETURN_TIME
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MqTimedReceive(osal_inner_mq_handle_t handle,
                                                      osal_milli_sec_t time_period,
                                                      void * p_buffer,
                                                      size_t buffer_size);

/*******************************************************************************************************************//**
* @brief            Delete message queue assigned to handle.
* @param[in]        handle         The address of osal_inner_mq_handle_t.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_MqDelete(osal_inner_mq_handle_t handle);

/*******************************************************************************************************************//**
* @brief            Acquire the time stamp on system according to HIGH_RESOLUTION.
* @param[out]       p_time_stamp   Set the pointer address of st_osal_time_t as timestamp.
* @return           e_osal_return_t
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_GetHighResoTimeStamp(st_osal_time_t *const p_time_stamp);

/*******************************************************************************************************************//**
* @brief           Acquire the difference between two times.
* @param[in]       p_time2           Set the pointer address of st_osal_time_t as timestamp.
* @param[in]       p_time1           Set the pointer address of st_osal_time_t as timestamp.
* @param[in,out]   p_time_difference To set the address of osal_nano_sec_t.
*                                    The OSAL sets the difference between time2 and time1 with a signed value.
* @return          e_osal_return_t
* @retval          OSAL_RETURN_OK
* @retval          OSAL_RETURN_FAIL
***********************************************************************************************************************/
extern e_osal_return_t R_OSAL_Internal_CalcTimeDifference(const st_osal_time_t *const p_time2, 
                                                          const st_osal_time_t *const p_time1,
                                                          osal_nano_sec_t *const p_time_difference);


/** @} */

/** @} OSAL_WRAPPER_TRAMPOLINE */

#endif /* OSAL_COMMON_H */
