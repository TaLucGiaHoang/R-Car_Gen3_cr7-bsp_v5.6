/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_state_manage.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of State Management
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 02.04.2021 0.01     New create
 *         : 22.04.2021 0.02     Move macro from r_imrdrv_state_manage_local.h
 *                               Delete tag CD_RD_XX.
 *         : 14.06.2021 0.03     Add and modify macro definition.
 *                               Add pattern for checking state to r_imrdrv_state_check_state() and
 *                               r_imrdrv_state_chk_valid_state().
 *                               Add pattern for updating state to r_imrdrv_state_update_state().
 *                               Add pattern for checking proccessing type to r_imrdrv_state_get_proc_index().
 *         : 29.10.2021 0.04     Add r_imrdrv_state_get_state().
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_imrdrv_private.h"
#include "r_imrdrv_state_manage.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* State index */
#define IMRDRV_STATE_INDEX_0    (0U)    /* Index 0 of state for checking state. */
#define IMRDRV_STATE_INDEX_1    (1U)    /* Index 1 of state for checking state. */
#define IMRDRV_STATE_INDEX_2    (2U)    /* Index 1 of state for checking state. */
#define IMRDRV_STATE_MAX        (3U)    /* The maximum number of states for checking state. */

/* State table index for processing type */
#define IMRDRV_STATE_TBL_ID_INIT        (0U)    /* State table index for IMRDRV_PROC_INIT. */
#define IMRDRV_STATE_TBL_ID_QUIT        (1U)    /* State table index for IMRDRV_PROC_QUIT. */
#define IMRDRV_STATE_TBL_ID_START       (2U)    /* State table index for IMRDRV_PROC_START. */
#define IMRDRV_STATE_TBL_ID_STOP        (3U)    /* State table index for IMRDRV_PROC_STOP. */
#define IMRDRV_STATE_TBL_ID_EXEC        (4U)    /* State table index for IMRDRV_PROC_EXECUTE. */
#define IMRDRV_STATE_TBL_ID_ATTR_SET     (5U)   /* State table index for IMRDRV_PROC_ATTR_SET. */
#define IMRDRV_STATE_TBL_ID_CB           (6U)   /* State table index for IMRDRV_PROC_CALLBACK. */
#define IMRDRV_STATE_TBL_ID_OPT_ATTR_SET (7U)   /* State table index for IMRDRV_PROC_OPT_ATTR_SET. */
#define IMRDRV_STATE_TBL_ID_PM_POLICY    (8U)  /* State table index for IMRDRV_PROC_PM_POLICY. */
#define IMRDRV_STATE_TBL_ID_OTHER        (9U)  /* State table index for IMRDRV_PROC_OTHER. */
#define IMRDRV_STATE_TBL_ID_INVALID      (10U)  /* State table invalid index. */

/* The number of state table */
#define IMRDRV_PROC_STATE_CHECK_NUM      (10U)  /* The number of states in the table for checking state. */
#define IMRDRV_PROC_STATE_UPDATE_NUM     (7U)   /* The number of states in the table for updating state. */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_state_chk_valid_state (
    const e_imrdrv_state_t  state
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_state_get_proc_index (
    const e_imrdrv_proc_type_t  proc_type,
    uint32_t                    *const p_proc_index
);

