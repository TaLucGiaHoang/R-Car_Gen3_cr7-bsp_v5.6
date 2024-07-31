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
* Description :  OSAL wrapper Threadsync Manager for Trampoline (AUTOSAR)
***********************************************************************************************************************/

#include "target/trampoline/r_osal_common.h"
#include "r_osal_base.h"

//#define OSAL_DEBUG_THSYNC(...)
#define OSAL_DEBUG_THSYNC OSAL_DEBUG

typedef struct st_mutex_info
{
    osal_mutex_id_t id;
    base_mutex_t mtx;
} st_mutex_info_t;

static st_mutex_info_t loc_Mutexes[OSAL_RCFG_MUTEX_MAX_NUM];


typedef struct st_condvar_info
{
    osal_cond_id_t id;
    unsigned int sema;
    unsigned int nr_waiting_threads;
} st_condvar_info_t;

static st_condvar_info_t loc_CondVars[OSAL_RCFG_COND_MAX_NUM];


static st_mutex_info_t *loc_FindMutex(base_mutex_t *mutex)
{
    int i;

    for (i = 0; i < OSAL_RCFG_MUTEX_MAX_NUM; i++) {
        if (mutex == &loc_Mutexes[i].mtx) {
            return &loc_Mutexes[i];
        }
    }

    return NULL;
}

static st_mutex_info_t *loc_FindUnusedMutex(void)
{
    int i;

    for (i = 0; i < OSAL_RCFG_MUTEX_MAX_NUM; i++) {
        if (loc_Mutexes[i].id == OSAL_MUTEX_INVALID_ID) {
            return &loc_Mutexes[i];
        }
    }

    return NULL;
}

static st_condvar_info_t *loc_FindUnusedCondvar(void)
{
    int i;

    for (i = 0; i < OSAL_RCFG_COND_MAX_NUM; i++) {
        if (loc_CondVars[i].id == OSAL_COND_INVALID_ID) {
            return &loc_CondVars[i];
        }
    }

    return NULL;
}


