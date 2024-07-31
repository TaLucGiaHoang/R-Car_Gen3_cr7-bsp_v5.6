/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_osal_io.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of OSAL Wrapper IO
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.04.2021 0.01     New create
 *         : 10.05.2021 0.02     Modify enumeration name of e_imrdrv_osal_pm_cmd_t.
 *                               Modify argument of r_imrdrv_osal_io_register_irq().
 *                               Add r_imrdrv_osal_io_unregister_irq().
 *                               Modify macro's name IMRDRV_OSAL_DEV_NAME_MAX_LEN.
 *         : 12.05.2021 0.03     Modify argument of r_imrdrv_osal_io_reg_enable_irq().
 *                               Modify enumeration name of e_imrdrv_osal_pm_cmd_t.
 *                               Initialize OSAL Wrapper IO area in r_imrdrv_osal_io_create().
 *                               Add r_imrdrv_osal_io_check_priority().
 *         : 09.06.2021 0.04     Add r_imrdrv_osal_io_read_reg().
 *         : 14.06.2021 0.05     Remove macro definition of IMRDRV_OSAL_IRQ_INDEX.
 *                               Modify r_imrdrv_osal_io_check_priority()'s attrobute from private function to
 *                               public function.
 *                               Modify comment in r_imrdrv_osal_io_create().
 *                               Modify indantation in r_imrdrv_osal_io_register_irq().
 *                               Add r_imrdrv_osal_io_read_reg(), r_imrdrv_osal_io_destroy().
 *                               Modify function header of r_imrdrv_osal_io_unregister_irq().
 *                               Implement r_imrdrv_osal_io_unregister_irq().
 *                               Add rule deviant comment to r_imrdrv_osal_io_reg_enable_irq().
 *         : 29.10.2021 0.06     Add r_imrdrv_osal_io_set_pm_policy(), r_imrdrv_osal_io_get_pm_policy() and 
 *                               r_imrdrv_osal_io_get_pm_state().
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_imrdrv_private.h"
#include "r_imrdrv_osal_io.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* The number of IO device list */
#define IMRDRV_OSAL_IO_DEVICE_NUM       (6U)     /* The maximum number of IO device. */
/* The maximum size of IO device list. */
#define IMRDRV_OSAL_IO_LIST_SIZE        (IMRDRV_OSAL_IO_DEVICE_NUM * (OSAL_DEVICE_NAME_MAX_LENGTH + 1U))

