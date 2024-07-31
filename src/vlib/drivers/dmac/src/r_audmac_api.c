/*************************************************************************************************************
* dmac_audmac_api_c
* Copyright (c) [2022-2023] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_audmac_api.c
 * Version      : 1.0
 * Description  :
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "r_audmac_api.h"
#include "audmac.h"
#include "rdmac_int.h"
#include "rdmac_reg.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define AUDMAC_CHNL_MAX_NUM            (8)
#define AUDMAC_XFER_MAX_LEN            (64)
#define AUDMAC_XFER_MIN_LEN            (4)

#define round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~round_mask(x, y))

#define AUDMAC_RESOURCE_ID         0x8000U
#define LOC_MUTEX_DRIVER_DMAC_00   (AUDMAC_RESOURCE_ID +  101U) /* OSAL_MUTEX_DRIVER_AUDMAC_00 */

#define LOC_MQ_AUDMAC00_CH00       (AUDMAC_RESOURCE_ID + 68U) /* OSAL_MQ_AUDMAC0_CH00 */
#define LOC_MQ_AUDMAC00_CH01       (AUDMAC_RESOURCE_ID + 69U) /* OSAL_MQ_AUDMAC0_CH01 */
#define LOC_MQ_AUDMAC00_CH02       (AUDMAC_RESOURCE_ID + 70U) /* OSAL_MQ_AUDMAC0_CH02 */
#define LOC_MQ_AUDMAC00_CH03       (AUDMAC_RESOURCE_ID + 71U) /* OSAL_MQ_AUDMAC0_CH03 */
#define LOC_MQ_AUDMAC00_CH04       (AUDMAC_RESOURCE_ID + 72U) /* OSAL_MQ_AUDMAC0_CH04 */
#define LOC_MQ_AUDMAC00_CH05       (AUDMAC_RESOURCE_ID + 73U) /* OSAL_MQ_AUDMAC0_CH05 */
#define LOC_MQ_AUDMAC00_CH06       (AUDMAC_RESOURCE_ID + 74U) /* OSAL_MQ_AUDMAC0_CH06 */
#define LOC_MQ_AUDMAC00_CH07       (AUDMAC_RESOURCE_ID + 75U) /* OSAL_MQ_AUDMAC0_CH07 */
#define LOC_MQ_AUDMAC00_CH08       (AUDMAC_RESOURCE_ID + 76U) /* OSAL_MQ_AUDMAC0_CH08 */
#define LOC_MQ_AUDMAC00_CH09       (AUDMAC_RESOURCE_ID + 77U) /* OSAL_MQ_AUDMAC0_CH09 */
#define LOC_MQ_AUDMAC01_CH00       (AUDMAC_RESOURCE_ID + 78U) /* OSAL_MQ_AUDMAC1_CH00 */
#define LOC_MQ_AUDMAC01_CH01       (AUDMAC_RESOURCE_ID + 79U) /* OSAL_MQ_AUDMAC1_CH01 */
#define LOC_MQ_AUDMAC01_CH02       (AUDMAC_RESOURCE_ID + 80U) /* OSAL_MQ_AUDMAC1_CH02 */
#define LOC_MQ_AUDMAC01_CH03       (AUDMAC_RESOURCE_ID + 81U) /* OSAL_MQ_AUDMAC1_CH03 */
#define LOC_MQ_AUDMAC01_CH04       (AUDMAC_RESOURCE_ID + 82U) /* OSAL_MQ_AUDMAC1_CH04 */
#define LOC_MQ_AUDMAC01_CH05       (AUDMAC_RESOURCE_ID + 83U) /* OSAL_MQ_AUDMAC1_CH05 */
#define LOC_MQ_AUDMAC01_CH06       (AUDMAC_RESOURCE_ID + 84U) /* OSAL_MQ_AUDMAC1_CH06 */
#define LOC_MQ_AUDMAC01_CH07       (AUDMAC_RESOURCE_ID + 85U) /* OSAL_MQ_AUDMAC1_CH07 */
#define LOC_MQ_AUDMAC01_CH08       (AUDMAC_RESOURCE_ID + 86U) /* OSAL_MQ_AUDMAC1_CH08 */
#define LOC_MQ_AUDMAC01_CH09       (AUDMAC_RESOURCE_ID + 87U) /* OSAL_MQ_AUDMAC1_CH09 */

#define LOC_MQ_AUDMAC_NUM_OF_MSG   1
#define LOC_MQ_AUDMAC_MSG_SIZE     sizeof(uint32_t)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
osal_device_handle_t osal_devhandle_audmac0;
osal_device_handle_t osal_devhandle_audmac1;

