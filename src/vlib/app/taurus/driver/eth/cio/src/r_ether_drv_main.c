/*************************************************************************************************************
* taurus_ether_drv_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"

#include "r_taurus_bridge.h"

#include "r_ether_drv_api.h"
#include "r_taurus_ether_protocol.h"

#include "ether_api.h"

#include "r_cio_api.h"
#include "r_cio_ether_public.h"

#include "EthIf_Cbk.h"
#include "r_print_api.h"

#define LOC_NUM_ETHER_IF          1


/*******************************************************************************
   Section: Local Defines
 */

/* Create Resource ID */
#define LOC_TAURUS_ETHER_RESOURCE_ID 0x8000U                                /* RESOURCE_ID_BSP                */
#define LOC_TAURUS_ETHER_THREAD_ID   (LOC_TAURUS_ETHER_RESOURCE_ID + 58U)    /* OSAL_THREAD_TAURUS_ETHER_EVENT */
#define LOC_TAURUS_ETHER_MQ_ID       (LOC_TAURUS_ETHER_RESOURCE_ID + 41U)     /* OSAL_MQ_TAURUS_PARAETHER       */
/* timeout (ms) */
#define LOC_ETHER_TIMEOUT_MS 1000 /* 1000 milisecond */

/* Mq definitions */
#define LOC_TAURUS_ETHER_MQ_NUM_OF_MSG    18
#define LOC_TAURUS_ETHER_MQ_MSG_SIZE      14

/*******************************************************************************
   Section: Local Types
 */

struct EtherInstance {
    char CioName[64];
    int Id;
    int Handle;
    int Initialized;
    osal_thread_handle_t EtherThreadHandle;
    int EventThreadShouldStop;
    osal_mq_handle_t    MqHandle;
};

/*******************************************************************************
   Section: Local Variables
 */


static int (*locSendSignal)(int Guest, R_TAURUS_SignalId_t Signal, void* Option) = 0;


static struct EtherInstance loc_EtherInstances[LOC_NUM_ETHER_IF] = {
    {
        .CioName = "RCar-Ether:0",
        .Id = 0,
        .Handle = -1,
        .Initialized = 0,
    },
};

static struct EtherInstance *etherInstance;

static int loc_ioctl_set_mode(struct taurus_ether_set_controller_mode_in *in,
                              struct taurus_ether_set_controller_mode_out *out)
{
    int ret = 0;
    r_cio_ether_mode ether_mode;
    ether_mode.CtrlIdx  = in->CtrlIdx;
    ether_mode.CtrlMode = in->CtrlMode;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_IOC_SET_MODE, (void *)&ether_mode, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_IOC_SET_MODE) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}
static int loc_ioctl_get_mode(struct taurus_ether_get_controller_mode_in *in,
                              struct taurus_ether_get_controller_mode_out *out)
{
    int ret = 0;
    r_cio_ether_mode ether_mode;

