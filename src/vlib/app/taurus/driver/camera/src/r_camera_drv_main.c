/*************************************************************************************************************
* taurus_camera_drv_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"

#include "r_taurus_bridge.h"

#include "r_camera_drv_api.h"
#include "r_taurus_camera_protocol.h"

#include "r_cio_api.h"
#include "r_cio_vin_public.h"

#include "r_print_api.h"

/*******************************************************************************
  Section: Local Defines and Types
*/

#define LOC_NUM_CAMERA             0x1
#define LOC_NUM_BUFFER_CELL        0x4

/* Create Resource ID */
#define LOC_TAURUS_CAMERA_RESOURCE_ID 0x8000U                               /* RESOURCE_ID_BSP               */
#define LOC_TAURUS_CAMERA_THREAD_ID   (LOC_TAURUS_CAMERA_RESOURCE_ID + 45U) /* OSAL_THREAD_TAURUS_PARACAMERA */
#define LOC_TAURUS_CAMERA_MQ_ID       (LOC_TAURUS_CAMERA_RESOURCE_ID + 21U) /* OSAL_MQ_TAURUS_PARAVIN        */
#define LOC_TAURUS_CAMERA_MUTEX_ID    (LOC_TAURUS_CAMERA_RESOURCE_ID + 71U) /* OSAL_MUTEX_TAURUS_PARACAMERA  */


/* timeout (ms) */
#define LOC_CAMERA_TIMEOUT_MS 1000 /* 1000 milisecond */

/* Mq definitions */
#define LOC_TAURUS_CAMERA_MQ_NUM_OF_MSG    1
#define LOC_TAURUS_CAMERA_MQ_MSG_SIZE      sizeof(uint32_t)

/* VIn VMC register INF bit supported */
#define LOC_TAURUS_CAMERA_VMC_INF_RGB888      0x01     /**< VIN VMC INF RGB888 */
#define LOC_TAURUS_CAMERA_VMC_INF_YUV8_BT601  0x02     /**< VIN VMC INF YUV8_BT601 */

/* VIn VMC register IM bit supported */
#define LOC_TAURUS_CAMERA_VMC_IM_ODD          (0 << 3)
#define LOC_TAURUS_CAMERA_VMC_IM_ODD_EVEN     (1 << 3)
#define LOC_TAURUS_CAMERA_VMC_IM_EVEN         (2 << 3)
#define LOC_TAURUS_CAMERA_VMC_IM_FULL         (3 << 3)

/* VIN Onput Interface Format */
#define LOC_TAURUS_CAMERA_OUTF_YCBCR420       0  /**< VIN Output Format YcBcr420 */
#define LOC_TAURUS_CAMERA_OUTF_ARGB8888       1  /**< VIN Output Format ARGB8888 */

typedef struct CameraInstance
{
    char                    CioName[64];
    uint32_t                Channel;
    int32_t                 Handle;
    int32_t                 Initialized;
    r_cio_vin_Config_t      Config;

    osal_mutex_handle_t     BufMutex;
    uint32_t                EmptyBufCnt;
    uint32_t                VacantBufCellCnt;
    taurus_camera_buffer_t  Buffer[LOC_NUM_BUFFER_CELL];

    osal_mq_handle_t        FrameReadyMq;
    uint32_t                StopEventThread;
    osal_thread_handle_t    CameraThreadHandle;
} CameraInstance_t;


/*******************************************************************************
  Section: Local Variables and Functions
*/
static int (*locSendSignal)(int Guest, R_TAURUS_SignalId_t Signal, void* Option) = 0;

static CameraInstance_t s_CameraInstance[LOC_NUM_CAMERA] = {
    {
        .CioName = "RCar-VIN:5",
        .Channel = 0,
        .Handle = -1,
        .Initialized = 0,
        .VacantBufCellCnt = LOC_NUM_BUFFER_CELL,
        .Config = {
            .ImageInWidth       = 720,
            .ImageInHeight      = 480,
            .ImageScaledWidth   = 720,
            .ImageScaledHeight  = 480,
            .ImageOutWidth      = 720,
            .ImageOutHeight     = 480,
            .ImageUvaof         = 0,
            .ImageStride        = 720,
            .VmcInf             = LOC_TAURUS_CAMERA_VMC_INF_YUV8_BT601,
            .VmcIm              = LOC_TAURUS_CAMERA_VMC_IM_FULL,
            .DataRate           = 216,
            .Lanes              = 1,
            .Interlaced         = true,
            .ColorSpaceOut      = 0,
            .BoundaryMbps       = 0,
            .Csi2Type           = R_CIO_VIN_CSI2_TYPE_CSI20,
            .OutputFmt          = LOC_TAURUS_CAMERA_OUTF_ARGB8888,
            .pos_x              = 0,
            .pos_y              = 0,
        },
    },
};

