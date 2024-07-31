/*************************************************************************************************************
* WindowMng_wm_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*
 *  Section Includes
 */
#include <stdint.h>
#include <string.h>
#include "r_wm_config.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_wm_api.h"
#include "r_wm_msg_com.h"
#include "r_print_api.h"

/*
 * Section: Local Defines
 */

/* ResourceID */
#define WM_RESOURCE_ID      0x8000U                 /* RESOURCE_ID_BSP   */
#define WM_MUTEX_CREATE_01  (WM_RESOURCE_ID + 30U)  /* OSAL_MUTEX_WM2_01 */

#define WM_MQ_CREATE_01     (WM_RESOURCE_ID + 46U)  /* OSAL_MQ_WM0_00    */

/* Mq definitions */
#define WM_MQ_NUM_OF_MSG    1
#define WM_MQ_MSG_SIZE      sizeof(uint32_t)

/*
 *  Section type definitions
 */

/**
 * Structure containing dynamically assignable semaphores
 */
typedef struct {
    uint32_t            Pid;        /**< process Id */
    osal_mq_handle_t    MqHandle;   /**< Handle for Message Queue */
} r_wm_SemaPool_t;

/**
 * Structure containing the WM Device information
 */
typedef struct {
    int32_t              ComCh;          /**< Reference to the communication chanel for the WM Dev */
    osal_mutex_handle_t  WmMutex;        /**< Need this to protect the ComCh initialization ONLY */
    r_wm_SemaPool_t      FlushWait[R_WM_NUM_LAYER_PER_VOUT];  /**< List of semaphores dynamically
                                                                assigned to flush wait */
    uint32_t ScreenWidth;       /**< Screen width of the Wm device in Pixel */
    uint32_t ScreenPitch;       /**< Screen pitch of the Wm device in bytes */
    uint32_t ScreenHeight;      /**< Screen height of the Wm device in Pixel */
    uint32_t HbackPorchOffset;  /**< Horizontal back porch offset (register for DOC settings) */
    uint32_t VbackPorchOffset;  /**< Vertical back porch offset (register for DOC settings) */
    bool     IsInitialized;     /**< Is the display already initialized */
} r_wm_Dev_Info_t;

/**
 * Array containing the information about the WM device
 */
static r_wm_Dev_Info_t loc_WmDevInfo[R_WM_DEV_LAST] = {
    {
        -1, OSAL_MUTEX_HANDLE_INVALID, {}, 0, 0, 0, 0, 0, false
    },{
        -1, OSAL_MUTEX_HANDLE_INVALID, {}, 0, 0, 0, 0, 0, false
    },{
        -1, OSAL_MUTEX_HANDLE_INVALID, {}, 0, 0, 0, 0, 0, false
    },{
        -1, OSAL_MUTEX_HANDLE_INVALID, {}, 0, 0, 0, 0, 0, false
    }
};

/**
 * Static storage for messages (pair of Tx and Rx message per WM Unit)
 * Access to elements must be protected by WmMutex.
 */
static r_wm_MsgTx_t MsgTx[R_WM_DEV_LAST];
static r_wm_MsgRx_t MsgRx[R_WM_DEV_LAST];

/*
 *  Section: local Functions
 */
static void loc_Error_Handler(r_wm_Error_t Err);

/**
 * Local Error handler. Depending on the debug level it will print
 *          a string corresponding to the error received.
 *
 * @param Err @ref r_wm_Error_t
 */