    ether_mode.CtrlIdx  = in->CtrlIdx;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_IOC_GET_MODE, (void *)&ether_mode, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_IOC_GET_MODE) fail(%d)\r\n", ret);
    } else {
        out->CtrlMode = ether_mode.CtrlMode;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_write_mii(struct taurus_ether_write_mii_in  *in,
                               struct taurus_ether_write_mii_out *out)
{
    int ret = 0;
    r_cio_ether_mii ether_mii;

    ether_mii.CtrlIdx = in->CtrlIdx;
    ether_mii.TrcvIdx = in->TrcvIdx;
    ether_mii.RegIdx  = in->RegIdx;
    ether_mii.RegVal  = in->RegVal;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&ether_mii, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_read_mii(struct taurus_ether_read_mii_in  *in,
                              struct taurus_ether_read_mii_out *out)
{
    int ret = 0;
    r_cio_ether_mii ether_mii;

    ether_mii.CtrlIdx = in->CtrlIdx;
    ether_mii.TrcvIdx = in->TrcvIdx;
    ether_mii.RegIdx  = in->RegIdx;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_IOC_READ_MII, (void *)&ether_mii, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_IOC_READ_MII) fail(%d)\r\n", ret);
    } else {
        out->RegVal  = ether_mii.RegVal;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_provide_tx_buff(struct taurus_ether_provide_tx_buffer_in  *in,
                                     struct taurus_ether_provide_tx_buffer_out *out)
{
    int ret = 0;
    r_cio_ether_tx_buff ether_tx_buff;
    Eth_BufIdxType BufIdx;
    uint16 LenByte = in->LenByte;
    Eth_DataType* LpEthBufPtr;

    ether_tx_buff.CtrlIdx     = in->CtrlIdx;
    ether_tx_buff.BufIdxPtr   = &BufIdx;
    ether_tx_buff.BufPtr      = &LpEthBufPtr;
    ether_tx_buff.LenBytePtr  = &LenByte;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_PROVIDE_TX_BUFF, (void *)&ether_tx_buff, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_PROVIDE_TX_BUFF) fail(%d)\r\n", ret);
    } else {
        out->BufIdx  = BufIdx;
        out->LenByte = LenByte;
        out->BufAddr = (uint32_t)LpEthBufPtr;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_tx_confirmation(struct taurus_ether_tx_confirmation_in *in,
                                     struct taurus_ether_tx_confirmation_out *out)
{
    int ret = 0;

    uint8 ctrlIdx = in->CtrlIdx;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_TX_CONFIRMATION, &ctrlIdx, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_TX_CONFIRMATION) fail(%d)\r\n", ret);
    } else {
        out->TxConfirmed = TxConfirmed;
    }
    out->res = ret;
    return ret;
}
static int loc_ioctl_get_version_info(struct taurus_ether_get_version_info_in  *in,
                                      struct taurus_ether_get_version_info_out *out)
{
    int ret = 0;
    r_cio_ether_version_info ether_version_info;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_GET_VERSION_INFO, (void *)&ether_version_info, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_GET_VERSION_INFO) fail(%d)\r\n", ret);
    } else {
        out->vendorID = ether_version_info.versionInfo.vendorID;
        out->moduleID = ether_version_info.versionInfo.moduleID;
        out->sw_major_version = ether_version_info.versionInfo.sw_major_version;
        out->sw_minor_version = ether_version_info.versionInfo.sw_minor_version;
        out->sw_patch_version = ether_version_info.versionInfo.sw_patch_version;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_get_phys_addr(struct taurus_ether_get_phys_addr_in  *in,
                                   struct taurus_ether_get_phys_addr_out *out)
{
    int ret = 0;
    int i = 0;
    r_cio_ether_phys_addr ether_phys_addr;

    ether_phys_addr.CtrlIdx = in->CtrlIdx;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_GET_PHYS_ADDR, &ether_phys_addr, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_GET_PHYS_ADDR) fail(%d)\r\n", ret);
    } else {
        for(i = 0; i < ETH_MACADDR_SIZE;i++)
        {
            out->PhysAddr[i] = ether_phys_addr.PhysAddrPtr[i];
        }
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_set_phys_addr(struct taurus_ether_set_phys_addr_in  *in,
                                   struct taurus_ether_set_phys_addr_out *out)
{
    int ret = 0;
    int i = 0;
    r_cio_ether_phys_addr ether_phys_addr;
    uint8 locPhysAddr[ETH_HEADER_SIZE];
    ether_phys_addr.CtrlIdx     = in->CtrlIdx;
    ether_phys_addr.PhysAddrPtr = NULL;
    for(i = 0;i < ETH_HEADER_SIZE ;i++)
    {
        locPhysAddr[i] = (uint8)in->PhysAddr[i];
    }
    ether_phys_addr.PhysAddrPtr = locPhysAddr;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_SET_PHYS_ADDR, (void *)&ether_phys_addr, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_SET_PHYS_ADDR) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_update_phys_addr_filter(struct taurus_ether_update_phys_addr_filter_in  *in,
                                             struct taurus_ether_update_phys_addr_filter_out *out)
{
    int ret = 0;
    int i = 0;
    r_cio_ether_phys_addr_filter ether_phys_addr_filter;

    ether_phys_addr_filter.CtrlIdx     = in->CtrlIdx;
    ether_phys_addr_filter.PhysAddrPtr = in->PhysAddr;
    ether_phys_addr_filter.Action      = in->Action;

    for(i = 0;i < ETH_MACADDR_SIZE ;i++)
    {
        ether_phys_addr_filter.PhysAddrPtr[i] = in->PhysAddr[i];
    }
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_UPDATE_PHYS_ADDR_FILTER, (void *)&ether_phys_addr_filter , 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_UPDATE_PHYS_ADDR_FILTER) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_get_current_time(struct taurus_ether_get_current_time_in *in,
                                      struct taurus_ether_get_current_time_out *out)
{
    int ret = 0;
    r_cio_ether_get_egress_time_stamp ether_current_time;
    Eth_TimeStampQualType locTimeQual;
    Eth_TimeStampType locTimeStamp;

    ether_current_time.CtrlIdx = in->CtrlIdx;
    ether_current_time.timeQualPtr  = &locTimeQual;
    ether_current_time.timeStampPtr = &locTimeStamp;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_GET_CURRENT_TIME, (void *)&ether_current_time, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_GET_CURRENT_TIME) fail(%d)\r\n", ret);
    } else {
        out->timeQual      = locTimeQual;
        out->nanoseconds   = locTimeStamp.nanoseconds;
        out->seconds       = locTimeStamp.seconds;
        out->secondsHi     = locTimeStamp.secondsHi;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_enable_egress_time_stamp(struct taurus_ether_enable_egress_time_stamp_in *in,
                                              struct taurus_ether_enable_egress_time_stamp_out *out)
{
    int ret = 0;

    r_cio_ether_get_egress_time_stamp ether_enable_egress_time_stamp;

    ether_enable_egress_time_stamp.CtrlIdx = in->CtrlIdx;
    ether_enable_egress_time_stamp.BufIdx  = in->BufIdx;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_ENABLE_EGRESS_TIME_STAMP, (void *)&ether_enable_egress_time_stamp, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_ENABLE_EGRESS_TIME_STAMP) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_get_egress_time_stamp(struct taurus_ether_get_egress_time_stamp_in  *in,
                                           struct taurus_ether_get_egress_time_stamp_out *out)
{
    int ret = 0;
    Eth_TimeStampQualType locTimeQual;
    Eth_TimeStampType     locTimeStamp;
    r_cio_ether_get_egress_time_stamp ether_get_egress_time_stamp;
    ether_get_egress_time_stamp.CtrlIdx = in->CtrlIdx;
    ether_get_egress_time_stamp.BufIdx = in->BufIdx;
    ether_get_egress_time_stamp.timeQualPtr  = &locTimeQual;
    ether_get_egress_time_stamp.timeStampPtr = &locTimeStamp;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_GET_EGRESS_TIME_STAMP, (void *)&ether_get_egress_time_stamp, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_GET_INGRESS_TIME_STAMP) fail(%d)\r\n", ret);
    } else {
        out->timeQual      = locTimeQual;
        out->nanoseconds   = locTimeStamp.nanoseconds;
        out->seconds       = locTimeStamp.seconds;
        out->secondsHi     = locTimeStamp.secondsHi;
    }
    out->res = ret;
    return ret;
}
static int loc_ioctl_get_ingress_time_stamp(struct taurus_ether_get_ingress_time_stamp_in *in,
                                            struct taurus_ether_get_ingress_time_stamp_out *out)
{
    int ret = 0;

