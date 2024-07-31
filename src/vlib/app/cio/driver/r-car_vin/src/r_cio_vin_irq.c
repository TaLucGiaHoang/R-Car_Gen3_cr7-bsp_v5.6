/*************************************************************************************************************
* cio_vin_irq_c
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
#include "r_cio_vin_irq.h"

#include "r_vin_api.h"
#include "r_print_api.h"

/*******************************************************************************
  Section: Global Variables
*/
osal_device_handle_t VIN_device_handle[R_CIO_VIN_MAX_INSTANCE_NUM];

/*******************************************************************************
  Section: Local Defines
*/

#define CIO_VIN_MQ_MSG_SIZE      sizeof(uint32_t)

/*******************************************************************************
  Section: Local Functions
*/
static void *loc_cio_vin_Irq(osal_device_handle_t device_handle, uint64_t irq_channel, void* Arg)
{
    R_CIO_VIN_PRV_Instance_t *pinst;
    int32_t i;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t returndata = 0;
    bool isMqFull = false;

    pinst = (R_CIO_VIN_PRV_Instance_t *)Arg;

    R_VIN_ClearInterrupt(pinst->VinIdx);

    for (i = 0; i < R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE; i++)
    {
        if (pinst->Consumer[i].IsCapturing)
        {
            if (NULL != pinst->Consumer[i].mq_handle)
            {
                osal_ret = R_OSAL_MqIsFull(*pinst->Consumer[i].mq_handle, &isMqFull);
                if (!isMqFull) {
                    osal_ret = R_OSAL_MqSendForTimePeriod(*pinst->Consumer[i].mq_handle, TIMEOUT_MS, &returndata, CIO_VIN_MQ_MSG_SIZE);
                    if(OSAL_RETURN_OK != osal_ret)
                    {
                        R_PRINT_Log("[CioVinIrq]: loc_cio_vin_Irq R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
                    }
                }
            }
        }
    }

    return 0;
}

/**
 * @brief Device opening function
 * - Get the number of device which related to "vin" device type by using R_OSAL_IoGetNumOfDevices
 * - Get the required size of the buffer which use to store list of "vin" devices by using R_OSAL_IoGetDeviceList
 * - Get the list of "vin" devices by using R_OSAL_IoGetDeviceList
 * - Convert the device list to an array of device ID type
 * - Open "vin_0x" which is the first element of the device ID array above by using R_OSAL_IoDeviceOpen
 *
 * @param[in] device_type Device type
 * @param[in] device_channel Device channel
 * @param[in, out] device_handle To set the address of osal_device_handle_t
 * @return 0 on success
 * @return -1 on failure
 */

static int loc_CioVinDeviceOpen(char *device_type, int device_channel, osal_device_handle_t *device_handle)
{
    /* local variable */
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int app_ret = 0;
    size_t numOfDevice = 0;
    size_t numOfByte = 0;
    size_t i = 0;
    size_t j = 0;
    osal_device_handle_t local_handle = OSAL_DEVICE_HANDLE_INVALID;
    char devicelist[400];

    osal_ret = R_OSAL_IoGetNumOfDevices(device_type, &numOfDevice);
    if(OSAL_RETURN_OK != osal_ret)
    {
        app_ret = -1;
        R_PRINT_Log("[CioVinIrq]: loc_CioVinDeviceOpen R_OSAL_IoGetNumOfDevices failed(%d)\r\n", osal_ret);
    }
    else
    {
        osal_ret = R_OSAL_IoGetDeviceList(device_type, &devicelist[0], sizeof(devicelist), &numOfByte);
        if(OSAL_RETURN_OK != osal_ret)
        {
            app_ret = -1;
            R_PRINT_Log("[CioVinIrq]: loc_CioVinDeviceOpen R_OSAL_IoGetDeviceList failed(%d)\r\n", osal_ret);
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
                R_PRINT_Log("[CioVinIrq]: loc_CioVinDeviceOpen j(%d)>numOfDevice(%d)\r\n", j, numOfDevice);
            }
            else
            {
                osal_ret = R_OSAL_IoDeviceOpen(deviceList[device_channel], &local_handle);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    app_ret = -1;
                    R_PRINT_Log("[CioVinIrq]: loc_CioVinDeviceOpen R_OSAL_IoDeviceOpen failed(%d)\r\n", osal_ret);
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

    return app_ret;
}

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function name: R_CIO_VIN_PRV_IrqInit
*/
int32_t R_CIO_VIN_PRV_IrqInit(R_CIO_VIN_PRV_Instance_t *Inst)
{
    char taskname[] = "vin";
    size_t irqChannel;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int app_ret = 0;
    int32_t ret = R_CIO_VIN_ERR_OK;

    app_ret = loc_CioVinDeviceOpen(taskname, Inst->VinIdx, &VIN_device_handle[Inst->VinIdx]);
    if (0 != app_ret)
    {
        ret = R_CIO_VIN_ERR_FAILED;
        R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqInit loc_CioVinDeviceOpen failed(%d)\r\n", app_ret);
    }
    else
    {
        Inst->device_handle = VIN_device_handle[Inst->VinIdx];
    }

    if (R_CIO_VIN_ERR_OK == ret)
    {
        osal_ret = R_OSAL_InterruptGetNumOfIrqChannels(Inst->device_handle, &irqChannel);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = R_CIO_VIN_ERR_FAILED;
            R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqInit R_OSAL_InterruptGetNumOfIrqChannels failed(%d)\r\n", osal_ret);
        }
    }

    if (R_CIO_VIN_ERR_OK == ret)
    {
        if (irqChannel > 0)
        {
            irqChannel--;
        }
        else
        {
            irqChannel = 0;
        }

        osal_ret = R_OSAL_InterruptRegisterIsr(Inst->device_handle, irqChannel, 0, (p_osal_isr_func_t)loc_cio_vin_Irq, (void*)Inst);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = R_CIO_VIN_ERR_FAILED;
            R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqInit R_OSAL_InterruptRegisterIsr failed(%d)\r\n", osal_ret);
        }
    }

    if (R_CIO_VIN_ERR_OK == ret)
    {
        osal_ret = R_OSAL_InterruptEnableIsr(Inst->device_handle, irqChannel);
        if (OSAL_RETURN_OK != osal_ret)
        {
            ret = R_CIO_VIN_ERR_FAILED;
            R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqInit R_OSAL_InterruptEnableIsr failed(%d)\r\n", osal_ret);
        }
    }

    return ret;
}

/*******************************************************************************
  Function name: R_CIO_VIN_PRV_IrqDeInit
*/
int32_t R_CIO_VIN_PRV_IrqDeInit(R_CIO_VIN_PRV_Instance_t *Inst)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int32_t ret = R_CIO_VIN_ERR_OK;
    size_t irqChannel;

    osal_ret = R_OSAL_InterruptGetNumOfIrqChannels(Inst->device_handle, &irqChannel);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqDeInit R_OSAL_InterruptGetNumOfIrqChannels failed(%d)\r\n", osal_ret);
        ret = R_CIO_VIN_ERR_FAILED;
    }

    if (R_CIO_VIN_ERR_OK == ret)
    {
        if (irqChannel > 0)
        {
            irqChannel--;
        }
        else
        {
            irqChannel = 0;
        }

        osal_ret = R_OSAL_InterruptDisableIsr(Inst->device_handle, irqChannel);
        if(OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqDeInit R_OSAL_InterruptDisableIsr irqChannel(%d) failed(%d)\r\n", irqChannel, osal_ret);
            ret = R_CIO_VIN_ERR_FAILED;
        }

        if (R_CIO_VIN_ERR_OK == ret)
        {
            osal_ret = R_OSAL_InterruptUnregisterIsr(Inst->device_handle, irqChannel, (p_osal_isr_func_t)loc_cio_vin_Irq);
            if(OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqDeInit R_OSAL_InterruptUnregisterIsr irqChannel(%d) failed(%d)\r\n", irqChannel, osal_ret);
                ret = R_CIO_VIN_ERR_FAILED;
            }
        }

        if (R_CIO_VIN_ERR_OK == ret)
        {
            /* Close device */
            osal_ret = R_OSAL_IoDeviceClose(Inst->device_handle);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[CioVinIrq]: R_CIO_VIN_PRV_IrqDeInit R_OSAL_IoDeviceClose failed(%d)\r\n", osal_ret);
                ret = R_CIO_VIN_ERR_FAILED;
            }
        }
    }

    return ret;
}

