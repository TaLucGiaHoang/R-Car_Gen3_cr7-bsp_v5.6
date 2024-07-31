/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : rearview_main.c
 * Version      : 1.0
 * Description  : Demostrate how to use a CIO vin device.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include "rcar-xos/osal/r_osal_api.h"
#include "r_cio_api.h"
#include "r_cio_vin_public.h"
#include "rearview_camera.h"
#include "display.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#if !defined(R_USE_VIN)
#error "rearview_camera requires the R_USE_VIN option. Please execute 'cmake -DR_USE_VIN=ON <build-dir>' to enable it."
#endif

#if defined(R_TARGET_BOARD_SALVATORXS)
#define RVC_CIO_VIN_NAME_CSI40  "RCar-VIN:0"
#define RVC_CIO_VIN_NAME_CSI20  "RCar-VIN:5"
#endif


#define REARVIEW_RESOURCE_ID         0x8000U

#define REARVIEW_THREAD_00_ID  (REARVIEW_RESOURCE_ID +  3U) /* OSAL_THREAD_REARVIEW_00 */
#define REARVIEW_THREAD_01_ID  (REARVIEW_RESOURCE_ID +  4U) /* OSAL_THREAD_REARVIEW_01 */

#define REARVIEW_MQ_ID_CLUSTER (REARVIEW_RESOURCE_ID + 18U) /* OSAL_MQ_REARVIEW_CAMERA_APP_00 */
#define REARVIEW_MQ_ID_CID     (REARVIEW_RESOURCE_ID + 38U) /* OSAL_MQ_REARVIEW_CAMERA_APP_01 */

#define REARVIEW_MQ_NUM_OF_MSG    1
#define REARVIEW_MQ_MSG_SIZE      sizeof(uint32_t)

#define RVC_TASK_PRIORITY (100)
#define RVC_LAYER_MAX_NUM (2)

#define RVC_OK  (0)
#define RVC_ERR (1)

#define RVC_DISP_SYNC_NO_WAIT (0)
#define RVC_DISP_SYNC_WAIT    (1)

#define RVC_INFO(info_str)                                                                                             \
    {                                                                                                                  \
        R_PRINT_Log(info_str);                                                                                       \
    }
#define RVC_ASSERT(true_condition, info_str)                                                                           \
    {                                                                                                                  \
        if (true_condition)                                                                                            \
            R_PRINT_Log(info_str);                                                                                   \
    }

#define TIMEOUT_MS 1000 // 1000 milisecond

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static const rvc_Config_t * loc_RvcCfgPtr;
static osal_mq_handle_t mq_handle[2];
static bool                 loc_LayerOn[RVC_LAYER_MAX_NUM] = {true, true};
extern osal_memory_manager_handle_t    g_hndl_mmngr;

extern const rvc_Surface_t SurfaceCluster[];
extern const rvc_Surface_t SurfaceCid[];

/**********************************************************************************************************************
 * Function Name: loc_RvcTask
 * Description  : main task function
 * Arguments    : Arg - thread ID.
 * Return Value :
 *********************************************************************************************************************/
