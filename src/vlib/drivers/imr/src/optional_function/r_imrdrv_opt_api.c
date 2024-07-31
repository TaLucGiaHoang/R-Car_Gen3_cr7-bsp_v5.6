/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_opt_api.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of Optional Function APIs
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14.06.2021 0.01     New create
 *         : 29.10.2021 0.02     Modify functions for Gen3e.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetRotatorScaler
* Description  : Set specified rotator and scaler parameters to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - The attribute of rotator and scaler.
* Return Value : IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_03_01_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetRotatorScaler (
    const imrdrv_ctrl_handle_t      handle,
    const st_imrdrv_attr_rs_param_t *const p_param
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        /* Set parameter error to return value because not support in Gen3e */
        func_ret = IMRDRV_ERROR_PAR;
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
* End of function R_IMRDRV_AttrSetRotatorScaler()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetCacheMode
* Description  : Set specified cache mode parameter to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - The attribute of cache mode
* Return Value : IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_03_01_002]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetCacheMode (
    const imrdrv_ctrl_handle_t        handle,
    const st_imrdrv_attr_cache_mode_t *const p_param
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        /* Set parameter error to return value because not support in Gen3e */
        func_ret = IMRDRV_ERROR_PAR;
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
* End of function R_IMRDRV_AttrSetCacheMode()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetSyncModule
* Description  : Set specified sync module parameter to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - The attribute of sync module.
* Return Value : IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_03_01_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetSyncModule (
    const imrdrv_ctrl_handle_t        handle,
    const st_imrdrv_attr_sync_param_t *const p_param
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        /* Set parameter error to return value because not support in Gen3e */
        func_ret = IMRDRV_ERROR_PAR;
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
* End of function R_IMRDRV_AttrSetSyncModule()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetMipmap
* Description  : Set specified pyramidal image parameter to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - The attribute of pyramidal image
* Return Value : IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_03_01_004]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetMipmap (
    const imrdrv_ctrl_handle_t          handle,
    const st_imrdrv_attr_mipmap_param_t *const p_param
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        /* Set parameter error to return value because not support in Gen3e */
        func_ret = IMRDRV_ERROR_PAR;
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
* End of function R_IMRDRV_AttrSetMipmap()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetDoubleLumaPlane
* Description  : Set specified double luminance plane parameter to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - The attribute of double luminance plane
* Return Value : IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_03_01_005]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetDoubleLumaPlane (
    const imrdrv_ctrl_handle_t       handle,
    const st_imrdrv_attr_dyp_param_t *const p_param
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        /* Set parameter error to return value because not support in Gen3e */
        func_ret = IMRDRV_ERROR_PAR;
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
* End of function R_IMRDRV_AttrSetDoubleLumaPlane()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetGaussianFilter
* Description  : Set specified gaussian filter parameter to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - The attribute of gaussian filter.
* Return Value : IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_03_01_006]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_AttrSetGaussianFilter (
    const imrdrv_ctrl_handle_t          handle,
    const st_imrdrv_attr_gauss_filter_t *const p_param
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        /* Set parameter error to return value because not support in Gen3e */
        func_ret = IMRDRV_ERROR_PAR;
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
* End of function R_IMRDRV_AttrSetGaussianFilter()
***********************************************************************************************************************/