/**********************************************************************************************************************
 Private (static) variables
 *********************************************************************************************************************/
static bool s_init_flag = false;
static osal_mutex_handle_t s_audmac_mutex = NULL;
static uint8_t s_channel_use_dmac0 = 0;
static uint8_t s_channel_use_dmac1 = 0;
static R_AUDMAC_Handle_t dmac_intreg[2][AUDMAC_CHNL_MAX_NUM];

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/
static R_AUDMAC_AllocateCh_t allocate_channel(void);

static uint32_t release_channel(R_AUDMAC_AllocateCh_t channel);

static int audmac_deviceopen(char *device_type, int device_channel, osal_device_handle_t *device_handle);

static uint32_t audmac_Interrupt_create(osal_mq_handle_t* mq_handle, R_AUDMAC_AllocateCh_t alloc_ch);

static uint32_t audmac_Interrupt_close(osal_mq_handle_t mq_handle, R_AUDMAC_AllocateCh_t alloc_ch);

/**********************************************************************************************************************
 Local functions
 *********************************************************************************************************************/
static R_AUDMAC_AllocateCh_t allocate_channel(void)
{
    uint8_t         i;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    R_AUDMAC_AllocateCh_t alloc_ch = {RCAR_AUDIO_DMAC_NONE, AUDMAC_CHNL_MAX_NUM};

    osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_audmac_mutex, 3000);
    if (OSAL_RETURN_OK == osal_ret)
    {
        if (0xFF > s_channel_use_dmac0)
        {
            /* Use AUDMAC0 */
            alloc_ch.mUseAudmac = RCAR_AUDIO_DMAC0;
            for (i = 0; i < AUDMAC_CHNL_MAX_NUM; i++)
            {
                if (((s_channel_use_dmac0 >> i) & 1) == 0)
                {
                    break;
                }
            }
        }
        else
        {
            /* Use AUDMAC1 */
            alloc_ch.mUseAudmac = RCAR_AUDIO_DMAC1;
            for (i = 0; i < AUDMAC_CHNL_MAX_NUM; i++)
            {
                if (((s_channel_use_dmac1 >> i) & 1) == 0)
                {
                    break;
                }
            }
        }

        if (i < AUDMAC_CHNL_MAX_NUM)
        {
            if (RCAR_AUDIO_DMAC0 == alloc_ch.mUseAudmac)
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
            alloc_ch.mUseAudmac = RCAR_AUDIO_DMAC_NONE;
            R_PRINT_Log("[allocate_channel]: Failed to allocate channel.\n");
        }

        osal_ret = R_OSAL_ThsyncMutexUnlock(s_audmac_mutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            alloc_ch.mUseAudmac = RCAR_AUDIO_DMAC_NONE;
            alloc_ch.mUseCh = AUDMAC_CHNL_MAX_NUM;
            R_PRINT_Log("[allocate_channel]: R_OSAL_ThsyncMutexUnlock() failed(%d).\n", osal_ret);
        }
    }
    else
    {
        R_PRINT_Log("[allocate_channel]: R_OSAL_ThsyncMutexLockForTimePeriod() failed(%d).\n", osal_ret);
    }

    return alloc_ch;
}

