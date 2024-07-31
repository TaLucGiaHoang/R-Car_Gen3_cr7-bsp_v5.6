/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_mainctl.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of Basic Function Main Control
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.04.2021 0.01     New create
 *         : 10.05.2021 0.02     Add r_imrdrv_mainctl_start(), r_imrdrv_mainctl_stop(),
 *                               r_imrdrv_mainctl_prepare_io () and r_imrdrv_mainctl_prepare_irq_pm().
 *         : 12.05.2021 0.03     Modify error code in the case of parameter error in r_imrdrv_mainctl_start().
 *                               Modify enumeration name of e_imrdrv_osal_pm_cmd_t in r_imrdrv_mainctl_start().
 *         : 09.06.2021 0.04     Add r_imrdrv_mainctl_proc_callback().
 *         : 14.06.2021 0.05     Remove unnecessary header file include.
 *                               Add macro definition of The lower limit of stride and Height and width.
 *                               Add function call of r_imrdrv_osal_io_check_priority() and
 *                               r_imrdrv_mainctl_clear_attr() to r_imrdrv_mainctl_init().
 *                               Implement r_imrdrv_mainctl_quit() and r_imrdrv_mainctl_stop().
 *                               Modify function header of r_imrdrv_mainctl_quit() and r_imrdrv_mainctl_stop().
 *                               Add r_imrdrv_mainctl_chk_dl_data(), r_imrdrv_mainctl_chk_src_data(),
 *                               r_imrdrv_mainctl_chk_dst_data(), r_imrdrv_mainctl_chk_tri_mode(),
 *                               r_imrdrv_mainctl_chk_img_tri(), r_imrdrv_mainctl_attr_set_param(),
 *                               r_imrdrv_mainctl_chk_attr_opt(), r_imrdrv_mainctl_execute(),
 *                               r_imrdrv_mainctl_start_render(), r_imrdrv_mainctl_clear_attr(),
 *                               r_imrdrv_mainctl_chk_comb_tri(), and r_imrdrv_mainctl_set_attr_reg()
 *         : 29.10.2021 0.06     Add r_imrdrv_mainctl_set_pm_policy() and r_imrdrv_mainctl_get_pm_policy().
 *                               Remove call to r_imrdrv_devctl_clear_opt_attr() from r_imrdrv_mainctl_attr_set_param().
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_imrdrv_private.h"
#include "r_imrdrv_basic_mainctl.h"
#include "r_imrdrv_basic_dev.h"
#include "r_imrdrv_osal_io.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* The lower limit of stride */
#define IMRDRV_LOWER_LIMIT_STRIDE_SRC (256U)   /* The lower limit of stride for the source image data. */
#define IMRDRV_LOWER_LIMIT_STRIDE_DST (64U)    /* The lower limit of stride for the destination image data. */

