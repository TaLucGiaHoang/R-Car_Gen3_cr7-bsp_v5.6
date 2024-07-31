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
* OSAL wrapper Thread Manager for Trampoline (AUTOSAR)
***********************************************************************************************************************/

#include <string.h>

#include "target/trampoline/r_osal_common.h"

#include "autosar_osal.h" /* app defines osal_autosar_thread_t structure */

extern int g_OsalSchedulerStarted;

#define LOC_TRAMPOLINE_THREAD_STACK_SIZE    (24 * 1024)

e_osal_return_t R_OSAL_ThreadCreate(const st_osal_thread_config_t *const p_config, osal_thread_id_t thread_id,
                                    osal_thread_handle_t *const p_handle)
{
    int nr_threads = R_OSAL_Sys_GetNrThreads();
    osal_autosar_thread_t *thread;
    int i;
    StatusType ret;

    /* Ignore thread_id as we use a pool of tasks */
    (void)thread_id;

    if ((NULL == p_config) || (NULL == p_handle)) {
        OSAL_DEBUG_ERROR("Thread Open: OSAL_RETURN_PAR\n");
        return OSAL_RETURN_PAR;
    }

    *p_handle = OSAL_THREAD_HANDLE_INVALID;

    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        /* Has the task already been assigned? */
        if (thread->Task != NULL) {
            continue;
        }

        if (p_config->priority < thread->Priority) {
            continue;
        }

        thread->Task = p_config->func;
        thread->Arg = p_config->userarg;
        if (p_config->task_name != NULL) {
            strncpy(thread->Name, p_config->task_name, R_MAX_TASK_NAME_LEN);
        } else {
            strncpy(thread->Name, "NoName", R_MAX_TASK_NAME_LEN);
        }
        if (g_OsalSchedulerStarted) {
            ret = ActivateTask(*(thread->AutosarTask_id));
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: ActivateTask() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
                return OSAL_RETURN_FAIL;
            }
        }

        if (p_config->task_name != NULL) {
            OSAL_DEBUG("%s '%s' (%p)\n", __func__, p_config->task_name, thread);
        } else {
            OSAL_DEBUG("%s no name (%p)\n", __func__, thread);
        }

        *p_handle = (osal_thread_handle_t)thread;
        return OSAL_RETURN_OK;
    }

    OSAL_DEBUG_ERROR("ERROR: %s: No available tasks (%s, pri %d)!\n", __func__, thread->Name, thread->Priority);
    return OSAL_RETURN_FAIL;
}

e_osal_return_t R_OSAL_ThreadSleepUntilTimeStamp(const st_osal_time_t * const p_time_stamp)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();
    TickType ticks;
    StatusType ret;
    st_osal_time_t current_time_stamp;
    e_osal_return_t osal_ret;
    osal_nano_sec_t timeout_ns;
    osal_milli_sec_t time_period;

    if (!thread) {
        OSAL_DEBUG_ERROR("ERROR: %s: thread not found!\n", __func__);
        return OSAL_RETURN_FAIL;
    }

    if (thread->AutosarTask_SleepAlarm == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s: '%s' thread has no sleep alarm!\n", __func__, thread->Name);
        return OSAL_RETURN_FAIL;
    }

    osal_ret = R_OSAL_Internal_GetTimestamp(&current_time_stamp);
    if (OSAL_RETURN_OK == osal_ret) {
        osal_ret = R_OSAL_Internal_GetDiffTime(p_time_stamp, &current_time_stamp, &timeout_ns);
    }

    time_period = timeout_ns / 1000000;

    R_OSAL_Internal_MillisecToTick(time_period, &ticks);

    ret = SetRelAlarm(*(thread->AutosarTask_SleepAlarm), ticks, 0);
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

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThreadSleepForTimePeriod(osal_milli_sec_t time_period)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();
    TickType ticks;
    StatusType ret;

    if (!thread) {
        OSAL_DEBUG_ERROR("ERROR: %s: thread not found!\n", __func__);
        return OSAL_RETURN_FAIL;
    }

    if (thread->AutosarTask_SleepAlarm == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s: '%s' thread has no sleep alarm!\n", __func__, thread->Name);
        return OSAL_RETURN_FAIL;
    }

    R_OSAL_Internal_MillisecToTick(time_period, &ticks);

    ret = SetRelAlarm(*(thread->AutosarTask_SleepAlarm), ticks, 0);
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

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThreadSelf(osal_thread_handle_t* const p_handle)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();

    if (!thread) {
        OSAL_DEBUG_ERROR("ERROR: %s: thread not found!\n", __func__);
        return OSAL_RETURN_FAIL;
    }

    *p_handle = (osal_thread_handle_t)thread;

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThreadEqual(osal_thread_handle_t handle1, osal_thread_handle_t handle2, bool *const p_result)
{
    if (handle1 != handle2) {
        *p_result = false;
    } else {
        *p_result = true;
    }
    
    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThreadJoin(osal_thread_handle_t handle, int64_t *const p_return_value)
{
    osal_autosar_thread_t *thread = (osal_autosar_thread_t *)handle;
    osal_autosar_thread_t *thisthread = R_OSAL_Sys_GetCurrentThreadData();
    TaskStateType task_state;
    StatusType ret;

    GetTaskState(*thread->AutosarTask_id, &task_state);

    /* Only wait if the task has not already run and terminated */
    if (task_state != SUSPENDED) {
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

e_osal_return_t R_OSAL_ThreadInitializeThreadConfigSt(st_osal_thread_config_t* const p_config)
{
    if (NULL == p_config) {
        return OSAL_RETURN_PAR;
    }

    // TODO
    p_config->func          = NULL;
    p_config->userarg       = NULL;
    p_config->priority      = 0;
    p_config->task_name     = "osal task";
    p_config->stack_size    = LOC_TRAMPOLINE_THREAD_STACK_SIZE;

    return OSAL_RETURN_OK;
}