static void loc_Error_Handler(r_wm_Error_t Err)
{
    switch (Err) {
    case R_WM_ERR_FAILED:
        R_PRINT_Log ("No %d: Failed - no info.\n\r", Err);
        break;
    case R_WM_ERR_NOT_INITIALIZED:
        R_PRINT_Log ("No %d: WMdev is not initialised.\n\r", Err);
        break;
    case R_WM_ERR_WRONG_PARAMETER:
        R_PRINT_Log ("No %d: Incorrect input parameter.\n\r", Err);
        break;
    case R_WM_ERR_NULL_POINTER:
        R_PRINT_Log ("No %d: One input parameter is null pointer.\n\r", Err);
        break;
    case R_WM_ERR_INVALID_PROP:
        R_PRINT_Log ("No %d: Property is not valid.\n\r", Err);
        break;
    case R_WM_ERR_RINGBUF_NOT_INITIALISED:
        R_PRINT_Log ("No %d: Ring buffer is not initialised.\n\r", Err);
        break;
    case R_WM_ERR_RINGBUFFER_OVERFLOW:
        R_PRINT_Log ("No %d: Ring buffer has overflowed.\n\r", Err);
        break;
    case R_WM_ERR_CLOSECOM_FAILED:
        R_PRINT_Log ("No %d: Failed to close communication.\n\r", Err);
        break;
    case R_WM_ERR_MSGIOCTL_FAILED:
        R_PRINT_Log ("No %d: Failed to send message.\n\r", Err);
        break;
    case R_WM_ERR_INVALID_CMD:
        R_PRINT_Log ("No %d: Invalid command.\n\r", Err);
        break;
    case R_WM_ERR_OS_SEMA:
        R_PRINT_Log ("No %d: OS return error on Semaphore operation.\n\r", Err);
        break;
    case R_WM_ERR_OS_MUTEX:
        R_PRINT_Log ("No %d: OS return error on Mutex operation.\n\r", Err);
        break;
    case R_WM_ERR_DEINIT_FAILED:
        R_PRINT_Log ("No %d:De-initialisation of the Wmdevice failed.\n\r", Err);
        break;
    case R_WM_ERR_UNIT_USED:
        R_PRINT_Log ("No %d: Wm Device is still in used.\n\r", Err);
        break;
    case R_WM_ERR_WRONG_PID:
        R_PRINT_Log ("No %d: Wrong PID. Not allowed to execute command.\n\r", Err);
        break;
    case R_WM_ERR_NO_SURF:
        R_PRINT_Log ("No %d: No surface available.\n\r", Err);
        break;
    case R_WM_ERR_TOO_MANY_THREADS:
        R_PRINT_Log ("No %d: the max number of thread using this WM Device is reached.\n\r", Err);
        break;
    case R_WM_ERR_DUINIT_FAILED:
    case R_WM_ERR_VSPDINIT_FAILED:
        R_PRINT_Log ("No %d: Failed to initialised the VOUT for the WM Device.\n\r", Err);
        break;
    case R_WM_ERR_VSPDBLEND_FAILED:
        R_PRINT_Log ("No %d: Failed to setup the blending operation.\n\r", Err);
        break;
    case R_WM_ERR_UNIT_ALREADY_INIT:
        R_PRINT_Log ("No %d: Device is already initialized.\n\r", Err);
        break;
    default:
        R_PRINT_Log ("No %d: Unknown Error Something went really wrong....\n\r", Err);
        break;
    }
}

/*
 * Section Global Functions
 */

