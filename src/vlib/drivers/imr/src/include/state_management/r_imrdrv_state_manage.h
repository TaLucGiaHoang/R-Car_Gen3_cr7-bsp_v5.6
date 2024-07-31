/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_state_manage.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver's sub-component public header of State Management
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 02.04.2021 0.01     New create
 *         : 14.06.2021 0.02     Add member to e_imrdrv_proc_type_t.
 *         : 29.10.2021 0.03     Add function prototype r_imrdrv_state_get_state().
 *********************************************************************************************************************/
#ifndef R_IMRDRV_STATE_MANAGE_H
#define R_IMRDRV_STATE_MANAGE_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
typedef enum
{
    IMRDRV_PROC_INIT     = 0,       /**< Processing of R_IMRDRV_Init                                             */
    IMRDRV_PROC_QUIT,               /**< Processing of R_IMRDRV_Quit                                             */
    IMRDRV_PROC_START,              /**< Processing of R_IMRDRV_Start                                            */
    IMRDRV_PROC_STOP,               /**< Processing of R_IMRDRV_Stop                                             */
    IMRDRV_PROC_EXECUTE,            /**< Processing of R_IMRDRV_Execute                                          */
    IMRDRV_PROC_ATTR_SET,           /**< Processing of R_IMRDRV_AttrSetParam.                                    */
    IMRDRV_PROC_OPT_ATTR_SET,       /**< Processing of R_IMRDRV_AttrSetXxx in optional IMR Driver API.           */
    IMRDRV_PROC_CALLBACK,           /**< Processing of callback to be called at completion of IMR-LX4 processing */
    IMRDRV_PROC_SM_CONFREG,         /**< Processing of Safety Mechanism (Configuration Register Check)           */
    IMRDRV_PROC_SM_MODULESTOP,      /**< Processing of Safety Mechanism (Unintended Module Stop Check)           */
    IMRDRV_PROC_PM_POLICY,          /**< Processing of Power Management Policy.                                  */
    IMRDRV_PROC_OTHER               /**< Other processing                                                        */
} e_imrdrv_proc_type_t;

typedef struct
{
    e_imrdrv_state_t state;    /* IMR Driver's state. */
} st_imrdrv_state_manage_area_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t r_imrdrv_state_check_state (
    const st_imrdrv_state_manage_area_t *const p_state_mng_area,
    const e_imrdrv_proc_type_t          proc_type
);
extern e_imrdrv_errorcode_t r_imrdrv_state_update_state (
    st_imrdrv_state_manage_area_t   *const p_state_mng_area,
    const e_imrdrv_proc_type_t       proc_type
);
extern e_imrdrv_errorcode_t r_imrdrv_state_preset_state (st_imrdrv_state_manage_area_t *const p_state_mng_area);

extern e_imrdrv_errorcode_t r_imrdrv_state_get_state(
    const st_imrdrv_state_manage_area_t *const p_state_mng_area,
    e_imrdrv_state_t                    *const p_state
);

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_STATE_MANAGE_H */