e_osal_return_t osal_mutex_initialize(void)
{
    int i;

    for (i = 0; i < OSAL_RCFG_MUTEX_MAX_NUM; i++) {
        loc_Mutexes[i].id = OSAL_MUTEX_INVALID_ID;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThsyncMutexCreate(osal_mutex_id_t mutex_id, osal_mutex_handle_t *const p_handle)
{
    st_mutex_info_t *mtx_info;

    if (NULL == p_handle || 0 == mutex_id) {
        OSAL_DEBUG_ERROR("%s: Bad handle or ID\n", __func__);
        return OSAL_RETURN_PAR;
    }

    /* Protected access */
    GetResource(osal_mutex);

    mtx_info = loc_FindUnusedMutex();
    if (mtx_info != NULL) {
        mtx_info->id = mutex_id;
        base_mutex_create(&mtx_info->mtx);
    }

    ReleaseResource(osal_mutex);
    /* end protected access */

    if (mtx_info == NULL) {
        *p_handle = NULL;
        return OSAL_RETURN_FAIL;
    }

    *p_handle = (osal_mutex_handle_t)&mtx_info->mtx;

    OSAL_DEBUG_THSYNC("%s Id %lx (%p)\n", __func__, (uint32_t)mutex_id, *p_handle);

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThsyncMutexDestroy(osal_mutex_handle_t handle)
{
    base_mutex_t *mtx = (base_mutex_t *)handle;
    st_mutex_info_t *mtx_info;

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

e_osal_return_t R_OSAL_ThsyncMutexLockUntilTimeStamp(osal_mutex_handle_t handle,
                                                     const st_osal_time_t *const p_time_stamp)
{
    st_osal_time_t current_time_stamp;
    e_osal_return_t osal_ret;
    osal_nano_sec_t timeout_ns;
    osal_milli_sec_t time_period;
    int ret = 0;

    osal_ret = R_OSAL_Internal_GetTimestamp(&current_time_stamp);
    if (OSAL_RETURN_OK == osal_ret) {
        osal_ret = R_OSAL_Internal_GetDiffTime(p_time_stamp, &current_time_stamp, &timeout_ns);
    }

    time_period = timeout_ns / 1000000;

    base_mutex_t *mtx = (base_mutex_t *)handle;

    if (0 > time_period) {
        OSAL_DEBUG_ERROR("%s: time must be >0\n", __func__);
        return OSAL_RETURN_PAR;
    }

    GetResource(osal_mutex);
    ret = base_mutex_lock_timeout(mtx, time_period);
    ReleaseResource(osal_mutex);

    return ret;
}

e_osal_return_t R_OSAL_ThsyncMutexLockForTimePeriod(osal_mutex_handle_t handle, osal_milli_sec_t time_period)
{
    base_mutex_t *mtx = (base_mutex_t *)handle;
    int ret = 0;

    if (0 > time_period) {
        OSAL_DEBUG_ERROR("%s: time must be >0\n", __func__);
        return OSAL_RETURN_PAR;
    }

    GetResource(osal_mutex);
    ret = base_mutex_lock_timeout(mtx, time_period);
    ReleaseResource(osal_mutex);
    return ret;
}

#if 1
e_osal_return_t R_OSAL_ThsyncMutexTryLock(osal_mutex_handle_t handle)
{
    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret;
 
    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    OSAL_DEBUG("Start (%p)\n", (void *)handle);

    GetResource(osal_mutex);

    osal_ret = base_mutex_lock_timeout(handle, 0);
    if (OSAL_RETURN_TIME == osal_ret)
    {
        OSAL_DEBUG("OSAL_RETURN_BUSY\n");
        osal_ret = OSAL_RETURN_BUSY;
    }

    OSAL_DEBUG("End (%u)\n", osal_ret);
    ReleaseResource(osal_mutex);
    return osal_ret;
}
#endif

e_osal_return_t R_OSAL_ThsyncMutexUnlock(osal_mutex_handle_t handle)
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


e_osal_return_t osal_cond_initialize(void)
{
    int i;

    for (i = 0; i < OSAL_RCFG_COND_MAX_NUM; i++) {
        loc_CondVars[i].id = OSAL_COND_INVALID_ID;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThsyncCondCreate(osal_cond_id_t cond_id, osal_cond_handle_t *const p_handle)
{
    st_condvar_info_t *condvar_info;

    if (NULL == p_handle || 0 == cond_id) {
        OSAL_DEBUG_ERROR("%s: Bad handle or ID\n", __func__);
        return OSAL_RETURN_PAR;
    }

    /* Protected access */
    GetResource(osal_mutex);

    condvar_info = loc_FindUnusedCondvar();
    if (condvar_info != NULL) {
        condvar_info->id = cond_id;
        condvar_info->sema = 0;
        condvar_info->nr_waiting_threads = 0;
    }

    ReleaseResource(osal_mutex);
    /* end protected access */

    *p_handle = (osal_cond_handle_t)condvar_info;
    OSAL_DEBUG_THSYNC("%s Id %lx (%p)\n", __func__, (uint32_t)cond_id, *p_handle);


    if (condvar_info == NULL) {
        return OSAL_RETURN_FAIL;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThsyncCondDestroy(osal_cond_handle_t handle)
{
    st_condvar_info_t *condvar_info = (st_condvar_info_t *)handle;

    if (NULL == handle) {
        OSAL_DEBUG_ERROR("%s: Null handle\n", __func__);
        return OSAL_RETURN_HANDLE;
    }

    /* Protected access */
    GetResource(osal_mutex);

    if (condvar_info != NULL) {
        condvar_info->id = OSAL_COND_INVALID_ID;
    }

    ReleaseResource(osal_mutex);
    /* end protected access */

    if (condvar_info == NULL) {
        OSAL_DEBUG_ERROR("%s: Bad handle\n", __func__);
        return OSAL_RETURN_HANDLE;
    }

    return OSAL_RETURN_OK;
}

static e_osal_return_t loc_CountSemaWaitTimeout(unsigned int *Sema, uint32_t Timeout_ms)
{
    osal_autosar_thread_t *thread = R_OSAL_Sys_GetCurrentThreadData();
    int need_to_wait = 0;
    StatusType ret;
    e_osal_return_t err = OSAL_RETURN_OK;
    TaskType Task_Id;
    EventMaskType Event;

    ret = GetTaskID(&Task_Id);

    if (thread == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s: no task!\n", __func__);
        return OSAL_RETURN_FAIL;
    }

    if (*Sema == 0) {
        thread->WaitingItem = (void *)Sema;
        need_to_wait = 1;
    } else {
        *Sema = *Sema - 1;
    }

    /* If sem has already been given, we don't need to wait */
    while (need_to_wait) {
        /* Set alarm for timeout */
        if (thread->AutosarTask_SleepAlarm == NULL) {
            OSAL_DEBUG_ERROR("ERROR: %s: no alarm for task %s\n", __func__, thread->Name);
            return OSAL_RETURN_FAIL;
        }
        ret = SetRelAlarm(*(thread->AutosarTask_SleepAlarm), Timeout_ms, 0);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: SetRelAlarm() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        ReleaseResource(osal_mutex);

        ret = WaitEvent(osal_event | R_AUTORSAR_ALARM_EVENT);
        GetResource(osal_mutex);
        if (ret != E_OK) {
            OSAL_DEBUG_ERROR("ERROR: %s: WaitEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
            return OSAL_RETURN_FAIL;
        }

        GetEvent(Task_Id, &Event);
        if (Event & R_AUTORSAR_ALARM_EVENT) {
            //OSAL_DEBUG_ERROR("WARNING: %s() timeout! task %s\n", __func__, thread->Name);
            need_to_wait = 0;

            ret = ClearEvent(R_AUTORSAR_ALARM_EVENT);
            if (ret != E_OK) {
                OSAL_DEBUG_ERROR("ERROR: %s: ClearEvent() %d (%s)\n", __func__, ret, R_OSAL_Sys_GetTplErrorStr(ret));
                err = OSAL_RETURN_FAIL;
            }

            err = OSAL_RETURN_BUSY;
        } else {
            /* It is possible that the alarm has already expired, so don't throw an error */
            CancelAlarm(*(thread->AutosarTask_SleepAlarm));

            if (*Sema != 0) {
                thread->WaitingItem = NULL;
                *Sema = *Sema - 1;
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

e_osal_return_t R_OSAL_ThsyncCondWaitForTimePeriod(osal_cond_handle_t cond_handle, osal_mutex_handle_t mutex_handle,
                                                   osal_milli_sec_t time_period)
{
    st_condvar_info_t *condvar_info = (st_condvar_info_t *)cond_handle;
    e_osal_return_t err;
    e_osal_return_t retval;

// TODO This has to be atomic:
    condvar_info->nr_waiting_threads++;

    R_OSAL_ThsyncMutexUnlock(mutex_handle);

    GetResource(osal_mutex);
    err = loc_CountSemaWaitTimeout(&condvar_info->sema, time_period);
    retval = err;
    ReleaseResource(osal_mutex);

    err = R_OSAL_ThsyncMutexLockForTimePeriod(mutex_handle, time_period);
    if (retval == OSAL_RETURN_OK) {
        retval = err;
    }

    return retval;
}

e_osal_return_t R_OSAL_ThsyncCondWaitUntilTimeStamp(osal_cond_handle_t cond_handle, osal_mutex_handle_t mutex_handle,
                                                    const st_osal_time_t *const p_time_stamp)
{
    st_condvar_info_t *condvar_info = (st_condvar_info_t *)cond_handle;
    e_osal_return_t err;
    e_osal_return_t retval;
    st_osal_time_t current_time_stamp;
    e_osal_return_t osal_ret;
    osal_nano_sec_t timeout_ns;
    osal_milli_sec_t time_period;

    osal_ret = R_OSAL_Internal_GetTimestamp(&current_time_stamp);
    if (OSAL_RETURN_OK == osal_ret) {
        osal_ret = R_OSAL_Internal_GetDiffTime(p_time_stamp, &current_time_stamp, &timeout_ns);
    }

    time_period = timeout_ns / 1000000;

// TODO This has to be atomic:
    condvar_info->nr_waiting_threads++;

    R_OSAL_ThsyncMutexUnlock(mutex_handle);

    GetResource(osal_mutex);
    err = loc_CountSemaWaitTimeout(&condvar_info->sema, time_period);
    retval = err;
    ReleaseResource(osal_mutex);

    err = R_OSAL_ThsyncMutexLockForTimePeriod(mutex_handle, time_period);
    if (retval == OSAL_RETURN_OK) {
        retval = err;
    }

    return retval;
}

e_osal_return_t R_OSAL_ThsyncCondBroadcast(osal_cond_handle_t cond_handle)
{
    st_condvar_info_t *condvar_info = (st_condvar_info_t *)cond_handle;

    GetResource(osal_mutex);

    while (condvar_info->nr_waiting_threads != 0) {
        condvar_info->sema = condvar_info->sema + 1;
        condvar_info->nr_waiting_threads--;
        R_OSAL_Sys_KickTask(&condvar_info->sema);
    }

    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_ThsyncCondSignal(osal_cond_handle_t cond_handle)
{
    st_condvar_info_t *condvar_info = (st_condvar_info_t *)cond_handle;

    GetResource(osal_mutex);

    condvar_info->sema = condvar_info->sema + 1;
    condvar_info->nr_waiting_threads--;
    R_OSAL_Sys_KickTask(&condvar_info->sema);

    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}