static int32_t pop_buffer(CameraInstance_t* pinst, taurus_camera_buffer_t* pbuf)
{
    int32_t ret = 1;
    int32_t i;
    e_osal_return_t osal_ret;

    do
    {
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->BufMutex, LOC_CAMERA_TIMEOUT_MS);
    } while (OSAL_RETURN_TIME == osal_ret);

    if (OSAL_RETURN_OK == osal_ret)
    {
        for (i = 0; i < LOC_NUM_BUFFER_CELL; i++)
        {
            if (pinst->Buffer[i].address != 0)
            {
                *pbuf = pinst->Buffer[i];
                pinst->Buffer[i].index = 0;
                pinst->Buffer[i].address = 0;
                pinst->VacantBufCellCnt++;
                pinst->EmptyBufCnt--;
                ret = 0;
                break;
            }
        }
    }
    else
    {
        R_PRINT_Log("[TaurusCameraMain]: pop_buffer Mutex lock failed(%d)\r\n", osal_ret);
    }

    osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->BufMutex);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[TaurusCameraMain]: pop_buffer Mutex unlock failed(%d)\r\n", osal_ret);
        ret = -1;
    }

    return ret;
}

static int32_t push_buffer(CameraInstance_t* pinst, taurus_camera_buffer_t* pbuf)
{
    int32_t ret = 1;
    int32_t i;
    e_osal_return_t osal_ret;

    do
    {
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->BufMutex, LOC_CAMERA_TIMEOUT_MS);
    } while (OSAL_RETURN_TIME == osal_ret);

    if ((pbuf != 0) && (pbuf->address != 0) && (OSAL_RETURN_OK == osal_ret))
    {
        for (i = 0; i < LOC_NUM_BUFFER_CELL; i++)
        {
            if (pinst->Buffer[i].address == 0)
            {
                pinst->Buffer[i] = *pbuf;
                pinst->VacantBufCellCnt--;
                pinst->EmptyBufCnt++;
                ret = 0;
                break;
            }
        }
    }
    else
    {
        R_PRINT_Log("[TaurusCameraMain]: push_buffer Mutex lock failed(%d)\r\n", osal_ret);
    }

    osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->BufMutex);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[TaurusCameraMain]: push_buffer Mutex unlock failed(%d)\r\n", osal_ret);
        ret = -1;
    }

    return ret;
}

static int64_t event_thread_main(void *data)
{
    R_TAURUS_SignalId_t signal = 0;
    CameraInstance_t *pinst = (CameraInstance_t *)data;
    taurus_camera_buffer_t buf;
    e_osal_return_t osal_ret;
    ssize_t ret;
    uint32_t recvdata;
    st_osal_mq_config_t mq_config;

    pinst->StopEventThread = 0;

    /* register frame ready event */
    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusCameraMain] event_thread_main R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
    }
    mq_config.max_num_msg = LOC_TAURUS_CAMERA_MQ_NUM_OF_MSG;
    mq_config.msg_size    = LOC_TAURUS_CAMERA_MQ_MSG_SIZE;
    osal_ret = R_OSAL_MqCreate(&mq_config, LOC_TAURUS_CAMERA_MQ_ID, &pinst->FrameReadyMq);
    if (osal_ret == OSAL_RETURN_OK) {
        ret = R_CIO_IoCtl(pinst->Handle,
                          (void *)R_CIO_VIN_IOC_REGISTER_FRAME_READY_EVT,
                          (void *)&pinst->FrameReadyMq,
                          0);
        if (-1 == ret) {
            pinst->StopEventThread = 1;
            R_PRINT_Log("[TaurusCameraMain] event_thread_main R_CIO_IoCtl(R_CIO_VIN_IOC_REGISTER_FRAME_READY_EVT) fail(%d)\r\n", ret);
        }
        while (0 == pinst->StopEventThread)
        {
            osal_ret = R_OSAL_MqReceiveForTimePeriod(pinst->FrameReadyMq,
                                                        LOC_CAMERA_TIMEOUT_MS,
                                                        (void*)&recvdata,
                                                        LOC_TAURUS_CAMERA_MQ_MSG_SIZE);
            if ((OSAL_RETURN_OK != osal_ret) && (OSAL_RETURN_TIME != osal_ret)) {
                R_PRINT_Log("[TaurusCameraMain]: event_thread_main R_OSAL_MqReceiveForTimePeriod failed(%d)\r\n", osal_ret);
            }

            if (OSAL_RETURN_OK == osal_ret) {
                if (0 == pop_buffer(pinst, &buf)) {
                    ret = R_CIO_IoCtl(pinst->Handle, (void *)R_CIO_VIN_IOC_GET_VALID_FRAME, &buf.address, 0);
                    if (-1 == ret) {
                        R_PRINT_Log("[TaurusCameraMain] event_thread_main R_CIO_IoCtl(R_CIO_VIN_IOC_GET_VALID_FRAME) fail(%d)\r\n", ret);
                    } else {
                        signal = TAURUS_CAMERA_EVT_FRAME_READY_VAL(pinst->Channel,
                                                                   pinst->EmptyBufCnt,
                                                                   pinst->VacantBufCellCnt,
                                                                   buf.index);
                    }
                } else {
                    signal = TAURUS_CAMERA_EVT_FEED_ME_VAL(pinst->Channel, pinst->VacantBufCellCnt);
                }
                locSendSignal(1, signal, NULL);
            }
        }
    } else {
        R_PRINT_Log("[TaurusCameraMain]: event_thread_main R_OSAL_MqCreate failed(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                    osal_ret, mq_config.max_num_msg, mq_config.msg_size, LOC_TAURUS_CAMERA_MQ_ID);
    }

    return 0;
}

