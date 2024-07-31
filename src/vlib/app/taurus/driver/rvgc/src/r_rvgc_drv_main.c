/*************************************************************************************************************
* taurus_rvgc_drv_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "r_taurus_bridge.h"

#include "r_rvgc_drv_api.h"
#include "r_taurus_rvgc_protocol.h"

/* Window Manager */
#include "r_wm_api.h"
#include "r_wm_config.h"

#include "r_print_api.h"

/*******************************************************************************
   Section: Local Defines
 */

/* Prio is lower than typical CIO drivers of (0x81),
   but higher than normal application (0x6a)
 */

#define LOC_VSYNCTHREAD_PRIO       0x75
#define LOC_VSYNCTHREAD_STACKSIZE  0x2000

/* Create Resource ID */
#define LOC_TAURUS_RVGC_RESOURCE_ID 0x8000U                             /* RESOURCE_ID_BSP            */
#define LOC_TAURUS_RVGC_THREAD_ID   (LOC_TAURUS_RVGC_RESOURCE_ID + 11U) /* OSAL_THREAD_TAURUS_RVGC    */
#define LOC_TAURUS_RVGC1_THREAD_ID  (LOC_TAURUS_RVGC_RESOURCE_ID + 59U) /* OSAL_THREAD_TAURUS_RVGC1   */
#define LOC_TAURUS_RVGC2_THREAD_ID  (LOC_TAURUS_RVGC_RESOURCE_ID + 60U) /* OSAL_THREAD_TAURUS_RVGC2   */
#define LOC_TAURUS_RVGC3_THREAD_ID  (LOC_TAURUS_RVGC_RESOURCE_ID + 61U) /* OSAL_THREAD_TAURUS_RVGC3   */
#define LOC_TAURUS_RVGC0_MQ_ID      (LOC_TAURUS_RVGC_RESOURCE_ID + 20U) /* OSAL_MQ_TAURUS_PARARVGC_00 */
#define LOC_TAURUS_RVGC1_MQ_ID      (LOC_TAURUS_RVGC_RESOURCE_ID + 42U) /* OSAL_MQ_TAURUS_PARARVGC_01 */
#define LOC_TAURUS_RVGC2_MQ_ID      (LOC_TAURUS_RVGC_RESOURCE_ID + 43U) /* OSAL_MQ_TAURUS_PARARVGC_02 */
#define LOC_TAURUS_RVGC3_MQ_ID      (LOC_TAURUS_RVGC_RESOURCE_ID + 44U) /* OSAL_MQ_TAURUS_PARARVGC_03 */

/* timeout (ms) */
#define LOC_RVGC_TIMEOUT_MS        1000U

#define LOC_NUM_DISPLAYS           4
#define LOC_MAX_NUM_LAYERS         5

/* Mq definitions */
#define LOC_TAURUS_RVGC_MQ_NUM_OF_MSG    1
#define LOC_TAURUS_RVGC_MQ_MSG_SIZE      sizeof(uint32_t)

/*******************************************************************************
   Section: Local Types
 */

struct Layer {
    uint32_t Initialized;
    r_wm_Surface_t Surf;
};

struct DisplayInstance {
    r_wm_Dev_t Disp;
    int Initialized;
    osal_thread_handle_t rvgc_thread_handle;
    uint64_t VsyncThreadStack[LOC_VSYNCTHREAD_STACKSIZE >> 3];
    uint32_t NumLayers;
    struct Layer Layers[LOC_MAX_NUM_LAYERS];
    r_wm_DispTiming_t DispTiming;
    osal_mq_handle_t    RvgcMqHandle;   /* Handle for Message Queue */
};

/*******************************************************************************
   Section: Local Variables
 */

static int (*locSendSignal)(int Guest, R_TAURUS_SignalId_t Signal, void* Option) = 0;

