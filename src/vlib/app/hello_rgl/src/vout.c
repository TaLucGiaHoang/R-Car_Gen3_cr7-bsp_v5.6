/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#include <stdint.h>

#include "hello_rgl.h"
#include "hello_rgl_intern.h"
#include "vout.h"

#include "r_print_api.h"
/*
   Update following definetion to enable R_PRINT_Log() in /vlib/drivers/print/src/r_print_api.c
   #define R_USE_LOG 0
*/

static osal_mutex_handle_t          FbMemMutex = 0;
extern osal_memory_manager_handle_t g_hndl_mmngr;

typedef struct{
    r_wm_ColorFmt_t wm_format;
    r_drw2d_PixelFormat_t drw2d_format;
    uint32_t bpp;
} format_mapping_t;

static const format_mapping_t format_mapping[]={
    {R_WM_FMT_ARGB8888, R_DRW2D_PIXELFORMAT_ARGB8888, 32},
    {R_WM_FMT_ARGB4444, R_DRW2D_PIXELFORMAT_ARGB4444, 16},
    {R_WM_FMT_ARGB1555, R_DRW2D_PIXELFORMAT_ARGB1555, 16},
    {R_WM_FMT_RGB565  , R_DRW2D_PIXELFORMAT_RGB565,   16},
    {R_WM_FMT_XRGB4444, R_DRW2D_PIXELFORMAT_ARGB4444, 16},
    {R_WM_FMT_XRGB1555, R_DRW2D_PIXELFORMAT_ARGB1555, 16},
};

static int locInitWmSurface(layer_t *pLayer)
{
    r_wm_Dev_t Disp = pLayer->Parent->Disp;
    r_wm_Error_t wmErr;
    int x, i;
    size_t size;
    e_osal_return_t osal_ret;
    osal_memory_manager_handle_t memHandle;

    memHandle = VoutGetMemhandle();
    if (0==memHandle) {
        R_PRINT_Log("VoutGetMemhandle failed\n");
        return -1;
    }

    wmErr = R_WM_SurfaceGet(Disp, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfaceGet failed Err= %d\n", wmErr);
        goto end;
    } else {
        R_PRINT_Log("R_WM_SurfaceGet. Handle = 0x%x\n", (unsigned int)pLayer->VoutSurface.Handle);
        if (0 == pLayer->VoutSurface.StrideY) {
            /* Set surface stride */
            pLayer->VoutSurface.StrideY = pLayer->VoutSurface.Width * pLayer->bpp / 8L;
            pLayer->VoutSurface.StrideC = pLayer->VoutSurface.Width * pLayer->bpp / 8L;
        }
    }

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_POS, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfacePropertySet: set position failed Err= %d\n", wmErr);
        wmErr = R_WM_SurfaceRelease(Disp, &pLayer->VoutSurface);
        goto end;
    }

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_SIZE, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfacePropertySet: set Size failed Err= %d\n", wmErr);
        wmErr = R_WM_SurfaceRelease(Disp, &pLayer->VoutSurface);
        goto end;
    }

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_COLORFMT, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfacePropertySet: set Format failed Err= %d\n", wmErr);
        wmErr = R_WM_SurfaceRelease(Disp, &pLayer->VoutSurface);
        goto end;
    }

    do {
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(FbMemMutex,
                                                       (osal_milli_sec_t)LOC_MUTEX_TIMEOUT_MS);
    } while (OSAL_RETURN_TIME == osal_ret) ;

    /* depending on layer count and multi-buffering we're using more than allocated by linker file */
    size = pLayer->VoutSurface.Width * pLayer->VoutSurface.Height * pLayer->bpp / 8L;
    for (i = 0; i < (sizeof(pLayer->FBuf) / sizeof(pLayer->FBuf[0])); i++) {
        osal_ret = R_OSAL_MmngrAlloc( memHandle, size, ALLOC_ALIGN, &pLayer->VidMemHandle[i] );
        if(OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("VoutInit: R_OSAL_MmngrAlloc failer, ret = %d \n", osal_ret);
            goto end;
        }

        osal_ret = R_OSAL_MmngrGetCpuPtr( pLayer->VidMemHandle[i], (void**)&pLayer->FBuf[i] );
        if(OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("VoutInit: R_OSAL_MmngrGetCpuPtr failer, ret = %d \n", osal_ret);
            goto end;
        }

        #if 0
            R_PRINT_Log("pLayer->FBuf[%d]\n", i);
            R_PRINT_Log(" : Memory Buffer Handle[%p] \n", pLayer->VidMemHandle[i]);
        #endif
    }
    R_OSAL_ThsyncMutexUnlock(FbMemMutex);

    pLayer->VoutSurface.FBuf = pLayer->FBuf[0];
    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_BUFFER, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        wmErr = R_WM_SurfaceRelease(Disp, &pLayer->VoutSurface);
        goto end;
    } else {
        R_PRINT_Log("Framebuffer = 0x%x\n", pLayer->VoutSurface.FBuf);
    }

    wmErr = R_WM_ScreenSurfacesFlush(Disp, 1);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("APP: Failed to Flush \n");
    }