static int32_t create_event_thread(CameraInstance_t *pInstance)
{
    int32_t ret = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    st_osal_thread_config_t config;

    config.func        = (p_osal_thread_func_t)event_thread_main;
    config.userarg     = pInstance;
    osal_ret = R_OSAL_ThreadCreate(&config, LOC_TAURUS_CAMERA_THREAD_ID, &pInstance->CameraThreadHandle);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusCameraMain]: create_event_thread R_OSAL_ThreadCreate failed(%d)\r\n", osal_ret);
        ret = -1;
    }

    return ret;
}

static int32_t ioc_get_info(struct taurus_camera_ioc_get_info_in *in, struct taurus_camera_ioc_get_info_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;

    out->cookie = in->cookie;

    out->channel_num = LOC_NUM_CAMERA;

    out->res = ret;
    return ret;
}

static int32_t ioc_channel_init(struct taurus_camera_ioc_channel_init_in *in,
                                struct taurus_camera_ioc_channel_init_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;
    int32_t locret = 0;
    CameraInstance_t *pinst;
    taurus_camera_buffer_t temp_buf;
    e_osal_return_t osal_ret;
    int cio_ret = 0;

    out->cookie = in->cookie;

    if (in->channel >= LOC_NUM_CAMERA)
    {
        R_PRINT_Log("[TaurusCameraMain]: ioc_channel_init channel(%d) failed\r\n", in->channel);
        ret = TAURUS_CAMERA_RES_ERR_PARA;
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst = &s_CameraInstance[in->channel];
        if (pinst->Initialized)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_init already initialized\r\n");
            ret = TAURUS_CAMERA_RES_ERR_REINIT;
        }
    }

    /* open cio vin channel */
    if ((TAURUS_CAMERA_RES_OK == ret) && (pinst->Handle <= 0))
    {
        pinst->Handle = R_CIO_Open(pinst->CioName, "rwb");
        if (pinst->Handle <= 0)
        {
            R_PRINT_Log("[TaurusCameraMain]: R_CIO_Open failed\r\n");
            ret = TAURUS_CAMERA_RES_ERR_CIO;
            pinst->Handle = 0;
        }
    }

    /* init mutex */
    if (TAURUS_CAMERA_RES_OK == ret)
    {
        osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_TAURUS_CAMERA_MUTEX_ID, &pinst->BufMutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[TaurusCameraMain]: channel_init Mutex create failed(%d)\r\n", osal_ret);
            ret = TAURUS_CAMERA_RES_ERR_MUTEX;
        }
    }

    /* try to config or get configured vin channel */
    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst->Config.FrameBuffer[0] = (uintptr_t)in->buffer[0].address;
        pinst->Config.FrameBuffer[1] = (uintptr_t)in->buffer[1].address;
        pinst->Config.FrameBuffer[2] = (uintptr_t)in->buffer[2].address;
        cio_ret = R_CIO_IoCtl(pinst->Handle, (void *)R_CIO_VIN_IOC_CONFIG, (void *)&pinst->Config, 0);
        if (R_CIO_VIN_ERR_IOC_CHN_CONFIGED == cio_ret)
        {
            locret  = push_buffer(pinst, &in->buffer[0]);
            locret |= push_buffer(pinst, &in->buffer[1]);
            locret |= push_buffer(pinst, &in->buffer[2]);
            if (0 == locret)
            {
                cio_ret = R_CIO_IoCtl(pinst->Handle, (void *)R_CIO_VIN_IOC_GET_CONFIG, (void *)&pinst->Config, 0);
                if (R_CIO_VIN_ERR_OK != cio_ret)
                {
                    R_PRINT_Log("[TaurusCameraMain]: R_CIO_IoCtl(R_CIO_VIN_IOC_GET_CONFIG) failed(%d)\r\n", cio_ret);
                    ret = TAURUS_CAMERA_RES_ERR_CIO;
                }
            }
            else
            {
                R_PRINT_Log("[TaurusCameraMain]: push_buffer failed(%d)\r\n", locret);
                ret = TAURUS_CAMERA_RES_ERR_PARA;
            }
        }
        else
        {
            if (R_CIO_VIN_ERR_OK != cio_ret)
            {
                R_PRINT_Log("[TaurusCameraMain]: R_CIO_IoCtl(R_CIO_VIN_IOC_CONFIG) failed(%d)\r\n", cio_ret);
                ret = TAURUS_CAMERA_RES_ERR_CIO;
                cio_ret = R_CIO_Close(pinst->Handle);
                if (0 != cio_ret)
                {
                    R_PRINT_Log("[TaurusCameraMain]: R_CIO_Close failed(%d)\r\n", cio_ret);
                }
                pinst->Handle = 0;
            }
        }
    }

    /* create a thread to handle frame ready event */
    if (TAURUS_CAMERA_RES_OK == ret)
    {
        locret = create_event_thread(pinst);
        if (0 != locret)
        {
            locret = 0;

            R_PRINT_Log("[TaurusCameraMain]: create_event_thread failed(%d)\r\n", locret);
            cio_ret = R_CIO_Close(pinst->Handle);
            if (R_CIO_VIN_ERR_OK != cio_ret)
            {
                R_PRINT_Log("[TaurusCameraMain]: R_CIO_Close failed(%d)\r\n", cio_ret);
            }
            pinst->Handle = 0;
            locret  = pop_buffer(pinst, &temp_buf);
            locret |= pop_buffer(pinst, &temp_buf);
            locret |= pop_buffer(pinst, &temp_buf);
            if (0 != locret)
            {
                R_PRINT_Log("[TaurusCameraMain]: pop_buffer failed(%d)\r\n", locret);
            }
            ret = TAURUS_CAMERA_RES_ERR_THREAD;
        }
        else
        {
            pinst->Initialized = 1;
            out->channel_info.vacant_buf_cell_cnt = pinst->VacantBufCellCnt;
            out->channel_info.width = pinst->Config.ImageOutWidth;
            out->channel_info.height = pinst->Config.ImageOutHeight;
        }
    }

    out->res = ret;
    return ret;
}