static struct DisplayInstance loc_DisplayInstances[LOC_NUM_DISPLAYS] = {
    {
        .Disp = R_WM_DEV_VOUT0,
        .Initialized = 0,
    },
    {
        .Disp = R_WM_DEV_VOUT1,
        .Initialized = 0,
        .DispTiming = {
            .ScreenWidth = 1920,
            .ScreenHeight = 1080,
            .PixelClock = 148500000,
            .VoutFlags = (uint32_t)(R_WM_VOUT_FLAG_HSYNC_ACTHI | R_WM_VOUT_FLAG_VSYNC_ACTHI
                                    | R_WM_VOUT_FLAG_DESYNC_ACTHI | R_WM_VOUT_FLAG_DVI_MODE),
            .VoutH = { 2200, 44, 148 },
            .VoutV = { 1125, 5, 36 },
            .VoutType = R_WM_VOUT_HDMI,
        },
    },
    {
        .Disp = R_WM_DEV_VOUT2,
        .Initialized = 0,
        .DispTiming = {
            .ScreenWidth = 1920,
            .ScreenHeight = 1080,
            .PixelClock = 148500000,
            .VoutFlags = (uint32_t)(R_WM_VOUT_FLAG_HSYNC_ACTHI | R_WM_VOUT_FLAG_VSYNC_ACTHI
                                    | R_WM_VOUT_FLAG_DESYNC_ACTHI | R_WM_VOUT_FLAG_DVI_MODE),
            .VoutH = { 2200, 44, 148 },
            .VoutV = { 1125, 5, 36 },
            .VoutType = R_WM_VOUT_HDMI,
        },
    },
    {
        .Disp = R_WM_DEV_VOUT3,
        .Initialized = 0,
    },
};

static int64_t loc_VSyncThreadFunc(void *data)
{
    r_wm_Error_t err;
    R_TAURUS_SignalId_t signal = 0;
    struct DisplayInstance *displ_inst = (struct DisplayInstance *)data;
    e_osal_return_t         osal_ret   = OSAL_RETURN_FAIL;
    osal_mq_id_t mq_id[] = {
        LOC_TAURUS_RVGC0_MQ_ID,
        LOC_TAURUS_RVGC1_MQ_ID,
        LOC_TAURUS_RVGC2_MQ_ID,
        LOC_TAURUS_RVGC3_MQ_ID
    };
    uint32_t id_num = ((uintptr_t)displ_inst - (uintptr_t)&loc_DisplayInstances[0]) / sizeof(struct DisplayInstance);
    uint32_t cmd;
    st_osal_mq_config_t mq_config;

    switch (displ_inst->Disp) {
    case R_WM_DEV_VOUT0:
        signal = RVGC_PROTOCOL_EVENT_VBLANK_DISPLAY0;
        break;
    case R_WM_DEV_VOUT1:
        signal = RVGC_PROTOCOL_EVENT_VBLANK_DISPLAY1;
        break;
    case R_WM_DEV_VOUT2:
        signal = RVGC_PROTOCOL_EVENT_VBLANK_DISPLAY2;
        break;
    case R_WM_DEV_VOUT3:
        signal = RVGC_PROTOCOL_EVENT_VBLANK_DISPLAY3;
        break;
    default:
        R_PRINT_Log("[TaurusRvgcMain]: loc_VSyncThreadFunc Disp(%d) faild\r\n", displ_inst->Disp);
        goto exit;
    }

    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusRvgcMain]: R_OSAL_MqInitializeMqConfigSt faild(%d)\r\n", osal_ret);
    }
    mq_config.max_num_msg = LOC_TAURUS_RVGC_MQ_NUM_OF_MSG;
    mq_config.msg_size    = LOC_TAURUS_RVGC_MQ_MSG_SIZE;
    osal_ret = R_OSAL_MqCreate(&mq_config, mq_id[id_num], &displ_inst->RvgcMqHandle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusRvgcMain]: R_OSAL_MqCreate failed(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                    osal_ret, mq_config.max_num_msg, mq_config.msg_size, mq_id[id_num]);
        goto exit;
    }

    err = R_WM_DevEventRegister(displ_inst->Disp, R_WM_EVENT_VBLANK, 0, (void *)&displ_inst->RvgcMqHandle);
    if (err != R_WM_ERR_SUCCESS) {
        R_PRINT_Log("[TaurusRvgcMain]: R_WM_DevEventRegister failed(%d)\r\n", err);
        goto clean;
    }

    while (1) {
        do {
            osal_ret = R_OSAL_MqReceiveForTimePeriod(displ_inst->RvgcMqHandle, 3000u, (void*)&cmd, LOC_TAURUS_RVGC_MQ_MSG_SIZE);
        } while (OSAL_RETURN_TIME == osal_ret);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[TaurusRvgcMain]: R_OSAL_MqReceiveForTimePeriod failed(%d)\r\n", osal_ret);
        }
        if (OSAL_RETURN_OK == osal_ret) {
            locSendSignal(1, signal, NULL);
        }
    }
