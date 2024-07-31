/*************************************************************************************************************
* dmac_rtdmac_api_c
* Copyright (c) [2019-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_rtdmac_api.c
 * Version      : 1.0
 * Description  :
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.08.2020 1.00     First Release
 *         : 09.09.2021          Changed OSAL function from CR7_OSAL to ADAS_OSAL.
 *         : 08.11.2021          Delete R_RTDMAC_SetMemory function.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "r_rtdmac_api.h"
#include "rtdmac.h"
#include "rdmac_int.h"
#include "rdmac_reg.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define RTDMAC_CHNL_MAX_NUM            (8)
#define RTDMAC_XFER_MAX_LEN            (64)
#define RTDMAC_XFER_MIN_LEN            (4)

#define round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~round_mask(x, y))

#define RTDMAC_RESOURCE_ID         0x8000U
#define LOC_MUTEX_DRIVER_DMAC_00   (RTDMAC_RESOURCE_ID +  3U) /* OSAL_MUTEX_DRIVER_DMAC_00 */

#define LOC_MQ_RTDMAC00_CH00       (RTDMAC_RESOURCE_ID + 22U) /* OSAL_MQ_RTDMAC0_CH00 */
#define LOC_MQ_RTDMAC00_CH01       (RTDMAC_RESOURCE_ID + 23U) /* OSAL_MQ_RTDMAC0_CH01 */
#define LOC_MQ_RTDMAC00_CH02       (RTDMAC_RESOURCE_ID + 24U) /* OSAL_MQ_RTDMAC0_CH02 */
#define LOC_MQ_RTDMAC00_CH03       (RTDMAC_RESOURCE_ID + 25U) /* OSAL_MQ_RTDMAC0_CH03 */
#define LOC_MQ_RTDMAC00_CH04       (RTDMAC_RESOURCE_ID + 26U) /* OSAL_MQ_RTDMAC0_CH04 */
#define LOC_MQ_RTDMAC00_CH05       (RTDMAC_RESOURCE_ID + 27U) /* OSAL_MQ_RTDMAC0_CH05 */
#define LOC_MQ_RTDMAC00_CH06       (RTDMAC_RESOURCE_ID + 28U) /* OSAL_MQ_RTDMAC0_CH06 */
#define LOC_MQ_RTDMAC00_CH07       (RTDMAC_RESOURCE_ID + 29U) /* OSAL_MQ_RTDMAC0_CH07 */
#define LOC_MQ_RTDMAC01_CH00       (RTDMAC_RESOURCE_ID + 30U) /* OSAL_MQ_RTDMAC1_CH00 */
#define LOC_MQ_RTDMAC01_CH01       (RTDMAC_RESOURCE_ID + 31U) /* OSAL_MQ_RTDMAC1_CH01 */
#define LOC_MQ_RTDMAC01_CH02       (RTDMAC_RESOURCE_ID + 32U) /* OSAL_MQ_RTDMAC1_CH02 */
#define LOC_MQ_RTDMAC01_CH03       (RTDMAC_RESOURCE_ID + 33U) /* OSAL_MQ_RTDMAC1_CH03 */
#define LOC_MQ_RTDMAC01_CH04       (RTDMAC_RESOURCE_ID + 34U) /* OSAL_MQ_RTDMAC1_CH04 */
#define LOC_MQ_RTDMAC01_CH05       (RTDMAC_RESOURCE_ID + 35U) /* OSAL_MQ_RTDMAC1_CH05 */
#define LOC_MQ_RTDMAC01_CH06       (RTDMAC_RESOURCE_ID + 36U) /* OSAL_MQ_RTDMAC1_CH06 */
#define LOC_MQ_RTDMAC01_CH07       (RTDMAC_RESOURCE_ID + 37U) /* OSAL_MQ_RTDMAC1_CH07 */

#define LOC_MQ_RTDMAC_NUM_OF_MSG   1
#define LOC_MQ_RTDMAC_MSG_SIZE     sizeof(uint32_t)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
osal_device_handle_t osal_devhandle_rtdmac0;
osal_device_handle_t osal_devhandle_rtdmac1;

/**********************************************************************************************************************
 Private (static) variables
 *********************************************************************************************************************/
