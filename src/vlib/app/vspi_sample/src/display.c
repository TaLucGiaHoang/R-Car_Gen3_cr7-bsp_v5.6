/*************************************************************************************************************
* logo_display_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <string.h>
#include "rcar-xos/osal/r_osal.h"
#include "r_wm_api.h"
#include "r_wm_config.h"
#include "r_vsp2_api.h"
#include "r_vsp2_vspi_api.h"

#include "config.h"
#include "display.h"

#include "r_print_api.h"

#define DISCOM_ERROR_SHOW 300

static uint32_t DiscomErrorCrc;
static bool DiscomErrorPrint = false;

uint8_t y_rgb_buffer[1920*1080*4] __attribute__ ((aligned (256)));

static const r_wm_DispTiming_t VoutTiming[3] = {
    {//HDMI, 1080p
     .ScreenWidth = 1920,
     .ScreenHeight = 1080,
     .PixelClock = 148500000,
     .VoutFlags = (uint32_t)(R_WM_VOUT_FLAG_HSYNC_ACTHI | R_WM_VOUT_FLAG_VSYNC_ACTHI | R_WM_VOUT_FLAG_DESYNC_ACTHI | R_WM_VOUT_FLAG_DVI_MODE),
     .VoutH = {2200, 44, 148},
     .VoutV = {1125, 5, 36},
     .VoutType = R_WM_VOUT_HDMI,
    },
    {// ARGB, XGA
     .ScreenWidth = 1024,
     .ScreenHeight = 768,
     .PixelClock = 65000000,
     .VoutFlags = (uint32_t)(R_WM_VOUT_FLAG_DESYNC_ACTHI),
     .VoutH = {1344, 144, 168},
     .VoutV = {806, 6, 29},
     .VoutType = R_WM_VOUT_VGA,
    },
    {//LVDS, 1080p
     .ScreenWidth = 1920,
     .ScreenHeight = 1080,
     .PixelClock = 148500000,
     .VoutFlags = (uint32_t)(R_WM_VOUT_FLAG_HSYNC_ACTHI | R_WM_VOUT_FLAG_VSYNC_ACTHI | R_WM_VOUT_FLAG_DESYNC_ACTHI | R_WM_VOUT_FLAG_DVI_MODE),
     .VoutH = {2200, 44, 148},
     .VoutV = {1125, 5, 36},
     .VoutType = R_WM_VOUT_LVDS,
    },
};

static r_wm_Surface_t VoutSurface[] = {
    {
     .PosX = 0,
     .PosY = 720,
     .PosZ = 3, /* layer idx */
     .Width = LOGO_DISP_SURFACE_WIDTH,
     .Height = LOGO_DISP_SURFACE_HEIGHT,
     .StrideY = 0x1E00, // width * bytes per pixel (1920 * 4 = 7680 = 0x1E00)
     .StrideC = 0,
     .Fmt = R_WM_FMT_ARGB8888,
     .Handle = 0, /* will be filled by the API */
     .FBuf = 0,
     .FBufC0 = 0,
     .FBufC1 = 0},
    {
     .PosX = 0,
     .PosY = 768 - LOGO_DISP_SURFACE_HEIGHT - 1,
     .PosZ = 3, /* layer idx */
     .Width = LOGO_DISP_SURFACE_WIDTH,
     .Height = LOGO_DISP_SURFACE_HEIGHT,
     .StrideY = 0x1000, // width * bytes per pixel (1024 * 4 = 4096 = 0x1000)
     .StrideC = 0,
     .Fmt = R_WM_FMT_ARGB8888,
     .Handle = 0, /* will be filled by the API */
     .FBuf = 0,
     .FBufC0 = 0,
     .FBufC1 = 0},
    {
     .PosX = 0,
     .PosY = 0,
     .PosZ = 0, /* layer idx */
     .Width = LOGO_DISP_SURFACE_WIDTH,
     .Height = LOGO_DISP_SURFACE_HEIGHT,
     .StrideY = 0x1400, // width * bytes per pixel (1280 * 4 = 5120 = 0x1400)
     .StrideC = 0,
     .Fmt = R_WM_FMT_ARGB8888,
     .Handle = 0, /* will be filled by the API */
     .FBuf = 0,
     .FBufC0 = 0,
     .FBufC1 = 0},
};

