/***********************************************************************************************************************
* Copyright [2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Description :  OSAL wrapper TimeClock Manager for Trampoline (AUTOSAR)
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * includes
***********************************************************************************************************************/
#include "target/trampoline/r_osal_common.h"

/***********************************************************************************************************************
* Start of function R_OSAL_ClockTimeGetTimeStamp()
* XOS1_OSAL_CD_CD_400
* [Covers: XOS1_OSAL_UD_UD_184]
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_312]
* [Covers: XOS1_OSAL_UD_UD_314]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_ClockTimeGetTimeStamp(e_osal_clock_type_t clock_type, st_osal_time_t *const p_time_stamp)
{
    e_osal_return_t osal_ret;

    if ((OSAL_CLOCK_TYPE_HIGH_PRECISION != clock_type) && (OSAL_CLOCK_TYPE_HIGH_RESOLUTION != clock_type))
    {
        OSAL_DEBUG("OSAL_RETURN_PAR\n");
        osal_ret = OSAL_RETURN_PAR;
    }
    else
    {
        osal_ret = R_OSAL_Internal_GetTimestamp(p_time_stamp);
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_ClockTimeGetTimeStamp()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function R_OSAL_ClockTimeCalculateTimeDifference()
* XOS1_OSAL_CD_CD_402
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_253]
* [Covers: XOS1_OSAL_UD_UD_254]
* [Covers: XOS1_OSAL_UD_UD_255]
* [Covers: XOS1_OSAL_UD_UD_001]
***********************************************************************************************************************/
e_osal_return_t R_OSAL_ClockTimeCalculateTimeDifference(const st_osal_time_t *const p_time2,
                                                        const st_osal_time_t *const p_time1,
                                                        osal_nano_sec_t *const p_time_difference)
{
    e_osal_return_t osal_ret;

    if ((NULL == p_time2) || (NULL == p_time1) || (NULL == p_time_difference))
    {
        OSAL_DEBUG("OSAL_RETURN_PAR\n");
        osal_ret = OSAL_RETURN_PAR;
    }
    else
    {
        osal_ret = R_OSAL_Internal_CalcDiffTime(p_time2, p_time1, p_time_difference);
    }

    return osal_ret;
}
/***********************************************************************************************************************
* End of function R_OSAL_ClockTimeCalculateTimeDifference()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function osal_clock_time_initialize()
* XOS1_OSAL_CD_CD_405
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_324]
***********************************************************************************************************************/
e_osal_return_t osal_clock_time_initialize(void)
{

    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    return osal_ret;
}
/***********************************************************************************************************************
* End of function osal_clock_time_initialize()
***********************************************************************************************************************/

/***********************************************************************************************************************
* Start of function osal_clock_time_deinitialize()
* XOS1_OSAL_CD_CD_406
* [Covers: XOS1_OSAL_UD_UD_292]
* [Covers: XOS1_OSAL_UD_UD_334]
***********************************************************************************************************************/
e_osal_return_t osal_clock_time_deinitialize(void)
{

    /*------------------------------------------------------------------------------------------------------------------
    Local variables
    ------------------------------------------------------------------------------------------------------------------*/
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /*------------------------------------------------------------------------------------------------------------------
    Function body
    ------------------------------------------------------------------------------------------------------------------*/
    return osal_ret;
}
/***********************************************************************************************************************
* End of function osal_clock_time_deinitialize()
***********************************************************************************************************************/