static uint32_t release_channel(R_AUDMAC_AllocateCh_t channel)
{
    uint32_t        ret      = drv_OK;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_audmac_mutex, 3000);
    if (OSAL_RETURN_OK == osal_ret)
    {
        if (channel.mUseCh < AUDMAC_CHNL_MAX_NUM)
        {
            if (RCAR_AUDIO_DMAC0 == channel.mUseAudmac)
            {
                s_channel_use_dmac0 &= ~(1 << channel.mUseCh);
            }
            else
            {
                s_channel_use_dmac1 &= ~(1 << channel.mUseCh);
            }
        }
        osal_ret = R_OSAL_ThsyncMutexUnlock(s_audmac_mutex);
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

static int audmac_deviceopen(char *device_type, int device_channel, osal_device_handle_t *device_handle)
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
            R_PRINT_Log("[audmac_deviceopen]: R_OSAL_IoGetNumOfDevices() Error. osal_ret(%d).\n", osal_ret);
        }
        else
        {
            osal_ret = R_OSAL_IoGetDeviceList(device_type, &devicelist[0], sizeof(devicelist), &numOfByte);
            if(OSAL_RETURN_OK != osal_ret)
            {
                app_ret = -1;
                R_PRINT_Log("[audmac_deviceopen]: R_OSAL_IoGetDeviceList() Error. osal_ret(%d).\n", osal_ret);
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
                    R_PRINT_Log("[audmac_deviceopen]: Device Number Error(%d).\n", j);
                }
                else
                {
                    osal_ret = R_OSAL_IoDeviceOpen(deviceList[device_channel], &local_handle);
                    if(OSAL_RETURN_OK != osal_ret)
                    {
                        app_ret = -1;
                        R_PRINT_Log("[audmac_deviceopen]: R_OSAL_IoDeviceOpen() Error. osal_ret(%d).\n", osal_ret);
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

static uint32_t audmac_Interrupt_create(osal_mq_handle_t* mq_handle, R_AUDMAC_AllocateCh_t alloc_ch)
{
    uint32_t        ret      = drv_OK;
    uint32_t        subch    = alloc_ch.mUseCh;
    R_AUDMAC_Dev_t  devch    = alloc_ch.mUseAudmac;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    osal_device_handle_t device_handle;
    st_osal_mq_config_t mq_config;

    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
    if(OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[audmac_Interrupt_create]: R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
    }
    mq_config.max_num_msg = LOC_MQ_AUDMAC_NUM_OF_MSG;
    mq_config.msg_size    = LOC_MQ_AUDMAC_MSG_SIZE;
    if (RCAR_AUDIO_DMAC0 == devch)
    {
        osal_ret = R_OSAL_MqCreate(&mq_config, (osal_mq_id_t)LOC_MQ_AUDMAC00_CH00+subch, mq_handle);
        if (osal_ret != OSAL_RETURN_OK)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[audmac_Interrupt_create]: R_OSAL_MqCreate() Error. devch(%d), subch(%ld), osal_ret(%d), max_num_msg(%lld), msg_size(%lld), mq_id(0x%04X)\r\n",
                    devch, subch, osal_ret, mq_config.max_num_msg, mq_config.msg_size, LOC_MQ_AUDMAC00_CH00+subch);
        }
        device_handle = osal_devhandle_audmac0;
    }
    else if (RCAR_AUDIO_DMAC1 == devch)
    {
        osal_ret = R_OSAL_MqCreate(&mq_config, (osal_mq_id_t)LOC_MQ_AUDMAC01_CH00+subch, mq_handle);
        if (osal_ret != OSAL_RETURN_OK)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[audmac_Interrupt_create]: R_OSAL_MqCreate() Error. devch(%d), subch(%ld), osal_ret(%d), max_num_msg(%lld), msg_size(%lld), mq_id(0x%04X)\r\n",
                    devch, subch, osal_ret, mq_config.max_num_msg, mq_config.msg_size, LOC_MQ_AUDMAC01_CH00+subch);
        }
        device_handle = osal_devhandle_audmac1;
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[audmac_Interrupt_create]: Device channel error. devch(%d)\n", devch);
    }

    if (drv_OK == ret)
    {
        dmac_intreg[devch][subch].mUnit     = (devch + DRV_AUDMAC_REG0);
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
                R_PRINT_Log("[audmac_Interrupt_create]: R_OSAL_InterruptEnableIsr() Error. devch(%d), subch(%ld), osal_ret(%d).\n",
                        devch, subch, osal_ret);
            }
        }
        else
        {
            ret = drv_FAIL;
            R_PRINT_Log("[audmac_Interrupt_create]: R_OSAL_InterruptRegisterIsr() Error. devch(%d), subch(%ld), osal_ret(%d).\n",
                    devch, subch, osal_ret);
        }
    }
    return ret;
}

