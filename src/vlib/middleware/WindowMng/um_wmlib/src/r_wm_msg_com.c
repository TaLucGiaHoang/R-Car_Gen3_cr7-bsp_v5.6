/*************************************************************************************************************
* WindowMng_wm_msg_com_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "rcar-xos/osal/r_osal.h"
#include "r_wm_bridge.h"
#include "r_wm_api.h"
#include "r_wm_msg_com.h"
#include "r_cio_api.h"
#include "r_print_api.h"

/*
 *  Section Macro definitions
 */
#define LOC_OPEN_TIMEOUT 100

/*
 *  Section local Variables
 */

/**
 * This variable keeps track of the number of opened CIO channel
 * It is used to initialised and de-initialised the CIO correctly.
 */
static uint32_t loc_OpenChannelCount = 0;

/**
 * Predefinded VOUT names.
 */
static const char *r_wm_vout_name[R_WM_DEV_LAST] = {"RCar-WM:0", "RCar-WM:1", "RCar-WM:2", "RCar-WM:3"};

/*
 * Section Global Functions
 */

r_wm_Error_t R_WM_Priv_MsgComOpen(r_wm_Dev_t WmUnit, int32_t *ComCh)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    const char  *name = NULL;
    uint32_t     loop_cnt = LOC_OPEN_TIMEOUT;

    if (WmUnit < R_WM_DEV_VOUT0 || WmUnit > R_WM_DEV_VOUT3) {
        ret = R_WM_ERR_OPENCOM_FAILED;
        R_PRINT_Log("Dev(%d) is out of range [%d, %d]!", WmUnit, R_WM_DEV_VOUT0, R_WM_DEV_VOUT3);
    }
    else {
        name = r_wm_vout_name[WmUnit];

        /* Open communication for the Wm Device */
        *ComCh = R_CIO_Open(name, "rwb");

        while ((-2 == *ComCh) && (0 < loop_cnt)) {
            R_OSAL_ThreadSleepForTimePeriod(50);
            /* When Mutex lock error, Retry */
            *ComCh = R_CIO_Open(name, "rwb");
            loop_cnt--;
        }
        if (-1 == *ComCh) {
            ret = R_WM_ERR_OPENCOM_FAILED;
            R_PRINT_Log("R_WM_Priv_MsgComOpen ComCh(%d) failed(%d)\n", *ComCh, ret);
        }
        else {
            /* incrment the number of opened channels */
            loc_OpenChannelCount++;
        }
    }
    return ret;
}

r_wm_Error_t R_WM_Priv_MsgComClose(int32_t ComCh)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    int32_t      err;

    /* Initialise CIO for the current process */
    err = R_CIO_Close(ComCh);
    if (0 != err) {
        ret = R_WM_ERR_CLOSECOM_FAILED;
        R_PRINT_Log("R_CIO_Close failed=%d\n",ret);
    } else {
        /* If no channels are in used then Deinit the CIO */
        loc_OpenChannelCount--;
    }

     return ret;
}

r_wm_Error_t R_WM_Priv_MsgSendReceive(int32_t ComCh, r_wm_MsgTx_t *MsgTx,
                r_wm_MsgRx_t * MsgRx)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    size_t       err = -1;
    osal_thread_handle_t self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t      osal_ret           = OSAL_RETURN_OK;

    osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_WM_Priv_MsgSendReceive R_OSAL_ThreadSelf failed(%d)\n" , osal_ret);
    }

    if (-1 == ComCh) {
        ret = R_WM_ERR_OPENCOM_FAILED;
        R_PRINT_Log("R_WM_Priv_MsgSendReceive ComCh(%d) failed(%d)\n" , ComCh, ret);
    } else {
        if (0 == MsgTx) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("R_WM_Priv_MsgSendReceive MsgTx(%d), ret(%d)\n", MsgTx, ret);
        } else {
            err = R_CIO_IoCtl(ComCh, MsgTx, MsgRx, NULL);
            if (-1 == err) {
                ret = R_WM_ERR_MSGIOCTL_FAILED;
                R_PRINT_Log("R_CIO_IoCtl failed(%d)\n",ret);
            } else {
                ret = MsgRx->Err;
            }
        }
    }
    return ret;
}