static void * loc_RvcTask(void * Arg)
{
    uint32_t           task_id         = (uint32_t)Arg;
    int                vin_cio_channel = 0;
    r_cio_vin_Config_t vin_cfg;
    uintptr_t          p_valid_frame;
    ssize_t            ret;
    r_wm_Display_t     display;
    r_wm_Layer_t       layer;
    rvc_DispCfg_t *    pdisp_cfg;
    rvc_Layer_t *      pdisp_layer;
    bool               enable_display;
    osal_memory_buffer_handle_t bufferhandle;
    uint32_t           alloc_size_csi = 0;
    void *cpuPTR;
    uint8_t *cpuPTR_tmp;
    uint32_t recvdata;
    st_osal_mq_config_t mq_config;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /* Initialize cluster and CID display layer */
    if (RVC_ID_LAYER_CLUSTER == task_id) {
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.PosX    = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].PosX;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.PosY    = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].PosY;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.PosZ    = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].PosZ;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.Width   = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Width;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.Height  = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Height;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.StrideY = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].StrideY;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.StrideC = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].StrideC;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.Fmt     = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Fmt;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.Handle  = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Handle;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.FBuf    = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].FBuf;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.FBufC0  = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].FBufC0;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.FBufC1  = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].FBufC1;
        loc_RvcCfgPtr->ClusterLayerPtr->VoutSurface.Doc     = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Doc;

        vin_cfg.ImageInWidth      = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].ImageInWidth;
        vin_cfg.ImageInHeight     = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].ImageInHeight;
        vin_cfg.ImageScaledWidth  = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].ImageScaledWidth;
        vin_cfg.ImageScaledHeight = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].ImageScaledHeight;
        vin_cfg.pos_x             = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].PreClipPosX;
        vin_cfg.pos_y             = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].PreClipPosY;
        vin_cfg.ImageUvaof        = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Uvaof;
        vin_cfg.VmcInf            = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].VmcInf;
        vin_cfg.VmcIm             = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].VmcIm;
        vin_cfg.DataRate          = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].DataRate;
        vin_cfg.Lanes             = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Lanes;
        vin_cfg.Interlaced        = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].Interlaced;
        vin_cfg.ColorSpaceOut     = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].ColorSpaceOut;
        vin_cfg.BoundaryMbps      = SurfaceCluster[loc_RvcCfgPtr->settings_in_cluster].BoundaryMbps;

        pdisp_layer = loc_RvcCfgPtr->ClusterLayerPtr;
        pdisp_cfg = loc_RvcCfgPtr->ClusterDisplayPtr;
    }
    else /* if (RVC_ID_LAYER_CID == task_id) */
    {
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.PosX    = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].PosX;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.PosY    = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].PosY;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.PosZ    = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].PosZ;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.Width   = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Width;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.Height  = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Height;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.StrideY = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].StrideY;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.StrideC = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].StrideC;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.Fmt     = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Fmt;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.Handle  = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Handle;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.FBuf    = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].FBuf;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.FBufC0  = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].FBufC0;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.FBufC1  = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].FBufC1;
        loc_RvcCfgPtr->CidLayerPtr->VoutSurface.Doc     = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Doc;

        vin_cfg.ImageInWidth      = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].ImageInWidth;
        vin_cfg.ImageInHeight     = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].ImageInHeight;
        vin_cfg.ImageScaledWidth  = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].ImageScaledWidth;
        vin_cfg.ImageScaledHeight = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].ImageScaledHeight;
        vin_cfg.pos_x             = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].PreClipPosX;
        vin_cfg.pos_y             = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].PreClipPosY;
        vin_cfg.ImageUvaof        = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Uvaof;
        vin_cfg.VmcInf            = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].VmcInf;
        vin_cfg.VmcIm             = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].VmcIm;
        vin_cfg.DataRate          = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].DataRate;
        vin_cfg.Lanes             = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Lanes;
        vin_cfg.Interlaced        = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].Interlaced;
        vin_cfg.ColorSpaceOut     = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].ColorSpaceOut;
        vin_cfg.BoundaryMbps      = SurfaceCid[loc_RvcCfgPtr->settings_in_cid].BoundaryMbps;

        pdisp_layer = loc_RvcCfgPtr->CidLayerPtr;
        pdisp_cfg = loc_RvcCfgPtr->CidDisplayPtr;
    }

    /* Open VIN channel and get handle */
    if (RVC_ID_LAYER_CLUSTER == task_id) /* CSI40 */
    {
        vin_cio_channel = R_CIO_Open(RVC_CIO_VIN_NAME_CSI40, "rwb");
    }
    else if (RVC_ID_LAYER_CID == task_id) /* CSI20 */
    {
        vin_cio_channel = R_CIO_Open(RVC_CIO_VIN_NAME_CSI20, "rwb");
    }
    if (vin_cio_channel == 0) {
        R_PRINT_Log("RVC: RCar-VIN CIO open failed.\r\n");
        while (1) {
            R_OSAL_ThreadSleepForTimePeriod(10);
        }
    }
    
    vin_cfg.ImageOutWidth     = pdisp_layer->VoutSurface.Width;
    vin_cfg.ImageOutHeight    = pdisp_layer->VoutSurface.Height;
    if ( R_WM_FMT_ARGB8888 == pdisp_layer->VoutSurface.Fmt)
    {
        vin_cfg.ImageStride    = pdisp_layer->VoutSurface.StrideY / 4;

        vin_cfg.OutputFmt      = R_VIN_OUTF_ARGB8888;
        alloc_size_csi = 3 * (vin_cfg.ImageStride * 4 * vin_cfg.ImageOutHeight);
    }
    else /* R_WM_FMT_YCBCR420 */
    {
        vin_cfg.ImageStride    = pdisp_layer->VoutSurface.StrideY;
        vin_cfg.OutputFmt      = R_VIN_OUTF_YCBCR420;
        alloc_size_csi = 3 * (vin_cfg.ImageUvaof + vin_cfg.ImageStride * vin_cfg.ImageOutHeight);
    }

    osal_ret = R_OSAL_MmngrAlloc(g_hndl_mmngr, alloc_size_csi, 256, &bufferhandle);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[loc_RvcTask]: ERROR!!! R_OSAL_MmngrAlloc return value = %d, task_id:%d\n", osal_ret, (int)task_id);
    }

    osal_ret = R_OSAL_MmngrGetCpuPtr(bufferhandle, &cpuPTR);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[loc_RvcTask]: ERROR!!! R_OSAL_MmngrGetCpuPtr return value = %d, task_id:%d\n", osal_ret, (int)task_id);
    }

    cpuPTR_tmp = (uint8_t*)cpuPTR;
    vin_cfg.FrameBuffer[0] = (uintptr_t)cpuPTR_tmp;
    vin_cfg.FrameBuffer[1] = (uintptr_t)cpuPTR_tmp+alloc_size_csi/3;
    vin_cfg.FrameBuffer[2] = (uintptr_t)cpuPTR_tmp+(alloc_size_csi/3)*2;

    /* Configure VIN channel */
    if (RVC_ID_LAYER_CLUSTER == task_id) /* CSI40 */
    {
        vin_cfg.Csi2Type       = R_CIO_VIN_CSI2_TYPE_CSI40;
    }
    else if (RVC_ID_LAYER_CID == task_id) /* CSI20 */
    {
        vin_cfg.Csi2Type       = R_CIO_VIN_CSI2_TYPE_CSI20;
    }

    ret = R_CIO_IoCtl(vin_cio_channel, (void *)R_CIO_VIN_IOC_CONFIG, (void *)&vin_cfg, 0);
    if (0 != ret)
    {
        R_PRINT_Log("RVC: RCar-VIN config failed(%d).\r\n", ret);
        while (1)
        {
                R_OSAL_ThreadSleepForTimePeriod(10);
        }
    }

    if (R_WM_ERR_SUCCESS == R_WM_DisplayConfig(pdisp_cfg->Port, &display)) {
        display.BackgroundColor = pdisp_cfg->BackgroundColor;
        if (R_WM_ERR_SUCCESS == R_WM_DisplayInit(&display)) {
            if (R_WM_ERR_SUCCESS == R_WM_DisplayEnable(&display, true)) {
                
                layer.ParentPtr             = &display;
                layer.VoutSurface.PosX      = pdisp_layer->VoutSurface.PosX;
                layer.VoutSurface.PosY      = pdisp_layer->VoutSurface.PosY;
                layer.VoutSurface.PosZ      = pdisp_layer->VoutSurface.PosZ;
                layer.VoutSurface.Width     = pdisp_layer->VoutSurface.Width;
                layer.VoutSurface.Height    = pdisp_layer->VoutSurface.Height;
                layer.VoutSurface.Fmt       = pdisp_layer->VoutSurface.Fmt;
                layer.VoutSurface.Handle    = 0;
                layer.VoutSurface.FBuf      = 0;
                layer.VoutSurface.FBufC0    = 0;
                layer.VoutSurface.FBufC1    = 0;
                layer.VoutSurface.StrideY   = pdisp_layer->VoutSurface.StrideY;
                layer.VoutSurface.StrideC   = pdisp_layer->VoutSurface.StrideC;

                if (R_WM_ERR_SUCCESS == R_WM_LayerInit(&layer)) {
                    ret = RVC_OK;
                }
            }
        }
    }

    /* Register a semaphore for frame ready event */
    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
    if(OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("RVC: R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
    }
    mq_config.max_num_msg = REARVIEW_MQ_NUM_OF_MSG;
    mq_config.msg_size    = REARVIEW_MQ_MSG_SIZE;
    if (RVC_ID_LAYER_CLUSTER == task_id)
    {
        osal_ret = R_OSAL_MqCreate(&mq_config, REARVIEW_MQ_ID_CLUSTER, &mq_handle[task_id]);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("R_OSAL_MqCreate(CLUSTER) fail(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                    osal_ret, mq_config.max_num_msg, mq_config.msg_size, REARVIEW_MQ_ID_CLUSTER);
        }
    }
    else if (RVC_ID_LAYER_CID == task_id)
    {
        osal_ret = R_OSAL_MqCreate(&mq_config, REARVIEW_MQ_ID_CID, &mq_handle[task_id]);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("R_OSAL_MqCreate(CID) fail(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                    osal_ret, mq_config.max_num_msg, mq_config.msg_size, REARVIEW_MQ_ID_CID);
        }
    }

    ret = R_CIO_IoCtl(vin_cio_channel, (void *)R_CIO_VIN_IOC_REGISTER_FRAME_READY_EVT, (void *)&mq_handle[task_id], NULL);
    /* Start capture */
    if (RVC_OK == ret) {
        RVC_INFO("[RVC] Start capturing...\r\n");
        ret = R_CIO_IoCtl(vin_cio_channel, (void *)R_CIO_VIN_IOC_START_CAPTURE, 0, 0);
    }
    RVC_ASSERT((RVC_OK != ret), "[RVC] Failed in starting VIN capture.\r\n");

    while (RVC_OK == ret)
    {
        /* Check and toggle layer */
        if (RVC_OK == ret) {
            ret = R_WM_LayerShow(&layer, loc_LayerOn[task_id]);
            RVC_ASSERT((RVC_OK != ret), "[RVC] Failed in toggling display.\r\n");
            enable_display = loc_LayerOn[task_id];
        }

        if (RVC_OK == ret) {
            do {
                osal_ret = R_OSAL_MqReceiveForTimePeriod(mq_handle[task_id], TIMEOUT_MS, (void*)&recvdata, REARVIEW_MQ_MSG_SIZE);
            } while (OSAL_RETURN_TIME == osal_ret);
            if ((OSAL_RETURN_OK == osal_ret) && (true == loc_LayerOn[task_id])) {

                p_valid_frame = 0;
                ret           = R_CIO_IoCtl(vin_cio_channel, (void *)R_CIO_VIN_IOC_GET_VALID_FRAME, &p_valid_frame, 0);
                if ((RVC_OK == ret) && (true == enable_display)) {
                    layer.VoutSurface.FBuf = (uintptr_t)p_valid_frame;
                    layer.VoutSurface.FBufC0 = (uintptr_t)p_valid_frame + vin_cfg.ImageUvaof;
                    layer.VoutSurface.FBufC1 = 0;

                    if (R_WM_ERR_SUCCESS == ret) {
                        ret = R_WM_SurfacePropertySet(layer.ParentPtr->Dev, R_WM_SURF_PROP_BUFFER, (void *)&layer.VoutSurface);
                    }
                    if (R_WM_ERR_SUCCESS == ret) {
                        ret = R_WM_ScreenSurfacesFlush(layer.ParentPtr->Dev, RVC_DISP_SYNC_NO_WAIT);
                    }
                }
            }
        }
    }

    if (vin_cio_channel != 0) {
        R_CIO_IoCtl(vin_cio_channel, (void *)R_CIO_VIN_IOC_STOP_CAPTURE, 0, 0);
        R_CIO_Close(vin_cio_channel);
    }

    RVC_ASSERT((RVC_OK != ret), "[RVC] Task terminated.\r\n");

    return 0;
}
/**********************************************************************************************************************
 * End of function loc_RvcTask
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: RVC_Main
 * Description  : main function of rearview camera app.
 * Arguments    : ConfigPtr - Pointer to app configuration.
 * Return Value : void
 *********************************************************************************************************************/
