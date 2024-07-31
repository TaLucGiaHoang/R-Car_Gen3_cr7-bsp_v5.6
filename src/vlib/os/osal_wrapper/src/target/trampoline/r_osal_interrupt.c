/***********************************************************************************************************************
* Copyright [2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
/**
 * OSAL Interrupt Manager for Trampoline (AUTOSAR)
 */

#include "target/trampoline/r_osal_common.h"
#include "interrupts.h"

//#define OSAL_DEBUG_IRQ(...)
#define OSAL_DEBUG_IRQ OSAL_DEBUG

/* Note: This is not the number of interrupts that are used by the software. It
 * is the maximum interrupt IDs on the SoC */
#define MAX_NUM_INTERRUPTS      512


typedef struct st_irq_info
{
    osal_device_handle_t  deviceHandle;     /*!< Handle of device according this interrupt */
    p_osal_isr_func_t     irq_func;         /*!< Pointer of Worker function of Interrupt thread */
    void                  *irq_arg;         /*!< Pointer of Worker function argument */
    uint32_t              irq_channel;      /*!< irq_channel for device information */
} st_irq_info_t;

static st_irq_info_t loc_InterruptControlTable[MAX_NUM_INTERRUPTS];


/*******************************************************************************************************************//**
* @brief            check interrupt handler
* @param[in]        handle  To set handle
* @param[in]        irq_channel  To set the irq channel
* @retval           OSAL_RETURN_OK
* @retval           OSAL_RETURN_PAR
* @retval           OSAL_RETURN_HANDLE
***********************************************************************************************************************/
OSAL_STATIC e_osal_return_t osal_check_interrupt(osal_device_handle_t handle, uint64_t irq_channel)
{
    e_osal_return_t osal_ret;

    if (NULL == handle) {
        OSAL_DEBUG_ERROR("%s OSAL_RETURN_HANDLE\n", __func__);
        osal_ret = OSAL_RETURN_HANDLE;
    } else if ((false == handle->dev_info->interrupt) ||
        (OSAL_DEVICE_HANDLE_ID != handle->handle_id)) {
        OSAL_DEBUG_ERROR("%s OSAL_RETURN_HANDLE\n", __func__);
        osal_ret = OSAL_RETURN_HANDLE;
    } else if (irq_channel >= handle->dev_info->irq_channels) {
        OSAL_DEBUG_ERROR("%s (%s) ask for irq_channel %lld, but only %lld\n", __func__, handle->dev_info->id, irq_channel, handle->dev_info->irq_channels);
        osal_ret = OSAL_RETURN_PAR;
    } else {
        osal_ret = OSAL_RETURN_OK;
    }

    return osal_ret;
}


/* This function is called by all Autosar Category 2 interrupt tasks, i.e. we
 * are running in a task context, not an ISR. All INTC-RT interrupts are handled
 * by this function */
void R_OSAL_Sys_IrqHandler(int IntId)
{
    st_irq_info_t *control;

    OSAL_DEBUG_IRQ("%s:%d Id %d\n", __func__, __LINE__, IntId);

    if (IntId < MAX_NUM_INTERRUPTS) {
        control = &loc_InterruptControlTable[IntId];

        if (NULL != control) {
            control->irq_func(control->deviceHandle, control->irq_channel, control->irq_arg);
            if (IntId < 71 || IntId > 95)
                Irq_Enable(IntId);
        }
    }
}

e_osal_return_t osal_interrupt_initialize(void)
{
    st_irq_info_t *control;
    int i;

    for (i = 0; i < MAX_NUM_INTERRUPTS; i++) {
        control = &loc_InterruptControlTable[i];

        control->deviceHandle = OSAL_DEVICE_INVALID_ID;
        control->irq_func = NULL;
        control->irq_arg = NULL;
        control->irq_channel = 0;
    }

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_InterruptGetNumOfIrqChannels(osal_device_handle_t handle, size_t *const p_numOfChannels)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (NULL == handle) {
        OSAL_DEBUG_ERROR("%s:%d OSAL_RETURN_HANDLE\n", __func__, __LINE__);
        osal_ret = OSAL_RETURN_HANDLE;
    } else if (OSAL_DEVICE_HANDLE_ID != handle->handle_id) {
        OSAL_DEBUG_ERROR("%s:%d OSAL_RETURN_HANDLE\n", __func__, __LINE__);
        osal_ret = OSAL_RETURN_HANDLE;
    } else if (NULL == handle->dev_info) {
        OSAL_DEBUG_ERROR("%s:%d OSAL_RETURN_HANDLE\n", __func__, __LINE__);
        osal_ret = OSAL_RETURN_HANDLE;
    } else if (NULL == p_numOfChannels) {
        OSAL_DEBUG_ERROR("%s OSAL_RETURN_PAR\n", __func__);
        osal_ret = OSAL_RETURN_PAR;
    } else {
        *p_numOfChannels = handle->dev_info->irq_channels;
    }

    return osal_ret;
}

