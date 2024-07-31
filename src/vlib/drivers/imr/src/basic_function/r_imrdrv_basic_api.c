/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_api.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of Basic Function APIs
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.04.2021 0.01     New create
 *         : 10.05.2021 0.02     Add R_IMRDRV_Start(), r_imrdrv_api_callback() and r_imrdrv_api_start().
 *         : 12.05.2021 0.03     Move declaration of local variable to top of the block.
 *         : 09.06.2021 0.04     Imprement r_imrdrv_api_callback().
 *         : 14.06.2021 0.05     Remove unnecessary header file include.
 *                               Add macro definition of IMRDRV_USR_CB_ERROR_OK.
 *                               Add R_IMRDRV_Quit(), R_IMRDRV_Stop(), R_IMRDRV_AttrSetParam(), R_IMRDRV_Execute(),
 *                               R_IMRDRV_GetVersion(), r_imrdrv_api_quit(), r_imrdrv_api_stop(),
 *                               r_imrdrv_api_chk_attr_set_param(), r_imrdrv_api_execute(), and
 *                               r_imrdrv_api_start_render().
 *         : 29.10.2021 0.06     Add R_IMRDRV_SetPMPolicy(), R_IMRDRV_GetPMPolicy() and R_IMRDRV_GetState().
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_api.h"
#include "rcar-xos/imr/r_imrdrv_version.h"
#include "r_imrdrv_private.h"
#include "r_imrdrv_basic_func_private.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Return value of User callback function */
#define IMRDRV_USR_CB_ERROR_OK  (0)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_check_arg_init (
    const st_imrdrv_initdata_t      *const p_initdata,
    const st_imrdrv_os_config_t     *const p_os_config,
    const p_imrdrv_callback_func_t   callback_func,
    const imrdrv_ctrl_handle_t      *const p_handle
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_init (
    const e_imrdrv_channelno_t       channel_no,
    const st_imrdrv_os_config_t     *const p_os_config,
    const p_imrdrv_callback_func_t   callback_func,
    void                            *const p_callback_arg,
    st_imrdrv_work_area_data_t      *const p_work_area
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_update_state_init (
    st_imrdrv_work_area_data_t  *const p_work_area
);

IMRDRV_STATIC void r_imrdrv_api_callback (
    osal_device_handle_t    device_handle,
    uint64_t                irq_channel,
    void                    *p_usr_arg
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_quit (
    st_imrdrv_work_area_data_t  *const p_work_area
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_start (
    st_imrdrv_work_area_data_t  *const p_work_area
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_stop (
    st_imrdrv_work_area_data_t  *const p_work_area
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_chk_attr_set_param (
    const st_imrdrv_attr_param_t *const p_attr_param
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_execute (
    const st_imrdrv_work_area_data_t *const p_work_area
);

IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_start_render (
    st_imrdrv_work_area_data_t *const p_work_area
);


/**********************************************************************************************************************
* Function Name: R_IMRDRV_Init
* Description  : Initializes the channel for IMR Driver.
* Arguments    : p_initdata        - Initialize data
*              : p_os_config       - OSAL configuration
*              : callback_func     - Callback function
*              : p_callback_arg    - Argument of the callback function
*              : p_handle          - Control structure handle
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_01_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_Init (
    const st_imrdrv_initdata_t      *const p_initdata,
    const st_imrdrv_os_config_t     *const p_os_config,
    const p_imrdrv_callback_func_t   callback_func,
    void                            *const p_callback_arg,
    imrdrv_ctrl_handle_t            *const p_handle
)
{
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameters */
    func_ret = r_imrdrv_api_check_arg_init(p_initdata, p_os_config, callback_func, p_handle);

    /* Check the result of checking parameter */
    if (IMRDRV_ERROR_OK == func_ret)
    {
        st_imrdrv_work_area_data_t *p_work_area;

        /* Initialize p_handle */
        *p_handle = NULL;

        /**************************************************************************************************************
         * MISRA Rule: Rule 11.5
         * Reason: Access to the area pointed by general-purpose pointer.
         * Verification: This cast is no problem because the area pointed by p_work_addr has sufficient size
         * compared with that of st_imrdrv_work_area_data_t and the area used by only IMR Driver.
         *************************************************************************************************************/
        p_work_area = (st_imrdrv_work_area_data_t *)p_initdata->p_work_addr;

        /* Clear and initialize work area */
        func_ret = r_imrdrv_api_init(p_initdata->channel_no, p_os_config, callback_func, p_callback_arg, p_work_area);

        /* Check the result of initializing work area */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Update the IMR Driver's state */
            func_ret = r_imrdrv_api_update_state_init(p_work_area);

            /* Check the result of updating the IMR Driver's state */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /******************************************************************************************************
                 * MISRA Rule: Rule 11.2
                 * Reason: Hide structure type from User Application.
                 * Verification: This cast is no problem because st_imrdrv_work_area_data_t* and
                 * imrdrv_ctrl_handle_t are the same type.
                 *****************************************************************************************************/
                /* Set the address of work area of target channel */
                *p_handle = (imrdrv_ctrl_handle_t)p_work_area;
            }
        } /* end of if (IMRDRV_ERROR_OK == func_ret) */
    } /* end of if (IMRDRV_ERROR_OK == func_ret) */

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_Init()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_Quit
* Description  : Uninitialize the channel for IMR Driver.
* Arguments    : handle         - Control structure handle
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_002]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_Quit (
    const imrdrv_ctrl_handle_t  handle
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if (NULL != handle)
    {
        st_imrdrv_work_area_data_t *p_work_area;

        p_work_area = (st_imrdrv_work_area_data_t *)handle;

        /* Check the IMR Driver's state */
        func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_QUIT);

        /* Check the result of checking the IMR Driver's state */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Quit the channel */
            func_ret = r_imrdrv_api_quit(p_work_area);
        }
        else
        {
            /* Check the return value is not ERROR_STATE */
            if (IMRDRV_ERROR_STATE != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    } /* end of if (NULL != handle) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_Quit()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_Start
* Description  : Start the channel for IMR Driver.
* Arguments    : handle         - Control structure handle
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_Start (
    const imrdrv_ctrl_handle_t  handle
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameters */
    if (NULL != handle)
    {
        st_imrdrv_work_area_data_t *p_work_area;

        /******************************************************************************************************
         * MISRA Rule: Rule 11.2
         * Reason: Hide structure type from User Application.
         * Verification: This cast is no problem because st_imrdrv_work_area_data_t* and
         * imrdrv_ctrl_handle_t are the same type.
         *****************************************************************************************************/
        p_work_area = (st_imrdrv_work_area_data_t *)handle;

        /* Lock the mutex */
        func_ret = r_imrdrv_osal_mutex_lock(&p_work_area->osal_mutex_area);

        /* Check the result of locking the mutex */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            e_imrdrv_errorcode_t mutex_ret;

            /* Check the IMR Driver's state */
            func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_START);

            /* Check the result of updating the IMR Driver's state */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Start the channel */
                func_ret = r_imrdrv_api_start(p_work_area);
            }
            else
            {
                /* Check the return value is not ERROR_STATE */
                if (IMRDRV_ERROR_STATE != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }

            /* Unlock the mutex */
            mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work_area->osal_mutex_area);

            /* Check the result of unlocking the mutex */
            if (IMRDRV_ERROR_OK != mutex_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }   /* end of if (IMRDRV_ERROR_OK == func_ret) */
        else
        {
            /* Check the return value is not ERROR_TIME */
            if (IMRDRV_ERROR_TIME != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }   /* end of if (NULL != handle) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_Start()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_Stop
* Description  : Stop the channel for IMR Driver.
* Arguments    : handle         - Control structure handle
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_004]
**********************************************************************************************************************/
e_imrdrv_errorcode_t R_IMRDRV_Stop (
    const imrdrv_ctrl_handle_t      handle
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameters */
    if (NULL != handle)
    {
        st_imrdrv_work_area_data_t  *p_work_area;
        e_imrdrv_errorcode_t        mutex_ret;

        p_work_area = (st_imrdrv_work_area_data_t *)handle;

        /* Lock the mutex */
        func_ret = r_imrdrv_osal_mutex_lock(&p_work_area->osal_mutex_area);

        /* Check the result of locking the mutex */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Check the IMR Driver's state */
            func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_STOP);

            /* Check the result of checking the IMR Driver's state */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Stop the channel */
                func_ret = r_imrdrv_api_stop(p_work_area);
            }
            else
            {
                /* Check the return value is not ERROR_STATE */
                if (IMRDRV_ERROR_STATE != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }

            /* Unlock the mutex */
            mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work_area->osal_mutex_area);

            /* Check the result of unlocking the mutex */
            if (IMRDRV_ERROR_OK != mutex_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        } /* end of if (IMRDRV_ERROR_OK == func_ret) */
        else
        {
            /* Check the return value is not ERROR_TIME */
            if (IMRDRV_ERROR_TIME != func_ret)
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
* End of function R_IMRDRV_Stop()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_AttrSetParam
* Description  : Set specified parameter to the work area of IMR Driver.
* Arguments    : handle         - Control structure handle
*              : p_param        - Attribute parameter
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_005]
**********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_AttrSetParam(
    const imrdrv_ctrl_handle_t     handle,
    const st_imrdrv_attr_param_t  *const p_param
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if ((NULL != handle) && (NULL != p_param))
    {
        func_ret = r_imrdrv_api_chk_attr_set_param(p_param);

        if (IMRDRV_ERROR_OK == func_ret)
        {
            st_imrdrv_work_area_data_t *p_work;

            p_work = (st_imrdrv_work_area_data_t *)handle;
            /* Lock the mutex */
            func_ret = r_imrdrv_osal_mutex_lock(&p_work->osal_mutex_area);

            /* Check the result of locking the mutex */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                e_imrdrv_errorcode_t mutex_ret;

                /* Check the IMR Driver's state */
                func_ret = r_imrdrv_state_check_state(&p_work->state_manage_area, IMRDRV_PROC_ATTR_SET);

                /* Check the result of checking the IMR Driver's state */
                if (IMRDRV_ERROR_OK == func_ret)
                {
                    /* Set attribute parameter to work area */
                    func_ret = r_imrdrv_mainctl_attr_set_param(p_param,
                                                               &p_work->basic_mainctl_area,
                                                               &p_work->basic_device_area);

                    /* Check the result of setting attribute parameter to work area */
                    if (IMRDRV_ERROR_OK == func_ret)
                    {
                        /* Update the IMR Driver's state */
                        func_ret = r_imrdrv_state_update_state(&p_work->state_manage_area, IMRDRV_PROC_ATTR_SET);

                        /* Check the result of updating the state */
                        if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_STATE != func_ret))
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
                    /* Check the return value is not ERROR_STATE */
                    if (IMRDRV_ERROR_STATE != func_ret)
                    {
                        /* Set unexpected error to return value */
                        func_ret = IMRDRV_ERROR_FAIL;
                    }
                }

                /* Unlock the mutex */
                mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work->osal_mutex_area);
                if (IMRDRV_ERROR_OK != mutex_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }
            else
            {
                /* Check the return value is not ERROR_TIME */
                if (IMRDRV_ERROR_TIME != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
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
* End of function R_IMRDRV_AttrSetParam()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_Execute
* Description  : Set specified parameter to the register and executes DL.
* Arguments    : handle         - Control structure handle
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_CONF (Not supported)
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_006]
**********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_Execute(
    const imrdrv_ctrl_handle_t     handle
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if (NULL != handle)
    {
        st_imrdrv_work_area_data_t *p_work_area;
        p_work_area = (st_imrdrv_work_area_data_t *)handle;
        /* Lock the mutex */
        func_ret = r_imrdrv_osal_mutex_lock(&p_work_area->osal_mutex_area);
        
        /* Check the result of locking the mutex */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            e_imrdrv_errorcode_t mutex_ret;
            
            /* Check the IMR Driver's state */
            func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_EXECUTE);
            
            /* Check the result of checking the IMR Driver's state */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Set specified parameter to the register */
                func_ret = r_imrdrv_api_execute(p_work_area);
                
                /* Check the result of setting specified parameter */
                if (IMRDRV_ERROR_OK == func_ret)
                {
                    /* Execute DL */
                    func_ret = r_imrdrv_api_start_render(p_work_area);
                }
            }
            else
            {
                /* Check the return value is not ERROR_STATE */
                if (IMRDRV_ERROR_STATE != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }

            /* Unlock the mutex */
            mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work_area->osal_mutex_area);
            /* Check the result of unlocking the mutex */
            if (IMRDRV_ERROR_OK != mutex_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Check the return value is not ERROR_TIME */
            if (IMRDRV_ERROR_TIME != func_ret)
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
* End of function R_IMRDRV_Execute()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_GetVersion
* Description  : Return the IMR Driver version
* Arguments    : None
* Return Value : The pointer of version information
* [Covers: UD_RD_UD01_01_01_007]
**********************************************************************************************************************/
const st_imrdrv_version_t* R_IMRDRV_GetVersion(void)
{
    static const st_imrdrv_version_t s_version = {
        IMRDRV_VERSION_MAJOR,
        IMRDRV_VERSION_MINOR,
        IMRDRV_VERSION_PATCH };
    return &s_version;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_GetVersion()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_SetPMPolicy
* Description  : This function sets the power management policy
* Arguments    : handle
*                policy
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_008]
**********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_SetPMPolicy(
    const imrdrv_ctrl_handle_t    handle,
    const e_osal_pm_policy_t      policy
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;
    /* variable for mutex unlock */
    e_imrdrv_errorcode_t mutex_ret;

    /* Check parameters */
    if (NULL == handle)
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else if ((OSAL_PM_POLICY_CG != policy) && (OSAL_PM_POLICY_PG != policy) && (OSAL_PM_POLICY_HP != policy))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter is OK */
    {
        st_imrdrv_work_area_data_t *p_work_area;
        p_work_area = (st_imrdrv_work_area_data_t *)handle;

        /* Lock the mutex */
        func_ret = r_imrdrv_osal_mutex_lock(&p_work_area->osal_mutex_area);
        /* Check the result of locking the mutex */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Check the IMR Driver's state */
            func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_PM_POLICY);
            /* Current state is Ready State/Attribute set State/Active State */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Setting PM Policy by OSAL */
                func_ret = r_imrdrv_mainctl_set_pm_policy(&p_work_area->osal_io_area, policy);
            }
            else
            {
                /* Check the return value is not ERROR_STATE */
                if (IMRDRV_ERROR_STATE != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }

            /* Unlock the mutex */
            mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work_area->osal_mutex_area);
            if(IMRDRV_ERROR_OK != mutex_ret)
            {
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Check the return value is not ERROR_TIME */
            if (IMRDRV_ERROR_TIME != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_SetPMPolicy()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_GetPMPolicy
* Description  : This function gets the current power management policy
* Arguments    : handle         - Control structure handle
*                p_policy       - Power management policy
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_009]
**********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_GetPMPolicy(
    const imrdrv_ctrl_handle_t    handle,
    e_osal_pm_policy_t            *const p_policy
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;
    /* variable for mutex unlock */
    e_imrdrv_errorcode_t mutex_ret;

    /* Check parameters */
    if ((NULL == handle) || (NULL == p_policy))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter is OK */
    {
        st_imrdrv_work_area_data_t *p_work_area;
        p_work_area = (st_imrdrv_work_area_data_t *)handle;

        /* Lock the mutex */
        func_ret = r_imrdrv_osal_mutex_lock(&p_work_area->osal_mutex_area);
        /* Check the result of locking the mutex */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Check the IMR Driver's state */
            func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_PM_POLICY);
            /* Current state is Ready State/Attribute set State/Active State */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Setting PM Policy by OSAL */
                func_ret = r_imrdrv_mainctl_get_pm_policy(&p_work_area->osal_io_area, p_policy);
            }
            else
            {
                /* Check the return value is not ERROR_STATE */
                if (IMRDRV_ERROR_STATE != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }

            /* Unlock the mutex */
            mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work_area->osal_mutex_area);
            if(IMRDRV_ERROR_OK != mutex_ret)
            {
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Check the return value is not ERROR_TIME */
            if (IMRDRV_ERROR_TIME != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_GetPMPolicy()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_IMRDRV_GetState
* Description  : This function gets current state.
* Arguments    : handle         - Control structure handle
*                p_state        - The IMR Driver's state
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_TIME
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_01_010]
**********************************************************************************************************************/
extern e_imrdrv_errorcode_t R_IMRDRV_GetState(
    const imrdrv_ctrl_handle_t    handle,
    e_imrdrv_state_t              *const p_state
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;
    /* variable for mutex unlock */
    e_imrdrv_errorcode_t mutex_ret;

    /* Check parameters */
    if ((NULL == handle) || (NULL == p_state))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else /* parameter is OK */
    {
        st_imrdrv_work_area_data_t *p_work_area;
        p_work_area = (st_imrdrv_work_area_data_t *)handle;

        /* Lock the mutex */
        func_ret = r_imrdrv_osal_mutex_lock(&p_work_area->osal_mutex_area);
        /* Check the result of locking the mutex */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            func_ret = r_imrdrv_state_get_state(&p_work_area->state_manage_area, p_state);

            /* Unlock the mutex */
            mutex_ret = r_imrdrv_osal_mutex_unlock(&p_work_area->osal_mutex_area);
            if(IMRDRV_ERROR_OK != mutex_ret)
            {
                func_ret = IMRDRV_ERROR_FAIL;
            }

        }
        else
        {
            /* Check the return value is not ERROR_TIME */
            if (IMRDRV_ERROR_TIME != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function R_IMRDRV_GetState()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_check_arg_init
* Description  : Check parameters set by User Application.
* Arguments    : p_initdata        - Initialize data
*              : p_os_config       - OSAL configuration
*              : callback_func     - Callback function
*              : p_handle          - Control structure handle
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_01_011]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_check_arg_init (
    const st_imrdrv_initdata_t      *const p_initdata,
    const st_imrdrv_os_config_t     *const p_os_config,
    const p_imrdrv_callback_func_t   callback_func,
    const imrdrv_ctrl_handle_t      *const p_handle
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_PAR;

    /* Check parameters */
    if ((NULL != p_initdata) && (NULL != p_os_config) && (NULL != callback_func) && (NULL != p_handle))
    {
        /* Check members of structures */
        if ((NULL != p_initdata->p_work_addr) &&
            (p_initdata->work_size >= IMRDRV_SIZE_WORKAREA) && 
            (p_os_config->mutex_wait_period >= IMRDRV_MUTEX_WAIT_TIME_MIN))
        {
            /* Set successful completion to return value */
            func_ret = IMRDRV_ERROR_OK;
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_check_arg_init()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_init
* Description  : Initializes the channel.
* Arguments    : channel_no         - Channel number
*              : p_os_config        - OSAL configuration
*              : callback_func      - Callback function
*              : p_callback_arg     - Argument of the callback function
*              : p_work_area        - Address of the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_01_012]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_init (
    const e_imrdrv_channelno_t       channel_no,
    const st_imrdrv_os_config_t     *const p_os_config,
    const p_imrdrv_callback_func_t   callback_func,
    void                            *const p_callback_arg,
    st_imrdrv_work_area_data_t      *const p_work_area
)
{
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_os_config) && (NULL != callback_func) && (NULL != p_work_area))
    {
        /* Clear and initialize work area */
        func_ret = r_imrdrv_mainctl_init(channel_no,
                                        p_os_config->dev_irq_priority,
                                        &p_work_area->basic_device_area,
                                        &p_work_area->basic_mainctl_area);

        /* Check the result of initializing work area */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Preset the IMR Driver's state */
            func_ret = r_imrdrv_state_preset_state(&p_work_area->state_manage_area);

            /* Check the result of presetting the state */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Create semaphore resource */
                func_ret = r_imrdrv_osal_mutex_create(p_os_config->mutex_id,
                                                        p_os_config->mutex_wait_period,
                                                        &p_work_area->osal_mutex_area);

                /* Check the result of creating semaphore resource is OK */
                if (IMRDRV_ERROR_OK == func_ret)
                {
                    /* Initialize the Basic Function APIs area */
                    p_work_area->basic_api_area.callback_func_app = callback_func;
                    p_work_area->basic_api_area.p_callback_arg = p_callback_arg;
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

            /* Check the return value is not OK */
            if (IMRDRV_ERROR_OK != func_ret)
            {
                e_imrdrv_errorcode_t    quit_ret;

                /* Clear work area */
                quit_ret = r_imrdrv_mainctl_quit(&p_work_area->basic_mainctl_area, &p_work_area->basic_device_area);

                /* Check the result of clear */
                if (IMRDRV_ERROR_OK != quit_ret)
                {
                    /** Do nothing because this function returns always ERROR_FAIL
                     * regardless of the result of r_imrdrv_mainctl_quit() */
                }
            }
        } /* end of if (IMRDRV_ERROR_OK == func_ret) */
    } /* end of if ((NULL != p_initdata) && ... */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_init()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_update_state_init
* Description  : Update IMR Driver's state to Initialized State.
* Arguments    : p_work_area       - Address of the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_01_013]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_update_state_init (
    st_imrdrv_work_area_data_t  *const p_work_area
)
{
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if (NULL != p_work_area)
    {
        func_ret = r_imrdrv_state_update_state(&p_work_area->state_manage_area, IMRDRV_PROC_INIT);

        /* Check the result of updating the IMR Driver's state */
        if (IMRDRV_ERROR_OK != func_ret)
        {
            e_imrdrv_errorcode_t    error_handling_ret;

            /* Clear Basic Function APIs area in the work area */
            p_work_area->basic_api_area.callback_func_app = NULL;
            p_work_area->basic_api_area.p_callback_arg = NULL;

            /* destroy semaphore resource */
            error_handling_ret = r_imrdrv_osal_mutex_destroy(&p_work_area->osal_mutex_area);

            /* Check the result of destroying semaphore resource */
            if (IMRDRV_ERROR_OK != error_handling_ret)
            {
                /** Do nothing because this function returns always ERROR_FAIL
                 * regardless of the result of r_imrdrv_osal_mutex_destroy() */
            }

            /* clear work area */
            error_handling_ret = r_imrdrv_mainctl_quit(&p_work_area->basic_mainctl_area,
                                                        &p_work_area->basic_device_area);

            /* Check the result of clear */
            if (IMRDRV_ERROR_OK != error_handling_ret)
            {
                /** Do nothing because this function returns always ERROR_FAIL
                 * regardless of the result of r_imrdrv_mainctl_quit() */
            }

            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        } /* end of if (IMRDRV_ERROR_OK != func_ret) */
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
* End of function r_imrdrv_api_update_state_init()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_callback
* Description  : Callback processing.
* Arguments    : device_handle     - IO handle
*              : irq_channel       - IRQ index
*              : p_usr_arg         - Argument of the callback function registered by OSAL user
* Return Value : -
* [Covers: UD_RD_UD01_01_01_020]
**********************************************************************************************************************/
IMRDRV_STATIC void r_imrdrv_api_callback (
    /******************************************************************************************************
     * MISRA Rule: Rule 8.13
     * Reason: This function is the callback function registered to OSAL.
     * The type of this argument defined by OSAL.
     * Verification: device_handle is designed not to change in this function.
     *****************************************************************************************************/
    osal_device_handle_t    device_handle,
    uint64_t                irq_channel,
    void                    *p_usr_arg
)
{
    e_imrdrv_errorcode_t func_ret;
    int32_t  cb_ret = IMRDRV_USR_CB_ERROR_OK;
    uint32_t sr_reg_val = 0U;

    /* Check parameters */
    if ((OSAL_DEVICE_HANDLE_INVALID != device_handle) &&
        (IMRDRV_OSAL_IRQ_INDEX == irq_channel) &&
        (NULL != p_usr_arg))
    {
        st_imrdrv_work_area_data_t *p_work_area;

        /******************************************************************************************************
         * MISRA Rule: Rule 11.5
         * Reason: Access to the area pointed by general-purpose pointer.
         * Verification: This cast is no problem because the area pointed by p_usr_arg has sufficient size
         * compared with that of st_imrdrv_work_area_data_t and the area used by only IMR Driver.
         *****************************************************************************************************/
        p_work_area = (st_imrdrv_work_area_data_t *)p_usr_arg;

        /* Check the IMR Driver's state */
        func_ret = r_imrdrv_state_check_state(&p_work_area->state_manage_area, IMRDRV_PROC_CALLBACK);

        /* Check the result of checking the IMR Driver's state */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Get result of IMR and mask IMR interrupt */
            func_ret = r_imrdrv_mainctl_proc_callback(&p_work_area->basic_device_area,
                                                      &p_work_area->osal_io_area, &sr_reg_val);

            /* Check the result of getting result of IMR and mask IMR interrupt */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                if (IMRDRV_REG_VAL_SR_INT != sr_reg_val)
                {
                    /* Update the IMR Driver's state */
                    func_ret = r_imrdrv_state_update_state(&p_work_area->state_manage_area, IMRDRV_PROC_CALLBACK);
                }

                /* Check the result of updating the IMR Driver's state */
                if (IMRDRV_ERROR_OK != func_ret)
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
            /* Check the return value is not ERROR_STATE */
            if (IMRDRV_ERROR_STATE != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }

        if (IMRDRV_REG_VAL_SR_INT != sr_reg_val)
        {
            /* Check the Callback function to User Application. */
            if (NULL != p_work_area->basic_api_area.callback_func_app)
            {
                cb_ret = p_work_area ->basic_api_area.callback_func_app(func_ret, sr_reg_val,
                                                                        p_work_area ->basic_api_area.p_callback_arg);
            }
            else
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    if ((IMRDRV_ERROR_OK != func_ret) ||
        (IMRDRV_USR_CB_ERROR_OK != cb_ret))
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    return;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_callback()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_quit
* Description  : Quit the channel.
* Arguments    : p_work_area        - Address of work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_014]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_quit (
    st_imrdrv_work_area_data_t  *const p_work_area
)
{
    /* Declare internal variable */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if (NULL != p_work_area)
    {
        /* Clear work area */
        func_ret = r_imrdrv_mainctl_quit(&p_work_area->basic_mainctl_area, &p_work_area->basic_device_area);

        /* Check the result of clear */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Clear the Basic Function APIs area in the work area */
            p_work_area->basic_api_area.callback_func_app = NULL;
            p_work_area->basic_api_area.p_callback_arg = NULL;

            /* Destroy semaphore resource */
            func_ret = r_imrdrv_osal_mutex_destroy(&p_work_area->osal_mutex_area);

            /* Check the result of destroying semaphore resource */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Update the IMR Driver's state */
                func_ret = r_imrdrv_state_update_state(&p_work_area->state_manage_area, IMRDRV_PROC_QUIT);

                /* Check the result of updating the state */
                if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_STATE != func_ret))
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
    } /* end of if (NULL != p_work_area) */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_quit()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_start
* Description  : Start the channel.
* Arguments    : p_work_area        - Address of work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_015]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_start (
    st_imrdrv_work_area_data_t  *const p_work_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if (NULL != p_work_area)
    {
        /* Start the channel */
        func_ret = r_imrdrv_mainctl_start(&p_work_area->basic_mainctl_area,
                                          &p_work_area->basic_device_area,
                                          r_imrdrv_api_callback,
                                          (imrdrv_ctrl_handle_t)p_work_area,
                                          &p_work_area->osal_io_area);

        /* Check the result of starting the channel */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Update the IMR Driver's state */
            func_ret = r_imrdrv_state_update_state(&p_work_area->state_manage_area, IMRDRV_PROC_START);

            /* Check the result of updating the state */
            if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_STATE != func_ret))
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }

            /* Check the return value is not OK */
            if (IMRDRV_ERROR_OK != func_ret)
            {
                e_imrdrv_errorcode_t    error_handling_ret;

                /* Stop the channel */
                error_handling_ret = r_imrdrv_mainctl_stop(&p_work_area->basic_device_area, &p_work_area->osal_io_area);

                /* Check the result of stopping the channel */
                if (IMRDRV_ERROR_OK != error_handling_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
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
    } /* end of if (NULL != p_work_area) */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_start()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_stop
* Description  : Stop the channel.
* Arguments    : p_work_area        - Address of work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_016]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_stop (
    st_imrdrv_work_area_data_t  *const p_work_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if (NULL != p_work_area)
    {
        /* Stop the channel */
        func_ret = r_imrdrv_mainctl_stop(&p_work_area->basic_device_area, &p_work_area->osal_io_area);

        /* Check the result of stopping the channel */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            func_ret = r_imrdrv_state_update_state(&p_work_area->state_manage_area, IMRDRV_PROC_STOP);

            /* Check the result of updating the state */
            if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_STATE != func_ret))
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
    } /* end of if (NULL != p_work_area) */
    else
    {
        /* Set unexpected error to return value */
        func_ret = IMRDRV_ERROR_FAIL;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_stop()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_chk_attr_set_param
* Description  : Check parameters set by User Application.
* Arguments    : p_attr_param       - Attribute parameter
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_01_017]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_chk_attr_set_param (
    const st_imrdrv_attr_param_t *const p_attr_param
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if (NULL != p_attr_param)
    {
        e_imrdrv_errorcode_t chk_dl_ret;
        e_imrdrv_errorcode_t chk_src_ret;
        e_imrdrv_errorcode_t chk_dst_ret;
        e_imrdrv_errorcode_t chk_tri_ret;

        /* Check the display list data */
        chk_dl_ret = r_imrdrv_mainctl_chk_dl_data(&(p_attr_param->dl_data));

        /* Check the source image data */
        chk_src_ret = r_imrdrv_mainctl_chk_src_data(&(p_attr_param->src_data));

        /* Check the source image data */
        chk_dst_ret = r_imrdrv_mainctl_chk_dst_data(&(p_attr_param->dst_data));

        /* Check the triangle mode */
        chk_tri_ret = r_imrdrv_mainctl_chk_tri_mode(&(p_attr_param->triangle_mode));

        if ((IMRDRV_ERROR_OK == chk_dl_ret ) &&
            (IMRDRV_ERROR_OK == chk_src_ret) &&
            (IMRDRV_ERROR_OK == chk_dst_ret) &&
            (IMRDRV_ERROR_OK == chk_tri_ret))
        {
            /* Check the combination between source / destination image data and triangle mode */
            func_ret = r_imrdrv_mainctl_chk_img_tri(
                           p_attr_param->src_data.color_format,
                           p_attr_param->src_data.bpp,
                           p_attr_param->dst_data.color_format,
                           p_attr_param->dst_data.bpp,
                           &p_attr_param->triangle_mode);
        }
        else
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
        }
    }   /* end of if (NULL != p_attr_param) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_api_chk_attr_set_param()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_execute
* Description  : Set specified parameter to the register
* Arguments    : p_work_area        - Address of work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_CONF (Not supported)
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_01_018]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_execute (
    const st_imrdrv_work_area_data_t *const p_work_area
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if (NULL != p_work_area)
    {
        /* Check the combination of the attribute parameter and the optional attribute parameter. */
        func_ret = r_imrdrv_mainctl_chk_attr_opt(&p_work_area->basic_mainctl_area,
                                                 &p_work_area->basic_device_area);

        /* Check the result of checking the combination of the attribute */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Execute software reset, writes parameters to registers */
            func_ret = r_imrdrv_mainctl_execute(&p_work_area->basic_mainctl_area,
                                                &p_work_area->basic_device_area,
                                                &p_work_area->osal_io_area);
            
            /* Check the result of executing software reset, writes parameters to registers */
            if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
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
* End of function r_imrdrv_api_execute()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_api_start_render
* Description  : Executes DL
* Arguments    : p_work_area        - Address of work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_01_01_019]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_api_start_render (
    st_imrdrv_work_area_data_t *const p_work_area
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameters */
    if (NULL != p_work_area)
    {
        /* Update the IMR Driver's state */
        func_ret = r_imrdrv_state_update_state(&p_work_area->state_manage_area, IMRDRV_PROC_EXECUTE);
        
        /* Check the result of updating the IMR Driver's state */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Execute DL */
            func_ret = r_imrdrv_mainctl_start_render(&p_work_area->basic_device_area,
                                                     &p_work_area->osal_io_area);
            
            /* Check the result of executing DL */
            if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
        else
        {
            /* Check the return value is not ERROR_STATE */
            if (IMRDRV_ERROR_STATE != func_ret)
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
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
* End of function r_imrdrv_api_start_render()
***********************************************************************************************************************/

