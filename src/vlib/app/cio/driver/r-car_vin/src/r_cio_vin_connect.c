/*************************************************************************************************************
* cio_vin_connect_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
  Section: Includes
*/
#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_vin_main.h"
#include "r_cio_vin_connect.h"
#include "r_cio_vin_public.h"

#include "r_print_api.h"

#include <string.h>


/*******************************************************************************
  Section: Global functions
*/

/*******************************************************************************
  Function name: VIN_PRV_Open
*/
int R_CIO_VIN_PRV_Open(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_CIO_VIN_PRV_Instance_t * pInst;
    int ret = R_CIO_VIN_ERR_FAILED;
    int32_t i = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_thread_handle_t self_thread_handle = OSAL_THREAD_HANDLE_INVALID;

    pInst = R_CIO_VIN_PRV_FindInstance(Addr);
    if (pInst != 0)
    {
        if (pInst->ConsumerCnt < R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE)
        {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pInst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS );
            if(OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Open R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n",
                            osal_ret);
                ret = R_CIO_VIN_ERR_FAILED;
            }
            else
            {
                for (i = 0; i < R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE; i++)
                {
                    if (pInst->Consumer[i].ThreadId == 0)
                    {
                        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
                        if (OSAL_RETURN_OK != osal_ret)
                        {
                            R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Open R_OSAL_ThreadSelf failed(%d)\r\n",
                                        osal_ret);
                            break;
                        }
                        pInst->Consumer[i].ThreadId = self_thread_handle;
                        pInst->ConsumerCnt++;
                        ret = R_CIO_VIN_ERR_OK;
                        break;
                    }
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(pInst->mtx_handle);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Open Open R_OSAL_ThsyncMutexUnlock failed(%d)\r\n",
                                osal_ret);
                }
            }
        }
        else
        {
            R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Open ConsumerCnt over(%d)\r\n",
                        pInst->ConsumerCnt);
        }
    }
    else
    {
        R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Open R_CIO_VIN_PRV_FindInstance failed. pinst is 0.\r\n");
    }

    return ret;
}

/*******************************************************************************
  Function name: VIN_PRV_Close
*/
int R_CIO_VIN_PRV_Close(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_CIO_VIN_PRV_Instance_t * pInst;
    int ret = R_CIO_VIN_ERR_FAILED;
    int32_t i = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_thread_handle_t self_thread_handle = OSAL_THREAD_HANDLE_INVALID;

    pInst = R_CIO_VIN_PRV_FindInstance(Addr);
    if (pInst != 0)
    {
        if (pInst->ConsumerCnt > 0)
        {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pInst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS );
            if(OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Close R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                ret = R_CIO_VIN_ERR_FAILED;
            }
            else
            {
                for (i = 0; i < R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE; i++)
                {
                    osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
                    if (OSAL_RETURN_OK != osal_ret)
                    {
                        R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Close R_OSAL_ThreadSelf failed(%d)\r\n", osal_ret);
                        break;
                    }
                    if (pInst->Consumer[i].ThreadId == self_thread_handle)
                    {
                        (void)memset(&pInst->Consumer[i], 0, sizeof(R_CIO_VIN_RPV_Consumer_t));
                        pInst->ConsumerCnt--;
                        ret = R_CIO_VIN_ERR_OK;
                        break;
                    }
                }

                if (pInst->ConsumerCnt == 0)
                {
                    pInst->State = R_CIO_VIN_STATE_UNINIT;
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(pInst->mtx_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinConnect]: R_CIO_VIN_PRV_Close R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                }
            }
        }
        else
        {
            R_PRINT_Log("[CioVinClose]:R_CIO_VIN_PRV_Close ConsumerCnt is 0.\r\n");
        }
    }
    else
    {
        R_PRINT_Log("[CioVinClose]:R_CIO_VIN_PRV_Close R_CIO_VIN_PRV_FindInstance failed. pinst is 0.\r\n");
    }
    return ret;
}