/* Height and width */
#define IMRDRV_LOWER_LIMIT_HEIGHT     (1U)      /* The lower limit of height. */
#define IMRDRV_LOWER_LIMIT_WIDTH      (2U)      /* The lower limit of width. */
#define IMRDRV_UPPER_LIMIT_HEIGHT     (2048U)   /* The upper limit of height. */
#define IMRDRV_UPPER_LIMIT_WIDTH      (2048U)   /* The upper limit of width. */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_prepare_irq_pm (
    const e_osal_interrupt_priority_t       dev_irq_priority,
    const p_osal_isr_func_t                 handler_func,
    imrdrv_ctrl_handle_t                    handle,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_prepare_io (
    const e_osal_interrupt_priority_t       dev_irq_priority,
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const p_osal_isr_func_t                 handler_func,
    imrdrv_ctrl_handle_t                    handle,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
);
IMRDRV_STATIC e_imrdrv_errorcode_t  r_imrdrv_mainctl_clear_attr (
    st_imrdrv_attr_param_t                  *const p_attr_param
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_comb_tri (
    const e_imrdrv_color_format_t           src_color_format,
    const e_imrdrv_bpp_t                    src_bpp,
    const e_imrdrv_color_format_t           dst_color_format,
    const e_imrdrv_bpp_t                    dst_bpp,
    const st_imrdrv_triangle_mode_t         *const p_triangle_mode
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_set_attr_reg (
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t          *const p_osal_io_area,
    const st_imrdrv_attr_register_value_t   *const p_reg_value_offset
);


/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_init
* Description  : Clear and initialize the work area.
* Arguments    : channel_no             - Channel number
*              : dev_irq_priority       - Interrupt priority
*              : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : p_basic_mainctl_area   - Address of Basic Function Main Control area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_init (
    const e_imrdrv_channelno_t          channel_no,
    const e_osal_interrupt_priority_t   dev_irq_priority,
    st_imrdrv_basic_device_area_t       *const p_basic_device_area,
    st_imrdrv_basic_mainctl_area_t      *const p_basic_mainctl_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameters */
    if ((NULL != p_basic_mainctl_area) && (NULL != p_basic_device_area))
    {
        e_imrdrv_errorcode_t    check_ch_ret;
        e_imrdrv_errorcode_t    check_priority_ret;

        /* Check channel number */
        check_ch_ret = r_imrdrv_devctl_check_ch(channel_no);

        /* Check the interrupt priority is within the range */
        check_priority_ret = r_imrdrv_osal_io_check_priority(dev_irq_priority);

        if ((IMRDRV_ERROR_OK == check_ch_ret) && (IMRDRV_ERROR_OK == check_priority_ret))
        {
            /* Initialize Basic Function Device Dependent Control area in the work area */
            func_ret = r_imrdrv_devctl_init(channel_no, p_basic_device_area);

            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Initialize Basic Function Main Control area in the work area */
                p_basic_mainctl_area->dev_irq_priority = dev_irq_priority;

                /* Initialize all attribute parameters in the work area. */
                func_ret = r_imrdrv_mainctl_clear_attr(&(p_basic_mainctl_area->attr_param));
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
    }
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_init()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_quit
* Description  : Clear work area.
* Arguments    : p_basic_mainctl_area      - Address of Basic Function Device Dependent Control area in the work area
*              : p_basic_device_area       - Address of Basic Function Main Control area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_020]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_quit (
    st_imrdrv_basic_mainctl_area_t    *const p_basic_mainctl_area,
    st_imrdrv_basic_device_area_t     *const p_basic_device_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_mainctl_area) && (NULL != p_basic_device_area))
    {
        /* Clear Basic Function Main Control area in the work area */
        p_basic_mainctl_area->dev_irq_priority = OSAL_INTERRUPT_PRIORITY_LOWEST;

        /* Reset all attribute parameters in the work area */
        func_ret = r_imrdrv_mainctl_clear_attr(&(p_basic_mainctl_area->attr_param));

        /* Check the result of reset */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Clear Basic Function Device Dependent Control area in the work area */
            func_ret = r_imrdrv_devctl_quit(p_basic_device_area);
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_quit()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_start
* Description  : Start the channel.
* Arguments    : p_basic_mainctl_area       - Address of Basic Function Main Control area in the work area
*              : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
*              : handler_func               - Callback function for interrupt handler of OSAL
*              : handle                     - Control structure handle (Argument of the callback function)
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_02_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_start (
    const st_imrdrv_basic_mainctl_area_t    *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const p_osal_isr_func_t                 handler_func,
    imrdrv_ctrl_handle_t                    handle,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_mainctl_area) &&
        (NULL != p_basic_device_area) &&
        (NULL != handler_func) &&
        (NULL != handle) &&
        (NULL != p_osal_io_area))
    {
        /* Prepare IO resource, interrupt function and clock to start the channel */
        func_ret = r_imrdrv_mainctl_prepare_io(p_basic_mainctl_area->dev_irq_priority, p_basic_device_area,
                                               handler_func, handle, p_osal_io_area);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Execute software reset for the specified channel */
            func_ret = r_imrdrv_devctl_reset_reg(p_basic_device_area, p_osal_io_area);

            /* Check the return value is OK */
            if(IMRDRV_ERROR_OK == func_ret)
            {
                /* Enable interrupt */
                func_ret = r_imrdrv_devctl_enable_irq(p_basic_device_area, p_osal_io_area);

                /* Check the result of enabling interrupt */
                if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }
            else
            {
                /* Check the return value is not ERROR_IO */
                if (IMRDRV_ERROR_IO != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }

            /* Check the return value is not OK */
            if (IMRDRV_ERROR_OK != func_ret)
            {
                e_imrdrv_errorcode_t    error_handling_ret;

                /* Disable clock */
                error_handling_ret = r_imrdrv_osal_io_ctl_pm(p_osal_io_area, IMRDRV_OSAL_PM_CMD_CPG_STOP);

                /* Check the return value is not OK */
                if (IMRDRV_ERROR_OK != error_handling_ret)
                {
                    /** Do nothing because this function returns ERROR_FAIL or ERROR_IO
                     * regardless of the result of r_imrdrv_osal_io_ctl_pm() */
                }

                /* Unregister the interrupt function */
                error_handling_ret = r_imrdrv_osal_io_unregister_irq(p_osal_io_area);

                /* Check the return value is not OK */
                if (IMRDRV_ERROR_OK != error_handling_ret)
                {
                    /** Do nothing because this function returns ERROR_FAIL or ERROR_IO
                     * regardless of the result of r_imrdrv_osal_io_unregister_irq() */
                }

                /* Destroy the IO resource */
                error_handling_ret = r_imrdrv_devctl_io_destroy(p_osal_io_area);

                /* Check the return value is not OK */
                if (IMRDRV_ERROR_OK != error_handling_ret)
                {
                    /** Do nothing because this function returns ERROR_FAIL or ERROR_IO
                     * regardless of the result of r_imrdrv_devctl_io_destroy() */
                }
            }   /* end of if (IMRDRV_ERROR_OK != func_ret) */
        }   /* end of if (IMRDRV_ERROR_OK == func_ret) */
    }   /* end of if ((NULL != p_basic_mainctl_area) && ...) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_start()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_stop
* Description  : Stop the channel.
* Arguments    : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_02_021]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_stop (
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        /* Disable IMR interrupt */
        func_ret = r_imrdrv_devctl_disable_irq(p_basic_device_area, p_osal_io_area);

        /* Check the result of disabling IMR interrupt */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Disable clock for the specified channel */
            func_ret = r_imrdrv_osal_io_ctl_pm(p_osal_io_area, IMRDRV_OSAL_PM_CMD_CPG_STOP);

            /* Check the result of disabling clock  */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Unregister the interrupt function */
                func_ret = r_imrdrv_osal_io_unregister_irq(p_osal_io_area);

                /* Check the result of unregistering the interrupt function */
                if (IMRDRV_ERROR_OK == func_ret)
                {
                    /* Destroy IO handle for specified channel */
                    func_ret = r_imrdrv_devctl_io_destroy(p_osal_io_area);

                    /* Check the result of destroying IO handle is not OK */
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
            } /* end of if (IMRDRV_ERROR_OK == func_ret) */
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        } /* end of if (IMRDRV_ERROR_OK == func_ret) */
        else
        {
            /* Check the return value is not ERROR_IO */
            if (IMRDRV_ERROR_IO != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    } /* end of if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area)) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_stop()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_proc_callback
* Description  : Get result of IMR and mask IMR interrupt.
* Arguments    : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
*              : p_rs_val                   - SR register's value
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_02_016]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_proc_callback (
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t          *const p_osal_io_area,
    uint32_t                                *const p_sr_val
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) &&
        (NULL != p_osal_io_area) &&
        (NULL != p_sr_val))
    {
        uint32_t  sr_reg_val;

        /* Read SR register's value */
        func_ret = r_imrdrv_devctl_read_reg(p_basic_device_area, IMRDRV_REG_OFFSET_SR,
                                                p_osal_io_area, &sr_reg_val);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            uint32_t  srcr_reg_val;

            /* Set SRCR register value */
            srcr_reg_val = sr_reg_val & IMRDRV_REG_VAL_SRCR_ALL;
            /* Write SRCR register  */
            func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area, IMRDRV_REG_OFFSET_SRCR,
                                                 srcr_reg_val, p_osal_io_area);

            /* Check the return value is OK */
            if((IMRDRV_ERROR_OK == func_ret) && (IMRDRV_REG_VAL_SR_INT != sr_reg_val))
            {
                /* Enable interrupt */
                func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area, IMRDRV_REG_OFFSET_IMR,
                                                     IMRDRV_REG_VAL_IMR_MASK, p_osal_io_area);
            }
        }

        /* Check the result of enabling interrupt */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            *p_sr_val = sr_reg_val;
        }
        else
        {
            if (IMRDRV_ERROR_IO != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_proc_callback()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_dl_data
* Description  : Check the display list data.
* Arguments    : p_dl_data                  - Display list data
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_02_006]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_dl_data (
    const st_imrdrv_dl_t    *const p_dl_data
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if ((NULL != p_dl_data) && (0U == (p_dl_data->phys_addr & 0x00000007U)))
    {
        /* Do nothing because only return success in this case */
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_dl_data()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_src_data
* Description  : Check the source image data.
* Arguments    : p_src_data                 - Source image data
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_02_007]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_src_data (
    const st_imrdrv_data_t  *const p_src_data
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if (NULL !=p_src_data)
    {
        if ((p_src_data->height >= IMRDRV_LOWER_LIMIT_HEIGHT    ) &&
            (p_src_data->height <= IMRDRV_UPPER_LIMIT_HEIGHT    ) &&
            (p_src_data->width  >= IMRDRV_LOWER_LIMIT_WIDTH     ) &&
            (p_src_data->width  <= IMRDRV_UPPER_LIMIT_WIDTH     ) &&
            (p_src_data->stride >= IMRDRV_LOWER_LIMIT_STRIDE_SRC) &&
            (0U == (p_src_data->stride    & 0x000000FFU)) &&
            (0U == (p_src_data->phys_addr & 0x000000FFU)))
        {
            /* Check the stride is within the upper limit. */
            func_ret = r_imrdrv_devctl_chk_stride(p_src_data->stride);
            
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Check the color format and bpp are valid */
                func_ret = r_imrdrv_devctl_chk_color_form(p_src_data->color_format, p_src_data->bpp);
            }
        }
        else
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_src_data()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_dst_data
* Description  : Check the source image data.
* Arguments    : p_dst_data                 - Destination image data
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_02_008]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_dst_data (
    const st_imrdrv_data_t  *const p_dst_data
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if (NULL !=p_dst_data)
    {
        if ((p_dst_data->height >= IMRDRV_LOWER_LIMIT_HEIGHT    ) &&
            (p_dst_data->height <= IMRDRV_UPPER_LIMIT_HEIGHT    ) &&
            (p_dst_data->width  >= IMRDRV_LOWER_LIMIT_WIDTH     ) &&
            (p_dst_data->width  <= IMRDRV_UPPER_LIMIT_WIDTH     ) &&
            (p_dst_data->stride >= IMRDRV_LOWER_LIMIT_STRIDE_DST) &&
            (0U == (p_dst_data->stride    & 0x0000003FU)) &&
            (0U == (p_dst_data->phys_addr & 0x0000003FU)))
        {
            /* Check the stride is within the upper limit. */
            func_ret = r_imrdrv_devctl_chk_stride(p_dst_data->stride);
            
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Check the color format and bpp are valid */
                func_ret = r_imrdrv_devctl_chk_color_form(p_dst_data->color_format, p_dst_data->bpp);
            }
        }
        else
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_dst_data()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_tri_mode
* Description  : Check the triangle mode.
* Arguments    : p_triangle_mode            - Triangle mode
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_02_009]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_tri_mode (
    const st_imrdrv_triangle_mode_t   *const p_triangle_mode
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ( NULL != p_triangle_mode )
    {
        if (((IMRDRV_MODE_OFF == p_triangle_mode->tfe   ) ||
             (IMRDRV_MODE_ON  == p_triangle_mode->tfe   )) &&
            ((IMRDRV_MODE_OFF == p_triangle_mode->tcm   ) ||
             (IMRDRV_MODE_ON  == p_triangle_mode->tcm   )) &&
            ((IMRDRV_MODE_OFF == p_triangle_mode->autosg) ||
             (IMRDRV_MODE_ON  == p_triangle_mode->autosg)) &&
            ((IMRDRV_MODE_OFF == p_triangle_mode->autodg) ||
             (IMRDRV_MODE_ON  == p_triangle_mode->autodg)) &&
            ((IMRDRV_MODE_OFF == p_triangle_mode->bfe   ) ||
             (IMRDRV_MODE_ON  == p_triangle_mode->bfe   )) &&
            ((IMRDRV_MODE_OFF == p_triangle_mode->tme   ) ||
             (IMRDRV_MODE_ON  == p_triangle_mode->tme   )))
        {
            /* Check the triangle mode for depending on device */
            func_ret = r_imrdrv_devctl_chk_tri_mode(
                           p_triangle_mode->uvshfval,
                           p_triangle_mode->shfval,
                           p_triangle_mode->uvshfe,
                           p_triangle_mode->shfe,
                           p_triangle_mode->rde);
        }
        else
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_tri_mode()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_img_tri
* Description  : Check the combination between source / destination image data and triangle mode.
* Arguments    : src_color_format           - Color format of the source image data
*              : src_bpp                    - Bpp of the source image data
*              : dst_color_format           - Color format of the destination image data
*              : dst_bpp                    - Bpp of the destination image data
*              : p_triangle_mode            - Triangle mode
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_012]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_img_tri (
    const e_imrdrv_color_format_t     src_color_format,
    const e_imrdrv_bpp_t              src_bpp,
    const e_imrdrv_color_format_t     dst_color_format,
    const e_imrdrv_bpp_t              dst_bpp,
    const st_imrdrv_triangle_mode_t   *const p_triangle_mode
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if (NULL != p_triangle_mode)
    {
        /* Check the combination of the color format */
        func_ret = r_imrdrv_devctl_chk_comb_form(src_color_format, dst_color_format);
        
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Check the combination of the color format */
            func_ret = r_imrdrv_mainctl_chk_comb_tri(
                           src_color_format,
                           src_bpp,
                           dst_color_format,
                           dst_bpp,
                           p_triangle_mode);
        }
    }
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_img_tri()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_attr_set_param
* Description  : Set the attribute parameter to the work area.
* Arguments    : p_param                    - Attribute parameter
*              : p_basic_mainctl_area       - Address of Basic Function Main Control area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_011]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_attr_set_param  (
    const st_imrdrv_attr_param_t        *const p_param,
    st_imrdrv_basic_mainctl_area_t      *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t *const p_basic_device_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_param) && (NULL != p_basic_mainctl_area) && (NULL != p_basic_device_area))
    {
        /* Set the attribute parameter */
        p_basic_mainctl_area->attr_param = *p_param;

        if ((IMRDRV_COLOR_FORM_SEMI420 != p_basic_mainctl_area->attr_param.src_data.color_format) &&
            (IMRDRV_COLOR_FORM_SEMI422 != p_basic_mainctl_area->attr_param.src_data.color_format))
        {
            p_basic_mainctl_area->attr_param.src_data.uv_set.uv_stride    =  0U;
            p_basic_mainctl_area->attr_param.src_data.uv_set.uv_phys_addr =  0U;
            p_basic_mainctl_area->attr_param.src_data.uv_set.uv_bpp       = IMRDRV_BPP_INVALID;
        }

        if ((IMRDRV_COLOR_FORM_SEMI420 != p_basic_mainctl_area->attr_param.dst_data.color_format) &&
            (IMRDRV_COLOR_FORM_SEMI422 != p_basic_mainctl_area->attr_param.dst_data.color_format))
        {
            p_basic_mainctl_area->attr_param.dst_data.uv_set.uv_stride    =  0U;
            p_basic_mainctl_area->attr_param.dst_data.uv_set.uv_phys_addr =  0U;
            p_basic_mainctl_area->attr_param.dst_data.uv_set.uv_bpp       = IMRDRV_BPP_INVALID;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_attr_set_param()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_attr_opt
* Description  : Check the combination of the attribute parameter and the optional attribute parameter.
* Arguments    : p_basic_mainctl_area       - Address of Basic Function Main Control
*              : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_013]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_attr_opt (
    const st_imrdrv_basic_mainctl_area_t *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t  *const p_basic_device_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_basic_mainctl_area) && (NULL != p_basic_device_area))
    {
        /* no operation for Gen3e */
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_attr_opt()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_execute
* Description  : Execute software reset and writes parameters to registers.
* Arguments    : p_basic_mainctl_area       - Address of Basic Function Main Control area in the work area
*              : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_02_014]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_execute (
    const st_imrdrv_basic_mainctl_area_t *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t  *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t       *const p_osal_io_area
)
{
    /* Deeclare internal variables */
    e_imrdrv_errorcode_t func_ret;
    
    /* Check parameters */
    if ((NULL != p_basic_mainctl_area) && (NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        /* Execute software reset and initialize register that is not initialized by software reset. */
        func_ret = r_imrdrv_devctl_reset_reg(p_basic_device_area, p_osal_io_area);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            st_imrdrv_attr_register_value_t reg_value_offset;
            /* Decide the setting value of registers */
            func_ret = r_imrdrv_devctl_get_reg_val(&p_basic_mainctl_area->attr_param,
                                                   p_basic_device_area,
                                                   &reg_value_offset);
            
            /* Check the return value is OK */
            if(IMRDRV_ERROR_OK == func_ret)
            {
                /* Write parameters to registers. */
                func_ret = r_imrdrv_mainctl_set_attr_reg(p_basic_device_area, p_osal_io_area, &reg_value_offset);
                
                /* Check the return value is not OK or ERROR_IO */
                if((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
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
            /* Check the return value is not ERROR_IO */
            if (IMRDRV_ERROR_IO != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_execute()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_set_pm_policy
* Description  : This function sets the power management policy.
* Arguments    : p_osal_io_area            - Address of OSAL Wrapper IO area in the work area.
*              : policy                    - Power management policy.
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_022]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_set_pm_policy (
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area,
    const e_osal_pm_policy_t            policy
)
{
    /* Return value of each functions */
    e_imrdrv_errorcode_t     func_ret;
    e_osal_pm_state_t pm_state = OSAL_PM_STATE_INVALID;

    /* Check parameters */
    if (NULL == p_osal_io_area)
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else if ((OSAL_PM_POLICY_CG != policy) && (OSAL_PM_POLICY_PG != policy) && (OSAL_PM_POLICY_HP != policy))
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter check is all OK */
    {
        /* Set power management policy to OSAL */
        func_ret = r_imrdrv_osal_io_set_pm_policy(p_osal_io_area, policy);
        if(IMRDRV_ERROR_OK != func_ret)
        {
            func_ret = IMRDRV_ERROR_FAIL;
        }
        else
        {
            /* Get OSAL power state */
            func_ret = r_imrdrv_osal_io_get_pm_state(p_osal_io_area, &pm_state);
            if(IMRDRV_ERROR_OK != func_ret)
            {
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_set_pm_policy()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_get_pm_policy
* Description  : This function gets the current power management policy.
* Arguments    : p_osal_io_area            - Address of OSAL Wrapper IO area in the work area.
*              : policy                    - Power management policy.
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_023]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_get_pm_policy (
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area,
    e_osal_pm_policy_t                  *const p_policy
)
{
    /* Return value of each functions */
    e_imrdrv_errorcode_t     func_ret;

    /* Check parameters */
    if (NULL == p_osal_io_area)
    {
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter check is all OK */
    {
        /* Get the  power management policy from OSAL */
        func_ret = r_imrdrv_osal_io_get_pm_policy(p_osal_io_area, p_policy);
        if(IMRDRV_ERROR_OK != func_ret)
        {
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_get_pm_policy()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_start_render
* Description  : Execute DL.
* Arguments    : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_02_015]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_mainctl_start_render (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
)
{
    /* Deeclare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        /* Set the interrupt mask */
        func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area,
                                              IMRDRV_REG_OFFSET_IMR,
                                              0x00000018U,
                                              p_osal_io_area);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Execute DL */
            func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area,
                                                 IMRDRV_REG_OFFSET_CR,
                                                 0x00000001U,
                                                 p_osal_io_area);
            
            /* Check the return value is not OK or ERROR_IO */
            if((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Check the return value is not ERROR_IO */
            if (IMRDRV_ERROR_IO != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
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

/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_start_render()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_prepare_io
* Description  : Prepare IO resource, interrupt function and clock to start the channel.
* Arguments    : dev_irq_priority           - Interrupt priority
*              : p_basic_device_area        - Address of Basic Function Device Dependent Control area in the work area
*              : handler_func               - Callback function for interrupt handler of OSAL
*              : handle                     - Control structure handle (Argument of the callback function)
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_004]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_prepare_io (
    const e_osal_interrupt_priority_t       dev_irq_priority,
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const p_osal_isr_func_t                 handler_func,
    imrdrv_ctrl_handle_t                    handle,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) &&
        (NULL != handler_func) &&
        (NULL != handle) &&
        (NULL != p_osal_io_area))
    {
        /* Create the IO resource */
        func_ret = r_imrdrv_devctl_io_create(p_basic_device_area, p_osal_io_area);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Register interrupt function and enable clock */
            func_ret = r_imrdrv_mainctl_prepare_irq_pm(dev_irq_priority, handler_func,
                                                       handle, p_osal_io_area);

            /* Check the return value is not OK */
            if (IMRDRV_ERROR_OK != func_ret)
            {
                e_imrdrv_errorcode_t    error_handling_ret;

                /* Destroy the IO resource */
                error_handling_ret = r_imrdrv_devctl_io_destroy(p_osal_io_area);

                /* Check the return value is not OK */
                if (IMRDRV_ERROR_OK != error_handling_ret)
                {
                    /** Do nothing because this function returns always ERROR_FAIL
                     * regardless of the result of r_imrdrv_devctl_io_destroy() */
                }
            }
        }   /* end of if (IMRDRV_ERROR_OK == func_ret) */
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }   /* end of if ((NULL != p_basic_device_area) && ...) */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_prepare_io()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_prepare_irq_pm
* Description  : Register interrupt function and enable clock to start the channel.
* Arguments    : dev_irq_priority           - Interrupt priority
*              : handler_func               - Callback function for interrupt handler of OSAL
*              : handle                     - Control structure handle (Argument of the callback function)
*              : p_osal_io_area             - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_005]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_prepare_irq_pm (
    const e_osal_interrupt_priority_t       dev_irq_priority,
    const p_osal_isr_func_t                 handler_func,
    imrdrv_ctrl_handle_t                    handle,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != handler_func) &&
        (NULL != handle) &&
        (NULL != p_osal_io_area))
    {
        /* Register the interrupt function */
        func_ret = r_imrdrv_osal_io_register_irq(dev_irq_priority, handler_func,
                                                 handle, p_osal_io_area);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Enable clock for specified channel */
            func_ret = r_imrdrv_osal_io_ctl_pm(p_osal_io_area, IMRDRV_OSAL_PM_CMD_CPG_ENABLE);

            /* Check the return value is not OK */
            if (IMRDRV_ERROR_OK != func_ret)
            {
                e_imrdrv_errorcode_t    error_handling_ret;

                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;

                /* Unregister the interrupt function */
                error_handling_ret = r_imrdrv_osal_io_unregister_irq(p_osal_io_area);

                /* Check the return value is not OK */
                if (IMRDRV_ERROR_OK != error_handling_ret)
                {
                    /** Do nothing because this function returns always ERROR_FAIL
                     * regardless of the result of r_imrdrv_osal_io_unregister_irq() */
                }
            }
        }   /* end of if (IMRDRV_ERROR_OK == func_ret) */
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }   /* end of if ((NULL != handler_func) && ...) */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_prepare_irq_pm()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_clear_attr
* Description  : Clear all attribute parameters in the work area.
* Arguments    : p_attr_param   - Address of the attribute parameter
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_018]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t  r_imrdrv_mainctl_clear_attr (
    st_imrdrv_attr_param_t                  *const p_attr_param
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if (NULL != p_attr_param)
    {
        /* Clear the attribute parameter. */
        p_attr_param->dl_data.phys_addr = 0U;

        p_attr_param->src_data.height       =  0U;
        p_attr_param->src_data.width        =  0U;
        p_attr_param->src_data.stride       =  0U;
        p_attr_param->src_data.phys_addr    =  0U;
        p_attr_param->src_data.color_format = IMRDRV_COLOR_FORM_INVALID;
        p_attr_param->src_data.bpp          = IMRDRV_BPP_INVALID;
        p_attr_param->src_data.uv_set.uv_stride    =  0U;
        p_attr_param->src_data.uv_set.uv_phys_addr =  0U;
        p_attr_param->src_data.uv_set.uv_bpp       = IMRDRV_BPP_INVALID;

        p_attr_param->dst_data.height       =  0U;
        p_attr_param->dst_data.width        =  0U;
        p_attr_param->dst_data.stride       =  0U;
        p_attr_param->dst_data.phys_addr    =  0U;
        p_attr_param->dst_data.color_format = IMRDRV_COLOR_FORM_INVALID;
        p_attr_param->dst_data.bpp          = IMRDRV_BPP_INVALID;
        p_attr_param->dst_data.uv_set.uv_stride    =  0U;
        p_attr_param->dst_data.uv_set.uv_phys_addr =  0U;
        p_attr_param->dst_data.uv_set.uv_bpp       = IMRDRV_BPP_INVALID;

        p_attr_param->triangle_mode.uvshfval = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.shfval   = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.uvshfe   = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.shfe     = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.rde      = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.tfe      = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.tcm      = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.autosg   = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.autodg   = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.bfe      = IMRDRV_MODE_INVALID;
        p_attr_param->triangle_mode.tme      = IMRDRV_MODE_INVALID;
    }
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_clear_attr()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_chk_comb_tri
* Description  : Check the combination between color format, bpp and triangle mode.
* Arguments    : src_color_format       - Color format of the source image data
*              : src_bpp                - Bpp of the source image data
*              : dst_color_format       - Color format of the destination image data
*              : dst_bpp                - Bpp of the destination image data
*              : p_triangle_mode        - Triangle mode
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_02_019]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_comb_tri (
    const e_imrdrv_color_format_t           src_color_format,
    const e_imrdrv_bpp_t                    src_bpp,
    const e_imrdrv_color_format_t           dst_color_format,
    const e_imrdrv_bpp_t                    dst_bpp,
    const st_imrdrv_triangle_mode_t         *const p_triangle_mode
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if (NULL == p_triangle_mode)
    {
        func_ret = IMRDRV_ERROR_FAIL;
    }
    else if ((IMRDRV_COLOR_FORM_INVALID == src_color_format  ) ||
             (IMRDRV_BPP_INVALID        == src_bpp           ) ||
             (IMRDRV_COLOR_FORM_INVALID == dst_color_format  ) ||
             (IMRDRV_BPP_INVALID        == dst_bpp           ) ||
             (IMRDRV_MODE_INVALID       == p_triangle_mode->tme))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else if ((IMRDRV_COLOR_FORM_DATA == src_color_format) &&
             ((IMRDRV_MODE_OFF == p_triangle_mode->tme     ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->uvshfval) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->shfval  ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->uvshfe  ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->shfe    ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->rde     ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->tfe     ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->tcm     ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->autosg  ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->autodg  ) ||
              (IMRDRV_MODE_ON  == p_triangle_mode->bfe     )))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else if ((IMRDRV_MODE_OFF == p_triangle_mode->tme) &&
             ((src_color_format != dst_color_format) ||
              (src_bpp          != dst_bpp         )))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else
    {
        /* Do nothing because only return success in this case */
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_chk_comb_tri()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_mainctl_set_attr_reg
* Description  : Writes parameters to registers.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
*              : p_reg_value_offset     - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_02_017]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_mainctl_set_attr_reg (
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t          *const p_osal_io_area,
    const st_imrdrv_attr_register_value_t   *const p_reg_value_offset
)
{
    /* Deeclare internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area) && (NULL != p_reg_value_offset))
    {
        uint32_t reg_cnt = 0U;
        
        /* Check the number of setting values is within the range of array */
        if (p_reg_value_offset->set_num <= IMRDRV_ATTR_SET_REG_NUM)
        {
            while ((reg_cnt < p_reg_value_offset->set_num) && (IMRDRV_ERROR_OK == func_ret))
            {
                /* Write parameters to registers */
                func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area,
                                                     p_reg_value_offset->reg_offset_value[reg_cnt].offset,
                                                     p_reg_value_offset->reg_offset_value[reg_cnt].value,
                                                     p_osal_io_area);
                reg_cnt++;
            }
            
            /* Check the return value is not OK or ERROR_IO */
            if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
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
/***********************************************************************************************************************
* End of function r_imrdrv_mainctl_set_attr_reg()
***********************************************************************************************************************/

