/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
#ifndef HELLO_RGL_INTERN_H
#define HELLO_RGL_INTERN_H

#include "r_drw2d_types.h"
#include "r_config_drw2d.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"

#ifdef DRW2D_ON_DHD
#include "r_drw2d_ctx_dhd.h"
#elif DRW2D_ON_CPU
#include "r_drw2d_ctx_cpu.h"
#endif

#ifndef MAX_NR_DISPLAYS
/* Set default display number. */
#ifdef R_TARGET_BOARD_EBISU
#define MAX_NR_DISPLAYS         1
#else // R_TARGET_BOARD_SALVATORXS
#define MAX_NR_DISPLAYS         3
#endif
#endif /* #ifndef MAX_NR_DISPLAYS */

#define MAX_NR_LAYER            2
#define MAX_NR_FBUF             3

#define MAX_THREAD              6

#if MAX_THREAD < (MAX_NR_DISPLAYS * MAX_NR_LAYER)
#error "Invalid value: MAX_THREAD"
#endif

#define ALLOC_ALIGN             256

struct display_struct;

typedef struct {
    struct display_struct *Parent;
    uintptr_t FBuf[MAX_NR_FBUF];  /* multi buffered */
    uint32_t bpp;
    r_wm_Surface_t VoutSurface;
    osal_thread_handle_t DrawThread;
    osal_mq_handle_t    VSyncMq;
    uint32_t VsyncMisses;
    r_drw2d_Device_t Drw2DDev;
#ifdef DRW2D_ON_DHD
    struct r_drw2d_DeviceDHD_s Drw2DGpuDev;
#elif DRW2D_ON_CPU
    struct r_drw2d_DeviceCPU_s Drw2DGpuDev;
#else
  #error "DRW2D Platform is not set correctly"
#endif
    r_drw2d_Framebuffer_t Drw2DFb;

    osal_memory_buffer_handle_t VidMemHandle[MAX_NR_FBUF];
}layer_t;

typedef struct display_struct {
    int Valid;
    r_wm_Dev_t Disp;
    uint32_t ScreenWidth;
    uint32_t ScreenHeight;
    /* this app is using two layers per display */
    layer_t Layer[MAX_NR_LAYER];
}display_t;

#endif /*HELLO_RGL_INTERN_H */