static bool s_init_flag = false;
static osal_mutex_handle_t s_rtdmac_mutex = NULL;
static uint8_t s_channel_use_dmac0 = 0;
static uint8_t s_channel_use_dmac1 = 0;
static R_RTDMAC_Handle_t dmac_intreg[2][RTDMAC_CHNL_MAX_NUM];

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/
static R_RTDMAC_AllocateCh_t allocate_channel(void);

static uint32_t release_channel(R_RTDMAC_AllocateCh_t channel);

static int rtdmac_deviceopen(char *device_type, int device_channel, osal_device_handle_t *device_handle);

static uint32_t rtdmac_Interrupt_create(osal_mq_handle_t* mq_handle, R_RTDMAC_AllocateCh_t alloc_ch);

static uint32_t rtdmac_Interrupt_close(osal_mq_handle_t mq_handle, R_RTDMAC_AllocateCh_t alloc_ch);

/**********************************************************************************************************************
 Local functions
 *********************************************************************************************************************/
static R_RTDMAC_AllocateCh_t allocate_channel(void)
{
    uint8_t         i;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    R_RTDMAC_AllocateCh_t alloc_ch = {RCAR_RT_DMAC_NONE, RTDMAC_CHNL_MAX_NUM};

    osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_rtdmac_mutex, 3000);
    if (OSAL_RETURN_OK == osal_ret)
    {
        if (0xFF > s_channel_use_dmac0)
        {
            /* Use RTDMAC0 */
            alloc_ch.mUseRtdmac = RCAR_RT_DMAC0;
            for (i = 0; i < RTDMAC_CHNL_MAX_NUM; i++)
            {
                if (((s_channel_use_dmac0 >> i) & 1) == 0)
                {
                    break;
                }
            }
        }
        else
        {
            /* Use RTDMAC1 */
            alloc_ch.mUseRtdmac = RCAR_RT_DMAC1;
            for (i = 0; i < RTDMAC_CHNL_MAX_NUM; i++)
            {
                if (((s_channel_use_dmac1 >> i) & 1) == 0)
                {
                    break;
                }
            }
        }

        if (i < RTDMAC_CHNL_MAX_NUM)
        {
            if (RCAR_RT_DMAC0 == alloc_ch.mUseRtdmac)
            {
                s_channel_use_dmac0 |= (1 << i);
            }
            else
            {
                s_channel_use_dmac1 |= (1 << i);
            }
            alloc_ch.mUseCh = i;
        }
        else
        {
            alloc_ch.mUseRtdmac = RCAR_RT_DMAC_NONE;
            R_PRINT_Log("[allocate_channel]: Failed to allocate channel.\n");
        }

        osal_ret = R_OSAL_ThsyncMutexUnlock(s_rtdmac_mutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            alloc_ch.mUseRtdmac = RCAR_RT_DMAC_NONE;
            alloc_ch.mUseCh = RTDMAC_CHNL_MAX_NUM;
            R_PRINT_Log("[allocate_channel]: R_OSAL_ThsyncMutexUnlock() failed(%d).\n", osal_ret);
        }
    }
    else
    {
        R_PRINT_Log("[allocate_channel]: R_OSAL_ThsyncMutexLockForTimePeriod() failed(%d).\n", osal_ret);
    }

    return alloc_ch;
}

static uint32_t release_channel(R_RTDMAC_AllocateCh_t channel)
{
    uint32_t        ret      = drv_OK;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_rtdmac_mutex, 3000);
    if (OSAL_RETURN_OK == osal_ret)
    {
        if (channel.mUseCh < RTDMAC_CHNL_MAX_NUM)
        {
            if (RCAR_RT_DMAC0 == channel.mUseRtdmac)
            {
                s_channel_use_dmac0 &= ~(1 << channel.mUseCh);
            }
            else
            {
                s_channel_use_dmac1 &= ~(1 << channel.mUseCh);
            }
        }
        osal_ret = R_OSAL_ThsyncMutexUnlock(s_rtdmac_mutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[release_channel]: R_OSAL_ThsyncMutexUnlock() Error. osal_ret(%d).\n", osal_ret);
        }
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[release_channel]: R_OSAL_ThsyncMutexLockForTimePeriod() Error. osal_ret(%d).\n", osal_ret);
    }

    return ret;
}