r_wm_Error_t R_WM_DevInit(r_wm_Dev_t WmUnit)
{
    r_wm_Error_t    ret = R_WM_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_mutex_handle_t  temp_mutex_handle = OSAL_MUTEX_HANDLE_INVALID;
    st_osal_mq_config_t mq_config;
    int32_t i;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DevInit: ");
        loc_Error_Handler(ret);
        goto clean;
    }

    if (loc_WmDevInfo[WmUnit].IsInitialized == true) {
        ret = R_WM_ERR_UNIT_ALREADY_INIT;
        R_PRINT_Log("R_WM_DevInit: ");
        loc_Error_Handler(ret);
        goto clean;
    }

    /* Initialise mutex */
    osal_ret = R_OSAL_ThsyncMutexCreate( (WM_MUTEX_CREATE_01 + WmUnit), &temp_mutex_handle);
    if (OSAL_RETURN_OK != osal_ret && OSAL_RETURN_BUSY != osal_ret) {
        ret = R_WM_ERR_FAILED;
        R_PRINT_Log("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);
        loc_Error_Handler(ret);
        goto clean;
    }
    if (OSAL_RETURN_OK == osal_ret) {
        loc_WmDevInfo[WmUnit].WmMutex = temp_mutex_handle;
    }
    do {
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
    } while (OSAL_RETURN_TIME == osal_ret);
    if (OSAL_RETURN_OK != osal_ret) {
        ret = R_WM_ERR_FAILED;
        R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        R_PRINT_Log("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);
        loc_Error_Handler(ret);
        goto clean;
    }


    /* Enter critical here to evaluate ComCh...if another R_WM_DevInit call made to same WmUnit
       before we get the comm channel initialized, we would have a hazard
     */
    if (0 >= loc_WmDevInfo[WmUnit].ComCh) {
        /* set ComCh=1 to singal the init process on this channel is running
           and another request has to go to the 'already initialized branch
        */
        loc_WmDevInfo[WmUnit].ComCh = 1;

        /* Init Com channel for the Wm Device
         * It will create a server task for the WM Unit if it has not been done
         * before and in case of RCAR initialise DU and VSPD unit.
         */
        ret = R_WM_Priv_MsgComOpen(WmUnit, &(loc_WmDevInfo[WmUnit].ComCh));
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("WM(%d):MsgComOpen=%d\n", WmUnit, ret);
        }
        osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
        if (OSAL_RETURN_OK != osal_ret) {
             R_PRINT_Log("WM(%d):MutexUnlock=%d\n", WmUnit, osal_ret);
        }

        /* Create the FlushWait semaphores
         * This doesn't have to be mutex protected as the conditional entry
         * will not extend the existing hazard (as described above)
         */
        if (R_WM_ERR_SUCCESS == ret) {
            osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
            if(OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d): R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", WmUnit, osal_ret);
            }
            mq_config.max_num_msg = WM_MQ_NUM_OF_MSG;
            mq_config.msg_size    = WM_MQ_MSG_SIZE;
            for (i = 0; i < R_WM_NUM_LAYER_PER_VOUT; i++ ) {
                osal_ret = R_OSAL_MqCreate(&mq_config, (WM_MQ_CREATE_01 + (R_WM_NUM_LAYER_PER_VOUT * WmUnit) + i),
                                                     &loc_WmDevInfo[WmUnit].FlushWait[i].MqHandle);
                if (OSAL_RETURN_OK != osal_ret) {
                    ret = R_WM_ERR_FAILED;
                    R_PRINT_Log("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);
                    R_PRINT_Log("WM(%d): R_OSAL_MqCreate(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                                 WmUnit, osal_ret, mq_config.max_num_msg, mq_config.msg_size, (WM_MQ_CREATE_01 + (R_WM_NUM_LAYER_PER_VOUT * WmUnit) + i));
                    loc_Error_Handler(ret);
                }
            }
        }
        if (R_WM_ERR_SUCCESS == ret) {
            do {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
            } while (OSAL_RETURN_TIME == osal_ret);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
            }
            if (OSAL_RETURN_OK == osal_ret) {
                (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
                (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
                /* Prepare Wm Message and send. Pass the device name as parameter*/
                MsgTx[WmUnit].Dev    = WmUnit;
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_INIT;

                ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
                osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
                }
                if (R_WM_ERR_SUCCESS != ret) {
                    R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
                    R_WM_Priv_MsgComClose(loc_WmDevInfo[WmUnit].ComCh);
                    loc_WmDevInfo[WmUnit].ComCh = -1;
                }
            }
        }
    } else {
        /* we were already initialized */
        osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexUnlock failed with already initialized root =%d\n",WmUnit, osal_ret);
        }
    }

    if (R_WM_ERR_SUCCESS == ret && OSAL_RETURN_OK == osal_ret) {
        loc_WmDevInfo[WmUnit].IsInitialized = true;
    }

clean:
    return ret;
}