clean:
    osal_ret = R_OSAL_MqDelete(displ_inst->RvgcMqHandle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusRvgcMain]: R_OSAL_MqDelete failed(%d)\r\n", err);
    }

exit:
    return 0;
}

static int32_t loc_CreateVsyncThread(struct DisplayInstance *DisplayInst)
{
    int32_t         ret      = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;

    st_osal_thread_config_t thread_config;

    osal_thread_id_t th_id[] = {
        LOC_TAURUS_RVGC_THREAD_ID,
        LOC_TAURUS_RVGC1_THREAD_ID,
        LOC_TAURUS_RVGC2_THREAD_ID,
        LOC_TAURUS_RVGC3_THREAD_ID
    };
    uint32_t id_num = ((uintptr_t)DisplayInst - (uintptr_t)&loc_DisplayInstances[0]) / sizeof(struct DisplayInstance);

    thread_config.func    = (p_osal_thread_func_t)loc_VSyncThreadFunc;
    thread_config.userarg = DisplayInst;
    osal_ret = R_OSAL_ThreadCreate(&thread_config, th_id[id_num], &DisplayInst->rvgc_thread_handle);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusRvgcMain]: R_OSAL_ThreadCreate failed(%d)\r\n", osal_ret);
        ret = 1;
    }

    return ret;
}

static r_wm_Error_t loc_DisplayInit(struct DisplayInstance *DisplayInst)
{
    r_wm_Error_t wmErr = R_WM_ERR_SUCCESS;
    r_wm_ColorFmt_t fmt = R_WM_FMT_ARGB8888;
    uint32_t color = 0xFF00FF00; /* green */
    r_wm_Dev_t Disp = DisplayInst->Disp;
    int32_t ret = 0;
    uint32_t layers;
    uint32_t width;
    uint32_t height;

    if (DisplayInst->Initialized) {
        R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit not Initialized\r\n");
        goto exit;
    }

    /* Initialise the WM Device */
    wmErr = R_WM_DevInit(Disp);
    if ((R_WM_ERR_SUCCESS != wmErr) && (R_WM_ERR_UNIT_ALREADY_INIT != wmErr)) {
        R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit R_WM_DevInit failed(%d)\r\n", wmErr);
        goto exit;
    }

    /* Will set-up VOut, i.e.: initi DU and VSPD */
    wmErr = R_WM_ScreenPropertySet(Disp, R_WM_SCREEN_PROP_TIMING, &DisplayInst->DispTiming);
    if ((R_WM_ERR_SUCCESS != wmErr) && (R_WM_ERR_UNIT_ALREADY_INIT != wmErr)) {
        R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit R_WM_ScreenPropertySet(R_WM_SCREEN_PROP_TIMING) failed(%d)\r\n", wmErr);
        goto exit;
    } else if (R_WM_ERR_SUCCESS == wmErr) {
        /*
         * Set the following two properties only if the WM Unit was
         * _not_ already initialized by another CR7 application
         * (i.e. skip if wmErr == R_WM_ERR_UNIT_ALREADY_INIT)
         */
        wmErr = R_WM_ScreenPropertySet(Disp, R_WM_SCREEN_PROP_FMT, &fmt);
        if (R_WM_ERR_SUCCESS != wmErr) {
            R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit R_WM_ScreenPropertySet(R_WM_SCREEN_PROP_FMT) failed(%d)\r\n", wmErr);
            goto exit;
        }
        wmErr = R_WM_ScreenPropertySet(Disp, R_WM_SCREEN_PROP_BKGCOLOR, &color);
        if (R_WM_ERR_SUCCESS != wmErr) {
            R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit R_WM_ScreenPropertySet(R_WM_SCREEN_PROP_BKGCOLOR) failed(%d)\r\n", wmErr);
            goto exit;
        }
    }

    wmErr = R_WM_DevInfoGet(Disp,
                            &layers,
                            &width,
                            &height);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit R_WM_DevInfoGet failed(%d)\r\n", wmErr);
        goto exit;
    }
    DisplayInst->NumLayers = layers;

    /* enable Screen */
    wmErr = R_WM_ScreenEnable(Disp, 1);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit R_WM_ScreenEnable failed(%d)\r\n", wmErr);
        goto exit;
    }

    ret = loc_CreateVsyncThread(DisplayInst);
    if (ret) {
        wmErr = R_WM_ERR_FAILED;
        R_PRINT_Log("[TaurusRvgcMain]: loc_DisplayInit loc_CreateVsyncThread failed(%d)\r\n", wmErr);
        goto exit;
    }

    DisplayInst->Initialized = 1;