void RVC_Main(const rvc_Config_t * ConfigPtr)
{
    st_osal_thread_config_t thrd1_cfg;
    st_osal_thread_config_t thrd2_cfg;

    osal_thread_handle_t    thrd_hndl = OSAL_THREAD_HANDLE_INVALID;
    e_osal_return_t         osal_ret  = OSAL_RETURN_OK;

    loc_RvcCfgPtr = ConfigPtr;

    R_PRINT_Log("=== Rearview Camera Start ===\r\n");

    if (loc_RvcCfgPtr->ClusterDisplayPtr) {
        RVC_INFO("[RVC] Start task...\r\n");

        /* Task1: Capture and display video on cluster display */
        thrd1_cfg.func       = (p_osal_thread_func_t)loc_RvcTask;
        thrd1_cfg.task_name  = "RvcTask-Cluster";
        thrd1_cfg.userarg    = (void *)RVC_ID_LAYER_CLUSTER;
        thrd1_cfg.stack_size = 0x4000;
        thrd1_cfg.priority   = OSAL_THREAD_PRIORITY_TYPE10;
        osal_ret = R_OSAL_ThreadCreate(&thrd1_cfg, REARVIEW_THREAD_00_ID, &thrd_hndl);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("RVC: R_OSAL_ThreadCreate Thread01 failed(%d)\r\n", osal_ret);
        }
    }

    if (loc_RvcCfgPtr->CidDisplayPtr) {
        /* Task2: Capture and display video on CID display */
        thrd2_cfg.func       = (p_osal_thread_func_t)loc_RvcTask;
        thrd1_cfg.task_name  = "RvcTask-CID";
        thrd2_cfg.userarg    = (void *)RVC_ID_LAYER_CID;
        thrd2_cfg.stack_size = 0x4000;
        thrd2_cfg.priority   = OSAL_THREAD_PRIORITY_TYPE10;
        osal_ret = R_OSAL_ThreadCreate(&thrd2_cfg, REARVIEW_THREAD_01_ID, &thrd_hndl);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("RVC: R_OSAL_ThreadCreate Thread02 failed(%d)\r\n", osal_ret);
        }
    }

    RVC_ASSERT((ConfigPtr == 0), "[RVC] Failed in checking configuration.\r\n");
}
/***********************************************************************************************************************
 End of function RVC_Main
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: RVC_ToggleDisplayLayer
 * Description  : Turn on or off specified display layer.
 * Arguments    : LayerIndex - Index of layer.
 *                On - 0:on, 1:off
 * Return Value : void
 *********************************************************************************************************************/
void RVC_ToggleDisplayLayer(uint8_t LayerIndex, bool On)
{
    if (LayerIndex < RVC_LAYER_MAX_NUM) {
        loc_LayerOn[LayerIndex] = On;
    }
}
/***********************************************************************************************************************
 End of function RVC_ToggleDisplayLayer
 **********************************************************************************************************************/