r_wm_Error_t R_WM_DevDeinit(r_wm_Dev_t WmUnit)
{
    r_wm_Error_t  ret = R_WM_ERR_FAILED;
    osal_thread_handle_t    self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int32_t i;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DevDeinit: ");
        loc_Error_Handler(ret);
        goto end;
    }
    if (0 >= loc_WmDevInfo[WmUnit].ComCh) {
        ret = R_WM_ERR_NOT_INITIALIZED;
        R_PRINT_Log("R_WM_DevDeinit: ");
        loc_Error_Handler(ret);
        goto end;
    }
    {
        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):ThreadSelf=%d\n",WmUnit, osal_ret);
        }

        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            /* Prepare Wm Message and send */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            /* Prepare Wm Message and send. Pass the device name as parameter*/
            MsgTx[WmUnit].Dev    = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_DEINIT;
            MsgTx[WmUnit].Data.Val = (intptr_t)self_thread_handle;

            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
        if ((R_WM_ERR_SUCCESS == ret) || (R_WM_ERR_UNIT_USED == ret)) {
            do {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
            } while (OSAL_RETURN_TIME == osal_ret);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
            }
            if (OSAL_RETURN_OK == osal_ret) {
                ret = R_WM_Priv_MsgComClose(loc_WmDevInfo[WmUnit].ComCh);
                osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
                }
                loc_WmDevInfo[WmUnit].ComCh = -1;
            }
        }
    }
    if (OSAL_RETURN_OK == osal_ret) {
        osal_ret = R_OSAL_ThsyncMutexDestroy(loc_WmDevInfo[WmUnit].WmMutex);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexDestroy=%d\n", WmUnit, osal_ret);
        }
    }
    if (OSAL_RETURN_OK == osal_ret) {
        for (i = 0; i < R_WM_NUM_LAYER_PER_VOUT; i++ ) {
            osal_ret = R_OSAL_MqDelete(loc_WmDevInfo[WmUnit].FlushWait[i].MqHandle);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MqDelete=%d\n", WmUnit, osal_ret);
            }
        }
    }

    if (R_WM_ERR_SUCCESS == ret && OSAL_RETURN_OK == osal_ret) {
        loc_WmDevInfo[WmUnit].IsInitialized = false;
    }

end:
    return ret;
}

r_wm_Error_t R_WM_DevInfoGet(r_wm_Dev_t WmUnit, uint32_t * MaxLayerNum,
                uint32_t * ScreenWidth, uint32_t * ScreenHeight)
{
    r_wm_Error_t    ret = R_WM_ERR_FAILED;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DevInfoGet: ");
        loc_Error_Handler(ret);
    } else {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            /* Prepare Wm Message and send */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            /* Prepare Wm Message and send. Pass the device name as parameter*/
            MsgTx[WmUnit].Dev    = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SC_TIMINGS;
            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            /* Currently Get LayerNb/ScreenWidth/ScreenHeight API does not exist,
               so uses R_WMDRV_VoutTimings instead of it and expects R_WM_ERR_UNIT_ALREADY_INIT */
            if (R_WM_ERR_UNIT_ALREADY_INIT == ret) {
                *MaxLayerNum  = MsgRx[WmUnit].Data.DevInfoData.LayerNb;
                *ScreenWidth  = MsgRx[WmUnit].Data.DevInfoData.ScreenWidth;
                *ScreenHeight = MsgRx[WmUnit].Data.DevInfoData.ScreenHeight;
                ret = R_WM_ERR_SUCCESS;
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }
    return ret;
}

r_wm_Error_t R_WM_DevEventRegister (r_wm_Dev_t WmUnit, r_wm_EventId_t  EventId,
                uint32_t *Arg, void *Mq)
{
    r_wm_Error_t ret = R_WM_ERR_FAILED;
    osal_thread_handle_t    self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /**
     * We only support R_WM_EVENT_VBLANK
     * ...R_WM_EVENT_FLUSHSYNC internal only
     */

    /* Test input prameters */
    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DevEventRegister: ");
        loc_Error_Handler(ret);
    } else {
        if (NULL == Mq) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("R_WM_DevEventRegister: ");
            loc_Error_Handler(ret);
        } else if (R_WM_EVENT_VBLANK != EventId) {
            ret = R_WM_ERR_WRONG_PARAMETER;
            R_PRINT_Log("R_WM_DevEventRegister: ");
            loc_Error_Handler(ret);
        } else {
            ret = R_WM_ERR_SUCCESS;
        }
    }

    if (R_WM_ERR_SUCCESS == ret) {
        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):ThreadSelf=%d\n", WmUnit, osal_ret);
        }

        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            /* Prepare Wm Message and send */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            /* Prepare Wm Message and send. Pass the device name as parameter*/
            MsgTx[WmUnit].Dev    = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_REGISTER_EVENT;
            MsgTx[WmUnit].Data.EventRegMsgData.Pid = (uint32_t)self_thread_handle;
            MsgTx[WmUnit].Data.EventRegMsgData.EvId = EventId;
            MsgTx[WmUnit].Data.EventRegMsgData.Arg = Arg;
            MsgTx[WmUnit].Data.EventRegMsgData.MqHandle = *((osal_mq_handle_t*)Mq);
            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }
    return ret;
}

