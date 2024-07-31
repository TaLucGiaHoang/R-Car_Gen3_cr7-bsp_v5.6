/*************************************************************************************************************
* vsp2_vspi_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "r_wm_common.h"
#include "r_vsp2_api.h"
#include "r_vsp2_rpf.h"
#include "r_vsp2_wpf.h"
#include "r_vsp2_dpr.h"
#include "r_vsp2_vspi_api.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"
#include "r_prr_api.h"
#include "r_cpg_api.h"
#include "r_sysc_api.h"
#include "r_vsp2_vspi_prv.h"
#include "rcar-xos/osal/r_osal.h"

#define LOC_VSPI_NUM                2
#define RESOURCE_ID_BSP             (0x8000U)
#define OSAL_MUTEX_VSPI_00          (RESOURCE_ID_BSP   + 102U)
#define OSAL_MUTEX_VSPI_01          (RESOURCE_ID_BSP   + 103U)
#define OSAL_COND_VSPI_00           (RESOURCE_ID_BSP  +  2U)
#define OSAL_COND_VSPI_01           (RESOURCE_ID_BSP  +  3U)
#define VSPI_CONVERT_TIMEOUT        (100U)

static osal_mutex_id_t loc_VspiMutexId[LOC_VSPI_NUM] = {
    OSAL_MUTEX_VSPI_00,
    OSAL_MUTEX_VSPI_01
};
static osal_cond_id_t loc_VspiCondId[LOC_VSPI_NUM] = {
    OSAL_COND_VSPI_00,
    OSAL_COND_VSPI_01
};

typedef struct {
    r_vsp2_Unit_t VspUnit;
    osal_device_handle_t device_handle;
    osal_mutex_handle_t mutex_handle;
    osal_cond_handle_t cond_handle;
    volatile bool running;
} r_VspiInfo_t;

typedef struct {
    r_vsp2_Rpf_t rpf_config;
    r_vsp2_Wpf_t wpf_config;
    r_VspiInfo_t vspi_config;
} r_vspi_State;

static r_vspi_State loc_VspiState[LOC_VSPI_NUM];

typedef struct {
    r_wm_ColorFmt_t key;
    uint32_t format;
    uint32_t bpp;
    uint32_t data_swap;
    uint32_t yc_swap;
    uint32_t uv_swap;
    uint32_t bytes_per_pixel[2];    // index 0 = Y/RGB Plane, index 1 = C Plane
} r_vspi_format_lookup_t;

static const r_vspi_format_lookup_t r_vspi_lookup[] = {
  /*| Format              | FMT |bpp|swap|ycswp|uvswp| Bpp       | Trace32 data.image test | rawpixels.net test                                                             |*/
    { R_WM_FMT_ARGB8888,    0x13, 32, 0xC, 0x00, 0x00, {4,0} },  // RGBX888LE            ,  BGRA, no ignore alpha, no alpha first, no little endian, packed, 8888
    { R_WM_FMT_RGB332,      0x00,  8, 0xF, 0x00, 0x00, {1,0} },  // "bbgggrrr"           ,  RGBA, no alpha, ignore alpha, no alpha first, no little endian, packed, 8888
    { R_WM_FMT_ARGB4444,    0x19, 16, 0xF, 0x00, 0x00, {2,0} },  // "bbbbggggrrrrxxxx"   ,  RGBA, no ignore alpha, alpha first, no little endian, packed, 4444
    { R_WM_FMT_XRGB4444,    0x01, 16, 0xF, 0x00, 0x00, {2,0} },  // "bbbbggggrrrrxxxx"   ,  RGBA, ignore alpha, alpha first, no little endian, packed, 4444
    { R_WM_FMT_ARGB1555,    0x1B, 16, 0xF, 0x00, 0x00, {2,0} },  // RGB555               ,  RGBA, no ignore alpha, alpha first, no little endian, packed, 1555
    { R_WM_FMT_XRGB1555,    0x04, 16, 0xF, 0x00, 0x00, {2,0} },  // RGB555               ,  RGBA, ignore alpha, alpha first, no little endian, packed, 1555
    { R_WM_FMT_RGB565,      0x06, 16, 0xF, 0x00, 0x00, {2,0} },  // RGB565               ,  RGBA, ignore alpha, no alpha first, no little endian, packed, 5650
    { R_WM_FMT_BGR888,      0x18, 24, 0xF, 0x00, 0x00, {3,0} },  // RGB888LE             ,  BGRA, ignore alpha, no alpha first, no little endian, packed, 8880
    { R_WM_FMT_RGB888,      0x15, 24, 0xF, 0x00, 0x00, {3,0} },  // RGB888               ,  RGBA, ignore alpha, no alpha first, no little endian, packed, 8880
    { R_WM_FMT_YCBCR420ITL, 0x49,  8, 0xF, 0x00, 0x00, {3,0} },  // Not available        ,  Not available
    { R_WM_FMT_YCBCR422ITL, 0x47, 16, 0xF, 0x00, 0x00, {2,0} },  // YUV422PS             ,  YUV, ignore alpha, no alpha first, little endian, packedYUV, 8888
    { R_WM_FMT_YCBCR420,    0x42,  8, 0xF, 0x00, 0x00, {1,2} },
    { R_WM_FMT_YCBCR422,    0x41,  8, 0xF, 0x00, 0x00, {1,2} },
    { R_WM_FMT_YCBCR444,    0x40,  8, 0xF, 0x00, 0x00, {1,2} },
    { R_WM_FMT_YCBCR420P,   0x4C,  8, 0xF, 0x00, 0x00, {1,2} },
    { R_WM_FMT_YCBCR422P,   0x4B,  8, 0xF, 0x00, 0x00, {1,2} },
    { R_WM_FMT_YCBCR444P,   0x4A,  8, 0xF, 0x00, 0x00, {1,1} },
};