static int32_t ioc_channel_start(struct taurus_camera_ioc_channel_start_in *in,
                                 struct taurus_camera_ioc_channel_start_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;
    CameraInstance_t *pinst;

    out->cookie = in->cookie;

    if (in->channel >= LOC_NUM_CAMERA)
    {
        R_PRINT_Log("[TaurusCameraMain]: ioc_channel_start channel(%d) failed\r\n", in->channel);
        ret = TAURUS_CAMERA_RES_ERR_PARA;
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst = &s_CameraInstance[in->channel];
        if (!pinst->Initialized)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_start not initialized\r\n");
            ret = TAURUS_CAMERA_RES_ERR_NOINIT;
        }
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        if (R_CIO_VIN_ERR_OK != R_CIO_IoCtl(pinst->Handle, (void *)R_CIO_VIN_IOC_START_CAPTURE, 0, 0))
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_start R_CIO_IoCtl(R_CIO_VIN_IOC_START_CAPTURE) failed\r\n");
            ret = TAURUS_CAMERA_RES_ERR_CIO;
        }
    }

    out->res = ret;
    return ret;
}

static int32_t ioc_channel_stop(struct taurus_camera_ioc_channel_stop_in *in,
                                struct taurus_camera_ioc_channel_stop_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;
    CameraInstance_t *pinst;

    out->cookie = in->cookie;

    if (in->channel >= LOC_NUM_CAMERA)
    {
        R_PRINT_Log("[TaurusCameraMain]: ioc_channel_stop channel(%d) failed\r\n", in->channel);
        ret = TAURUS_CAMERA_RES_ERR_PARA;
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst = &s_CameraInstance[in->channel];
        if (!pinst->Initialized)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_stop not initialized\r\n");
            ret = TAURUS_CAMERA_RES_ERR_NOINIT;
        }
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        if (R_CIO_VIN_ERR_OK != R_CIO_IoCtl(pinst->Handle, (void *)R_CIO_VIN_IOC_STOP_CAPTURE, 0, 0))
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_stop R_CIO_IoCtl(R_CIO_VIN_IOC_STOP_CAPTURE) failed\r\n");
            ret = TAURUS_CAMERA_RES_ERR_CIO;
        }
    }

    out->res = ret;
    return ret;
}