/*
 * Group: WM Screen Functions
 */

r_wm_Error_t R_WM_ScreenPropertySet (r_wm_Dev_t WmUnit, r_wm_ScreenProp_t Prop, void * Data)
{
    r_wm_Error_t    ret = R_WM_ERR_FAILED;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /* Check input paramters */
    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_ScreenPropertySet WmUnit >=: ");
        loc_Error_Handler(ret);
        goto end;
    }
    if (R_WM_SCREEN_PROP_LAST <= Prop) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_ScreenPropertySet Prop >=: ");
        loc_Error_Handler(ret);
        goto end;
    }
    if (0 == Data) {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("R_WM_ScreenPropertySet Data Null: ");
        loc_Error_Handler(ret);
        goto end;
    }
    /* end check  */

    {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            /* Prepare Wm Message and send */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            /* Prepare Wm Message and send. Pass the device name as parameter*/
            MsgTx[WmUnit].Dev = WmUnit;
            switch (Prop) {
            case R_WM_SCREEN_PROP_TIMING:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SC_TIMINGS;
                MsgTx[WmUnit].Data.Ptr = Data;
                break;
            case R_WM_SCREEN_PROP_FMT:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SC_FMT;
                MsgTx[WmUnit].Data.Val = (uint32_t ) *((r_wm_ColorFmt_t *)Data);
                break;

            default: /* R_WM_SCREEN_PROP_BKGCOLOR */
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SC_BKGCL;
                MsgTx[WmUnit].Data.Val = *((uint32_t *)Data);
                break;
            }
            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }

end:
    return ret;
}

r_wm_Error_t R_WM_ScreenEnable (r_wm_Dev_t WmUnit, uint32_t Enable)
{
    r_wm_Error_t    ret = R_WM_ERR_FAILED;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_ScreenEnable: ");
        loc_Error_Handler(ret);
    }
    else {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            /* clean local message */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            MsgTx[WmUnit].Dev    = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SC_ENABLE;
            MsgTx[WmUnit].Data.Val = Enable;
            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }
    return ret;
}

