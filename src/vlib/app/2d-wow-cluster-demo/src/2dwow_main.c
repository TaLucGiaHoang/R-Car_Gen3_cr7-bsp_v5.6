/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
/*******************************************************************************
   Title: Main Module

   This module contains the main and the main loop functions.
 */

#include "rcar-xos/osal/r_osal.h"
#include "r_wm_config.h"
#include "r_wm_api.h"

#include "r_print_api.h"
#include "r_prr_api.h"

#include "r_config_drw2d.h"
#include "r_drw2d_types.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_drw2d_ctx_dhd.h"

#include "2dwow.h"
#include "2dwow_main.h"
#include "demo.h"
#include "app_cluster.h"

extern osal_memory_manager_handle_t    g_hndl_mmngr;

#define ALLOC_SIZE  (1024u * 1024u * 32u)
#define MUTEX_TIMEOUT_MS   (1000u) /* 1000 milisecond */

static const cluster_Config_t * loc_ConfigRef = 0;
static r_wm_Display_t loc_Display;
static r_wm_Layer_t loc_Layer;

static davehd_dev_t dhd_dev_obj;
davehd_dev_t *dhd_dev = &dhd_dev_obj;

static r_drw2d_Framebuffer_t loc_fb_obj;
r_drw2d_Framebuffer_t *fb = &loc_fb_obj;

#define MAX_NR_FBUF    2
#define ALLOC_ALIGN    128
static osal_memory_buffer_handle_t     VidMemHandle[MAX_NR_FBUF] = { NULL, };

static void *loc_VidFramebuffer[MAX_NR_FBUF] = { NULL, };

extern int  demo_init(davehd_dev_t *Dev);
extern int  demo_render(davehd_dev_t *Dev, int first, int state);
extern void demo_shutdown(davehd_dev_t *Dev);

static int32_t loc_WmInit()
{
    int32_t                  ret       = -1;
    const cluster_Config_t * p_cfg     = loc_ConfigRef;
    r_wm_Display_t *         p_display = &loc_Display;
    r_wm_Layer_t *           p_layer   = &loc_Layer;

    if (R_WM_ERR_SUCCESS == R_WM_DisplayConfig(p_cfg->Port, p_display)) {
        if (R_WM_ERR_SUCCESS == R_WM_DisplayInit(p_display)) {
            if (R_WM_ERR_SUCCESS == R_WM_DisplayEnable(p_display, true)) {
                p_layer->ParentPtr             = p_display;
                p_layer->VoutSurface.PosX      = p_cfg->PosX;
                p_layer->VoutSurface.PosY      = p_cfg->PosY;
                p_layer->VoutSurface.PosZ      = p_cfg->PosZ;
                p_layer->VoutSurface.Width     = p_cfg->Width;
                p_layer->VoutSurface.Height    = p_cfg->Height;
                p_layer->VoutSurface.Fmt       = R_WM_FMT_ARGB8888;
                p_layer->VoutSurface.Handle    = 0;
                p_layer->VoutSurface.FBuf      = 0;
                p_layer->VoutSurface.FBufC0    = 0;
                p_layer->VoutSurface.FBufC1    = 0;
                p_layer->VoutSurface.Alpha     = 0;
                p_layer->VoutSurface.StrideY   = p_cfg->Width * 4;
                p_layer->VoutSurface.StrideC   = p_cfg->Width * 4;

                if (R_WM_ERR_SUCCESS == R_WM_LayerInit(p_layer)) {
                    ret = 0;
                }
            }
        }
    }

    return ret;
}

int32_t Drw2DInit(void)
{
    int32_t ret = 0;

    ret |= R_DRW2D_Init();
    ret |= R_DRW2D_Open(0, 0, &dhd_dev->sys, &dhd_dev->drw2d);
    ret |= R_DRW2D_NativeDriverBegin(dhd_dev->drw2d, R_DRW2D_NATIVEDRV_KEEPSTATE);
    ret |= R_DRW2D_NativeDriverHandleGet(dhd_dev->drw2d, &dhd_dev->dhd);
    if (ret) {
        R_PRINT_Log("error init drw2d \r\n");
        return 1;
    }
    return 0;
}

int32_t Drw2DDeinit(void)
{
    R_DRW2D_NativeDriverEnd(dhd_dev->drw2d, R_DRW2D_NATIVEDRV_APPCONTEXT);
    return 0;
}