static r_vsp2_Dpr_Conf_t VSPI_Dpr_Conf =
{/* DPR settings for VSPI */
    R_VSP_DPR_ROUTE_WPF0,       /**< Vi6_Dpr_Rpf0_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Rpf1_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Rpf2_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Rpf3_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Rpf4_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Sru_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uds_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Lut_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Clu_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hst_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hsi_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Bru_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Ilv_Brs_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Shp_Route regster */
    R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uif4_Route regster */
    R_VSP_DPR_ROUTE_DISABLE     /**< Vi6_Dpr_Uif5_Route regster */
};

static int loc_Vsp2UnitIndex(r_vsp2_Unit_t Vsp2Unit)
{
    int vspi;

    switch (Vsp2Unit) {
    case R_VSP2_VSPI0:
        vspi = 0;
        break;
    case R_VSP2_VSPI1:
        vspi = 1;
        break;
    default:
        vspi = -1;
        R_PRINT_Log("[%s] : VSP2 Unit is not a valid VSPI.\r\n", __func__);
        break;  /* wrong vspi */
    }

    return vspi;
}

static int loc_FormatLookup(r_wm_ColorFmt_t fmt)
{
    int i;
    for (i=0; i < sizeof(r_vspi_lookup)/sizeof(r_vspi_lookup[0]); i++) {
        if (fmt == r_vspi_lookup[i].key) {
            return i;
        }
    }
    return -1;
}

static int locDeviceOpen(char *device_type, int device_channel, osal_device_handle_t *device_handle)
{
    /* local variable */
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int app_ret = 0;
    size_t numOfDevice = 0;
    size_t numOfByte = 0;
    size_t i = 0;
    size_t j = 0;
    osal_device_handle_t local_handle = OSAL_DEVICE_HANDLE_INVALID;
    char devicelist[200];

    osal_ret = R_OSAL_IoGetNumOfDevices(device_type, &numOfDevice);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("VSPI:GetNumOfDevices=%d\r\n", osal_ret);
        app_ret = -1;
    } else {
        osal_ret = R_OSAL_IoGetDeviceList(device_type, &devicelist[0], sizeof(devicelist), &numOfByte);
        if(OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("VSPI:GetDeviceList=%d\r\n", osal_ret);
            app_ret = -1;
        } else {
            char *deviceList[numOfDevice + 1];
            deviceList[0] = &devicelist[0];
            for (i = 0; i < numOfByte; i++) {
                if (devicelist[i] == '\n') {
                    j++;
                    devicelist[i] = '\0';
                    deviceList[j] = &devicelist[i+1];
                }
            }

            if(j > numOfDevice) {
                app_ret = -1;
                R_PRINT_Log("VSPI:numOfDevice=%d\r\n", app_ret);
            } else {
                osal_ret = R_OSAL_IoDeviceOpen(deviceList[device_channel], &local_handle);
                if(OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("VSPI:R_OSAL_IoDeviceOpen=%d\r\n", osal_ret);
                    app_ret = -1;
                } else {
                    *device_handle = local_handle;
                }
            }
        }
    }

    if(0 != app_ret) {
        *device_handle = OSAL_DEVICE_HANDLE_INVALID;
    }

    return app_ret;
}