static int rtdmac_deviceopen(char *device_type, int device_channel, osal_device_handle_t *device_handle)
{
    /* local variable */
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int app_ret = 0;
    size_t numOfDevice = 0;
    size_t numOfByte = 0;
    size_t i = 0;
    size_t j = 0;
    osal_device_handle_t local_handle = OSAL_DEVICE_HANDLE_INVALID;
    char devicelist[200];

    if (NULL == *device_handle)
    {
        osal_ret = R_OSAL_IoGetNumOfDevices(device_type, &numOfDevice);
        if(OSAL_RETURN_OK != osal_ret)
        {
            app_ret = -1;
            R_PRINT_Log("[rtdmac_deviceopen]: R_OSAL_IoGetNumOfDevices() Error. osal_ret(%d).\n", osal_ret);
        }
        else
        {
            osal_ret = R_OSAL_IoGetDeviceList(device_type, &devicelist[0], sizeof(devicelist), &numOfByte);
            if(OSAL_RETURN_OK != osal_ret)
            {
                app_ret = -1;
                R_PRINT_Log("[rtdmac_deviceopen]: R_OSAL_IoGetDeviceList() Error. osal_ret(%d).\n", osal_ret);
            }
            else
            {
                char *deviceList[numOfDevice + 1];
                deviceList[0] = &devicelist[0];
                for (i = 0; i < numOfByte; i++)
                {
                    if (devicelist[i] == '\n')
                    {
                        j++;
                        devicelist[i] = '\0';
                        deviceList[j] = &devicelist[i+1];
                    }
                }

                if(j > numOfDevice)
                {
                    app_ret = -1;
                    R_PRINT_Log("[rtdmac_deviceopen]: Device Number Error(%d).\n", j);
                }
                else
                {
                    osal_ret = R_OSAL_IoDeviceOpen(deviceList[device_channel], &local_handle);
                    if(OSAL_RETURN_OK != osal_ret)
                    {
                        app_ret = -1;
                        R_PRINT_Log("[rtdmac_deviceopen]: R_OSAL_IoDeviceOpen() Error. osal_ret(%d).\n", osal_ret);
                    }
                    else
                    {
                        *device_handle = local_handle;
                    }
                }
            }
        }

        if(0 != app_ret)
        {
            *device_handle = OSAL_DEVICE_HANDLE_INVALID;
        }
    }
    return app_ret;
}

static uint32_t rtdmac_Interrupt_create(osal_mq_handle_t* mq_handle, R_RTDMAC_AllocateCh_t alloc_ch)
{
    uint32_t        ret      = drv_OK;
    uint32_t        subch    = alloc_ch.mUseCh;
    R_RTDMAC_Dev_t  devch    = alloc_ch.mUseRtdmac;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    osal_device_handle_t device_handle;
    st_osal_mq_config_t mq_config;

    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
    if(OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[rtdmac_Interrupt_create]: R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
    }
    mq_config.max_num_msg = LOC_MQ_RTDMAC_NUM_OF_MSG;
    mq_config.msg_size    = LOC_MQ_RTDMAC_MSG_SIZE;
    if (RCAR_RT_DMAC0 == devch)
    {
        osal_ret = R_OSAL_MqCreate(&mq_config, (osal_mq_id_t)LOC_MQ_RTDMAC00_CH00+subch, mq_handle);
        if (osal_ret != OSAL_RETURN_OK)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[rtdmac_Interrupt_create]: R_OSAL_MqCreate() Error. devch(%d), subch(%ld), osal_ret(%d), max_num_msg(%lld), msg_size(%lld), mq_id(0x%04X)\r\n",
                    devch, subch, osal_ret, mq_config.max_num_msg, mq_config.msg_size, LOC_MQ_RTDMAC00_CH00+subch);
        }
        device_handle = osal_devhandle_rtdmac0;
    }
    else if (RCAR_RT_DMAC1 == devch)
    {
        osal_ret = R_OSAL_MqCreate(&mq_config, (osal_mq_id_t)LOC_MQ_RTDMAC01_CH00+subch, mq_handle);
        if (osal_ret != OSAL_RETURN_OK)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[rtdmac_Interrupt_create]: R_OSAL_MqCreate() Error. devch(%d), subch(%ld), osal_ret(%d), max_num_msg(%lld), msg_size(%lld), mq_id(0x%04X)\r\n",
                    devch, subch, osal_ret, mq_config.max_num_msg, mq_config.msg_size, LOC_MQ_RTDMAC01_CH00+subch);
        }
        device_handle = osal_devhandle_rtdmac1;
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[rtdmac_Interrupt_create]: Device channel error. devch(%d)\n", devch);
    }

    if (drv_OK == ret)
    {
        dmac_intreg[devch][subch].mUnit     = (devch + DRV_RTDMAC_REG0);
        dmac_intreg[devch][subch].mSubCh    = (uint8_t)subch;
        dmac_intreg[devch][subch].mMqHandle = *mq_handle;

        osal_ret = R_OSAL_InterruptRegisterIsr(device_handle, subch, 0,
                                                (p_osal_isr_func_t)rDmacInterruptHandler,
                                                (void*)(&dmac_intreg[devch][subch]));

        if (OSAL_RETURN_OK == osal_ret)
        {
            osal_ret = R_OSAL_InterruptEnableIsr(device_handle, dmac_intreg[devch][subch].mSubCh);
            if (OSAL_RETURN_OK != osal_ret)
            {
                ret = drv_FAIL;
                R_PRINT_Log("[rtdmac_Interrupt_create]: R_OSAL_InterruptEnableIsr() Error. devch(%d), subch(%ld), osal_ret(%d).\n",
                        devch, subch, osal_ret);
            }
        }
        else
        {
            ret = drv_FAIL;
            R_PRINT_Log("[rtdmac_Interrupt_create]: R_OSAL_InterruptRegisterIsr() Error. devch(%d), subch(%ld), osal_ret(%d).\n",
                    devch, subch, osal_ret);
        }
    }
    return ret;
}

