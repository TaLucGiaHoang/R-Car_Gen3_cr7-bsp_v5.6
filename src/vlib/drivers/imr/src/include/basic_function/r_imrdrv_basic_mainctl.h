/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_mainctl.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver Basic Function Main Control Public header
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.04.2021 0.01     New create
 *         : 10.05.2021 0.02     Add function prototype.
 *         : 14.06.2021 0.03     Add structure member to st_imrdrv_basic_mainctl_area_t.
 *                               Add function prototype.
 *         : 29.10.2021 0.04     Add function prototype(r_imrdrv_mainctl_set_pm_policy, r_imrdrv_mainctl_get_pm_policy).
 *********************************************************************************************************************/

#ifndef R_IMRDRV_BASIC_MAINCTL_H
#define R_IMRDRV_BASIC_MAINCTL_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "r_imrdrv_basic_dev.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    e_osal_interrupt_priority_t dev_irq_priority;   /* Interrupt priority */
    st_imrdrv_attr_param_t      attr_param;         /* The attribute parameter. */
} st_imrdrv_basic_mainctl_area_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t r_imrdrv_mainctl_init (
    const e_imrdrv_channelno_t          channel_no,
    const e_osal_interrupt_priority_t   dev_irq_priority,
    st_imrdrv_basic_device_area_t       *const p_basic_device_area,
    st_imrdrv_basic_mainctl_area_t      *const p_basic_mainctl_area
);

extern e_imrdrv_errorcode_t r_imrdrv_mainctl_quit (
    st_imrdrv_basic_mainctl_area_t    *const p_basic_mainctl_area,
    st_imrdrv_basic_device_area_t     *const p_basic_device_area
);

extern e_imrdrv_errorcode_t r_imrdrv_mainctl_start (
    const st_imrdrv_basic_mainctl_area_t    *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const p_osal_isr_func_t                 handler_func,
    imrdrv_ctrl_handle_t                    handle,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
);

extern e_imrdrv_errorcode_t r_imrdrv_mainctl_stop (
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    st_imrdrv_osal_io_area_t                *const p_osal_io_area
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_dl_data (
    const st_imrdrv_dl_t    *const p_dl_data
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_src_data (
    const st_imrdrv_data_t  *const p_src_data
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_dst_data (
    const st_imrdrv_data_t  *const p_dst_data
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_tri_mode (
    const st_imrdrv_triangle_mode_t   *const p_triangle_mode
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_img_tri (
    const e_imrdrv_color_format_t     src_color_format,
    const e_imrdrv_bpp_t              src_bpp,
    const e_imrdrv_color_format_t     dst_color_format,
    const e_imrdrv_bpp_t              dst_bpp,
    const st_imrdrv_triangle_mode_t   *const p_triangle_mode
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_attr_set_param  (
    const st_imrdrv_attr_param_t         *const p_param,
    st_imrdrv_basic_mainctl_area_t       *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t  *const p_basic_device_area
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_chk_attr_opt (
    const st_imrdrv_basic_mainctl_area_t *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t  *const p_basic_device_area
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_execute (
    const st_imrdrv_basic_mainctl_area_t *const p_basic_mainctl_area,
    const st_imrdrv_basic_device_area_t  *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t       *const p_osal_io_area
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_set_pm_policy (
    const st_imrdrv_osal_io_area_t       *const p_osal_io_area,
    const e_osal_pm_policy_t             policy
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_get_pm_policy (
    const st_imrdrv_osal_io_area_t       *const p_osal_io_area,
    e_osal_pm_policy_t                   *const p_policy
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_start_render (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
);

e_imrdrv_errorcode_t r_imrdrv_mainctl_proc_callback (
    const st_imrdrv_basic_device_area_t     *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t          *const p_osal_io_area,
    uint32_t                                *const p_sr_val
);
#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_BASIC_MAINCTL_H */