static void *locVspiIrq(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg)
{
    r_VspiInfo_t *inst = (r_VspiInfo_t*)irq_arg;
    R_VSP2_Isr(inst->VspUnit, 1, 0);
}

static int locVspiIrqInit(r_vsp2_Unit_t Vsp2Unit)
{
    e_osal_return_t osal_err = OSAL_RETURN_OK;
    int vspi = loc_Vsp2UnitIndex(Vsp2Unit);
    if (vspi < 0) {
        R_PRINT_Log("[%s] Invalid VSPI Unit\r\n", __func__);
        return -1;
    }
    r_vspi_State * vspi_state = &loc_VspiState[vspi];

    if (locDeviceOpen("vspi", vspi, &vspi_state->vspi_config.device_handle) < 0) {
        R_PRINT_Log("[%s] VSPI%d : Failed to open device\r\n", __func__, vspi);
        return -1;
    }

    osal_err = R_OSAL_InterruptRegisterIsr(vspi_state->vspi_config.device_handle,
                                           0,
                                           0,
                                           (p_osal_isr_func_t)locVspiIrq,
                                           (void*)(&vspi_state->vspi_config));
    if (OSAL_RETURN_OK != osal_err) {
        R_PRINT_Log("[%s] VSPI%d : Failed to register VSPI interrupt\r\n", __func__, vspi);
        return -1;
    }
    osal_err = R_OSAL_InterruptEnableIsr(vspi_state->vspi_config.device_handle, 0);
    if (OSAL_RETURN_OK != osal_err) {
        R_PRINT_Log("[%s] VSPI%d : Failed to enable VSPI interrupt\r\n", __func__, vspi);
        return -1;
    }
    return 0;
}

r_vsp2_Error_t R_VSP2_VSPI_Init(r_vsp2_Unit_t Vsp2Unit)
{
    r_vsp2_Error_t vsp2_err;
    uint32_t power_stat = 0;
    int vspi = loc_Vsp2UnitIndex(Vsp2Unit);

    if (vspi == -1) {
        R_PRINT_Log("[%s] Invalid Vsp2Unit\r\n", __func__);
        return R_VSP2_ERR_INVALID_PARAMETER;
    }
    r_vspi_State * vspi_state = &loc_VspiState[vspi];

    /* Enable VSPI Power Domain */
    R_PMA_SYSC_SetPowerStat(R_SYSC_A3VP, true);

    do {
        R_PMA_SYSC_GetPowerStat(R_SYSC_A3VP, true, &power_stat);
    } while (power_stat == 0);

    if (vspi == 0) {
        /* Disable VSPI0 Module Stop Bit */
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT31, false);
        /* Disable FCPVI0 Module Stop Bit */
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT11, false);
    }
    else {
        /* Disable VSPI1 Module Stop Bit */
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT30, false);
        /* Disable FCPVI1 Module Stop Bit */
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT10, false);
    }

    /* Initialize VSPI for conversion */
    vsp2_err = R_VSP2_Init(Vsp2Unit, 0, 0, 0, 0, &VSPI_Dpr_Conf, 0);
    if (R_VSP2_ERR_SUCCESS != vsp2_err) {
        R_PRINT_Log("[%s] R_VSP2_Init failed init for VSPI%d.\r\n", __func__, vspi);
        return vsp2_err;
    }

    /* Initialize driver internal state */
    vspi_state->vspi_config.VspUnit = Vsp2Unit;
    vspi_state->vspi_config.running = false;

    /* Create Mutex and Conditional Variable for Interrupt/Thread sync */
    if (R_OSAL_ThsyncMutexCreate(loc_VspiMutexId[vspi], &vspi_state->vspi_config.mutex_handle) != OSAL_RETURN_OK) {
        R_PRINT_Log("[VSPI%d] Failed to create mutex\r\n", vspi);
        return R_VSP2_ERR_INVALID_STATE;
    }
    if (R_OSAL_ThsyncCondCreate(loc_VspiCondId[vspi], &vspi_state->vspi_config.cond_handle) != OSAL_RETURN_OK) {
        R_PRINT_Log("[VSPI%d] Failed to create cond var\r\n", vspi);
        return R_VSP2_ERR_INVALID_STATE;
    }

    /* Initialize Interrupts */
    if (locVspiIrqInit(Vsp2Unit) < 0) {
        return R_VSP2_ERR_INVALID_STATE;
    }
    return R_VSP2_ERR_SUCCESS;
}

