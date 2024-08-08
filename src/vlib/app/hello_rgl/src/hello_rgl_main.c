/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#include "hello_rgl.h"
#include "hello_rgl_intern.h"
#include "vout.h"
#include "gpu.h"
#include "draw.h"

#include "r_print_api.h"
/*
   Update following definetion to enable R_PRINT_Log() in /vlib/drivers/print/src/r_print_api.c
   #define R_USE_LOG 0
*/

/* Maximum number of displays supported */
/*  MAX_NR_DISPLAYS is defined in "hello_rgl_intern.h" */

static display_t display[MAX_NR_DISPLAYS] = {
#if MAX_NR_DISPLAYS == 1
    {
        .Valid = 0,
#ifdef R_TARGET_BOARD_EBISU
        .Disp = R_WM_DEV_VOUT0,
#else // R_TARGET_BOARD_SALVATORXS
        .Disp = R_WM_DEV_VOUT1,
#endif
    },
#elif MAX_NR_DISPLAYS == 2
    {
        .Valid = 0,
        .Disp = R_WM_DEV_VOUT1,
    },
    {
        .Valid = 0,
        .Disp = R_WM_DEV_VOUT2,
    },
#elif MAX_NR_DISPLAYS == 3
    {
        .Valid = 0,
        .Disp = R_WM_DEV_VOUT0,
    },
    {
        .Valid = 0,
        .Disp = R_WM_DEV_VOUT1,
    },
    {
        .Valid = 0,
        .Disp = R_WM_DEV_VOUT2,
    },
#endif
};

void *main_thread(void *Arg)
{
    int x, i;
    (void)Arg;

#ifdef DRW2D_ON_DHD
    R_PRINT_Log("GPU init: ");
    x = GpuInit();
    if (x != 0) {
        R_PRINT_Log("failed\n");
        goto end;
    }
    R_PRINT_Log("OK\n");
#endif

    for (i = 0; i < MAX_NR_DISPLAYS; i++) {
        R_PRINT_Log("VOUT %d init: ", i);
        x = VoutInit(&display[i]);
        if (x != 0) {
            R_PRINT_Log("failed\n");
            continue;
        }
        R_PRINT_Log("OK\n");

        display[i].Valid = 1;

        R_PRINT_Log("DRW2D init: ");
        x = DrawInit(&display[i]);
        if (x != 0) {
            R_PRINT_Log("failed\n");
        } else {
            R_PRINT_Log("OK\n");
        }
    }

    DrawLoop();

    for (i = 0; i < MAX_NR_DISPLAYS; i++) {
        if (display[i].Valid == 0) {
            continue;
        }
        x = DrawDeinit(&display[i]);
        if (x != 0) {
            R_PRINT_Log("DRW2D deinit failed\n");
        }

        x = VoutDeinit(&display[i]);
        if (x != 0) {
            R_PRINT_Log("VOUT deinit failed\n");
        }
    }

#ifdef DRW2D_ON_DHD
    x = GpuDeinit();
    if (x != 0) {
        R_PRINT_Log("GPU deinit failed\n");
    }
#endif

end:
    R_PRINT_Log("===END Hello RGL===\n");
    return (void *)x;
}

int hello_main(void)
{
    e_osal_return_t      osal_ret     = OSAL_RETURN_OK;
    st_osal_thread_config_t thread_config;
    osal_thread_handle_t thread;

#ifdef DRW2D_ON_DHD
    R_PRINT_Log("===START Hello RGL on DHD===\n");
#elif DRW2D_ON_CPU
    R_PRINT_Log("===START Hello RGL on CPU===\n");
#endif

    R_PRINT_Log("Compilation %s %s\n",__DATE__,__TIME__);

    thread_config.priority   = LOC_MAINTHREAD_PRIO;
    thread_config.task_name  = "main_thread";
    thread_config.func       = (p_osal_thread_func_t)main_thread;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = LOC_MAINTHREAD_STACKSIZE;

    osal_ret = R_OSAL_ThreadCreate(&thread_config,
                                   (osal_thread_id_t)LOC_OSAL_THREAD_DRW2D_MAINTHREAD,
                                   &thread );
    R_PRINT_Log("hello_main: Create Thread[%s], ret = %d \n", thread_config.task_name, osal_ret);

    return 0;
}