e_osal_return_t R_OSAL_InterruptRegisterIsr(osal_device_handle_t handle, uint64_t irq_channel,
                                            e_osal_interrupt_priority_t priority_value,
                                            p_osal_isr_func_t irqHandlerFunction, void *irq_arg)
{
    e_osal_return_t osal_ret;
    uint32_t spi_number;
    st_irq_info_t *control;

    (void)priority_value;   /* unused argument */

    osal_ret = osal_check_interrupt(handle, irq_channel);

    if ((OSAL_RETURN_OK == osal_ret) && (NULL == irqHandlerFunction)) {
        OSAL_DEBUG_ERROR("%s OSAL_RETURN_PAR\n", __func__);
        osal_ret = OSAL_RETURN_PAR;
    }

    if (OSAL_RETURN_OK == osal_ret) {
        spi_number = handle->dev_info->interrupt[irq_channel].irq_number;
        OSAL_DEBUG_IRQ("%s:%d Id %lu\n", __func__, __LINE__, spi_number + OSAL_INTERRUPT_SPI_OFFSET);

        control = &loc_InterruptControlTable[spi_number + OSAL_INTERRUPT_SPI_OFFSET];

        control->deviceHandle = handle;
        control->irq_func = irqHandlerFunction;
        control->irq_arg = irq_arg;
        control->irq_channel = (uint32_t)irq_channel;
    }

    return osal_ret;
}

e_osal_return_t R_OSAL_InterruptUnregisterIsr(osal_device_handle_t handle, uint64_t irq_channel,
                                              p_osal_isr_func_t irqHandlerFunction)
{
    e_osal_return_t osal_ret;
    uint32_t spi_number;
    st_irq_info_t *control = NULL;

    osal_ret = osal_check_interrupt(handle, irq_channel);

    if ((OSAL_RETURN_OK == osal_ret) && (NULL == irqHandlerFunction)) {
        OSAL_DEBUG_ERROR("%s OSAL_RETURN_PAR\n", __func__);
        osal_ret = OSAL_RETURN_PAR;
    }

    if (OSAL_RETURN_OK == osal_ret) {
        spi_number = handle->dev_info->interrupt[irq_channel].irq_number;
        OSAL_DEBUG_IRQ("%s:%d Id %lu\n", __func__, __LINE__, spi_number + OSAL_INTERRUPT_SPI_OFFSET);

        control = &loc_InterruptControlTable[spi_number + OSAL_INTERRUPT_SPI_OFFSET];

        if (irqHandlerFunction != control->irq_func) {
            OSAL_DEBUG_ERROR("%s OSAL_RETURN_PAR\n", __func__);
            osal_ret = OSAL_RETURN_PAR;
        } else {
            control->deviceHandle = OSAL_DEVICE_INVALID_ID;
            control->irq_func = NULL;
            control->irq_arg = NULL;
            control->irq_channel = 0;
        }
    }

    return osal_ret;
}

e_osal_return_t R_OSAL_InterruptEnableIsr(osal_device_handle_t handle, uint64_t irq_channel)
{
    e_osal_return_t osal_ret;
    uint32_t spi_number;

    osal_ret = osal_check_interrupt(handle, irq_channel);

    if (OSAL_RETURN_OK == osal_ret) {
        spi_number = handle->dev_info->interrupt[irq_channel].irq_number;
        OSAL_DEBUG_IRQ("%s:%d Id %lu\n", __func__, __LINE__, spi_number + OSAL_INTERRUPT_SPI_OFFSET);

        Irq_Enable(spi_number + OSAL_INTERRUPT_SPI_OFFSET);
    }

    return osal_ret;
}

e_osal_return_t R_OSAL_InterruptDisableIsr(osal_device_handle_t handle, uint64_t irq_channel)
{
    e_osal_return_t osal_ret;
    uint32_t spi_number;

    osal_ret = osal_check_interrupt(handle, irq_channel);

    if (OSAL_RETURN_OK == osal_ret) {
        spi_number = handle->dev_info->interrupt[irq_channel].irq_number;
        OSAL_DEBUG_IRQ("%s:%d Id %lu\n", __func__, __LINE__, spi_number + OSAL_INTERRUPT_SPI_OFFSET);

        Irq_Disable(spi_number + OSAL_INTERRUPT_SPI_OFFSET);
    }

    return osal_ret;
}

e_osal_return_t R_OSAL_InterruptIsISRContext(bool *const p_is_isr)
{
    /* All calling code is in task context */
    *p_is_isr = false;

    return OSAL_RETURN_OK;
}

e_osal_return_t R_OSAL_InterruptSetIsrPriority(osal_device_handle_t handle, uint64_t irq_channel,
                                               e_osal_interrupt_priority_t priority_value)
{
    /* We can't set the priority for tasks */
    return OSAL_RETURN_OK;
}