/*******************************************************************************
*/
int32_t AppInit(void)
{
    int32_t ret = 0;
    e_osal_return_t osal_ret;
    size_t i;
    size_t size;
    r_prr_DevName_t soc;

    R_PRINT_Log("CR7 START\r\n");

    R_PRINT_Log("about WM init\r\n");
    ret = loc_WmInit();
    if (-1 == ret) {
        R_PRINT_Log("WM init err\r\n");
    }
    else {
        if ( 0 != g_hndl_mmngr ) {
            size = loc_Layer.VoutSurface.Width * loc_Layer.VoutSurface.Height * 4;
            for ( i = 0 ; i < MAX_NR_FBUF ; i++ ) {
                osal_ret = R_OSAL_MmngrAlloc( g_hndl_mmngr, size, ALLOC_ALIGN, &VidMemHandle[i] );
                if(OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("Error R_OSAL_MmngrAlloc[%d] failer, ret = %d \n", i, osal_ret);
                    ret = -1;
                }
                else {
                    osal_ret = R_OSAL_MmngrGetHwAddr( VidMemHandle[i], OSAL_AXI_BUS_ID_IPA_MAIN_MEMORY, 
                                   (uintptr_t*)&loc_VidFramebuffer[i] );
                    if(OSAL_RETURN_OK != osal_ret) {
                        R_PRINT_Log("Error R_OSAL_MmngrGetHwAddr failer, ret = %d \n", osal_ret);
                        ret = -1;
                    }
                }
            }
        }
        else {
            R_PRINT_Log("\n\nError/MMNGR!!\n\n");
            ret = -1;
        }
    }
    R_PRINT_Log("WM init done\r\n");

    if (-1 != ret) {
        Drw2DInit();

        demo_init(dhd_dev);

        /* Init framebuffer object */
        fb->Flags = 0;
        fb->Handle = 0;
        fb->Buffer.Pitch = loc_Layer.VoutSurface.Width;
        fb->Buffer.Size.Width = loc_Layer.VoutSurface.Width;
        fb->Buffer.Size.Height = loc_Layer.VoutSurface.Height;
        fb->Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
        fb->Buffer.Data = 0;

        R_PRINT_Log("App Init done\r\n");
    }
    return ret;
}

void AppDeInit(unsigned int ret)
{
    r_wm_Error_t wmErr;

    /* Start exiting the the application */
    /* release the surfaces */
    wmErr = R_WM_LayerDeInit(&loc_Layer);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfaceRelease failed Err= %d\n", wmErr);
    }

    wmErr = R_WM_DisplayDeInit(&loc_Display);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_DevDeinit failed Err= %d\n", wmErr);
    }
    R_PRINT_Log("loc_MainLoop END, ret = %d \r\n", ret);
    demo_shutdown(dhd_dev);

    Drw2DDeinit();
    return;
}

static int64_t AppLoop(void *Arg)
{
    uint32_t cnt;
    int32_t ret = 0;
    r_wm_Error_t wmErr = 0;

    ret = AppInit();

    for (cnt = 0; (ret == 0 && wmErr == 0); cnt++) {
        int idx = (cnt % 2 == 1);

        fb->Buffer.Data =    loc_VidFramebuffer[idx];
        ret |= R_DRW2D_FramebufferSet(dhd_dev->drw2d, fb);
        demo_set_fb(fb);
        demo_render(dhd_dev, (cnt == 0), 0);
        ret |= R_DRW2D_GpuFinish(dhd_dev->drw2d, R_DRW2D_FINISH_WAIT); /* why do the finish ?? */

        wmErr = R_WM_LayerFlush(&loc_Layer, (void *)loc_VidFramebuffer[idx], 1);
    }

    AppDeInit(ret);

    return 0;
}

static int64_t main_thread(void *Arg)
{
    e_osal_return_t osal_ret;
    int64_t return_value;
    st_osal_thread_config_t thread_config;
    osal_thread_handle_t thread;

    r_dhd_ServerInfo_t            dhdInitInfo;

    R_PRINT_Log("===START 2d cluster wow on DHD===\n");

    /* Start DHD Server. */
    dhdInitInfo.VideoMemoryHandle = (dhd_handle_t)g_hndl_mmngr;
    dhdInitInfo.VideoMemorySize   = ALLOC_SIZE;
    dhdInitInfo.TimeOut           = MUTEX_TIMEOUT_MS;

    dhd_server_main(&dhdInitInfo);

    /* Threads init */
    osal_ret = R_OSAL_ThreadInitializeThreadConfigSt( &thread_config );
    if( OSAL_RETURN_OK != osal_ret )
    {
        ClusterError(-1);
    }

    thread = OSAL_THREAD_HANDLE_INVALID;
    
    thread_config.priority   = LOC_MAINLOOP_PRIO;
    thread_config.task_name  = "MainLoop";
    thread_config.func       = (p_osal_thread_func_t)AppLoop;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = LOC_COM_THREAD_STACK_SIZE;
    
    osal_ret = R_OSAL_ThreadCreate( &thread_config,
                                    (osal_thread_id_t)LOC_OSAL_THREAD_DRW2D_MAINLOOP,
                                    &thread );
    if( OSAL_RETURN_OK != osal_ret )
    {
        ClusterError(-1);
    }

    /* The code below will wait for DHD and application threads to exit */
    R_OSAL_ThreadJoin( thread, &return_value );

    R_PRINT_Log("end all threads\n\r");
    return 0;
}

int x2dwow_main(const cluster_Config_t * ConfigRef)
{
    st_osal_thread_config_t thread_config;
    osal_thread_handle_t thread;

    loc_ConfigRef = ConfigRef;

    thread_config.priority   = LOC_XMAINLOOP_PRIO;
    thread_config.task_name  = "main_thread";
    thread_config.func       = (p_osal_thread_func_t)main_thread;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = LOC_XMAINLOOP_STACKSIZE;

    R_OSAL_ThreadCreate(&thread_config,
                        (osal_thread_id_t)LOC_OSAL_THREAD_DRW2D_MAINTHREAD,
                        &thread);
    return 0;
}

// DEBUG Code
#if 0

/***********************************************************
  Function: DHD_SYS_SQRTF
  Returns a single-precision floating point number of the square-root result
*/

float64_t DHD_SYS_SQRTF(float64_t val)
{
    float64_t result;
    result = (float64_t) sqrt(val);
    return result;
}

#endif


