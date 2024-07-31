/*************************************************************************************************************
* cio_ether_ioctl_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
   Section: Includes
 */
#include <string.h>
#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_ether_main.h"
#include "r_cio_ether_ioctl.h"
#include "ether_api.h"
#include "r_print_api.h"

/*******************************************************************************
   Section: Global functions
 */
extern osal_mq_handle_t Ether_mq_handle_received;

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_IoCtl
 */

ssize_t R_CIO_ETHER_PRV_IoCtl(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    ssize_t result = 0;
    r_cio_ether_Ctl_t ctl = (r_cio_ether_Ctl_t)Par1;
    switch (ctl) {

    case R_CIO_ETHER_IOC_SET_MODE: /* Eth_SetControllerMode(uint8 CtrlIdx,Eth_ModeType CtrlMode); */
    {
        r_cio_ether_mode *p_ether_mode = (r_cio_ether_mode*)Par2;
        result = (ssize_t)Eth_SetControllerMode(p_ether_mode->CtrlIdx, p_ether_mode->CtrlMode);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_SetControllerMode failed(%d).\n", result);
        }
        break;
    }
    case R_CIO_ETHER_IOC_GET_MODE: /* Eth_GetControllerMode(uint8 CtrlIdx, Eth_ModeType* CtrlModePtr); */
    {
        r_cio_ether_mode *p_ether_mode = (r_cio_ether_mode*)Par2;
        result = Eth_GetControllerMode(p_ether_mode->CtrlIdx, &(p_ether_mode->CtrlMode));
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_GetControllerMode failed(%d).\n", result);
        }
        break;
    }
    case R_CIO_ETHER_IOC_WRITE_MII: /* Eth_WriteMii(uint8 CtrlIdx, uint8 TrcvIdx, uint8 RegIdx, uint16 RegVal); */
    {
        #if (ETH_CTRL_ENABLE_MII == STD_ON)
        r_cio_ether_mii *p_ether_mii = (r_cio_ether_mii*)Par2;
        result = Eth_WriteMii(p_ether_mii->CtrlIdx, p_ether_mii->TrcvIdx, p_ether_mii->RegIdx, p_ether_mii->RegVal);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_WriteMii failed(%d).\n", result);
        }
        #endif
        break;
    }
    case R_CIO_ETHER_IOC_READ_MII: /* Eth_ReadMii(uint8 CtrlIdx, uint8 TrcvIdx, uint8 RegIdx, uint16* RegValPtr); */
    {
        #if (ETH_CTRL_ENABLE_MII == STD_ON)
        r_cio_ether_mii *p_ether_mii = (r_cio_ether_mii*)Par2;
        result = Eth_ReadMii(p_ether_mii->CtrlIdx, p_ether_mii->TrcvIdx, p_ether_mii->RegIdx, &(p_ether_mii->RegVal));
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_ReadMii failed(%d).\n", result);
        }
        #endif
        break;
    }
    case R_CIO_ETHER_PROVIDE_TX_BUFF: /* Eth_ProvideTxBuffer(uint8 CtrlIdx, uint8 Priority, Eth_BufIdxType* BufIdxPtr, uint8** BufPtr, uint16* LenBytePtr); */
    {
        r_cio_ether_tx_buff *p_ether_tx_buff = (r_cio_ether_tx_buff*)Par2;
        #if (ETH_QOS_SUPPORT == STD_ON)
        result = (ssize_t)Eth_ProvideTxBuffer(p_ether_tx_buff->CtrlIdx, p_ether_tx_buff->Priority, p_ether_tx_buff->BufIdxPtr, p_ether_tx_buff->BufPtr, p_ether_tx_buff->LenBytePtr);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_ProvideTxBuffer failed(%d).\n", result);
        }
        #else
        result = (ssize_t)Eth_ProvideTxBuffer(p_ether_tx_buff->CtrlIdx, p_ether_tx_buff->BufIdxPtr, p_ether_tx_buff->BufPtr, p_ether_tx_buff->LenBytePtr);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_ProvideTxBuffer failed(%d).\n", result);
        }
        #endif
        break;
    }
    case R_CIO_ETHER_TX_CONFIRMATION: /* Eth_TxConfirmation(uint8 CtrlIdx); */
    {
        uint8 *ctrlIdx = (uint8 *)Par2;
        Eth_TxConfirmation(*ctrlIdx);
        break;
    }
    case R_CIO_ETHER_GET_VERSION_INFO: /* Eth_GetVersionInfo(&versionInfo); */
    {
        #if (ETH_VERSION_INFO_API == STD_ON)
        r_cio_ether_version_info *p_ether_version_info = (r_cio_ether_version_info *)Par2;
        Eth_GetVersionInfo(&(p_ether_version_info->versionInfo));
        #endif
        break;
    }
    case R_CIO_ETHER_GET_PHYS_ADDR: /* Eth_GetPhysAddr(uint8 CtrlIdx, uint8* PhysAddrPtr) */
    {
        r_cio_ether_phys_addr *p_ether_phys_addr = (r_cio_ether_phys_addr*)Par2;
        Eth_GetPhysAddr(p_ether_phys_addr->CtrlIdx, p_ether_phys_addr->PhysAddrPtr);
        break;
    }
    case R_CIO_ETHER_SET_PHYS_ADDR: /* Eth_SetPhysAddr(uint8 CtrlIdx, uint8* PhysAddrPtr) */
    {
        r_cio_ether_phys_addr *p_ether_phys_addr = (r_cio_ether_phys_addr*)Par2;
        Eth_SetPhysAddr(p_ether_phys_addr->CtrlIdx, p_ether_phys_addr->PhysAddrPtr);
        break;
    }
    case R_CIO_ETHER_UPDATE_PHYS_ADDR_FILTER: /* Eth_UpdatePhysAddrFilter(uint8 CtrlIdx, const uint8* PhysAddrPtr, Eth_FilterActionType Action) */
    {
        #if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
        r_cio_ether_phys_addr_filter *p_ether_phys_addr_filter = (r_cio_ether_phys_addr_filter *)Par2;
        result = Eth_UpdatePhysAddrFilter(p_ether_phys_addr_filter->CtrlIdx, p_ether_phys_addr_filter->PhysAddrPtr, p_ether_phys_addr_filter->Action);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_UpdatePhysAddrFilter failed(%d).\n", result);
        }
        #endif
        break;
    }

    case R_CIO_ETHER_GET_CURRENT_TIME: /* Eth_GetCurrentTime(uint8 CtrlIdx, Eth_TimeStampQualType* timeQualPtr, Eth_TimeStampType* timeStampPtr) */
    {
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
        r_cio_ether_get_egress_time_stamp *p_ether_get_egress_time_stamp = (r_cio_ether_get_egress_time_stamp*)Par2;
        result = Eth_GetCurrentTime(p_ether_get_egress_time_stamp->CtrlIdx, p_ether_get_egress_time_stamp->timeQualPtr, p_ether_get_egress_time_stamp->timeStampPtr);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_GetCurrentTime failed(%d).\n", result);
        }
        #endif
        break;
    }
    case R_CIO_ETHER_ENABLE_EGRESS_TIME_STAMP: /* Eth_EnableEgressTimeStamp( uint8 CtrlIdx, uint8 BufIdx ) */
    {
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
        r_cio_enable_egress_time_stamp *p_enable_egress_time_stamp = (r_cio_enable_egress_time_stamp *)Par2;
        Eth_EnableEgressTimeStamp(p_enable_egress_time_stamp->CtrlIdx, p_enable_egress_time_stamp->BufIdx);
        #endif
        break;
    }
    case R_CIO_ETHER_GET_EGRESS_TIME_STAMP: /* Eth_GetEgressTimeStamp( uint8 CtrlIdx, uint8 BufIdx, Eth_TimeStampQualType* timeQualPtr, Eth_TimeStampType* timeStampPtr ) */
    {
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
        r_cio_ether_get_egress_time_stamp *p_ether_get_egress_time_stamp = (r_cio_ether_get_egress_time_stamp *)Par2;
        Eth_GetEgressTimeStamp(p_ether_get_egress_time_stamp->CtrlIdx, p_ether_get_egress_time_stamp->BufIdx, p_ether_get_egress_time_stamp->timeQualPtr, p_ether_get_egress_time_stamp->timeStampPtr);
        #endif
        break;
    }
    case R_CIO_ETHER_GET_INGRESS_TIME_STAMP: /* Eth_GetIngressTimeStamp( uint8 CtrlIdx, Eth_DataType* DataPtr, Eth_TimeStampQualType* timeQualPtr, Eth_TimeStampType* timeStampPtr ) */
    {
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
        r_cio_ether_get_ingress_time_stamp *p_ether_get_ingress_time_stamp = (r_cio_ether_get_ingress_time_stamp*)Par2;
        Eth_GetIngressTimeStamp(p_ether_get_ingress_time_stamp->CtrlIdx, p_ether_get_ingress_time_stamp->DataPtr, p_ether_get_ingress_time_stamp->timeQualPtr, p_ether_get_ingress_time_stamp->timeStampPtr);
        #endif
        break;
    }
    case R_CIO_ETHER_SET_GLOBAL_TIME: /* Eth_SetGlobalTime( uint8 CtrlIdx, const Eth_TimeStampType* timeStampPtr ) */
    {
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
        r_cio_ether_set_global_time *p_ether_set_global_time = (r_cio_ether_set_global_time*)Par2;
        result = Eth_SetGlobalTime(p_ether_set_global_time->CtrlIdx, p_ether_set_global_time->timeStampPtr);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_SetGlobalTime failed(%d).\n", result);
        }
        #endif
        break;
    }
    case R_CIO_ETHER_SET_CORRECTION_TIME: /* Eth_SetCorrectionTime( uint8 CtrlIdx, const Eth_TimeIntDiffType* timeOffsetPtr, const Eth_RateRatioType* rateRatioPtr ) */
    {
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
        r_cio_ether_set_correction_time *p_ether_set_correction_time = (r_cio_ether_set_correction_time *)Par2;
        Eth_SetCorrectionTime(p_ether_set_correction_time->CtrlIdx, p_ether_set_correction_time->timeOffsetPtr, p_ether_set_correction_time->rateRatioPtr);
        #endif
        break;
    }
    case R_CIO_ETHER_GET_DROP_COUNT: /* Eth_GetDropCount( uint8 CtrlIdx, uint8 CountValues, uint32* DropCount) */
    {
        #if (ETH_GET_DROP_COUNT_API == STD_ON)
        r_cio_ether_get_drop_count *p_ether_get_drop_count = (r_cio_ether_get_drop_count*)Par2;
        result = Eth_GetDropCount(p_ether_get_drop_count->CtrlIdx, p_ether_get_drop_count->CountValues, p_ether_get_drop_count->DropCount);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_GetDropCount failed(%d).\n", result);
        }
        #endif
        break;
    }
    case R_CIO_ETHER_RECEIVE: /* Eth_Receive */
    {
        #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_OFF)
        r_cio_ether_receive *p_ether_receive = (r_cio_ether_receive*)Par2;
        Eth_Receive(p_ether_receive->CtrlIdx, &(p_ether_receive->RxStatus));
        #endif
        break;
    }
    case R_CIO_ETHER_TRANSMIT: /* Eth_Transmit */
    {
        r_cio_ether_transmit *p_ether_transmit= (r_cio_ether_transmit *)Par2;
        result = Eth_Transmit(p_ether_transmit->CtrlIdx, p_ether_transmit->BufIdx, p_ether_transmit->FrameType, p_ether_transmit->TxConfirmation,  p_ether_transmit->LenByte, p_ether_transmit->PhysAddrPtr);
        if (0 != result)
        {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Eth_Transmit failed(%d).\n", result);
        }
        break;
    }
    case R_CIO_ETHER_REGISTER_RX_EVENT:
    {
        if (NULL != Par2) {
            Ether_mq_handle_received = (osal_mq_handle_t)Par2;
        } else {
            R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Par2 is NULL.\n");
            result = -1;
        }
        break;
    }
    default:
        /* unknown Event */
        R_PRINT_Log("[CioEtherIoCtl]: R_CIO_ETHER_PRV_IoCtl Unknowm Event. ctl(%d)\n", ctl);
        result = -1;
        break;
    }
    return result;
}
