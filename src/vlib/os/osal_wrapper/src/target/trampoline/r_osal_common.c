/*************************************************************************************************************
* OSAL Wrapper
* Copyright (c) [2020-2022] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/***********************************************************************************************************************
* File Name :    r_osal_common.c
* Version :      1.0.0
* Product Name : OSAL
* Device(s) :    R-Car
* Description :  OSAL wrapper common functions for Trampoline (OSS AutoSAR)
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * includes
***********************************************************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "tpl_os.h"

#include "target/trampoline/r_osal_common.h"
#include "target/trampoline/r_osal_dummy.h"

/*******************************************************************************************************************//**
 * @var gs_osal_common_is_initialized
 * Initialize flag
***********************************************************************************************************************/
OSAL_STATIC bool gs_osal_common_is_initialized = false;

typedef struct st_mutex_info
{
    osal_mutex_id_t id;
    base_mutex_t mtx;
} st_internal_mutex_info_t;

static st_internal_mutex_info_t loc_Internal_Mutexes[OSAL_RCFG_MUTEX_MAX_NUM];

static R_OS_Internal_MqData_t R_OS_Internal_MqPool[OSAL_RCFG_MQ_MAX_NUM];

/*******************************************************************************
  Allocate a MQ data object from the pool.

  Returns:
  ==0 - No object allocated
  !=0 - Pointer to am MQ data object
*/
static R_OS_Internal_MqData_t *loc_AllocMqData(void)
{
    unsigned int i;

    i = 0;
    while (i < OSAL_RCFG_MQ_MAX_NUM) {
        if (R_OS_Internal_MqPool[i].Id == 0) {
            break;
        }
        i++;
    }
    if (i >= OSAL_RCFG_MQ_MAX_NUM) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        return NULL;
    }

    return &R_OS_Internal_MqPool[i];
}

static st_internal_mutex_info_t *loc_FindMutex(base_mutex_t *mutex)
{
    int i;

    for (i = 0; i < OSAL_RCFG_MUTEX_MAX_NUM; i++) {
        if (mutex == &loc_Internal_Mutexes[i].mtx) {
            return &loc_Internal_Mutexes[i];
        }
    }

    return NULL;
}

static st_internal_mutex_info_t *loc_FindUnusedMutex(void)
{
    int i;

    for (i = 0; i < OSAL_RCFG_MUTEX_MAX_NUM; i++) {
        if (loc_Internal_Mutexes[i].id == OSAL_MUTEX_INVALID_ID) {
            return &loc_Internal_Mutexes[i];
        }
    }

    return NULL;
}

e_osal_return_t osal_internal_mutex_initialize(void)
{
    int i;

    for (i = 0; i < OSAL_RCFG_MUTEX_MAX_NUM; i++) {
        loc_Internal_Mutexes[i].id = OSAL_MUTEX_INVALID_ID;
    }

    return OSAL_RETURN_OK;
}

/*******************************************************************************
  Find existing MQ by its Id.

  Returns:
  ==0 - No object found
  !=0 - Pointer to am MQ data object
*/
static R_OS_Internal_MqData_t *loc_FindMqById(osal_mq_id_t Id)
{
    unsigned int i;

    i = 0;
    while (i < OSAL_RCFG_MQ_MAX_NUM) {
        if (R_OS_Internal_MqPool[i].Id == Id) {
            break;
        }
        i++;
    }
    if (i >= OSAL_RCFG_MQ_MAX_NUM) {
        return 0;
    }
    return &R_OS_Internal_MqPool[i];
}

/******************************************************************************************************************//**
 * @ingroup OSAL_WRAPPER
 * @defgroup OSAL_Private_Defines_Common Common macro definitions
 *
 * @{
 *********************************************************************************************************************/

/** @} */

int g_OsalSchedulerStarted;

/** @} */

/* Activate all other tasks that were created before R_OSAL_StartOS().
 * This is called by the "startup_task". This task is special in that it is the
 * only task that is automatically started by Autosar in our environment.
 */
