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
* Description :  OSAL basic mutex functions for Trampoline (AUTOSAR)
***********************************************************************************************************************/

#include "tpl_os.h"
#include "tpl_com_definitions.h" /*for E_COM_ID*/

#include "r_osal_types.h"
#include "r_osal_base.h"
#include "target/trampoline/r_osal_common.h"

#include "autosar_osal.h" /* app defines osal_autosar_thread_t structure */

static const char *loc_autosar_error_str[31] = {
	[E_OK]                       = "Ok",
	[E_OS_ACCESS]                = "Calling task is not an extended task",
	[E_OS_CALLEVEL]              = "Call at interrupt level",
	[E_OS_ID]                    = "Invalid Id",
	[E_OS_LIMIT]                 = "Too many task activations",
	[E_OS_NOFUNC]                = "Unused Id",
	[E_OS_RESOURCE]              = "Calling task occupies resource",
	[E_OS_STATE]                 = "Wrong state",
	[E_OS_VALUE]                 = "Wrong value",
	[E_NOT_OK]                   = "not ok",
	[E_COM_ID]                   = "Invalid invalid",
	[E_COM_LENGTH]               = "Invalid length",
	[E_COM_LIMIT]                = "FIFO full",
	[E_COM_NOMSG]                = "FIFO empty",
};

const char *R_OSAL_Sys_GetTplErrorStr(int errno)
{
    if (errno >= E_COM_NOMSG) {
        return NULL;
    }
    return (loc_autosar_error_str[errno]);
}

osal_autosar_thread_t *R_OSAL_Sys_GetCurrentThreadData(void)
{
    int nr_threads = R_OSAL_Sys_GetNrThreads();
    osal_autosar_thread_t *thread;
    int i;
    TaskType Task_Id;
    StatusType ret;

    ret = GetTaskID(&Task_Id);
    if (ret != E_OK) {
        return NULL;
    }

    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        if (*(thread->AutosarTask_id) == Task_Id) {
            return thread;
        }
    }

    return NULL;
}


#ifdef PRINT_CPU_IDLE
osal_nano_sec_t task_time[64] = {0};
osal_nano_sec_t task_time_total_ns = 0;
uint32_t cpu_usage;
st_osal_time_t start_ts;
#endif

/* Called by Trampoline if PRETASKHOOK = TRUE */
void PreTaskHook(void)
{
#ifdef PRINT_CPU_IDLE
    TaskType task_id;
    R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &start_ts);
#endif
}

/* Called by Trampoline if POSTTASKHOOK = TRUE */
void PostTaskHook(void)
{
#ifdef PRINT_CPU_IDLE
    st_osal_time_t end_ts, dummy_ts = {0};
    osal_nano_sec_t time_ns, total_time_ns;
    TaskType task_id;
    (void)GetTaskID(&task_id);
    if (task_id >= 64 || task_id < 0) {
        return;
    }
    R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &end_ts);
    R_OSAL_Internal_CalcDiffTime(&end_ts, &start_ts, &time_ns);
    R_OSAL_Internal_CalcDiffTime(&end_ts, &dummy_ts, &total_time_ns);
    task_time[task_id] += time_ns;
    task_time_total_ns += time_ns;
    cpu_usage = (task_time_total_ns * 1000) / total_time_ns;

    static int counter = 0;
    counter++;

    if ( (counter % 50000) == 0)
    {
        printf("CPU Idle: %d.%d%%\n", (1000 - cpu_usage) / 10, 10 - (cpu_usage % 10));
    }
#endif
}

/* This is called when a Mutex or semaphore is given.
 * WARNING: Must be called under GetResource(osal_mutex);
 * It looks for the first task, in order of priority, that is waiting on a mutex
 * or semaphore.
 * If one is found, it will call SetEvent() to release it. It is important to
 * note that another task may run before the task that is sent an event. This other
 * task may get the mutex or semaphore, meaning the task receiving the event
 * needs to check that it really can get it.
 */
