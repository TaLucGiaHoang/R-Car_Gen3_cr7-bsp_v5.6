/*************************************************************************************************************
* cio_ether_main_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
   Section: Includes
 */

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_ether_public.h"
#include "r_cio_ether_main.h"


#include "r_cio_ether_irq.h"
#include "r_cio_ether_read.h"
#include "r_cio_ether_write.h"
#include "r_cio_ether_status.h"
#include "r_cio_ether_ioctl.h"
#include "r_cio_ether_public.h"
#include "r_cio_ether_connect.h"

#include "ether_api.h"
#include "r_print_api.h"

/*******************************************************************************
   Section: Local defines
 */
#define ETHER_RX_MQ_ID    (0x8000U + 66U)
#define ETHER_MQ_NUM_OF_MSG    1
#define ETHER_MQ_MSG_SIZE      sizeof(uint32_t)

/*******************************************************************************
   Section: Global Variables
 */
osal_device_handle_t Ether_device_handle;
osal_mutex_handle_t  Ether_mutex_handle;

osal_mq_handle_t Ether_mq_handle_rx;

/*******************************************************************************
   Section: Local types
 */
#define ETHER_CIO_RESOURCE_ID 0x8000u
#define ETHER_MUTEX_ID (ETHER_CIO_RESOURCE_ID + 97U)
/*******************************************************************************
   Section: Local Variables
 */

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_Init
 */
/**
 * @brief ETHER internal initialization
 *
 * @param[in] Addr       - Channel id of the given ETHER instance (unused)
 * @param[in] IrqNum     - Interrupt ID (unused)
 *
 * @return == 0 - OK
 * @return != 0 - NG
 */
static int R_CIO_ETHER_PRV_Init(size_t Addr, int IrqNum)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int ret = -1;
    size_t numOfByte = 0;
    char devicelist[32];
    e_osal_pm_required_state_t osal_state;

    st_osal_mq_config_t Ether_Rx_mq_config;

    /* Open device */
    osal_ret = R_OSAL_IoGetDeviceList("ethernet", &devicelist[0], sizeof(devicelist), &numOfByte);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_IoGetDeviceList failed(%d)\r\n", osal_ret);
    }
    else
    {
        osal_ret = R_OSAL_IoDeviceOpen(&devicelist[0], &Ether_device_handle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_IoDeviceOpen failed(%d)\r\n", osal_ret);
        }
        else
        {
            /* Enable supply clock for ETH */
            osal_ret = R_OSAL_PmSetRequiredState(Ether_device_handle, OSAL_PM_REQUIRED_STATE_REQUIRED, false);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
            }
            else
            {
                /* Wait ETH enabled */
                do
                {
                    R_OSAL_ThreadSleepForTimePeriod(1);
                    osal_ret = R_OSAL_PmGetRequiredState(Ether_device_handle, &osal_state);
                    if (OSAL_RETURN_OK != osal_ret)
                    {
                        R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                    }
                } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);

                ret = 0;
            }
        }
    }
    if (0 == ret)
    {
        osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)ETHER_MUTEX_ID, &Ether_mutex_handle); /* Protecting Critical Sessions */
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = -1;
            R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_ThsyncMutexCreate failed(%d)\r\n", osal_ret);
        }
        else
        {
            /* Create Mq for ether interrupt */
            osal_ret = R_OSAL_MqInitializeMqConfigSt(&Ether_Rx_mq_config);
            if(OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
                ret = -1;
            }
            Ether_Rx_mq_config.max_num_msg = ETHER_MQ_NUM_OF_MSG;
            Ether_Rx_mq_config.msg_size    = ETHER_MQ_MSG_SIZE;
            osal_ret = R_OSAL_MqCreate(&Ether_Rx_mq_config, (osal_mq_id_t)ETHER_RX_MQ_ID, &Ether_mq_handle_rx);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_CIO_ETHER_PRV_Init] R_OSAL_MqCreate fail(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                            osal_ret, Ether_Rx_mq_config.max_num_msg, Ether_Rx_mq_config.msg_size, (osal_mq_id_t)ETHER_RX_MQ_ID);
            }

            /* Initialize Ethernet driver */
            Eth_Init(EthConfigSet0);
        }
    }

    return ret;
}

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_DeInit
 */
/**
 * @brief ETHER internal de-initialization
 *
 * @param[in] Addr       - Channel id of the given ETHER instance (unused)
 *
 * @return == 0 - OK
 * @return != 0 - NG
 */
static int R_CIO_ETHER_PRV_DeInit(size_t Addr)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int ret = -1;

    /* Mq delete */
    osal_ret = R_OSAL_MqDelete(Ether_mq_handle_rx);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[R_CIO_ETHER_PRV_DeInit] R_OSAL_MqDelete error. return value = %d\r\n", osal_ret);
    }
    else
    {
        osal_ret = R_OSAL_ThsyncMutexDestroy(Ether_mutex_handle);
        if (OSAL_RETURN_OK == osal_ret)
        {
            /* Close device */
            osal_ret = R_OSAL_IoDeviceClose(Ether_device_handle);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[ETH Sample App] R_OSAL_IoDeviceClose failed(%d)\r\n", osal_ret);
            }
            else
            {
                /* After called R_OSAL_IoDeviceClose,  EAVB-IF module will be stopped */
                ret = 0;
            }
        }
        else
        {
            R_PRINT_Log("[CioEtherMain]: R_CIO_ETHER_PRV_DeInit R_OSAL_ThsyncMutexDestroy failed(%d)\r\n", osal_ret);
        }
    }
    return ret;
}
/*******************************************************************************
   Section: Global variables
 */

const r_cio_Driver_t R_CIO_ETHER_Driver = {
    "R-CAR_cio_ether",
    R_CIO_ETHER_PRV_Init,
    R_CIO_ETHER_PRV_DeInit,
    R_CIO_ETHER_PRV_Open,
    R_CIO_ETHER_PRV_Close,
    R_CIO_ETHER_PRV_Read,
    R_CIO_ETHER_PRV_Write,
    R_CIO_ETHER_PRV_IoCtl,
    R_CIO_ETHER_PRV_Status
};