end:
    if (R_WM_ERR_SUCCESS != wmErr) {
        x = -1;
    } else {
        x = 0;
    }
    return x;
}

static int locDispInit(display_t *pDisp)
{
    r_wm_Error_t wmErr = R_WM_ERR_SUCCESS;
    uint32_t Fmt = R_WM_FMT_ARGB8888;
    uint32_t BgColor = 0xFF808080; /* grey */
    uint32_t max_nr_layers;
    int x;

    /* Initialise the WM Device. The WM is responsible for resolution and refresh rates */
    /* Since we may be opening devices that are not available on some boards, do not
     * print a warning if this fails */
    wmErr = R_WM_DevInit(pDisp->Disp);

    if (R_WM_ERR_SUCCESS == wmErr) {
        wmErr = R_WM_ScreenPropertySet(pDisp->Disp, R_WM_SCREEN_PROP_FMT, (void *)&Fmt);
        if (R_WM_ERR_SUCCESS != wmErr) {
            R_PRINT_Log("WM_ScreenPropertySet, FMT Set failed Err= %d", wmErr);
        }
    } else {
        R_PRINT_Log("R_WM_DevInit(%d), rfailed Err= %d\n", pDisp->Disp, wmErr );
    }

    if (R_WM_ERR_SUCCESS == wmErr) {
        wmErr = R_WM_ScreenPropertySet(pDisp->Disp, R_WM_SCREEN_PROP_BKGCOLOR, (void *)&BgColor);
        if (R_WM_ERR_SUCCESS != wmErr) {
            R_PRINT_Log("WM_ScreenPropertySet, BKGCOLOR Set failed Err= %d", wmErr);
        }
    }

    if (R_WM_ERR_SUCCESS == wmErr) {
        /* Get the resolution */
        wmErr = R_WM_DevInfoGet(pDisp->Disp, &max_nr_layers, &(pDisp->ScreenWidth), &(pDisp->ScreenHeight));
        if (R_WM_ERR_SUCCESS != wmErr) {
            R_PRINT_Log("R_WM_DevInfoGet FAILED= %d", wmErr);
        }
    }
    R_PRINT_Log("Display[%d]: ScreenSize(%d,%d)\n", pDisp->Disp, pDisp->ScreenWidth, pDisp->ScreenHeight);

    if (R_WM_ERR_SUCCESS == wmErr) {
        /* enable Screen */
        wmErr = R_WM_ScreenEnable(pDisp->Disp, 1);
        if (R_WM_ERR_SUCCESS != wmErr) {
            R_PRINT_Log("R_WM_ScreenEnable failed Err= %d\n", wmErr);
        }
    }

    if (wmErr == R_WM_ERR_SUCCESS) {
        x = 0;
    } else {
        x = -1;
    }
    return x;
}

int VoutSurfaceInit(layer_t *pLayer)
{
    return locInitWmSurface(pLayer);
}

int VoutSetFB(layer_t *pLayer, uint32_t BufIndex)
{
    r_wm_Error_t wmErr;
    r_wm_Dev_t Disp = pLayer->Parent->Disp;

    size_t size;
    e_osal_return_t osal_ret;

    osal_ret = R_OSAL_MmngrGetHwAddr(pLayer->VidMemHandle[BufIndex], OSAL_AXI_BUS_ID_IPA_MAIN_MEMORY, 
                                     (uintptr_t *)&pLayer->VoutSurface.FBuf);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("R_OSAL_MmngrGetHwAddr failed Err= %d\n", osal_ret);
        return -1;
    }

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_BUFFER, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfacePropertySet failed Err= %d\n", wmErr);
        wmErr = R_WM_SurfaceRelease(Disp, &pLayer->VoutSurface);
        return -1;
    }

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_POS, &pLayer->VoutSurface);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("R_WM_SurfacePropertySet: set position failed Err= %d\n", wmErr);
        wmErr = R_WM_SurfaceRelease(Disp, &pLayer->VoutSurface);
        return -1;
    }

#ifdef DRW2D_ON_CPU
    /* Flush CPU cache before R_WM_ScreenSurfacesFlush() */
    size = pLayer->VoutSurface.Width * pLayer->VoutSurface.Height * pLayer->bpp / 8L;
    R_OSAL_MmngrFlush(pLayer->VidMemHandle[BufIndex], 0, size);
#endif

    wmErr = R_WM_ScreenSurfacesFlush(Disp, 0);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("APP: Failed to Flush \n");
        return -1;
    }
    return 0;
}