static uint32_t audmac_Interrupt_close(osal_mq_handle_t mq_handle, R_AUDMAC_AllocateCh_t alloc_ch)
{
    uint32_t    ret      = drv_OK;
    R_AUDMAC_Dev_t devch = alloc_ch.mUseAudmac;
    osal_device_handle_t device_handle = NULL;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    if (RCAR_AUDIO_DMAC0 == devch)
    {
        device_handle = osal_devhandle_audmac0;
    }
    else if (RCAR_AUDIO_DMAC1 == devch)
    {
        device_handle = osal_devhandle_audmac1;
    }
    else
    {
        R_PRINT_Log("[audmac_Interrupt_close]: Device channel error. devch(%d).\n", devch);
        ret = drv_FAIL;
    }

    if (NULL != device_handle)
    {
        osal_ret = R_OSAL_InterruptDisableIsr(device_handle, alloc_ch.mUseCh);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[audmac_Interrupt_close]: R_OSAL_InterruptDisableIsr() Error. devch(%d) alloc_ch(%d), osal_ret(%d).\n",
                    devch, alloc_ch.mUseCh, osal_ret);
        }

        osal_ret = R_OSAL_InterruptUnregisterIsr(device_handle, alloc_ch.mUseCh,
                                                (p_osal_isr_func_t)rDmacInterruptHandler);

        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[audmac_Interrupt_close]: R_OSAL_InterruptUnregisterIsr() Error. devch(%d), alloc_ch(%d), osal_ret(%d).\n",
                    devch, alloc_ch.mUseCh, osal_ret);

        }

        osal_ret = R_OSAL_MqDelete(mq_handle);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[audmac_Interrupt_close]: R_OSAL_MqDelete() Error. devch(%d), alloc_ch(%d), osal_ret(%d).\n",
                    devch, alloc_ch.mUseCh, osal_ret);
        }
    }
    return ret;
}

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
uint32_t R_AUDMAC_Initialize(void)
{
    uint32_t        ret      = drv_OK;
    int             dev_ret;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    if (NULL == s_audmac_mutex)
    {
        osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_DRIVER_DMAC_00 ,&s_audmac_mutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[R_AUDMAC_Initialize]: R_OSAL_ThsyncMutexCreate() Error. osal_ret(%d).\n", osal_ret);
        }
    }

    if (drv_OK == ret)
    {
        do
        {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_audmac_mutex, 3000);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[R_AUDMAC_Initialize]: R_OSAL_ThsyncMutexLockForTimePeriod() Error. osal_ret(%d).\n", osal_ret);
        }
        else
        {
            if (false == s_init_flag)
            {
                osal_devhandle_audmac0 = 0;
                osal_devhandle_audmac1 = 0;

                dev_ret = audmac_deviceopen("audmac", RCAR_AUDIO_DMAC0, &osal_devhandle_audmac0);
                dev_ret |= audmac_deviceopen("audmac", RCAR_AUDIO_DMAC1, &osal_devhandle_audmac1);
                if (0 != dev_ret)
                {
                    R_PRINT_Log("[R_AUDMAC_Initialize]: audmac_deviceopen() Error. dev_ret(%d)\n", dev_ret);
                    ret = drv_FAIL;
                }

                s_channel_use_dmac0 = 0;
                s_channel_use_dmac1 = 0;
                s_init_flag = true;
            }
            else
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_AUDMAC_Initialize]: Already Initialize.\n");
            }

            osal_ret = R_OSAL_ThsyncMutexUnlock(s_audmac_mutex);
            if (OSAL_RETURN_OK != osal_ret)
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_AUDMAC_Initialize]: R_OSAL_ThsyncMutexUnlock() Error. osal_ret(%d).\n", osal_ret);
            }
        }
    }

    return ret;
}

uint32_t R_AUDMAC_DeInitialize(void)
{
    uint32_t        ret      = drv_OK;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    if (true == s_init_flag)
    {
        do
        {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(s_audmac_mutex, 3000);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = drv_FAIL;
            R_PRINT_Log("[R_AUDMAC_DeInitialize]: R_OSAL_ThsyncMutexLockForTimePeriod() Error. osal_ret(%d).\n", osal_ret);
        }

        if (drv_OK == ret)
        {
            R_AUDMAC_Release(RCAR_AUDIO_DMAC0);
            R_AUDMAC_Release(RCAR_AUDIO_DMAC1);

            osal_ret = R_OSAL_IoDeviceClose(osal_devhandle_audmac0);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_AUDMAC_DeInitialize]: AUDMAC0 R_OSAL_IoDeviceClose() Error. osal_ret(%d).\n", osal_ret);
            }
            osal_ret = R_OSAL_IoDeviceClose(osal_devhandle_audmac1);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[R_AUDMAC_DeInitialize]: AUDMAC1 R_OSAL_IoDeviceClose() Error. osal_ret(%d).\n", osal_ret);
            }

            osal_devhandle_audmac0 = 0;
            osal_devhandle_audmac1 = 0;

            s_init_flag = false;

            osal_ret = R_OSAL_ThsyncMutexUnlock(s_audmac_mutex);
            if (OSAL_RETURN_OK != osal_ret)
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_AUDMAC_DeInitialize]: R_OSAL_ThsyncMutexUnlock() Error. osal_ret(%d).\n", osal_ret);
            }
            osal_ret = R_OSAL_ThsyncMutexDestroy(s_audmac_mutex);
            if (OSAL_RETURN_OK == osal_ret)
            {
                s_audmac_mutex = NULL;
            }
            else
            {
                ret = drv_FAIL;
                R_PRINT_Log("[R_AUDMAC_DeInitialize]: R_OSAL_ThsyncMutexDestroy() Error. osal_ret(%d).\n", osal_ret);
            }
        }
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[R_AUDMAC_DeInitialize]: It is not initialized.\n");
    }

    return ret;
}