static uint32_t rtdmac_Interrupt_close(osal_mq_handle_t mq_handle, R_RTDMAC_AllocateCh_t alloc_ch)
{
    uint32_t    ret      = drv_OK;
    R_RTDMAC_Dev_t devch = alloc_ch.mUseRtdmac;
    osal_device_handle_t device_handle = NULL;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    if (RCAR_RT_DMAC0 == devch)
    {
        device_handle = osal_devhandle_rtdmac0;
    }
    else if (RCAR_RT_DMAC1 == devch)
    {
        device_handle = osal_devhandle_rtdmac1;
    }
    else
    {
        R_PRINT_Log("[rtdmac_Interrupt_close]: Device channel error. devch(%d).\n", devch);
        ret = drv_FAIL;
    }

    if (NULL != device_handle)
    {
        osal_ret = R_OSAL_InterruptDisableIsr(device_handle, alloc_ch.mUseCh);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[rtdmac_Interrupt_close]: R_OSAL_InterruptDisableIsr() Error. devch(%d) alloc_ch(%d), osal_ret(%d).\n",
                    devch, alloc_ch.mUseCh, osal_ret);
        }

        osal_ret = R_OSAL_InterruptUnregisterIsr(device_handle, alloc_ch.mUseCh,
                                                (p_osal_isr_func_t)rDmacInterruptHandler);

        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[rtdmac_Interrupt_close]: R_OSAL_InterruptUnregisterIsr() Error. devch(%d), alloc_ch(%d), osal_ret(%d).\n",
                    devch, alloc_ch.mUseCh, osal_ret);

        }

        osal_ret = R_OSAL_MqDelete(mq_handle);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[rtdmac_Interrupt_close]: R_OSAL_MqDelete() Error. devch(%d), alloc_ch(%d), osal_ret(%d).\n",
                    devch, alloc_ch.mUseCh, osal_ret);
        }
    }
    return ret;
}

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
uint32_t R_RTDMAC_Initialize(void)
{
    uint32_t        ret      = drv_OK;
    int             dev_ret;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    if (NULL == s_rtdmac_mutex)
    {
        osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_DRIVER_DMAC_00 ,&s_rtdmac_mutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[R_RTDMAC_Initialize]: R_OSAL_ThsyncMutexCreate() Error. osal_ret(%d).\n", osal_ret);
        }
    }

    if (drv_OK == ret)
    {
        do
        {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_rtdmac_mutex, 3000);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[R_RTDMAC_Initialize]: R_OSAL_ThsyncMutexLockForTimePeriod() Error. osal_ret(%d).\n", osal_ret);
        }
        else
        {
            if (false == s_init_flag)
            {
                osal_devhandle_rtdmac0 = 0;
                osal_devhandle_rtdmac1 = 0;

                dev_ret = rtdmac_deviceopen("rtdmac", RCAR_RT_DMAC0, &osal_devhandle_rtdmac0);
                dev_ret |= rtdmac_deviceopen("rtdmac", RCAR_RT_DMAC1, &osal_devhandle_rtdmac1);
                if (0 != dev_ret)
                {
                    R_PRINT_Log("[R_RTDMAC_Initialize]: rtdmac_deviceopen() Error. dev_ret(%d)\n", dev_ret);
                    ret = drv_FAIL;
                }

                s_channel_use_dmac0 = 0;
                s_channel_use_dmac1 = 0;

                ret = R_RTDMAC_Init(RCAR_RT_DMAC0, DRV_DMAC_PRIO_RR);
                ret |= R_RTDMAC_Init(RCAR_RT_DMAC1, DRV_DMAC_PRIO_RR);
                if (drv_OK == ret)
                {
                    s_init_flag = true;
                }
                else
                {
                    R_PRINT_Log("[R_RTDMAC_Initialize]: R_RTDMAC_Init() Error. ret(%d)\n", ret);
                }
            }
            else
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_RTDMAC_Initialize]: Already Initialize.\n");
            }

            osal_ret = R_OSAL_ThsyncMutexUnlock(s_rtdmac_mutex);
            if (OSAL_RETURN_OK != osal_ret)
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_RTDMAC_Initialize]: R_OSAL_ThsyncMutexUnlock() Error. osal_ret(%d).\n", osal_ret);
            }
        }
    }

    return ret;
}