#define IMRDRV_OSAL_IRQ_CHANNEL_NUM    (1U)     /* The number of IRQ channels of IMR-LX4    */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_check_id (
    const char_t    *const p_device_type,
    const char_t    *const p_device_name
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_search_id (
    const char_t    *const p_device_name,
    const size_t    device_num,
    const char_t    *const p_device_list,
    const size_t    list_size
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_strncmp (
    const uint32_t  str_size,
    const char_t    *const p_str1,
    const char_t    *const p_str2
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_reg_enable_irq(
    const e_osal_interrupt_priority_t   dev_irq_priority,
    const p_osal_isr_func_t             handler_func,
    imrdrv_ctrl_handle_t                handle,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
);

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_create
* Description  : Create the IO handle.
* Arguments    : p_device_type  - Device type
*              : p_device_name  - Device name
*              : p_osal_io_area - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_create(
    const char_t                *const p_device_type,
    const char_t                *const p_device_name,
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != p_device_type) && (NULL != p_device_name) && (NULL != p_osal_io_area))
    {
        /* Check the specified device name is the valid device ID */
        func_ret = r_imrdrv_osal_io_check_id(p_device_type, p_device_name);

        if (IMRDRV_ERROR_OK == func_ret)
        {
            e_osal_return_t osal_ret;

            /* Open the IO device */
            osal_ret = R_OSAL_IoDeviceOpen(p_device_name, &p_osal_io_area->io_handle);

            if ((OSAL_RETURN_OK == osal_ret) && (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle))
            {
                /* Clear OSAL Wrapper IO area in the work arae */
                p_osal_io_area->handler_func = NULL;
            }
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }   /* end of if ((NULL != p_device_type) && (NULL != p_device_name) && (NULL != p_osal_io_area)) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_create
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_ctl_pm
* Description  : Control the OSAL Power Manager.
* Arguments    : p_osal_io_area - Address of OSAL Wrapper IO area in the work area
*              : cmd            - Control command
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_002]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_ctl_pm (
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    const e_imrdrv_osal_pm_cmd_t    cmd
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_io_area) && (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle) &&
        ((IMRDRV_OSAL_PM_CMD_CPG_ENABLE == cmd) || (IMRDRV_OSAL_PM_CMD_CPG_STOP == cmd)))
    {
        /* Declare internal variable */
        e_osal_return_t osal_ret;
        e_osal_pm_required_state_t required_state;

        /* Check command and required-status */
        if (IMRDRV_OSAL_PM_CMD_CPG_ENABLE == cmd)
        {
            required_state = OSAL_PM_REQUIRED_STATE_REQUIRED;
        }
        else
        {
            /* Argument check confirms that cmd is IMRDRV_OSAL_PM_CMD_CPG_ENABLE or IMRDRV_OSAL_PM_CMD_CPG_STOP,
             * so there is only IMRDRV_OSAL_PM_CMD_CPG_STOP except IMRDRV_OSAL_PM_CMD_CPG_ENABLE.                */
            required_state = OSAL_PM_REQUIRED_STATE_RELEASED;
        }

        /* Set the required-status to control CPG */
        osal_ret = R_OSAL_PmSetRequiredState(p_osal_io_area->io_handle, required_state, true);

        if (OSAL_RETURN_OK != osal_ret)
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }   /* end of if (check parameters) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_ctl_pm
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_register_irq
* Description  : Register the interrupt function.
* Arguments    : dev_irq_priority - Interrupt priority
*              : handler_func     - Callback function for interrupt handler of OSAL
*              : handle           - Control structure handle (Argument of the callback function)
*              : p_osal_io_area   - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_register_irq (
    const e_osal_interrupt_priority_t   dev_irq_priority,
    const p_osal_isr_func_t             handler_func,
    imrdrv_ctrl_handle_t                handle,
    st_imrdrv_osal_io_area_t            *const p_osal_io_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handler_func) && (NULL != handle) && (NULL != p_osal_io_area) &&
        (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle))
    {
        /* Check the interrupt priority is within the range */
        func_ret = r_imrdrv_osal_io_check_priority(dev_irq_priority);

        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Declare internal variable */
            e_osal_return_t osal_ret;
            size_t irq_num = 0U;

            /* Get the number of IRQ channels */
            osal_ret = R_OSAL_InterruptGetNumOfIrqChannels(p_osal_io_area->io_handle, &irq_num);

            /* Check the result of getting the number of IRQ channels */
            if ((OSAL_RETURN_OK == osal_ret) && (IMRDRV_OSAL_IRQ_CHANNEL_NUM == irq_num))
            {
                    /******************************************************************************************************
                     * MISRA Rule: Rule 11.2
                     * Reason: Hide structure type to register the argument of the callback function.
                     * Verification: This cast is executed to register the argument of the callback function to OSAL.
                     * In callback function, the argument is casted to the same type as imrdrv_ctrl_handle_t.
                     *****************************************************************************************************/
                /* Register and enable the interrupt function */
                    func_ret = r_imrdrv_osal_io_reg_enable_irq(dev_irq_priority, handler_func, handle, p_osal_io_area);

                    if (IMRDRV_ERROR_OK == func_ret)
                    {
                        /* Set OSAL Wrapper IO area in the work area */
                        p_osal_io_area->handler_func = handler_func;
                    }
            }
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }   /* end of if (IMRDRV_ERROR_OK == func_ret) */
    }   /* end of if (check parameters) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_register_irq
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_write_reg
* Description  : Write the register with register's offset.
* Arguments    : offset         - Register's offset
*              : value          - Register's value
*              : p_osal_io_area - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_05_01_004]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_write_reg (
    const uint32_t                  offset,
    const uint32_t                  value,
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_io_area) && (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle))
    {
        /* Declare internal variable */
        e_osal_return_t osal_ret;

        /* Write the register */
        osal_ret = R_OSAL_IoWrite32(p_osal_io_area->io_handle, offset, value);

        if (OSAL_RETURN_OK != osal_ret)
        {
            /* Set the register operation error to return value */
            func_ret = IMRDRV_ERROR_IO;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_write_reg
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_read_reg
* Description  : Read the register's value with register's offset.
* Arguments    : offset         - Register's offset
*              : p_osal_io_area - Address of OSAL Wrapper IO area in the work area
*              : p_value        - Register's value
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_05_01_010]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_read_reg (
    const uint32_t                  offset,
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    uint32_t                        *const p_value
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_io_area) &&
        (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle) &&
        (NULL != p_value))
    {
        /* Declare internal variable */
        e_osal_return_t osal_ret;
        uint32_t reg_val;

        /* Read the register */
        osal_ret = R_OSAL_IoRead32(p_osal_io_area->io_handle, offset, &reg_val);

        if (OSAL_RETURN_OK == osal_ret)
        {
            /* Set read register's value to [a]p_value */
            *p_value = reg_val;
        }
        else
        {
            /* Set the register operation error to return value */
            func_ret = IMRDRV_ERROR_IO;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_read_reg
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_destroy
* Description  : Create the IO handle.
* Arguments    : p_device_type  - Device type
*              : p_device_name  - Device name
*              : p_osal_io_area - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_011]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_destroy(
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if (NULL != p_osal_io_area)
    {
        e_osal_return_t osal_ret;

        /* Close the IO device */
        osal_ret = R_OSAL_IoDeviceClose(p_osal_io_area->io_handle);

        if (OSAL_RETURN_OK == osal_ret)
        {
            /* Clear OSAL Wrapper IO area in the work arae */
            p_osal_io_area->io_handle = OSAL_DEVICE_HANDLE_INVALID;
        }
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_destroy
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_unregister_irq
* Description  : Unregister the interrupt function.
* Arguments    : p_osal_io_area   - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_012]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_unregister_irq (
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_io_area) && (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle))
    {
        e_osal_return_t osal_ret;
        size_t irq_num = 0U;

        /* Get the number of IRQ channels */
        osal_ret = R_OSAL_InterruptGetNumOfIrqChannels(p_osal_io_area->io_handle, &irq_num);

        /* Check the result of getting the number of IRQ channels */
        if ((OSAL_RETURN_OK == osal_ret) && (IMRDRV_OSAL_IRQ_CHANNEL_NUM == irq_num))
        {
            /* Disable the interrupt function */
            osal_ret = R_OSAL_InterruptDisableIsr(p_osal_io_area->io_handle, IMRDRV_OSAL_IRQ_INDEX);

            /* Check the result of disabling the interrupt function */
            if (OSAL_RETURN_OK == osal_ret)
            {
                /* Unregister the interrupt function */
                osal_ret = R_OSAL_InterruptUnregisterIsr(p_osal_io_area->io_handle, IMRDRV_OSAL_IRQ_INDEX, p_osal_io_area->handler_func);

                /* Check the result of unregistering the interrupt function */
                if (OSAL_RETURN_OK == osal_ret)
                {
                    /* Clear OSAL Wrapper IO area in the work area */
                    p_osal_io_area->handler_func = NULL;
                }
	            else
	            {
	                /* Set unexpected error to return value */
	                func_ret = IMRDRV_ERROR_FAIL;
	            }
            }
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_unregister_irq
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_check_priority
* Description  : Check the interrupt priority is within the range.
* Arguments    : dev_irq_priority   - Interrupt priority
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_05_01_009]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_check_priority(
    const e_osal_interrupt_priority_t   dev_irq_priority
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    switch(dev_irq_priority)
    {
        case OSAL_INTERRUPT_PRIORITY_TYPE0:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE1:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE2:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE3:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE4:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE5:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE6:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE7:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE8:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE9:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE10:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE11:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE12:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE13:
            /* Fall through */
        case OSAL_INTERRUPT_PRIORITY_TYPE14:
            /** Do nothing because only return success in this case */
            break;
        default:
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
            break;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_check_priority
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_set_pm_policy
* Description  : Set power management policy to OSAL.
* Arguments    : p_osal_io_area   - Address of OSAL Wrapper IO area in the work area.
*                policy           - Power management policy.
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_013]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_set_pm_policy(
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    const e_osal_pm_policy_t        policy
)
{
    /* Return value of IMR functions */
    e_imrdrv_errorcode_t func_ret    = IMRDRV_ERROR_OK;
    /* Return value of OSAL functions */
    e_osal_return_t      osal_ret;

    /* Check parameters */
    if(NULL == p_osal_io_area)
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else if((OSAL_PM_POLICY_CG != policy) && (OSAL_PM_POLICY_PG != policy) && (OSAL_PM_POLICY_HP != policy))
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter check is all OK */
    {
        /* Set power management policy to OSAL */
        osal_ret = R_OSAL_PmSetPolicy(p_osal_io_area->io_handle, policy, true);
        if(OSAL_RETURN_OK != osal_ret)
        {
            /* Convert OSAL's error code to IMR's error code */
            func_ret = IMRDRV_ERROR_FAIL;
        }

    } /* End of if (check parameters) */

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_set_pm_policy()
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_get_pm_policy
* Description  : Get the current power management policy from OSAL.
* Arguments    : p_osal_io_area   - Address of OSAL Wrapper IO area in the work area.
*                p_policy         - Power management policy.
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_014]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_get_pm_policy(
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    e_osal_pm_policy_t              *const p_policy
)
{
    /* Return value of IMR functions */
    e_imrdrv_errorcode_t func_ret    = IMRDRV_ERROR_OK;
    /* Return value of OSAL functions */
    e_osal_return_t      osal_ret;

    /* Check parameters */
    if((NULL == p_osal_io_area) || (OSAL_DEVICE_HANDLE_INVALID == p_osal_io_area->io_handle))
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter check is all OK */
    {
        /* Get current power management state from OSAL */
        osal_ret = R_OSAL_PmGetPolicy(p_osal_io_area->io_handle, p_policy);
        if(OSAL_RETURN_OK != osal_ret)
        {
            /* Convert OSAL's error code to IMR's error code */
            func_ret = IMRDRV_ERROR_FAIL;
        }

    } /* End of if (check parameters) */

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_get_pm_policy()
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_get_pm_state
* Description  : This function gets current power management state from OSAL.
* Arguments    : p_osal_io_area   - Address of OSAL Wrapper IO area in the work area.
*                p_pm_state       - Power management state.
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_015]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_io_get_pm_state(
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    e_osal_pm_state_t               *const p_pm_state
)
{
    /* Return value of IMR functions */
    e_imrdrv_errorcode_t func_ret    = IMRDRV_ERROR_OK;
    /* Return value of OSAL functions */
    e_osal_return_t      osal_ret;

    /* Check parameters */
    if((NULL == p_osal_io_area) || (OSAL_DEVICE_HANDLE_INVALID == p_osal_io_area->io_handle))
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else if(NULL ==  p_pm_state)
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter check is all OK */
    {
        /* Get current power management state from OSAL */
        osal_ret = R_OSAL_PmGetState(p_osal_io_area->io_handle, p_pm_state);
        if(OSAL_RETURN_OK != osal_ret)
        {
            /* Convert OSAL's error code to IMR's error code */
            func_ret = IMRDRV_ERROR_FAIL;
        }

    } /* End of if (check parameters) */

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_get_pm_state
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_check_id
* Description  : Check the specified device name is the valid device ID.
* Arguments    : p_device_type         - Device type
*              : p_device_name         - Device name
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_005]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_check_id (
    const char_t    *const p_device_type,
    const char_t    *const p_device_name
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != p_device_type) && (NULL != p_device_name))
    {
        /* Declare internal variable */
        e_osal_return_t osal_ret;
        size_t device_num = 0U;

        /* Get the number of devices */
        osal_ret = R_OSAL_IoGetNumOfDevices(p_device_type, &device_num);

        if ((OSAL_RETURN_OK == osal_ret) && (0U != device_num))
        {
            /* Declare internal variable */
            char_t device_list[IMRDRV_OSAL_IO_LIST_SIZE];
            size_t list_size = 0U;

            /* Get the device list */
            osal_ret = R_OSAL_IoGetDeviceList(p_device_type, device_list, sizeof(device_list), &list_size);

            /* Check the result of getting the device list and the size is valid */
            if ((OSAL_RETURN_OK == osal_ret) && (list_size > 0U) && (list_size <= IMRDRV_OSAL_IO_LIST_SIZE))
            {
                /* Search target device name from device list */
                func_ret = r_imrdrv_osal_io_search_id(p_device_name, device_num, device_list, list_size);

                if (IMRDRV_ERROR_OK != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }   /* end of if ((OSAL_RETURN_OK == osal_ret) && (0U != device_num)) */
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }   /* end of if ((NULL != p_device_type) && (NULL != p_device_name)) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_check_id
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_search_id
* Description  : Search the target device ID from device list.
* Arguments    : p_device_name     - Device name
*              : device_num        - Number of devices
*              : p_device_list     - Device ID list
*              : list_size         - Size of device ID list
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_006]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_search_id (
    const char_t    *const p_device_name,
    const size_t    device_num,
    const char_t    *const p_device_list,
    const size_t    list_size
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != p_device_name) && (device_num > 0U) && (NULL != p_device_list) && (list_size > 0U))
    {
        /* Declare internal variable */
        size_t      device_idx = 0U;
        size_t      buffer_idx = 0U;
        uint32_t    id_idx;
        char_t      device_id[IMRDRV_OSAL_DEV_NAME_MAX_SIZE];
        bool        loop_flag = true;

        /* Search the target device ID from device list */
        while ((device_num > device_idx) && (list_size > buffer_idx) && (false != loop_flag))
        {
            id_idx = 0U;

            /* Copy characters from device ID list */
            while ((list_size > buffer_idx) && (id_idx < IMRDRV_OSAL_DEV_NAME_MAX_SIZE) && 
                   ('\n' != p_device_list[buffer_idx]) && ('\0' != p_device_list[buffer_idx]))
            {
                device_id[id_idx] = p_device_list[buffer_idx];
                buffer_idx++;
                id_idx++;
            }

            /* Check the index is not overflow */
            if (id_idx < IMRDRV_OSAL_DEV_NAME_MAX_SIZE)
            {
                /* Check character is '\n' */
                if ('\n' == p_device_list[buffer_idx])
                {
                    /* Increment the buffer_idx to set the index to the start of the next Device ID */
                    buffer_idx++;
                }

                /* Set null character to end of device ID */
                device_id[id_idx] = '\0';

                /* Compare 'device_id' and [a]p_device_name */
                func_ret = r_imrdrv_osal_io_strncmp((uint32_t)IMRDRV_OSAL_DEV_NAME_MAX_SIZE, device_id, p_device_name);

                /* Set loop flag if the result of comparing string is not failure */
                if (IMRDRV_ERROR_FAIL != func_ret)
                {
                    loop_flag = false;
                }
            }
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;

                /* Set loop flag to end loop */
                loop_flag = false;
            }

            /* Set unexpected error to return value if return value is not OK */
            if (IMRDRV_ERROR_OK != func_ret)
            {
                func_ret = IMRDRV_ERROR_FAIL;
            }

            device_idx++;
        }   /* end of while ((device_num > device_idx) && ... */
    }   /* end of if ((NULL != p_device_name) && ... */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_search_id
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_strncmp
* Description  : Compare the target string 1 and 2.
* Arguments    : str_size       - String max length
*              : p_str1         - Target string 1
*              : p_str2         - Target string 2
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_007]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_strncmp (
    const uint32_t  str_size,
    const char_t    *const p_str1,
    const char_t    *const p_str2
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((str_size > 0U) && (NULL != p_str1) && (NULL != p_str2))
    {
        /* Declare internal variable */
        uint32_t str_idx = 0U;
        bool loop_flag = true;

        /* Compare the target string 1 and 2 */
        while ((str_size > str_idx) && (true == loop_flag))
        {
            /* Check the character of each string */
            if (p_str1[str_idx] == p_str2[str_idx])
            {
                /* Check the character is null */
                if ('\0' == p_str1[str_idx])
                {
                    /* Each string reaches its end. */
                    /* Set to end loop */
                    loop_flag = false;
                }
            }
            else
            {
                /* String 1 and 2 are mismatched. */
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
                /* Set to end loop */
                loop_flag = false;
            }

            str_idx++;
        }
    }   /* end of if ((0U < str_size) && (NULL != p_str1) && (NULL != p_str2)) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_strncmp
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_io_reg_enable_irq
* Description  : Register and enable the interrupt function.
* Arguments    : dev_irq_priority   - Interrupt priority
*              : handler_func       - Callback function for interrupt handler of OSAL
*              : handle           - Control structure handle (Argument of the callback function)
*              : p_osal_io_area     - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_01_008]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_osal_io_reg_enable_irq(
    const e_osal_interrupt_priority_t   dev_irq_priority,
    const p_osal_isr_func_t             handler_func,
    /******************************************************************************************************
     * MISRA Rule: Rule 8.13
     * Reason: Register as general-purpose pointer for callback function's argument without const.
     * Verification: handle is designed not to change in this function.
     *****************************************************************************************************/
    imrdrv_ctrl_handle_t                handle,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    func_ret = r_imrdrv_osal_io_check_priority(dev_irq_priority);

    if ((NULL != handler_func) && (NULL != handle) && (NULL != p_osal_io_area) &&
        (OSAL_DEVICE_HANDLE_INVALID != p_osal_io_area->io_handle) && (IMRDRV_ERROR_OK == func_ret))
    {
        e_osal_return_t osal_ret;

        /* Register the function called when interrupt */
        osal_ret = R_OSAL_InterruptRegisterIsr(p_osal_io_area->io_handle,
                                                IMRDRV_OSAL_IRQ_INDEX, 
                                                dev_irq_priority,
                                                handler_func,
        /******************************************************************************************************
         * MISRA Rule: Dir 1.1
         * Reason: Register as general-purpose pointer for callback function's argument.
         * Verification: This cast is no problem because handle passed to callback function's argument and
         * the argument is casted to the same type as imrdrv_ctrl_handle_t in the callback function.
         *****************************************************************************************************/
                                                (void*)handle);

        /* Check the result of registering the function is OK */
        if (OSAL_RETURN_OK == osal_ret)
        {
            /* Enable the interrupt function */
            osal_ret = R_OSAL_InterruptEnableIsr(p_osal_io_area->io_handle, IMRDRV_OSAL_IRQ_INDEX);

            /* Check the result of enabling the interrupt function is not OK */
            if (OSAL_RETURN_OK != osal_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;

                /* Unregister the function called when interrupt */
                osal_ret = R_OSAL_InterruptUnregisterIsr(p_osal_io_area->io_handle,
                                                            IMRDRV_OSAL_IRQ_INDEX,
                                                            handler_func);

                /* Check the result of unregistering the function is not OK */
                if (OSAL_RETURN_OK != osal_ret)
                {
                    /** Do nothing because this function returns always ERROR_FAIL
                     * regardless of the result of R_OSAL_InterruptUnregisterIsr() */
                }
            }
        } /* end of if (OSAL_RETURN_OK == osal_ret) */
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    } /* end of if ((NULL != handler_func) && ... */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_io_reg_enable_irq
 *********************************************************************************************************************/