    Eth_TimeStampQualType locTimeQual;
    Eth_TimeStampType locTimeStamp;
    r_cio_ether_get_ingress_time_stamp ether_get_ingress_time_stamp;

    ether_get_ingress_time_stamp.CtrlIdx = in->CtrlIdx;
    ether_get_ingress_time_stamp.DataPtr = in->DataAddr;
    ether_get_ingress_time_stamp.timeQualPtr  = &locTimeQual;
    ether_get_ingress_time_stamp.timeStampPtr = &locTimeStamp;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_GET_INGRESS_TIME_STAMP, (void *)&ether_get_ingress_time_stamp, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_GET_INGRESS_TIME_STAMP) fail(%d)\r\n", ret);
    } else {
        out->timeQual      = locTimeQual;
        out->nanoseconds   = locTimeStamp.nanoseconds;
        out->seconds       = locTimeStamp.seconds;
        out->secondsHi     = locTimeStamp.secondsHi;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_set_global_time(struct taurus_ether_set_global_time_in *in,
                                     struct taurus_ether_set_global_time_out *out)
{
    int ret = 0;

    r_cio_ether_set_global_time ether_set_global_time;

    Eth_TimeStampType eth_time_stamp;

    eth_time_stamp.nanoseconds = in->nanoseconds;
    eth_time_stamp.seconds     = in->seconds;
    eth_time_stamp.secondsHi   = in->secondsHi;

    ether_set_global_time.CtrlIdx               = in->CtrlIdx;
    ether_set_global_time.timeStampPtr          = &eth_time_stamp;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_SET_GLOBAL_TIME, (void *)&ether_set_global_time, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_SET_GLOBAL_TIME) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}