uint32_t R_RTDMAC_DeInitialize(void)
{
    uint32_t        ret      = drv_OK;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    if (true == s_init_flag)
    {
        do
        {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_rtdmac_mutex, 3000);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[R_RTDMAC_DeInitialize]: R_OSAL_ThsyncMutexLockForTimePeriod() Error. osal_ret(%d).\n", osal_ret);
        }

        if (drv_OK == ret)
        {
            R_RTDMAC_Release(RCAR_RT_DMAC0);
            R_RTDMAC_Release(RCAR_RT_DMAC1);

            osal_ret = R_OSAL_IoDeviceClose(osal_devhandle_rtdmac0);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_RTDMAC_DeInitialize]: RTDMAC0 R_OSAL_IoDeviceClose() Error. osal_ret(%d).\n", osal_ret);
            }
            osal_ret = R_OSAL_IoDeviceClose(osal_devhandle_rtdmac1);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_RTDMAC_DeInitialize]: RTDMAC1 R_OSAL_IoDeviceClose() Error. osal_ret(%d).\n", osal_ret);
            }

            osal_devhandle_rtdmac0 = 0;
            osal_devhandle_rtdmac1 = 0;

            s_init_flag = false;

            osal_ret = R_OSAL_ThsyncMutexUnlock(s_rtdmac_mutex);
            if (OSAL_RETURN_OK != osal_ret)
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_RTDMAC_DeInitialize]: R_OSAL_ThsyncMutexUnlock() Error. osal_ret(%d).\n", osal_ret);
            }
            osal_ret = R_OSAL_ThsyncMutexDestroy(s_rtdmac_mutex);
            if (OSAL_RETURN_OK == osal_ret)
            {
                s_rtdmac_mutex = NULL;
            }
            else
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_RTDMAC_DeInitialize]: R_OSAL_ThsyncMutexDestroy() Error. osal_ret(%d).\n", osal_ret);
            }
        }
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[R_RTDMAC_DeInitialize]: It is not initialized.\n");
    }

    return ret;
}

