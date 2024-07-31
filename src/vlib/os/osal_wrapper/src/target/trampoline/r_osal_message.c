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
/** OSAL Message Manager for Trampoline (AUTOSAR) */

#include <stdlib.h>

#include "r_osal_api.h"
#include "target/trampoline/r_osal_common.h"

//#define OSAL_DEBUG_MQ(...)
#define OSAL_DEBUG_MQ OSAL_DEBUG

/*******************************************************************************
  Mq structure.

  Members:
  Start       - Pointer to 1st valid message in queue
  End         - Pointer to next free message buffer
*/
typedef struct {
    osal_mq_id_t               Id;
    unsigned int               Open;
    const st_osal_mq_config_t  *p_config;
    base_mutex_t               Lock;
    base_sema_t                Signal;
    base_sema_t                Space;
    char                       *Buffer;
    char                       *Start;
    char                       *End;
    int                        ValidByte;
} R_OS_MqData_t;


/*******************************************************************************
  MQ data pool.

  When a MQ is created, this pool is used to keep the MQ's internal data.
*/
static R_OS_MqData_t R_OS_MqPool[OSAL_RCFG_MQ_MAX_NUM];


static const st_osal_mq_control_t *const loc_GetControl(osal_mq_id_t Id)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_return_t api_ret;
    size_t num_of_info;
    uint32_t count = 0;
    st_osal_mq_control_t *control = NULL;

    num_of_info = R_OSAL_RCFG_GetNumOfMq();

    while((count < num_of_info) && (OSAL_RETURN_OK == osal_ret))
    {
        api_ret = R_OSAL_RCFG_GetMqInfo(count, &control);
        if ((OSAL_RETURN_OK != api_ret) || (NULL == control) || (NULL == control->usr_config))
        {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            return NULL;
        }

        if (control->usr_config->id == Id) {
            return control;
        }
        count++;
    }

    return NULL;
}

e_osal_return_t osal_mq_initialize(void)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_return_t api_ret;
    size_t num_of_info;
    uint32_t count = 0;
    st_osal_mq_control_t *control = NULL;

    OSAL_DEBUG_MQ("%s\n", __func__);

    num_of_info = R_OSAL_RCFG_GetNumOfMq();

    while (count < num_of_info) {
        api_ret = R_OSAL_RCFG_GetMqInfo(count, &control);
        if ((OSAL_RETURN_OK != api_ret) || (NULL == control) || (NULL == control->usr_config)) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            osal_ret = OSAL_RETURN_FAIL;
            break;
        }

        /* check Message queue configuration */
        if ((0U == control->usr_config->config.max_num_msg) ||
            (0U == control->usr_config->config.msg_size)) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            osal_ret = OSAL_RETURN_CONF;
            break;
        }

        /* allocate memory for the Message queue */
        control->buffer = malloc(control->usr_config->config.max_num_msg * control->usr_config->config.msg_size);
        if (control->buffer == NULL) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            osal_ret = OSAL_RETURN_MEM;
            break;
        }

        count++;
    }

    if (OSAL_RETURN_OK != osal_ret) {
        (void)osal_mq_deinitialize();
    }

    return osal_ret;
}

e_osal_return_t osal_mq_deinitialize(void)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_return_t api_ret;
    size_t num_of_info;
    uint32_t count = 0;
    st_osal_mq_control_t *control = NULL;

    num_of_info = R_OSAL_RCFG_GetNumOfMq();

    while (count < num_of_info) {
        api_ret = R_OSAL_RCFG_GetMqInfo(count, &control);
        if ((OSAL_RETURN_OK != api_ret) || (NULL == control) || (NULL == control->usr_config)) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            osal_ret = OSAL_RETURN_FAIL;
            break;
        }

        free(control->buffer);

        count++;
    }

    return osal_ret;
}

/*******************************************************************************
  Allocate a MQ data object from the pool.

  Returns:
  ==0 - No object allocated
  !=0 - Pointer to am MQ data object
*/
static R_OS_MqData_t *loc_AllocMqData(void)
{
    unsigned int i;

    i = 0;
    while (i < OSAL_RCFG_MQ_MAX_NUM) {
        if (R_OS_MqPool[i].Id == 0) {
            break;
        }
        i++;
    }
    if (i >= OSAL_RCFG_MQ_MAX_NUM) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        return NULL;
    }

    return &R_OS_MqPool[i];
}