static int loc_ioctl_enable_set_correction_time(struct taurus_ether_set_correction_time_in *in,
                                                struct taurus_ether_set_correction_time_out *out)
{
    int ret = 0;
    r_cio_ether_set_correction_time ether_set_correction_time;

    Eth_TimeIntDiffType eth_timeOffset;
    Eth_RateRatioType eth_rate_ratio;

    ether_set_correction_time.CtrlIdx = in->CtrlIdx;

    eth_timeOffset.diff.nanoseconds = in->diff_nanoseconds;
    eth_timeOffset.diff.seconds     = in->diff_seconds;
    eth_timeOffset.diff.secondsHi   = in->diff_secondsHi;

    eth_rate_ratio.IngressTimeStampDelta.diff.nanoseconds = in->InTs_nanosecounds;
    eth_rate_ratio.IngressTimeStampDelta.diff.seconds     = in->InTs_seconds;
    eth_rate_ratio.IngressTimeStampDelta.diff.secondsHi   = in->InTs_secondsHi;
    eth_rate_ratio.OriginTimeStampDelta.diff.nanoseconds  = in->OrTs_nanoseconds;
    eth_rate_ratio.OriginTimeStampDelta.diff.seconds      = in->OrTs_seconds;
    eth_rate_ratio.OriginTimeStampDelta.diff.secondsHi    = in->OrTs_secondsHi;

    ether_set_correction_time.timeOffsetPtr = &eth_timeOffset;
    ether_set_correction_time.rateRatioPtr  = &eth_rate_ratio;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_SET_CORRECTION_TIME, (void *)&ether_set_correction_time, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_SET_CORRECTION_TIME) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_enable_get_drop_count(struct taurus_ether_get_drop_count_in  *in,
                                           struct taurus_ether_get_drop_count_out *out)
{
    int ret = 0;
    uint32 locDropCount;
    r_cio_ether_get_drop_count ether_get_drop_count;

    ether_get_drop_count.CtrlIdx       = in->CtrlIdx;
    ether_get_drop_count.CountValues   = in->CountValues;
    ether_get_drop_count.DropCount     = &locDropCount;
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_GET_DROP_COUNT, (void *)&ether_get_drop_count, 0);
    
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_GET_DROP_COUNT) fail(%d)\r\n", ret);
    } else {
        out->DropCount = locDropCount;
    }
    out->res = ret;

    return ret;
}

static int loc_ioctl_ether_receive(struct taurus_ether_receive_in  *in,
                                   struct taurus_ether_receive_out *out)
{
    int ret = 0;
    r_cio_ether_receive ether_receive;

    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_RECEIVE, (void *)&ether_receive, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_RECEIVE) fail(%d)\r\n", ret);
    } else {
        out->RxStatus = ether_receive.RxStatus;
    }
    out->res = ret;
    return ret;
}

static int loc_ioctl_ether_transmit(struct taurus_ether_transmit_in  *in,
                                    struct taurus_ether_transmit_out *out)
{
    int ret = 0;
    r_cio_ether_transmit ether_transmit;

