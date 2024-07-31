/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Irq.c                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains Interrupt Service Routine for  Ethernet Driver Component*/
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/* Unless otherwise agreed upon in writing between your company and           */
/* Renesas Electronics Corporation the following shall apply!                 */
/*                                                                            */
/* Warranty Disclaimer                                                        */
/*                                                                            */
/* There is no warranty of any kind whatsoever granted by Renesas. Any        */
/* warranty is expressly disclaimed and excluded by Renesas, either expressed */
/* or implied, including but not limited to those for non-infringement of     */
/* intellectual property, merchantability and/or fitness for the particular   */
/* purpose.                                                                   */
/*                                                                            */
/* Renesas shall not have any obligation to maintain, service or provide bug  */
/* fixes for the supplied Product(s) and/or the Application.                  */
/*                                                                            */
/* Each User is solely responsible for determining the appropriateness of     */
/* using the Product(s) and assumes all risks associated with its exercise    */
/* of rights under this Agreement, including, but not limited to the risks    */
/* and costs of program errors, compliance with applicable laws, damage to    */
/* or loss of data, programs or equipment, and unavailability or              */
/* interruption of operations.                                                */
/*                                                                            */
/* Limitation of Liability                                                    */
/*                                                                            */
/* In no event shall Renesas be liable to the User for any incidental,        */
/* consequential, indirect, or punitive damage (including but not limited     */
/* to lost profits) regardless of whether such liability is based on breach   */
/* of contract, tort, strict liability, breach of warranties, failure of      */
/* essential purpose or otherwise and even if advised of the possibility of   */
/* such damages. Renesas shall not be liable for any services or products     */
/* provided by third party vendors, developers or consultants identified or   */
/* referred to the User by Renesas in connection with the Product(s) and/or   */
/* the Application.                                                           */
/*                                                                            */
/*============================================================================*/
/* Environment:                                                               */
/*              Devices: R-Car Series, 3rd Generation                         */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
********************************************************************************
* V1.0.0:  08-Dec-2015  : Initial Version
* V1.0.1:  05-Aug-2016  : Remove Renesas specific number 59
*                         Implement Interrupt Controller Check code
*                         Unify version number
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  08-Dec-2016    Update Read/write verify for RFSO register.
*                         Added code to reported interrupt controller check and
*                         Payload CRC error to the external systems through
*                         RFSO external pins.
*                         Added code to target the EDC error notification to
*                         internal interrupt signal (INTC)
*                         Adding additional Interrupt handlers for testing
* V1.1.2:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.3:  22-Mar-2017  : Implemented Rx Frame Lost detection (#73275)
* V1.1.4:  17-Apr-2017  : Change name of CAT2 interrupt of Service Handler for
*                         Rx Queue 1 to ETH_AVBCH01ISR_CAT2_ISR
*                         Add timeout check for loop conditions
* V1.1.5:  16-May-2017  : Updated source code based on FuSa review
* V1.1.6:  16-May-2017  : Revised Eth_AvbCh24Isr to support Magic Packet
*                         notification
* V1.1.7:  18-Jun-2017  : Remove Payload CRC Module from R-Car V3M device
*                         Add pre-compile option
*                         ETH_INTERRUPT_CTL_CHK_ERROR_DETECT to enable/disable
*                         unintended interrupt check
* V1.1.8:  08-Sep-2017  : Replace 'STATIC' keyword by 'static' keyword
* V1.1.9:  12-Sep-2017  : Replace Eth_GaaDevice[].BaseAddr global variable by
*                         Eth_GaaBaseAddr constant
* V1.1.10: 27-Nov-2017  : Move functions: Eth_RxIrqHdlr_01, Eth_TxIrqHdlr_01
*                         from Eth.c to Eth_Irq.c
* V1.1.11: 15-Dec-2017    Remove redundant input parameters
*                         Move declaration of static function from header file
*                         to source file
*                         Replace static by STATIC
* V1.1.12: 15-Dec-2017  : Remove function Eth_GetRxQueueFullId
*                         Unify timeout handling
* V1.1.13: 22-Dec-2017  : Remove redundant void casting
* V1.1.14: 26-Dec-2017  : Use descriptor interrupt for transmission
* V1.1.15: 22-Mar-2018  : Getting next Rx descriptor is moved out of
*                         Eth_RxQueueProcess
*                         Remove redundant step in Eth_AvbCh22Isr that reset
*                         reception Queue when it is full
*                         Remove clearing action of RIS0 in
*                         Eth_ProcessRxInterrupt
*                         Revise interrupt controller check to check only
*                         enable sources
*                         Remove handling code of disable interrupt source
*                         Report run-time error for Timestamp FIFO Full error
*                         in Line B
*                         Report extended error for other AVB-DMAC Error
*                         in Line A
*                         Update unread frame checking
* V1.1.16: 05-Jun-2018  : Change "STATIC" keyword to "static".
* V1.1.17: 21-Aug-2018  : Move step that clears interrupt status to before
*                         interrupt handling in function Eth_AvbCh23Isr and
*                         Eth_ProcessTxInterrupt
*                         Update MISRA C rule violations
*                         Fix unintended bit clear issue in functions
*                         Eth_AvbCh22Isr, Eth_AvbCh23Isr and
*                         Eth_ProcessTxInterrupt
* V1.1.18: 23-Apr-2020  : Add MISRA C rule violation message Msg(4:5087)
* V1.2.0:  23-Mar-2020  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Eth_Irq.h"
#include "Eth_Dma.h"
#include "Eth_Mac.h"
#include "Eth_Ram.h"
#include "ether_api.h"
#include "RCar_FuSa.h"
#include "Eth_Common.h"
#if (ETH_RUNTIME_ERROR_DETECT == STD_ON)
/* Included for the declaration of Det_ReportRuntimeError() */
#include "Det.h"
#endif
/* Included for declaration of the function Dem_ReportErrorStatus() */
#include "Dem.h"
#include "r_print_api.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_IRQ_C_AR_RELEASE_MAJOR_VERSION \
  ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_IRQ_C_AR_RELEASE_MINOR_VERSION \
  ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_IRQ_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_IRQ_C_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION_VALUE
#define ETH_IRQ_C_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

/* Functionality related to R4.0 */
#if (ETH_AR_RELEASE_MAJOR_VERSION != \
  ETH_IRQ_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Irq.c : Mismatch in Release Major Version"
#endif
#if (ETH_AR_RELEASE_MINOR_VERSION != \
  ETH_IRQ_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Irq.c : Mismatch in Release Minor Version"
#endif
#if (ETH_AR_RELEASE_REVISION_VERSION != \
  ETH_IRQ_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Irq.c : Mismatch in Release Revision Version"
#endif

#if (ETH_SW_MAJOR_VERSION != ETH_IRQ_C_SW_MAJOR_VERSION)
#error "Eth_Irq.c : Mismatch in Software Major Version"
#endif
#if (ETH_SW_MINOR_VERSION != ETH_IRQ_C_SW_MINOR_VERSION)
#error "Eth_Irq.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
   Section: Local Defines
 */

#define ETHER_RX_MQ_ID        (0x8000U + 66U)
#define ETHER_MQ_NUM_OF_MSG    1
#define ETHER_MQ_MSG_SIZE      sizeof(uint32_t)
#define TIMEOUT_MS 1000 /* 1000 milisecond */

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
extern osal_mq_handle_t Ether_mq_handle_rx;
extern osal_mq_handle_t Ether_mq_handle_tx;
extern osal_device_handle_t Ether_device_handle;

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */

#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
FUNC(void, ETH_CODE_FAST)Eth_ProcessRxInterrupt(uint8 LucRxQueueNo);
#endif

#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
FUNC(void, ETH_CODE_FAST)Eth_ProcessTxInterrupt(uint8 LucTxQueueNo);
#endif

#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
/*******************************************************************************
**                      MISRA C Rule Violations                               **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0306) [I] Cast between a pointer to object and an       */
/*                 integral type.                                             */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access descriptor and buffer   */
/*                 of ETH driver                                              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0306)-1 and                           */
/*                 END Msg(4:0306)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-2 and                           */
/*                 END Msg(2:2814)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2971) Definite: Passing address of uninitialized        */
/*                 object '%s' to a function parameter declared as a pointer  */
/*                 to const.                                                  */
/* Rule          : MISRA-C:2004 Rule 9.1                                      */
/* Justification : This message occurs because address of LstRxFrame is pass  */
/*                 to parameter pFrame of function Eth_RxFilterProcess when   */
/*                 LstRxFrame is not initialized, but actually LstRxFrame has */
/*                 been initialized by previous calling Eth_RxQueueProcess    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2971)-3 and                           */
/*                 END Msg(4:2971)-3 tags in the code.                        */
/******************************************************************************/
/* 4. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2982) This assignment is redundant. The value of this   */
/*                 object is never used before being modified.                */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Assign the initialized value for local variables, it is    */
/*                 used to avoid the case that local variable is used before  */
/*                 setting                                                    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2982)-4 and                           */
/*                 END Msg(4:2982)-4 tags in the code.                        */
/******************************************************************************/
/* 5. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-5 and                           */
/*                 END Msg(4:5087)-5 tags in the code.                        */
/******************************************************************************/

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
/*******************************************************************************
** Function Name         : Eth_AvbCh00Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 0
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH00ISR_CAT2_ISR) || defined (ETH_AVBCH00ISR_CAT2_ISR)
ISR(ETH_AVBCH00ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_023] */
/* TRACE[ETH_DDD_INF_001] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh00Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_BECHANNEL;
  bool is_mq_full;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_BECHANNEL);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      /* Send message queue */
      osal_ret = R_OSAL_MqIsFull(Ether_mq_handle_rx, &is_mq_full);
      if(false == is_mq_full) {
        osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh00Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
        }
        else
        {
          /* ISR Disable */
          osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)0);
          if (OSAL_RETURN_OK != osal_ret)
          {
            R_PRINT_Log("[Eth_AvbCh00Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
          }
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh01Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 1
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH01ISR_CAT2_ISR) || defined (ETH_AVBCH01ISR_CAT2_ISR)
ISR(ETH_AVBCH01ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_024] */
/* TRACE[ETH_DDD_INF_002] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh01Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_NCCHANNEL;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_NCCHANNEL);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh01Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_NCCHANNEL);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh01Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh02Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 2
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH02ISR_CAT2_ISR) || defined (ETH_AVBCH02ISR_CAT2_ISR)
ISR(ETH_AVBCH02ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_025] */
/* TRACE[ETH_DDD_INF_003] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh02Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL1;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL1);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh02Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL1);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh02Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh03Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 3
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH03ISR_CAT2_ISR) || defined (ETH_AVBCH03ISR_CAT2_ISR)
ISR(ETH_AVBCH03ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_026] */
/* TRACE[ETH_DDD_INF_004] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh03Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL2;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL2);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh03Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL2);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh03Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh04Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 4
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH04ISR_CAT2_ISR) || defined (ETH_AVBCH04ISR_CAT2_ISR)
ISR(ETH_AVBCH04ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_027] */
/* TRACE[ETH_DDD_INF_005] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh04Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL3;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL3);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh04Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL3);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh04Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh05Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 5
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH05ISR_CAT2_ISR) || defined (ETH_AVBCH05ISR_CAT2_ISR)
ISR(ETH_AVBCH05ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_028] */
/* TRACE[ETH_DDD_INF_006] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh05Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL4;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL4);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh05Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL4);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh05Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh06Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 6
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH06ISR_CAT2_ISR) || defined (ETH_AVBCH06ISR_CAT2_ISR)
ISR(ETH_AVBCH06ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_029] */
/* TRACE[ETH_DDD_INF_007] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh06Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL5;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL5);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh06Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL5);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh06Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */


/*******************************************************************************
** Function Name         : Eth_AvbCh07Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 7
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH07ISR_CAT2_ISR) || defined (ETH_AVBCH07ISR_CAT2_ISR)
ISR(ETH_AVBCH07ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_030] */
/* TRACE[ETH_DDD_INF_008] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh07Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL6;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL6);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh07Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL6);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh07Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh08Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 8
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH08ISR_CAT2_ISR) || defined (ETH_AVBCH08ISR_CAT2_ISR)
ISR(ETH_AVBCH08ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_031] */
/* TRACE[ETH_DDD_INF_009] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh08Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL7;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL7);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh08Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL7);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh08Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh09Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 9
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH09ISR_CAT2_ISR) || defined (ETH_AVBCH09ISR_CAT2_ISR)
ISR(ETH_AVBCH09ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_032] */
/* TRACE[ETH_DDD_INF_010] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh09Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL8;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL8);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh09Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL8);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh09Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh10Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 10
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH10ISR_CAT2_ISR) || defined (ETH_AVBCH10ISR_CAT2_ISR)
ISR(ETH_AVBCH10ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_033] */
/* TRACE[ETH_DDD_INF_011] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh10Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL9;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL9);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh10Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL9);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh10Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh11Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 11
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH11ISR_CAT2_ISR) || defined (ETH_AVBCH11ISR_CAT2_ISR)
ISR(ETH_AVBCH11ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_034] */
/* TRACE[ETH_DDD_INF_012] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh11Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL10;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL10);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh11Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL10);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh11Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh12Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 12
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH12ISR_CAT2_ISR) || defined (ETH_AVBCH12ISR_CAT2_ISR)
ISR(ETH_AVBCH12ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_035] */
/* TRACE[ETH_DDD_INF_013] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh12Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL11;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL11);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh12Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL11);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh12Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh13Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 13
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH13ISR_CAT2_ISR) || defined (ETH_AVBCH13ISR_CAT2_ISR)
ISR(ETH_AVBCH13ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_036] */
/* TRACE[ETH_DDD_INF_014] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh13Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL12;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL12);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh13Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL12);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh13Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh14Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 14
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH14ISR_CAT2_ISR) || defined (ETH_AVBCH14ISR_CAT2_ISR)
ISR(ETH_AVBCH14ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_037] */
/* TRACE[ETH_DDD_INF_015] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh14Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL13;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL13);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh14Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL13);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh14Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh15Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 15
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH15ISR_CAT2_ISR) || defined (ETH_AVBCH15ISR_CAT2_ISR)
ISR(ETH_AVBCH15ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_038] */
/* TRACE[ETH_DDD_INF_016] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh15Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL14;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL14);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh15Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL14);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh15Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh16Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 16
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH16ISR_CAT2_ISR) || defined (ETH_AVBCH16ISR_CAT2_ISR)
ISR(ETH_AVBCH16ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_039] */
/* TRACE[ETH_DDD_INF_017] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh16Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL15;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL15);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh16Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
      else
      {
        /* ISR Disable */
        osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL15);
        if (OSAL_RETURN_OK != osal_ret)
        {
          R_PRINT_Log("[Eth_AvbCh16Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
        }
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh17Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue 17
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_ReadBitRegister32, R_OSAL_MqSendForTimePeriod
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**                         RIS0
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH17ISR_CAT2_ISR) || defined (ETH_AVBCH17ISR_CAT2_ISR)
ISR(ETH_AVBCH17ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_040] */
/* TRACE[ETH_DDD_INF_018] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh17Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulRIS0RegVal;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32_t returndata = (uint32_t)ETH_SCHANNEL16;

  LulRIS0RegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS0, ETH_SCHANNEL16);

  if (ETH_ZERO_32 != LulRIS0RegVal)
  {
    /* Inform about the event */
    if (NULL != Ether_mq_handle_rx)
    {
      osal_ret = R_OSAL_MqSendForTimePeriod(Ether_mq_handle_rx, TIMEOUT_MS, &returndata, ETHER_MQ_MSG_SIZE);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh17Isr]: R_OSAL_MqSendForTimePeriod failed(%d)\r\n", osal_ret);
      }
    }
    else
    {
      /* ISR Disable */
      osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, (uint64_t)ETH_SCHANNEL16);
      if (OSAL_RETURN_OK != osal_ret)
      {
        R_PRINT_Log("[Eth_AvbCh17Isr]: R_OSAL_InterruptDisableIsr failed(%d)\r\n", osal_ret);
      }
    }
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT, RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh18Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Tx Queue 0
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GstBufferLock
**
** Function(s) invoked   : Eth_ProcessTxInterrupt, Eth_ReadBitRegister32,
**                         Dem_ReportErrorStatus, RCar_RfsoFaultControl
**
** Registers Used        : TIS, FSO_CTL
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH18ISR_CAT2_ISR) || defined (ETH_AVBCH18ISR_CAT2_ISR)
ISR(ETH_AVBCH18ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_041] */
/* TRACE[ETH_DDD_INF_019] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh18Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulTISRegValue;

  LulTISRegValue = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_TIS,
    ETH_TIS_TDPF0_BIT);
  if (ETH_ZERO_32 != LulTISRegValue)
  {
    Eth_ProcessTxInterrupt(ETH_BECHANNEL);
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif /* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh19Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Tx Queue 1
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GstBufferLock
**
** Function(s) invoked   : Eth_ProcessTxInterrupt, Eth_ReadBitRegister32,
**                         Dem_ReportErrorStatus, RCar_RfsoFaultControl
**
** Registers Used        : TIS, FSO_CTL
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH19ISR_CAT2_ISR) || defined (ETH_AVBCH19ISR_CAT2_ISR)
ISR(ETH_AVBCH19ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_042] */
/* TRACE[ETH_DDD_INF_020] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh19Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulTISRegValue;

  LulTISRegValue = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_TIS,
    ETH_TIS_TDPF1_BIT);
  if (ETH_ZERO_32 != LulTISRegValue)
  {
    Eth_ProcessTxInterrupt(ETH_NCCHANNEL);
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif/* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh20Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Tx Queue 2
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GstBufferLock
**
** Function(s) invoked   : Eth_ProcessTxInterrupt, Eth_ReadBitRegister32,
**                         Dem_ReportErrorStatus, RCar_RfsoFaultControl
**
** Registers Used        : TIS, FSO_CTL
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH20ISR_CAT2_ISR) || defined (ETH_AVBCH20ISR_CAT2_ISR)
ISR(ETH_AVBCH20ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_043] */
/* TRACE[ETH_DDD_INF_021] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh20Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulTISRegValue;

  LulTISRegValue = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_TIS,
    ETH_TIS_TDPF2_BIT);
  if (ETH_ZERO_32 != LulTISRegValue)
  {
    Eth_ProcessTxInterrupt(ETH_SCHANNEL1);
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif/* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh21Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Tx Queue 3
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GstBufferLock
**
** Function(s) invoked   : Eth_ProcessTxInterrupt, Eth_ReadBitRegister32,
**                         Dem_ReportErrorStatus, RCar_RfsoFaultControl
**
** Registers Used        : TIS, FSO_CTL
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH21ISR_CAT2_ISR) || defined (ETH_AVBCH21ISR_CAT2_ISR)
ISR(ETH_AVBCH21ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_044] */
/* TRACE[ETH_DDD_INF_022] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh21Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulTISRegValue;

  LulTISRegValue = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_TIS,
    ETH_TIS_TDPF3_BIT);
  if (ETH_ZERO_32 != LulTISRegValue)
  {
    Eth_ProcessTxInterrupt(ETH_SCHANNEL2);
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}
#endif/* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_AvbCh22Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for
**                         Line0_DiA | Line1_A | Line2_A
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaDevice, Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_WriteRegister32,
**                         Eth_ReadBitRegister32, RCar_RfsoFaultControl,
**                         Dem_ReportErrorStatus, Eth_ReadRegister32
**
** Registers Used        : EIS, RIS2, FSO_CTL
*******************************************************************************/
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH22ISR_CAT2_ISR) || defined (ETH_AVBCH22ISR_CAT2_ISR)
ISR(ETH_AVBCH22ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_045] */
/* TRACE[ETH_DDD_INF_023] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh22Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint8, AUTOMATIC) LucQueueIdx;
  VAR(uint32, AUTOMATIC) LulTempRegVal;
  VAR(uint32, AUTOMATIC) LulRxQueueFull;
  #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
  VAR(boolean, AUTOMATIC) LblIRCCheck;

  LblIRCCheck = ETH_FALSE;
  #endif
  /***************************************************************************
  * Line 1_A                                                                 *
  ***************************************************************************/
  /* Read EIS */
  /* TRACE[ETH_DDD_ACT_045_REG002] */
  LulTempRegVal = Eth_ReadRegister32 (Eth_GaaBaseAddr[ETH_AVB0] +
    ETH_EIS);
  if (ETH_ZERO_32 != (LulTempRegVal & ETH_EIS_QEF_MASK))
  {
    /* Clear Queue Error Flag
    * Write 1 to all other status bits of register EIS to avoid
    * unintended clearing
    * Write 0 to reserved bits
    */
    /* TRACE[ETH_DDD_ACT_045_REG003] */
    Eth_WriteRegister32 (Eth_GaaBaseAddr[ETH_AVB0] + ETH_EIS,
      (~ETH_EIS_QEF_MASK) & ETH_EIS_INT_MASK);
    /* AVB-DMAC related error are detected - DEM is required */
    Dem_ReportErrorStatus(ETH_E_AVB_DMAC, DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    LblIRCCheck = ETH_TRUE;
    #endif
  }
  else
  {
    /* Interrupt source is not generated from this line */
    /* No action required */
  }
  /* Read RIS2.RFFF */
  /* TRACE[ETH_DDD_ACT_045_REG004] */
  LulTempRegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] +
    ETH_RIS2, ETH_RIS2_RFFF_BIT);
  if (ETH_ZERO_32 != LulTempRegVal)
  {
    /* Clear RIS2.RFFF (MAC Fifo Full) in any case
    * Write 1 to Receive Queue Full Interrupt Status to avoid
    * unintended clearing
    * Write 0 to reserved bits
    */
    /* TRACE[ETH_DDD_ACT_045_REG005] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS2,
      (~ETH_RIS2_RFFF_MASK) & ETH_RX_INT_MASK);
    /* Increase Rx Frames Lost Counter */
    Eth_GaaDevice[ETH_AVB0].DropPacketNo++;
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    LblIRCCheck = ETH_TRUE;
    #endif
  }
  else
  {
    /* Interrupt source is not generated from this line */
    /* No action required */
  }
  /*Read RIS2 */
  /* TRACE[ETH_DDD_ACT_045_REG006] */
  LulTempRegVal = Eth_ReadRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_RIS2);
  if ((LulTempRegVal & ETH_RX_INT_MASK) != ETH_ZERO_32)
  {
    for (LucQueueIdx = ETH_ZERO; LucQueueIdx < ETH_RXQ_MAXNUM; LucQueueIdx++)
    {
      LulRxQueueFull = (LulTempRegVal & ((uint32)(ETH_ONE_32 <<
        (uint32)LucQueueIdx)));
      if (LulRxQueueFull != ETH_ZERO_32)
      {
        /* Full Queue is found */
        Eth_GaaDevice[ETH_AVB0].DropPacketNo++;
        /* Clear RIS2 Flag corresponding to this Queue and write 1 to other
        * queues to avoid unintended clearing
        * Write 0 to Reserved bits
        */
        LulRxQueueFull = (~LulRxQueueFull) & ETH_RX_INT_MASK;
        /* Write 1 to Receive FIFO Full Interrupt Status to avoid unintended
        * clearing
        */
        LulRxQueueFull |= ETH_RIS2_RFFF_MASK;
        /* TRACE[ETH_DDD_ACT_045_REG014] */
        Eth_WriteRegister32(Eth_GaaBaseAddr[ETH_AVB0] +
          ETH_RIS2, LulRxQueueFull);
      }
      else
      {
        /* No action required */
      }
    } /*
       * End of for (LucQueueIdx = ETH_ZERO; LucQueueIdx < ETH_RXQ_MAXNUM;
       * LucQueueIdx++)
       */
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    LblIRCCheck = ETH_TRUE;
    #endif
  } /* End if ((LulTempRegVal & ETH_RX_INT_MASK) != ETH_ZERO_32) */
  else
  {
    /* Interrupt source is not generated from this line */
    /* No action required */
  }
  #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
  if (ETH_TRUE == LblIRCCheck)
  {
    /* No action required */
  }
  else
  {
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK, DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
  }
  #endif
}