uint32_t R_AUDMAC_CopyMemory(void* dst, void* src, uint32_t len, rDmacResource_t resource, uintptr_t arg)
{
    uint32_t ret = drv_OK;
    rDmacCfg_t op_ch1;
    uint32_t xfer_cnt_chn1;

    osal_mq_handle_t mq_ch1_handle = NULL;
    uint32_t recvdata = drv_OK;

    R_AUDMAC_AllocateCh_t chn1 = {RCAR_AUDIO_DMAC_NONE, AUDMAC_CHNL_MAX_NUM};
    uint32_t aligned_len;
    uint32_t remained_bytes;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    aligned_len = round_down(len, AUDMAC_XFER_MIN_LEN);
    remained_bytes = len % AUDMAC_XFER_MIN_LEN;

    if (true == s_init_flag)
    {
        chn1 = allocate_channel();
        if (chn1.mUseCh < AUDMAC_CHNL_MAX_NUM)
        {
            ret = audmac_Interrupt_create(&mq_ch1_handle, chn1);
            if (drv_OK == ret)
            {
                op_ch1.mDMAMode = DRV_DMAC_DMA_NO_DESCRIPTOR;
                op_ch1.mSrcAddrMode = DRV_DMAC_ADDR_INCREMENTED;
                op_ch1.mDestAddrMode = DRV_DMAC_ADDR_FIXED;
                op_ch1.mTransferUnit = DRV_DMAC_TRANS_UNIT_4BYTE;
                op_ch1.mResource = DRV_DMAC_MEMORY;
                op_ch1.mLowSpeed = DRV_DMAC_SPEED_NORMAL;
                op_ch1.mPrioLevel = 0;
                op_ch1.mSrcAddr = (uintptr_t)src;
                op_ch1.mDestAddr = (uintptr_t)dst;
                op_ch1.mTransferCount = len / 4;
                op_ch1.mResource = resource;

                ret = R_AUDMAC_Exec(chn1.mUseAudmac, chn1.mUseCh, &op_ch1, NULL, arg);
                if (drv_OK != ret)
                {
                    R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte R_AUDMAC_Exec() Error. ret(%ld)\n", ret);
                    ret = drv_FAIL;
                }
            }
            else
            {
                R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte audmac_Interrupt_create() Error. ret(%ld).\n", ret);
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
                                                            AUDMAC_MQ_TIMEOUT,
                                                            (void*)&recvdata,
                                                            LOC_MQ_AUDMAC_MSG_SIZE);
                } while (OSAL_RETURN_TIME == osal_ret);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte R_OSAL_MqReceiveForTimePeriod() Error. osal_ret(%d)\n", osal_ret);
                }

                if (drv_OK != recvdata)
                {
                    R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte Error. recvdata(%ld), osal_ret(%d)\n", recvdata, osal_ret);
                    ret = R_AUDMAC_Stop(chn1.mUseAudmac, chn1.mUseCh);
                    if (drv_OK != ret)
                    {
                        R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte R_AUDMAC_Stop Error. ret(%ld)\n", ret);
                    }
                    ret = drv_FAIL;
                }
            }
        }

        /* release resource */
        if (NULL != mq_ch1_handle)
        {
            ret = audmac_Interrupt_close(mq_ch1_handle, chn1);
            if (drv_OK != ret)
            {
                R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte audmac_Interrupt_close() Error. ret(%ld).\n", ret);
            }
        }
        if (chn1.mUseCh < AUDMAC_CHNL_MAX_NUM)
        {
            ret = release_channel(chn1);
            if (drv_OK != ret)
            {
                R_PRINT_Log("[R_AUDMAC_CopyMemory]: Trans64byte release_channel() Error. ret(%ld).\n", ret);
            }
        }
    }
    else
    {
        ret = drv_FAIL;
        R_PRINT_Log("[R_AUDMAC_CopyMemory]: It is not initialized.\n");
    }
    return ret;
}