    ether_transmit.CtrlIdx        = in->CtrlIdx;
    ether_transmit.BufIdx         = in->BufIdx;
    ether_transmit.FrameType      = in->FrameType;
    ether_transmit.TxConfirmation = in->TxConfirmation;
    ether_transmit.LenByte        = in->LenByte;
    ether_transmit.PhysAddrPtr    = in->PhysAddr;

    TxConfirmed = 0; /* Clear flag before send data */
    ret = R_CIO_IoCtl(etherInstance->Handle, (void *)R_CIO_ETHER_TRANSMIT, (void *)&ether_transmit, 0);
    if (0 != ret) {
        R_PRINT_Log("[TaurusEtherMain] R_CIO_IoCtl(R_CIO_ETHER_TRANSMIT) fail(%d)\r\n", ret);
    }
    out->res = ret;
    return ret;
}

static int64_t loc_EventThreadFunc(void *data)
{

    R_TAURUS_SignalId_t signal = 0;
    struct EtherInstance *etherInstance = (struct EtherInstance *)data;
    e_osal_return_t     osal_ret = OSAL_RETURN_FAIL;
    st_osal_mq_config_t mq_config;
    r_ether_mq_receive_t mq_receive_data;
    R_TAURUS_Eth_Data_t receive_data;
    ssize_t ret;

    switch (etherInstance->Id) {
    case 0:
        signal = ETHER_PROTOCOL_EVENT_PKT_AVAIL_CH0;
        break;
    default:
        R_PRINT_Log("[TaurusEtherMain] loc_EventThreadFunc etherInstance->Id(%d) failed\r\n", etherInstance->Id);
        goto exit;
    }

    /* The Taurus ETHER server sends a signal to the client whenever
     * some data is available. This way the client doesn't have to
     * poll and will request a read operation only when data is
     * ready. */
    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
    if(OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[TaurusEtherMain] R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
    }
    mq_config.max_num_msg = LOC_TAURUS_ETHER_MQ_NUM_OF_MSG;
    mq_config.msg_size    = LOC_TAURUS_ETHER_MQ_MSG_SIZE;
    osal_ret = R_OSAL_MqCreate(&mq_config, (osal_mq_id_t)LOC_TAURUS_ETHER_MQ_ID, &etherInstance->MqHandle);
    if (OSAL_RETURN_OK == osal_ret) {
        ret = R_CIO_IoCtl(etherInstance->Handle,
                          (void *)R_CIO_ETHER_REGISTER_RX_EVENT,
                          (void *)etherInstance->MqHandle,
                          NULL);
        if (ret == -1) {
            etherInstance->EventThreadShouldStop = 1;
            R_PRINT_Log("[TaurusEtherMain] EventThreadFunc R_CIO_IoCtl(R_CIO_ETHER_REGISTER_RX_EVENT) fail(%d)\r\n", ret);
        }
        else
        {
            while (0 == etherInstance->EventThreadShouldStop) {
                osal_ret = R_OSAL_MqReceiveForTimePeriod(etherInstance->MqHandle,
                                                         (osal_milli_sec_t)LOC_ETHER_TIMEOUT_MS,
                                                         (void*)&mq_receive_data,
                                                         LOC_TAURUS_ETHER_MQ_MSG_SIZE);
                if ((OSAL_RETURN_OK != osal_ret) && (OSAL_RETURN_TIME != osal_ret)) {
                    R_PRINT_Log("[TaurusEtherMain]: EventThreadFunc Queue receive failed(%d)\r\n", osal_ret);
                }

                if (OSAL_RETURN_OK == osal_ret) {
                    receive_data.Addr = (uint32_t)mq_receive_data.DataPtr;
                    receive_data.Size = (uint32_t)mq_receive_data.LenByte;
                    locSendSignal(1, signal, (void*)&receive_data);
                }
            }
        }
        osal_ret = R_OSAL_MqDelete(etherInstance->MqHandle);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[TaurusEtherMain]: EventThreadFunc Queue delete failed(%d)\r\n", osal_ret);
        }
    } else {
        R_PRINT_Log("[TaurusEtherMain]: EventThreadFunc Queue create failed(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                    osal_ret, mq_config.max_num_msg, mq_config.msg_size, LOC_TAURUS_ETHER_MQ_ID);
    }

exit:
    return 0;
}