/*******************************************************************************
** Function Name         : Eth_AvbCh23Isr
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for
**                         Line0_DiB | Line1_B | Line2_B
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_ReadBitRegister32, Eth_WriteRegister32
**                         RCar_RfsoFaultControl, Det_ReportRuntimeError,
**                         Dem_ReportErrorStatus
**
** Registers Used        : EIS, FSO_CTL
*******************************************************************************/
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH23ISR_CAT2_ISR) || defined (ETH_AVBCH23ISR_CAT2_ISR)
ISR(ETH_AVBCH23ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_046] */
/* TRACE[ETH_DDD_INF_024] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh23Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  VAR(uint32, AUTOMATIC) LulTempRegVal;
  /***************************************************************************
  * Line 1_B                                                                 *
  ***************************************************************************/
  /* Read EIS */
  /* TRACE[ETH_DDD_ACT_046_REG018] */
  LulTempRegVal = Eth_ReadBitRegister32(Eth_GaaBaseAddr[ETH_AVB0] +
    ETH_EIS, ETH_EIS_TFFF_BIT);

  if (ETH_ZERO_32 != LulTempRegVal)
  {
    /* Clear Time Stamp FIFO Full Error Interrupt Status bit (EIS.TFFF)
    * Write 1 to all other status bits of register EIS to avoid
    * unintended clearing
    * Write 0 to reserved bits
    */
    Eth_WriteRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_EIS,
      (~ETH_EIS_TFFF_MASK) & ETH_EIS_INT_MASK);
    /* Error Handling */
    #if (ETH_RUNTIME_ERROR_DETECT == STD_ON)
    (void)Det_ReportRuntimeError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_AVBCH23ISR_SID, ETH_E_TIMESTAMP_FIFO_FULL);
    #endif
  } /* (LblIRCCheck) */
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /* Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK , DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}

/*******************************************************************************
** Function Name         : Eth_AvbCh24Isr
**
** Service ID            : NA
**
** Description           : MAC-ISR
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Function(s) invoked   : Eth_WriteRegister32, Dem_ReportErrorStatus,
**                         RCar_RfsoFaultControl, Eth_ReadRegister32
**
** Registers Used        : ECSR, ECSIPR, FSO_CTL
*******************************************************************************/
/* Defines the CAT2 interrupt mapping */
#if defined (Os_ETH_AVBCH24ISR_CAT2_ISR) || defined (ETH_AVBCH24ISR_CAT2_ISR)
ISR(ETH_AVBCH24ISR_CAT2_ISR)
#else
/* TRACE[ETH_DDD_ACT_047] */
/* TRACE[ETH_DDD_INF_025] */
_INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh24Isr
(
    osal_device_handle_t device_handle,
    uint64_t irq_channel,
    void* irq_arg
)
#endif
{
  /*****************************************************************************
  * Line 3 - E-MAC interrupt                                                  *
  *****************************************************************************/
  VAR(uint32, AUTOMATIC) LulECSRVal;
  VAR(uint32, AUTOMATIC) LulECSIPRVal;

  /* Read ECSR */
  /* TRACE[ETH_DDD_ACT_047_REG001] */
  LulECSRVal = Eth_ReadRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_ECSR);
  /* Read ECSIPR */
  /* TRACE[ETH_DDD_ACT_047_REG003] */
  LulECSIPRVal = Eth_ReadRegister32(Eth_GaaBaseAddr[ETH_AVB0] +
    ETH_ECSIPR);

  if (((LulECSRVal & ETH_MDP_BIT_MASK) != ETH_ZERO_32) &&
    ((LulECSIPRVal & ETH_MDPIP_BIT_MASK) != ETH_ZERO_32))
  {
    /* Notify Magic Packet Detection */
    /* MISRA Violation: START Msg(2:2814)-2 */
    if (NULL_PTR != Eth_GpConfigPtr->pMagicNotification)
    /* MISRA Violation: END Msg(2:2814)-2 */
    {
      Eth_GpConfigPtr->pMagicNotification();
    }
    else
    {
      /* No action required */
    }
    /* Clear MDP status bit */
    /* TRACE[ETH_DDD_ACT_047_REG002] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[ETH_AVB0] +
      ETH_ECSR, ETH_MDP_BIT_MASK);
  }
  else
  {
    #if (ETH_INTERRUPT_CTL_CHK_ERROR_DETECT == STD_ON)
    /*Error on Interrupt Controller Check - DEM is required */
    Dem_ReportErrorStatus(ETH_E_INTERRUPT_CTL_CHECK, DEM_EVENT_STATUS_FAILED);
    #if (ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
    #endif
  }
}

