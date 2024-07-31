/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_osal_io.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver's sub-component public header of OSAL Wrapper IO
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.04.2021 0.01     New create
 *         : 10.05.2021 0.02     Modify name of macro and enumeration.
 *                               Add member handler_func to st_imrdrv_osal_io_area_t.
 *                               Modify/Add function prototype.
 *         : 12.05.2021 0.03     Modify name of enumeration.
 *         : 14.06.2021 0.04     Add macro definition and function prototype.
 *         : 29.10.1021 0.05     Add function prototype(r_imrdrv_osal_io_set_pm_policy, r_imrdrv_osal_io_get_pm_policy
 *                               and r_imrdrv_osal_io_get_pm_state)
 *********************************************************************************************************************/

#ifndef R_IMRDRV_OSAL_IO_H
#define R_IMRDRV_OSAL_IO_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "rcar-xos/osal/r_osal.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* The maximum size of device name including null character. */
#define IMRDRV_OSAL_DEV_NAME_MAX_SIZE   (OSAL_DEVICE_NAME_MAX_LENGTH + 1U)

/* The maximum size of device type including null character. */
#define IMRDRV_OSAL_DEV_TYPE_MAX_SIZE   (OSAL_DEVICE_TYPE_NAME_MAX_LENGTH + 1U)

/* The IRQ index to specify the IRQ in OSAL */
#define IMRDRV_OSAL_IRQ_INDEX           (0U)
    
/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef enum
{
    IMRDRV_OSAL_PM_CMD_CPG_ENABLE  = 0,      /**< Enable clock. */
    IMRDRV_OSAL_PM_CMD_CPG_STOP              /**< Stop clock. */
} e_imrdrv_osal_pm_cmd_t;

typedef char char_t;

typedef struct
{
    osal_device_handle_t     io_handle;       /* OSAL IO handle */
    p_osal_isr_func_t        handler_func;    /* Callback function for interrupt handler of OSAL */
} st_imrdrv_osal_io_area_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t r_imrdrv_osal_io_create (
    const char_t                *const p_device_type,
    const char_t                *const p_device_name,
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_ctl_pm (
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    const e_imrdrv_osal_pm_cmd_t    cmd
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_register_irq (
    const e_osal_interrupt_priority_t   dev_irq_priority,
    const p_osal_isr_func_t             handler_func,
    imrdrv_ctrl_handle_t                handle,
    st_imrdrv_osal_io_area_t            *const p_osal_io_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_write_reg (
    const uint32_t                  offset,
    const uint32_t                  value,
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_destroy(
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_unregister_irq (
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_check_priority(
    const e_osal_interrupt_priority_t   dev_irq_priority
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_read_reg (
    const uint32_t                  offset,
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    uint32_t                        *const p_value
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_set_pm_policy(
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    const e_osal_pm_policy_t        policy
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_get_pm_policy(
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    e_osal_pm_policy_t              *const p_policy
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_io_get_pm_state(
    const st_imrdrv_osal_io_area_t  *const p_osal_io_area,
    e_osal_pm_state_t        *const p_pm_state
);

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_OSAL_IO_H */