static void loc_StartInitialThreads(void)
{
    osal_autosar_thread_t *thisthread = R_OSAL_Sys_GetCurrentThreadData();
    int nr_threads;
    osal_autosar_thread_t *thread;
    int i;
    StatusType ret;

    /* Protected access to task list */
    GetResource(osal_mutex);

    nr_threads = R_OSAL_Sys_GetNrThreads();

    /* Activate all tasks that were created before the OS was started */
    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        if (thread != thisthread && thread->Task != NULL) {
            ret = ActivateTask(*(thread->AutosarTask_id));
            if (ret != E_OK) {
                printf_raw("ERROR: %s: ActivateTask() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            }
        }
    }

    ReleaseResource(osal_mutex);
    /* end protected access to task list */
}


/***********************************************************************************************************************
* Start of function R_OSAL_Initialize()
* XOS1_OSAL_CD_CD_800
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_293]
* [Covers: XOS1_OSAL_UD_UD_156]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Initialize(void)
{

    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    if (true == gs_osal_common_is_initialized)
    {
        osal_ret = OSAL_RETURN_STATE;
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_resolve_soc_dependencies();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_clock_time_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_mutex_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_internal_mutex_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_cond_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_mq_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_io_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_pm_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_interrupt_initialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_memory_initialize();
    }
#if 0	/* TODO: Add for iVDP1C dummy osal */
	/* power on */
	osal_dummy_power_on();
	/* enable clock */
	osal_dummy_clock_enable();
#endif
    if (OSAL_RETURN_OK == osal_ret)
    {
        gs_osal_common_is_initialized = true;
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Initialize()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Deinitialize()
* XOS1_OSAL_CD_CD_801
* [Covers: XOS1_OSAL_UD_UD_002]
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_293]
* [Covers: XOS1_OSAL_UD_UD_157]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Deinitialize(void)
{

    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    OSAL_DEBUG("%s\n", __func__);
    if (true != gs_osal_common_is_initialized)
    {
        osal_ret = OSAL_RETURN_STATE;
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_clock_time_deinitialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret =  osal_mq_deinitialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_io_deinitialize();
    }
#if 1
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_pm_deinitialize();
    }
#endif
    if (OSAL_RETURN_OK == osal_ret)
    {
        osal_ret = osal_memory_deinitialize();
    }
    if (OSAL_RETURN_OK == osal_ret)
    {
        gs_osal_common_is_initialized = false;
    }
    OSAL_DEBUG("%s End\n", __func__);
    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Deinitialize()
***********************************************************************************************************************/

e_osal_return_t R_OSAL_StartOS(void* Arg)
{
    OSAL_DEBUG_INFO("=== Trampoline OS Start ===\n");
    g_OsalSchedulerStarted = 1;
    StartOS(OSDEFAULTAPPMODE);

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_CriticalEnter(void)
{
    DisableAllInterrupts();
    return OSAL_RETURN_OK;
}


e_osal_return_t R_OSAL_CriticalExit(void)
{
    EnableAllInterrupts();
    return OSAL_RETURN_OK;
}

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_GetISRContext()
* XOS1_OSAL_CD_CD_802
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_293]
* [Covers: XOS1_OSAL_UD_UD_338]
***********************************************************************************************************************/
void R_OSAL_Internal_GetISRContext(bool *is_rsr)
{
    if (NULL != is_rsr) {
        *is_rsr = false;    /* task context */
    }
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_GetISRContext()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_CheckTimestamp()
* XOS1_OSAL_CD_CD_811
* [Covers: XOS1_OSAL_UD_UD_341]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Internal_CheckTimestamp(const st_osal_time_t *const p_time_stamp)
{

    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    if (NULL == p_time_stamp)
    {
        osal_ret = OSAL_RETURN_PAR;
    }
    else if (OSAL_MAX_SEC < p_time_stamp->tv_sec)
    {
        osal_ret = OSAL_RETURN_PAR;
    }
    else if ((OSAL_MAX_TV_NSEC < p_time_stamp->tv_nsec) || (0L > p_time_stamp->tv_nsec))
    {
        osal_ret = OSAL_RETURN_PAR;
    }
    else
    {
        osal_ret = OSAL_RETURN_OK;
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_CheckTimestamp()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_GetTimestamp()
* XOS1_OSAL_CD_CD_808
* [Covers: XOS1_OSAL_UD_UD_344]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Internal_GetTimestamp(st_osal_time_t *const p_time_stamp)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    TickType current_tick;
    osal_milli_sec_t current_milli_sec;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    if (NULL == p_time_stamp)
    {
        OSAL_DEBUG("Thread Get TimeStamp: OSAL_RETURN_RAR\n");
        osal_ret = OSAL_RETURN_PAR;
    }

    if (OSAL_RETURN_OK == osal_ret)
    {
        current_tick = R_OS_Get1msTickCount();

        R_OSAL_Internal_TickToMillisec(&current_milli_sec, current_tick);
        p_time_stamp->tv_sec = (time_t)(current_milli_sec / OSAL_SECTOMSEC);
        p_time_stamp->tv_nsec = (int32_t)((current_milli_sec - (p_time_stamp->tv_sec * OSAL_SECTOMSEC)) * OSAL_MSECTONSEC);

        if ((0 > p_time_stamp->tv_nsec) || (OSAL_MAX_TV_NSEC < p_time_stamp->tv_nsec))
        {
            OSAL_DEBUG("Thread Get TimeStamp: OSAL_RETURN_FAIL\n");
            osal_ret = OSAL_RETURN_FAIL;
        }
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_GetTimestamp()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_GetDiffTime()
* XOS1_OSAL_CD_CD_812
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_337]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Internal_GetDiffTime(const st_osal_time_t *const p_time2,
                                            const st_osal_time_t *const p_time1,
                                            osal_nano_sec_t *const p_time_difference)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret;
    osal_nano_sec_t p_nano_time;


    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    if ((NULL == p_time2) || (NULL == p_time1) || (NULL == p_time_difference))
    {
        OSAL_DEBUG("OSAL_RETURN_PAR\n");
        osal_ret = OSAL_RETURN_PAR;
    }
    else
    {
        osal_ret = R_OSAL_Internal_CalcDiffTime(p_time2, p_time1, &p_nano_time);

        if (OSAL_RETURN_OK == osal_ret)
        {
            if (((int64_t)(OSAL_MAX_SEC / 2) * (int64_t)(OSAL_SECTONSEC)) < p_nano_time)
            {
                *p_time_difference = 0;
            }
            else
            {
                *p_time_difference = p_nano_time;
            }
        }
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_GetDiffTime()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_CalcDiffTime()
* XOS1_OSAL_CD_CD_809
* [Covers: XOS1_OSAL_UD_UD_345]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_Internal_CalcDiffTime(const st_osal_time_t *const p_time2,
                                             const st_osal_time_t *const p_time1,
                                             osal_nano_sec_t *const p_time_difference)
{

    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int64_t result_sec;
    int64_t result_nsec;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    if ((NULL == p_time2) || (NULL == p_time1) || (NULL == p_time_difference))
    {
        OSAL_DEBUG("OSAL_RETURN_PAR\n");
        osal_ret = OSAL_RETURN_PAR;
    }
    else
    {
        if (p_time2->tv_sec > p_time1->tv_sec) /* Normal case */
        {
            OSAL_DEBUG("p_time2->tv_sec > p_time1->tv_sec\n");
            result_sec = (int64_t)(p_time2->tv_sec - p_time1->tv_sec);
            result_nsec = (int64_t)((int64_t)p_time2->tv_nsec - (int64_t)p_time1->tv_nsec);
            *p_time_difference = (result_sec * OSAL_SECTONSEC) + result_nsec;
        }
        else
        {
            if ((p_time2->tv_sec == p_time1->tv_sec) &&
                (p_time2->tv_nsec >= p_time1->tv_nsec))
            {
                OSAL_DEBUG("p_time2->tv_sec == p_time1->tv_sec\n");
                *p_time_difference = (int64_t)((int64_t)p_time2->tv_nsec - (int64_t)p_time1->tv_nsec);
            }
            else
            {
                /* There are two types of this case */
                /* Case 1: 0 crossing of the clock */
                /* Case 2: Round is almost when the clock goes around, may be past time */
                result_sec = (int64_t)(OSAL_MAX_SEC - (p_time1->tv_sec - p_time2->tv_sec));

                if(p_time2->tv_nsec > p_time1->tv_nsec)
                {
                    OSAL_DEBUG("p_time2->tv_nsec > p_time1->tv_nsec else\n");
                    result_nsec = (int64_t)p_time2->tv_nsec - (int64_t)p_time1->tv_nsec;
                }
                else
                {
                    OSAL_DEBUG("p_time2->tv_nsec < p_time1->tv_nsec\n");
                    result_sec = result_sec - 1;
                    result_nsec = (OSAL_SECTONSEC - ((int64_t)p_time1->tv_nsec - (int64_t)p_time2->tv_nsec));
                }
                *p_time_difference = (result_sec * OSAL_SECTONSEC) + result_nsec;
            }
        }
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_CalcDiffTime()
***********************************************************************************************************************/

void R_OSAL_Internal_MillisecToTick(osal_milli_sec_t time_period, TickType *tick)
{
    if (NULL != tick) {
        /* Each timer tick is 1ms */
        *tick = (TickType)time_period;
    }
}

void R_OSAL_Internal_TickToMillisec(osal_milli_sec_t *time_period, TickType tick)
{
    if (NULL != time_period) {
        /* Each timer tick is 1ms */
        *time_period = (osal_milli_sec_t) tick;
    }
}


/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MsSleep()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MsSleep(osal_milli_sec_t milliseconds)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret      = OSAL_RETURN_OK;
    TickType sleep_tick = 0;
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();
    StatusType ret;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    R_OSAL_Internal_MillisecToTick(milliseconds, &sleep_tick);
    ret = SetRelAlarm(*(thread->AutosarTask_SleepAlarm), sleep_tick, 0);
    if (ret != E_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s: SetRelAlarm() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
        return OSAL_RETURN_FAIL;
    }

    ret = WaitEvent(R_AUTORSAR_ALARM_EVENT);
    if (ret != E_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s: WaitEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
        return OSAL_RETURN_FAIL;
    }

    ret = ClearEvent(R_AUTORSAR_ALARM_EVENT);
    if (ret != E_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s: ClearEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
        return OSAL_RETURN_FAIL;
    }
    
    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MsSleep()
***********************************************************************************************************************/
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MsSleep()
***********************************************************************************************************************/
st_osal_inner_thread_attrinfo_t g_osal_inner_thread_attrinfo;
/***********************************************************************************************************************
* Start of function R_OSAL_Internal_SetThreadAttr()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_SetThreadAttr(const st_osal_inner_thread_attrinfo_t *const p_attr_info,
                                                     osal_inner_thread_attr_t *const p_attr)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret      = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    (void)p_attr;
    g_osal_inner_thread_attrinfo.policy = p_attr_info->policy;
    g_osal_inner_thread_attrinfo.priority = p_attr_info->priority;
    g_osal_inner_thread_attrinfo.stack_size = p_attr_info->stack_size;

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_SetThreadAttr()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_ThreadCreate()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_ThreadCreate(const st_osal_inner_thread_config_t *const p_config,
                                                    osal_inner_thread_handle_t *const p_handle)
{
    int nr_threads = R_OSAL_Sys_GetNrThreads();
    osal_autosar_thread_t *thread;
    int i;
    StatusType ret;

    if ((NULL == p_config) || (NULL == p_handle)) {
        OSAL_DEBUG_ERROR("Thread Open: OSAL_RETURN_PAR\n");
        return OSAL_RETURN_PAR;
    }

    *p_handle = NULL;

    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        /* Has the task already been assigned? */
        if (thread->Task != NULL) {
            continue;
        }

        if (OSAL_THREAD_PRIORITY_HIGHEST < thread->Priority) {
            continue;
        }

        thread->Task = (void *)p_config->func;
        thread->Arg = p_config->userarg;
        thread->Priority = g_osal_inner_thread_attrinfo.priority;
        strncpy(thread->Name, "PMA Task", R_MAX_TASK_NAME_LEN);
        if (g_OsalSchedulerStarted) {
            ret = ActivateTask(*(thread->AutosarTask_id));
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: ActivateTask() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
                return OSAL_RETURN_FAIL;
            }
        }

        *p_handle = (osal_inner_thread_handle_t)thread;
        return OSAL_RETURN_OK;
    }

    OSAL_DEBUG_ERROR("ERROR: %s: No available tasks (%s, pri %d)!\n", __func__, thread->Name, thread->Priority);
    return OSAL_RETURN_FAIL;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_ThreadCreate()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_ThreadTimedJoin()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_ThreadTimedJoin(osal_inner_thread_handle_t const handle,
                                                       int32_t *const p_return_value,
                                                       const osal_milli_sec_t time_period)
{
    osal_autosar_thread_t *thread = (osal_autosar_thread_t *)handle;
    osal_autosar_thread_t *thisthread = R_OSAL_Sys_GetCurrentThreadData();
    TaskStateType task_state;
    StatusType ret;
    TickType sleep_tick = 0;

    GetTaskState(*thread->AutosarTask_id, &task_state);

    /* Only wait if the task has not already run and terminated */
    if (task_state != SUSPENDED) {
        R_OSAL_Internal_MillisecToTick(time_period, &sleep_tick);
        ret = SetRelAlarm(*(thread->AutosarTask_SleepAlarm), sleep_tick, 0);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: SetRelAlarm() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        thisthread->WaitingItem = (int *)thread;

        ret = WaitEvent(osal_event);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: WaitEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        ret = ClearEvent(osal_event);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: ClearEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        thisthread->WaitingItem = NULL;
    }

    if (p_return_value) {
        *p_return_value = (intptr_t)thread->Ret;
    }

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_ThreadTimedJoin()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_SetMutexAttr()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_SetMutexAttr(osal_inner_mutexattr_t *const p_mutex_attr)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret      = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    (void)p_mutex_attr;

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_SetMutexAttr()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MutexInit()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MutexInit(const osal_inner_mutexattr_t *const p_mutex_attr,
                                                 osal_inner_mutex_handle_t *const p_handle)
{
    st_internal_mutex_info_t *mtx_info;

    /* Protected access */
    GetResource(osal_mutex);

    mtx_info = loc_FindUnusedMutex();
    if (mtx_info != NULL) {
        mtx_info->id = (int)p_handle;
        base_mutex_create(&mtx_info->mtx);
    }

    ReleaseResource(osal_mutex);
    /* end protected access */

    if (mtx_info == NULL) {
        *p_handle = NULL;
        return OSAL_RETURN_FAIL;
    }

    *p_handle = (osal_inner_mutex_handle_t)&mtx_info->mtx;

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MutexInit()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MutexTimedLock()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MutexTimedLock(osal_inner_mutex_handle_t handle,
                                                      osal_milli_sec_t time_period)
{
    base_mutex_t *mtx = (base_mutex_t *)handle;

    if (0 > time_period) {
        OSAL_DEBUG_ERROR("%s: time must be >0\n", __func__);
        return OSAL_RETURN_PAR;
    }

    GetResource(osal_mutex);
    base_mutex_lock_timeout(mtx, time_period);
    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MutexTimedLock()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MutexUnlock()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MutexUnlock(osal_inner_mutex_handle_t handle)
{
    base_mutex_t *mtx = (base_mutex_t *)handle;

    if (NULL == handle) {
        OSAL_DEBUG_ERROR("%s: Null handle\n", __func__);
        return OSAL_RETURN_HANDLE;
    }
    
    GetResource(osal_mutex);
    base_mutex_unlock(mtx);
    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MutexDestroy()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MutexDestroy(osal_inner_mutex_handle_t handle)
{
    base_mutex_t *mtx = (base_mutex_t *)handle;
    st_internal_mutex_info_t *mtx_info;

    if (NULL == handle) {
        OSAL_DEBUG_ERROR("%s: Null handle\n", __func__);
        return OSAL_RETURN_HANDLE;
    }

    /* Protected access */
    GetResource(osal_mutex);

    mtx_info = loc_FindMutex(mtx);
    if (mtx_info != NULL) {
        mtx_info->id = OSAL_MUTEX_INVALID_ID;
        base_mutex_destroy(mtx);
    }
    
    ReleaseResource(osal_mutex);
    /* end protected access */

    if (mtx_info == NULL) {
        return OSAL_RETURN_FAIL;
    }

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MutexDestroy()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MqCreate()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MqCreate(const st_osal_inner_mq_config_t *const p_config,
                                                osal_inner_mq_handle_t *const p_handle)
{
    R_OS_Internal_MqData_t *mq;
    e_osal_return_t x;
    int max_num_msg;
    int i;

    (void)p_config;

    GetResource(osal_mutex);

    mq = loc_FindMqById((int)p_handle);
    if (mq) {
        mq->Open++;
    } else {
        mq = loc_AllocMqData();
        if (mq == NULL) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            return OSAL_RETURN_FAIL;
        }

        x = base_mutex_create(&mq->Lock);
        if (x != 0) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            return OSAL_RETURN_FAIL;
        }

        x = base_sema_create(&mq->Signal);
        if (x != 0) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            return OSAL_RETURN_FAIL;
        }

        x = base_sema_create(&mq->Space);
        max_num_msg = (int)p_config->max_num_msg;
        for (i = 0; i < max_num_msg; i++) {
            x |= base_sema_give(&mq->Space);
        }

        mq->Id            = (int)p_handle;
        mq->Open          = 1;
        mq->Buffer        = malloc(p_config->max_num_msg * p_config->msg_size);
        mq->Start         = mq->Buffer;
        mq->End           = mq->Start;
        mq->ValidByte     = 0;
        mq->p_config.max_num_msg = p_config->max_num_msg;
        mq->p_config.msg_size = p_config->msg_size;
    }

    ReleaseResource(osal_mutex);

    *p_handle = (osal_inner_mq_handle_t)mq;

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MqCreate()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MqTimedSend()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MqTimedSend(osal_inner_mq_handle_t handle,
                                                   osal_milli_sec_t time_period,
                                                   const void * p_buffer,
                                                   size_t buffer_size)
{
    R_OS_Internal_MqData_t *mq = (R_OS_Internal_MqData_t *)handle;
    int            x;
    size_t         n;
    char          *eob;
    const char    *MsgPtr = p_buffer;

    if (mq == NULL) {
        return OSAL_RETURN_HANDLE;
    }

    GetResource(osal_mutex);

    /* Wait until there is space in the queue */
    x = base_sema_take_timeout(&mq->Space, time_period);
    if (x != OSAL_RETURN_OK) {
        if (x == OSAL_RETURN_TIME) {
            OSAL_DEBUG_WARNING("WARNING: %s mq=%p timeout\n", __func__, mq);
        } else {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        }
        ReleaseResource(osal_mutex);
        return x;
    }

    /* --- Protect access to the queue --- */
    x = base_mutex_lock_timeout(&mq->Lock, time_period);
    if (x != OSAL_RETURN_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return x;
    }

    /* send message */
    n = 0;
    eob = mq->Buffer + (mq->p_config.msg_size * mq->p_config.max_num_msg);

    while ((n < mq->p_config.msg_size) && (n < buffer_size)) {
        *mq->End = *MsgPtr;
        mq->ValidByte++;
        MsgPtr++;
        mq->End++;
        n++;
        if (mq->End >= eob) {
            mq->End = mq->Buffer;
        }
    }

    base_mutex_unlock(&mq->Lock);

    /* Signal waiting threads that there is a message in the queue */
    base_sema_give(&mq->Signal);

    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MqTimedSend()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MqTimedReceive()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MqTimedReceive(osal_inner_mq_handle_t handle,
                                                      osal_milli_sec_t time_period,
                                                      void * p_buffer,
                                                      size_t buffer_size)
{
    R_OS_Internal_MqData_t *mq = (R_OS_Internal_MqData_t *)handle;
    int            x;
    size_t         n;
    char          *eob;
    char          *MsgPtr = p_buffer;

    if (mq == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        return OSAL_RETURN_HANDLE;
    }

    GetResource(osal_mutex);

    /* Waiting until there is a message in the queue */
    x = base_sema_take_timeout(&mq->Signal, time_period);
    if (x != OSAL_RETURN_OK) {
        if (x != OSAL_RETURN_TIME) {
            OSAL_DEBUG("DEBUG: %s:%d\n", __func__, __LINE__);
        }
        ReleaseResource(osal_mutex);
        return x;
    }

    /* --- Protect access to the queue --- */
    x = base_mutex_lock_timeout(&mq->Lock, time_period);
    if (x != OSAL_RETURN_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return x;
    }

    if (mq->ValidByte < buffer_size) {
        /* We're trying to read more data than was sent */
        OSAL_DEBUG_ERROR("ERROR: %s mq=%p, ValidByte %d, want %d\n", __func__, mq, mq->ValidByte, buffer_size);
        n = OSAL_RETURN_PAR;
    } else {
        /* read message */
        n = 0;
        eob = mq->Buffer + (mq->p_config.msg_size * mq->p_config.max_num_msg);

        while ((n < mq->p_config.msg_size) && (n < buffer_size)) {
            *MsgPtr = *mq->Start;
            mq->ValidByte--;
            MsgPtr++;
            mq->Start++;
            n++;
            if (mq->Start >= eob) {
                mq->Start = mq->Buffer;
            }
        }
    }
    OSAL_DEBUG("\n");

    base_mutex_unlock(&mq->Lock);
    /* --- End protect access to the queue --- */

    /* Signal waiting threads that there is space in the queue */
    base_sema_give(&mq->Space);
    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MqTimedReceive()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_MqDelete()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_MqDelete(osal_inner_mq_handle_t handle)
{
    // TODO
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_MqDelete()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_GetHighResoTimeStamp()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_GetHighResoTimeStamp(st_osal_time_t *const p_time_stamp)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret      = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    osal_ret = R_OSAL_Internal_GetTimestamp(p_time_stamp);

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_GetHighResoTimeStamp()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_Internal_CalcTimeDifference()
***********************************************************************************************************************/
/* PRQA S 9106 1 # This function is intended to be used in common. */
e_osal_return_t R_OSAL_Internal_CalcTimeDifference(const st_osal_time_t *const p_time2, 
                                                          const st_osal_time_t *const p_time1,
                                                          osal_nano_sec_t *const p_time_difference)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret      = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    osal_ret = R_OSAL_Internal_CalcDiffTime(p_time2, p_time1, p_time_difference);

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_Internal_CalcTimeDifference()
***********************************************************************************************************************/


/***********************************************************************************************************************
* Debug functions
***********************************************************************************************************************/
#include <stdarg.h>
#define LOC_LOG_SIZE (32 * 1024)
static char loc_log[LOC_LOG_SIZE];
static char *loc_log_next = loc_log;
static char *loc_log_last = loc_log + LOC_LOG_SIZE;

void dbg_show_tasks(void)
{
    int nr_threads = R_OSAL_Sys_GetNrThreads();
    osal_autosar_thread_t *thread;
    int i;

    thread = R_OSAL_Sys_GetCurrentThreadData();
    if (thread != NULL) {
        printf_raw("CURRENT TASK:%s\n", thread->Name);
    }

    printf_raw("TASK LIST:\n");

    /* Look for a task waiting on this event */
    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        if (thread->Task != NULL) {
            if (thread->WaitingItem != NULL) {
                printf_raw(" %s waiting on %p (count %d)\n", thread->Name, thread->WaitingItem, *thread->WaitingItem);
            } else {
                printf_raw(" %s running\n", thread->Name);
            }
        }
    }
    printf_raw("\n\n");
}

void log_flush(void)
{
    printf_raw("%s", loc_log);
    loc_log_next = loc_log;
    loc_log_last = loc_log + LOC_LOG_SIZE;

    dbg_show_tasks();
}

/* Send the info to a buffer. Once full, dump the output of the buffer and start over */
void log_printf(const char* format, ...)
{
    va_list args;
    size_t max_len = loc_log_last - loc_log_next;

    if (max_len == 0) {
        log_flush();
    }

    va_start(args, format);
    vsnprintf(loc_log_next, max_len, format, args);
    va_end(args);

    while (*loc_log_next != '\0') {
        loc_log_next++;
    }
}