exit:
    return wmErr;
}



static int loc_ioctl_layer_set_addr(struct taurus_rvgc_ioc_layer_set_addr_in *in,
                                    struct taurus_rvgc_ioc_layer_set_addr_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    struct Layer *layer;
    r_wm_Surface_t *surf;
    int ret = 0;

    out->res = 0;

    if (in->display >= LOC_NUM_DISPLAYS) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_addr in->display(%d) failed\r\n", in->display);
        goto exit;
    }

    displayInst = &loc_DisplayInstances[in->display];
    if (!displayInst->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_addr disp not initialized\n");
        goto exit;
    }

    if (in->layer >= displayInst->NumLayers) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_addr in->layer(%d) failed\n", in->layer);
        goto exit;
    }

    layer = &displayInst->Layers[in->layer];
    if (!layer->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_addr layer not initialized\n");
        goto exit;
    }

    surf = &layer->Surf;
    surf->FBuf = in->paddr;
    wmErr = R_WM_SurfacePropertySet(displayInst->Disp, R_WM_SURF_PROP_BUFFER, surf);
    if (R_WM_ERR_SUCCESS != wmErr) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_addr R_WM_SurfacePropertySet(R_WM_SURF_PROP_BUFFER) failed(%d)\n", wmErr);
        goto exit;
    }

exit:
    return ret;
}

static int loc_ioctl_layer_set_pos(struct taurus_rvgc_ioc_layer_set_pos_in *in,
                                   struct taurus_rvgc_ioc_layer_set_pos_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    struct Layer *layer;
    r_wm_Surface_t *surf;
    int ret = 0;

    out->res = 0;

    if (in->display >= LOC_NUM_DISPLAYS) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_pos in->display(%d) failed\r\n", in->display);
        goto exit;
    }

    displayInst = &loc_DisplayInstances[in->display];
    if (!displayInst->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_pos disp not initialized\n");
        goto exit;
    }

    if (in->layer >= displayInst->NumLayers) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_pos in->layer(%d) failed\n", in->layer);
        goto exit;
    }

    layer = &displayInst->Layers[in->layer];
    if (!layer->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_pos layer not initialized\n");
        goto exit;
    }

    surf = &layer->Surf;
    surf->PosX = in->pos_x;
    surf->PosY = in->pos_y;
    wmErr = R_WM_SurfacePropertySet(displayInst->Disp, R_WM_SURF_PROP_POS, surf);
    if (R_WM_ERR_SUCCESS != wmErr) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_pos R_WM_SurfacePropertySet(R_WM_SURF_PROP_POS) failed(%d)\n", wmErr);
        goto exit;
    }

exit:
    return ret;
}

static int loc_ioctl_layer_set_size(struct taurus_rvgc_ioc_layer_set_size_in *in,
                                    struct taurus_rvgc_ioc_layer_set_size_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    struct Layer *layer;
    r_wm_Surface_t *surf;
    int ret = 0;

    out->res = 0;

    if (in->display >= LOC_NUM_DISPLAYS) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_size in->display(%d) failed\r\n", in->display);
        goto exit;
    }

    displayInst = &loc_DisplayInstances[in->display];
    if (!displayInst->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_size disp not initialized\n");
        goto exit;
    }

    if (in->layer >= displayInst->NumLayers) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_size in->layer(%d) failed\n", in->layer);
        goto exit;
    }

    layer = &displayInst->Layers[in->layer];
    if (!layer->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_size layer not initialized\n");
        goto exit;
    }

    surf = &layer->Surf;
    surf->Width = in->size_w;
    surf->Height = in->size_h;
    surf->StrideY = in->size_w * 4;
    wmErr = R_WM_SurfacePropertySet(displayInst->Disp, R_WM_SURF_PROP_SIZE, surf);
    if (R_WM_ERR_SUCCESS != wmErr) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_set_pos R_WM_SurfacePropertySet(R_WM_SURF_PROP_SIZE) failed(%d)\n", wmErr);
        goto exit;
    }