static int32_t loc_CreateEventThread(struct EtherInstance *loc_etherInstance)
{
    int32_t         ret      = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    st_osal_thread_config_t config;

    config.func        = (p_osal_thread_func_t)loc_EventThreadFunc;
    config.userarg     = loc_etherInstance;
    osal_ret = R_OSAL_ThreadCreate(&config, LOC_TAURUS_ETHER_THREAD_ID, &loc_etherInstance->EtherThreadHandle);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[TaurusEtherMain]: CreateEventThread Thread create failed(%d)\r\n", osal_ret);
        ret = -1;
    }
    return ret;
}

int R_ETHER_DRV_Open(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    struct taurus_ether_cmd_msg     *cmd;
    struct taurus_ether_res_msg     *res;
    struct taurus_ether_init_in     *params_in;
    struct taurus_ether_init_out    *params_out;

    int handle;
    int ret = -1;

    cmd = (struct taurus_ether_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_ether_res_msg *)(size_t)Cmd->Par3;

    res->hdr.Aux = sizeof(struct taurus_ether_res_msg);

    params_in  = &cmd->params.eth_init;
    params_out = &res->params.open;

    params_out->cookie = params_in->cookie;

    etherInstance = &loc_EtherInstances[0];

    if (1 == etherInstance->Initialized) {
        params_out->res = 2;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Open failed(%lld)\r\n", params_out->res);
    } else {
        handle = R_CIO_Open("RCar-Ether:0", "rwb");
        if (handle <= 0) {
            params_out->res = 2;
            R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Open failed(%lld)\r\n", params_out->res);
        } else {
            etherInstance->Handle = handle;
            etherInstance->Initialized = 1;
            etherInstance->EventThreadShouldStop = 0;
            ret = (int)loc_CreateEventThread(etherInstance);
            if (0 == ret) {
                params_out->res = 0;
            } else {
                R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Open create thread failed\r\n");
            }
        }
    }
    return ret;
}

int R_ETHER_DRV_Close(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    struct taurus_ether_cmd_msg     *cmd;
    struct taurus_ether_res_msg     *res;
    struct taurus_ether_close_in    *params_in;
    struct taurus_ether_close_out   *params_out;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int ret = -1;

    cmd = (struct taurus_ether_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_ether_res_msg *)(size_t)Cmd->Par3;

    res->hdr.Aux = sizeof(struct taurus_ether_res_msg);

    params_in = &cmd->params.close;
    params_out = &res->params.close;

    params_out->cookie = params_in->cookie;

    etherInstance = &loc_EtherInstances[0];

    if (0 == etherInstance->Initialized) {
        params_out->res = 2;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Close failed(%lld)\r\n", params_out->res);
    } else {
        ret = R_CIO_Close(etherInstance->Handle);
        if (0 != ret) {
            params_out->res = 3;
            R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Close failed(%lld)\r\n", params_out->res);
        } else {
            etherInstance->Initialized = 0;
            etherInstance->Handle = -1;
            etherInstance->EventThreadShouldStop = 1;
            params_out->res = 0;

            osal_ret = R_OSAL_ThreadJoin(etherInstance->EtherThreadHandle, 0);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("[TaurusEtherMain]: Thread join failed(%d)\n", osal_ret);
            }
        }
    }
    return ret;
}


ssize_t R_ETHER_DRV_Read(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    struct taurus_ether_cmd_msg     *cmd;
    struct taurus_ether_res_msg     *res;
    struct taurus_ether_receive_in     *params_in;
    struct taurus_ether_receive_out    *params_out;

    r_cio_ether_receive ether_receive;
    ssize_t ret = -1;

    cmd = (struct taurus_ether_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_ether_res_msg *)(size_t)Cmd->Par3;

    res->hdr.Aux = sizeof(struct taurus_ether_res_msg);

    params_in  = &cmd->params.receive;
    params_out = &res->params.receive;

    params_out->cookie = params_in->cookie;

    etherInstance = &loc_EtherInstances[0];

    if (0 == etherInstance->Initialized) {
        params_out->res = 2;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Read failed(%lld)\r\n", params_out->res);
        goto exit;
    }

    ret = R_CIO_Read(etherInstance->Handle, (void *)&ether_receive, sizeof(r_cio_ether_receive));
    if (ret != sizeof(r_cio_ether_receive)) {
        params_out->res = 3;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Read failed(%lld)\r\n", params_out->res);
        ret = -1;
    } else {
        params_out->RxStatus = ether_receive.RxStatus;
        params_out->res = 0;
        ret = 0;
    }

exit:
    return ret;
}