static r_vsp2_Error_t R_VSP2_VSPI_PRV_SrcSurfaceSet(r_vsp2_Unit_t Vsp2Unit, r_vspi_Surface_t * Surf)
{
    int vspi = loc_Vsp2UnitIndex(Vsp2Unit);
    if (0 > vspi) {
        R_PRINT_Log("[%s] Invalid Vsp2Unit\r\n", __func__);
        return R_VSP2_ERR_INVALID_PARAMETER;
    }
    r_vsp2_Rpf_t *rpf = &loc_VspiState[vspi].rpf_config;
    int fmt_index = loc_FormatLookup(Surf->Fmt);
    if (fmt_index < 0) {
        R_PRINT_Log("[%s] Invalid format provided in VSPI Surface.\r\n");
        return R_VSP2_ERR_INVALID_PARAMETER;
    }
    rpf->mem_addr_y_rgb = Surf->FBuf;
    rpf->mem_addr_c0 = Surf->FBufC0;
    rpf->mem_addr_c1 = Surf->FBufC1;
    rpf->size_w = Surf->Width;
    rpf->size_h = Surf->Height;
    rpf->stride_y = Surf->StrideY;
    rpf->stride_c = Surf->StrideC;
    rpf->mem_stride = ((Surf->StrideY << 16) | Surf->StrideC);
    rpf->mem_stride_alpha = 0;
    rpf->data_swap = r_vspi_lookup[fmt_index].data_swap;
    rpf->format = r_vspi_lookup[fmt_index].format;
    rpf->bpp = r_vspi_lookup[fmt_index].bpp;
    rpf->yc_swap = r_vspi_lookup[fmt_index].yc_swap;
    rpf->uv_swap = r_vspi_lookup[fmt_index].uv_swap;
    return R_VSP2_ERR_SUCCESS;
}

static r_vsp2_Error_t R_VSP2_VSPI_PRV_DstSurfaceSet(r_vsp2_Unit_t Vsp2Unit, r_vspi_Surface_t *Surf)
{
    int vspi = loc_Vsp2UnitIndex(Vsp2Unit);
    if (0 > vspi) {
        R_PRINT_Log("[%s] Invalid Vsp2Unit\r\n", __func__);
        return R_VSP2_ERR_INVALID_PARAMETER;
    }
    r_vsp2_Wpf_t *wpf = &loc_VspiState[vspi].wpf_config;
    int fmt_index = loc_FormatLookup(Surf->Fmt);
    if (fmt_index < 0) {
        R_PRINT_Log("[%s] Invalid format provided in VSPI Surface.\r\n");
        return R_VSP2_ERR_INVALID_PARAMETER;
    }
    wpf->mem_addr_y_rgb = Surf->FBuf;
    wpf->mem_addr_c0 = Surf->FBufC0;
    wpf->mem_addr_c1 = Surf->FBufC1;
    wpf->clip_h_enable = 0;
    wpf->clip_v_enable = 0;
    wpf->writeback_enable = 1;
    wpf->pxa = 1;
    wpf->source_rpf = R_VSP2_VI6_WPFn_SRCRPF_RPF0_ACT_MASTER;
    wpf->data_swap = r_vspi_lookup[fmt_index].data_swap;
    wpf->format = r_vspi_lookup[fmt_index].format;
    wpf->yc_swap = r_vspi_lookup[fmt_index].yc_swap;
    wpf->uv_swap = r_vspi_lookup[fmt_index].uv_swap;
    Surf->StrideY = Surf->Width * r_vspi_lookup[fmt_index].bytes_per_pixel[0];
    Surf->StrideC = Surf->Width * r_vspi_lookup[fmt_index].bytes_per_pixel[1];
    wpf->mem_stride_y_rgb = Surf->StrideY;
    wpf->mem_stride_c = Surf->StrideC;
    return R_VSP2_ERR_SUCCESS;
}