int R_OSAL_Sys_KickTask(int * Mutex)
{
    int nr_threads = R_OSAL_Sys_GetNrThreads();
    osal_autosar_thread_t *thread;
    StatusType ret;
    int i;
    uint8_t this_task_priority;

    thread = R_OSAL_Sys_GetCurrentThreadData();
    if (thread == NULL) {
        /* If in an interrupt task, we don't want to switch tasks */
        this_task_priority = 255;
    } else {
        this_task_priority = thread->Priority;
    }

    /* Look for a task waiting on this event */
    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        /* Only poke the waiting task if the mutex/semaphore is available */
        if (thread->WaitingItem != NULL && *thread->WaitingItem != 0 && thread->WaitingItem == Mutex) {
            OSAL_DEBUG("%s:%s\n", __func__, thread->Name);
            ret = SetEvent(*thread->AutosarTask_id, osal_event);
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: SetEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            }
            if (thread->Priority >= this_task_priority) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

int R_OSAL_Sys_KickTaskById(osal_autosar_thread_t *task_to_kick)
{
    int nr_threads = R_OSAL_Sys_GetNrThreads();
    osal_autosar_thread_t *thread;
    StatusType ret;
    int i;
    uint8_t this_task_priority;

    thread = R_OSAL_Sys_GetCurrentThreadData();
    if (thread == NULL) {
        /* If in an interrupt task, we don't want to switch tasks */
        this_task_priority = 255;
    } else {
        this_task_priority = thread->Priority;
    }

    /* Look for a task waiting on this event */
    for (i = 0; i < nr_threads; i++) {
        thread = R_OSAL_Sys_GetThread(i);

        /* Only poke the waiting task if the mutex/semaphore is available */
        if (task_to_kick == thread && thread->WaitingItem == thread) {
            thread->WaitingItem = NULL;
            OSAL_DEBUG("%s:%s\n", __func__, thread->Name);
            ret = SetEvent(*thread->AutosarTask_id, osal_event);
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: SetEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            }
            if (thread->Priority >= this_task_priority) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

/* This function runs a thread inside an Autosar task */
void R_OSAL_Sys_TaskRunner(void)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();

    if (thread == NULL || thread->Task == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s: Bad thread\n", __func__);
        return;
    }

    OSAL_DEBUG("===> %s: Starting task %s\n", __func__, thread->Name);
    thread->Ret = thread->Task(thread->Arg);

    R_OSAL_Sys_KickTask((int*)thread);

    thread->Task = NULL;
    TerminateTask();
}

static int base_mutex_sem_take(int *Mutex, int is_mtx)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();
    int need_to_wait = 0;
    StatusType ret;
    int err = OSAL_RETURN_OK;

    if (thread == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s: no task!\n", __func__);
        return OSAL_RETURN_FAIL;
    }

    if (*Mutex == 0) {
        thread->WaitingItem = (void *)Mutex;
        need_to_wait = 1;
    } else {
        if (is_mtx) {
            *Mutex = 0;
        } else { /* semaphore */
            *Mutex = *Mutex - 1;
        }
    }

    /* If mutex has already been given, we don't need to wait */
    while (need_to_wait) {
        ReleaseResource(osal_mutex);
        ret = WaitEvent(osal_event);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: WaitEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        /* Protected access to mutex */
        GetResource(osal_mutex);

        if (*Mutex != 0) {
            thread->WaitingItem = NULL;
            if (is_mtx) {
                *Mutex = 0;
            } else { /* semaphore */
                *Mutex = *Mutex - 1;
            }
            need_to_wait = 0;
        }

        ret = ClearEvent(osal_event);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: ClearEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            err = OSAL_RETURN_FAIL;
        }
    }

    return err;
}

