/*************************************************************************************************************
* cio_vin_main_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
  Section: Includes
*/
#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_vin.h"
#include "r_cio_vin_main.h"
#include "r_cio_vin_irq.h"
#include "r_cio_vin_read.h"
#include "r_cio_vin_write.h"
#include "r_cio_vin_status.h"
#include "r_cio_vin_ioctl.h"
#include "r_cio_vin_connect.h"

#include "r_vin_api.h"

#include "r_print_api.h"

#include <string.h>


/*******************************************************************************
  Section: Local defines
*/
#define LOC_MUTEX_CIO_VIN_CH00    (0x8000U + 2U)     /* OSAL_MUTEX_CIOVIN_00 */
#define LOC_MUTEX_CIO_VIN_CH01    (0x8000U + 20U)    /* OSAL_MUTEX_CIOVIN_01 */
#define LOC_MUTEX_CIO_VIN_CH02    (0x8000U + 90U)    /* OSAL_MUTEX_CIO_VIN_CH02 */
#define LOC_MUTEX_CIO_VIN_CH03    (0x8000U + 91U)    /* OSAL_MUTEX_CIO_VIN_CH03 */
#define LOC_MUTEX_CIO_VIN_CH04    (0x8000U + 92U)    /* OSAL_MUTEX_CIO_VIN_CH04 */
#define LOC_MUTEX_CIO_VIN_CH05    (0x8000U + 93U)    /* OSAL_MUTEX_CIO_VIN_CH05 */
#define LOC_MUTEX_CIO_VIN_CH06    (0x8000U + 94U)    /* OSAL_MUTEX_CIO_VIN_CH06 */
#define LOC_MUTEX_CIO_VIN_CH07    (0x8000U + 95U)    /* OSAL_MUTEX_CIO_VIN_CH07 */

/*******************************************************************************
  Section: Local variables
*/
static R_CIO_VIN_PRV_Instance_t locInstance[R_CIO_VIN_MAX_INSTANCE_NUM];


/*******************************************************************************
  Section: Local functions
*/



/*******************************************************************************
  Section: Global functions
*/

/*******************************************************************************
  Function name: R_CIO_VIN_PRV_Init
*/
int R_CIO_VIN_PRV_Init(size_t Addr, int IrqNum)
{
    int ret = R_CIO_VIN_ERR_OK;
    R_CIO_VIN_PRV_Instance_t *pinst;
    osal_mutex_handle_t mtx_handle = OSAL_MUTEX_HANDLE_INVALID;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    R_PRINT_Log("CIO %s driver channel %d init\n", R_CIO_VIN_Driver.Name, Addr);

    pinst = R_CIO_VIN_PRV_FindInstance(Addr);
    if (0 == pinst)
    {
        R_PRINT_Log("[CioVinMain]: R_CIO_VIN_PRV_Init R_CIO_VIN_PRV_FindInstance failed. pinst is 0.\r\n");
        ret = R_CIO_VIN_ERR_FAILED;
    }
    else
    {
        (void)memset(pinst, 0, sizeof(R_CIO_VIN_PRV_Instance_t));
        pinst->VinIdx = Addr;
        pinst->IrqNum = IrqNum;

        switch(pinst->VinIdx)
        {
            case 0:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH00, &mtx_handle);
                break;
            case 1:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH01, &mtx_handle);
                break;
            case 2:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH02, &mtx_handle);
                break;
            case 3:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH03, &mtx_handle);
                break;
            case 4:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH04, &mtx_handle);
                break;
            case 5:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH05, &mtx_handle);
                break;
            case 6:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH06, &mtx_handle);
                break;
            case 7:
                osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_CIO_VIN_CH07, &mtx_handle);
                break;
            default:
                R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_Init. VinIdx(%d) is out of range.\r\n", pinst->VinIdx);
                ret = R_CIO_VIN_ERR_FAILED;
                break;
        }

        if (OSAL_RETURN_OK == osal_ret)
        {
            pinst->mtx_handle = mtx_handle;
        }
        else
        {
            R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_Init R_OSAL_ThsyncMutexCreate failed(%d), VinIdx(%d)\r\n", osal_ret, pinst->VinIdx);
            ret = R_CIO_VIN_ERR_FAILED;
        }
    }

    if (R_CIO_VIN_ERR_OK == ret)
    {
        ret = (int)R_CIO_VIN_PRV_IrqInit(pinst);
        if (R_CIO_VIN_ERR_OK != ret)
        {
            R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_Init R_CIO_VIN_PRV_IrqInit failed(%d)\r\n", ret);
        }
    }

    return ret;
}