exit:
    return ret;
}

static int loc_ioctl_layer_reserve(struct taurus_rvgc_ioc_layer_reserve_in *in,
                                   struct taurus_rvgc_ioc_layer_reserve_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    struct Layer *layer;
    r_wm_Surface_t *surf;
    int ret = 0;

    out->res = 0;

    if (in->display >= LOC_NUM_DISPLAYS) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve in->display(%d) failed\r\n", in->display);
        goto exit;
    }

    displayInst = &loc_DisplayInstances[in->display];
    if (!displayInst->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve disp not initialized\n");
        goto exit;
    }

    if (in->layer >= displayInst->NumLayers) { /* Fixed Problem Layer number 0-4 number NumLayer=5! */
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve in->layer(%d) failed\n", in->layer);
        goto exit;
    }

    layer = &displayInst->Layers[in->layer];
    if (layer->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve layer already initialized\n");
        goto exit;
    }

    surf = &layer->Surf;

    /* Get a Surface */
    wmErr = R_WM_SurfaceGet(displayInst->Disp, surf);
    if (R_WM_ERR_SUCCESS != wmErr) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve R_WM_SurfaceGet failed(%d)\n", wmErr);
        goto exit;
    }

    /* Initialize surface position. At this point in time it is
     * important only to set PosZ, because that defines which HW layer
     * is going to be requested. */
    layer->Surf.PosX = 0;
    layer->Surf.PosY = 0;
    layer->Surf.PosZ = in->layer;
    wmErr = R_WM_SurfacePropertySet(displayInst->Disp, R_WM_SURF_PROP_POS, surf);
    if (R_WM_ERR_SUCCESS != wmErr) {
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve R_WM_SurfacePropertySet(R_WM_SURF_PROP_POS) failed(%d)\n", wmErr);
        /* When few lines above the R_WM_SurfaceGet() returned success
         * it means that "a" layer was available, but unfortunately we
         * don't know which one...
         *
         * The reason why now R_WM_SurfacePropertySet() failed is
         * probably because the HW layer we want to use (specified
         * with PosZ) is already taken.
         *
         * Release the Surface and return error.
         */
        wmErr = R_WM_SurfaceRelease(displayInst->Disp, surf);
        if (R_WM_ERR_SUCCESS != wmErr) {
            /* Oops, this shouldn't have happened */
            R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_reserve R_WM_SurfaceRelease failed(%d)\n", wmErr);
        }

        ret = 1;
        out->res = 1;
        goto exit;
    }

    layer->Initialized = 1;

exit:
    return ret;
}

static int loc_ioctl_layer_release(struct taurus_rvgc_ioc_layer_release_in *in,
                                   struct taurus_rvgc_ioc_layer_release_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    struct Layer *layer;
    r_wm_Surface_t *surf;
    int ret = 0;

    out->res = 0;

    if (in->display >= LOC_NUM_DISPLAYS) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_release in->display(%d) failed\r\n", in->display);
        goto exit;
    }

    displayInst = &loc_DisplayInstances[in->display];
    if (!displayInst->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_release disp not initialized\n");
        goto exit;
    }

    if (in->layer >= displayInst->NumLayers) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_release in->layer(%d) failed\n", in->layer);
        goto exit;
    }

    layer = &displayInst->Layers[in->layer];
    if (!layer->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_release layer not initialized\n");
        goto exit;
    }

    surf = &layer->Surf;
    wmErr = R_WM_SurfaceRelease(displayInst->Disp, surf);
    if (R_WM_ERR_SUCCESS != wmErr) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_layer_release R_WM_SurfaceRelease failed(%d)\n", wmErr);
        goto exit;
    }

    layer->Initialized = 0;