r_wm_Error_t R_WM_ScreenSurfacesFlush(r_wm_Dev_t WmUnit, uint32_t VSync)
{
    r_wm_Error_t    ret = R_WM_ERR_FAILED;
    r_wm_SemaPool_t *pPool;
    int32_t PoolIndex;
    osal_thread_handle_t    self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    uint32_t myPid;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t cmd;

    osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("WM(%d):ThreadSelf=%d\n", WmUnit, osal_ret);
    }
    myPid = (uint32_t)self_thread_handle;

    /* Actually flush the ring buffer for the given WmUnit, independdantly of the
     * the surface. Surface is an input paramter that we do no use
     * Currently Blocking. default is VSync = 1
     */
    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_ScreenSurfacesFlush: ");
        loc_Error_Handler(ret);
    } else {
        pPool = loc_WmDevInfo[WmUnit].FlushWait;

        if (0 != VSync) {
            /* Find available semaphore if we're going to be waiting */
            do {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
            } while (OSAL_RETURN_TIME == osal_ret);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
            }
            if (OSAL_RETURN_OK == osal_ret) {
                for (PoolIndex = 0 ; R_WM_NUM_LAYER_PER_VOUT > PoolIndex; PoolIndex++, pPool++) {
                    if (0 == pPool->Pid) {
                        pPool->Pid = myPid;
                        break;
                    }
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
                }
                if (R_WM_NUM_LAYER_PER_VOUT <= PoolIndex) {
                    return(R_WM_ERR_TOO_MANY_THREADS);
                }
                ret = R_WM_ERR_SUCCESS;
            }
        } else {
            ret = R_WM_ERR_SUCCESS;
        }

        if (R_WM_ERR_SUCCESS == ret) {
            osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):ThreadSelf=%d\n", WmUnit, osal_ret);
            }
            do {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
            } while (OSAL_RETURN_TIME == osal_ret);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
            }
            if (OSAL_RETURN_OK == osal_ret) {
                (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
                (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
                MsgTx[WmUnit].Dev    = WmUnit;
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SC_FLUSH;
                MsgTx[WmUnit].Data.ScreenFlushData.MqHandle  = (0 == VSync) ? 0 : pPool->MqHandle;
                MsgTx[WmUnit].Data.ScreenFlushData.Pid = (uint32_t)self_thread_handle;
                ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
                if (R_WM_ERR_SUCCESS != ret) {
                    R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
                    ret = R_WM_ERR_FAILED;
                }
            } else {
                ret = R_WM_ERR_FAILED;
            }

            if ((R_WM_ERR_SUCCESS == ret) && (0 != VSync)) {
                /* Wait for the flush complete if requested to */
                do {
                    osal_ret = R_OSAL_MqReceiveForTimePeriod(pPool->MqHandle, 3000u, (void*)&cmd, WM_MQ_MSG_SIZE);
                } while (OSAL_RETURN_TIME == osal_ret);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WM(%d):MqReceive=%d\n",WmUnit, osal_ret);
                }
                if (OSAL_RETURN_OK == osal_ret) {
                    do {
                        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
                    } while (OSAL_RETURN_TIME == osal_ret);
                    if (OSAL_RETURN_OK != osal_ret) {
                        R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
                    }
                    if (OSAL_RETURN_OK == osal_ret) {
                        /* release back to pool */
                        pPool->Pid = 0;
                        osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
                        if (OSAL_RETURN_OK != osal_ret) {
                            R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
                        }
                    }
                }
                if (OSAL_RETURN_OK != osal_ret) {
                    ret = R_WM_ERR_FAILED;
                }
            }
        }
    }

    return ret;
}


/*
 * Group: Wm Surface Functions
 */

r_wm_Error_t R_WM_SurfaceGet(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf)
{
    r_wm_Error_t    ret = R_WM_ERR_FAILED;
    osal_thread_handle_t    self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_SurfaceGet: ");
        loc_Error_Handler(ret);
    } else if (0 == Surf) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_SurfaceGet: ");
        loc_Error_Handler(ret);
    } else {
        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):ThreadSelf=%d\n", WmUnit, osal_ret);
        }

        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            MsgTx[WmUnit].Dev    = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_GET;
            MsgTx[WmUnit].Data.Val = (uint32_t)self_thread_handle;

            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS == ret) {
                Surf->Handle = MsgRx[WmUnit].Data.Ptr;
            } else {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }
    return ret;
}