/*******************************************************************************
  Find existing MQ by its Id.

  Returns:
  ==0 - No object found
  !=0 - Pointer to am MQ data object
*/
static R_OS_MqData_t *loc_FindMqById(osal_mq_id_t Id)
{
    unsigned int i;

    i = 0;
    while (i < OSAL_RCFG_MQ_MAX_NUM) {
        if (R_OS_MqPool[i].Id == Id) {
            break;
        }
        i++;
    }
    if (i >= OSAL_RCFG_MQ_MAX_NUM) {
        return 0;
    }
    return &R_OS_MqPool[i];
}

e_osal_return_t R_OSAL_MqCreate(const st_osal_mq_config_t *const p_config, osal_mq_id_t mq_Id,
                                osal_mq_handle_t *const p_handle)
{
    const st_osal_mq_control_t *control = NULL;
    R_OS_MqData_t *mq;
    e_osal_return_t x;
    int max_num_msg;
    int i;

    OSAL_DEBUG_MQ("%s Id %lx\n", __func__, (uint32_t)mq_Id);

    if (mq_Id == 0 || p_handle == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        return OSAL_RETURN_PAR;
    }

    /* Protect Mq list */
    GetResource(osal_mutex);

    /* Note: The actual size of the queue is defined in osal_usr_cfg and allocated
     * before we get here, so any config passed in is ignored.
     * That's handy as some users pass NULL for p_config, some pass a valid ptr.
     * Those that pass a valid ptr specify config that is larger than specified
     * in osal_usr_cfg.
     */
    (void)p_config;

    control = loc_GetControl(mq_Id);
    if (control == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return OSAL_RETURN_FAIL;
    }

    mq = loc_FindMqById(mq_Id);
    if (mq) {
        mq->Open++;
    } else {
        mq = loc_AllocMqData();
        if (mq == NULL) {
            OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
            ReleaseResource(osal_mutex);
            return OSAL_RETURN_FAIL;
        }

        base_mutex_create(&mq->Lock);
        base_sema_create(&mq->Signal);
        base_sema_create(&mq->Space);
        max_num_msg = (int)control->usr_config->config.max_num_msg;
        mq->Space = max_num_msg;
        mq->Id            = mq_Id;
        mq->Open          = 1;
        mq->Buffer        = control->buffer;
        mq->Start         = mq->Buffer;
        mq->End           = mq->Start;
        mq->ValidByte     = 0;
        mq->p_config      = &control->usr_config->config;
    }

    *p_handle = (osal_mq_handle_t)mq;
    if (p_config == NULL) {
        OSAL_DEBUG_MQ("%s: Id %lx, mq=%p (%d x %d), Signal %p, Space %p\n", __func__, (uint32_t)mq_Id, mq, (int)mq->p_config->max_num_msg, (int)mq->p_config->msg_size, &mq->Signal, &mq->Space);
    } else {
        OSAL_DEBUG_MQ("%s: Id %lx, mq=%p (%d x %d), Signal %p, Space %p\n", __func__, (uint32_t)mq_Id, mq, (int)mq->p_config->max_num_msg, (int)mq->p_config->msg_size, &mq->Signal, &mq->Space);
    }

    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqDelete(osal_mq_handle_t handle)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;

    mq->Open--;
    if (mq->Open == 0) {
        mq->Id = 0;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqReset(osal_mq_handle_t handle)
{
    //TODO
    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqGetConfig(osal_mq_handle_t handle, st_osal_mq_config_t *const p_config)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;

    if (mq != NULL) {
        *p_config = *mq->p_config;
        return OSAL_RETURN_OK;
    }

    return OSAL_RETURN_FAIL;
}

e_osal_return_t R_OSAL_MqSendUntilTimeStamp(osal_mq_handle_t handle, const st_osal_time_t *const p_time_stamp,
                                            const void * p_buffer, size_t MsgLen)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;
    int            x;
    size_t         n;
    char          *eob;
    const char    *MsgPtr = p_buffer;
    st_osal_time_t current_time_stamp;
    e_osal_return_t osal_ret;
    osal_nano_sec_t timeout_ns;

    OSAL_DEBUG_MQ("%s mq=%p Send %d bytes (space %d)\n", __func__, mq, MsgLen, (int)mq->Space);

    GetResource(osal_mutex);

    if (mq == NULL) {
        ReleaseResource(osal_mutex);
        return OSAL_RETURN_HANDLE;
    }

    osal_ret = R_OSAL_Internal_GetTimestamp(&current_time_stamp);
    if (OSAL_RETURN_OK == osal_ret) {
        osal_ret = R_OSAL_Internal_GetDiffTime(p_time_stamp, &current_time_stamp, &timeout_ns);
    }

    /* Wait until there is space in the queue */
    x = base_sema_take_timeout(&mq->Space, timeout_ns / 1000000);
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
    x = base_mutex_lock_timeout(&mq->Lock, timeout_ns / 1000000);
    if (x != OSAL_RETURN_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return x;
    }

    /* send message */
    n = 0;
    eob = mq->Buffer + (mq->p_config->msg_size * mq->p_config->max_num_msg);

//    OSAL_DEBUG_MQ("%s mq=%p mq->p_config->msg_size %d\n", __func__, mq, (int)mq->p_config->msg_size);
    while ((n < mq->p_config->msg_size) && (n < MsgLen)) {
        *mq->End = *MsgPtr;
//        OSAL_DEBUG_MQ("0x%x, ", *MsgPtr);
        mq->ValidByte++;
        MsgPtr++;
        mq->End++;
        n++;
        if (mq->End >= eob) {
            mq->End = mq->Buffer;
        }
    }
    OSAL_DEBUG("\n");
//    OSAL_DEBUG_MQ("%s mq=%p ValidByte %d\n", __func__, mq, mq->ValidByte);

    base_mutex_unlock(&mq->Lock);
    /* --- End protect access to the queue --- */

    /* Signal waiting threads that there is a message in the queue */
    base_sema_give(&mq->Signal);

    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqReceiveUntilTimeStamp(osal_mq_handle_t handle, const st_osal_time_t *const p_time_stamp,
                                               void *p_buffer, size_t MsgLen)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;
    int            x;
    size_t         n;
    char          *eob;
    char          *MsgPtr = p_buffer;
    st_osal_time_t current_time_stamp;
    e_osal_return_t osal_ret;
    osal_nano_sec_t timeout_ns;

    OSAL_DEBUG_MQ("%s mq=%p MsgLen %d\n", __func__, mq, MsgLen);

    GetResource(osal_mutex);

    if (mq == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return OSAL_RETURN_HANDLE;
    }

    osal_ret = R_OSAL_Internal_GetTimestamp(&current_time_stamp);
    if (OSAL_RETURN_OK == osal_ret) {
        osal_ret = R_OSAL_Internal_GetDiffTime(p_time_stamp, &current_time_stamp, &timeout_ns);
    }

    /* Waiting until there is a message in the queue */
    x = base_sema_take_timeout(&mq->Signal, timeout_ns / 1000000);
    if (x != OSAL_RETURN_OK) {
        if (x != OSAL_RETURN_TIME) {
            OSAL_DEBUG("DEBUG: %s:%d\n", __func__, __LINE__);
        }
        ReleaseResource(osal_mutex);
        return x;
    }

    /* --- Protect access to the queue --- */
    x = base_mutex_lock_timeout(&mq->Lock, timeout_ns / 1000000);
    if (x != OSAL_RETURN_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return x;
    }

    if (mq->ValidByte < MsgLen) {
        /* We're trying to read more data than was sent */
        OSAL_DEBUG_ERROR("ERROR: %s mq=%p, ValidByte %d, want %d\n", __func__, mq, mq->ValidByte, MsgLen);
        n = OSAL_RETURN_PAR;
    } else {
        /* read message */
        n = 0;
        eob = mq->Buffer + (mq->p_config->msg_size * mq->p_config->max_num_msg);

        while ((n < mq->p_config->msg_size) && (n < MsgLen)) {
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

e_osal_return_t R_OSAL_MqReceiveForTimePeriod(osal_mq_handle_t handle, osal_milli_sec_t timeout_ms,
                                              void *p_buffer, size_t MsgLen)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;
    int            x;
    size_t         n;
    char          *eob;
    char          *MsgPtr = p_buffer;

    OSAL_DEBUG_MQ("%s mq=%p MsgLen %d\n", __func__, mq, MsgLen);

    if (mq == NULL) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        return OSAL_RETURN_HANDLE;
    }

    GetResource(osal_mutex);

    /* Waiting until there is a message in the queue */
    x = base_sema_take_timeout(&mq->Signal, timeout_ms);
    if (x != OSAL_RETURN_OK) {
        if (x != OSAL_RETURN_TIME) {
            OSAL_DEBUG("DEBUG: %s:%d\n", __func__, __LINE__);
        }
        ReleaseResource(osal_mutex);
        return x;
    }

    /* --- Protect access to the queue --- */
    x = base_mutex_lock_timeout(&mq->Lock, timeout_ms);
    if (x != OSAL_RETURN_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return x;
    }

    if (mq->ValidByte < MsgLen) {
        /* We're trying to read more data than was sent */
        OSAL_DEBUG_ERROR("ERROR: %s mq=%p, ValidByte %d, want %d\n", __func__, mq, mq->ValidByte, MsgLen);
        n = OSAL_RETURN_PAR;
    } else {
        /* read message */
        n = 0;
        eob = mq->Buffer + (mq->p_config->msg_size * mq->p_config->max_num_msg);

        while ((n < mq->p_config->msg_size) && (n < MsgLen)) {
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

e_osal_return_t R_OSAL_MqSendForTimePeriod(osal_mq_handle_t handle, osal_milli_sec_t timeout_ms,
                                           const void *p_buffer, size_t MsgLen)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;
    int            x;
    size_t         n;
    char          *eob;
    const char    *MsgPtr = p_buffer;

    OSAL_DEBUG_MQ("%s mq=%p Send %d bytes (space %d)\n", __func__, mq, MsgLen, (int)mq->Space);

    if (mq == NULL) {
        return OSAL_RETURN_HANDLE;
    }

    GetResource(osal_mutex);

    /* Wait until there is space in the queue */
    x = base_sema_take_timeout(&mq->Space, timeout_ms);
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
    x = base_mutex_lock_timeout(&mq->Lock, timeout_ms);
    if (x != OSAL_RETURN_OK) {
        OSAL_DEBUG_ERROR("ERROR: %s:%d\n", __func__, __LINE__);
        ReleaseResource(osal_mutex);
        return x;
    }

    /* send message */
    n = 0;
    eob = mq->Buffer + (mq->p_config->msg_size * mq->p_config->max_num_msg);

//    OSAL_DEBUG_MQ("%s mq=%p mq->p_config->msg_size %d\n", __func__, mq, (int)mq->p_config->msg_size);
    while ((n < mq->p_config->msg_size) && (n < MsgLen)) {
        *mq->End = *MsgPtr;
//        OSAL_DEBUG_MQ("0x%x, ", *MsgPtr);
        mq->ValidByte++;
        MsgPtr++;
        mq->End++;
        n++;
        if (mq->End >= eob) {
            mq->End = mq->Buffer;
        }
    }
    OSAL_DEBUG("\n");
//    OSAL_DEBUG_MQ("%s mq=%p ValidByte %d\n", __func__, mq, mq->ValidByte);

    base_mutex_unlock(&mq->Lock);
    /* --- End protect access to the queue --- */

    /* Signal waiting threads that there is a message in the queue */
    base_sema_give(&mq->Signal);

    ReleaseResource(osal_mutex);

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqIsFull(osal_mq_handle_t handle, bool *const p_result)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;

    if (mq == NULL) {
        return OSAL_RETURN_HANDLE;
    }

    /* Note: There is no need to lock when reading a single element from the
     * message queue structure */
    if (mq->ValidByte == mq->p_config->msg_size * mq->p_config->max_num_msg || mq->Space == 0) {
        *p_result = true;
    } else {
        *p_result = false;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqIsEmpty(osal_mq_handle_t handle, bool *const p_result)
{
    R_OS_MqData_t *mq = (R_OS_MqData_t *)handle;

    if (mq == NULL) {
        return OSAL_RETURN_HANDLE;
    }

    /* Note: There is no need to lock when reading a single element from the
     * message queue structure */
    if (mq->ValidByte == 0 && mq->Space != 0) {
        *p_result = true;
    } else {
        *p_result = false;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_MqInitializeMqConfigSt(st_osal_mq_config_t *const p_config)
{
    if (NULL == p_config) {
        return OSAL_RETURN_PAR;
    }

    p_config->max_num_msg   = 10;
    p_config->msg_size      = 128;

    return OSAL_RETURN_OK;
}