uint32_t R_RTDMAC_CopyMemory(void* dst, void* src, uint32_t len)
{
    uint32_t ret = drv_OK;
    rDmacCfg_t op_ch1, op_ch2;
    uint32_t xfer_cnt_chn1, xfer_cnt_chn2;

    osal_mq_handle_t mq_ch1_handle = NULL;
    osal_mq_handle_t mq_ch2_handle = NULL;
    uint32_t recvdata = drv_OK;

    R_RTDMAC_AllocateCh_t chn1 = {RCAR_RT_DMAC_NONE, RTDMAC_CHNL_MAX_NUM};
    R_RTDMAC_AllocateCh_t chn2 = {RCAR_RT_DMAC_NONE, RTDMAC_CHNL_MAX_NUM};
    uint32_t aligned_len;
    uint32_t remained_bytes;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    aligned_len = round_down(len, RTDMAC_XFER_MIN_LEN);
    remained_bytes = len % RTDMAC_XFER_MIN_LEN;

    if (true == s_init_flag)
    {
        /* need two trannsfers for general case */
        xfer_cnt_chn1 = aligned_len / RTDMAC_XFER_MAX_LEN;
        xfer_cnt_chn2 = (aligned_len % RTDMAC_XFER_MAX_LEN) / RTDMAC_XFER_MIN_LEN;

        /* 1st transfered in DRV_DMAC_TRANS_UNIT_64BYTE */
        if (xfer_cnt_chn1 > 0)
        {
            chn1 = allocate_channel();
            if (chn1.mUseCh < RTDMAC_CHNL_MAX_NUM)
            {
                ret = rtdmac_Interrupt_create(&mq_ch1_handle, chn1);
                if (drv_OK == ret)
                {
                    op_ch1.mDMAMode = DRV_DMAC_DMA_NO_DESCRIPTOR;
                    op_ch1.mSrcAddrMode = DRV_DMAC_ADDR_INCREMENTED;
                    op_ch1.mDestAddrMode = DRV_DMAC_ADDR_INCREMENTED;
                    op_ch1.mTransferUnit = DRV_DMAC_TRANS_UNIT_64BYTE;
                    op_ch1.mResource = DRV_DMAC_MEMORY;
                    op_ch1.mLowSpeed = DRV_DMAC_SPEED_NORMAL;
                    op_ch1.mPrioLevel = 0;
                    op_ch1.mSrcAddr = (uintptr_t)src;
                    op_ch1.mDestAddr = (uintptr_t)dst;
                    op_ch1.mTransferCount = xfer_cnt_chn1;

                    ret = R_RTDMAC_Exec(chn1.mUseRtdmac, chn1.mUseCh, &op_ch1, 0, 0);
                    if (drv_OK != ret)
                    {
                        R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte R_RTDMAC_Exec() Error. ret(%ld)\n", ret);
                        ret = drv_FAIL;
                    }
                }
                else
                {
                    R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte rtdmac_Interrupt_create() Error. ret(%ld).\n", ret);
                    ret = drv_FAIL;
                }
            }
            else
            {
                R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte allocate_channel() Error. chn1.mUseCh(%d)\n", chn1.mUseCh);
                ret = drv_FAIL;
            }
        }

        /* 2nd transfered in DRV_DMAC_TRANS_UNIT_4BYTE */
        if ((xfer_cnt_chn2 > 0) && (drv_OK == ret))
        {
            chn2 = allocate_channel();
            if (chn2.mUseCh < RTDMAC_CHNL_MAX_NUM)
            {
                ret = rtdmac_Interrupt_create(&mq_ch2_handle, chn2);
                if (drv_OK == ret)
                {
                    op_ch2.mDMAMode = DRV_DMAC_DMA_NO_DESCRIPTOR;
                    op_ch2.mSrcAddrMode = DRV_DMAC_ADDR_INCREMENTED;
                    op_ch2.mDestAddrMode = DRV_DMAC_ADDR_INCREMENTED;
                    op_ch2.mTransferUnit = DRV_DMAC_TRANS_UNIT_4BYTE;
                    op_ch2.mResource = DRV_DMAC_MEMORY;
                    op_ch2.mLowSpeed = DRV_DMAC_SPEED_NORMAL;
                    op_ch2.mPrioLevel = 0;
                    op_ch2.mSrcAddr = (uintptr_t)src + RTDMAC_XFER_MAX_LEN * xfer_cnt_chn1;
                    op_ch2.mDestAddr = (uintptr_t)dst + RTDMAC_XFER_MAX_LEN * xfer_cnt_chn1;
                    op_ch2.mTransferCount = xfer_cnt_chn2;

                    ret = R_RTDMAC_Exec(chn2.mUseRtdmac, chn2.mUseCh, &op_ch2, 0, 0);
                    if (drv_OK != ret)
                    {
                        R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte R_RTDMAC_Exec() Error. ret(%ld)\n", ret);
                        ret = drv_FAIL;
                    }
                }
                else
                {
                    R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte rtdmac_Interrupt_create() Error. ret(%ld).\n", ret);
                    ret = drv_FAIL;
                }
            }
            else
            {
                R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte allocate_channel() Error. chn2.mUseCh(%d)\n", chn2.mUseCh);
                ret = drv_FAIL;
            }
        }

        /* wait transfer completed */
        if (drv_OK == ret)
        {
            if(NULL != mq_ch1_handle)
            {
                do
                {
                    osal_ret = R_OSAL_MqReceiveForTimePeriod(mq_ch1_handle,
                                                            RTDMAC_MQ_TIMEOUT,
                                                            (void*)&recvdata,
                                                            LOC_MQ_RTDMAC_MSG_SIZE);
                } while (OSAL_RETURN_TIME == osal_ret);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte R_OSAL_MqReceiveForTimePeriod() Error. osal_ret(%d)\n", osal_ret);
                }

                if (drv_OK != recvdata)
                {
                    R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte Error. recvdata(%ld), osal_ret(%d)\n", recvdata, osal_ret);
                    ret = R_RTDMAC_Stop(chn1.mUseRtdmac, chn1.mUseCh);
                    if (drv_OK != ret)
                    {
                        R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte R_RTDMAC_Stop Error. ret(%ld)\n", ret);
                    }
                    ret = drv_FAIL;
                }
            }

            if (drv_OK == ret)
            {
                if (NULL != mq_ch2_handle)
                {
                    do
                    {
                        osal_ret = R_OSAL_MqReceiveForTimePeriod(mq_ch2_handle,
                                                                RTDMAC_MQ_TIMEOUT,
                                                                (void*)&recvdata,
                                                                LOC_MQ_RTDMAC_MSG_SIZE);
                    } while (OSAL_RETURN_TIME == osal_ret);
                    if (OSAL_RETURN_OK != osal_ret)
                    {
                        R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte R_OSAL_MqReceiveForTimePeriod() Error. osal_ret(%d)\n", osal_ret);
                    }

                    if (drv_OK != recvdata)
                    {
                        R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte Error. recvdata(%ld), osal_ret(%d)\n", recvdata, osal_ret);
                        ret = R_RTDMAC_Stop(chn2.mUseRtdmac, chn2.mUseCh);
                        if (drv_OK != ret)
                        {
                            R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte R_RTDMAC_Stop Error. ret(%ld)\n", ret);
                        }
                        ret = drv_FAIL;
                    }
                }

                if (drv_OK == recvdata)
                {
                    if (remained_bytes > 0)
                    {
                        (void)memcpy(((uint8_t *)dst + len - remained_bytes), ((uint8_t *)src + len - remained_bytes), remained_bytes);
                    }
                }
            }
        }

        /* release resource */
        if (NULL != mq_ch1_handle)
        {
            ret = rtdmac_Interrupt_close(mq_ch1_handle, chn1);
            if (drv_OK != ret)
            {
                R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte rtdmac_Interrupt_close() Error. ret(%ld).\n", ret);
            }
        }
        if (chn1.mUseCh < RTDMAC_CHNL_MAX_NUM)
        {
            ret = release_channel(chn1);
            if (drv_OK != ret)
            {
                R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans64byte release_channel() Error. ret(%ld).\n", ret);
            }
        }
        if (NULL != mq_ch2_handle)
        {
            ret = rtdmac_Interrupt_close(mq_ch2_handle, chn2);
            if (drv_OK != ret)
            {
                R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte rtdmac_Interrupt_close() Error. ret(%ld).\n", ret);
            }
        }
        if (chn2.mUseCh < RTDMAC_CHNL_MAX_NUM)
        {
            ret = release_channel(chn2);
            if (drv_OK != ret)
            {
                R_PRINT_Log("[R_RTDMAC_CopyMemory]: Trans4byte release_channel() Error. ret(%ld).\n", ret);
            }
        }
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[R_RTDMAC_CopyMemory]: It is not initialized.\n");
    }
    return ret;
}