ssize_t R_ETHER_DRV_Write(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    struct taurus_ether_cmd_msg     *cmd;
    struct taurus_ether_res_msg     *res;
    struct taurus_ether_transmit_in    *params_in;
    struct taurus_ether_transmit_out   *params_out;

    r_cio_ether_transmit ether_tx;

    int ret = -1;

    cmd = (struct taurus_ether_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_ether_res_msg *)(size_t)Cmd->Par3;

    res->hdr.Aux = sizeof(struct taurus_ether_res_msg);

    params_in  = &cmd->params.transmit;
    params_out = &res->params.transmit;

    params_out->cookie = params_in->cookie;

    ether_tx.CtrlIdx        = params_in->CtrlIdx;
    ether_tx.BufIdx         = params_in->BufIdx;
    ether_tx.FrameType      = params_in->FrameType;
    ether_tx.TxConfirmation = params_in->TxConfirmation;
    ether_tx.LenByte        = params_in->LenByte;
    ether_tx.PhysAddrPtr    = params_in->PhysAddr;

    etherInstance = &loc_EtherInstances[0];

    if (0 == etherInstance->Initialized) {
        params_out->res = 2;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Write failed(%lld)\r\n", params_out->res);
        goto exit;
    }

    ret = R_CIO_Write(etherInstance->Handle, (void *)&ether_tx, sizeof(r_cio_ether_transmit));
    if (ret != sizeof(r_cio_ether_transmit)) {
        params_out->res = 3;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Write failed(%lld)\r\n", params_out->res);
        ret = -1;
    } else {
        params_out->res = 0;
        ret = 0;
    }

exit:
    return ret;
}