exit:
    return ret;
}

static int loc_ioctl_display_flush(struct taurus_rvgc_ioc_display_flush_in *in,
                                   struct taurus_rvgc_ioc_display_flush_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    int ret = 0;

    out->res = 0;

    if (in->display < LOC_NUM_DISPLAYS) {
        displayInst = &loc_DisplayInstances[in->display];
        if (displayInst->Initialized) {
            wmErr = R_WM_ScreenSurfacesFlush(displayInst->Disp, in->blocking);
            if (R_WM_ERR_SUCCESS != wmErr) {
                ret = 1;
                out->res = 1;
                R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_flush R_WM_ScreenSurfacesFlush failed(%d)\n", wmErr);
                goto exit;
            }
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_flush not Initialized\n");
            ret = 1;
            out->res = 1;
        }
    } else {
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_flush in->display(%d) failed\n", in->display);
        ret = 1;
        out->res = 1;
    }

exit:
    return ret;
}

static int loc_ioctl_display_init(struct taurus_rvgc_ioc_display_init_in *in,
                                  struct taurus_rvgc_ioc_display_init_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    int ret = 0;

    out->res = 0;

    if (in->display < LOC_NUM_DISPLAYS) {
        displayInst = &loc_DisplayInstances[in->display];
        if (!displayInst->Initialized) {
            wmErr = loc_DisplayInit(displayInst);
            if (R_WM_ERR_SUCCESS != wmErr) {
                ret = 1;
                out->res = 1;
                R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_init loc_DisplayInit failed(%d)\n", wmErr);
                goto exit;
            }
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_init already Initialized\n");
            ret = 1;
            out->res = 1;
        }
    } else {
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_init in->display(%d) failed\n", in->display);
        ret = 1;
        out->res = 1;
    }

exit:
    return ret;
}

static int loc_ioctl_display_get_info(struct taurus_rvgc_ioc_display_get_info_in *in,
                                      struct taurus_rvgc_ioc_display_get_info_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    int ret = 0;
    uint32_t layers = 0;
    uint32_t width  = 0;
    uint32_t height = 0;

    out->res = 0;

    if (in->display < LOC_NUM_DISPLAYS) {
        displayInst = &loc_DisplayInstances[in->display];
        if (displayInst->Initialized) {
            wmErr = R_WM_DevInfoGet(displayInst->Disp,
                                    &layers,
                                    &width,
                                    &height);
            if (R_WM_ERR_SUCCESS != wmErr) {
                ret = 1;
                out->res = 1;
                R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_get_info R_WM_DevInfoGet failed(%d)\n", wmErr);
                goto exit;
            } else {
                out->layers = layers;
                out->width  = width;
                out->height = height;
            }
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_get_info not Initialized\n");
            ret = 1;
            out->res = 1;
        }
    } else {
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_display_get_info in->display(%d) failed\n", in->display);
        ret = 1;
        out->res = 1;
    }

exit:
    return ret;
}

static int loc_ioctl_surface_alpha_set(struct taurus_rvgc_ioc_surf_alpha_set_in *in,
                                  struct taurus_rvgc_ioc_surf_alpha_set_out *out)
{
    r_wm_Error_t wmErr;
    struct DisplayInstance *displayInst;
    struct Layer *layer;
    r_wm_Surface_t *surf;
    int ret = 0;

    out->res = 0;

    if (in->display >= LOC_NUM_DISPLAYS) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_surface_alpha_set in->display(%d) failed\r\n", in->display);
        goto exit;
    }

    displayInst = &loc_DisplayInstances[in->display];
    if (!displayInst->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_surface_alpha_set disp not initialized\n");
        goto exit;
    }

    if (in->layer >= displayInst->NumLayers) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_surface_alpha_set in->layer(%d) failed\n", in->layer);
        goto exit;
    }

    layer = &displayInst->Layers[in->layer];
    if (!layer->Initialized) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_surface_alpha_set layer not initialized\n");
        goto exit;
    }

    surf = &layer->Surf;
    surf->Alpha = in->alpha;
    wmErr = R_WM_SurfacePropertySet(displayInst->Disp, R_WM_SURF_ALPHA_SET, surf);

    if (R_WM_ERR_SUCCESS != wmErr) {
        ret = 1;
        out->res = 1;
        R_PRINT_Log("[TaurusRvgcMain]: loc_ioctl_surface_alpha_set R_WM_SurfacePropertySet(R_WM_SURF_ALPHA_SET) failed(%d)\n", wmErr);
        goto exit;
    }