static int base_mutex_sem_take_timeout(int *Mutex, unsigned int Timeout_ms, int is_mtx)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();
    int need_to_wait = 0;
    StatusType ret;
    int err = OSAL_RETURN_OK;
    TaskType Task_Id;
    EventMaskType Event;
    TickType ticks;

    /* Special handling for interrupt tasks - we don't sleep */
    if (thread == NULL) {
        if (*Mutex == 0) {
            OSAL_DEBUG_ERROR("ERROR: %s: mtx/sem is 0!\n", __func__);
            return OSAL_RETURN_FAIL;
        } else {
            if (is_mtx) {
                *Mutex = 0;
            } else { /* semaphore */
                *Mutex = *Mutex - 1;
            }
        }
        return OSAL_RETURN_OK;
    }

    ret = GetTaskID(&Task_Id);

    if (*Mutex == 0) {
        thread->WaitingItem = (void *)Mutex;
        need_to_wait = 1;
    } else {
        if (is_mtx) {
            *Mutex = 0;
        } else { /* semaphore */
            *Mutex = *Mutex - 1;
        }
    }

    /* If mutex has already been given, we don't need to wait */
    while (need_to_wait) {
        /* Set alarm for timeout */
        if (thread->AutosarTask_SleepAlarm == NULL) {
            OSAL_DEBUG_ERROR("ERROR: %s: no alarm for task %s\n", __func__, thread->Name);
            return OSAL_RETURN_FAIL;
        }

        R_OSAL_Internal_MillisecToTick(Timeout_ms, &ticks);
        ReleaseResource(osal_mutex);
        ret = SetRelAlarm(*(thread->AutosarTask_SleepAlarm), ticks, 0);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: SetRelAlarm() %d (%s), ticks %lu\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret), ticks);
            return OSAL_RETURN_FAIL;
        }

        ret = WaitEvent(osal_event | R_AUTORSAR_ALARM_EVENT);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: WaitEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        GetResource(osal_mutex);

        /* It is possible that the alarm has already expired, so don't throw an error */
        CancelAlarm(*(thread->AutosarTask_SleepAlarm));

        GetEvent(Task_Id, &Event);
        if (Event & R_AUTORSAR_ALARM_EVENT) {
            //printf_raw("WARNING: %s() timeout! task %s\n", __func__, thread->Name);
            need_to_wait = 0;

            ret = ClearEvent(R_AUTORSAR_ALARM_EVENT);
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: ClearEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
                err = OSAL_RETURN_FAIL;
            }

            err = OSAL_RETURN_TIME;
        } else {
            if (*Mutex != 0) {
                thread->WaitingItem = NULL;
                if (is_mtx) {
                    *Mutex = 0;
                } else { /* semaphore */
                    *Mutex = *Mutex - 1;
                }
                need_to_wait = 0;
            }

            ret = ClearEvent(osal_event);
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: ClearEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
                err = OSAL_RETURN_FAIL;
            }
        }
    }

    return err;
}

static int base_mutex_sem_give(int *Mutex, int is_mtx)
{
    if (is_mtx) {
        *Mutex = 1;
    } else { /* semaphore */
        *Mutex = *Mutex + 1;
    }
    
    R_OSAL_Sys_KickTask(Mutex);

    return OSAL_RETURN_OK;
}

static int base_mutex_sema_get_count(int *Mutex)
{
    int count;
    count = *Mutex;
    return count;
}

int base_mutex_create(base_mutex_t *Mutex)
{
    *Mutex = 1;
    return OSAL_RETURN_OK;
}

int base_mutex_lock(base_mutex_t *Mutex)
{
    return base_mutex_sem_take(Mutex, 1);
}

int base_mutex_lock_timeout(base_mutex_t *Mutex, unsigned int Timeout_ms)
{
    return base_mutex_sem_take_timeout(Mutex, Timeout_ms, 1);
}

int base_mutex_unlock(base_mutex_t *Mutex)
{
    return base_mutex_sem_give(Mutex, 1);
}

int base_mutex_destroy(base_mutex_t *Mutex)
{
    (void)Mutex;
    return OSAL_RETURN_OK;
}

int base_mutex_is_unlocked(base_mutex_t *Mutex)
{
    return base_mutex_sema_get_count(Mutex);
}


int base_sema_create(base_sema_t *sema)
{
    *sema = 0;

    return OSAL_RETURN_OK;
}

int base_sema_take(base_sema_t *sema)
{
    return base_mutex_sem_take(sema, 0);
}

int base_sema_take_timeout(base_sema_t *sema, unsigned int Timeout_ms)
{
    return base_mutex_sem_take_timeout(sema, Timeout_ms, 0);
}

int base_sema_give(base_sema_t *sema)
{
    return base_mutex_sem_give(sema, 0);
}

int base_sema_destroy(base_sema_t *sema)
{
    (void)sema;
    return OSAL_RETURN_OK;
}

int base_sema_get_count(base_sema_t *sema)
{
    return base_mutex_sema_get_count(sema);
}