static int locDispInit(r_wm_Dev_t Disp, const r_wm_DispTiming_t* pDispTiming)
{
    int x;
    r_wm_Error_t wmErr;
    r_wm_ColorFmt_t fmt = R_WM_FMT_ARGB8888;
    uint32_t color = 0xFF000000; /* black */

    /* Initialise the WM Device */
    wmErr = R_WM_DevInit(Disp);
    /* Will set-up VOut, i.e.: init DU and VSPD */
    wmErr = R_WM_ScreenPropertySet(Disp, R_WM_SCREEN_PROP_FMT, (void *)&fmt);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        R_PRINT_Log("WM_ScreenPropertySet, FMT Set failed Err= %d\r\n", wmErr);
    }
    wmErr = R_WM_ScreenPropertySet(Disp, R_WM_SCREEN_PROP_BKGCOLOR, (void *)&color);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        R_PRINT_Log("WM_ScreenPropertySet, BKGCOLOR Set failed Err= %d\r\n", wmErr);
    }

    if (R_WM_ERR_SUCCESS == wmErr)
    {
        /* enable Screen */
        wmErr = R_WM_ScreenEnable(Disp, 1);
        if (R_WM_ERR_SUCCESS != wmErr)
        {
            R_PRINT_Log("R_WM_ScreenEnable failed Err= %d\r\n", wmErr);
        }
    }

    if (wmErr == R_WM_ERR_SUCCESS)
    {
        x = 0;
    }
    else
    {
        x = -1;
    }
    return x;
}

static void DiscomCallback(void *pCbArg, uint32_t crc)
{
    static int cnt = 0;

    cnt++;

    if( cnt == DISCOM_ERROR_SHOW ){
        DiscomErrorCrc = crc;
        DiscomErrorPrint = true;
        cnt = 0;
    }
}

static void CreateVspiSurfaceFromWmSurface(r_vspi_Surface_t * vSurf, r_wm_Surface_t * wSurf)
{
    vSurf->Width = wSurf->Width;
    vSurf->Height = wSurf->Height;
    vSurf->FBuf = wSurf->FBuf;
    vSurf->FBufC0 = wSurf->FBufC0;
    vSurf->FBufC1 = wSurf->FBufC1;
    vSurf->Fmt = wSurf->Fmt;
    vSurf->StrideY = wSurf->StrideY;
    vSurf->StrideC = wSurf->StrideC;
}

static int locInitWmSurface(r_wm_Dev_t Disp, r_wm_Surface_t *Surf, uintptr_t Fb)
{
    r_wm_Error_t wmErr;
    int x;
    r_wm_Discom_t Discom;
    r_vspi_Surface_t src_surf;
    r_vspi_Surface_t dst_surf;
    r_vsp2_Unit_t vspi_unit = R_VSP2_VSPI0;
    r_vsp2_Error_t vsp_err;
    r_wm_ColorFmt_t convert_fmt = R_WM_FMT_YCBCR422ITL;

    memset(&Discom, 0, sizeof(r_wm_Discom_t));

    wmErr = R_WM_SurfaceGet(Disp, Surf);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        R_PRINT_Log("R_WM_SurfaceGet failed Err= %d\n", wmErr);
        goto end;
    }
    else
    {
        R_PRINT_Log("R_WM_SurfaceGet. Handle = 0x%p\n", Surf->Handle);
    }

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_POS, Surf);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        wmErr = R_WM_SurfaceRelease(Disp, Surf);
        R_PRINT_Log("R_WM_SurfacePropertySet: set position failed Err= %d\n", wmErr);
        goto end;
    }

    Surf->FBuf = Fb;

    /******************** For DISCOM TEST ********************/
    Discom.Callback = DiscomCallback;
    Discom.CbArg = NULL;
#if 0
    Discom.Crc = 0x2a440d5f;  /* right CRC value */
#else
    /* happens the DISCOM error */
    Discom.Crc = 0x12345678;  /* wrong CRC value */