exit:
    return ret;
}

int R_Rvgc_DRV_Open(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 1;
}


int R_Rvgc_DRV_Close(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 0;
}


ssize_t R_Rvgc_DRV_Read(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 0;
}


ssize_t R_Rvgc_DRV_Write(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 0;
}


ssize_t R_Rvgc_DRV_IoCtl(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    ssize_t x;
    uint32_t ioctl_id;
    struct taurus_rvgc_cmd_msg     *cmd;
    struct taurus_rvgc_res_msg     *res;

    ioctl_id = Cmd->Par1;
    cmd = (struct taurus_rvgc_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_rvgc_res_msg *)(size_t)Cmd->Par3;

    switch (ioctl_id) {
    case RVGC_PROTOCOL_IOC_LAYER_SET_ADDR:
        x = loc_ioctl_layer_set_addr(&cmd->params.ioc_layer_set_addr, &res->params.ioc_layer_set_addr);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_layer_set_addr(RVGC_PROTOCOL_IOC_LAYER_SET_ADDR) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_LAYER_SET_POS:
        x = loc_ioctl_layer_set_pos(&cmd->params.ioc_layer_set_pos, &res->params.ioc_layer_set_pos);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_layer_set_pos(RVGC_PROTOCOL_IOC_LAYER_SET_POS) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_LAYER_SET_SIZE:
        x = loc_ioctl_layer_set_size(&cmd->params.ioc_layer_set_size, &res->params.ioc_layer_set_size);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_layer_set_size(RVGC_PROTOCOL_IOC_LAYER_SET_SIZE) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_LAYER_RESERVE:
        x = loc_ioctl_layer_reserve(&cmd->params.ioc_layer_reserve, &res->params.ioc_layer_reserve);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_layer_reserve(RVGC_PROTOCOL_IOC_LAYER_RESERVE) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_LAYER_RELEASE:
        x = loc_ioctl_layer_release(&cmd->params.ioc_layer_release, &res->params.ioc_layer_release);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_layer_release(RVGC_PROTOCOL_IOC_LAYER_RELEASE) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_DISPLAY_FLUSH:
        x = loc_ioctl_display_flush(&cmd->params.ioc_display_flush, &res->params.ioc_display_flush);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_display_flush(RVGC_PROTOCOL_IOC_DISPLAY_FLUSH) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_DISPLAY_INIT:
        x = loc_ioctl_display_init(&cmd->params.ioc_display_init, &res->params.ioc_display_init);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_display_init(RVGC_PROTOCOL_IOC_DISPLAY_INIT) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_DISPLAY_GET_INFO:
        x = loc_ioctl_display_get_info(&cmd->params.ioc_display_get_info, &res->params.ioc_display_get_info);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_display_get_info(RVGC_PROTOCOL_IOC_DISPLAY_GET_INFO) failed(%d)\n", x);
        }
        break;
    case RVGC_PROTOCOL_IOC_SURFACE_ALPHA_SET:
        x = loc_ioctl_surface_alpha_set(&cmd->params.ioc_surf_alpha_set, &res->params.ioc_surf_alpha_set);
        if (x == 0) {
            res->hdr.Aux = sizeof(struct taurus_rvgc_res_msg);
        } else {
            R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl loc_ioctl_surface_alpha_set(RVGC_PROTOCOL_IOC_SURFACE_ALPHA_SET) failed(%d)\n", x);
        }
        break;
    default:
        x = 1; //fail
        R_PRINT_Log("[TaurusRvgcMain]: R_Rvgc_DRV_IoCtl ioctl_id(%d) failed(%d)\n", ioctl_id, x);
        break;
    }

    return x;
}


ssize_t R_Rvgc_DRV_Status(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 0;
}

void R_Rvgc_DRV_SetSendSignalImpl(int (*SendSignalImpl)(int Guest, R_TAURUS_SignalId_t Signal, void* Option))
{
    locSendSignal = SendSignalImpl;
}