/*******************************************************************************
  Function name: R_CIO_VIN_PRV_DeInit
*/
int R_CIO_VIN_PRV_DeInit(size_t Addr)
{
    int ret = R_CIO_VIN_ERR_OK;
    R_CIO_VIN_PRV_Instance_t *pinst;
    int32_t cio_rt = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    pinst = R_CIO_VIN_PRV_FindInstance(Addr);
    if (0 == pinst)
    {
        R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_DeInit R_CIO_VIN_PRV_FindInstance failed. pinst is 0.\r\n");
        ret = R_CIO_VIN_ERR_FAILED;
    }
    else
    {
        osal_ret = R_OSAL_ThsyncMutexDestroy(pinst->mtx_handle);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_DeInit R_OSAL_ThsyncMutexDestroy failed(%d)\r\n", osal_ret);
            ret = R_CIO_VIN_ERR_FAILED;
        }
        cio_rt = (int)R_CIO_VIN_PRV_IrqDeInit(pinst);
        if (R_CIO_VIN_ERR_OK != cio_rt)
        {
            R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_DeInit R_CIO_VIN_PRV_IrqDeInit failed(%d)\r\n", cio_rt);
            ret |= R_CIO_VIN_ERR_FAILED;
        }
    }

    return ret;
}

/*******************************************************************************
  Function name: R_CIO_VIN_PRV_FindInstance
*/
R_CIO_VIN_PRV_Instance_t *R_CIO_VIN_PRV_FindInstance(size_t Addr)
{
    R_CIO_VIN_PRV_Instance_t * pinst;

    if (Addr < R_CIO_VIN_MAX_INSTANCE_NUM)
    {
        pinst = &locInstance[Addr];
    }
    else
    {
        R_PRINT_Log("[CioVinmain]: R_CIO_VIN_PRV_FindInstance Addr is out of range(%d)\r\n", Addr);
        pinst = 0;
    }

    return pinst;
}

/*******************************************************************************
  Function name: R_CIO_VIN_RPV_FindConsumer
*/
int32_t R_CIO_VIN_RPV_FindConsumer(R_CIO_VIN_PRV_Instance_t *pInst, osal_thread_handle_t pThread)
{
    int32_t i = 0;
    osal_thread_handle_t pCurThread;
    osal_thread_handle_t self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t      osal_ret = OSAL_RETURN_OK;
    int32_t idx = R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE;

    if (0 == pThread)
    {
        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[CioVinmain]: R_CIO_VIN_RPV_FindConsumer R_OSAL_ThreadSelf failed(%d)\r\n", osal_ret);
        }
        pCurThread = self_thread_handle;
    }
    else
    {
        pCurThread = pThread;
    }

    if (OSAL_RETURN_OK == osal_ret)
    {
        for (i = 0; i < R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE; i++)
        {
            if (pInst->Consumer[i].ThreadId == pCurThread)
            {
                idx = i;
                break;
            }
        }

        if (R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE == idx)
        {
            R_PRINT_Log("[CioVinmain]: R_CIO_VIN_RPV_FindConsumer ThreadId does not match\r\n");
            idx = -1;
        }
    }

    return idx;
}

/*******************************************************************************
  Section: Global variables
*/
const r_cio_Driver_t R_CIO_VIN_Driver = {
    "R-CAR_VIN",
    R_CIO_VIN_PRV_Init,
    R_CIO_VIN_PRV_DeInit,
    R_CIO_VIN_PRV_Open,
    R_CIO_VIN_PRV_Close,
    R_CIO_VIN_PRV_Read,
    R_CIO_VIN_PRV_Write,
    R_CIO_VIN_PRV_IoCtl,
    R_CIO_VIN_PRV_Status
};