#endif
    Discom.Enable = true;
    Discom.PosX = 0;
    Discom.PosY = 0;
    Discom.Width = 1920;
    Discom.Height = 1080;
    Surf->Discom = &Discom;

    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_DISCOM, Surf);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        wmErr = R_WM_SurfaceRelease(Disp, Surf);
        printf("R_WM_SurfacePropertySet: set Buffer failed Err= %d\n", wmErr);
        goto end;
    }
    /******************** For DISCOM TEST ********************/

    /************* Start RGB->YUV Conversion Test ************/

    vsp_err = R_VSP2_VSPI_Init(vspi_unit);
    if (R_VSP2_ERR_SUCCESS != vsp_err) {
        R_PRINT_Log("[VSPI Sample] Failed to initialize VSPI!\r\n");
    }

    /* Create initial surfaces for VSPI */
    CreateVspiSurfaceFromWmSurface(&src_surf, Surf);
    CreateVspiSurfaceFromWmSurface(&dst_surf, Surf);

    /* Set Destination buffer */
    dst_surf.FBuf = (uintptr_t)y_rgb_buffer; // Set output Y Plane/YUV/RGB Buffer

    /* Convert ARGB8888 to YUV422 */
    dst_surf.Fmt = convert_fmt;
    vsp_err = R_VSP2_VSPI_ConvertSurface(vspi_unit, &src_surf, &dst_surf);
    if (R_VSP2_ERR_SUCCESS != vsp_err) {
        R_PRINT_Log("[VSPI Sample] Failed to convert ARGB8888 to YUV422!\r\n");
    }

    Surf->Fmt = dst_surf.Fmt;
    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_COLORFMT, Surf);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        wmErr = R_WM_SurfaceRelease(Disp, Surf);
        R_PRINT_Log("R_WM_SurfacePropertySet: set ColorFmt failed Err= %d\n", wmErr);
        goto end;
    }

    /* Set Display Surface to use the YUV422 converted from ARGB8888 */
    Surf->Width = dst_surf.Width;
    Surf->Height = dst_surf.Height;
    Surf->StrideY = dst_surf.StrideY;
    Surf->StrideC = dst_surf.StrideC;
    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_SIZE, Surf);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        wmErr = R_WM_SurfaceRelease(Disp, Surf);
        R_PRINT_Log("R_WM_SurfacePropertySet: set Size failed Err= %d\n", wmErr);
        goto end;
    }

    Surf->FBuf = dst_surf.FBuf;
    wmErr = R_WM_SurfacePropertySet(Disp, R_WM_SURF_PROP_BUFFER, Surf);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        wmErr = R_WM_SurfaceRelease(Disp, Surf);
        R_PRINT_Log("R_WM_SurfacePropertySet: set Buffer failed Err= %d\n", wmErr);
        goto end;
    }
    else
    {
        R_PRINT_Log("Framebuffer = 0x%x\n", Surf->FBuf);
    }

    /************** End RGB->YUV Conversion Test *************/

    wmErr = R_WM_ScreenSurfacesFlush(Disp, 1);
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        R_PRINT_Log("APP: Failed to Flush \n");
    }

end:
    if (R_WM_ERR_SUCCESS != wmErr)
    {
        x = -1;
    }
    else
    {
        x = 0;
    }
    return x;
}

int Disp_Init(uint8_t DisplayUnit, uint8_t TimingCfgIdx)
{
    return locDispInit(DisplayUnit, &VoutTiming[TimingCfgIdx]);;
}

int Disp_Deinit(void)
{
    return 0;
}

int Disp_InitSurface(uint8_t DisplayUnit, uint8_t SurfaceIdx, uintptr_t pFrameBuffer)
{
    return locInitWmSurface(DisplayUnit, &VoutSurface[SurfaceIdx], pFrameBuffer);
}

void Disp_PrintDiscomError(void)
{
    if (DiscomErrorPrint)
    {
        DiscomErrorPrint = false;
        R_PRINT_Log("A DISCOM error occurred %d times\n", DISCOM_ERROR_SHOW);
        R_PRINT_Log("The target's CRC value = %x\n", DiscomErrorCrc);
    }
}
