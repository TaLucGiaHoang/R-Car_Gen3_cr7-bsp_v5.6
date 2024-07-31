/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_api.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver's public header of API.
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 16.04.2021 0.01     New create
 *         : 10.05.2021 0.02     Add function header of R_IMRDRV_Start.
 *         : 14.06.2021 0.03     Add function header of R_IMRDRV_Quit, R_IMRDRV_Stop, R_IMRDRV_AttrSetParam, and
 *                               R_IMRDRV_Execute.
 *                               Modify precondition of R_IMRDRV_Start in function header.
 *         : 29.10.2021 0.04     Add declaration of R_IMRDRV_SetPMPolicy, R_IMRDRV_GetPMPolicy, R_IMRDRV_SetState,
 *                               R_IMRDRV_AttrSetRotatorScaler, R_IMRDRV_AttrSetCacheMode, R_IMRDRV_AttrSetMipmap,
 *                               R_IMRDRV_AttrSetDoubleLumaPlane, R_IMRDRV_AttrSetSyncModule and 
 *                               R_IMRDRV_AttrSetGaussianFilter.
 *********************************************************************************************************************/

#ifndef R_IMRDRV_API_H
#define R_IMRDRV_API_H
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup     IMRgroup IMR driver
 * @ingroup drivers
 * @brief   IMR API driver interface
 * @{
 * @}
 */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
/******************************************************************************************************************//**

/**
 * \defgroup IMRapi   API
 * @ingroup IMRgroup
 * @{
 * */

/******************************************************************************************************************//**
 * @func            R_IMRDRV_Init
 * @Brief           Initialize the channel for IMR Driver.
 * @pre             This API must be called at first. 
                    After call, this API must not be called until R_IMRDRV_Quit is completed.
 * @warning         The area pointed by p_callback_arg shall not be freed until the completion of R_IMRDRV_Quit.
 * @param[in]       p_os_config             OSAL configuration.
 * @param[in]       callback_func           Callback function.
 * @param[in]       p_callback_arg          Argument of the callback function.
 * @param[in,out]   p_initdata              Initialize data.
 * @param[out]      p_handle                Control structure handle.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_FAIL
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_Init(
    const st_imrdrv_initdata_t     *const p_initdata,
    const st_imrdrv_os_config_t    *const p_os_config,
    const p_imrdrv_callback_func_t callback_func,
    void                           *const p_callback_arg,
    imrdrv_ctrl_handle_t           *const p_handle
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_Quit
 * @Brief           Uninitialize the channel for IMR Driver.
 * @pre             Must be in IMRDRV_STATE_INIT.
 * @param[in]       handle                 Control structure handle.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_Quit(
    const imrdrv_ctrl_handle_t handle
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_Start
 * @Brief           Start the channel for IMR Driver.
 * @pre             Must be in IMRDRV_STATE_INIT.
 * @param[in]       handle                 Control structure handle.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_IO
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_Start(
    const imrdrv_ctrl_handle_t handle
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_Stop
 * @Brief           Stop the channel for IMR Driver.
 * @pre             Must be in IMRDRV_STATE_READY or IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_IO
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_Stop(
    const imrdrv_ctrl_handle_t handle
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetParam
 * @Brief           Set specified parameter to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_READY.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                Attribute parameter
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_AttrSetParam(
    const imrdrv_ctrl_handle_t handle,
    const st_imrdrv_attr_param_t *const p_param
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_Execute
 * @Brief           Set specified parameter to the register and executes DL.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_CONF (Not supported)
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_IO
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_Execute(
    const imrdrv_ctrl_handle_t     handle
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_SetPMPolicy
 * @Brief           Set power management policy by OSAL.
 * @pre             Must be in IMRDRV_STATE_READY/IMRDRV_STATE_ATTR_SET/IMRDRV_STATE_ACTIVE.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       policy                 Power management policy.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_SetPMPolicy(
    const imrdrv_ctrl_handle_t     handle,
    const e_osal_pm_policy_t       policy
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_GetPMPolicy
 * @Brief           Get current power management policy by OSAL.
 * @pre             Must be in IMRDRV_STATE_READY/IMRDRV_STATE_ATTR_SET/IMRDRV_STATE_ACTIVE.
 * @param[in]       handle                 Control structure handle.
 * @param[out]      p_policy               Power management policy.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_FAIL
 * @retval          IMRDRV_ERROR_STATE
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_GetPMPolicy(
    const imrdrv_ctrl_handle_t     handle,
    e_osal_pm_policy_t             *const p_policy
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_GetState
 * @Brief           Gets the IMR Driver's state.
 * @pre             Must be in IMRDRV_STATE_INIT/IMRDRV_STATE_READY/IMRDRV_STATE_ATTR_SET/IMRDRV_STATE_ACTIVE.
 * @param[in]       handle                 Control structure handle.
 * @param[out]      p_state                the IMR Driver's state.
 * @retval          IMRDRV_ERROR_OK
 * @retval          IMRDRV_ERROR_PAR
 * @retval          IMRDRV_ERROR_TIME
 * @retval          IMRDRV_ERROR_FAIL
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_GetState(
    const imrdrv_ctrl_handle_t     handle,
    e_imrdrv_state_t               *const p_state
);


/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetRotatorScaler
 * @Brief           Set specified rotator and scaler parameters to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                The attribute of rotator and scaler.
 * @retval          IMRDRV_ERROR_PAR
 *********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetRotatorScaler (
    const imrdrv_ctrl_handle_t       handle,
    const st_imrdrv_attr_rs_param_t *const p_param
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetCacheMode
 * @Brief           Set specified cache mode parameter to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                The attribute of cache mode.
 * @retval          IMRDRV_ERROR_PAR
 *********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetCacheMode (
    const imrdrv_ctrl_handle_t         handle,
    const st_imrdrv_attr_cache_mode_t *const p_param
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetMipmap
 * @Brief           Set specified pyramidal image parameter to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                The attribute of pyramidal image.
 * @retval          IMRDRV_ERROR_PAR
 *********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetMipmap (
    const imrdrv_ctrl_handle_t           handle,
    const st_imrdrv_attr_mipmap_param_t *const p_param
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetDoubleLumaPlane
 * @Brief           Set specified double luminance plane parameter to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                The attribute of double luminance plane.
 * @retval          IMRDRV_ERROR_PAR
 *********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetDoubleLumaPlane (
    const imrdrv_ctrl_handle_t        handle,
    const st_imrdrv_attr_dyp_param_t *const p_param
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetSyncModule
 * @Brief           Set specified sync module parameter to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                The attribute of sync module.
 * @retval          IMRDRV_ERROR_PAR
 *********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetSyncModule (
    const imrdrv_ctrl_handle_t         handle,
    const st_imrdrv_attr_sync_param_t *const p_param
);

/******************************************************************************************************************//**
 * @func            R_IMRDRV_AttrSetGaussianFilter
 * @Brief           Set specified gaussian filter parameter to the work area of IMR Driver.
 * @pre             Must be in IMRDRV_STATE_ATTR_SET.
 * @param[in]       handle                 Control structure handle.
 * @param[in]       p_param                The attribute of gaussian filter.
 * @retval          IMRDRV_ERROR_PAR
 *********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetGaussianFilter (
    const imrdrv_ctrl_handle_t           handle,
    const st_imrdrv_attr_gauss_filter_t *const p_param
);

/** @} api */

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_API_H */