/*******************************************************************************
** Function Name         : Eth_ProcessRxInterrupt
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Rx Queue Interrupt
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucRxQueueNo - Rx queue number
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaPayloadCrc,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Function(s) invoked   : Eth_RxIrqHdlr_01
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
/* TRACE[ETH_DDD_ACT_049] */
/* TRACE[ETH_DDD_INF_027] */
FUNC(void, ETH_CODE_FAST)Eth_ProcessRxInterrupt(uint8 LucRxQueueNo)
{
  /* Received from Receive Queue */
  Eth_RxIrqHdlr_01(LucRxQueueNo);
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_ProcessTxInterrupt
**
** Service ID            : NA
**
** Description           : Data Interrupt Service Handler for Tx Queue Interrupt
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucTxQueueNo - Tx queue number
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GstBufferLock
**
** Function(s) invoked   : Eth_WriteRegister32, Eth_TxIrqHdlr_01,
**
** Registers Used        : TIS
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
/* TRACE[ETH_DDD_ACT_050] */
/* TRACE[ETH_DDD_INF_028] */
FUNC(void, ETH_CODE_FAST)Eth_ProcessTxInterrupt(uint8 LucTxQueueNo)
{
  VAR(uint32, AUTOMATIC) LulTISRegValue;
  /* Clear Corresponding Transmit Descriptor Processed flag in TIS Reg.
  * Write 1 to all other interrupt flags of register TIS to avoid unintended
  * clearing
  * Write 0 to Reserved bits
  */
  LulTISRegValue = ~(ETH_BIT0_SET_32 << (ETH_TIS_TDPF_OFFSET + LucTxQueueNo));
  LulTISRegValue &= ETH_TIS_INT_MASK;
  /* TRACE[ETH_DDD_ACT_050_REG002] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[ETH_AVB0] + ETH_TIS,
    LulTISRegValue);
  /* Descriptor has been processed */
  Eth_TxIrqHdlr_01();
}
#endif /* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
/*******************************************************************************
** Function Name         : Eth_RxIrqHdlr_01
**
** Service ID            : NA
**
** Description           : This API handles frame reception interrupts
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : Qidx - Queue index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GaaDevice, Eth_GstDescTable, Eth_GaaTempTsDescr,
**                         Eth_GaaTempTimeStamp, Eth_GucRxMultiBuffFrame,
**                         Eth_GaaPayloadCrc, Eth_GaaBaseAddr,
**                         Eth_GaaBroadcastAddr, Eth_GstControllerFilterAddr,
**                         Eth_GenAddrFilterMode
**
** Functions invoked     : Eth_UFCounterGet, Eth_RxQueueProcess,
**                         Eth_RxCallEthIf, Eth_UFCounterDecrement,
**                         Eth_RxFilterProcess, Dem_ReportErrorStatus,
**                         Eth_RxDescChainUpdate, Eth_UnreadDataCheck
**
** Registers Used        : UFCVi i = 0,1,...,4, UFCDi i = 0,1,...,4,
**
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
/* TRACE[ETH_DDD_ACT_020] */
/* TRACE[ETH_DDD_INF_116] */
FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr_01 (uint8 Qidx)
{

  VAR(uint32, AUTOMATIC) LulRxUnread;
  VAR(uint32, AUTOMATIC) LulUFCVValue;
  VAR(Eth_RxSingleFrameType, AUTOMATIC) LstRxFrame;
  VAR(Std_ReturnType, AUTOMATIC) LddFilterResult;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(uint32, AUTOMATIC) LulDescAddr;

  /* MISRA Violation: START Msg(4:2982)-4 */
  LenReturnError = ETH_ERR_OK;
  /* MISRA Violation: END Msg(4:2982)-4 */
  LulRxUnread = Eth_UFCounterGet(ETH_AVB0, Qidx);
  if (ETH_TRUE == Eth_GaaDevice[ETH_AVB0].UFCOverflow[Qidx])
  {
     /*
     * Decrease UFCV by 1 when it is greater than 1 to reduce redundancy
     * interrupt invoking
     */
    if (ETH_ONE_32 < LulRxUnread)
    {
      Eth_UFCounterDecrement(ETH_AVB0, Qidx);
    }
    else
    {
      /* No action required */
    }
    LulRxUnread = Eth_UnreadDataCheck(Qidx);
    if (ETH_ZERO_32 == LulRxUnread)
    {
      /* Decrement unread frame counter by 1 */
      Eth_UFCounterDecrement(ETH_AVB0, Qidx);
      /* Clear overflow mode when UFCV = 0 and no frame in the Queue */
      LulUFCVValue = Eth_UFCounterGet(ETH_AVB0, Qidx);
      if (ETH_ZERO_32 == LulUFCVValue)
      {
        Eth_GaaDevice[ETH_AVB0].UFCOverflow[Qidx] = ETH_FALSE;
      }
      else
      {
        /* No action required */
      }
    }
    else
    {
      /* No action required */
    }
  }
  else if (LulRxUnread > ETH_ZERO_32)
  {
    /* Decrement unread frame counter by 1 */
    Eth_UFCounterDecrement(ETH_AVB0, Qidx);
    LulUFCVValue = Eth_UFCounterGet(ETH_AVB0, Qidx);
    if ((LulUFCVValue >= (uint32)(ETH_UFCV_MAX_VAL - ETH_ONE)) &&
      (ETH_FALSE == Eth_GaaDevice[ETH_AVB0].UFCOverflow[Qidx]))
    {
      /* Maybe there are unread frames that do not include in UFCV */
      Eth_GaaDevice[ETH_AVB0].UFCOverflow[Qidx] = ETH_TRUE;
    }
    else
    {
      /* No action required */
    }
  }
  else
  {
    /* No action required */
  }
  if (LulRxUnread != ETH_ZERO_32)
  {
    LulDescAddr = Eth_GaaDevice[ETH_AVB0].NextRxDesc[Qidx];
    /* MISRA Violation: START Msg(4:0306)-1 */
    LenReturnError = Eth_RxQueueProcess(ETH_AVB0, Qidx, (uint32)(&LstRxFrame),
    /* MISRA Violation: END Msg(4:0306)-1 */
      LulDescAddr);
    if (ETH_ERR_OK == LenReturnError)
    {
      /* Filter the received frame */
      /* MISRA Violation: START Msg(4:2971)-3 */
      LddFilterResult = Eth_RxFilterProcess(ETH_AVB0, &LstRxFrame, Qidx);
      /* MISRA Violation: END Msg(4:2971)-3 */

      if (E_OK == LddFilterResult)
      {
        /* Frame valid, call EthIf Callback */
        Eth_RxCallEthIf(ETH_AVB0, Qidx, &LstRxFrame);
      }
      else
      {
        /* Frame is discarded, no call of EthIf */
        /* Increase Discarded Packets */
        #if (ETH_GET_DROP_COUNT_API == STD_ON)
        Eth_GaaDevice[ETH_AVB0].DropCount.DiscardedPacketNo++;
        #endif
      }
      /* Update descriptor chain */
      Eth_RxDescChainUpdate(LulDescAddr, Qidx);
    }
    else
    {
      /* Error happened, report DEM */
      Dem_ReportErrorStatus(ETH_E_DESC_DATA, DEM_EVENT_STATUS_FAILED);
    }
  } /* (ETH_ZERO_32 != LulRxUnread) */
  else
  {
    /* No action required */
  }
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_TxIrqHdlr_01
**
** Service ID            : 0x11
**
** Description           : This API Handles frame transmission interrupts
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GstBufferLock
**
** Functions invoked     : Eth_TxBufferCheck
**
** Registers Used        : None
**
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
/* TRACE[ETH_DDD_ACT_021] */
/* TRACE[ETH_DDD_INF_117] */
FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr_01(void)
{
  /* Check the Tx buffers resource and release if data has been fetched */
  Eth_TxBufferCheck(ETH_AVB0);
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-5 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-5 */
#endif /* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