static int32_t ioc_channel_feed_buffer(struct taurus_camera_ioc_channel_feed_buffer_in *in,
                                       struct taurus_camera_ioc_channel_feed_buffer_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;
    CameraInstance_t *pinst;
    int32_t i;

    out->cookie = in->cookie;

    if (in->channel >= LOC_NUM_CAMERA)
    {
        R_PRINT_Log("[TaurusCameraMain]: ioc_channel_feed_buffer channel(%d) failed\r\n", in->channel);
        ret = TAURUS_CAMERA_RES_ERR_PARA;
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst = &s_CameraInstance[in->channel];
        if (!pinst->Initialized)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_feed_buffer not initialized\r\n");
            ret = TAURUS_CAMERA_RES_ERR_NOINIT;
        }
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        for (i = 0; i < in->buf_cnt; i++)
        {
            if (in->buffer[i].address != 0)
            {
                if (0 == push_buffer(pinst, &in->buffer[i]))
                    out->accepted_buf_cnt++;
                else
                    break;
            }
        }
        out->empty_buf_cnt = pinst->EmptyBufCnt;
        out->empty_buf_cnt = pinst->VacantBufCellCnt;
    }

    out->res = ret;
    return ret;
}

static int32_t ioc_channel_release(struct taurus_camera_ioc_channel_release_in *in,
                                   struct taurus_camera_ioc_channel_release_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;
    CameraInstance_t *pinst;
    uint32_t i;
    taurus_camera_buffer_t temp_buf;
    e_osal_return_t osal_ret;

    out->cookie = in->cookie;

    if (in->channel >= LOC_NUM_CAMERA)
    {
        R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release channel(%d) failed\r\n", in->channel);
        ret = TAURUS_CAMERA_RES_ERR_PARA;
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst = &s_CameraInstance[in->channel];
        if (!pinst->Initialized)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release not initialized\r\n");
            ret = TAURUS_CAMERA_RES_ERR_NOINIT;
        }
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        if (R_CIO_VIN_ERR_OK != R_CIO_Close(pinst->Handle))
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release R_CIO_Close failed\r\n");
            ret = TAURUS_CAMERA_RES_ERR_CIO;
        }

        pinst->Initialized = 0;
        pinst->Handle = -1;
        pinst->StopEventThread = 1;
        for (i = 0; i < LOC_NUM_BUFFER_CELL; i++)
        {
            if (0 != pop_buffer(pinst, &temp_buf))
                break;
        }
        osal_ret = R_OSAL_ThreadJoin(pinst->CameraThreadHandle, 0);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release Thread join failed(%d)\n", osal_ret);
        }

        osal_ret = R_OSAL_MqDelete(pinst->FrameReadyMq);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release Queue delete failed(%d)\r\n", osal_ret);
            ret = TAURUS_CAMERA_RES_ERR_MUTEX;
        }
        osal_ret = R_OSAL_ThsyncMutexDestroy(pinst->BufMutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release Mutex destroy failed(%d)\r\n", osal_ret);
            ret = TAURUS_CAMERA_RES_ERR_MUTEX;
        }
    }

    out->res = ret;
    return ret;
}

static int32_t ioc_get_channel_info(struct taurus_camera_ioc_get_channel_info_in *in,
                                   struct taurus_camera_ioc_get_channel_info_out *out)
{
    int32_t ret = TAURUS_CAMERA_RES_OK;
    CameraInstance_t *pinst;
    struct r_cio_vin_Config config;
    out->cookie = in->cookie;

