/*************************************************************************************************************
* RIVP Sample Application main
* Copyright (C) 2021 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


#include "rcar-xos/osal/r_osal.h"
#include "rivp_ldd_sample.h"

#if !defined(RIVP_SUPPORTED)
#error "rivp_sample requires the RIVP_SUPPORTED option. Please execute 'cmake -DRIVP_SUPPORTED=ON <build-dir>' to enable it."
#endif

#define RIVP_THREAD_ID              0x8000U                   /* RESOURCE_ID_BSP                 */
#define LOC_THREAD_ID_START         (RIVP_THREAD_ID + 8U)

void loc_RivpLddSampleThread(void * Arg) {
    e_osal_return_t osal_ret;

    /* Init OSAL */
    osal_ret  = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    rivp_ldd_main();
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(loc_RivpLddSampleThread);
    return(-1);
}