int VoutInit(display_t *pDisplay)
{
    osal_memory_manager_handle_t memHandle;
    e_osal_return_t osal_ret     = OSAL_RETURN_OK;
    int x, j, format_select;

    memHandle = VoutGetMemhandle();
    if (OSAL_MEMORY_MANAGER_HANDLE_INVALID==memHandle) {
        R_PRINT_Log("VoutGetMemhandle failed\n");
        return -1;
    }

    if (0==FbMemMutex) {
        osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_ID_VOUT,
                                            (osal_mutex_handle_t*)&FbMemMutex);
        if(OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("R_OSAL_ThsyncMutexCreate failed, Err= %d \n", osal_ret);
            return -1;
        }
    }

    x = locDispInit(pDisplay);
    if (0 != x) {
        R_PRINT_Log("locDispInit failed, return= %d\n", x );
        return x;
    }

    /* currently both displays have same number layers configured here */
    for (j = 0; j < (sizeof(pDisplay->Layer) / sizeof(pDisplay->Layer[0])); j++) {
        /* mix up our format selection for some variety */
        format_select = (pDisplay->Disp + j) % (sizeof(format_mapping)/sizeof(format_mapping[0]));
	//format_select = 0; // Select RGB 8888 only for debug purpose
        layer_t *pLayer = &pDisplay->Layer[j];
        pLayer->Parent = pDisplay;
        /* assign layers:PosZ 1:1 */
        pLayer->VoutSurface.PosZ = j;

        pLayer->VoutSurface.Fmt = format_mapping[format_select].wm_format;
        pLayer->Drw2DFb.Buffer.PixelFormat = format_mapping[format_select].drw2d_format;
        pLayer->bpp = format_mapping[format_select].bpp;

        /* make layer size subset of display
           (TODO: do we have any alignment issues?) */
        pLayer->VoutSurface.Height = (pLayer->Parent->ScreenHeight * 1) / 2;
        pLayer->VoutSurface.Width = (pLayer->Parent->ScreenWidth * 1) / 2;
        /* stagger layer placement (as long as PosZ on even/odd) */
        pLayer->VoutSurface.PosX = pLayer->Parent->ScreenWidth - pLayer->VoutSurface.Width;
        pLayer->VoutSurface.PosX *= (pLayer->VoutSurface.PosZ & 0x01);

        pLayer->VoutSurface.PosY = pLayer->Parent->ScreenHeight - pLayer->VoutSurface.Height;
        pLayer->VoutSurface.PosY *= (pLayer->VoutSurface.PosZ & 0x01);

        /* surface MUST be intialized in draw thread otherwise WM complains about future FB updates due to PID */
    }
    return x;
}

int VoutDeinit(display_t *pDisplay)
{
    int i;
    layer_t *pLayer;
    e_osal_return_t osal_ret     = OSAL_RETURN_OK;
    osal_memory_manager_handle_t memHandle = 0;

    R_WM_ScreenEnable(pDisplay->Disp, 0);

    do {
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(FbMemMutex,
                                                       (osal_milli_sec_t)LOC_MUTEX_TIMEOUT_MS);
    } while (OSAL_RETURN_TIME == osal_ret) ;

    memHandle = VoutGetMemhandle();
    if (0==memHandle) {
        for (i = 0; i < (sizeof(pLayer->FBuf) / sizeof(pLayer->FBuf[0])); i++) {
            pLayer = &pDisplay->Layer[i];
            osal_ret = R_OSAL_MmngrDealloc( memHandle, pLayer->VidMemHandle[i] );
            if(OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("VoutDeinit: R_OSAL_MmngrDealloc failer, ret = %d \n", osal_ret);
            }
        }
    }

    R_OSAL_ThsyncMutexUnlock(FbMemMutex);

    R_WM_DevDeinit(pDisplay->Disp);

    return 0;
}

osal_memory_manager_handle_t VoutGetMemhandle(void)
{
    osal_memory_manager_handle_t memHandle = OSAL_MEMORY_MANAGER_HANDLE_INVALID;
    st_osal_mmngr_config_t memConfig;

    e_osal_return_t osal_ret     = OSAL_RETURN_OK;
    if (OSAL_MEMORY_MANAGER_HANDLE_INVALID == g_hndl_mmngr) {
        /* Get Screen surface memory form OSAL MMNGR. */
        osal_ret = R_OSAL_MmngrGetOsalMaxConfig( &memConfig );
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("R_OSAL_MmngrGetOsalMaxConfig failed, Err=%d \n", osal_ret);
            return 0;
        }

        osal_ret = R_OSAL_MmngrOpen( &memConfig, &memHandle );
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("R_OSAL_MmngrOpen failed, Err=%d \n", osal_ret);
            return 0;
        }
        g_hndl_mmngr = memHandle;
    }

    return g_hndl_mmngr;
}