    if (in->channel >= LOC_NUM_CAMERA)
    {
        R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release channel(%d) failed\r\n", in->channel);
        ret = TAURUS_CAMERA_RES_ERR_PARA;
    }

    if (TAURUS_CAMERA_RES_OK == ret)
    {
        pinst = &s_CameraInstance[in->channel];
    }

    /* open cio vin channel */
    if ((TAURUS_CAMERA_RES_OK == ret) && (pinst->Handle <= 0))
    {
        pinst->Handle = R_CIO_Open(pinst->CioName, "rwb");
        if (pinst->Handle <= 0)
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release R_CIO_Open failed\r\n");
            ret = TAURUS_CAMERA_RES_ERR_CIO;
            pinst->Handle = 0;
        }
    }
    if (TAURUS_CAMERA_RES_OK == ret)
    {
        if (R_CIO_VIN_ERR_OK != R_CIO_IoCtl(pinst->Handle, (void *)R_CIO_VIN_IOC_GET_CONFIG, (void*)&config, 0))
        {
            R_PRINT_Log("[TaurusCameraMain]: ioc_channel_release R_CIO_IoCtl(R_CIO_VIN_IOC_GET_CONFIG) failed\r\n");
            /* CIO VIN could be not initialized here just return taurus default setting.  */
            config = pinst->Config;
        }
    }
    if (TAURUS_CAMERA_RES_OK == ret)
    {
        out->width = config.ImageOutWidth;
        out->height = config.ImageOutHeight;
    }
    out->res = ret;
    return ret;
}


/*******************************************************************************
  Section: Global Functions
*/
int R_Camera_DRV_Open(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return TAURUS_CAMERA_RES_OK;
}

int R_Camera_DRV_Close(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return TAURUS_CAMERA_RES_OK;
}

ssize_t R_Camera_DRV_Read(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 0;
}

ssize_t R_Camera_DRV_Write(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return 0;
}

ssize_t R_Camera_DRV_IoCtl(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    ssize_t                  ret;
    uint32_t                        ioctl_id;
    struct taurus_camera_cmd_msg    *cmd;
    struct taurus_camera_res_msg    *res;

    ioctl_id    = Cmd->Par1;
    cmd         = (struct taurus_camera_cmd_msg*)(ssize_t)Cmd->Par2;
    res         = (struct taurus_camera_res_msg*)(ssize_t)Cmd->Par3;

    switch (ioctl_id)
    {
        case CAMERA_PROTOCOL_IOC_GET_INFO:
            ret = ioc_get_info(&cmd->params.ioc_get_info, &res->params.ioc_get_info);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        case CAMERA_PROTOCOL_IOC_CHANNEL_INIT:
            ret = ioc_channel_init(&cmd->params.ioc_channel_init, &res->params.ioc_channel_init);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        case CAMERA_PROTOCOL_IOC_CHANNEL_START:
            ret = ioc_channel_start(&cmd->params.ioc_channel_start, &res->params.ioc_channel_start);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        case CAMERA_PROTOCOL_IOC_CHANNEL_STOP:
            ret = ioc_channel_stop(&cmd->params.ioc_channel_stop, &res->params.ioc_channel_stop);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        case CAMERA_PROTOCOL_IOC_CHANNEL_FEED_BUFFER:
            ret = ioc_channel_feed_buffer(&cmd->params.ioc_channel_feed_buffer, &res->params.ioc_channel_feed_buffer);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        case CAMERA_PROTOCOL_IOC_CHANNEL_RELEASE:
            ret = ioc_channel_release(&cmd->params.ioc_channel_release, &res->params.ioc_channel_release);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        case CAMERA_PROTOCOL_IOC_GET_CHANNEL_INFO:
            ret = ioc_get_channel_info(&cmd->params.ioc_get_channel_info, &res->params.ioc_get_channel_info);
            if (ret == 0)
                res->hdr.Aux = sizeof(struct taurus_camera_res_msg);
            break;
        default:
            R_PRINT_Log("[TaurusCameraMain]: R_Camera_DRV_IoCtl ioctl_id(%d) failed\r\n", ioctl_id);
            ret = 1;
            break;
    }

    return ret;
}

ssize_t R_Camera_DRV_Status(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    return TAURUS_CAMERA_RES_OK;
}

void R_Camera_DRV_SetSendSignalImpl(int (*SendSignalImpl)(int Guest, R_TAURUS_SignalId_t Signal, void* Option))
{
    locSendSignal = SendSignalImpl;
}
