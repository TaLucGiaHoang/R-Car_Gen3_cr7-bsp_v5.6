/***********************************************************************************************************************
* Copyright [2020-2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
* Description :  OSAL wrapper version function for Trampoline (AUTOSAR)
***********************************************************************************************************************/

/**
 * @file r_osal_version.c
 */

#include "target/trampoline/r_osal_common.h"

#define OSAL_WRAPPER_VERSION_MAJOR   (0)
#define OSAL_WRAPPER_VERSION_MINOR   (1)
#define OSAL_WRAPPER_VERSION_PATCH   (0)

/***********************************************************************************************************************
* Start of function R_OSAL_GetVersion()
***********************************************************************************************************************/
const st_osal_version_t* R_OSAL_GetVersion(void)
{
    static const st_osal_version_t s_version =
    {
        {
            OSAL_VERSION_MAJOR,
            OSAL_VERSION_MINOR,
            OSAL_VERSION_PATCH
        },
        {
            OSAL_WRAPPER_VERSION_MAJOR,
            OSAL_WRAPPER_VERSION_MINOR,
            OSAL_WRAPPER_VERSION_PATCH
        }
    };

    return &s_version;
}
/***********************************************************************************************************************
* End of function R_OSAL_GetVersion()
***********************************************************************************************************************/
