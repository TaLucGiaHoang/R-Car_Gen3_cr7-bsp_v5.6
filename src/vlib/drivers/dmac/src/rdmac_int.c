/*************************************************************************************************************
* dmac_int_c
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "drv.h"
#include "rdmac_common.h"
#include "rdmac_reg.h"
#include "rdmac_int.h"
#include "r_rtdmac_api.h"
#include "r_print_api.h"

#define LOC_MQ_RTDMAC_MSG_SIZE     sizeof(uint32_t)

/*!
 *  @brief          InterruptHandler
 *  @details
 *  @param[in]      device_handle device handle.
 *  @param[in]      irq_channel   irq channel.
 *  @param[in]      irq_arg       user parameter.
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
void rDmacInterruptHandler(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg)
{
    R_RTDMAC_Handle_t *args = (R_RTDMAC_Handle_t *)irq_arg;
    uint8_t dev = args->mUnit;
    uint8_t ch = args->mSubCh;
    uint32_t Value;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    uint32_t returndata = drv_OK;

    /* Select Interrupt Ch */
    Value = rDmacGet_RDMISTA(dev);
    if (0 != (Value & rDmacCH_MaskTable[ch]))
    {
        Value = rDmacGet_RDMOR(dev);

        /* Check Address error Flag */
        if (0 != (Value & DRV_DMAC_REG_RDMOR_AE))
        {
            /* RT-DMAC address error interrupt occurs during DMA transfer */
            R_PRINT_Log("rDmacInterruptHandler RT-DMAC address error. Value = 0x%08x\n", Value);
            rDmacClear_RDMCHCR_CAE(dev, ch);
            Value = rDmacClear_RDMCHCR_DE(dev, ch);
            if (drv_OK != Value)
            {
                R_PRINT_Log("rDmacInterruptHandler clear RDMCHCR_DE failed. retrun(drv_FAIL)\n");
            }
            returndata = drv_FAIL;
        }
        else
        {
            /* Check TE Interrupt */
            Value = rDmacGet_RDMCHCR(dev, ch);
            if (0 != (Value & DRV_DMAC_REG_RDMCHCR_TE))
            {
                rDmacClear_RDMCHCR_TE(dev, ch);
                if (DRV_DMAC_REG_RDMCHCR_DPM_REP != (Value & DRV_DMAC_REG_RDMCHCR_DPM))
                {
                    Value = rDmacClear_RDMCHCR_DE(dev, ch);
                    if (drv_OK != Value)
                    {
                        R_PRINT_Log("rDmacInterruptHandler clear RDMCHCR_DE failed. retrun(drv_FAIL)\n");
                    }
                }
            }
            /* Check DSE Interrupt */
            else if (0 != (Value & DRV_DMAC_REG_RDMCHCR_DSE))
            {
                rDmacClear_RDMCHCR_DSE(dev, ch);
                if (DRV_DMAC_REG_RDMCHCR_DPM_READ == (Value & DRV_DMAC_REG_RDMCHCR_DPM))
                {
                    Value = rDmacGet_RDMDPCR(dev, ch);
                }
            }
            returndata = drv_OK;
        }
    }
    osal_ret = R_OSAL_MqSendForTimePeriod(args->mMqHandle, RTDMAC_MQ_TIMEOUT, &returndata, LOC_MQ_RTDMAC_MSG_SIZE);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("rDmacInterruptHandler R_OSAL_MqSendForTimePeriod Error osal_ret = %d\n", osal_ret);
    }
}