r_wm_Error_t R_WM_SurfacePropertySet(r_wm_Dev_t WmUnit, r_wm_SurfaceProp_t Prop,
                r_wm_Surface_t *Surf)
{
    r_wm_Error_t   ret = R_WM_ERR_FAILED;
    osal_thread_handle_t    self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_SurfacePropertySet: ");
        loc_Error_Handler(ret);
    } else if (0 == Surf) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_SurfacePropertySet: ");
        loc_Error_Handler(ret);
    } else {
        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):ThreadSelf=%d\n", WmUnit, osal_ret);
        }

        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            ret = R_WM_ERR_SUCCESS;
            /* Prepare commands and message */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            MsgTx[WmUnit].Dev    = WmUnit;
            switch (Prop){
            case R_WM_SURF_PROP_POS:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_POS;
                MsgTx[WmUnit].Data.SurfPosSetMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.SurfPosSetMsgData.PosX = Surf->PosX;
                MsgTx[WmUnit].Data.SurfPosSetMsgData.PosY = Surf->PosY;
                MsgTx[WmUnit].Data.SurfPosSetMsgData.PosZ = Surf->PosZ;
                MsgTx[WmUnit].Data.SurfPosSetMsgData.Handle = Surf->Handle;
                break;
            case R_WM_SURF_PROP_SIZE:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_SIZE;
                MsgTx[WmUnit].Data.SurfSizeSetMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.SurfSizeSetMsgData.Width = Surf->Width;
                MsgTx[WmUnit].Data.SurfSizeSetMsgData.Height = Surf->Height;
                MsgTx[WmUnit].Data.SurfSizeSetMsgData.StrideY = Surf->StrideY;
                MsgTx[WmUnit].Data.SurfSizeSetMsgData.StrideC = Surf->StrideC;
                MsgTx[WmUnit].Data.SurfSizeSetMsgData.Handle = Surf->Handle;
                break;
            case R_WM_SURF_PROP_BUFFER:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_BUF;
                MsgTx[WmUnit].Data.SurfBufSetMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.SurfBufSetMsgData.Fb = Surf->FBuf;
                MsgTx[WmUnit].Data.SurfBufSetMsgData.C0 = Surf->FBufC0;
                MsgTx[WmUnit].Data.SurfBufSetMsgData.C1 = Surf->FBufC1;
                MsgTx[WmUnit].Data.SurfBufSetMsgData.Handle = Surf->Handle;
                break;
            case R_WM_SURF_PROP_COLORFMT:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_FMT;
                MsgTx[WmUnit].Data.SurfFormatSetMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.SurfFormatSetMsgData.Format = Surf->Fmt;
                MsgTx[WmUnit].Data.SurfFormatSetMsgData.Handle = Surf->Handle;
                break;
            case R_WM_SURF_PROP_DOC:
                /* May be better to move DOC config to separate API */
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_DOC_CFG;
                MsgTx[WmUnit].Data.DocMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.DocMsgData.Callback = Surf->Doc->Callback;
                MsgTx[WmUnit].Data.DocMsgData.CbArg = Surf->Doc->CbArg;
                MsgTx[WmUnit].Data.DocMsgData.DocMonitors = Surf->Doc->DocMonitors;
                MsgTx[WmUnit].Data.DocMsgData.DU_param = Surf->Doc->DuParam;
                MsgTx[WmUnit].Data.DocMsgData.Enable = Surf->Doc->Enable;

                switch (WmUnit) {
                case R_WM_DEV_VOUT0:
                    MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 0;
                    MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 0;
                    break;
                case R_WM_DEV_VOUT1:
                    MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 1;
                    MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 0;
                    break;
                case R_WM_DEV_VOUT2:
                    MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 0;
                    MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 1;
                    break;
                default: /* R_WM_DEV_VOUT3 */
                    MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 1;
                    MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 1;
                    break;
                }
                break;
            case R_WM_SURF_PROP_DISCOM:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_DISCOM_CFG;
                MsgTx[WmUnit].Data.DiscomMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.DiscomMsgData.Callback = Surf->Discom->Callback;
                MsgTx[WmUnit].Data.DiscomMsgData.CbArg = Surf->Discom->CbArg;
                MsgTx[WmUnit].Data.DiscomMsgData.Crc = Surf->Discom->Crc;
                MsgTx[WmUnit].Data.DiscomMsgData.Enable = Surf->Discom->Enable;
                MsgTx[WmUnit].Data.DiscomMsgData.Height = Surf->Discom->Height;
                MsgTx[WmUnit].Data.DiscomMsgData.Width = Surf->Discom->Width;
                MsgTx[WmUnit].Data.DiscomMsgData.PosX = Surf->Discom->PosX;
                MsgTx[WmUnit].Data.DiscomMsgData.PosY = Surf->Discom->PosY;
                MsgTx[WmUnit].Data.DiscomMsgData.Handle = Surf->Handle;
                break;
            case R_WM_SURF_ALPHA_SET:
                MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURFACE_ALPHA_SET;
                MsgTx[WmUnit].Data.SurfAlphaSetMsgData.Pid = (uint32_t)self_thread_handle;
                MsgTx[WmUnit].Data.SurfAlphaSetMsgData.Alpha = Surf->Alpha;
                MsgTx[WmUnit].Data.SurfAlphaSetMsgData.rpfid = Surf->PosZ;
                MsgTx[WmUnit].Data.SurfAlphaSetMsgData.Handle = Surf->Handle;
                break;
            case R_WM_SURF_PROP_LAST:
            default:
                ret = R_WM_ERR_WRONG_PARAMETER;
                R_PRINT_Log("WM(%d):WmUnit Prop(%d) failed=%d\n", WmUnit, Prop, ret);
                break;
            }
            if (R_WM_ERR_SUCCESS == ret && OSAL_RETURN_OK == osal_ret) {
                ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
                if (R_WM_ERR_SUCCESS != ret) {
                    R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
                }
            }
        }
    }
    return ret;
}