/**********************************************************************************************************************
* Function Name: r_imrdrv_state_check_state
* Description  : Check the IMR Driver's state whether it is equal to the expected state.
* Arguments    : p_state_mng_area - Address of the State Management area in the work area
*              : proc_type        - Processing type
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_04_01_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_state_check_state (
    const st_imrdrv_state_manage_area_t *const p_state_mng_area,
    const e_imrdrv_proc_type_t          proc_type
)
{
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if (NULL != p_state_mng_area)
    {
        /* Initialize internal variables */
        e_imrdrv_state_t current_state = p_state_mng_area->state;

        /* Check valid state */
        func_ret = r_imrdrv_state_chk_valid_state(current_state);

        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Initialize internal variables */
            static const e_imrdrv_state_t s_expected_state_table[IMRDRV_PROC_STATE_CHECK_NUM][IMRDRV_STATE_MAX] = 
            {
                {
                    IMRDRV_STATE_UNINIT, IMRDRV_STATE_UNINIT, IMRDRV_STATE_UNINIT
                }, /* state for IMRDRV_PROC_INIT */
                {
                    IMRDRV_STATE_INIT, IMRDRV_STATE_INIT, IMRDRV_STATE_INIT
                }, /* state for IMRDRV_PROC_QUIT */
                {
                    IMRDRV_STATE_INIT, IMRDRV_STATE_INIT, IMRDRV_STATE_INIT
                }, /* state for IMRDRV_PROC_START */
                {
                    IMRDRV_STATE_READY, IMRDRV_STATE_ATTR_SET, IMRDRV_STATE_ATTR_SET
                }, /* state for IMRDRV_PROC_STOP */
                {
                    IMRDRV_STATE_ATTR_SET, IMRDRV_STATE_ATTR_SET, IMRDRV_STATE_ATTR_SET
                }, /* state for IMRDRV_PROC_EXECUTE */
                {
                    IMRDRV_STATE_READY, IMRDRV_STATE_READY, IMRDRV_STATE_READY
                }, /* state for IMRDRV_PROC_ATTR_SET */
                {
                    IMRDRV_STATE_ACTIVE, IMRDRV_STATE_ACTIVE, IMRDRV_STATE_ACTIVE
                }, /* state for IMRDRV_PROC_CALLBACK */
                {
                    IMRDRV_STATE_ATTR_SET, IMRDRV_STATE_ATTR_SET, IMRDRV_STATE_ATTR_SET
                }, /* state for IMRDRV_PROC_OPT_ATTR_SET */
                {
                    IMRDRV_STATE_READY, IMRDRV_STATE_ATTR_SET, IMRDRV_STATE_ACTIVE
                }, /* state for IMRDRV_PROC_PM_POLICY */
                {
                    IMRDRV_STATE_READY, IMRDRV_STATE_READY, IMRDRV_STATE_READY
                }  /* state for IMRDRV_PROC_OTHER */
            };
            uint32_t proc_index = IMRDRV_PROC_STATE_CHECK_NUM;

            /* Get index of processing type */
            func_ret = r_imrdrv_state_get_proc_index(proc_type, &proc_index);

            if ((IMRDRV_ERROR_OK == func_ret) && (proc_index < IMRDRV_PROC_STATE_CHECK_NUM))
            {
                /* Compare state */
                if ((s_expected_state_table[proc_index][IMRDRV_STATE_INDEX_0] != current_state) &&
                    (s_expected_state_table[proc_index][IMRDRV_STATE_INDEX_1] != current_state) &&
                    (s_expected_state_table[proc_index][IMRDRV_STATE_INDEX_2] != current_state))
                {
                    /* Set status error to return value */
                    func_ret = IMRDRV_ERROR_STATE;
                }
            }
            else
            {
                /* Set parameter error to return value */
                func_ret = IMRDRV_ERROR_PAR;
            }
        } /* end of if (IMRDRV_ERROR_OK == func_ret) */
    } /* end of if (NULL != p_state_mng_area) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_state_check_state()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_state_update_state
* Description  : Update the IMR Driver's state.
* Arguments    : p_state_mng_area - Address of the State Management area in the work area
*              : proc_type        - Processing type
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_STATE
* [Covers: UD_RD_UD01_04_01_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_state_update_state (
    st_imrdrv_state_manage_area_t   *const p_state_mng_area,
    const e_imrdrv_proc_type_t       proc_type
)
{
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if (NULL != p_state_mng_area)
    {
        /* Check the IMR Driver's state */
        func_ret = r_imrdrv_state_check_state(p_state_mng_area, proc_type);

        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Initialize internal variables */
            static const e_imrdrv_state_t s_update_state_table[IMRDRV_PROC_STATE_UPDATE_NUM] =
            {
                IMRDRV_STATE_INIT,      /* state for IMRDRV_PROC_INIT */
                IMRDRV_STATE_UNINIT,    /* state for IMRDRV_PROC_QUIT */
                IMRDRV_STATE_READY,     /* state for IMRDRV_PROC_START */
                IMRDRV_STATE_INIT,      /* state for IMRDRV_PROC_STOP */
                IMRDRV_STATE_ACTIVE,    /* state for IMRDRV_PROC_EXECUTE */
                IMRDRV_STATE_ATTR_SET,  /* state for IMRDRV_PROC_ATTR SET */
                IMRDRV_STATE_READY      /* state for IMRDRV_PROC_CALLBACK */
            };
            uint32_t proc_index = IMRDRV_PROC_STATE_UPDATE_NUM;

            /* Get index of processing type */
            func_ret = r_imrdrv_state_get_proc_index(proc_type, &proc_index);

            if ((IMRDRV_ERROR_OK == func_ret) && (proc_index < IMRDRV_PROC_STATE_UPDATE_NUM))
            {
                /* Update the IMR Driver's state */
                p_state_mng_area->state = s_update_state_table[proc_index];
            }
            else
            {
                /* Set parameter error to return value */
                func_ret = IMRDRV_ERROR_PAR;
            }
        } /* end of if (IMRDRV_ERROR_OK == func_ret) */
        else if (IMRDRV_ERROR_STATE == func_ret)
        {
            /** Do nothing because only return state error in this case */
        }
        else
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    } /* end of if (NULL != p_state_mng_area) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_state_update_state()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_state_preset_state
* Description  : Preset the IMR Driver's state to "Uninitialized State".
* Arguments    : p_state_mng_area - Address of the State Management area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_04_01_004]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_state_preset_state (
    st_imrdrv_state_manage_area_t *const p_state_mng_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if (NULL != p_state_mng_area)
    {
        /* Update state */
        p_state_mng_area->state = IMRDRV_STATE_UNINIT;
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
* End of function r_imrdrv_state_preset_state()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_state_get_state
* Description  : Gets the IMR Driver's state.
* Arguments    : p_state_mng_area - Address of the State Management area in the work area
*              : p_state          - The IMR Driver's state
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_04_01_002]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_state_get_state (
    const st_imrdrv_state_manage_area_t *const p_state_mng_area,
    e_imrdrv_state_t                    *const p_state
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if ((NULL == p_state_mng_area) || (NULL == p_state))
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    else
    {
       /* Gets current IMR Driver's state */
        *p_state = p_state_mng_area->state;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_state_get_state()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_state_chk_valid_state
* Description  : Check the IMR Driver's state is valid.
* Arguments    : state - IMR Driver's state
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_04_01_005]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_state_chk_valid_state (
    const e_imrdrv_state_t  state
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    switch (state)
    {
        case IMRDRV_STATE_UNINIT:
            /* Fall through */
        case IMRDRV_STATE_INIT:
            /* Fall through */
        case IMRDRV_STATE_READY:
            /* Fall through */
        case IMRDRV_STATE_ATTR_SET:
            /* Fall through */
        case IMRDRV_STATE_ACTIVE:
            /* Do nothing because only return success in this case */
            break;
        default:
            /* IMR Driver's state is invalid.
             * Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
            break;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_state_chk_valid_state()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_state_get_proc_index
* Description  : Get the index according to the processing type.
* Arguments    : proc_type    - Processing type
*              : p_proc_index - Index of processing type
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_04_01_006]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_state_get_proc_index (
    const e_imrdrv_proc_type_t  proc_type,
    uint32_t                    *const p_proc_index
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if (NULL != p_proc_index)
    {
        /* Initialize internal variables */
        uint32_t result_index = IMRDRV_STATE_TBL_ID_INVALID;

        /* Check processing type and get index */
        switch (proc_type)
        {
            case IMRDRV_PROC_INIT:
                result_index = IMRDRV_STATE_TBL_ID_INIT;
                break;
            case IMRDRV_PROC_QUIT:
                result_index = IMRDRV_STATE_TBL_ID_QUIT;
                break;
            case IMRDRV_PROC_START:
                result_index = IMRDRV_STATE_TBL_ID_START;
                break;
            case IMRDRV_PROC_STOP:
                result_index = IMRDRV_STATE_TBL_ID_STOP;
                break;
            case IMRDRV_PROC_EXECUTE:
                result_index = IMRDRV_STATE_TBL_ID_EXEC;
                break;
            case IMRDRV_PROC_ATTR_SET:
                result_index = IMRDRV_STATE_TBL_ID_ATTR_SET;
                break;
            case IMRDRV_PROC_OPT_ATTR_SET :
                result_index = IMRDRV_STATE_TBL_ID_OPT_ATTR_SET;
                break;
            case IMRDRV_PROC_CALLBACK:
                result_index = IMRDRV_STATE_TBL_ID_CB;
                break;
            case IMRDRV_PROC_PM_POLICY:
                result_index = IMRDRV_STATE_TBL_ID_PM_POLICY;
                break;
            case IMRDRV_PROC_OTHER:
                result_index = IMRDRV_STATE_TBL_ID_OTHER;
                break;
            default:
                /* Processing type is invalid.
                 * Set parameter error to return value */
                func_ret = IMRDRV_ERROR_PAR;
                break;
        } /* end of switch (proc_type) */

        /* Set index */
        *p_proc_index = result_index;
    } /* end of if (NULL == p_proc_index) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_state_get_proc_index()
***********************************************************************************************************************/