ssize_t R_ETHER_DRV_IoCtl(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{
    ssize_t x;
    uint32_t ioctl_id;
    struct taurus_ether_cmd_msg     *cmd;
    struct taurus_ether_res_msg     *res;

    ioctl_id = Cmd->Par1;
    cmd = (struct taurus_ether_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_ether_res_msg *)(size_t)Cmd->Par3;

    res->hdr.Aux = sizeof(struct taurus_ether_res_msg);

    switch (ioctl_id) {
    case ETHER_PROTOCOL_IOC_SET_MODE:
        x = loc_ioctl_set_mode(&cmd->params.eth_set_mode, &res->params.set_mode);
        break;
    case ETHER_PROTOCOL_IOC_GET_MODE:
        x = loc_ioctl_get_mode(&cmd->params.eth_get_mode, &res->params.get_mode);
        break;
    case ETHER_PROTOCOL_IOC_WRITE_MII:
        x = loc_ioctl_write_mii(&cmd->params.write_mii, &res->params.write_mii);
        break;
    case ETHER_PROTOCOL_IOC_READ_MII:
        x = loc_ioctl_read_mii(&cmd->params.read_mii, &res->params.read_mii);
        break;
    case ETHER_PROTOCOL_IOC_PROVIDE_TX_BUFF:
        x = loc_ioctl_provide_tx_buff(&cmd->params.tx_buffer, &res->params.tx_buffer);
        break;
    case ETHER_PROTOCOL_IOC_TX_CONFIRMATION:
        x = loc_ioctl_tx_confirmation(&cmd->params.tx_confirmation, &res->params.tx_confirmation);
        break;
    case ETHER_PROTOCOL_IOC_GET_VERSION_INFO:
        x = loc_ioctl_get_version_info(&cmd->params.get_version_info, &res->params.get_version_info);
        break;
    case ETHER_PROTOCOL_IOC_GET_PHYS_ADDR:
        x = loc_ioctl_get_phys_addr(&cmd->params.get_phys, &res->params.get_phys);
        break;
    case ETHER_PROTOCOL_IOC_SET_PHYS_ADDR:
        x = loc_ioctl_set_phys_addr(&cmd->params.set_phys, &res->params.set_phys);
        break;
    case ETHER_PROTOCOL_IOC_UPDATE_PHYS_ADDR_FILTER:
        x = loc_ioctl_update_phys_addr_filter(&cmd->params.update_phys, &res->params.update_phys);
        break;
    case ETHER_PROTOCOL_IOC_GET_CURRENT_TIME:
        x = loc_ioctl_get_current_time(&cmd->params.get_current_time, &res->params.get_current_time);
        break;
    case ETHER_PROTOCOL_IOC_ENABLE_EGRESS_TIME_STAMP:
        x = loc_ioctl_enable_egress_time_stamp(&cmd->params.enable_egress_time_stamp, &res->params.enable_egress_time_stamp);
        break;
    case ETHER_PROTOCOL_IOC_GET_EGRESS_TIME_STAMP:
        x = loc_ioctl_get_egress_time_stamp(&cmd->params.get_egress_time_stamp, &res->params.get_egress_time_stamp);
        break;
    case ETHER_PROTOCOL_IOC_GET_INGRESS_TIME_STAMP:
        x = loc_ioctl_get_ingress_time_stamp(&cmd->params.get_ingress_time_stamp, &res->params.get_ingress_time_stamp);
        break;
    case ETHER_PROTOCOL_IOC_SET_GLOBAL_TIME:
        x = loc_ioctl_set_global_time(&cmd->params.set_global_time, &res->params.set_global_time);
        break;
    case ETHER_PROTOCOL_IOC_SET_CORRECTION_TIME:
        x = loc_ioctl_enable_set_correction_time(&cmd->params.set_correction_time, &res->params.set_correction_time);
        break;
    case ETHER_PROTOCOL_IOC_GET_DROP_COUNT:
        x = loc_ioctl_enable_get_drop_count(&cmd->params.get_drop_count, &res->params.get_drop_count);
        break;
    case ETHER_PROTOCOL_IOC_RECEIVE:
        x = loc_ioctl_ether_receive(&cmd->params.receive, &res->params.receive);
        break;
    case ETHER_PROTOCOL_IOC_TRANSMIT:
        x = loc_ioctl_ether_transmit(&cmd->params.transmit, &res->params.transmit);
        break;

    default:
        x = 1; /* fail */
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_IoCtl ioctl_id(%d) failed(%d)\r\n", ioctl_id, x);
        break;
    }

    return x;
}


ssize_t R_ETHER_DRV_Status(int Guest, R_TAURUS_CmdMsg_t *Cmd)
{

    struct taurus_ether_cmd_msg     *cmd;
    struct taurus_ether_res_msg     *res;
    ssize_t ret;

    cmd = (struct taurus_ether_cmd_msg *)(size_t)Cmd->Par2;
    res = (struct taurus_ether_res_msg *)(size_t)Cmd->Par3;

    etherInstance = &loc_EtherInstances[0];
    r_cio_ether_get_ether_stats ether_get_ether_stats;

    ether_get_ether_stats.CtrlIdx = cmd->params.get_ether_stats.CtrlIdx;
    ret = R_CIO_Status(etherInstance->Handle, &ether_get_ether_stats, 0, 0);
    if (0 != ret) {
        res->params.get_ether_stats.res = 1;
        R_PRINT_Log("[TaurusEtherMain]: R_ETHER_DRV_Status failed(%lld)\r\n", res->params.get_ether_stats.res);
    } else {
        res->params.get_ether_stats.res = 0;
        res->params.get_ether_stats.etherStats = ether_get_ether_stats.etherStats;
        res->hdr.Aux = sizeof(struct taurus_ether_res_msg);
    }
    return 0;
}

void R_ETHER_DRV_SetSendSignalImpl(int (*SendSignalImpl)(int Guest, R_TAURUS_SignalId_t Signal, void* Option))
{
    locSendSignal = SendSignalImpl;
}
