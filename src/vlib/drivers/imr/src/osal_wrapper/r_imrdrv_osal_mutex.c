/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_osal_mutex.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of OSAL Wrapper Mutex
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 02.04.2021 0.01     New create
 *         : 20.04.2021 0.02     Add r_imrdrv_osal_mutex_lock() and r_imrdrv_osal_mutex_unlock()
 *                               Delete tag CD_RD_XX.
 *         : 14.06.2021 0.03     Add r_imrdrv_osal_mutex_destroy().
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_imrdrv_private.h"
#include "r_imrdrv_osal_mutex.h"

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
* Function Name: r_imrdrv_osal_mutex_create
* Description  : Create the mutex handle.
* Arguments    : mutex_id          - Mutex ID
*              : mutex_wait_period - The wait time (msec) of locking mutex
*              : p_osal_mutex_area - Address of OSAL Wrapper Mutex area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_02_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_mutex_create (
    const osal_mutex_id_t mutex_id,
    const osal_milli_sec_t mutex_wait_period,
    st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
)
{
    /* Initialize internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_mutex_area) && (mutex_wait_period >= IMRDRV_MUTEX_WAIT_TIME_MIN))
    {
        e_osal_return_t osal_ret;

        /* Create the mutex */
        osal_ret = R_OSAL_ThsyncMutexCreate(mutex_id, &p_osal_mutex_area->mutex_handle);

        if ((OSAL_RETURN_OK == osal_ret) && (OSAL_MUTEX_HANDLE_INVALID != p_osal_mutex_area->mutex_handle))
        {
            /* Set data to OSAL Wrapper Mutex area */
            p_osal_mutex_area->mutex_wait_period = mutex_wait_period;
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
 End of function r_imrdrv_osal_mutex_create
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_mutex_destroy
* Description  : Destroy the mutex handle.
* Arguments    : p_osal_mutex_area - Address of OSAL Wrapper Mutex area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_02_002]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_mutex_destroy (
    st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
)
{
    /* Initialize internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_mutex_area) && (OSAL_MUTEX_HANDLE_INVALID != p_osal_mutex_area->mutex_handle))
    {
        e_osal_return_t osal_ret;

        /* Destroy the mutex */
        osal_ret = R_OSAL_ThsyncMutexDestroy(p_osal_mutex_area->mutex_handle);

        /* Check the result of destroying the mutex */
        if (OSAL_RETURN_OK == osal_ret)
        {
            /* Clear the OSAL Wrapper Mutex area */
            p_osal_mutex_area->mutex_wait_period = IMRDRV_MUTEX_WAIT_TIME_MIN;
            p_osal_mutex_area->mutex_handle = OSAL_MUTEX_HANDLE_INVALID;
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
 End of function r_imrdrv_osal_mutex_destroy
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_mutex_lock
* Description  : Lock the mutex.
* Arguments    : p_osal_mutex_area - Address of OSAL Wrapper Mutex area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_TIME
* [Covers: UD_RD_UD01_05_02_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_mutex_lock (
    const st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
)
{
    /* Initialize internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_mutex_area) &&
        (p_osal_mutex_area->mutex_wait_period >= IMRDRV_MUTEX_WAIT_TIME_MIN) && 
        (OSAL_MUTEX_HANDLE_INVALID != p_osal_mutex_area->mutex_handle))
    {
        e_osal_return_t osal_ret;
        bool osal_context;

        /* Check context is ISR */
        osal_ret = R_OSAL_InterruptIsISRContext(&osal_context);

        /* Check OSAL API returns OK */
        if (OSAL_RETURN_OK == osal_ret)
        {
            osal_milli_sec_t time_period;

            /* Set timeout value according to the context type */
            if (true == osal_context)
            {
                time_period = IMRDRV_MUTEX_WAIT_TIME_MIN;
            }
            else
            {
                time_period = p_osal_mutex_area->mutex_wait_period;
            }

            /* Lock the mutex */
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(p_osal_mutex_area->mutex_handle, time_period);

            /* Check OSAL API returns timeout error */
            if (OSAL_RETURN_TIME != osal_ret)
            {
                /* Set unexpected error if OSAL API does not return OK */
                if (OSAL_RETURN_OK != osal_ret)
                {
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }
            else
            {
                /* Set timeout error to return value */
                func_ret = IMRDRV_ERROR_TIME;
            }
        } /* end of if (OSAL_RETURN_OK == osal_ret) */
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    } /* end of if ((NULL != p_osal_mutex_area) && */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/**********************************************************************************************************************
 End of function r_imrdrv_osal_mutex_lock
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_osal_mutex_unlock
* Description  : Unlock the mutex.
* Arguments    : p_osal_mutex_area - Address of OSAL Wrapper Mutex area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_05_02_004]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_osal_mutex_unlock (
    const st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
)
{
    /* Initialize internal variable */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameters */
    if ((NULL != p_osal_mutex_area) && (OSAL_MUTEX_HANDLE_INVALID != p_osal_mutex_area->mutex_handle))
    {
        e_osal_return_t osal_ret;

        /* Unlock the mutex */
        osal_ret = R_OSAL_ThsyncMutexUnlock(p_osal_mutex_area->mutex_handle);

        /* Set unexpected error to return value if OSAL API does not return OK */
        if (OSAL_RETURN_OK != osal_ret)
        {
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
 End of function r_imrdrv_osal_mutex_unlock
 *********************************************************************************************************************/

