/***********************************************************************************************************************
* IMR Driver sample program
* Copyright (c) [2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "imr_sample.h"

/* main task configuration */
#define IMRSMP_MAIN_STACKSIZE      (0x2000U)
#define IMRSMP_MAIN_PRIO           (OSAL_THREAD_PRIORITY_TYPE7)
#define IMRSMP_MAIN_NAME           "IMR_WRAPPER_MAIN"
#define IMRSMP_MAIN_THREAD_ID      (0x8004U)



/* main task function */
int64_t loc_ImrSampleThread(void * Arg)
{
    int64_t         ret = -1;
    e_osal_return_t osal_ret;
    st_osal_time_t  tmp_time = {0, 0};

    osal_ret  = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    osal_ret = R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &tmp_time);
    if (osal_ret != OSAL_RETURN_OK) {
        printf("[imr_wrapper] R_OSAL_ClockTimeGetTimeStamp failed with error %d\n", osal_ret);
    } else {
        tmp_time.tv_sec += 3u; /* 3 sec sleep */
        // osal_ret = R_OSAL_ThreadSleepUntilTimeStamp(&tmp_time);
        osal_ret = R_OSAL_ThreadSleepForTimePeriod(3000U);
        if (osal_ret != OSAL_RETURN_OK) {
            printf("[imr_wrapper] R_OSAL_ThreadSleepUntilTimeStamp failed with error %d\n", osal_ret);
        } else {
            /* call IMR sample application */
            ret = imr_main();
        }
    }

    return ret;
}

int main(void)
{
    /* Start OS and initial thread */
    /* Note: parameter unused by AutoSAR, see TASK(maintask) */
    R_OSAL_StartOS(loc_ImrSampleThread);
    return(-1);
}