r_wm_Error_t R_WM_SurfaceRelease(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    osal_thread_handle_t    self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_SurfaceRelease: ");
        loc_Error_Handler(ret);
    } else if (0 == Surf) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_SurfaceRelease: ");
        loc_Error_Handler(ret);
    } else {
        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):ThreadSelf=%d\n", WmUnit, osal_ret);
        }

        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            /* Prepare Wm Message and send */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            MsgTx[WmUnit].Dev    = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_SURF_RELEASE;
            MsgTx[WmUnit].Data.SurfReleaseMsgData.Pid = (uint32_t)self_thread_handle;
            MsgTx[WmUnit].Data.SurfReleaseMsgData.Handle = Surf->Handle;

            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
            if (R_WM_ERR_SUCCESS == ret) {
                Surf->Handle = 0;
            } else {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
        }
    }
    return ret;
}

r_wm_Error_t R_WM_DocEnableMonitors(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DocEnableMonitors: ");
        loc_Error_Handler(ret);
    } else if (0 == Surf) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DocEnableMonitors: ");
        loc_Error_Handler(ret);
    } else {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        } else {
            /* Prepare commands and message */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            MsgTx[WmUnit].Dev = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_DOC_ENABLE;
            switch (WmUnit) {
            case R_WM_DEV_VOUT0:
                MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 0;
                MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 0;
                break;
            case R_WM_DEV_VOUT1:
                MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 1;
                MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 0;
                break;
            case R_WM_DEV_VOUT2:
                MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 0;
                MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 1;
                break;
            default: /* R_WM_DEV_VOUT3 */
                MsgTx[WmUnit].Data.DocMsgData.DU_CH  = 1;
                MsgTx[WmUnit].Data.DocMsgData.DOC_CH = 1;
                break;
            }

            MsgTx[WmUnit].Data.DocMsgData.DocMonitors = Surf->Doc->DocMonitors;
            MsgTx[WmUnit].Data.DocMsgData.Enable = Surf->Doc->Enable;
            MsgTx[WmUnit].Data.DocMsgData.ActMonEnable = Surf->Doc->ActMonEnable;
            MsgTx[WmUnit].Data.DocMsgData.ActMonUpperTime = Surf->Doc->ActMonUpperTime;
            MsgTx[WmUnit].Data.DocMsgData.ActMonLowerTime = Surf->Doc->ActMonLowerTime;

            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }

    return ret;
}

r_wm_Error_t R_WM_DocClearStatus(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST <= WmUnit) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DocClearStatus: ");
        loc_Error_Handler(ret);
    } else if (0 == Surf) {
        ret = R_WM_ERR_WRONG_PARAMETER;
        R_PRINT_Log("R_WM_DocClearStatus: ");
        loc_Error_Handler(ret);
    } else {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(loc_WmDevInfo[WmUnit].WmMutex, 3000u);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WM(%d):MutexLock=%d\n",WmUnit, osal_ret);
        } else {
            /* Prepare commands and message */
            (void)memset(&MsgTx[WmUnit], 0, sizeof(r_wm_MsgTx_t));
            (void)memset(&MsgRx[WmUnit], 0, sizeof(r_wm_MsgRx_t));
            MsgTx[WmUnit].Dev = WmUnit;
            MsgTx[WmUnit].MsgCmd = R_WM_MSG_CMD_DOC_CLEAR;

            ret = R_WM_Priv_MsgSendReceive(loc_WmDevInfo[WmUnit].ComCh, &MsgTx[WmUnit], &MsgRx[WmUnit]);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("WM(%d):MsgSendReceive=%d\n", WmUnit, ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(loc_WmDevInfo[WmUnit].WmMutex);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("WM(%d):MutexUnlock=%d\n",WmUnit, osal_ret);
            }
        }
    }

    return ret;
}