static r_vsp2_Error_t R_VSP2_VSPI_PRV_Convert(r_vsp2_Unit_t Vsp2Unit)
{
    r_vsp2_Error_t ret = R_VSP2_ERR_SUCCESS;
    e_osal_return_t osal_err = OSAL_RETURN_OK;
    uint32_t reg_val;
    int vspi = loc_Vsp2UnitIndex(Vsp2Unit);
    if (0 > vspi) {
        R_PRINT_Log("[%s] : Invalid Vsp2Unit\r\n", __func__);
        return R_VSP2_ERR_INVALID_PARAMETER;
    }
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
    r_vspi_State * vspi_state = &loc_VspiState[vspi];
    r_vsp2_Wpf_t *wpf = &vspi_state->wpf_config;
    r_vsp2_Rpf_t *rpf = &vspi_state->rpf_config;

    if (R_OSAL_ThsyncMutexTryLock(vspi_state->vspi_config.mutex_handle) != OSAL_RETURN_OK) {
        R_PRINT_Log("[%s] VSPI%d : failed to lock mutex\r\n", __func__, vspi);
        return R_VSP2_ERR_INVALID_STATE;
    }

    /* Set RPF Registers */
    ret = R_VSP2_PRV_RpfSet(Vsp2Unit, 0, rpf);
    if (R_VSP2_ERR_SUCCESS != ret) {
         R_PRINT_Log("[%s] VSPI%d : R_VSP2_PRV_RpfSet Failed(%d)\r\n", __func__, vspi, ret);
         goto exit;
    }

    /* Set WPF Registers */
    ret = R_VSP2_PRV_WpfSet(Vsp2Unit, 0, wpf);
    if (R_VSP2_ERR_SUCCESS != ret) {
         R_PRINT_Log("[%s] VSPI%d : R_VSP2_PRV_WpfSet Failed(%d)\r\n", __func__, vspi, ret);
         goto exit;
    }

    vspi_state->vspi_config.running = true;

    /* Issue start command to VSP */
    ret = R_VSP2_Start(Vsp2Unit, 0);
    if (R_VSP2_ERR_SUCCESS != ret) {
        R_PRINT_Log("[%s] VSPI%d : Failed to start for format conversion\r\n", __func__, vspi);
        vspi_state->vspi_config.running = false;
        goto exit;
    }

    /* Wait for interrupt to signal conditional variable */
    while ((vspi_state->vspi_config.running == true) &&
           (OSAL_RETURN_OK == osal_err)) {
        osal_err = R_OSAL_ThsyncCondWaitForTimePeriod(vspi_state->vspi_config.cond_handle,
                                                      vspi_state->vspi_config.mutex_handle,
                                                      VSPI_CONVERT_TIMEOUT);
    }

    if (OSAL_RETURN_OK != osal_err) {
        R_PRINT_Log("[%s] VSPI%d : format conversion failed (osal_error=%d)\r\n", __func__, vspi, osal_err);
        vspi_state->vspi_config.running = false;
        ret = R_VSP2_ERR_INVALID_STATE;
        goto exit;
    }

exit:
    R_OSAL_ThsyncMutexUnlock(vspi_state->vspi_config.mutex_handle);
    return ret;
}

r_vsp2_Error_t R_VSP2_VSPI_ConvertSurface(r_vsp2_Unit_t Vsp2Unit, r_vspi_Surface_t * src_surf, r_vspi_Surface_t * dst_surf)
{
    r_vsp2_Error_t ret = R_VSP2_ERR_SUCCESS;
    ret = R_VSP2_VSPI_PRV_SrcSurfaceSet(Vsp2Unit, src_surf);
    if (R_VSP2_ERR_SUCCESS != ret) {
        return ret;
    }
    ret = R_VSP2_VSPI_PRV_DstSurfaceSet(Vsp2Unit, dst_surf);
    if (R_VSP2_ERR_SUCCESS != ret) {
        return ret;
    }
    ret = R_VSP2_VSPI_PRV_Convert(Vsp2Unit);
    return ret;
}

void R_VSP2_VSPI_PRV_FrameEndCallback(r_vsp2_Unit_t Vsp2Unit)
{
    int vspi = loc_Vsp2UnitIndex(Vsp2Unit);
    if (vspi < 0) return;
    loc_VspiState[vspi].vspi_config.running = false;
    R_OSAL_ThsyncCondSignal(loc_VspiState[vspi].vspi_config.cond_handle);
}