/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Dma.c                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains DMAC functions implementation of Ethernet               */
/* Driver    Component.                                                       */
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
* V1.0.1:  05-Aug-2016  : Implement Write Verify Check code
*                         Modify Eth_IsQueueConfigured function
*                         Remove Renesas specific number 59
*                         Unify version number
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  08-Dec-2016  : Implement read/write verify check
*                         Added code to reported interrupt controller and
*                         Payload CRC check
*                         Implement EDC error detection
*                         Change behavior of Eth_IsQueueConfigured function
* V1.1.2:  10-Jan-2017  : Revise read verify check
*                         Revise Eth_IsQueueConfigured, Eth_SetDescChain
*                         Eth_DescConfig to cover C1
* V1.2.0:  07-Mar-2017  : Updated source code to support static memory
*                         allocation for descriptors and buffers
*                         via linker script (#68800)
* V1.2.1:  08-Mar-2017  : Updated MISRA-C checking
*                         Change H3_ES2_0 to H3_WS2_0
* V1.2.2:  22-Mar-2017  : Added Eth_ControllerAccessCheck function (#73275)
* V1.2.3:  04-Apr-2017  : Fixed flow of Eth_RxQueueProcess
*                         Removed Eth_ReconfSepFilter, Eth_IsRxFrameValid
* V1.2.4:  05-Apr-2017  : Added Eth_UnreadDataCheck, Eth_RxNormalDescDataCheck,
*                         Eth_RxExtDescDataCheck
* V1.3.0:  05-Apr-2017  : Updated Eth_RxQueueProcess to support reception of
*                         single frame multi buffers
*                         Removed Eth_RxBeProcess, Eth_RxNcProcess,
*                         Eth_RxSProcess function
*                         Added Eth_RxNormalQueueProcess, Eth_RxExtQueueProcess
*                         Added Eth_GetRxNormalDescAddr, Eth_GetRxExtDescAddr
* V1.3.1:  17-Apr-2017  : Change name of Eth_PayloadCRCAreaStartset to
*                         Eth_PayloadCRCAreaStartSet
*                         Add timeout check for loop conditions
* V1.3.2:  16-May-2017  : Updated source code based on FuSa review
*                         Added Eth_DMACReset function
*                         Added Eth_GetIncrementFromRatio and
*                         Eth_AdjustPTPIncrement functions
* V1.3.3:  17-May-2017  : Modified the check for elapsed time in
*                         Eth_GetTimeOutValue
*                         Corrected basic steps to change mode
* V1.3.4:  17-May-2017  : Revised the flow of Eth_SingleDescFrameSend,
*                         Eth_TxReqStart
* V1.3.5:  20-May-2017  : Modified functions Eth_ResetPTPTime, Eth_GetPTPTime
*                         Eth_SetPTPTime, Eth_ReadTSDescriptor
*                         Removed functions Eth_SetPTPOffset, Eth_CompareHeader
*                         Added functions Eth_AdjustPTPOffset,Eth_AdjustPTPTime
* V1.3.6:  17-Jun-2017  : Added volatile to local pointers which are used to
*                         access shared data
* V1.3.7:  18-Jun-2017  : Remove Payload CRC Module and Error Detection Circuit
*                         (EDC)for Stream Buffer from R-Car V3M device
*                         Add function Eth_TxQueueEmptyCheck
*                         Update memory mapping for functions which are related
*                         to interrupt
*                         Move function Eth_DMACReset from Eth_Dma.c to Eth.c
*                         and add function Eth_DMACResetHW
* V1.3.8:  19-Jun-2017  : Added critical section protection to register
*                         read-modify-write accesses
* V1.3.9:  22-Jun-2017  : Added the initialization of some global variables
*                         in Eth_DescConfig
*                         Removed redundant step in Eth_EnableController
* V1.3.10: 08-Sep-2017  : Replace 'STATIC' keyword by 'static' keyword
* V1.4.0:  12-Sep-2017  : Remove Eth_DMACStructConfig and
*                         Eth_EthernetAvbIfBaseAddr functions
*                         Modify Eth_RxNormalQueueProcess, Eth_RxQueueProcess
*                         Eth_RxExtQueueProcess, Eth_SingleDescFrameSend
*                         Eth_CommonConfig, Eth_DescConfig, Eth_DescUpdate,
*                         Eth_DescTsUpdate, Eth_ConfigureDMAC, Eth_OpModeChange,
*                         Eth_QBaseAddrLoad, Eth_RxSet, Eth_RxQueueSet,
*                         Eth_RxPaddingSet, Eth_RxDescChainConfig, Eth_TxSet
*                         Eth_RxDescChainUpdate, Eth_ErrIntMaskSet to remove
*                         some global variables usage
*                         Replace Eth_GaaDevice[].BaseAddr global variable by
*                         Eth_GaaBaseAddr constant
* V1.5.0:  28-Sep-2017  : Modify Eth_SingleDescFrameSend to:
*                         - Store the descriptor in order to check it later.
*                         - Wait for Tx Queue empty before triggering frame
*                           transmission.
*                         Added Eth_TxDescEmptyCheck and Eth_WaitTxQueueEmpty.
*                         Release the oldest timestamp entry in Eth_ReadTSFifo
*                         instead of Eth_EnableTSFifo.
*                         Modify Eth_ReadTSFifo to check the fill level of
*                         time-stamp FIFO before reading it.
* V1.5.1:  04-Dec-2017  : Replace the switch statements with if-elsif-else
*                         Update memory mapping for function
*                         Eth_TxQueueEmptyCheck
*                         Move functions: Eth_TxBufferCheck, Eth_RxCallEthIf,
*                         Eth_RxFilterProcess, Eth_RxCallEthIf,
*                         Eth_IsRxFrameValid, Eth_GetRxFilterMode from Eth.c to
*                         Eth_Dma.c
*                         Add #include "EthIf_Cbk.h"
*                         Update type of operating status variable from uint8
*                         to Eth_OpStatusType
* V1.5.2:  12-Dec-2017  : Added clock internal delay setting in Eth_CommonConfig
* V1.5.3:  15-Dec-2017  : Update Compiler Abstraction
*                         Remove redundant input parameters
*                         Update casting to fix MISRA C Violations
*                         Move declaration of static function from header file
*                         to source file
*                         Replace static by STATIC
* V1.5.4:  15-Dec-2017  : Unify timeout handling
*                         Remove function Eth_TxQueueEmptyCheck
*                         Add function Eth_WaitForOpModeChange
* V1.5.5:  22-Dec-2017  : Remove redundant void casting
* V1.5.6:  23-Dec-2017  : Invalidate data cache during Rx Queue process
*                         Update Eth_RxQueueProcess to return descriptor address
*                         and Rx buffer mode via pointer
*                         Remove Eth_RxDescChainUpdate in Eth_RxQueueProcess
*                         Update Eth_IsRxFrameValid to use global MAC address
* V1.5.7:  25-Dec-2017  : Correct Operating mode transition to follow HW UM
*                         Report DEM error when CDAR is NULL or out of range
* V1.5.8:  25-Dec-2017  : Remove function Eth_GetTimeOutValue
* V1.5.9:  26-Dec-2017  : Use descriptor interrupt for transmission
* V1.5.10: 26-Dec-2017  : Add more statistic Packet counters
* V1.5.11: 24-Jan-2018  : Enable Rx Signals Timing Adjuster for V3M
* V1.5.12: 07-Feb-2018  : Add branch process without
*                         ETH_AVB_EDC_ERR_NOTIFICATION for M3.
*                         Remove register read-verify check for registers which
*                         can be changed by hardware
* V1.5.13: 22-Mar-2018  : Re-factoring reception Queue and descriptor updating
*                         process
*                         Remove function Eth_GetRxExtDescAddr,
*                         Eth_WaitForOpModeChange, Eth_GetRxNormalDescAddr
*                         and Eth_GetRxDescrAddr
*                         Get descriptor address via global variable in
*                         Eth_RxQueueProcess in both polling and interrupt mode
*                         Remove Dem_ReportErrorStatus in Eth_RxExtQueueProcess
*                         and Eth_RxNormalQueueProcess
*                         Only copy the receive data up to the portion of
*                         ETH_MAX_FRAME_SIZE to Eth_GucRxMultiBuffFrame
*                         Support updating filter addresses in ACTIVE mode
*                         Remove function Eth_GetRxFilterMode
*                         Use queue specific interrupt line 0
*                         Remove source code which enable MFWE and MFUE
* V1.5.14: 26-Apr-2018    Remove input parameter Action in function
*                         Eth_WriteIntoSFPReg
*                         Remove functions Eth_RxDescChainConfig,
*                         Eth_QBaseAddrLoad
*                         Update Increment time checking in function
*                         Eth_SetIncrementPTPTime
*                         Update Eth_OpModeGet for getting ETH_RESET mode
* V1.5.15: 10-May-2018  : Add pre-processor for M3N
* V1.5.16: 18-May-2018  : Add return E_NOT_OK and report ETH_E_OPMODE to DEM
*                         when Error on mode transition happen in function
*                         Eth_DMACResetHW
* V1.5.17: 05-Jun-2018    Change "STATIC" keyword to "static".
* V1.5.18: 11-Jun-2018  : Invalidate "Payload CRC" for M3
* V1.5.19: 26-Jun-2018    Update Eth_SingleDescFrameSend to move step trigger
*                         buffer confirmation right before step trigger frame
*                         transmission
* V1.5.20: 21-Aug-2018  : Update Eth_TxBufferCheck to move buffer release from
*                         Eth_Transmit to Eth_TxConfirmation when TxConfirmation
*                         is false
*                         Remove safety mechanism EDC for EthernetAVB-IF
*                         Update MISRA C rule violations
*                         Fix unintended bit clear issue in functions
*                         Eth_TxReqStart, Eth_EnableTSFifo and Eth_ReadTSFifo
* V1.5.21: 22-Aug-2018  : Invalidate "Payload CRC" for M3N
* V1.5.22: 23-Oct-2018  : Reset egress time-stamp flag after successful
*                         transmission in function Eth_TxBufferCheck
* V1.5.23: 21-May-2019  : Set LunApsr.Bit.Tdm = ETH_ONE for R-CarV3H device
*                         since Tx internal delay mode is not supported and bit
*                         APSR.TDM should always be 1 as delayed mode
* V1.5.24: 17-Apr_2020  : Suppress unused parameter warnings in functions
*                         Add MISRA C rule violation message Msg(4:5087) and
*                         (4:3112)
* V1.6.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for prototypes for internal functions of Ethernet Component */
#include "Eth_Dma.h"
/* Included for Ethernet Component register types used within the module */
#include "Eth_Ram.h"
#include "Eth_Mac.h"
#include "Eth_DMACRegStruct.h"
#include "RCar_FuSa.h"
#include "EthIf_Cbk.h"
#include "Eth_Common.h"
#if ((ETH_WRITE_VERIFY_CHECK != WV_DISABLE) || \
  (ETH_READ_VERIFY_CHECK != RV_DISABLE))
/* Included for declaration of the function Dem_ReportErrorStatus() */
#include "Dem.h"
#endif
/* Inclusion of the Critical Section handle */
#if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
#include "SchM_Eth.h"
#endif
#if (ETH_DEV_ERROR_DETECT == STD_ON)
/* Included for the declaration of Det_ReportError() */
#include "Det.h"
#endif
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_DMA_C_AR_RELEASE_MAJOR_VERSION ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_DMA_C_AR_RELEASE_MINOR_VERSION ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_DMA_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_DMA_C_SW_MAJOR_VERSION        ETH_SW_MAJOR_VERSION_VALUE
#define ETH_DMA_C_SW_MINOR_VERSION        ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/
#if (ETH_DMA_AR_RELEASE_MAJOR_VERSION != ETH_DMA_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Dma.c : Mismatch in Release Major Version"
#endif
#if (ETH_DMA_AR_RELEASE_MINOR_VERSION != ETH_DMA_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Dma.c : Mismatch in Release Minor Version"
#endif
#if (ETH_DMA_AR_RELEASE_REVISION_VERSION != \
  ETH_DMA_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Dma.c : Mismatch in Release Revision Version"
#endif

#if (ETH_DMA_SW_MAJOR_VERSION != ETH_DMA_C_SW_MAJOR_VERSION)
#error "Eth_Dma.c : Mismatch in Software Major Version"
#endif
#if (ETH_DMA_SW_MINOR_VERSION != ETH_DMA_C_SW_MINOR_VERSION)
#error "Eth_Dma.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
extern osal_memory_buffer_handle_t Eth_GucRxBuff_handle;
extern osal_memory_buffer_handle_t Eth_GucTxBuff_handle;
/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

static FUNC(void, ETH_CODE_SLOW) Eth_SetFempty
(
  CONSTP2VAR(Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) pDescr,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection,
  uint8 LucBufIndex
);

static FUNC(void, ETH_CODE_SLOW) Eth_SetExtFempty
(
  CONSTP2VAR(Eth_ExtDataDescType, AUTOMATIC, ETH_APPL_DATA) pDescr,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection,
  uint8 LucBufIndex
);

static FUNC(void, ETH_CODE_SLOW) Eth_SetLinkFix
(
  uint32 LinkDesc,
  uint32 AddrToLink
);

static FUNC(uint32, ETH_CODE_SLOW) Eth_GetQueueAddr
(
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
);

static FUNC(void, ETH_CODE_SLOW) Eth_FillDescMemory
(
  CONSTP2VAR(Eth_LinkDescType, AUTOMATIC, ETH_APPL_DATA) pLinkDescr,
  uint32 DescAddr
);

static FUNC(boolean, ETH_CODE_SLOW) Eth_IsQueueConfigured
(
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
);

#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
static FUNC(uint32, ETH_CODE_SLOW) Eth_GetIncrementFromRatio
(
  uint32 OldIncrementVal,
  Eth_TimeStampType OriginTimeStampDelta,
  Eth_TimeStampType IngressTimeStampDelta
);

static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ResetPTPTime (uint8 CtrlIdx);

static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_WaitPTPRequestIsComplete
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  VAR(uint8, AUTOMATIC) ucBit
);

static FUNC(void, ETH_CODE_SLOW) Eth_ReadTSFromAnAddr
(
  CONSTP2CONST(uint32, AUTOMATIC, ETH_APPL_CONST) pAddr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) ptimeStampPtr,
  VAR(Eth_TimeStampQualType, AUTOMATIC) Action
);
#endif

static FUNC(void, ETH_CODE_SLOW) Eth_CommonConfig
(
  uint8 LucCtrlIdx
);

static FUNC(void, ETH_CODE_SLOW) Eth_SetDescChain
(
  uint32 LulAddr,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_DescTableBaseSet
(
  uint8 LucCtrlIdx,
  uint32 ulBaseAddr
);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxSet
(
  uint8 LucCtrlIdx
);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_TxSet
(
  uint8 LucCtrlIdx
);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_TxReqStart
(
  uint8 LucCtrlIdx,
  uint8 LulQIndex
);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_ErrIntMaskSet
(
  uint8 LucCtrlIdx
);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_DescTableSet
(
  uint8 LucCtrlIdx,
  uint32 ulBaseAddr
);

#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
static FUNC(uint32, ETH_CODE_SLOW) Eth_GetIncrementFromInputClock(void);

static FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_SetIncrementPTPTime
(
  uint8 CtrlIdx,
  uint32 LulIncrement
);
#endif

#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF) || \
  (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
static FUNC(uint8, ETH_CODE_SLOW) Eth_RxNormalDescDataCheck
(
  uint32 LulDescPtr
);
#endif

static FUNC(uint8, ETH_CODE_SLOW) Eth_RxExtDescDataCheck
(
  uint32 LulDescPtr
);

static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_WaitTxQueueEmpty
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  VAR(uint8, AUTOMATIC) LucQueueIdx
);

#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
static FUNC(boolean, ETH_CODE_FAST) Eth_IsRxFrameValid
(
  uint8 CtrlIdx,
  CONSTP2CONST(volatile Eth_RxSingleFrameType, AUTOMATIC, ETH_APPL_CONST)
    pRxFrame
);

static FUNC(void, ETH_CODE_FAST) Eth_DescTsUpdate
(
  uint32 LulDescPtr
);

#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF) || \
  (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
static FUNC(void, ETH_CODE_FAST) Eth_DescUpdate
(
  uint32 LulDescPtr
);
#endif
static FUNC(uint32, ETH_CODE_FAST) Eth_CurrentDescGet
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
);

#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF) || \
  (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
static FUNC(Eth_ErrorType, ETH_CODE_FAST) Eth_RxNormalQueueProcess
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex,
  uint32 LulRxFramePtr,
  uint32 LulDescPtr
);
#endif
static FUNC(Eth_ErrorType, ETH_CODE_FAST) Eth_RxExtQueueProcess
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex,
  uint32 LulRxFramePtr,
  uint32 LulExtDescPtr
);
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/*******************************************************************************
**                      MISRA C Rule Violations                               **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0303) [I] Cast between a pointer to volatile object     */
/*                 and an integral type.                                      */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access descriptor and register */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0303)-1 and                           */
/*                 END Msg(4:0303)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0306) [I] Cast between a pointer to object and an       */
/*                 integral type.                                             */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access descriptor and buffer   */
/*                 of ETH driver                                              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0306)-2 and                           */
/*                 END Msg(4:0306)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0310) Casting to different object pointer type.         */
/* Rule          : MISRA-C:2004 Rule 11.4                                     */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : This is used to archive better throughput when copying     */
/*                 data                                                       */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0310)-3 and                           */
/*                 END Msg(4:0310)-3 tags in the code.                        */
/******************************************************************************/
/* 4. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0312) Dangerous pointer cast results in loss of         */
/*                 volatile qualification.                                    */
/* Rule          : MISRA-C:2004 Rule 11.5                                     */
/* Justification : The variable is used as constant, there is no need         */
/*                 volatile qualification in this case                        */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0312)-4 and                           */
/*                 END Msg(4:0312)-4 tags in the code.                        */
/******************************************************************************/
/* 5. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0488) Performing pointer arithmetic.                    */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.6 Additive Operators - Constraints */
/* Justification : Increment is done on a successfully allocated  portion of  */
/*                 memory. These increments and decrements are used to easy   */
/*                 access to memory.                                          */
/*                 All these pointer operations are done/covered in condition */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0488)-5 and                           */
/*                 END Msg(4:0488)-5 tags in the code.                        */
/******************************************************************************/
/* 6. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0489) The integer value 1 is being added or subtracted  */
/*                 from a pointer.                                            */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.6 Additive Operators - Constraints */
/* Justification : Increment is done on a successfully allocated  portion of  */
/*                 memory. These increments and decrements are used to easy   */
/*                 access to memory.                                          */
/*                 All these pointer operation is done/covered in condition   */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0489)-6 and                           */
/*                 END Msg(4:0489)-6 tags in the code.                        */
/******************************************************************************/
/* 7. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0491) Array subscripting applied to an object of        */
/*                 pointer type.                                              */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.2.1 Array Subscripting             */
/*                             ISO:C90 6.2.2.1 Lvalues and Function           */
/*                                     Designators                            */
/*                             ISO:C90 6.7.1 Function Definitions - Semantics */
/* Justification : This is perfectly legitimate in C as Array and pointer are */
/*                 substantially the same thing                               */
/*                 Array subscript implementation is correct.                 */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0491)-6 and                           */
/*                 END Msg(4:0491)-6 tags in the code.                        */
/******************************************************************************/
/* 8. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0492) Array subscripting applied to a function          */
/*                 parameter declared as a pointer.                           */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.2.1 Array Subscripting             */
/*                             ISO:C90 6.2.2.1 Lvalues and Function           */
/*                                     Designators                            */
/*                             ISO:C90 6.7.1 Function Definitions - Semantics */
/* Justification : Array is used to easy store data in memory, pointer is     */
/*                 used to easy access to memory. This is perfectly legitimate*/
/*                 in C as Array and pointer are substantially the same thing.*/
/*                 Array subscript implementation is correct.                 */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0492)-8 and                           */
/*                 END Msg(4:0492)-8 tags in the code.                        */
/******************************************************************************/
/* 9. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0580) [C] Constant is too large to be representable.    */
/* Rule          : MISRA-C:2004 Rule 1.1                                      */
/*                 REFERENCE - ISO:C90-6.4 Constant Expressions - Constraints */
/* Justification : Using uint64 for time-stamp calculation                    */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0580)-9 and                           */
/*                 END Msg(4:0580)-9 tags in the code.                        */
/******************************************************************************/
/* 10. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:0759) An object of union type has been defined.         */
/* Rule          : MISRA-C:2004 Rule 18.4                                     */
/* Justification : To access only the lower byte of the converted values      */
/*                 from the conversion register.                              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0759)-10 and                          */
/*                 END Msg(4:0759)-10 tags in the code.                       */
/******************************************************************************/
/* 11. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:0771) More than one 'break' statement has been used to  */
/*                 terminate this iteration statement.                        */
/* Rule          : MISRA-C:2004 Rule 14.6                                     */
/* Justification : Use to break the loop after achieving the target and avoid */
/*                 unnecessary steps                                          */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0771)-11 and                          */
/*                 END Msg(4:0771)-11 tags in the code.                       */
/******************************************************************************/
/* 12. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2790) Constant: Right hand operand of shift operator    */
/*                 is negative or too large.                                  */
/* Rule          : MISRA-C:2004 Rule 12.8                                     */
/* Justification : To shift a uint64 variable. It is necessary to calculate   */
/*                 time stamp value                                           */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2790)-12 and                          */
/*                 END Msg(4:2790)-12 tags in the code.                       */
/******************************************************************************/
/* 13. MISRA C RULE VIOLATION:                                                */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-13 and                          */
/*                 END Msg(2:2814)-13 tags in the code.                       */
/******************************************************************************/
/* 14. MISRA C RULE VIOLATION:                                                */
/* Message       : (2:2824) Possible: Arithmetic operation on NULL pointer.   */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2824)-14 and                          */
/*                 END Msg(2:2824)-14 tags in the code.                       */
/******************************************************************************/
/* 15. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2880) This code is unreachable.                         */
/* Rule          : MISRA-C:2004 Rule 14.1                                     */
/* Justification : There are 2 cases:                                         */
/*                 + The code is only un-read when both configuration         */
/*                 parameters EthRxSQueueTimeStamp and EthRxBeQueueTimeStamp  */
/*                 are enabled. The implementation is used to reduce          */
/*                 the complexity of source code                              */
/*                 + The 'if' condition cannot be true, it make the code is   */
/*                 unreachable. But actually, the 'if' statement check HW     */
/*                 object and it can be true if HW object is updated          */
/*                 by HW as expected                                          */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2880)-15 and                          */
/*                 END Msg(4:2880)-15 tags in the code.                       */
/******************************************************************************/
/* 16. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2910) Constant: Wraparound in unsigned arithmetic       */
/*                 operation.                                                 */
/* Rule          : MISRA-C:2004 Rule 12.11                                    */
/* Justification : The violation occurs in addition or multiplication if      */
/*                 the result is too large to be represented. This addition or*/
/*                 multiplication is nescessary to calculate the time stamp   */
/*                 value. The result of calculation has been casted to uint64,*/
/*                 there is no lossing data.                                  */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2910)-16 and                          */
/*                 END Msg(4:2910)-16 tags in the code.                       */
/******************************************************************************/
/* 17. MISRA C RULE VIOLATION:                                                */
/* Message       : (2:2921) Definite: Left shift operation on expression of   */
/*                 unsigned type results in loss of high order bits.          */
/* Rule          : None                                                       */
/* Justification : The unsign 32 bits value is shift-left 32 bits but it is   */
/*                 cast to usign 64 bits, so there is no lossing value. This  */
/*                 necessary to calculate the time stamp                      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2921)-17 and                          */
/*                 END Msg(2:2921)-17 tags in the code.                       */
/******************************************************************************/
/* 18. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2982) This assignment is redundant. The value of this   */
/*                 object is never used before being modified.                */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Assign the initialized value for local variables, it is    */
/*                 used to avoid the case that local variable is used before  */
/*                 setting                                                    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2982)-18 and                          */
/*                 END Msg(4:2982)-18 tags in the code.                       */
/******************************************************************************/
/* 19. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2984) This operation is redundant. The value of the     */
/*                 result is always '%1s'.                                    */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : In specific case, the result is contant value but it uses  */
/*                 to unify with other calculation. This is easy to review    */
/*                 and maintain                                               */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2984)-19 and                          */
/*                 END Msg(4:2984)-19 tags in the code.                       */
/******************************************************************************/
/* 20. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2985) This operation is redundant. The value of the     */
/*                 result is always that of the left-hand operand.            */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : There are 2 cases:                                         */
/*                 + Access to register, descriptor: they can be changed by   */
/*                 HW, so actually the result of calculation is updated       */
/*                 + It is used to unify with other calculation. This is easy */
/*                 to review and maintain                                     */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2985)-20 and                          */
/*                 END Msg(4:2985)-20 tags in the code.                       */
/******************************************************************************/
/* 21. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2986) This operation is redundant. The value of the     */
/*                 result is always that of the right-hand operand.           */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : There are 2 cases:                                         */
/*                 + Access to register, descriptor: they can be changed by   */
/*                 HW, so actually the result of calculation is updated       */
/*                 + It is used to unify with other calculation. This is easy */
/*                 to review and maintain                                     */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2986)-21 and                          */
/*                 END Msg(4:2986)-21 tags in the code.                       */
/******************************************************************************/
/* 22. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2991) The value of this 'if' controlling expression is  */
/*                 always 'true'.                                             */
/* Rule          : MISRA-C:2004 Rule 13.7                                     */
/* Justification : The 'if' statement check HW object to assign true          */
/*                 or false, it can be false if HW object is updated by       */
/*                 HW as expected                                             */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2991)-22 and                          */
/*                 END Msg(4:2991)-22 tags in the code.                       */
/******************************************************************************/
/* 23. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2992) The value of this 'if' controlling expression is  */
/*                 always 'false'.                                            */
/* Rule          : MISRA-C:2004 Rule 13.7                                     */
/* Justification : There are 2 cases:                                         */
/*                 + The 'if' is always 'false' when both configuration       */
/*                 parameters EthRxSQueueTimeStamp and EthRxBeQueueTimeStamp  */
/*                 are enabled. The implementation is used to reduce          */
/*                 the complexity of source code                              */
/*                 + The case check HW object to assign true or false         */
/*                 the controlling expression can be true if HW object        */
/*                 is updated by HW as expected                               */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2992)-23 and                          */
/*                 END Msg(4:2992)-23 tags in the code.                       */
/******************************************************************************/
/* 24. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2995) The result of this logical operation is always    */
/*                 'true'.                                                    */
/* Rule          : MISRA-C:2004 Rule 13.7                                     */
/* Justification : The logical operation check HW object to assign true       */
/*                 or false, the controlling expression can be false          */
/*                 if HW object is updated by HW as expected                  */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2995)-24 and                          */
/*                 END Msg(4:2995)-24 tags in the code.                       */
/******************************************************************************/
/* 25. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2996) The result of this logical operation is always    */
/*                 'false'.                                                   */
/* Rule          : MISRA-C:2004 Rule 13.7                                     */
/* Justification : There are 2 cases:                                         */
/*                 + The logical operation is always 'false' when both        */
/*                 configuration parameters EthRxSQueueTimeStamp and          */
/*                 EthRxBeQueueTimeStamp are enabled. The implementation is   */
/*                 used to reduce the complexity of source code               */
/*                 + Logical operation check HW object to assign true or false*/
/*                 the logical operation can be true if HW object is          */
/*                 updated by HW as expected                                  */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2996)-25 and                          */
/*                 END Msg(4:2996)-25 tags in the code.                       */
/******************************************************************************/
/* 26. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:3415) Right hand operand of '&&' or '||' is an          */
/*                 expression with possible side effects.                     */
/* Rule          : MISRA-C:2004 Rule 12.4                                     */
/*                 REFERENCE - ISO:C90-5,1,2,3 Program Execution              */
/* Justification : To access a volatile object. The opertion of '&&' and '||' */
/*                 still correct when volatile object is changed              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:3415)-26 and                          */
/*                 END Msg(4:3415)-26 tags in the code.                       */
/******************************************************************************/
/* 27. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:4424) An expression of 'essentially enum' type (%1s)    */
/*                 is being converted to unsigned type, '%2s' on assignment.  */
/* Rule          : MISRA-C:2004 Rule 10.1                                     */
/* Justification : Assign enum to uint32 type. This assigning is              */
/*                 necessary to access register                               */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:4424)-27 and                          */
/*                 END Msg(4:4424)-27 tags in the code.                       */
/******************************************************************************/
/* 28. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:3112) This statement has no side-effect                 */
/*                  - it can be removed.                                      */
/* Rule          : MISRA-C:2004 Rule 14.2                                     */
/* Justification : The statement is added intendedly to avoid unused parameter*/
/*                 error message from compiler, it does not impact to         */
/*                 the functionality of ETH driver                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:3112)-28 and                          */
/*                 END Msg(4:3112)-28 tags in the code.                       */
/******************************************************************************/
/* 29. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-29 and                          */
/*                 END Msg(4:5087)-29 tags in the code.                       */
/******************************************************************************/

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/
void Eth_CacheInvalidate
(
  uint32 arrayAddr,
  size_t Size
)
{
  R_OSAL_CacheInvalidate(arrayAddr, Size, R_OSAL_CACHE_D);
}

void Eth_CacheFlush
(
  uint32 arrayAddr,
  size_t Size
)
{
  R_OSAL_CacheFlush(arrayAddr, Size, R_OSAL_CACHE_D);
}

#if defined(USE_ETHER_CA_ENABLE) /* Use CA only */
void Eth_CacheFlush_RxBuff
(
  uint32 arrayAddr
)
{
  e_osal_return_t osal_ret = OSAL_RETURN_OK;
  uint32 offset = 0;
  uint32 idx;

  if(((uintptr_t)Eth_GucRxBuff <= arrayAddr) && (arrayAddr < ((uintptr_t)Eth_GucRxBuff + alignment64(sizeof(uint8) * ETH_RX_BUF_SIZE_CACHE_ALIGN))))
  {
    offset = arrayAddr - (uintptr_t)Eth_GucRxBuff;
    idx = offset / ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN;
    offset = idx * ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN;
    osal_ret = R_OSAL_MmngrFlush(Eth_GucRxBuff_handle, offset, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
  }
  else
  {
    R_PRINT_Log("Out of scope of Eth_GucRxBuff, Addr(0x%08X)\n", arrayAddr);
  }
  if(osal_ret != OSAL_RETURN_OK)
  {
    R_PRINT_Log("Error in R_OSAL_MmngrFlush ret(%d), Addr(0x%08X)\n", osal_ret, arrayAddr);
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_RECEIVE_SID, ETH_E_GET_MEMORY);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  }
}
#endif

/*******************************************************************************
** Function Name         : Eth_RxNormalQueueProcess
**
** Service ID            : NA
**
** Description           : This process normal descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx    - Index of the ETH Controller
**                         LucQIndex     - Queue Index which has received
**                         LulRxFramePtr - Pointer to rx frame
**                         LulDescPtr    - Pointer to rx Descriptor
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - return type of error
**
** Preconditions         : None
**
** Global Variables      : Eth_GaaDevice, Eth_GstDescTable, Eth_GaaPayloadCrc,
**                         Eth_GucRxMultiBuffFrame, Eth_GaaTempTsDescr,
**                         Eth_GaaTempTimeStamp
**
** Functions invoked     : Eth_RamCopy, CR7_Invalidate_DCache_By_Addr,
**                         Eth_DescUpdate
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF) || \
  (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
/* TRACE[ETH_DDD_ACT_165] */
/* TRACE[ETH_DDD_INF_131] */
static FUNC(Eth_ErrorType, ETH_CODE_FAST) Eth_RxNormalQueueProcess
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex,
  uint32 LulRxFramePtr,
  uint32 LulDescPtr
)
{
  P2VAR(volatile Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) LpDataDesc;
  P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) LpMultiBuff;
  VAR(uint32, AUTOMATIC) LulNextFreeDataIdx;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint32, AUTOMATIC) LulRxQueueStart;
  VAR(uint32, AUTOMATIC) LulRxQueueEnd;
  VAR(uint32, AUTOMATIC) LulCopyDataSize;

  LulNextFreeDataIdx = ETH_ZERO_32;
  /* MISRA Violation: START Msg(4:2982)-18 */
  LulCopyDataSize = ETH_ZERO_32;
  /* MISRA Violation: END Msg(4:2982)-18 */
  LenReturnError = ETH_ERR_OK;
  LucCount = ETH_ZERO_8;

  /* Get Start Descriptor Address for the corresponding Queue */
  LulRxQueueStart = Eth_GstDescTable[ETH_RXBEQ_OFFSET + LucQIndex].Dptr;

  /* Get End Descriptor Address for the corresponding Queue */
  LulRxQueueEnd = LulRxQueueStart +
    (ETH_NORMAL_DESC_SIZE * (ETH_RX_BUF_TOTAL - ETH_ONE_8));
  /* MISRA Violation: START Msg(4:0303)-1 */
  LpDataDesc = (volatile  Eth_DataDescType *)LulDescPtr;
  /* MISRA Violation: END Msg(4:0303)-1 */

  /* MISRA Violation: START Msg(2:2814)-13 */
  if (ETH_DESC_FSINGLE == (LpDataDesc->Header).Dt)
  /* MISRA Violation: END Msg(2:2814)-13 */
  {
    /* Descriptor type correct */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameAddr = LpDataDesc->Dptr;
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength =
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
      (LpDataDesc->Header).Ds;
    /* Invalidate buffer data */
    /*CR7_Invalidate_DCache_By_Addr(LpDataDesc->Dptr,
      ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
    Eth_CacheInvalidate(LpDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
    #if (ETH_DEVICE_NAME == RCAR_H3)
    #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
    /* Check the state information (PC bit) of the received descriptor. */
    /* TRACE[ETH_DDD_ACT_165_GLB003] */
    /* Cast to unify type in calculation */
    Eth_GaaPayloadCrc[LucQIndex] =
      (uint8)((LpDataDesc->Header).Ctrl >> ETH_THREE_8) & ETH_ONE_8;
    #endif
    #endif /* (ETH_DEVICE_NAME == RCAR_H3) */
  } /* (ETH_DESC_FSINGLE == (LpDataDesc->Header).Dt) */
  else if (ETH_DESC_FSTART == (LpDataDesc->Header).Dt)
  {
    /* Single frame multi buffer */
    /* Get the multi buffer address of corresponding Queue */
    LpMultiBuff = Eth_GucRxMultiBuffFrame[LucQIndex];
    /* Assign address of the combined received frame */
    /* MISRA Violation: START Msg(4:0306)-2 */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameAddr = (uint32)(LpMultiBuff);
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* Invalidate buffer data */
    /*CR7_Invalidate_DCache_By_Addr(LpDataDesc->Dptr,
      ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
    Eth_CacheInvalidate(LpDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
    /* Get the size of buffer to be copied */
    LulCopyDataSize = (LpDataDesc->Header).Ds;
    /* MISRA Violation: START Msg(4:2986)-21 */
    if ((LulNextFreeDataIdx + LulCopyDataSize) > ETH_MAX_FRAME_SIZE)
    /* MISRA Violation: END Msg(4:2986)-21 */
    {
      /* Only copy the portion up to ETH_MAX_FRAME_SIZE */
      /* MISRA Violation: START Msg(4:2984)-19 */
      LulCopyDataSize = ETH_MAX_FRAME_SIZE - LulNextFreeDataIdx;
      /* MISRA Violation: END Msg(4:2984)-19 */
    }
    else
    {
      /* No action required */
    }
    /* Copy the START of received frame to combined frame */
    /* Cast to be compatible with type of input parameter */
    /* MISRA Violation: START Msg(4:0488)-5 */
    Eth_RamCopy((uint8 *)(LpMultiBuff + LulNextFreeDataIdx),
    /* MISRA Violation: END Msg(4:0488)-5 */
      /* MISRA Violation: START Msg(4:0306)-2 */
      (const uint8 *)LpDataDesc->Dptr, LulCopyDataSize);
      /* MISRA Violation: END Msg(4:0306)-2 */
    /* Increase to the next free buffer index */
    /* MISRA Violation: START Msg(4:2986)-21 */
    LulNextFreeDataIdx += LulCopyDataSize;
    /* MISRA Violation: END Msg(4:2986)-21 */
    /* Cumulate the size */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength = LulCopyDataSize;
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */

    /* Start cumulating the size and combining frame until the end */
    for (LucCount = ETH_ONE_8; LucCount < ETH_RX_BUF_TOTAL; LucCount++)
    {
      /* Increase to the next descriptor */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpDataDesc++;
      /* MISRA Violation: END Msg(4:0489)-6 */
      if (ETH_DESC_LINKFIX == (LpDataDesc->Header).Dt)
      {
        /* MISRA Violation: START Msg(4:0306)-2 */
        LpDataDesc = (Eth_DataDescType *)LpDataDesc->Dptr;
        /* MISRA Violation: END Msg(4:0306)-2 */
      }
      else
      {
        /* No action required */
      }
      if (ETH_DESC_FMID == (LpDataDesc->Header).Dt)
      {
        /* Invalidate buffer data */
        /*CR7_Invalidate_DCache_By_Addr(LpDataDesc->Dptr,
          ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
        Eth_CacheInvalidate(LpDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
        /* Get the size of buffer to be copied */
        LulCopyDataSize = (LpDataDesc->Header).Ds;
        if ((LulNextFreeDataIdx + LulCopyDataSize) > ETH_MAX_FRAME_SIZE)
        {
          /* Only copy the portion up to ETH_MAX_FRAME_SIZE */
          LulCopyDataSize = ETH_MAX_FRAME_SIZE - LulNextFreeDataIdx;
        }
        else
        {
          /* No action required */
        }
        /* Copy the MID of received frame to combined frame */
        /* Cast to be compatible with type of input parameter */
        /* MISRA Violation: START Msg(4:0488)-5 */
        Eth_RamCopy((uint8 *)(LpMultiBuff + LulNextFreeDataIdx),
        /* MISRA Violation: END Msg(4:0488)-5 */
          /* MISRA Violation: START Msg(4:0306)-2 */
          (const uint8 *)LpDataDesc->Dptr, LulCopyDataSize);
          /* MISRA Violation: END Msg(4:0306)-2 */
        /* Increase to the next free buffer index */
        LulNextFreeDataIdx += LulCopyDataSize;
        /* Cumulate the size */
        /* MISRA Violation: START Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:0306)-2 */
        ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength +=
        /* MISRA Violation: END Msg(4:0306)-2 */
        /* MISRA Violation: END Msg(2:2814)-13 */
          LulCopyDataSize;
        /* Reset descriptor to FEMPTY */
        /* MISRA Violation: START Msg(4:0303)-1 */
        Eth_DescUpdate((uint32)LpDataDesc);
        /* MISRA Violation: END Msg(4:0303)-1 */
      }
      else if (ETH_DESC_FEND == (LpDataDesc->Header).Dt)
      {
        /* End of frame */
        /* Invalidate buffer data */
        /*CR7_Invalidate_DCache_By_Addr(LpDataDesc->Dptr,
          ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
        Eth_CacheInvalidate(LpDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
        /* Get the size of buffer to be copied */
        LulCopyDataSize = (LpDataDesc->Header).Ds;
        if ((LulNextFreeDataIdx + LulCopyDataSize) > ETH_MAX_FRAME_SIZE)
        {
          /* Only copy the portion up to ETH_MAX_FRAME_SIZE */
          LulCopyDataSize = ETH_MAX_FRAME_SIZE - LulNextFreeDataIdx;
        }
        else
        {
          /* No action required */
        }
        /* Copy the END of received frame to combined frame */
        /* Cast to be compatible with type of input parameter */
        /* MISRA Violation: START Msg(4:0488)-5 */
        Eth_RamCopy((uint8 *)(LpMultiBuff + LulNextFreeDataIdx),
        /* MISRA Violation: END Msg(4:0488)-5 */
          /* MISRA Violation: START Msg(4:0306)-2 */
          (const uint8 *)LpDataDesc->Dptr, LulCopyDataSize);
          /* MISRA Violation: END Msg(4:0306)-2 */
        /* Cumulate the size */
        /* MISRA Violation: START Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:0306)-2 */
        ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength +=
        /* MISRA Violation: END Msg(4:0306)-2 */
        /* MISRA Violation: END Msg(2:2814)-13 */
          LulCopyDataSize;
        #if (ETH_DEVICE_NAME == RCAR_H3)
        #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
        /* Check the state information (PC bit) of the received descriptor. */
        /* TRACE[ETH_DDD_ACT_165_GLB005] */
        Eth_GaaPayloadCrc[LucQIndex] =
          (uint8)((LpDataDesc->Header).Ctrl >> ETH_THREE_8) & ETH_ONE_8;
        #endif
        #endif
        /* Reset descriptor to FEMPTY */
        /* MISRA Violation: START Msg(4:0303)-1 */
        Eth_DescUpdate((uint32)LpDataDesc);
        /* MISRA Violation: END Msg(4:0303)-1 */
        break;
      }
      else
      {
        /* Invalid Descriptor type */
        LenReturnError = ETH_ERR_NG;
        /* MISRA Violation: START Msg(4:0771)-11 */
        break;
        /* MISRA Violation: END Msg(4:0771)-11 */
      }
    }
  } /* (ETH_DESC_FSTART == (LpDataDesc->Header).Dt) */
  else
  {
    /* Invalid Descriptor type */
    LenReturnError = ETH_ERR_NG;
  } /* (ETH_DESC_FSINGLE == (LpDataDesc->Header).Dt) */

  if ((ETH_RX_BUF_TOTAL > LucCount) && (ETH_ERR_OK == LenReturnError) &&
    /* MISRA Violation: START Msg(4:0303)-1 */
    ((uint32)LpDataDesc <= LulRxQueueEnd) &&
    /* MISRA Violation: END Msg(4:0303)-1 */
    /* MISRA Violation: START Msg(4:0303)-1 */
    ((uint32)LpDataDesc >= LulRxQueueStart))
    /* MISRA Violation: END Msg(4:0303)-1 */
  {
    /* No error happened, increase to next descriptor */
    /* MISRA Violation: START Msg(4:0489)-6 */
    LpDataDesc++;
    /* MISRA Violation: END Msg(4:0489)-6 */
    if (ETH_DESC_LINKFIX == (LpDataDesc->Header).Dt)
    {
      /* MISRA Violation: START Msg(4:0306)-2 */
      LpDataDesc = (Eth_DataDescType *)LpDataDesc->Dptr;
      /* MISRA Violation: END Msg(4:0306)-2 */
    }
    else
    {
      /* No action required */
    }
    /* MISRA Violation: START Msg(4:0303)-1 */
    if (((uint32)LpDataDesc > LulRxQueueEnd) ||
    /* MISRA Violation: END Msg(4:0303)-1 */
      /* MISRA Violation: START Msg(4:0303)-1 */
      ((uint32)LpDataDesc < LulRxQueueStart))
      /* MISRA Violation: END Msg(4:0303)-1 */
    {
      /* Invalid Descriptor type */
      LenReturnError = ETH_ERR_NG;
    }
    else
    {
      /* TRACE[ETH_DDD_ACT_165_GLB001] */
      /* MISRA Violation: START Msg(4:0303)-1 */
      Eth_GaaDevice[LucCtrlIdx].NextRxDesc[LucQIndex] = (uint32)LpDataDesc;
      /* MISRA Violation: END Msg(4:0303)-1 */
      #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
      /* No timestamp is stored in normal descriptors */
      /* TRACE[ETH_DDD_ACT_165_GLB006] */
      Eth_GaaTempTsDescr[LucQIndex] = NULL_PTR;
      /* TRACE[ETH_DDD_ACT_165_GLB007] */
      Eth_GaaTempTimeStamp[LucQIndex].Timestamp0 = ETH_ZERO_32;
      /* TRACE[ETH_DDD_ACT_165_GLB008] */
      Eth_GaaTempTimeStamp[LucQIndex].Timestamp1 = ETH_ZERO_32;
      /* TRACE[ETH_DDD_ACT_165_GLB009] */
      Eth_GaaTempTimeStamp[LucQIndex].Timestamp2 = ETH_ZERO_16;
      #endif
    }
  }
  else
  {
    /* Error happened in previous step */
    LenReturnError = ETH_ERR_NG;
  }

  return LenReturnError;
}
#endif
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxExtQueueProcess
**
** Service ID            : NA
**
** Description           : This process extend descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx    - Index of the ETH Controller
**                         LucQIndex     - Queue Index which has received
**                         LulRxFramePtr - Pointer to rx frame
**                         LulExtDescPtr - Pointer to rx Descriptor
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - return type of error
**
** Preconditions         : None
**
** Global Variables      : Eth_GaaDevice, Eth_GstDescTable, Eth_GaaTempTsDescr,
**                         Eth_GaaTempTimeStamp, Eth_GaaPayloadCrc,
**                         Eth_GucRxMultiBuffFrame
**
** Functions invoked     : Eth_RamCopy, CR7_Invalidate_DCache_By_Addr,
**                         Eth_DescTsUpdate
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_164] */
/* TRACE[ETH_DDD_INF_132] */
static FUNC(Eth_ErrorType, ETH_CODE_FAST) Eth_RxExtQueueProcess
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex,
  uint32 LulRxFramePtr,
  uint32 LulExtDescPtr
)
{
  P2VAR(volatile Eth_ExtDataDescType, AUTOMATIC, ETH_APPL_DATA) LpExtDataDesc;
  P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) LpMultiBuff;
  VAR(uint32, AUTOMATIC) LulNextFreeDataIdx;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint32, AUTOMATIC) LulRxQueueStart;
  VAR(uint32, AUTOMATIC) LulRxQueueEnd;
  VAR(uint32, AUTOMATIC) LulCopyDataSize;

  LulNextFreeDataIdx = ETH_ZERO_32;
  /* MISRA Violation: START Msg(4:2982)-18 */
  LulCopyDataSize = ETH_ZERO_32;
  /* MISRA Violation: END Msg(4:2982)-18 */
  LenReturnError = ETH_ERR_OK;
  LucCount = ETH_ZERO_8;

  /* Get Start Descriptor Address for the corresponding Queue */
  LulRxQueueStart = Eth_GstDescTable[ETH_RXBEQ_OFFSET + LucQIndex].Dptr;

  /* Get End Descriptor Address for the corresponding Queue */
  LulRxQueueEnd = LulRxQueueStart +
    (ETH_SPECIAL_DESC_SIZE * (ETH_RX_BUF_TOTAL - ETH_ONE_8));
  /* Timestamp enabled */
  /* MISRA Violation: START Msg(4:0303)-1 */
  LpExtDataDesc = (volatile Eth_ExtDataDescType *)LulExtDescPtr;
  /* MISRA Violation: END Msg(4:0303)-1 */

  /* MISRA Violation: START Msg(2:2814)-13 */
  if (ETH_DESC_FSINGLE == (LpExtDataDesc->Header).Dt)
  /* MISRA Violation: END Msg(2:2814)-13 */
  {
    /* Single frame single buffer */
    /* Descriptor type correct */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameAddr =
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
      LpExtDataDesc->Dptr;
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength =
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
      (LpExtDataDesc->Header).Ds;
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->Timestamp =
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
      LpExtDataDesc->Timestamp;
    /* Invalidate buffer data */
    /*CR7_Invalidate_DCache_By_Addr(LpExtDataDesc->Dptr,
      ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
    Eth_CacheInvalidate(LpExtDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
    #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
    /* Store Active descriptor where I have received the TimeStamp */
    /* This will be used in the GetIngressTimestamp */
    /* TRACE[ETH_DDD_ACT_164_GLB004] */
    Eth_GaaTempTsDescr[LucQIndex] = LpExtDataDesc;
    /* TRACE[ETH_DDD_ACT_164_GLB005] */
    Eth_GaaTempTimeStamp[LucQIndex] = LpExtDataDesc->Timestamp;
    #endif

    #if (ETH_DEVICE_NAME == RCAR_H3)
    #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
    /* Check the state information (PC bit) of the received descriptor. */
    /* TRACE[ETH_DDD_ACT_164_GLB006] */
    Eth_GaaPayloadCrc[LucQIndex] =
      (uint8)((LpExtDataDesc->Header).Ctrl >> ETH_THREE_8) & ETH_ONE_8;
    #endif
    #endif /* (ETH_DEVICE_NAME == RCAR_H3) */
  } /* (ETH_DESC_FSINGLE == (LpExtDataDesc->Header).Dt) */
  else if (ETH_DESC_FSTART == (LpExtDataDesc->Header).Dt)
  {
    /* Single frame multi buffer */
    /* Get the multi buffer address of corresponding Queue */
    LpMultiBuff = Eth_GucRxMultiBuffFrame[LucQIndex];
    /* Assign address of the combined received frame */
    /* MISRA Violation: START Msg(4:0306)-2 */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameAddr = (uint32)(LpMultiBuff);
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* Invalidate buffer data */
    /*CR7_Invalidate_DCache_By_Addr(LpExtDataDesc->Dptr,
      ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
    Eth_CacheInvalidate(LpExtDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
    /* Get the size of buffer to be copied */
    LulCopyDataSize = (LpExtDataDesc->Header).Ds;
    /* MISRA Violation: START Msg(4:2986)-21 */
    if ((LulNextFreeDataIdx + LulCopyDataSize) > ETH_MAX_FRAME_SIZE)
    /* MISRA Violation: END Msg(4:2986)-21 */
    {
      /* Only copy the portion up to ETH_MAX_FRAME_SIZE */
      /* MISRA Violation: START Msg(4:2984)-19 */
      LulCopyDataSize = ETH_MAX_FRAME_SIZE - LulNextFreeDataIdx;
      /* MISRA Violation: END Msg(4:2984)-19 */
    }
    else
    {
      /* No action required */
    }
    /* Copy the START of received frame to combined frame */
    /* Cast to be compatible with type of input parameter */
    /* MISRA Violation: START Msg(4:0488)-5 */
    Eth_RamCopy((uint8 *)(LpMultiBuff + LulNextFreeDataIdx),
    /* MISRA Violation: END Msg(4:0488)-5 */
      /* MISRA Violation: START Msg(4:0306)-2 */
      (const uint8 *)LpExtDataDesc->Dptr, LulCopyDataSize);
      /* MISRA Violation: END Msg(4:0306)-2 */
    /* Increase to the next free buffer index */
    /* MISRA Violation: START Msg(4:2986)-21 */
    LulNextFreeDataIdx += LulCopyDataSize;
    /* MISRA Violation: END Msg(4:2986)-21 */
    /* Cumulate the size */
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength =
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
      LulCopyDataSize;
    /* Start cumulating the size and combining frame until the end */
    for (LucCount = ETH_ONE_8; LucCount < ETH_RX_BUF_TOTAL; LucCount++)
    {
      /* Increase to the next descriptor */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpExtDataDesc++;
      /* MISRA Violation: END Msg(4:0489)-6 */
      if (ETH_DESC_LINKFIX == (LpExtDataDesc->Header).Dt)
      {
        /* MISRA Violation: START Msg(4:0306)-2 */
        LpExtDataDesc = (Eth_ExtDataDescType *)LpExtDataDesc->Dptr;
        /* MISRA Violation: END Msg(4:0306)-2 */
      }
      else
      {
        /* No action required */
      }
      if (ETH_DESC_FMID == (LpExtDataDesc->Header).Dt)
      {
        /* Invalidate buffer data */
        /*CR7_Invalidate_DCache_By_Addr(LpExtDataDesc->Dptr,
          ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
        Eth_CacheInvalidate(LpExtDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
        /* Get the size of buffer to be copied */
        LulCopyDataSize = (LpExtDataDesc->Header).Ds;
        if ((LulNextFreeDataIdx + LulCopyDataSize) > ETH_MAX_FRAME_SIZE)
        {
          /* Only copy the portion up to ETH_MAX_FRAME_SIZE */
          LulCopyDataSize = ETH_MAX_FRAME_SIZE - LulNextFreeDataIdx;
        }
        else
        {
          /* No action required */
        }
        /* Copy the MID of received frame to combined frame */
        /* Cast to be compatible with type of input parameter */
        /* MISRA Violation: START Msg(4:0488)-5 */
        Eth_RamCopy((uint8 *)(LpMultiBuff + LulNextFreeDataIdx),
        /* MISRA Violation: END Msg(4:0488)-5 */
          /* MISRA Violation: START Msg(4:0306)-2 */
          (const uint8 *)LpExtDataDesc->Dptr, LulCopyDataSize);
          /* MISRA Violation: END Msg(4:0306)-2 */
        /* Increase to the next free buffer index */
        LulNextFreeDataIdx += LulCopyDataSize;
        /* Cumulate the size */
        /* MISRA Violation: START Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:0306)-2 */
        ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength +=
        /* MISRA Violation: END Msg(4:0306)-2 */
        /* MISRA Violation: END Msg(2:2814)-13 */
          LulCopyDataSize;
        /* Reset descriptor to FEMPTY */
        /* MISRA Violation: START Msg(4:0303)-1 */
        Eth_DescTsUpdate((uint32)LpExtDataDesc);
        /* MISRA Violation: END Msg(4:0303)-1 */
      }
      else if (ETH_DESC_FEND == (LpExtDataDesc->Header).Dt)
      {
        /* End of frame */
        /* Invalidate buffer data */
        /*CR7_Invalidate_DCache_By_Addr(LpExtDataDesc->Dptr,
          ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);*/
        Eth_CacheInvalidate(LpExtDataDesc->Dptr, ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN);
        /* Get the size of buffer to be copied */
        LulCopyDataSize = (LpExtDataDesc->Header).Ds;
        if ((LulNextFreeDataIdx + LulCopyDataSize) > ETH_MAX_FRAME_SIZE)
        {
          /* Only copy the portion up to ETH_MAX_FRAME_SIZE */
          LulCopyDataSize = ETH_MAX_FRAME_SIZE - LulNextFreeDataIdx;
        }
        else
        {
          /* No action required */
        }
        /* Copy the END of received frame to combined frame */
        /* Cast to be compatible with type of input parameter */
        /* MISRA Violation: START Msg(4:0488)-5 */
        Eth_RamCopy((uint8 *)(LpMultiBuff + LulNextFreeDataIdx),
        /* MISRA Violation: END Msg(4:0488)-5 */
          /* MISRA Violation: START Msg(4:0306)-2 */
          (const uint8 *)LpExtDataDesc->Dptr, LulCopyDataSize);
          /* MISRA Violation: END Msg(4:0306)-2 */
        /* Cumulate the size */
        /* MISRA Violation: START Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:0306)-2 */
        ((Eth_RxSingleFrameType *)LulRxFramePtr)->FrameLength +=
        /* MISRA Violation: END Msg(4:0306)-2 */
        /* MISRA Violation: END Msg(2:2814)-13 */
          LulCopyDataSize;
        #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
        /* Store Active descriptor where I have received the TimeStamp */
        /* This will be used in the GetIngressTimestamp */
        /* TRACE[ETH_DDD_ACT_164_GLB008] */
        Eth_GaaTempTsDescr[LucQIndex] = LpExtDataDesc;
        /* TRACE[ETH_DDD_ACT_164_GLB009] */
        Eth_GaaTempTimeStamp[LucQIndex] = LpExtDataDesc->Timestamp;
        #endif
        #if (ETH_DEVICE_NAME == RCAR_H3)
        #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
        /* Check the state information (PC bit) of the received descriptor. */
        /* TRACE[ETH_DDD_ACT_164_GLB010] */
        Eth_GaaPayloadCrc[LucQIndex] =
          (uint8)((LpExtDataDesc->Header).Ctrl >> ETH_THREE_8) & ETH_ONE_8;
        #endif
        #endif
        /* Reset descriptor to FEMPTY */
        /* MISRA Violation: START Msg(4:0303)-1 */
        Eth_DescTsUpdate((uint32)LpExtDataDesc);
        /* MISRA Violation: END Msg(4:0303)-1 */
        break;
      }
      else
      {
        /* Invalid Descriptor type */
        LenReturnError = ETH_ERR_NG;
        /* MISRA Violation: START Msg(4:0771)-11 */
        break;
        /* MISRA Violation: END Msg(4:0771)-11 */
      }
    }
  }
  else
  {
    /* Invalid Descriptor type */
    LenReturnError = ETH_ERR_NG;
  } /* (ETH_DESC_FSINGLE == (LpExtDataDesc->Header).Dt) */

  if ((ETH_RX_BUF_TOTAL > LucCount) && (ETH_ERR_OK == LenReturnError) &&
    /* MISRA Violation: START Msg(4:0303)-1 */
    ((uint32)LpExtDataDesc <= LulRxQueueEnd) &&
    /* MISRA Violation: END Msg(4:0303)-1 */
    /* MISRA Violation: START Msg(4:0303)-1 */
    ((uint32)LpExtDataDesc >= LulRxQueueStart))
    /* MISRA Violation: END Msg(4:0303)-1 */
  {
    /* MISRA Violation: START Msg(4:0489)-6 */
    LpExtDataDesc++;
    /* MISRA Violation: END Msg(4:0489)-6 */
    if (ETH_DESC_LINKFIX == (LpExtDataDesc->Header).Dt)
    {
      /* MISRA Violation: START Msg(4:0306)-2 */
      LpExtDataDesc = (Eth_ExtDataDescType *)LpExtDataDesc->Dptr;
      /* MISRA Violation: END Msg(4:0306)-2 */
    }
    else
    {
      /* No action required */
    }
    /* MISRA Violation: START Msg(4:0303)-1 */
    if (((uint32)LpExtDataDesc > LulRxQueueEnd) ||
    /* MISRA Violation: END Msg(4:0303)-1 */
      /* MISRA Violation: START Msg(4:0303)-1 */
      ((uint32)LpExtDataDesc < LulRxQueueStart))
      /* MISRA Violation: END Msg(4:0303)-1 */
    {
      /* Invalid descriptor chain */
      LenReturnError = ETH_ERR_NG;
    }
    else
    {
      /* TRACE[ETH_DDD_ACT_164_GLB001] */
      /* MISRA Violation: START Msg(4:0303)-1 */
      Eth_GaaDevice[LucCtrlIdx].NextRxDesc[LucQIndex] = (uint32)LpExtDataDesc;
      /* MISRA Violation: END Msg(4:0303)-1 */
    }
  }
  else
  {
    /* Error happened in previous step */
    LenReturnError = ETH_ERR_NG;
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_SingleDescFrameSend
**
** Service ID            : NA
**
** Description           : Send frame (single frame single buffer)
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         blTxConfirmation - Transmission confirmation
**                         BufIdx     - Buffer index
**                         LpFrame    - Tx Frame Pointer
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - return type of error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstBufferLock, Eth_GaaBaseAddr
**
** Functions invoked     : Eth_CurrentDescGet, Eth_TxReqStart,
**                         Dem_ReportErrorStatus, Eth_WaitTxQueueEmpty
**
** Registers Used        : CDARq q = 0,1...,21, TCCR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_055] */
/* TRACE[ETH_DDD_INF_053] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_SingleDescFrameSend
(
  uint8 LucCtrlIdx,
  const boolean blTxConfirmation,
  Eth_BufIdxType BufIdx,
  CONSTP2CONST(Eth_TxSingleFrameType, AUTOMATIC, ETH_APPL_CONST) LpFrame
)
{
  P2VAR(Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) LpDataDesc;
  /* MISRA Violation: START Msg(4:0759)-10 */
  Eth_TxRxCtrlType LunTxCtrl;
  /* MISRA Violation: END Msg(4:0759)-10 */
  Eth_ErrorType LenReturnError;
  Eth_TxChannelType LenQIndex;
  VAR(Std_ReturnType, AUTOMATIC) LddIsQueueEmpty;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LenReturnError = ETH_ERR_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  LenQIndex = Eth_GstBufferLock[BufIdx].ucChannel;

  LddIsQueueEmpty = Eth_WaitTxQueueEmpty(LucCtrlIdx, (uint8)LenQIndex);

  if (E_OK == LddIsQueueEmpty)
  {
    /* MISRA Violation: START Msg(4:0306)-2 */
    LpDataDesc = (Eth_DataDescType *)Eth_CurrentDescGet(LucCtrlIdx,
    /* MISRA Violation: END Msg(4:0306)-2 */
      (uint8)LenQIndex, ETH_TX);
    if (NULL_PTR != LpDataDesc)
    {
      if (ETH_DESC_LINKFIX == (LpDataDesc->Header).Dt)
      {
        /* MISRA Violation: START Msg(4:0306)-2 */
        LpDataDesc = (Eth_DataDescType *)LpDataDesc->Dptr;
        /* MISRA Violation: END Msg(4:0306)-2 */
      }
      else
      {
        /* No action required */
      }
      LunTxCtrl.Word = ETH_ZERO_32;
      /* MISRA Violation: START Msg(2:2814)-13 */
      if ((uint8)ETH_ENABLE == LpFrame->Timestamp)
      /* MISRA Violation: END Msg(2:2814)-13 */
      {
        LunTxCtrl.TxCtrl.Tsr = ETH_ONE;
      }
      else
      {
        /* Do Nothing*/
      }

      if (ETH_ENABLE == LpFrame->MacStatus)
      {
        LunTxCtrl.TxCtrl.Msr = ETH_ONE;
      }
      else
      {
        /* Do Nothing*/
      }

      LunTxCtrl.TxCtrl.Tag = LpFrame->Tag;

      /* Build the descriptor */
      LpDataDesc->Header.Dt   = ETH_DESC_FSINGLE;
      LpDataDesc->Header.Die  = ETH_ZERO;
      LpDataDesc->Header.Ctrl = LunTxCtrl.Word;
      LpDataDesc->Header.Ds   = LpFrame->Length;
      LpDataDesc->Dptr        = LpFrame->Address;

      /* Store the descriptor for TxConfirmation checking */
      /* TRACE[ETH_DDD_ACT_055_GLB001] */
      /* MISRA Violation: START Msg(4:0306)-2 */
      Eth_GstBufferLock[BufIdx].descAddr = (uint32)LpDataDesc;
      /* MISRA Violation: END Msg(4:0306)-2 */
      /* End of Set */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpDataDesc++;
      /* MISRA Violation: END Msg(4:0489)-6 */
      if (ETH_DESC_LINKFIX == (LpDataDesc->Header).Dt)
      {
        /* MISRA Violation: START Msg(4:0306)-2 */
        LpDataDesc = (Eth_DataDescType *)LpDataDesc->Dptr;
        /* MISRA Violation: END Msg(4:0306)-2 */
      }
      else
      {
        /* No action required */
      }
      LpDataDesc->Header.Dt   = ETH_DESC_EOS;
      LpDataDesc->Header.Die  = ETH_ONE;
      LpDataDesc->Header.Ctrl = ETH_ZERO;
      LpDataDesc->Header.Ds   = ETH_ZERO;
      LpDataDesc->Dptr        = ETH_ZERO_32;

      Eth_GstBufferLock[BufIdx].bufTxConfirm = blTxConfirmation;
      Eth_GstBufferLock[BufIdx].bufTxFlag = ETH_TRUE;
      LenReturnError = Eth_TxReqStart(LucCtrlIdx, (uint8)LenQIndex);
    } /* (NULL_PTR != LpDataDesc) */
    else
    {
      /* Error in descriptor chain, report DEM */
      Dem_ReportErrorStatus(ETH_E_DESC_DATA, DEM_EVENT_STATUS_FAILED);
      LenReturnError = ETH_ERR_NG;
    }
  }
  else
  {
    LenReturnError = ETH_ERR_NG;
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxQueueProcess
**
** Service ID            : NA
**
** Description           : Process Receive Queue
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx    - Index of the ETH Controller
**                         LucQIdx       - Channel index
**                         LulRxFramePtr - Rx Frame Number
**                         LulDescAddr   - Pointer to descriptor address
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - return type of error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaDevice, Eth_GstDescTable, Eth_GaaTempTsDescr,
**                         Eth_GaaTempTimeStamp, Eth_GucRxMultiBuffFrame,
**                         Eth_GaaPayloadCrc, Eth_GaaBaseAddr
**
** Functions invoked     : Eth_RxExtQueueProcess, Eth_RxNormalQueueProcess
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_054] */
/* TRACE[ETH_DDD_INF_054] */
FUNC(Eth_ErrorType, ETH_CODE_FAST) Eth_RxQueueProcess
(
  uint8 LucCtrlIdx,
  uint8 LucQIdx,
  uint32 LulRxFramePtr,
  uint32 LulDescAddr
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LenReturnError = ETH_ERR_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */

  if (ETH_BECHANNEL == LucQIdx)
  {
    /* Best effort channel */
    #if (ETH_RX_BEQUEUE_TIMESTAMP == STD_ON)
    /* Timestamp enabled */
    LenReturnError = Eth_RxExtQueueProcess(LucCtrlIdx, LucQIdx,
      LulRxFramePtr, LulDescAddr);
    #else /* (ETH_RX_BEQUEUE_TIMESTAMP == STD_ON) */
    /* Timestamp disabled */
    LenReturnError = Eth_RxNormalQueueProcess(LucCtrlIdx, LucQIdx,
      LulRxFramePtr, LulDescAddr);
    #endif /* (ETH_RX_BEQUEUE_TIMESTAMP == STD_ON) */
  } /* (ETH_BECHANNEL == LucQIdx) */
  else if (ETH_NCCHANNEL == LucQIdx)
  {
    /* Network control channel */
    LenReturnError = Eth_RxExtQueueProcess(LucCtrlIdx, LucQIdx,
      LulRxFramePtr, LulDescAddr);
  } /* (ETH_NCCHANNEL == LucQIdx) */
  else
  {
    /* Stream channel */
    #if (ETH_RX_SQUEUE_TIMESTAMP == STD_ON)
    /* Timestamp enabled */
    LenReturnError = Eth_RxExtQueueProcess(LucCtrlIdx, LucQIdx,
      LulRxFramePtr, LulDescAddr);
    #else /* (ETH_RX_BEQUEUE_TIMESTAMP == STD_ON) */
    /* Timestamp disabled */
    LenReturnError = Eth_RxNormalQueueProcess(LucCtrlIdx, LucQIdx,
      LulRxFramePtr, LulDescAddr);
    #endif /* (ETH_RX_BEQUEUE_TIMESTAMP == STD_ON) */
  } /* (ETH_NCCHANNEL == LucQIdx) */

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_OpModeGet
**
** Service ID            : NA
**
** Description           : Get AVB operation mode
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LenOpMode - Operating mode status
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32
**
** Registers Used        : CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_126] */
/* TRACE[ETH_DDD_INF_055] */
FUNC(Eth_OpStatusType, ETH_CODE_SLOW) Eth_OpModeGet(uint8 LucCtrlIdx)
{
  VAR(Eth_OpStatusType, AUTOMATIC) LenOpMode;
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_CsrType, AUTOMATIC) LunCsr;
  /* MISRA Violation: END Msg(4:0759)-10 */

  LenOpMode = ETH_RESET;
  /* TRACE[ETH_DDD_ACT_126_REG001] */
  LunCsr.Word =
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CSR);

  if (ETH_ONE == LunCsr.Bit.Ops)
  {
    LenOpMode = ETH_RESET;
  }
  else if (ETH_TWO == LunCsr.Bit.Ops)
  {
    LenOpMode = ETH_CONFIG;
  }
  else if (ETH_FOUR == LunCsr.Bit.Ops)
  {
    LenOpMode = ETH_OPERATION;
  }
  else if (ETH_EIGHT == LunCsr.Bit.Ops)
  {
    LenOpMode = ETH_STANDBY;
  }
  else
  {
    /* No action required */
  }
  return LenOpMode;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_CommonConfig
**
** Service ID            : NA
**
** Description           : Common configuration
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx  - Index of the ETH Controller
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
** Functions invoked     : Eth_ReadRegister32, Eth_VerifyReadRegister32
**                         Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : CCC, APSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_057] */
/* TRACE[ETH_DDD_INF_056] */
static FUNC(void, ETH_CODE_SLOW) Eth_CommonConfig
(
  uint8 LucCtrlIdx
)
{
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_CccType, AUTOMATIC) LunCcc;
  /* MISRA Violation: END Msg(4:0759)-10 */
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_ApsrType, AUTOMATIC) LunApsr;
  /* MISRA Violation: END Msg(4:0759)-10 */

  /* TRACE[ETH_DDD_ACT_057_REG001] */
  LunCcc.Word =
    /* MISRA Violation: START Msg(4:2985)-20 */
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CCC);
    /* MISRA Violation: END Msg(4:2985)-20 */
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_057_REG004] */
  /* MISRA Violation: START Msg(4:2985)-20 */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CCC,
  /* MISRA Violation: END Msg(4:2985)-20 */
    LunCcc.Word);

  /* MISRA Violation: START Msg(2:2814)-13 */
  LunCcc.Bit.Lbme = Eth_GpConfigPtr->stComConfig.Lbme;
  /* MISRA Violation: END Msg(2:2814)-13 */

  /* TRACE[ETH_DDD_ACT_057_REG002] */
  /* MISRA Violation: START Msg(4:2985)-20 */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
  /* MISRA Violation: END Msg(4:2985)-20 */
    ETH_CCC, LunCcc.Word);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_057_REG003] */
  /* MISRA Violation: START Msg(4:2985)-20 */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
  /* MISRA Violation: END Msg(4:2985)-20 */
    ETH_CCC, LunCcc.Word, ETH_REG_MASK);

  /* Tx/Rx clock internal delay setting */
  LunApsr.Word     = ETH_ZERO_32;
  #if (ETH_DEVICE_NAME == RCAR_V3H)
  LunApsr.Bit.Tdm = ETH_ONE;
  #else
  LunApsr.Bit.Tdm = Eth_GpConfigPtr->stClkDelayConfig.Tdm;
  #endif
  LunApsr.Bit.Rdm = Eth_GpConfigPtr->stClkDelayConfig.Rdm;
  /* Enable Rx Signals Timing Adjuster */
  #if (ETH_DEVICE_NAME == RCAR_V3M)
  /* MISRA Violation: START Msg(4:4424)-27 */
  LunApsr.Bit.Rsta  = ETH_ENABLE;
  /* MISRA Violation: END Msg(4:4424)-27 */
  #endif
  /* TRACE[ETH_DDD_ACT_057_REG005] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_APSR, LunApsr.Word);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_057_REG006] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_APSR, LunApsr.Word, ETH_REG_MASK);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_DescConfig
**
** Service ID            : NA
**
** Description           : Configure descriptor chains
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx  - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaDevice, Eth_GpConfigPtr, Eth_GucRxBuff
**
** Functions invoked     : Eth_SetDescChain
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_061] */
/* TRACE[ETH_DDD_INF_059] */
FUNC(void, ETH_CODE_SLOW) Eth_DescConfig(uint8 LucCtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucQIndex;
  VAR(uint8, AUTOMATIC) LucLoopCnt;
  VAR(uint8, AUTOMATIC) LucNumberOfRxQueue;
  P2VAR(Eth_LinkDescType, AUTOMATIC, ETH_APPL_DATA) LpLinkAddr;

  /* MISRA Violation: START Msg(2:2814)-13 */
  LucNumberOfRxQueue = Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfRxQueue;
  /* MISRA Violation: END Msg(2:2814)-13 */
  /* Init link descriptor */
  /* MISRA Violation: START Msg(4:0310)-3 */
  LpLinkAddr = (Eth_LinkDescType *)(&Eth_GstDescTable[ETH_ZERO]);
  /* MISRA Violation: END Msg(4:0310)-3 */

  /* TX Queues */
  for (LucLoopCnt = ETH_ZERO_8; LucLoopCnt < ETH_TXQ_MAXNUM; LucLoopCnt++)
  {
    if (ETH_ZERO_32 !=  LpLinkAddr->Dptr)
    {
      /* Get Tx Queue index */
      LucQIndex =
        /* MISRA Violation: START Msg(4:0491)-7 */
        Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucLoopCnt].id;
        /* MISRA Violation: END Msg(4:0491)-7 */

      Eth_SetDescChain((LpLinkAddr->Dptr), LucQIndex, ETH_TX);
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpLinkAddr++;
      /* MISRA Violation: END Msg(4:0489)-6 */
    }
    else
    {
      /* This Queue is not configured */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpLinkAddr++;
      /* MISRA Violation: END Msg(4:0489)-6 */
    }
  }
  /* RX Queues */
  for (LucLoopCnt = ETH_ZERO_8; LucLoopCnt < LucNumberOfRxQueue; LucLoopCnt++)
  {
    if (ETH_ZERO_32 !=  LpLinkAddr->Dptr)
    {
      LucQIndex =
        /* MISRA Violation: START Msg(4:0491)-7 */
        Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucLoopCnt].id;
        /* MISRA Violation: END Msg(4:0491)-7 */
      Eth_SetDescChain((LpLinkAddr->Dptr), LucQIndex, ETH_RX);
      /* Initialize Next free Rx descriptor address */
      /* TRACE[ETH_DDD_ACT_061_GLB001] */
      Eth_GaaDevice[LucCtrlIdx].NextRxDesc[LucQIndex] =
      (uint32)(LpLinkAddr->Dptr);
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpLinkAddr++;
      /* MISRA Violation: END Msg(4:0489)-6 */
    }
    else
    {
      /* This Queue is not configured */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpLinkAddr++;
      /* MISRA Violation: END Msg(4:0489)-6 */
    }
  }
}

/*******************************************************************************
** Function Name         : Eth_SetDescChain
**
** Service ID            : NA
**
** Description           : Configured the Descriptor chain for each Queue
**                         (Circular Chain)
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LulAddr      - Address of the first Queue descriptor
**                         LucQIndex    - Index of the Queue
**                                        (0..3 for Tx and 0..17 for Rx)
**                         LenDirection - Queue Direction (e.g. TX or RX)
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GucRxBuff
**
** Functions invoked     : Eth_SetLinkFix, Eth_SetFempty, Eth_SetExtFempty
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_121] */
/* TRACE[ETH_DDD_INF_060] */
static FUNC(void, ETH_CODE_SLOW) Eth_SetDescChain
(
  uint32 LulAddr,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
)
{
  VAR(uint8, AUTOMATIC) LucIndex;
  P2VAR(Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) LpCurrentDataDesc;
  P2VAR(Eth_ExtDataDescType, AUTOMATIC, ETH_APPL_DATA) LpCurrentExtDataDesc;
  VAR(boolean, AUTOMATIC) LblRxTimeStamp;

  /* Initialize the counter for any Queue */
  /* MISRA Violation: START Msg(4:2982)-18 */
  LucIndex = ETH_ZERO_8;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* Time stamp is enabled by default for RX NC Queue */
  LblRxTimeStamp = ETH_TRUE;

  /* Determine time stamp for RX BE and RX S */
  if ((ETH_RX == LenDirection) && (ETH_NCCHANNEL != LucQIndex))
  {
    if (ETH_BECHANNEL == LucQIndex)
    {
      /* RX BE */
      #if (ETH_RX_BEQUEUE_TIMESTAMP == STD_ON)
      LblRxTimeStamp = ETH_TRUE;
      #else
      LblRxTimeStamp = ETH_FALSE;
      #endif
    }
    else
    {
      /* RX S */
      #if (ETH_RX_SQUEUE_TIMESTAMP == STD_ON)
      LblRxTimeStamp = ETH_TRUE;
      #else
      LblRxTimeStamp = ETH_FALSE;
      #endif
    }
  }
  else
  {
    /* Do nothing if TX of RX NC */
  }

  if (ETH_TX == LenDirection)
  {
    /* TX */
    /* MISRA Violation: START Msg(4:0306)-2 */
    LpCurrentDataDesc = (Eth_DataDescType *) LulAddr;
    /* MISRA Violation: END Msg(4:0306)-2 */
    for (LucIndex = ETH_ZERO_8; LucIndex < ETH_TX_BUF_TOTAL; LucIndex++)
    {
      /* Set FEmpty*/
      Eth_SetFempty(LpCurrentDataDesc,
        LucQIndex, ETH_TX, LucIndex);
      /* MISRA Violation: START Msg(2:2824)-14 */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpCurrentDataDesc++;
      /* MISRA Violation: END Msg(4:0489)-6 */
      /* MISRA Violation: END Msg(2:2824)-14 */
    }
    /* Set FEmpty for EOS */
    Eth_SetFempty(LpCurrentDataDesc,
      LucQIndex, ETH_TX, LucIndex);
    /* MISRA Violation: START Msg(4:0489)-6 */
    LpCurrentDataDesc++;
    /* MISRA Violation: END Msg(4:0489)-6 */
    /* Set LinkFix */
    /* MISRA Violation: START Msg(4:0306)-2 */
    Eth_SetLinkFix((uint32)LpCurrentDataDesc, LulAddr);
    /* MISRA Violation: END Msg(4:0306)-2 */
  }
  /* MISRA Violation: START Msg(4:2996)-25 */
  /* MISRA Violation: START Msg(4:2992)-23 */
  else if (ETH_FALSE == LblRxTimeStamp)
  /* MISRA Violation: END Msg(4:2992)-23 */
  /* MISRA Violation: END Msg(4:2996)-25 */
  {
    /* RX TimeStamp disable*/
    /* MISRA Violation: START Msg(4:0306)-2 */
    /* MISRA Violation: START Msg(4:2880)-15 */
    LpCurrentDataDesc = (Eth_DataDescType *) LulAddr;
    /* MISRA Violation: END Msg(4:2880)-15 */
    /* MISRA Violation: END Msg(4:0306)-2 */
    for (LucIndex = ETH_ZERO_8; LucIndex < ETH_RX_BUF_TOTAL; LucIndex++)
    {
      /* Set FEmpty for normal descriptor */
      Eth_SetFempty(LpCurrentDataDesc,
        LucQIndex, ETH_RX, LucIndex);
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpCurrentDataDesc++;
      /* MISRA Violation: END Msg(4:0489)-6 */
    }
    /* Set LinkFix */
    /* MISRA Violation: START Msg(4:0306)-2 */
    Eth_SetLinkFix((uint32)LpCurrentDataDesc, LulAddr);
    /* MISRA Violation: END Msg(4:0306)-2 */
  }
  else
  {
    /* RX TimeStamp enable*/
    /* MISRA Violation: START Msg(4:0306)-2 */
    LpCurrentExtDataDesc = (Eth_ExtDataDescType *)LulAddr;
    /* MISRA Violation: END Msg(4:0306)-2 */
    for (LucIndex = ETH_ZERO_8; LucIndex < ETH_RX_BUF_TOTAL; LucIndex++)
    {
      /* Set FEmpty for Extended Data Descriptor*/
      /* Extended Data Descriptor*/
      Eth_SetExtFempty(LpCurrentExtDataDesc,
        LucQIndex, ETH_RX, LucIndex);
      /* MISRA Violation: START Msg(2:2824)-14 */
      /* MISRA Violation: START Msg(4:0489)-6 */
      LpCurrentExtDataDesc++;
      /* MISRA Violation: END Msg(4:0489)-6 */
      /* MISRA Violation: END Msg(2:2824)-14 */
    }
    /* MISRA Violation: START Msg(4:0306)-2 */
    Eth_SetLinkFix((uint32)LpCurrentExtDataDesc, LulAddr);
    /* MISRA Violation: END Msg(4:0306)-2 */
  }
}

/*******************************************************************************
** Function Name         : Eth_SetFempty
**
** Service ID            : NA
**
** Description           : Set Fempty in the standard (8Bytes) descriptor chain
**                         for passed Queue and Buffer Index
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucQIndex    - Index of the Queue
**                                        (0..3 for Tx and 0..17 for Rx)
**                         LenDirection - Queue Direction (e.g. TX or RX)
**                         LucBufIndex  - Buffer index
**
** InOut Parameters      : pDescr       - Pointer to a Data Descriptor
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GucRxBuff
**
** Functions invoked     : Eth_GetRxBuffAddr
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_123] */
/* TRACE[ETH_DDD_INF_061] */
static FUNC(void, ETH_CODE_SLOW) Eth_SetFempty
(
  CONSTP2VAR(Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) pDescr,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection,
  uint8 LucBufIndex
)
{
  if (NULL_PTR != pDescr)
  {
    if (ETH_TX == LenDirection)
    {
      /* TX Queue*/
      pDescr->Header.Dt   = ETH_DESC_FEMPTY;
      pDescr->Header.Die  = ETH_ZERO;
      pDescr->Header.Ctrl = ETH_ZERO;
      /* It will be set only during Transmission at run time */
      pDescr->Header.Ds = ETH_ZERO;
      pDescr->Dptr = ETH_ZERO_32;
    }
    else if (ETH_BECHANNEL == LucQIndex)
    {
      /* RX BE */
      pDescr->Header.Dt   = ETH_DESC_FEMPTY;
      pDescr->Header.Die  = ETH_ZERO;
      pDescr->Header.Ctrl = ETH_ZERO;
      pDescr->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
      pDescr->Dptr = Eth_GetRxBuffAddr(LucQIndex, LucBufIndex);
    }
    else if (ETH_NCCHANNEL == LucQIndex)
    {
      /* RX NC */
      /* This is an Error as RX NC is always Extended Descriptor */
    }
    else
    {
      /* RX S*/
      pDescr->Header.Dt   = ETH_DESC_FEMPTY;
      pDescr->Header.Die  = ETH_ZERO;
      pDescr->Header.Ctrl = ETH_ZERO;
      pDescr->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
      pDescr->Dptr = Eth_GetRxBuffAddr(LucQIndex, LucBufIndex);
    }
  } /* (NULL_PTR != pDescr) */
  else
  {
    /* Cannot Set Fempty for NULL Address */
  }
}

/*******************************************************************************
** Function Name         : Eth_SetExtFempty
**
** Service ID            : NA
**
** Description           : Set Fempty in the extended (20Bytes) descriptor
**                         chain for passed Queue and Buffer Index
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucQIndex    - Index of the Queue
**                                        (0..3 for Tx and 0..17 for Rx)
**                         LenDirection - Queue Direction (e.g. TX or RX)
**                         LucBufIndex  - Buffer index
**
** InOut Parameters      : pDescr       - Pointer to a Data Descriptor
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GucRxBuff
**
** Functions invoked     : Eth_GetRxBuffAddr
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_122] */
/* TRACE[ETH_DDD_INF_062] */
static FUNC(void, ETH_CODE_SLOW) Eth_SetExtFempty
(
  CONSTP2VAR(Eth_ExtDataDescType, AUTOMATIC, ETH_APPL_DATA) pDescr,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection,
  uint8 LucBufIndex
)
{
  if (NULL_PTR != pDescr)
  {
    if (ETH_TX == LenDirection)
    {
      /* Error as TX Descriptor are always Standard and not extended*/
    }
    else if (ETH_BECHANNEL == LucQIndex)
    {
      /* RX BE */
      pDescr->Header.Dt   = ETH_DESC_FEMPTY;
      pDescr->Header.Die  = ETH_ZERO;
      pDescr->Header.Ctrl = ETH_ZERO;
      pDescr->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
      pDescr->Dptr = Eth_GetRxBuffAddr(LucQIndex, LucBufIndex);
      pDescr->Timestamp.Timestamp0 = ETH_ZERO_32;
      pDescr->Timestamp.Timestamp1 = ETH_ZERO_32;
      pDescr->Timestamp.Timestamp2 = ETH_ZERO_16;
    } /* (ETH_BECHANNEL == LucQIndex) */
    else if (ETH_NCCHANNEL == LucQIndex)
    {
      /* RX NC */
      pDescr->Header.Dt   = ETH_DESC_FEMPTY;
      pDescr->Header.Die  = ETH_ZERO;
      pDescr->Header.Ctrl = ETH_ZERO;
      pDescr->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
      pDescr->Dptr = Eth_GetRxBuffAddr(LucQIndex, LucBufIndex);
      pDescr->Timestamp.Timestamp0 = ETH_ZERO_32;
      pDescr->Timestamp.Timestamp1 = ETH_ZERO_32;
      pDescr->Timestamp.Timestamp2 = ETH_ZERO_16;
    } /* (ETH_NCCHANNEL == LucQIndex) */
    else
    {
      /* RX S*/
      pDescr->Header.Dt   = ETH_DESC_FEMPTY;
      pDescr->Header.Die  = ETH_ZERO;
      pDescr->Header.Ctrl = ETH_ZERO;
      pDescr->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
      pDescr->Dptr = Eth_GetRxBuffAddr(LucQIndex, LucBufIndex);
      pDescr->Timestamp.Timestamp0 = ETH_ZERO_32;
      pDescr->Timestamp.Timestamp1 = ETH_ZERO_32;
      pDescr->Timestamp.Timestamp2 = ETH_ZERO_16;
    } /* (ETH_NCCHANNEL == LucQIndex) */
  } /* (NULL_PTR != pDescr) */
  else
  {
    /* Cannot Set Fempty for NULL Address */
  }
}

/*******************************************************************************
** Function Name         : Eth_SetLinkFix
**
** Service ID            : NA
**
** Description           : Set LinkFix in the descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LinkDesc   - Pointer to Descriptor to be set
**                         AddrToLink - Link Address to be set
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_124] */
/* TRACE[ETH_DDD_INF_063] */
static FUNC(void, ETH_CODE_SLOW) Eth_SetLinkFix
(
  uint32 LinkDesc,
  uint32 AddrToLink
)
{
  P2VAR(Eth_LinkDescType, AUTOMATIC, ETH_APPL_DATA) LpLinkDesc;

  /* MISRA Violation: START Msg(4:0306)-2 */
  LpLinkDesc = (Eth_LinkDescType *) LinkDesc;
  /* MISRA Violation: END Msg(4:0306)-2 */

  /* MISRA Violation: START Msg(2:2814)-13 */
  LpLinkDesc->Header.Dt     = ETH_DESC_LINKFIX;
  /* MISRA Violation: END Msg(2:2814)-13 */
  LpLinkDesc->Header.Die    = ETH_ZERO;
  LpLinkDesc->Header.Res    = ETH_ZERO;
  LpLinkDesc->Dptr          = AddrToLink;

}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_DescUpdate
**
** Service ID            : NA
**
** Description           : Update descriptor chain (without Timestamp support)
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LulDescPtr - Pointer to descriptor to be updated
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF) || \
  (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
/* TRACE[ETH_DDD_ACT_064] */
/* TRACE[ETH_DDD_INF_064] */
static FUNC(void, ETH_CODE_FAST) Eth_DescUpdate
(
  uint32 LulDescPtr
)
{
  P2VAR(Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) LpDataDesc;

  /* MISRA Violation: START Msg(4:0306)-2 */
  LpDataDesc = (Eth_DataDescType *)LulDescPtr;
  /* MISRA Violation: END Msg(4:0306)-2 */

  /* Update descriptor */
  /* MISRA Violation: START Msg(2:2814)-13 */
  LpDataDesc->Header.Dt   = ETH_DESC_FEMPTY;
  /* MISRA Violation: END Msg(2:2814)-13 */
  LpDataDesc->Header.Die  = ETH_ZERO;
  LpDataDesc->Header.Ctrl = ETH_ZERO;
  LpDataDesc->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
}
#endif
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_DescTsUpdate
**
** Service ID            : NA
**
** Description           : Update descriptor chain (with Timestamp support).
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LulDescPtr - Pointer to descriptor to be updated
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_063] */
/* TRACE[ETH_DDD_INF_065] */
static FUNC(void, ETH_CODE_FAST) Eth_DescTsUpdate
(
  uint32 LulDescPtr
)
{
  P2VAR(Eth_ExtDataDescType, AUTOMATIC, ETH_APPL_DATA) LpExtDataDesc;

  /* MISRA Violation: START Msg(4:0306)-2 */
  LpExtDataDesc = (Eth_ExtDataDescType *)LulDescPtr;
  /* MISRA Violation: END Msg(4:0306)-2 */
  /* Update START or SINGLE frame */
  /* MISRA Violation: START Msg(2:2814)-13 */
  LpExtDataDesc->Header.Dt   = ETH_DESC_FEMPTY;
  /* MISRA Violation: END Msg(2:2814)-13 */
  LpExtDataDesc->Header.Die  = ETH_ZERO;
  LpExtDataDesc->Header.Ctrl = ETH_ZERO;
  LpExtDataDesc->Header.Ds   = ETH_CTRL_RX_BUF_LEN_BYTE;
  LpExtDataDesc->Timestamp.Timestamp0 = ETH_ZERO_32;
  LpExtDataDesc->Timestamp.Timestamp1 = ETH_ZERO_32;
  LpExtDataDesc->Timestamp.Timestamp2 = ETH_ZERO_16;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_ConfigureDMAC
**
** Service ID            : NA
**
** Description           : Configure EAVB-IF Direct Memory Access Controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx  - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LenReturnError - Error Type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstDescTable, Eth_GaaBaseAddr, Eth_GpConfigPtr,
**                         Eth_GstTxQueue, Eth_GstRxBeQueue, Eth_GstRxNcQueue,
**                         Eth_GstRxStreamQueue
**
** Functions invoked     : Eth_GetIncrementFromInputClock, Eth_CommonConfig,
**                         Eth_SetIncrementPTPTime, Eth_ErrIntMaskSet,
**                         Eth_RxSet, Eth_TxSet, Eth_DescTableSet,
**                         Dem_ReportErrorStatus
**
** Registers Used        : CSR, CCC, APSR, EIC, RCR, TGC, DBAT, FSO_CTL, EIL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_058] */
/* TRACE[ETH_DDD_INF_066] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_ConfigureDMAC
(
  uint8 LucCtrlIdx
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
  VAR(uint32, AUTOMATIC) LulIncrement;
  VAR(Std_ReturnType, AUTOMATIC) LddSetIncrementReturn;
  #endif

  /* MISRA Violation: START Msg(4:2982)-18 */
  LenReturnError = ETH_ERR_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddSetIncrementReturn = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* Set gPTP.TIC if TIMESTAMP is selected */
  LulIncrement = Eth_GetIncrementFromInputClock();

  /* Set Increment into HW */
  LddSetIncrementReturn =
    Eth_SetIncrementPTPTime(LucCtrlIdx, LulIncrement);
  /* MISRA Violation: START Msg(4:2995)-24 */
  /* MISRA Violation: START Msg(4:2991)-22 */
  if (E_OK != LddSetIncrementReturn)
  /* MISRA Violation: END Msg(4:2991)-22 */
  /* MISRA Violation: END Msg(4:2995)-24 */
  {
    /*Error on GTI Setting - DEM is required */
    Dem_ReportErrorStatus(ETH_E_TIMERINC_FAILED,
      DEM_EVENT_STATUS_FAILED);
    LenReturnError = ETH_ERR_NG;
  }
  else
  {
    /* No error */
    /* MISRA Violation: START Msg(4:2880)-15 */
    LenReturnError = ETH_ERR_OK;
    /* MISRA Violation: END Msg(4:2880)-15 */
  }

  /* MISRA Violation: START Msg(4:2996)-25 */
  /* MISRA Violation: START Msg(4:2992)-23 */
  if (ETH_ERR_OK == LenReturnError)
  /* MISRA Violation: END Msg(4:2992)-23 */
  /* MISRA Violation: END Msg(4:2996)-25 */
  {
  #endif /* (ETH_GLOBAL_TIME_SUPPORT == STD_ON) */
    /* MISRA Violation: START Msg(4:2880)-15 */
    Eth_CommonConfig(LucCtrlIdx);
    /* MISRA Violation: END Msg(4:2880)-15 */

    /* Configure Error interrupt mask*/
    LenReturnError = Eth_ErrIntMaskSet(LucCtrlIdx);

    if (ETH_ERR_OK == LenReturnError)
    {
      /* Configure reception part */
      LenReturnError = Eth_RxSet(LucCtrlIdx);
    }
    else
    {
      /* No action required */
    }
    if (ETH_ERR_OK == LenReturnError)
    {
      /* Configure transmission part */
      LenReturnError = Eth_TxSet(LucCtrlIdx);
    }
    else
    {
      /* No action required */
    }
    if (ETH_ERR_OK == LenReturnError)
    {
      /* Config Descriptor table */
      LenReturnError = Eth_DescTableSet(LucCtrlIdx,
        /* MISRA Violation: START Msg(4:0306)-2 */
        (uint32)(&Eth_GstDescTable[ETH_ZERO_8]));
        /* MISRA Violation: END Msg(4:0306)-2 */
    }
    else
    {
      /* No action required */
    }
  #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
  }
  else
  {
    /* Device structure not initialized to the HW Base Address*/
    LenReturnError = ETH_ERR_NOTLOCKED;
  }
  #endif /* (ETH_GLOBAL_TIME_SUPPORT == STD_ON) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_OpModeChange
**
** Service ID            : NA
**
** Description           : Change Operating Mode
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LenMode    - Mode to change
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GenDriverState,
**                         Eth_GpConfigPtr
**
** Functions invoked     : Eth_ReadRegister32,
**                         Eth_VerifyReadRegister32, Eth_WriteRegister32,
**                         Eth_VerifyWriteRegister32, Eth_OpModeGet
**
** Registers Used        : CCC, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_073] */
/* TRACE[ETH_DDD_INF_067] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_OpModeChange
(
  uint8 LucCtrlIdx,
  Eth_OpStatusType LenMode
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_CccType, AUTOMATIC) LunCcc;
  /* MISRA Violation: END Msg(4:0759)-10 */
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenReturnError = ETH_ERR_OK;
  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else
  {
    LenCurrentMode = Eth_OpModeGet(LucCtrlIdx);
    if (LenCurrentMode != LenMode)
    {
      /* TRACE[ETH_DDD_ACT_073_REG001] */
      LunCcc.Word =
        /* MISRA Violation: START Msg(4:2985)-20 */
        Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CCC);
        /* MISRA Violation: END Msg(4:2985)-20 */
      /* Register read-verify check */
      /* TRACE[ETH_DDD_ACT_073_REG009] */
      /* MISRA Violation: START Msg(4:2985)-20 */
      Eth_VerifyReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CCC,
      /* MISRA Violation: END Msg(4:2985)-20 */
        LunCcc.Word);

      /* MISRA Violation: START Msg(4:4424)-27 */
      LunCcc.Bit.Opc = LenMode;
      /* MISRA Violation: END Msg(4:4424)-27 */
      #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
      if ((ETH_STATE_UNINIT == Eth_GenDriverState) && (ETH_CONFIG == LenMode))
      {
        /* MISRA Violation: START Msg(4:4424)-27 */
        LunCcc.Bit.Gac = ETH_ENABLE;
        /* MISRA Violation: END Msg(4:4424)-27 */
        /* MISRA Violation: START Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:4424)-27 */
        LunCcc.Bit.Csel = Eth_GpConfigPtr->stComConfig.Csel;
        /* MISRA Violation: END Msg(4:4424)-27 */
        /* MISRA Violation: END Msg(2:2814)-13 */
      }
      else
      {
        /* No action required */
      }
      #endif
      /* TRACE[ETH_DDD_ACT_073_REG002] */
      /* MISRA Violation: START Msg(4:2985)-20 */
      Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      /* MISRA Violation: END Msg(4:2985)-20 */
        ETH_CCC, LunCcc.Word);
      if (LenMode != ETH_RESET)
      {
        /* Register write-verify check */
        /* TRACE[ETH_DDD_ACT_073_REG003] */
        /* MISRA Violation: START Msg(4:2985)-20 */
        Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        /* MISRA Violation: END Msg(4:2985)-20 */
          ETH_CCC, LunCcc.Word, ETH_REG_MASK);
      }
      else
      {
       /* No write verify checking in case of DMAC is reset */
      }
    } /* (LenCurrentMode != LenMode) */
    else
    {
      /* No HW Mode Changes as Macro is already in the requested state */
      /* No action required */
    }
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_DescTableBaseSet
**
** Service ID            : NA
**
** Description           : Set Descriptor table Base
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         ulBaseAddr - Base Address of Descriptor table
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_OpModeGet, Eth_WriteRegister32,
**                         Eth_VerifyWriteRegister32
**
** Registers Used        : DBAT, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_062] */
/* TRACE[ETH_DDD_INF_068] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_DescTableBaseSet
(
  uint8 LucCtrlIdx,
  uint32 ulBaseAddr
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenOpMode;
  LenOpMode = Eth_OpModeGet(LucCtrlIdx);
  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else if (ETH_CONFIG != LenOpMode)
  {
    LenReturnError = ETH_ERR_OPMODE;
  }
  else
  {
    /* TRACE[ETH_DDD_ACT_062_REG001] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_DBAT, ulBaseAddr);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_062_REG002] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_DBAT, ulBaseAddr, ETH_REG_MASK);

    LenReturnError = ETH_ERR_OK;
  }
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_CurrentDescGet
**
** Service ID            : NA
**
** Description           : Get Current Descriptor Address
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx   - Index of the ETH Controller
**                         LucQIndex    - Index of the Queue
**                                        (0..3 for Tx and 0..17 for Rx)
**                         LenDirection - Queue Direction (e.g. TX or RX)
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Descriptor Address
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32
**
** Registers Used        : CDARq q = 0,1...,21, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_059] */
/* TRACE[ETH_DDD_INF_070] */
static FUNC(uint32, ETH_CODE_FAST) Eth_CurrentDescGet
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
)
{
  VAR(uint32, AUTOMATIC) LulDescAddr;

  if (ETH_TX == LenDirection)
  {
    if ((ETH_TXQ_MAXNUM - ETH_ONE_8) < LucQIndex)
    {
      LulDescAddr = ETH_ZERO_32;
    } /* ((ETH_TXQ_MAXNUM - ETH_ONE_8) < LucQIndex) */
    else
    {
      /* TRACE[ETH_DDD_ACT_059_REG001] */
      LulDescAddr = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_CDARBASE + ((uint32)LucQIndex * ETH_FOUR_32));
    } /* ((ETH_TXQ_MAXNUM - ETH_ONE_8) < LucQIndex) */
  } /* (ETH_TX == LenDirection) */
  else
  {
    if ((ETH_RXQ_MAXNUM - ETH_ONE_8) < LucQIndex)
    {
      LulDescAddr = ETH_ZERO_32;
    }
    else
    {
      /* TRACE[ETH_DDD_ACT_059_REG002] */
      LulDescAddr = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_CDARBASE + (((uint32)LucQIndex + ETH_FOUR_32) * ETH_FOUR_32));
    }
  } /* (ETH_TX == LenDirection) */
  return LulDescAddr;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxSet
**
** Service ID            : NA
**
** Description           : Set Receive Configuration Structure
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32,
**                         Eth_OpModeGet
**
** Registers Used        : RCR, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_092] */
/* TRACE[ETH_DDD_INF_071] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxSet
(
  uint8 LucCtrlIdx
)
{
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_RcrType, AUTOMATIC) LunRcr;
  /* MISRA Violation: END Msg(4:0759)-10 */

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenReturnError = ETH_ERR_OK;
  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else
  {
    LenCurrentMode = Eth_OpModeGet(LucCtrlIdx);
    if (ETH_CONFIG == LenCurrentMode)
    {
      LunRcr.Word     = ETH_ZERO;
      /* MISRA Violation: START Msg(2:2814)-13 */
      LunRcr.Bit.Effs = Eth_GpConfigPtr->stRxConfig.Effs;
      /* MISRA Violation: END Msg(2:2814)-13 */
      LunRcr.Bit.Encf = Eth_GpConfigPtr->stRxConfig.Encf;
      /* MISRA Violation: START Msg(4:4424)-27 */
      LunRcr.Bit.Esf  = Eth_GpConfigPtr->stRxConfig.Esf;
      /* MISRA Violation: END Msg(4:4424)-27 */
      LunRcr.Bit.Ets0 = ETH_RX_BEQUEUE_TIMESTAMP;
      LunRcr.Bit.Ets2 = ETH_RX_SQUEUE_TIMESTAMP;
      LunRcr.Bit.Rfcl = ETH_RX_FIFO_CRIT_LVL;

      /* TRACE[ETH_DDD_ACT_092_REG001] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RCR, LunRcr.Word);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_092_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RCR, LunRcr.Word, ETH_REG_MASK);
    } /* (ETH_CONFIG == LenCurrentMode) */
    else
    {
      LenReturnError = ETH_ERR_OPMODE;
    }
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxQueueSet
**
** Service ID            : NA
**
** Description           : Set Receive Queue Configuration Structure
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LulQIndex - Queue Index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32,
**                         Eth_OpModeGet
**
** Registers Used        : RQCi i = 0,1,...,4, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_090] */
/* TRACE[ETH_DDD_INF_072] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxQueueSet
(
  uint8 LucCtrlIdx,
  uint32 LulQIndex
)
{
  VAR(uint32, AUTOMATIC) LulConfig;

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenReturnError = ETH_ERR_OK;
  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else
  {
    LenCurrentMode = Eth_OpModeGet(LucCtrlIdx);
    if ((ETH_CONFIG == LenCurrentMode) && ((uint32)ETH_RXQ_MAXNUM > LulQIndex))
    {
      /* MISRA Violation: START Msg(4:2984)-19 */
      LulConfig = (uint32)(((uint32)(ETH_RXNORMAL) <<
      /* MISRA Violation: END Msg(4:2984)-19 */
        /* MISRA Violation: START Msg(4:2984)-19 */
        /* MISRA Violation: START Msg(4:2984)-19 */
        ((LulQIndex % ETH_FOUR_32) * ETH_EIGHT_32)) | ((uint32)(ETH_MAXF0) <<
        /* MISRA Violation: END Msg(4:2984)-19 */
        /* MISRA Violation: END Msg(4:2984)-19 */
        /* MISRA Violation: START Msg(4:2984)-19 */
        (((LulQIndex % ETH_FOUR_32) * ETH_EIGHT_32)+ ETH_TWO_32)) |
        /* MISRA Violation: END Msg(4:2984)-19 */
        /* MISRA Violation: START Msg(4:2984)-19 */
        ((uint32)(ETH_UFCC0) <<
        /* MISRA Violation: END Msg(4:2984)-19 */
        (((LulQIndex % ETH_FOUR_32) * ETH_EIGHT_32) + ETH_FOUR_32)));

      /* TRACE[ETH_DDD_ACT_090_REG001] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_RQCBASE +
        ((LulQIndex / ETH_FOUR_32) * ETH_FOUR_32), LulConfig);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_090_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RQCBASE + ((LulQIndex / ETH_FOUR_32) * ETH_FOUR_32), LulConfig,
        ETH_REG_MASK);
    }/*
     * ((ETH_CONFIG == LenCurrentMode) &&
     * ((uint32)ETH_RXQ_MAXNUM > LulQIndex))
     */
    else
    {
      LenReturnError = ETH_ERR_OPMODE;
    }
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxPaddingSet
**
** Service ID            : NA
**
** Description           : Set Receive Padding
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LpPadding - Pointer to Rx padding Configuration
**                                     structure
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32,
**                         Eth_OpModeGet
**
** Registers Used        : RPC, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_088] */
/* TRACE[ETH_DDD_INF_073] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxPaddingSet
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(Eth_PaddingConfigType, AUTOMATIC, ETH_APPL_CONST) LpPadding
)
{
  VAR(uint32, AUTOMATIC) LulVal;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenReturnError = ETH_ERR_OK;
  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else
  {
    LenCurrentMode = Eth_OpModeGet(LucCtrlIdx);
    if (ETH_CONFIG == LenCurrentMode)
    {
      /* MISRA Violation: START Msg(2:2814)-13 */
      LulVal = (uint32)(((uint32)LpPadding->Dcnt << ETH_SIXTEEN) |
      /* MISRA Violation: END Msg(2:2814)-13 */
        ((uint32)LpPadding->Pcnt << ETH_EIGHT));

      /* TRACE[ETH_DDD_ACT_088_REG001] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_RPC, LulVal);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_088_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RPC, LulVal, ETH_REG_MASK);
    } /* (ETH_CONFIG == LenCurrentMode) */
    else
    {
      LenReturnError = ETH_ERR_OPMODE;
    }
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_UFCounterGet
**
** Service ID            : NA
**
** Description           : Get Unread Frame Counter
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LucQIndex - Queue Index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulCount - Number of unread frames
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32
**
** Registers Used        : UFCVi i = 0,1,...,4
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_098] */
/* TRACE[ETH_DDD_INF_074] */
FUNC(uint32, ETH_CODE_FAST) Eth_UFCounterGet(uint8 LucCtrlIdx, uint8 LucQIndex)
{
  VAR(uint32, AUTOMATIC) LulUfcv;
  VAR(uint32, AUTOMATIC) LulCount;

  /* TRACE[ETH_DDD_ACT_098_REG001] */
  LulUfcv = Eth_ReadRegister32((Eth_GaaBaseAddr[LucCtrlIdx]) +
    (ETH_UFCVBASE) + (uint8)(ETH_FOUR_8* (LucQIndex >> ETH_TWO_8)));

  LulCount = (uint32) ((LulUfcv >>
    (((uint32)LucQIndex % ETH_FOUR_32) * ETH_EIGHT_32)) & ETH_UFCV_CV_RANGE);

  return LulCount;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxDescChainUpdate
**
** Service ID            : NA
**
** Description           : Update receive descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LulDescPtr - Descriptor pointer
**                         LucQIdx - Channel Index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : Eth_DescUpdate, Eth_DescTsUpdate
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_084] */
/* TRACE[ETH_DDD_INF_076] */
FUNC(void, ETH_CODE_FAST) Eth_RxDescChainUpdate
(
  uint32 LulDescPtr,
  uint8 LucQIdx
)
{
  /* Update descriptor to FEmpty */
  if (ETH_BECHANNEL == LucQIdx)
  {
    #if (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
    Eth_DescUpdate(LulDescPtr);
    #else
    Eth_DescTsUpdate(LulDescPtr);
    #endif
  }
  else if (ETH_NCCHANNEL == LucQIdx)
  {
    Eth_DescTsUpdate(LulDescPtr);
  }
  else
  {
    #if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF)
    Eth_DescUpdate(LulDescPtr);
    #else
    Eth_DescTsUpdate(LulDescPtr);
    #endif
  }
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_TxSet
**
** Service ID            : NA
**
** Description           : Set Tx parameters
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32,
**                         Eth_OpModeGet
**
** Registers Used        : TGC, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_097] */
/* TRACE[ETH_DDD_INF_077] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_TxSet
(
  uint8 LucCtrlIdx
)
{
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_TgcType, AUTOMATIC) LunTgc;
  /* MISRA Violation: END Msg(4:0759)-10 */

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenReturnError = ETH_ERR_OK;
  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else
  {
    LenCurrentMode = Eth_OpModeGet(LucCtrlIdx);
    if (ETH_CONFIG == LenCurrentMode)
    {
      LunTgc.Word = ETH_ZERO;
      /* MISRA Violation: START Msg(4:4424)-27 */
      LunTgc.Bit.Tsm0 = ETH_TXNORMAL;
      /* MISRA Violation: END Msg(4:4424)-27 */
      /* MISRA Violation: START Msg(4:4424)-27 */
      LunTgc.Bit.Tsm1 = ETH_TXNORMAL;
      /* MISRA Violation: END Msg(4:4424)-27 */
      /* MISRA Violation: START Msg(4:4424)-27 */
      LunTgc.Bit.Tsm2 = ETH_TXNORMAL;
      /* MISRA Violation: END Msg(4:4424)-27 */
      /* MISRA Violation: START Msg(4:4424)-27 */
      LunTgc.Bit.Tsm3 = ETH_TXNORMAL;
      /* MISRA Violation: END Msg(4:4424)-27 */
      /* MISRA Violation: START Msg(2:2814)-13 */
      LunTgc.Bit.Ecbs = Eth_GpConfigPtr->stTxConfig.Ecbs;
      /* MISRA Violation: END Msg(2:2814)-13 */
      /* MISRA Violation: START Msg(4:4424)-27 */
      LunTgc.Bit.Tqp  = Eth_GpConfigPtr->stTxConfig.Tqp;
      /* MISRA Violation: END Msg(4:4424)-27 */
      LunTgc.Bit.Tbd0 = ETH_TWO;
      LunTgc.Bit.Tbd1 = ETH_TWO;
      LunTgc.Bit.Tbd2 = ETH_TWO;
      LunTgc.Bit.Tbd3 = ETH_TWO;

      /* TRACE[ETH_DDD_ACT_097_REG001] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_TGC, LunTgc.Word);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_097_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_TGC, LunTgc.Word, ETH_REG_MASK);
    } /* (ETH_CONFIG == LenCurrentMode) */
    else
    {
      LenReturnError = ETH_ERR_OPMODE;
    } /* (ETH_CONFIG == LenCurrentMode) */
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_TxReqStart
**
** Service ID            : NA
**
** Description           : Tx request start
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LulQIndex  - Queue index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32, Eth_WriteRegister32
**
** Registers Used        : TCCR
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_096] */
/* TRACE[ETH_DDD_INF_078] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_TxReqStart
(
  uint8 LucCtrlIdx,
  uint8 LulQIndex
)
{
  VAR(uint32, AUTOMATIC) LulTCCRRegVal;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else if ((uint32)ETH_THREE_8 < LulQIndex)
  {
    LenReturnError = ETH_ERR_NG;
  }
  else
  {
    /* TRACE[ETH_DDD_ACT_096_REG001] */
    LulTCCRRegVal = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_TCCR);
    /* Write 0 to Time Stamp FIFO Release bit (TCCR.TFR) and Transmit Start
    * Request bits (TCCR.TSRQn, n = 0,1,2,3) to avoid unintended request
    */
    LulTCCRRegVal &= ((~ETH_TCCR_TSRQ_MASK) & (~ETH_TCCR_TFR_MASK));
    /* Issue a request to start transmission for corresponding queue */
    LulTCCRRegVal |= (uint32)((uint32)ETH_ONE_8 << LulQIndex);

    /* TRACE[ETH_DDD_ACT_096_REG003] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_TCCR, LulTCCRRegVal);
    LenReturnError = ETH_ERR_OK;
  } /* ((uint32)ETH_THREE_8 < LulQIndex) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_ErrIntMaskSet
**
** Service ID            : NA
**
** Description           : Set error interrupt mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32,
**                         Eth_SetBitRegister32
**
** Registers Used        : EIC, FSO_CTL, EIL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_067] */
/* TRACE[ETH_DDD_INF_079] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_ErrIntMaskSet
(
  uint8 LucCtrlIdx
)
{
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_EicType, AUTOMATIC) LunEic;
  /* MISRA Violation: END Msg(4:0759)-10 */

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  else
  {
    LunEic.Word = ETH_ZERO;
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Mree  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Mtee  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Qee   = ETH_ENABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.See   = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Clle0 = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Clle1 = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Cule0 = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Cule1 = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Tffe  = ETH_ENABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */
    /* MISRA Violation: START Msg(4:4424)-27 */
    LunEic.Bit.Mffe  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-27 */

    /* TRACE[ETH_DDD_ACT_067_REG001] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_EIC, LunEic.Word);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_067_REG002] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_EIC, LunEic.Word, ETH_REG_MASK);

    /* Use Interrupt line B for Timestamp FIFO Full interrupt */
    Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_EIL,
      ETH_EIL_TFFL_BIT);

    /* Register write-verify check */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_EIL,
      (ETH_BIT0_SET_32 << ETH_EIL_TFFL_BIT),
        (ETH_BIT0_SET_32 << ETH_EIL_TFFL_BIT));

    LenReturnError = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxIntMaskSet
**
** Service ID            : NA
**
** Description           : Set Rx Interrupt Mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Functions invoked     : Eth_SetBitRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : CIE, RIC0, FSO_CTL
**
*******************************************************************************/
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_085] */
/* TRACE[ETH_DDD_INF_080] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxIntMaskSet(uint8 LucCtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucBitNo;

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  else
  {
    /* Enable Common Receive Interrupt */
    /* TRACE[ETH_DDD_ACT_085_REG001] */
    Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CIE,
      ETH_CIE_CRIE_BIT);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_085_REG002] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_CIE, ETH_BIT0_SET_32, ETH_BIT0_SET_32);

    for (LucCount = ETH_ZERO_8;
      /* MISRA Violation: START Msg(2:2814)-13 */
      LucCount < Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfRxQueue; LucCount++)
      /* MISRA Violation: END Msg(2:2814)-13 */
    {
      /* MISRA Violation: START Msg(4:0491)-7 */
      LucBitNo = Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucCount].id;
      /* MISRA Violation: END Msg(4:0491)-7 */
      /* TRACE[ETH_DDD_ACT_085_REG003] */
      Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RIC0, LucBitNo);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_085_REG004] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RIC0, (ETH_BIT0_SET_32 << LucBitNo), (ETH_BIT0_SET_32 << LucBitNo));

    }
    LenReturnError = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#endif /* #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_RxWarningIntMaskSet
**
** Service ID            : NA
**
** Description           : Set Rx Warning Interrupt Mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LulMask    - Mask Value
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : RIC1, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_093] */
/* TRACE[ETH_DDD_INF_082] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxWarningIntMaskSet
(
  uint8 LucCtrlIdx,
  uint32 LulMask
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  }
  else
  {
    /* TRACE[ETH_DDD_ACT_093_REG001] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_RIC1, LulMask & ETH_RXQ_INT_MASK);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_093_REG002] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_RIC1, (LulMask & ETH_RXQ_INT_MASK), ETH_RXQ_INT_MASK);

    LenReturnError = ETH_ERR_OK;
  }
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxQFullIntMaskSet
**
** Service ID            : NA
**
** Description           : Set Rx queue full interrupt mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Functions invoked     : Eth_SetBitRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : RIC2, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_089] */
/* TRACE[ETH_DDD_INF_083] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxQFullIntMaskSet(uint8 LucCtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucBitNo;

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  else
  {
    for (LucCount = ETH_ZERO_8;
      /* MISRA Violation: START Msg(2:2814)-13 */
      LucCount < Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfRxQueue; LucCount++)
      /* MISRA Violation: END Msg(2:2814)-13 */
    {
      /* MISRA Violation: START Msg(4:0491)-7 */
      LucBitNo = Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucCount].id;
      /* MISRA Violation: END Msg(4:0491)-7 */

      /* TRACE[ETH_DDD_ACT_089_REG001] */
      Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RIC2, LucBitNo);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_089_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_RIC2, (ETH_BIT0_SET_32 << LucBitNo), (ETH_BIT0_SET_32 << LucBitNo));
    }

    /* Set RIC2.RFFE in any case  */
    /* TRACE[ETH_DDD_ACT_089_REG003] */
    Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_RIC2,
      ETH_RIC2_RFFE_BIT);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_089_REG004] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_RIC2, ETH_BIT31_SET_32, ETH_BIT31_SET_32);

    LenReturnError = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_TxIntMaskSet
**
** Service ID            : NA
**
** Description           : Set Tx Interrupt Mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Functions invoked     : Eth_SetBitRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : CIE, TIC, FSO_CTL
**
*******************************************************************************/
#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_094] */
/* TRACE[ETH_DDD_INF_084] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_TxIntMaskSet(uint8 LucCtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucBitNo;

  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  else
  {
    /* Enable Common Transmit Interrupt */
    /* TRACE[ETH_DDD_ACT_094_REG007] */
    Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CIE,
      ETH_CIE_CTIE_BIT);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_094_REG008] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_CIE, ETH_BIT8_SET_32, ETH_BIT8_SET_32);

    for (LucCount = ETH_ZERO_8;
      /* MISRA Violation: START Msg(2:2814)-13 */
      LucCount < Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfTxQueue; LucCount++)
      /* MISRA Violation: END Msg(2:2814)-13 */
    {
      /* MISRA Violation: START Msg(4:0491)-7 */
      LucBitNo = Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].id +
      /* MISRA Violation: END Msg(4:0491)-7 */
        ETH_TIC_TDPE_OFFSET;

      /* TRACE[ETH_DDD_ACT_094_REG001] */
      Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_TIC, LucBitNo);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_094_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TIC,
        (ETH_BIT0_SET_32 << LucBitNo), (ETH_BIT0_SET_32 << LucBitNo));
    }

    LenReturnError = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#endif /* #if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)*/

/*******************************************************************************
** Function Name         : Eth_GPtpIntMaskSet
**
** Service ID            : NA
**
** Description           : Set gPTP interrupt mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LulMask    - Mask Value
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : GIC, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_068] */
/* TRACE[ETH_DDD_INF_086] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_GPtpIntMaskSet
(
  uint8 LucCtrlIdx,
  uint32 LulMask
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  else
  {
    /* TRACE[ETH_DDD_ACT_068_REG001] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_GIC, LulMask & (uint8)ETH_SEVEN_8);
    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_068_REG002] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_GIC, LulMask & (uint8)ETH_SEVEN_8, ETH_SEVEN_8);

    LenReturnError = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_CommonLineIntMaskSet
**
** Service ID            : NA
**
** Description           : Set Common Line 0 Interrupt Mask
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_SetBitRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : CIE
**
*******************************************************************************/
#if ((ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) || \
  (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON))
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_091] */
/* TRACE[ETH_DDD_INF_087] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_CommonLineIntMaskSet(uint8 LucCtrlIdx)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnError = ETH_ERR_RANGE;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  else
  {
    /* Use queue specific interrupt line 0 */
    Eth_SetBitRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CIE,
      ETH_CIE_CL0M_BIT);

    /* Register write-verify check */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CIE,
      (ETH_BIT0_SET_32 << ETH_CIE_CL0M_BIT),
      (ETH_BIT0_SET_32 << ETH_CIE_CL0M_BIT));

    LenReturnError = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#endif /* #if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)*/

/*******************************************************************************
** Function Name         : Eth_DescTableSet
**
** Service ID            : NA
**
** Description           : Setting descriptor table
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         ulBaseAddr - Base Address of Descriptor table
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - Error type
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GpConfigPtr, Eth_GstRxStreamQueue
**                         Eth_GstTxQueue, Eth_GstRxBeQueue, Eth_GstRxNcQueue
**
** Functions invoked     : Eth_FillDescMemory, Eth_IsQueueConfigured
**                         Eth_GetQueueAddr, Eth_DescTableBaseSet
**
** Registers Used        : DBAT, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_172] */
/* TRACE[ETH_DDD_INF_088] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_DescTableSet
(
  uint8 LucCtrlIdx,
  uint32 ulBaseAddr
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;

  VAR(Eth_DirectionType, AUTOMATIC) LenDirection;
  VAR(uint8, AUTOMATIC) LucQIndex;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint32, AUTOMATIC) LulDescAddr;
  P2VAR(Eth_LinkDescType, AUTOMATIC, ETH_APPL_DATA) LpLinkDesc;
  VAR(boolean, AUTOMATIC) LblReturnVal;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LblReturnVal = ETH_FALSE;
  /* MISRA Violation: END Msg(4:2982)-18 */

  /* MISRA Violation: START Msg(4:0306)-2 */
  LpLinkDesc = (Eth_LinkDescType *)(ulBaseAddr);
  /* MISRA Violation: END Msg(4:0306)-2 */

  LenReturnError = ETH_ERR_OK;

  for (LucCount = ETH_ZERO_8;
    ((uint8)ETH_TXQ_MAXNUM + (uint8)ETH_RXQ_MAXNUM) > LucCount; LucCount++)
  {
    /* Index need to be reduced by 4 for a RX Queue */
    if (ETH_TXQ_MAXNUM > LucCount)
    {
      /* TX Queue */
      LenDirection = ETH_TX;
      LucQIndex = LucCount;
    }
    else
    {
      /* Rx Queue */
      LenDirection = ETH_RX;
      LucQIndex = LucCount - ETH_TXQ_MAXNUM;
    }
    LblReturnVal = Eth_IsQueueConfigured(LucQIndex, LenDirection);
    if (ETH_TRUE == LblReturnVal)
    {
      /* Queue has been configured */
      /* Get address of corresponding Queue */
      LulDescAddr = Eth_GetQueueAddr(LucQIndex, LenDirection);
      /* MISRA Violation: START Msg(4:2995)-24 */
      /* MISRA Violation: START Msg(4:2991)-22 */
      /* MISRA Violation: START Msg(4:0306)-2 */
      if ((uint32)NULL_PTR != LulDescAddr)
      /* MISRA Violation: END Msg(4:0306)-2 */
      /* MISRA Violation: END Msg(4:2991)-22 */
      /* MISRA Violation: END Msg(4:2995)-24 */
      {
        Eth_FillDescMemory(LpLinkDesc, LulDescAddr);
      }
      else
      {
        /* Can not get address of Rx Queue */
        /* MISRA Violation: START Msg(4:2880)-15 */
        LenReturnError = ETH_ERR_NG;
        /* MISRA Violation: END Msg(4:2880)-15 */
        break;
      }
    }
    else
    {
      /* Queue not configured */
      /* Set Null Dptr field in Link Descriptor*/
      Eth_FillDescMemory(LpLinkDesc, ETH_ZERO_32);
    }

    /* Go to the next DBA element */
    /* MISRA Violation: START Msg(2:2824)-14 */
    /* MISRA Violation: START Msg(4:0489)-6 */
    LpLinkDesc++;
    /* MISRA Violation: END Msg(4:0489)-6 */
    /* MISRA Violation: END Msg(2:2824)-14 */
  } /* End loop for all Tx and Rx Queues  */

  /* MISRA Violation: START Msg(4:2995)-24 */
  /* MISRA Violation: START Msg(4:2991)-22 */
  if (ETH_ERR_OK == LenReturnError)
  /* MISRA Violation: END Msg(4:2991)-22 */
  /* MISRA Violation: END Msg(4:2995)-24 */
  {
    LenReturnError = Eth_DescTableBaseSet(LucCtrlIdx, ulBaseAddr);
  }
  else
  {
    /* Can not set up descriptor table because error happened */
  }

  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_GetQueueAddr
**
** Service ID            : NA
**
** Description           : Get Queue address
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucQIndex    - Queue index
**                         LenDirection - Queue direction
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Address of corresponding Queue
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstTxQueue, Eth_GstRxBeQueue, Eth_GstRxNcQueue,
**                         Eth_GstRxStreamQueue
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_173] */
/* TRACE[ETH_DDD_INF_090] */
static FUNC(uint32, ETH_CODE_SLOW) Eth_GetQueueAddr
(
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
)
{
  VAR(uint32, AUTOMATIC) LulDescAddr;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LulDescAddr = ETH_ZERO_32;
  /* MISRA Violation: END Msg(4:2982)-18 */
  if (ETH_TX == LenDirection)
  {
    /* TX */
    #if (ETH_TX_QUEUE_COUNT != ETH_QUEUE_SIZE_ZERO)
    if (LucQIndex < ETH_TX_QUEUE_COUNT)
    {
      /* MISRA Violation: START Msg(4:0306)-2 */
      LulDescAddr = (uint32)&Eth_GstTxQueue[LucQIndex];
      /* MISRA Violation: END Msg(4:0306)-2 */
    }
    else
    {
      /* MISRA Violation: START Msg(4:0306)-2 */
      LulDescAddr = (uint32)NULL_PTR;
      /* MISRA Violation: END Msg(4:0306)-2 */
    }
    #endif
  }
  else
  {
    /* Rx */
    if (ETH_BECHANNEL == LucQIndex)
    {
      /* RX BE */
      /* MISRA Violation: START Msg(4:0306)-2 */
      LulDescAddr = (uint32)&Eth_GstRxBeQueue[ETH_ZERO];
      /* MISRA Violation: END Msg(4:0306)-2 */
    }
    else if (ETH_NCCHANNEL == LucQIndex)
    {
      /* RX NC */
      #if (ETH_RX_NCQUEUE_NUM != ETH_QUEUE_SIZE_ZERO)
      /* MISRA Violation: START Msg(4:0306)-2 */
      LulDescAddr = (uint32)&Eth_GstRxNcQueue[ETH_ZERO];
      /* MISRA Violation: END Msg(4:0306)-2 */
      #endif
    }
    else
    {
      /* RX Stream */
      #if (ETH_RX_SQUEUE_NUM != ETH_QUEUE_SIZE_ZERO)
      if ((LucQIndex - ETH_RXSQ_ID_OFFSET) < ETH_RX_SQUEUE_NUM)
      {
        LulDescAddr =
          /* MISRA Violation: START Msg(4:0306)-2 */
          (uint32)&Eth_GstRxStreamQueue[LucQIndex - ETH_RXSQ_ID_OFFSET];
          /* MISRA Violation: END Msg(4:0306)-2 */
      }
      else
      {
        /* MISRA Violation: START Msg(4:0306)-2 */
        LulDescAddr = (uint32)NULL_PTR;
        /* MISRA Violation: END Msg(4:0306)-2 */
      }
      #endif
    }
  }
  return LulDescAddr;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_FillDescMemory
**
** Service ID            : NA
**
** Description           : Set LinkFix in the descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : DescAddr   - Address pointed by descriptor pointer
**
** InOut Parameters      : pLinkDescr - Pointer to descriptor
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_133] */
/* TRACE[ETH_DDD_INF_091] */
static FUNC(void, ETH_CODE_SLOW) Eth_FillDescMemory
(
  CONSTP2VAR(Eth_LinkDescType, AUTOMATIC, ETH_APPL_DATA) pLinkDescr,
  uint32 DescAddr
)
{
  if (NULL_PTR != pLinkDescr)
  {
    pLinkDescr->Header.Dt  = ETH_DESC_LINKFIX;
    pLinkDescr->Header.Die = ETH_ZERO;
    pLinkDescr->Header.Res = ETH_ZERO;
    pLinkDescr->Dptr       = DescAddr;
  }
  else
  {
    /* No action at this level*/
  }
}

/*******************************************************************************
** Function Name         : Eth_IsQueueConfigured
**
** Service ID            : NA
**
** Description           : Use the Binary Search Algorithm to discover if
**                         a Queue is configured and return its Direction;
**                         otherwise return false and Direction is irrelevant
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucQIndex    - Queue index
**                         LenDirection - Queue Direction (e.g. TX or RX)
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Queue configured status
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GpConfigPtr
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_095] */
/* TRACE[ETH_DDD_INF_092] */
static FUNC(boolean, ETH_CODE_SLOW) Eth_IsQueueConfigured
(
  uint8 LucQIndex,
  Eth_DirectionType LenDirection
)
{
  /* Last Item Index */
  VAR(uint8, AUTOMATIC) LucLastVal;
  /* Middle Item Index */
  VAR(uint8, AUTOMATIC) LucMidVal;
  /* First Item Index */
  VAR(uint8, AUTOMATIC) LucFirstVal;
  /* Return value */
  VAR(boolean, AUTOMATIC) LblReturnVal;

  LblReturnVal = ETH_FALSE;

  if (ETH_TX == LenDirection)
  {
    /* Tx Queues */
    LucFirstVal = ETH_ZERO_8;
    /* MISRA Violation: START Msg(2:2814)-13 */
    LucLastVal = Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfTxQueue - ETH_ONE_8;
    /* MISRA Violation: END Msg(2:2814)-13 */

    if (LucFirstVal < LucLastVal)
    {
      do
      {
        LucMidVal = LucFirstVal + ((LucLastVal - LucFirstVal) >> ETH_ONE);
        if (LucQIndex <
          /* MISRA Violation: START Msg(4:0491)-7 */
          Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucMidVal].id)
          /* MISRA Violation: END Msg(4:0491)-7 */
        {
          /* Lower Part of the Array */
          LucLastVal = LucMidVal - ETH_ONE_8;
        }
        else if (LucQIndex >
          /* MISRA Violation: START Msg(4:0491)-7 */
          Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucMidVal].id)
          /* MISRA Violation: END Msg(4:0491)-7 */
        {

          /* Upper Part of the Array */
          LucFirstVal = LucMidVal + ETH_ONE_8;
        }
        else
        {
          /* Search is done  */
          LblReturnVal = ETH_TRUE;
        }
      } while ((ETH_FALSE == LblReturnVal) &&
        (LucFirstVal < LucLastVal) && (ETH_ZERO_8 != LucMidVal));

      if ((ETH_FALSE == LblReturnVal) && (LucFirstVal == LucLastVal))
      {
        if (LucQIndex ==
          /* MISRA Violation: START Msg(4:0491)-7 */
          Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucFirstVal].id)
          /* MISRA Violation: END Msg(4:0491)-7 */
        {
          LblReturnVal = ETH_TRUE;
        }
        else
        {
          /* Do nothing */
        }
      }
      else
      {
        /* Do nothing */
      }
    } /* (LucFirstVal < LucLastVal) */
    else
    {
      if (LucQIndex ==
        /* MISRA Violation: START Msg(4:0491)-7 */
        Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucFirstVal].id)
        /* MISRA Violation: END Msg(4:0491)-7 */
      {
        LblReturnVal = ETH_TRUE;
      }
      else
      {
        /* Do nothing */
      }
    } /* (LucFirstVal < LucLastVal) */
  } /* (ETH_TX == LenDirection) */
  else
  {
    /* Rx Queues */
    LucFirstVal = ETH_ZERO_8;
    LucLastVal = Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfRxQueue - ETH_ONE_8;

    if (LucFirstVal < LucLastVal)
    {
      do
      {
        LucMidVal = LucFirstVal + ((LucLastVal - LucFirstVal) >> ETH_ONE);
        if (LucQIndex <
          /* MISRA Violation: START Msg(4:0491)-7 */
          Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucMidVal].id)
          /* MISRA Violation: END Msg(4:0491)-7 */
        {
          /* Lower Part of the Array */
          LucLastVal = LucMidVal - ETH_ONE_8;
        }
        else if (LucQIndex >
          /* MISRA Violation: START Msg(4:0491)-7 */
          Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucMidVal].id)
          /* MISRA Violation: END Msg(4:0491)-7 */
        {

          /* Upper Part of the Array  */
          LucFirstVal = LucMidVal + ETH_ONE_8;
        }
        else
        {
          /* Search is done  */
          LblReturnVal = ETH_TRUE;
        }
      }
      while ((ETH_FALSE == LblReturnVal) &&
        (LucFirstVal < LucLastVal) && (ETH_ZERO_8 != LucMidVal));
      if ((ETH_FALSE == LblReturnVal) && (LucFirstVal == LucLastVal))
      {
        if (LucQIndex ==
          /* MISRA Violation: START Msg(4:0491)-7 */
          Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucFirstVal].id)
          /* MISRA Violation: END Msg(4:0491)-7 */
        {
          LblReturnVal = ETH_TRUE;
        }
        else
        {
          /* Do nothing */
        }
      }
      else
      {
        /* Do nothing */
      }
    } /* (LucFirstVal < LucLastVal) */
    else
    {
      if (LucQIndex ==
        /* MISRA Violation: START Msg(4:0491)-7 */
        Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucFirstVal].id)
        /* MISRA Violation: END Msg(4:0491)-7 */
      {
        LblReturnVal = ETH_TRUE;
      }
      else
      {
        /* Do nothing */
      }
    } /* (LucFirstVal < LucLastVal) */
  } /* (ETH_TX == LenDirection) */
  return LblReturnVal;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_DisableController
**
** Service ID            : NA
**
** Description           : Disable the indexed Ethernet Controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
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
** Functions invoked     : Eth_MacCommunicationSet
**
** Registers Used        : ECMR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_065] */
/* TRACE[ETH_DDD_INF_093] */
FUNC(void, ETH_CODE_SLOW) Eth_DisableController(uint8 CtrlIdx)
{
  /* Disable Receive */
  Eth_MacCommunicationSet(CtrlIdx, ETH_RX, ETH_DISABLE);
  /* Disable Transmit*/
  Eth_MacCommunicationSet(CtrlIdx, ETH_TX, ETH_DISABLE);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_EnableController
**
** Service ID            : NA
**
** Description           : Enable the indexed Ethernet Controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
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
** Functions invoked     : Eth_MacCommunicationSet
**
** Registers Used        : ECMR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_066] */
/* TRACE[ETH_DDD_INF_094] */
FUNC(void, ETH_CODE_SLOW) Eth_EnableController(uint8 CtrlIdx)
{
  /* Enable Rx and Tx in the same way done in Eth_ControllerInit */
  Eth_MacCommunicationSet(CtrlIdx, ETH_RX, ETH_ENABLE);
  Eth_MacCommunicationSet(CtrlIdx, ETH_TX, ETH_ENABLE);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/*******************************************************************************
** Function Name         : Eth_SetRxFilter
**
** Service ID            : NA
**
** Description           : Configure SFO and SFM0, SFM1 registers
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
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
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : SFO and SFM0, SFM1, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_114] */
/* TRACE[ETH_DDD_INF_096] */
FUNC(void, ETH_CODE_SLOW) Eth_SetRxFilter( VAR(uint8, AUTOMATIC) CtrlIdx)
{
  /* TRACE[ETH_DDD_ACT_114_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_SFO, ETH_ZERO_32);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_114_REG002] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_SFO,
    ETH_ZERO_32, ETH_REG_MASK);

  /* TRACE[ETH_DDD_ACT_114_REG003] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
    ETH_SFM0, ETH_REG_MASK);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_114_REG004] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_SFM0,
    ETH_REG_MASK, ETH_REG_MASK);

  /* TRACE[ETH_DDD_ACT_114_REG005] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_SFM1,
    ETH_SFM1_REG_MASK);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_114_REG006] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_SFM1,
    ETH_SFM1_REG_MASK, ETH_REG_MASK);
}

/*******************************************************************************
** Function Name         : Eth_WriteIntoSFPReg
**
** Service ID            : NA
**
** Description           : Write In corresponding SFPi the passed address (for
**                         Rx Filter)
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx   - Index of the ETH Controller
**                         LpMacAddrPtr - Pointer to the address to be stored
**                         ucQueueIdx   - In case of clear this is the index of
**                                        the SFP register minus offset
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : AVB shall be in Configuration Mode
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : SFPi i = 0, 1..31, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_117] */
/* TRACE[ETH_DDD_INF_097] */
FUNC(void, ETH_CODE_SLOW) Eth_WriteIntoSFPReg
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) LpMacAddrPtr,
  VAR(uint8, AUTOMATIC) ucQueueIdx
)
{
  /* Store Register Address for SFP 0, 2, 4,... 30 */
  VAR(uint32, AUTOMATIC) LulSFPEven;

  /* Store Register Address for SFP 1, 3, 5,... 31 */
  VAR(uint32, AUTOMATIC) LulSFPOdd;

  VAR(uint32, AUTOMATIC) LulTempReg;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LulTempReg = ETH_ZERO_32;
  /* MISRA Violation: END Msg(4:2982)-18 */

  LulSFPEven = (Eth_GaaBaseAddr[LucCtrlIdx] + ETH_SFPBASE +
    (uint32)(uint8)(ETH_FOUR_8 * ((uint8)(ucQueueIdx << ETH_ONE_8) -
      ETH_FOUR_8)));

  LulSFPOdd = (Eth_GaaBaseAddr[LucCtrlIdx] + ETH_SFPBASE +
    (uint32)(uint8)(ETH_FOUR_8 * ((uint8)(ucQueueIdx << ETH_ONE_8) -
      ETH_THREE_8)));

    /* Little Endian Core */
    /* Write into SFP 1,3,...31 */
    /* MISRA Violation: START Msg(2:2824)-14 */
    /* MISRA Violation: START Msg(4:0492)-8 */
    LulTempReg = ((uint32)LpMacAddrPtr[ETH_FIVE] << ETH_EIGHT) |
    /* MISRA Violation: END Msg(4:0492)-8 */
    /* MISRA Violation: END Msg(2:2824)-14 */
      /* MISRA Violation: START Msg(4:0492)-8 */
      ((uint32)LpMacAddrPtr[ETH_FOUR]);
      /* MISRA Violation: END Msg(4:0492)-8 */
    /* TRACE[ETH_DDD_ACT_117_REG001] */
    Eth_WriteRegister32(LulSFPOdd, LulTempReg);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_117_REG002] */
    Eth_VerifyWriteRegister32(LulSFPOdd, LulTempReg, ETH_REG_MASK);

    /* Write into SFP 0,2,..30 */
    /* MISRA Violation: START Msg(4:0492)-8 */
    LulTempReg = ((uint32)LpMacAddrPtr[ETH_THREE] << ETH_TWENTYFOUR) |
    /* MISRA Violation: END Msg(4:0492)-8 */
      /* MISRA Violation: START Msg(4:0492)-8 */
      ((uint32)LpMacAddrPtr[ETH_TWO] << ETH_SIXTEEN) |
      /* MISRA Violation: END Msg(4:0492)-8 */
      /* MISRA Violation: START Msg(4:0492)-8 */
      ((uint32)LpMacAddrPtr[ETH_ONE] << ETH_EIGHT)  |
      /* MISRA Violation: END Msg(4:0492)-8 */
      /* MISRA Violation: START Msg(4:0492)-8 */
      ((uint32)LpMacAddrPtr[ETH_ZERO]);
      /* MISRA Violation: END Msg(4:0492)-8 */
    /* TRACE[ETH_DDD_ACT_117_REG003] */
    Eth_WriteRegister32(LulSFPEven, LulTempReg);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_117_REG004] */
    Eth_VerifyWriteRegister32(LulSFPEven, LulTempReg, ETH_REG_MASK);
}

/*******************************************************************************
** Function Name         : Eth_SetIncrementPTPTime
**
** Service ID            : NA
**
** Description           : Set a value to GTI and issue a load request
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx      - Index of the ETH Controller
**                         LulIncrement - Value by which the timer is increased
**                                        each time a cycle of that clock signal
**                                        elapses
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WaitPTPRequestIsComplete, Eth_ReadRegister32,
**                         Eth_VerifyReadRegister32, Eth_WriteRegister32,
**                         Eth_VerifyWriteRegister32, Eth_SetBitRegister32
**
** Registers Used        : GCCR, GTI, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_111] */
/* TRACE[ETH_DDD_INF_098] */
#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetIncrementPTPTime
(
  uint8 CtrlIdx,
  uint32 LulIncrement
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulVal;

  if (GTI_MIN_VALUE <= LulIncrement)
  {
    /* TRACE[ETH_DDD_ACT_111_REG001] */
    LulVal = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR);

    if (ETH_ZERO_32 == (LulVal & ETH_GPTP_CAPTURE_MASK))
    {
      /* No Active Request */
      /* Set GTI */
      /* TRACE[ETH_DDD_ACT_111_REG005] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_GTI, LulIncrement);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_111_REG003] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_GTI, LulIncrement, ETH_REG_MASK);

      /* Set Request to load GTI */
      /* TRACE[ETH_DDD_ACT_111_REG004] */
      Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
        ETH_GPTP_LTI_BIT);
      LddStdReturn = Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_GPTP_LTI_BIT);
    } /* (ETH_ZERO_32 == (LulVal & ETH_GPTP_CAPTURE_MASK)) */
    else
    {
      /* Another Request is pending to gPTP */
      LddStdReturn = E_NOT_OK;
    } /* (ETH_ZERO_32 == (LulVal & ETH_GPTP_CAPTURE_MASK)) */
  } /* (GTI_MIN_VALUE <= LulIncrement) */
  else
  {  /* Wrong AVB State */
    LddStdReturn = E_NOT_OK;
  }
  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_ResetPTPTime
**
** Service ID            : NA
**
** Description           : Issue a reset to the gPTP Counter
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WaitPTPRequestIsComplete, Eth_ReadRegister32,
**                         Eth_OpModeGet, Eth_SetBitRegister32
**
** Registers Used        : GCCR, CSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_109] */
/* TRACE[ETH_DDD_INF_099] */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ResetPTPTime(uint8 CtrlIdx)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulValue;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete0;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete1;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenCurrentMode = Eth_OpModeGet(CtrlIdx);

  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete0 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete1 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  if (ETH_OPERATION == LenCurrentMode)
  {
    /* TRACE[ETH_DDD_ACT_109_REG001] */
    LulValue = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR);
    if (ETH_ZERO_32 == (LulValue & ETH_GPTP_CAPTURE_MASK))
    {
      /* No Pending Timer Request */
      /* Issue a request to reset gPTP */

      /* Set GCCR.TCR = 01*/
      /* TRACE[ETH_DDD_ACT_109_REG002] */
      Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
        ETH_GCCR_TCR_BIT0);

      /* Wait for GCCR.TCR = 00 */
      LddPTPRequestComplete0 =
        Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ZERO_8);
      LddPTPRequestComplete1 =
        Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ONE_8);
      if ((E_OK == LddPTPRequestComplete0) && (E_OK == LddPTPRequestComplete1))
      {
        /* Reset Terminated Successfully */
        LddStdReturn = E_OK;
      }
      else
      {
        /* Reset not terminated */
        LddStdReturn = E_NOT_OK;
      }
    } /* (ETH_ZERO_32 == (LulValue & ETH_GPTP_CAPTURE_MASK)) */
    else
    {
      /* Pending Timer Request */
      LddStdReturn = E_NOT_OK;
    } /* (ETH_ZERO_32 == (LulValue & ETH_GPTP_CAPTURE_MASK)) */
  } /* (ETH_OPERATION == LenCurrentMode) */
  else
  {
    LddStdReturn = E_NOT_OK;
  }

  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_GetPTPTime
**
** Service ID            : NA
**
** Description           : Return non-corrected TimeStamp (TCSS = 00) from the
**                         HW register previously started
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : timeQualPtr - Quality of the returned HW TimeStamp
**                         timeStampPtr - Current TimeStamp value
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WaitPTPRequestIsComplete, Eth_ReadRegister32
**                         Eth_VerifyReadRegister32, Eth_OpModeGet
**                         Eth_WriteRegister32, Eth_ReadTSFromAnAddr
**
** Registers Used        : GCCR, CCC, GCTi i = 0,1,2, CSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_102] */
/* TRACE[ETH_DDD_INF_100] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetPTPTime
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulGccrReg;
  VAR(uint32, AUTOMATIC) LulCccReg;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete0;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete1;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;

  LenCurrentMode = Eth_OpModeGet(CtrlIdx);

  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete0 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete1 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */

  if (ETH_OPERATION == LenCurrentMode)
  {
    /* Read Csel */
    /* TRACE[ETH_DDD_ACT_102_REG001] */
    /* MISRA Violation: START Msg(4:2985)-20 */
    LulCccReg = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CCC);
    /* MISRA Violation: END Msg(4:2985)-20 */
    /* Register read-verify check */
    /* TRACE[ETH_DDD_ACT_102_REG003] */
    /* MISRA Violation: START Msg(4:2985)-20 */
    Eth_VerifyReadRegister32(Eth_GaaBaseAddr[CtrlIdx] +
    /* MISRA Violation: END Msg(4:2985)-20 */
      ETH_CCC, LulCccReg);
    /* Read Current GCCR.TCR*/
    /* TRACE[ETH_DDD_ACT_102_REG002] */
    LulGccrReg = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR);
    if ((ETH_ZERO_32 != (LulCccReg & CCC_CSEL_MASK)) &&
      (ETH_ZERO_32 == (LulGccrReg & ETH_GPTP_CAPTURE_MASK)))
    {
      /* There is a clock to gPTP and no Active Time Request */
      /* Set GCCR.TCR = 11 to initiate the capture */
      /* TRACE[ETH_DDD_ACT_102_REG005] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
        (LulGccrReg | ETH_GPTP_CAPTURE_MASK));

      /* Wait for GCCR.TCR = 00 */
      LddPTPRequestComplete0 =
        Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ZERO_8);
      LddPTPRequestComplete1 =
        Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ONE_8);
      if ((E_OK == LddPTPRequestComplete0) && (E_OK == LddPTPRequestComplete1))
      {
        /* Timeout Not Occurred - Return Valid TimeStamp */
        /* TS will be given with good quality */
        /* MISRA Violation: START Msg(2:2814)-13 */
        *timeQualPtr = ETH_VALID;
        /* MISRA Violation: END Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:0306)-2 */
        Eth_ReadTSFromAnAddr((uint32 *)(Eth_GaaBaseAddr[CtrlIdx] +
        /* MISRA Violation: END Msg(4:0306)-2 */
          ETH_GCTBASE), timeStampPtr, ETH_VALID);
        LddStdReturn = E_OK;
      } /*
        * ((E_OK == LddPTPRequestComplete0) &&
        * (E_OK == LddPTPRequestComplete1))
        */
      else
      {
        /* Timeout - Return Not valid TimeStamp  */
        *timeQualPtr = ETH_INVALID;

        /* Return 0 in TS element */
        /* MISRA Violation: START Msg(4:0306)-2 */
        Eth_ReadTSFromAnAddr((uint32 *)(Eth_GaaBaseAddr[CtrlIdx] +
        /* MISRA Violation: END Msg(4:0306)-2 */
          ETH_GCTBASE), timeStampPtr, ETH_INVALID);

        LddStdReturn = E_NOT_OK;
      } /*
        * ((E_OK == LddPTPRequestComplete0) &&
        * (E_OK == LddPTPRequestComplete1))
        */
    } /*
     * ((ETH_ZERO_32 != (LulCccReg & CCC_CSEL_MASK)) &&
     * (ETH_ZERO_32 == (LulGccrReg & ETH_GPTP_CAPTURE_MASK)))
     */
    else
    {
      LddStdReturn = E_NOT_OK;

      *timeQualPtr = ETH_INVALID;
      /* Return 0 in TS element */
      /* MISRA Violation: START Msg(4:0306)-2 */
      Eth_ReadTSFromAnAddr((uint32 *)(Eth_GaaBaseAddr[CtrlIdx] +
      /* MISRA Violation: END Msg(4:0306)-2 */
        ETH_GCTBASE), timeStampPtr, ETH_INVALID);
    }
  } /* (ETH_OPERATION == LenCurrentMode) */
  else
  {
    /* Controller not Initialized or Device not in Operational Mode  */
    LddStdReturn = E_NOT_OK;
  }
  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_SetPTPTime
**
** Service ID            : NA
**
** Description           : Set Absolute value in gPTP
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         timeStampPtr - Current TimeStamp value to be set
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WaitPTPRequestIsComplete, Eth_WriteRegister32,
**                         Eth_VerifyWriteRegister32, Eth_SetBitRegister32,
**                         Eth_ResetPTPTime
**
** Registers Used        : GTO0, GTO1, GTO2, GCCR, CSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_113] */
/* TRACE[ETH_DDD_INF_101] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetPTPTime
(
  uint8 CtrlIdx,
  P2CONST(Eth_TimeStampType, AUTOMATIC, ETH_APPL_CONST) timeStampPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete0;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete1;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete0 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete1 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */

  /* Set Timer Offset */
  /* TRACE[ETH_DDD_ACT_113_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO0,
    /* MISRA Violation: START Msg(2:2814)-13 */
    timeStampPtr->nanoseconds);
    /* MISRA Violation: END Msg(2:2814)-13 */

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_113_REG002] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO0,
    timeStampPtr->nanoseconds & ETH_GTO0_REG_MASK, ETH_GTO0_REG_MASK);

  /* TRACE[ETH_DDD_ACT_113_REG003] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO1,
    timeStampPtr->seconds);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_113_REG004] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO1,
    timeStampPtr->seconds, ETH_REG_MASK);

  /* TRACE[ETH_DDD_ACT_113_REG005] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO2,
    (uint32)(timeStampPtr->secondsHi));

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_113_REG006] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO2,
    /* MISRA Violation: START Msg(4:2985)-20 */
    ((uint32)timeStampPtr->secondsHi) & ETH_GTO2_REG_MASK, ETH_GTO2_REG_MASK);
    /* MISRA Violation: END Msg(4:2985)-20 */

  /* Write into GCCR.TCCS = 01 - Set Corrected Time gPTP+ GTO as Source */
  /* TRACE[ETH_DDD_ACT_113_REG007] */
  Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
    GPTP_GCCR_TCSS_BIT0);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_113_REG008] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
    (ETH_BIT0_SET_32 << GPTP_GCCR_TCSS_BIT0), GPTP_GCCR_TCSS_MASK);

  /* Set Request to update GTO */
  /* TRACE[ETH_DDD_ACT_113_REG009] */
  Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
    ETH_GPTP_LTO_BIT);

  /* Wait TCR = 00 */
  LddPTPRequestComplete0 = Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ZERO_8);
  LddPTPRequestComplete1 = Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ONE_8);
  if ((E_OK == LddPTPRequestComplete0) && (E_OK == LddPTPRequestComplete1))
  {
    /* GTO Updated Successfully  */
    LddStdReturn = Eth_ResetPTPTime(CtrlIdx);
  }
  else
  {
    /* GTO Not updated */
    LddStdReturn = E_NOT_OK;
  }
  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_WaitPTPRequestIsComplete
**
** Service ID            : NA
**
** Description           : Wait for a Gptp request is complete
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         ucBit      - Bit to poll in GCCR Register
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WaitForTimeout
**
** Registers Used        : GCCR
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_116] */
/* TRACE[ETH_DDD_INF_102] */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_WaitPTPRequestIsComplete
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  VAR(uint8, AUTOMATIC) ucBit
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulMask;

  /* Wait for a Gptp request is complete */
  LulMask = (uint32)(ETH_BIT0_SET_32 << (ucBit));
  LddStdReturn = Eth_WaitForTimeout(ETH_E_TIMEOUT, ETH_TIMEOUT_COUNT,
    /* MISRA Violation: START Msg(4:0306)-2 */
    ETH_OS_COUNTER_ID, (uint32 *)(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_GCCR),
    /* MISRA Violation: END Msg(4:0306)-2 */
      LulMask, ETH_ZERO_32);

  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_AdjustPTPOffset
**
** Service ID            : NA
**
** Description           : Adjust gPTP offset with an given delta
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx       - Index of the ETH Controller
**                         timeOffsetPtr - Delta TimeStamp
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32, Eth_VerifyReadRegister32,
**                         Eth_WriteRegister32, Eth_VerifyWriteRegister32
**                         Eth_WaitPTPRequestIsComplete, Eth_SetBitRegister32
**
** Registers Used        : GTO0, GTO1, GTO2, GCCR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_174] */
/* TRACE[ETH_DDD_INF_145] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AdjustPTPOffset
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeIntDiffType, AUTOMATIC, ETH_APPL_CONST) timeOffsetPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulTempReg;
  VAR(Eth_TimeStampStoreType, AUTOMATIC) LstOldGTO;
  VAR(Eth_TimeStampStoreType, AUTOMATIC) LstNewGTO;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete0;
  VAR(Std_ReturnType, AUTOMATIC) LddPTPRequestComplete1;
  VAR(uint32, AUTOMATIC) LulTempNanoseconds;
  VAR(uint64, AUTOMATIC) LullTempSeconds;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete0 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddPTPRequestComplete1 = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  LullTempSeconds = (uint64)ETH_ZERO_32;

  /* Read Old GTO register */
  /* TRACE[ETH_DDD_ACT_174_REG010] */
  LstOldGTO.Timestamp0 = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] +
    ETH_GTO0);
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG011] */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO0,
    LstOldGTO.Timestamp0);
  /* TRACE[ETH_DDD_ACT_174_REG012] */
  LstOldGTO.Timestamp1 = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] +
    ETH_GTO1);
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG013] */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO1,
    LstOldGTO.Timestamp1);

  /* TRACE[ETH_DDD_ACT_174_REG014] */
  LulTempReg = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO2);
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG015] */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO2,
    LulTempReg);
  /* Cast from uint32 to uint16 to get lower bits of LulTempReg */
  LstOldGTO.Timestamp2 = (uint16)(LulTempReg & ETH_LOW_BYTE_32);
  /* Calculate nanoseconds part first */
  LulTempNanoseconds = (LstOldGTO.Timestamp0 & ETH_NANOSECOND_MASK) +
    /* MISRA Violation: START Msg(2:2814)-13 */
    timeOffsetPtr->diff.nanoseconds;
    /* MISRA Violation: END Msg(2:2814)-13 */
  if (ETH_MAX_NANOSECOND < LulTempNanoseconds)
  {
    /* The result is larger than 1s */
    LulTempNanoseconds = LulTempNanoseconds - ETH_ONEBILLION;
    LstNewGTO.Timestamp0 = LulTempNanoseconds & ETH_NANOSECOND_MASK;
    /* Carry one second to the seconds part */
    /* MISRA Violation: START Msg(4:2984)-19 */
    LullTempSeconds = LullTempSeconds + (uint64)ETH_ONE_32;
    /* MISRA Violation: END Msg(4:2984)-19 */
  }
  else
  {
    /* The result is not larger than 1s */
    LstNewGTO.Timestamp0 =
      LstOldGTO.Timestamp0 + timeOffsetPtr->diff.nanoseconds;
  }
  /* Calculate seconds part */
  LullTempSeconds = LullTempSeconds + (uint64)(LstOldGTO.Timestamp1) +
    /* MISRA Violation: START Msg(4:2985)-20 */
    (uint64)(timeOffsetPtr->diff.seconds) +
    /* MISRA Violation: END Msg(4:2985)-20 */
    /* MISRA Violation: START Msg(4:2985)-20 */
    /* MISRA Violation: START Msg(4:2984)-19 */
    /* MISRA Violation: START Msg(2:2921)-17 */
    /* MISRA Violation: START Msg(4:2790)-12 */
    ((uint64)(LstOldGTO.Timestamp2) << ETH_THIRTYTWO) +
    /* MISRA Violation: END Msg(4:2790)-12 */
    /* MISRA Violation: END Msg(2:2921)-17 */
    /* MISRA Violation: END Msg(4:2984)-19 */
    /* MISRA Violation: END Msg(4:2985)-20 */
    /* MISRA Violation: START Msg(4:2984)-19 */
    /* MISRA Violation: START Msg(2:2921)-17 */
    /* MISRA Violation: START Msg(4:2790)-12 */
    ((uint64)(timeOffsetPtr->diff.secondsHi) << ETH_THIRTYTWO);
    /* MISRA Violation: END Msg(4:2790)-12 */
    /* MISRA Violation: END Msg(2:2921)-17 */
    /* MISRA Violation: END Msg(4:2984)-19 */
  /* Separate the seconds and secondsHi parts */
  /* MISRA Violation: START Msg(4:2985)-20 */
  LstNewGTO.Timestamp1 = (uint32)(LullTempSeconds & ETH_LOW_BYTE_64);
  /* MISRA Violation: END Msg(4:2985)-20 */
  LstNewGTO.Timestamp2 =
    /* MISRA Violation: START Msg(4:2984)-19 */
    /* MISRA Violation: START Msg(4:2790)-12 */
    /* MISRA Violation: START Msg(4:0580)-9 */
    /* MISRA Violation: START Msg(4:2984)-19 */
    (uint16)((LullTempSeconds & ETH_16_HIGH_BYTE_64) >> ETH_THIRTYTWO);
    /* MISRA Violation: END Msg(4:2984)-19 */
    /* MISRA Violation: END Msg(4:0580)-9 */
    /* MISRA Violation: END Msg(4:2790)-12 */
    /* MISRA Violation: END Msg(4:2984)-19 */

  /* Set Timer Offset */
  /* TRACE[ETH_DDD_ACT_174_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO0,
    LstNewGTO.Timestamp0);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG002] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO0,
    LstNewGTO.Timestamp0 & ETH_GTO0_REG_MASK, ETH_GTO0_REG_MASK);

  /* TRACE[ETH_DDD_ACT_174_REG003] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO1,
    LstNewGTO.Timestamp1);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG004] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO1,
    LstNewGTO.Timestamp1, ETH_REG_MASK);

  /* TRACE[ETH_DDD_ACT_174_REG005] */
  /* Cast to be compatible with type of input parameter */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO2,
    (uint32)(LstNewGTO.Timestamp2));

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG006] */
  /* Cast to be compatible with type of input parameter */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTO2,
    /* MISRA Violation: START Msg(4:2984)-19 */
    ((uint32)LstNewGTO.Timestamp2) & ETH_GTO2_REG_MASK, ETH_GTO2_REG_MASK);
    /* MISRA Violation: END Msg(4:2984)-19 */
  /* Write into GCCR.TCCS = 01 - Set Corrected Time gPTP+ GTO as Source  */
  /* TRACE[ETH_DDD_ACT_174_REG007] */
  Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
    GPTP_GCCR_TCSS_BIT0);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_174_REG008] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
    (ETH_BIT0_SET_32 << GPTP_GCCR_TCSS_BIT0), GPTP_GCCR_TCSS_MASK);

  /* Set Request to update GTO */
  /* TRACE[ETH_DDD_ACT_174_REG009] */
  Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
    ETH_GPTP_LTO_BIT);
  /* Wait TCR = 00 */
  LddPTPRequestComplete0 = Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ZERO_8);
  LddPTPRequestComplete1 = Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_ONE_8);
  if ((E_OK == LddPTPRequestComplete0) && (E_OK == LddPTPRequestComplete1))
  {
    /* GTO Updated Successfully  */
    LddStdReturn = E_OK;
  }
  else
  {
    /* GTO Not updated */
    LddStdReturn = E_NOT_OK;
  }

  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_AdjustPTPTime
**
** Service ID            : NA
**
** Description           : Adjust gPTP time with an given delta
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx       - Index of the ETH Controller
**                         timeOffsetPtr - Delta TimeStamp
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_GetPTPTime, Eth_SetPTPTime
**
** Registers Used        : GCCR, CCC, GCTi i = 0,1,2, CSR,GTO0, GTO1, GTO2,
**                         FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_175] */
/* TRACE[ETH_DDD_INF_146] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AdjustPTPTime
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeIntDiffType, AUTOMATIC, ETH_APPL_CONST) timeOffsetPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint64, AUTOMATIC) LullTempSeconds;
  VAR(Eth_TimeStampQualType, AUTOMATIC) timeQualPtr;
  VAR(Eth_TimeStampType, AUTOMATIC) timeStampPtr;

  /* Get the current time */
  LddStdReturn = Eth_GetPTPTime(CtrlIdx, &timeQualPtr, &timeStampPtr);
  /* Calculate the seconds part of the current time */
  /* MISRA Violation: START Msg(4:2985)-20 */
  LullTempSeconds = (uint64)(timeStampPtr.seconds) +
  /* MISRA Violation: END Msg(4:2985)-20 */
    /* MISRA Violation: START Msg(4:2984)-19 */
    /* MISRA Violation: START Msg(2:2921)-17 */
    /* MISRA Violation: START Msg(4:2790)-12 */
    ((uint64)(timeStampPtr.secondsHi) << ETH_THIRTYTWO);
    /* MISRA Violation: END Msg(4:2790)-12 */
    /* MISRA Violation: END Msg(2:2921)-17 */
    /* MISRA Violation: END Msg(4:2984)-19 */
  if (E_OK == LddStdReturn)
  {
    /* Reduce nanoseconds part of the current time by the offset value */
    /* MISRA Violation: START Msg(2:2814)-13 */
    if (timeStampPtr.nanoseconds < timeOffsetPtr->diff.nanoseconds)
    /* MISRA Violation: END Msg(2:2814)-13 */
    {
      /* Borrow one second from seconds part */
      timeStampPtr.nanoseconds = (ETH_ONEBILLION -
        timeOffsetPtr->diff.nanoseconds) + timeStampPtr.nanoseconds;
      /* Reduce the seconds part by one */
      LullTempSeconds =  LullTempSeconds - (uint64)ETH_ONE;
    }
    else
    {
      timeStampPtr.nanoseconds = timeStampPtr.nanoseconds -
        timeOffsetPtr->diff.nanoseconds;
    }
    /* Reduce seconds part of the current time by the offset value */
    LullTempSeconds = LullTempSeconds -
      /* MISRA Violation: START Msg(4:2985)-20 */
      ((uint64)(timeOffsetPtr->diff.seconds) +
      /* MISRA Violation: END Msg(4:2985)-20 */
      /* MISRA Violation: START Msg(4:2984)-19 */
      /* MISRA Violation: START Msg(2:2921)-17 */
      /* MISRA Violation: START Msg(4:2790)-12 */
      ((uint64)(timeOffsetPtr->diff.secondsHi) << ETH_THIRTYTWO));
      /* MISRA Violation: END Msg(4:2790)-12 */
      /* MISRA Violation: END Msg(2:2921)-17 */
      /* MISRA Violation: END Msg(4:2984)-19 */
    /* Separate the seconds and secondsHi parts */
    /* MISRA Violation: START Msg(4:2985)-20 */
    timeStampPtr.seconds = (uint32)(LullTempSeconds & ETH_LOW_BYTE_64);
    /* MISRA Violation: END Msg(4:2985)-20 */
    timeStampPtr.secondsHi =
      /* MISRA Violation: START Msg(4:2984)-19 */
      /* MISRA Violation: START Msg(4:2790)-12 */
      /* MISRA Violation: START Msg(4:0580)-9 */
      /* MISRA Violation: START Msg(4:2984)-19 */
      (uint16)((LullTempSeconds & ETH_16_HIGH_BYTE_64) >> ETH_THIRTYTWO);
      /* MISRA Violation: END Msg(4:2984)-19 */
      /* MISRA Violation: END Msg(4:0580)-9 */
      /* MISRA Violation: END Msg(4:2790)-12 */
      /* MISRA Violation: END Msg(4:2984)-19 */
    /* Set the current time */
    LddStdReturn = Eth_SetPTPTime(CtrlIdx, &timeStampPtr);
  }
  else
  {
    /* No action */
  }
  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_AdjustPTPIncrement
**
** Service ID            : NA
**
** Description           : Adjust gPTP increment value with rate ratio
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx       - Index of the ETH Controller
**                         rateRatioPtr  - Ratio element
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32, Eth_VerifyReadRegister32,
**                         Eth_WriteRegister32, Eth_VerifyWriteRegister32
**                         Eth_WaitPTPRequestIsComplete,
**                         Eth_GetIncrementFromRatio, Eth_SetBitRegister32
**
** Registers Used        : GTI, GCCR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_176] */
/* TRACE[ETH_DDD_INF_147] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AdjustPTPIncrement
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_RateRatioType, AUTOMATIC, ETH_APPL_CONST) rateRatioPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulOldGTIValue;
  VAR(uint32, AUTOMATIC) LulNewGTIValue;
  VAR(uint32, AUTOMATIC) LulGCCRValue;
  VAR(Eth_TimeIntDiffType, AUTOMATIC) LstOriginTimeStampDelta;
  VAR(Eth_TimeIntDiffType, AUTOMATIC) LstIngressTimeStampDelta;

  /* MISRA Violation: START Msg(2:2814)-13 */
  LstOriginTimeStampDelta = rateRatioPtr->OriginTimeStampDelta;
  /* MISRA Violation: END Msg(2:2814)-13 */
  LstIngressTimeStampDelta = rateRatioPtr->IngressTimeStampDelta;

  if ((ETH_TRUE == LstOriginTimeStampDelta.sign) &&
    (ETH_TRUE == LstIngressTimeStampDelta.sign))
  {
    /* Time Stamp Delta is positive */
    /* TRACE[ETH_DDD_ACT_176_REG001] */
    LulGCCRValue = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] +
      ETH_GCCR);
    if (ETH_ZERO_32 == (LulGCCRValue & ETH_GPTP_CAPTURE_MASK))
    {
      /* No Active Request */
      /* Set GTI */
      /* Read Current GTI value */
      /* TRACE[ETH_DDD_ACT_176_REG003] */
      LulOldGTIValue = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_GTI);
      /* Register read-verify check */
      /* TRACE[ETH_DDD_ACT_176_REG004] */
      Eth_VerifyReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTI,
        LulOldGTIValue);

      /* Mask unnecessary bits */
      LulOldGTIValue = LulOldGTIValue & ETH_GTI_MASK;

      LulNewGTIValue = Eth_GetIncrementFromRatio(LulOldGTIValue,
        LstOriginTimeStampDelta.diff, LstIngressTimeStampDelta.diff);

      if (LulNewGTIValue != LulOldGTIValue)
      {
        /* Set GTI */
        /* TRACE[ETH_DDD_ACT_176_REG005] */
        Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GTI,
          LulNewGTIValue);
        /* Register write-verify check */
        /* TRACE[ETH_DDD_ACT_176_REG006] */
        Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
          ETH_GTI, LulNewGTIValue, ETH_GTI_MASK);
        /* Set Request to load GTI */
        /* TRACE[ETH_DDD_ACT_176_REG007] */
        Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_GCCR,
          ETH_GPTP_LTI_BIT);
        LddStdReturn = Eth_WaitPTPRequestIsComplete(CtrlIdx, ETH_GPTP_LTI_BIT);
      }
      else
      {
        /* No action required */
        LddStdReturn = E_NOT_OK;
      }
    }
    else
    {
      /* Another Request is pending to gPTP */
      LddStdReturn = E_NOT_OK;
    } /* (ETH_ZERO_32 == (LulGCCRValue & ETH_GPTP_CAPTURE_MASK)) */
  }/*
   * ((ETH_TRUE == LstOriginTimeStampDelta->sign) &&
   * (ETH_TRUE == IngressTimeStampDelta->sign))
   */
  else
  {
    /* Time Stamp Delta is negative */
    /* No action required */
    /* GTI Not updated */
    LddStdReturn = E_NOT_OK;
  }
  return LddStdReturn;
}

/*******************************************************************************
** Function Name         : Eth_EnableTSFifo
**
** Service ID            : NA
**
** Description           : Service Api to set TFEN and TFR BIt in TCCR Register
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx   - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32, Eth_VerifyWriteRegister32
**                         Eth_WriteRegister32
**
** Registers Used        : TCCR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_101] */
/* TRACE[ETH_DDD_INF_104] */
FUNC(void, ETH_CODE_SLOW) Eth_EnableTSFifo (VAR(uint8, AUTOMATIC) CtrlIdx)
{
  VAR(uint32, AUTOMATIC) LulTCCRRegVal;

  LulTCCRRegVal = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_TCCR);
  /* Write 0 to Time Stamp FIFO Release bit (TCCR. TFR) and Transmit Start
  * Request bits (TCCR.TSRQn, n = 0,1,2,3) to avoid unintended request
  */
  LulTCCRRegVal &= ((~ETH_TCCR_TSRQ_MASK) & (~ETH_TCCR_TFR_MASK));
  /* Enable TimeStamp in Tx - TCCR.TFEN = 1 */
  LulTCCRRegVal |= ETH_TCCR_TFEN_MASK;

  /* TRACE[ETH_DDD_ACT_101_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_TCCR, LulTCCRRegVal);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_101_REG002] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_TCCR,
    ETH_TCCR_TFEN_MASK, ETH_TCCR_TFEN_MASK);
}

/*******************************************************************************
** Function Name         : Eth_ReadTSFifo
**
** Service ID            : NA
**
** Description           : Read TimeStamp from a message just transmitted if tag
**                         match with Buffer index
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx      - Index of the ETH Controller
**                         BufIdx       - Buffer index
**
** InOut Parameters      : None
**
** Output Parameters     : timeQualPtr  - Quality of the Read TS
**                         timeStampPtr - Value passed above
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32, Eth_ReadTSFromAnAddr,
**                         Eth_WriteRegister32
**
** Registers Used        : TFA2, TFA0, TCCR, TSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_107] */
/* TRACE[ETH_DDD_INF_105] */
FUNC(void, ETH_CODE_SLOW) Eth_ReadTSFifo(
  VAR(uint8, AUTOMATIC) CtrlIdx,
  VAR(uint8, AUTOMATIC) BufIdx,
  P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
)
{
  VAR(uint8, AUTOMATIC) LucFillLevel;
  VAR(uint16, AUTOMATIC) LucTagEntry;
  VAR(uint32, AUTOMATIC) LulTempReg;

  if (NULL_PTR != timeQualPtr)
  {
    /* TRACE[ETH_DDD_ACT_107_REG001] */
    LulTempReg = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_TFA2);
    /* Cast from uint32 to uint16 to get lower bits of LulTempReg */
    LucTagEntry = (uint16)(LulTempReg >> ETH_SIXTEEN);
    /* Read the fill level */
    /* TRACE[ETH_DDD_ACT_107_REG003] */
    LulTempReg = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx]+ ETH_TSR);
    /* Cast from uint32 to uint8 to get TFFL bit of TSR register */
    LucFillLevel =
      (uint8)((LulTempReg & ETH_TSR_TFFL_MASK) >> ETH_TSR_TFFL_BIT);
    if ((LucFillLevel != ETH_ZERO_8) && (LucTagEntry == (uint16)BufIdx))
    {
      /* Time Stamp found the the passed buffer index */
      *timeQualPtr = ETH_VALID;
      /* Cast to larger type to be compatible with type of input parameter */
      /* MISRA Violation: START Msg(4:0306)-2 */
      Eth_ReadTSFromAnAddr((uint32 *)(Eth_GaaBaseAddr[CtrlIdx] +
      /* MISRA Violation: END Msg(4:0306)-2 */
        ETH_TFA0), timeStampPtr, ETH_VALID);
      LulTempReg = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_TCCR);
      /* Write 0 to Transmit Start Request bits (TCCR.TSRQn, n = 0,1,2,3) to
      * avoid unintended request
      */
      LulTempReg &= (~ETH_TCCR_TSRQ_MASK);
      /* Release Old Entry in TS Fifo - TCCR.TFR = 1 */
      LulTempReg |= ETH_TCCR_TFR_MASK;
      /* TRACE[ETH_DDD_ACT_107_REG004] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_TCCR,
        LulTempReg);
    } /* (LucTagEntry == (uint16)BufIdx) */
    else
    {
      /* Time Stamp not found give invalid and set it to 0*/
      *timeQualPtr = ETH_INVALID;

      /* Return 0 in TS element */
      /* Cast to larger type to be compatible with type of input parameter */
      /* MISRA Violation: START Msg(4:0306)-2 */
      Eth_ReadTSFromAnAddr((uint32 *)(Eth_GaaBaseAddr[CtrlIdx] +
      /* MISRA Violation: END Msg(4:0306)-2 */
        ETH_TFA0), timeStampPtr, ETH_INVALID);
    } /* (LucTagEntry == (uint16)BufIdx) */
  }/* (NULL_PTR != timeQualPtr) */
  else
  {
    /* Device Not Initialized */
  }
}

/*******************************************************************************
** Function Name         : Eth_ReadTSFromAnAddr
**
** Service ID            : NA
**
** Description           : Read Timestamp from several registers (TFAi, GCTi,
**                         GTOi) and store in the Autosar format variables
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : pAddr  - Address of the register from which the time
**                                  stamp need to be returned to context of the
**                                  ETH Driver
**                         Action - Valid or Invalid TS need to be
**
** InOut Parameters      : None
**
** Output Parameters     : ptimeStampPtr - Value passed above
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : None
**
** Functions invoked     : Eth_ReadRegister32
**
** Registers Used        : Specified by pAddr
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_108] */
/* TRACE[ETH_DDD_INF_106] */
static FUNC(void, ETH_CODE_SLOW) Eth_ReadTSFromAnAddr
(
  CONSTP2CONST(uint32, AUTOMATIC, ETH_APPL_CONST) pAddr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) ptimeStampPtr,
  VAR(Eth_TimeStampQualType, AUTOMATIC) Action
)
{
  VAR(uint32, AUTOMATIC) LulTempReg;

  if (ETH_VALID == Action)
  {
    /* Cast from uint32 * to uint32 to access to register */
    /* TRACE[ETH_DDD_ACT_108_REG001] */
    /* MISRA Violation: START Msg(4:0306)-2 */
    /* MISRA Violation: START Msg(2:2814)-13 */
    ptimeStampPtr->nanoseconds = Eth_ReadRegister32((uint32)pAddr);
    /* MISRA Violation: END Msg(2:2814)-13 */
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* Eth_ReadRegister32(pAddr+1); */
    /* TRACE[ETH_DDD_ACT_108_REG002] */
    /* MISRA Violation: START Msg(2:2824)-14 */
    /* MISRA Violation: START Msg(4:0488)-5 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    ptimeStampPtr->seconds = Eth_ReadRegister32((uint32)(pAddr + ETH_ONE_32));
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(4:0488)-5 */
    /* MISRA Violation: END Msg(2:2824)-14 */
    /* Eth_ReadRegister32(pAddr+2); */
    /* TRACE[ETH_DDD_ACT_108_REG003] */
    /* MISRA Violation: START Msg(4:0488)-5 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    LulTempReg = Eth_ReadRegister32((uint32)(pAddr + ETH_TWO_32));
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(4:0488)-5 */
    /* Cast from uint32 to uint16 to get lower bits of LulTempReg */
    ptimeStampPtr->secondsHi = (uint16)(LulTempReg & ETH_LOW_BYTE_32);
  } /* (ETH_VALID == Action) */
  else if (ETH_INVALID == Action)
  {
    /* Return 0 as TS*/
    ptimeStampPtr->nanoseconds = ETH_ZERO_32;
    ptimeStampPtr->seconds     = ETH_ZERO_32;
    ptimeStampPtr->secondsHi   = ETH_ZERO_16;
  }
  else
  {
    /* Cannot read anything from null Address */
  }
}

/*******************************************************************************
** Function Name         : Eth_ReadTSDescriptor
**
** Service ID            : NA
**
** Description           : Read TimeStamp from a message received and store in
**                         Autosar Format
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : timeQualPtr  - Quality of the Time Stamp
**                         timeStampPtr - TS read from the Rx Descriptor.
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaTempTsDescr, Eth_GaaTempTimeStamp,
**                         Eth_GucTsQueueIdx
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_106] */
/* TRACE[ETH_DDD_INF_107] */
FUNC(void, ETH_CODE_SLOW) Eth_ReadTSDescriptor
(
  P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
)
{
  /* MISRA Violation: START Msg(4:3415)-26 */
  if ((Eth_GucTsQueueIdx < ETH_RX_QUEUE_COUNT) &&
  /* MISRA Violation: END Msg(4:3415)-26 */
    (NULL_PTR != Eth_GaaTempTsDescr[Eth_GucTsQueueIdx]))
  {
    /* TimeStamp is valid  */
    /* MISRA Violation: START Msg(2:2814)-13 */
    *timeQualPtr = ETH_VALID;
    /* MISRA Violation: END Msg(2:2814)-13 */
    /* Read TimeStamp from the rxframe.Timestamp */
    /* MISRA Violation: START Msg(2:2814)-13 */
    timeStampPtr->nanoseconds =
    /* MISRA Violation: END Msg(2:2814)-13 */
      Eth_GaaTempTimeStamp[Eth_GucTsQueueIdx].Timestamp0;
    timeStampPtr->seconds     =
      Eth_GaaTempTimeStamp[Eth_GucTsQueueIdx].Timestamp1;
    timeStampPtr->secondsHi   =
      Eth_GaaTempTimeStamp[Eth_GucTsQueueIdx].Timestamp2;
  }
  else
  {
    *timeQualPtr = ETH_INVALID;
    timeStampPtr->nanoseconds = ETH_ZERO_32;
    timeStampPtr->seconds     = ETH_ZERO_32;
    timeStampPtr->secondsHi   = ETH_ZERO_16;
  }
}

/*******************************************************************************
** Function Name         : Eth_GetIncrementFromInputClock
**
** Service ID            : NA
**
** Description           : Return how many nanosecond the gPTP counter is
**                         increment based on the clock supplied to the module
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
** Return parameter      : LulTempValue: Return how many nanosecond
**                         the gPTP counter is increment based on
**                         the clock supplied to the module
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_153] */
/* TRACE[ETH_DDD_INF_109] */
static FUNC(uint32, ETH_CODE_SLOW) Eth_GetIncrementFromInputClock(void)
{
  VAR(uint32, AUTOMATIC) LulReturnValue;

  /* High-speed peripheral bus clock */
  /* MISRA Violation: START Msg(4:2910)-16 */
  LulReturnValue = (uint32)((((uint64)(ETH_ONE_32 << ETH_TWENTY_8)) *
  /* MISRA Violation: END Msg(4:2910)-16 */
    /* MISRA Violation: START Msg(4:2910)-16 */
    (uint64)ETH_ONEBILLION) / (uint64)ETH_PERI_CLOCK_HZ);
    /* MISRA Violation: END Msg(4:2910)-16 */

  return LulReturnValue;
}

/*******************************************************************************
** Function Name         : Eth_GetIncrementFromRatio
**
** Service ID            : NA
**
** Description           : Return how many nanosecond the gPTP counter is
**                         increment based on the ratio of the frequency of the
**                         grandmaster to the frequency of the LocalClock
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : OldIncrementVal - Current GTI value
**                         OriginTimeStampDelta - Origin TimeStamp Delta
**                         IngressTimeStampDelta - Ingress TimeStamp Delta
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulReturnValue: Return how many nanosecond
**                         the gPTP counter is increment based on
**                         the rate ratio
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_177] */
/* TRACE[ETH_DDD_INF_148] */
static FUNC(uint32, ETH_CODE_SLOW) Eth_GetIncrementFromRatio
(
  uint32 OldIncrementVal,
  Eth_TimeStampType OriginTimeStampDelta,
  Eth_TimeStampType IngressTimeStampDelta
)
{
  VAR(uint32, AUTOMATIC) LulReturnValue;
  VAR(uint64, AUTOMATIC) LullTempValue;
  VAR(uint64, AUTOMATIC) LullOriginTimeStampDelta;
  VAR(uint64, AUTOMATIC) LullIngressTimeStampDelta;

  LulReturnValue = OldIncrementVal;

  if ((ETH_ZERO_16 == OriginTimeStampDelta.secondsHi) &&
    (ETH_ZERO_16 == IngressTimeStampDelta.secondsHi))
  {
    /* Convert to nanosecond before calculating */
    LullOriginTimeStampDelta = (((uint64)OriginTimeStampDelta.seconds)*
      (uint64)ETH_ONEBILLION) + (uint64)OriginTimeStampDelta.nanoseconds;
    LullIngressTimeStampDelta = (((uint64)IngressTimeStampDelta.seconds)*
      (uint64)ETH_ONEBILLION) + (uint64)IngressTimeStampDelta.nanoseconds;
    if (((uint64)ETH_ZERO_32 < LullIngressTimeStampDelta) &&
      /* MISRA Violation: START Msg(4:2995)-24 */
      ((uint64)ETH_MAX_TIME_STAMP_DELTA >= LullIngressTimeStampDelta) &&
      /* MISRA Violation: END Msg(4:2995)-24 */
      /* MISRA Violation: START Msg(4:2995)-24 */
      ((uint64)ETH_MAX_TIME_STAMP_DELTA >= LullOriginTimeStampDelta))
      /* MISRA Violation: END Msg(4:2995)-24 */
    {
      /* Calculate new increment value */
      LullTempValue = ((uint64)OldIncrementVal*LullOriginTimeStampDelta)/
        LullIngressTimeStampDelta;
      if (((uint64)GTI_MIN_VALUE < LullTempValue) &&
        (((uint64)GTI_MAX_VALUE >= LullTempValue)))
      {
        /* MISRA Violation: START Msg(4:2985)-20 */
        LulReturnValue = ((uint32)LullTempValue) & ETH_GTI_MASK;
        /* MISRA Violation: END Msg(4:2985)-20 */
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
  else
  {
    /* No action required */
  }
  return LulReturnValue;
}

#endif /* GLOBAL_TIME_SUPPORT */

/*******************************************************************************
** Function Name         : Eth_SetCbsParameter
**
** Service ID            : NA
**
** Description           : Determine the CBS value for the Controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx      - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GpConfigPtr, Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : CIVR0, CIVR1, CDVR0, CDVR1, CUL0, CUL1, CLL0, CLL1,
**                         FSO_CTL
**
*******************************************************************************/
#if (ETH_QOS_SUPPORT == STD_ON)
/* TRACE[ETH_DDD_ACT_120] */
/* TRACE[ETH_DDD_INF_111] */
FUNC(void, ETH_CODE_SLOW) Eth_SetCbsParameter(uint8 CtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint32, AUTOMATIC) LulTempCiv;
  /* Negative number is represented in 2'complement */
  VAR(uint32, AUTOMATIC) LulTempCdv;
  VAR(uint8, AUTOMATIC) LucNumberOfTxQueue;

  /* Loop through all Tx Queues configured  */
  /* MISRA Violation: START Msg(2:2814)-13 */
  LucNumberOfTxQueue = Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfTxQueue;
  /* MISRA Violation: END Msg(2:2814)-13 */
  for (LucCount = ETH_ZERO_8; LucCount < LucNumberOfTxQueue; LucCount++)
  {
    if (((uint8)ETH_TX_SB ==
      /* MISRA Violation: START Msg(4:0491)-7 */
      Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].id) &&
      /* MISRA Violation: END Msg(4:0491)-7 */
      /* MISRA Violation: START Msg(4:0491)-7 */
      (ETH_CBS == Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].
      /* MISRA Violation: END Msg(4:0491)-7 */
      TxQueueShaper.TxQPolicy))
    {
      /* Q2*/
      LulTempCiv = (uint32)(Eth_GpConfigPtr->
        /* MISRA Violation: START Msg(4:0491)-7 */
        Eth_QueueConfig.pTxQueueConfig[LucCount].TxQueueShaper.civ);
        /* MISRA Violation: END Msg(4:0491)-7 */

      LulTempCdv = (uint32)(((ETH_MAX_CDV +
        ((uint32)(Eth_GpConfigPtr->Eth_QueueConfig.
        /* MISRA Violation: START Msg(4:0491)-7 */
        pTxQueueConfig[LucCount].TxQueueShaper.cdv) + ETH_ONE_32))) |
        /* MISRA Violation: END Msg(4:0491)-7 */
        (ETH_MIN_CDV));


      /* Write into CIVR0 , CDVR0 */
      /* TRACE[ETH_DDD_ACT_120_REG001] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CIVR0, LulTempCiv);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG002] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CIVR0,
        LulTempCiv, ETH_REG_MASK);

      /* TRACE[ETH_DDD_ACT_120_REG003] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CDVR0, LulTempCdv);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG004] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CDVR0,
        LulTempCdv, ETH_REG_MASK);

      /* Set Upper Limit => 65535 */
      /* TRACE[ETH_DDD_ACT_120_REG005] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CUL0, ETH_MAX_CIV);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG006] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CUL0,
        ETH_MAX_CIV, ETH_REG_MASK);

      /* Set Lower Limit => -65536 */
      /* TRACE[ETH_DDD_ACT_120_REG007] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CLL0, ETH_MIN_CDV);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG008] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CLL0,
        ETH_MIN_CDV, ETH_REG_MASK);

    }/*
     * (((uint8)ETH_TX_SB ==
     * Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].id) &&
     * (ETH_CBS == Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].
     * TxQueueShaper.TxQPolicy))
     */

    else if (((uint8)(ETH_TX_SA) ==
      /* MISRA Violation: START Msg(4:0491)-7 */
      (Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].id)) &&
      /* MISRA Violation: END Msg(4:0491)-7 */
      ((ETH_CBS == Eth_GpConfigPtr->Eth_QueueConfig.
      /* MISRA Violation: START Msg(4:0491)-7 */
      pTxQueueConfig[LucCount].TxQueueShaper.TxQPolicy)))
      /* MISRA Violation: END Msg(4:0491)-7 */
    {
      /* Q3 */
      LulTempCiv = (uint32)(Eth_GpConfigPtr->Eth_QueueConfig.
        /* MISRA Violation: START Msg(4:0491)-7 */
        pTxQueueConfig[LucCount].TxQueueShaper.civ);
        /* MISRA Violation: END Msg(4:0491)-7 */

      LulTempCdv = (uint32)((ETH_MAX_CDV +
        ((uint32)(Eth_GpConfigPtr->Eth_QueueConfig.
        /* MISRA Violation: START Msg(4:0491)-7 */
        pTxQueueConfig[LucCount].TxQueueShaper.cdv) + ETH_ONE_32)) |
        /* MISRA Violation: END Msg(4:0491)-7 */
        (ETH_MIN_CDV));

      /* Write into CIVR1 , CDVR1 */
      /* TRACE[ETH_DDD_ACT_120_REG009] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CIVR1, LulTempCiv);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG010] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CIVR1,
        LulTempCiv, ETH_REG_MASK);

      /* TRACE[ETH_DDD_ACT_120_REG011] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CDVR1, LulTempCdv);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG012] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CDVR1,
        LulTempCdv, ETH_REG_MASK);

      /* Set Upper Limit => 65535 */
      /* TRACE[ETH_DDD_ACT_120_REG013] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
        ETH_CUL1, ETH_MAX_CIV);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG014] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CUL1,
        ETH_MAX_CIV, ETH_REG_MASK);

      /* Set Lower Limit => -65536 */
      /* TRACE[ETH_DDD_ACT_120_REG015] */
      Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CLL1,
        ETH_MIN_CDV);

      /* Register write-verify check */
      /* TRACE[ETH_DDD_ACT_120_REG016] */
      Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CLL1,
        ETH_MIN_CDV, ETH_REG_MASK);

    }/*
     * ((uint8)(ETH_TX_SA) ==
     * (Eth_GpConfigPtr->Eth_QueueConfig.pTxQueueConfig[LucCount].id) &&
     * (ETH_CBS == Eth_GpConfigPtr->Eth_QueueConfig.
     * pTxQueueConfig[LucCount].TxQueueShaper.TxQPolicy))
     */
    else
    {
      /* Nothing to set for this Tx Queue*/
    }
  }
}

#endif /* (ETH_QOS_SUPPORT == STD_ON) */
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/*******************************************************************************
** Function Name         : Eth_UFCounterDecrement
**
** Service ID            : NA
**
** Description           : Decrease Unread Frame Counter
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ucCtrlIdx - Index of the ETH Controller
**                         ucRxQueueIdx - Rx Queue Index
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
** Functions invoked     : Eth_WriteRegister32
**
** Registers Used        : UFCD
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_157] */
/* TRACE[ETH_DDD_INF_128] */
FUNC(void, ETH_CODE_FAST) Eth_UFCounterDecrement
(
  uint8 ucCtrlIdx,
  uint8 ucRxQueueIdx
)
{
  /* Decrease Unread Frame Counter by write to UFCDi register */
  /* TRACE[ETH_DDD_ACT_157_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[ucCtrlIdx] + ETH_UFCDBASE +
    (uint32)(((uint32)ucRxQueueIdx >> ETH_TWO_32) * ETH_FOUR_32),
    ((uint32)(ETH_ONE_32 << (uint32)((uint32)((uint32)ucRxQueueIdx %
      ETH_FOUR_32) * ETH_EIGHT_32))));
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_PayloadCRCAreaStartSet
**
** Device                : H3
**
** Service ID            : NA
**
** Description           : Set the number of the first data byte in received
**                         frame to check a potential payload CRC
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ucCtrlIdx - Index of the ETH Controller
**                         ucCrcAreaStart - Payload CRC Area Start Position
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
** Functions invoked     : Eth_WriteRegister32, Eth_VerifyWriteRegister32,
**
** Registers Used        : PCRC, FSO_CTL
**
*******************************************************************************/
#if (ETH_DEVICE_NAME == RCAR_H3)
#if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* H3 TRACE[ETH_DDD_ACT_156] */
/* H3 TRACE[ETH_DDD_INF_130] */
FUNC(void, ETH_CODE_SLOW) Eth_PayloadCRCAreaStartSet
(
  uint8 ucCtrlIdx,
  uint8 ucCrcAreaStart
)
{
  /* TRACE[ETH_DDD_ACT_156_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[ucCtrlIdx] + ETH_PCRC,
    (uint32)ucCrcAreaStart);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_156_REG002] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[ucCtrlIdx] + ETH_PCRC,
    (uint32)ucCrcAreaStart, ETH_REG_MASK);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#endif
#endif /* (ETH_DEVICE_NAME == RCAR_H3) */

/*******************************************************************************
** Function Name         : Eth_UnreadDataCheck
**
** Service ID            : NA
**
** Description           : Check available frames
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucQIdx       - Channel index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulCount
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstDescTable
**
** Functions invoked     : Eth_RxNormalDescDataCheck, Eth_RxExtDescDataCheck
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_162] */
/* TRACE[ETH_DDD_INF_135] */
FUNC(uint8, ETH_CODE_SLOW) Eth_UnreadDataCheck
(
  uint8 LucQIdx
)
{
  VAR(uint32, AUTOMATIC) LulDescBaseAddr;
  VAR(uint8, AUTOMATIC) LucCount;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LucCount = ETH_ZERO_8;
  /* MISRA Violation: END Msg(4:2982)-18 */

  /* Get the base address of the corresponding Rx Queue */
  LulDescBaseAddr = Eth_GstDescTable[ETH_TXQ_MAXNUM + LucQIdx].Dptr;

  if (ETH_BECHANNEL == LucQIdx)
  {
    /* Best effort channel */
    #if (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
    LucCount = Eth_RxNormalDescDataCheck(LulDescBaseAddr);
    #else
    LucCount = Eth_RxExtDescDataCheck(LulDescBaseAddr);
    #endif
  }
  else if (ETH_NCCHANNEL == LucQIdx)
  {
    /* Network control channel */
    LucCount = Eth_RxExtDescDataCheck(LulDescBaseAddr);
  }
  else
  {
    /* Stream channel */
    #if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF)
    LucCount = Eth_RxNormalDescDataCheck(LulDescBaseAddr);
    #else
    LucCount = Eth_RxExtDescDataCheck(LulDescBaseAddr);
    #endif
  }

  return LucCount;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxNormalDescDataCheck
**
** Service ID            : NA
**
** Description           : This process data checking for normal
**                         descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LulDescPtr    - Pointer to Rx Descriptor
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulCount
**
** Preconditions         : None
**
** Global Variables      : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF) || \
  (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
/* TRACE[ETH_DDD_ACT_168] */
/* TRACE[ETH_DDD_INF_136] */
static FUNC(uint8, ETH_CODE_SLOW) Eth_RxNormalDescDataCheck
(
  uint32 LulDescPtr
)
{
  P2VAR(volatile Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) LpDataDesc;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucIndex;

  LucCount = ETH_ZERO_8;

  /* MISRA Violation: START Msg(4:0303)-1 */
  LpDataDesc = (volatile Eth_DataDescType *)LulDescPtr;
  /* MISRA Violation: END Msg(4:0303)-1 */

  for (LucIndex = ETH_ZERO_8; LucIndex < ETH_RX_BUF_TOTAL; LucIndex++)
  {
    /* MISRA Violation: START Msg(2:2814)-13 */
    if ((ETH_DESC_FSINGLE == (LpDataDesc->Header).Dt) ||
    /* MISRA Violation: END Msg(2:2814)-13 */
      (ETH_DESC_FSTART == (LpDataDesc->Header).Dt))
    {
      LucCount++;
    }
    else
    {
      /* No action */
    }
    /* Increase to the next descriptor */
    /* MISRA Violation: START Msg(4:0489)-6 */
    LpDataDesc++;
    /* MISRA Violation: END Msg(4:0489)-6 */
  }

  return LucCount;
}
#endif
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxExtDescDataCheck
**
** Service ID            : NA
**
** Description           : This process data checking for extended
**                         descriptor chain
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LulDescPtr    - Pointer to Rx Descriptor
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LucCount
**
** Preconditions         : None
**
** Global Variables      : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_169] */
/* TRACE[ETH_DDD_INF_137] */
static FUNC(uint8, ETH_CODE_SLOW) Eth_RxExtDescDataCheck
(
  uint32 LulDescPtr
)
{
  P2VAR(volatile Eth_ExtDataDescType, AUTOMATIC, ETH_APPL_DATA) LpExtDataDesc;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucIndex;

  LucCount = ETH_ZERO_8;

  /* MISRA Violation: START Msg(4:0303)-1 */
  LpExtDataDesc = (volatile Eth_ExtDataDescType *)LulDescPtr;
  /* MISRA Violation: END Msg(4:0303)-1 */

  for (LucIndex = ETH_ZERO_8; LucIndex < ETH_RX_BUF_TOTAL; LucIndex++)
  {
    /* MISRA Violation: START Msg(2:2814)-13 */
    if ((ETH_DESC_FSINGLE == (LpExtDataDesc->Header).Dt) ||
    /* MISRA Violation: END Msg(2:2814)-13 */
      (ETH_DESC_FSTART == (LpExtDataDesc->Header).Dt))
    {
      LucCount++;
    }
    else
    {
      /* No action */
    }
    /* Increase to the next descriptor */
    /* MISRA Violation: START Msg(4:0489)-6 */
    LpExtDataDesc++;
    /* MISRA Violation: END Msg(4:0489)-6 */
  }

  return LucCount;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/*******************************************************************************
** Function Name         : Eth_ControllerAccessCheck
**
** Service ID            : NA
**
** Description           : Check the access to the Ethernet controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturn
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ReadRegister32
**
** Registers Used        : VRR, GECMR
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_159] */
/* TRACE[ETH_DDD_INF_138] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ControllerAccessCheck(uint8 ucCtrlIdx)
{
  VAR(uint32, AUTOMATIC) LulVRRRegValue;
  VAR(uint32, AUTOMATIC) LulGECMRRegValue;
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;

  /* Read the first register */
  /* TRACE[ETH_DDD_ACT_159_REG001] */
  LulVRRRegValue = Eth_ReadRegister32(Eth_GaaBaseAddr[ucCtrlIdx] +
    ETH_VRR);

  if (ETH_VRR_VALUE == (LulVRRRegValue & ETH_VRR_GECMR_MASK))
  {
    /* Read the second register */
    /* TRACE[ETH_DDD_ACT_159_REG002] */
    LulGECMRRegValue = Eth_ReadRegister32(Eth_GaaBaseAddr[ucCtrlIdx] +
      ETH_GECMR);
    if (ETH_ZERO_32 == (LulGECMRRegValue & ETH_VRR_GECMR_MASK))
    {
      /* Controller Access Check Pass */
      LddStdReturn = E_OK;
    }
    else
    {
      /* Controller Access Check Fail */
      LddStdReturn = E_NOT_OK;
    }
  }
  else
  {
    /* Controller Access Check Fail */
    LddStdReturn = E_NOT_OK;
  }

  return LddStdReturn;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_DMACResetHW
**
** Service ID            : NA
**
** Description           : Set AVB DMAC mode to reset mode
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : Eth_GaaBaseAddr, Eth_GenMainFunctionState,
**                         Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GpConfigPtr
**
** Functions invoked     : Eth_WaitForTimeout, Eth_ReadRegister32
**                         Eth_VerifyReadRegister32, Eth_OpModeGet
**                         Eth_WriteRegister32, Eth_VerifyWriteRegister32
**                         Eth_OpModeChange, Dem_ReportErrorStatus
**
** Registers Used        : CCC, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_179] */
/* TRACE[ETH_DDD_INF_160] */
FUNC(Std_ReturnType, ETH_CODE_FAST) Eth_DMACResetHW(uint8 CtrlIdx)
{
  /* MISRA Violation: START Msg(4:0759)-10 */
  VAR(Eth_CccType, AUTOMATIC) LunCcc;
  /* MISRA Violation: END Msg(4:0759)-10 */
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(Eth_OpStatusType, AUTOMATIC) LenCurrentMode;
  VAR(Eth_ErrorType, AUTOMATIC) LenOpModeReturnError;

  LddReturnValue = E_OK;
  LenCurrentMode = Eth_OpModeGet(CtrlIdx);

  if (ETH_OPERATION == LenCurrentMode)
  {
    /* TRACE[ETH_DDD_ACT_179_REG001] */
    /* MISRA Violation: START Msg(4:2985)-20 */
    LunCcc.Word = Eth_ReadRegister32(Eth_GaaBaseAddr[CtrlIdx] +
    /* MISRA Violation: END Msg(4:2985)-20 */
      ETH_CCC);
    /* Register read-verify check */
    /* TRACE[ETH_DDD_ACT_179_REG002] */
    /* MISRA Violation: START Msg(4:2985)-20 */
    Eth_VerifyReadRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CCC,
    /* MISRA Violation: END Msg(4:2985)-20 */
      LunCcc.Word);
    /* Set Data Transmission Suspend Request in CCC */
    LunCcc.Bit.Dtsr = ETH_ONE;

    /* TRACE[ETH_DDD_ACT_179_REG003] */
    /* MISRA Violation: START Msg(4:2985)-20 */
    Eth_WriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_CCC,
    /* MISRA Violation: END Msg(4:2985)-20 */
      LunCcc.Word);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_179_REG004] */
    /* MISRA Violation: START Msg(4:2985)-20 */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] +
    /* MISRA Violation: END Msg(4:2985)-20 */
      ETH_CCC, LunCcc.Word, ETH_REG_MASK);

    LddReturnValue = Eth_WaitForTimeout(ETH_E_TIMEOUT, ETH_TIMEOUT_COUNT,
      /* MISRA Violation: START Msg(4:0306)-2 */
      ETH_OS_COUNTER_ID, (uint32 *)(Eth_GaaBaseAddr[CtrlIdx] + ETH_CSR),
      /* MISRA Violation: END Msg(4:0306)-2 */
        ETH_CSR_DTS_MASK, ETH_CSR_DTS_MASK);
  } /* (ETH_OPERATION == LenCurrentMode) */
  else
  {
    /* No action */
  }

  if (E_OK == LddReturnValue)
  {
    LenOpModeReturnError = Eth_OpModeChange(CtrlIdx, ETH_RESET);
    if (ETH_ERR_OK == LenOpModeReturnError)
    {
      LenCurrentMode = Eth_OpModeGet(CtrlIdx);
      if (ETH_RESET == LenCurrentMode)
      {
        /* No Timeout - Mode changed  */
        /* Reset global status variables */
        Eth_GenMainFunctionState = AVB_IDLE;
        Eth_GenDriverState = ETH_STATE_UNINIT;
        Eth_GenControllerMode = ETH_MODE_DOWN;
      }
      else
      {
        /* Error on mode transition - DEM is required */
        Dem_ReportErrorStatus(ETH_E_OPMODE, DEM_EVENT_STATUS_FAILED);
        LddReturnValue = E_NOT_OK;
      }
    }
    else
    {
      /* Error on mode transition - DEM is required */
      Dem_ReportErrorStatus(ETH_E_OPMODE, DEM_EVENT_STATUS_FAILED);
      LddReturnValue = E_NOT_OK;
    }
  }
  else
  {
    /* No action required */
  }
  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_TxDescEmptyCheck
**
** Service ID            : NA
**
** Description           : Check the transmission descriptor is empty or not
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : TxBuffIdx - Index of the transmission buffer
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LblReturnValue
**
** Preconditions         : None
**
** Global Variables      : Eth_GstBufferLock
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_188] */
/* TRACE[ETH_DDD_INF_163] */
FUNC(boolean, ETH_CODE_FAST) Eth_TxDescEmptyCheck
(
  Eth_BufIdxType TxBuffIdx
)
{
  P2VAR(Eth_DataDescType, AUTOMATIC, ETH_APPL_DATA) LpDataDesc;
  VAR(boolean, AUTOMATIC) LblReturnValue;

  /* Check the descriptor of corresponding buffer */
  /* MISRA Violation: START Msg(4:0306)-2 */
  LpDataDesc = (Eth_DataDescType *)Eth_GstBufferLock[TxBuffIdx].descAddr;
  /* MISRA Violation: END Msg(4:0306)-2 */
  /* MISRA Violation: START Msg(2:2814)-13 */
  if (ETH_DESC_FEMPTY == LpDataDesc->Header.Dt)
  /* MISRA Violation: END Msg(2:2814)-13 */
  {
    LblReturnValue = ETH_TRUE;
  }
  else
  {
    LblReturnValue = ETH_FALSE;
  }
  return LblReturnValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_WaitTxQueueEmpty
**
** Service ID            : NA
**
** Description           : Wait for Tx Queue is empty or stopped
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         LucQueueIdx - Queue index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_WaitForTimeout
**
** Registers Used        : TCCR
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_190] */
/* TRACE[ETH_DDD_INF_162] */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_WaitTxQueueEmpty
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  VAR(uint8, AUTOMATIC) LucQueueIdx
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturn;
  VAR(uint32, AUTOMATIC) LulMask;

  LulMask = (uint32)(ETH_BIT0_SET_32 << (LucQueueIdx));
  /* Check the transmission queue is empty or stopped  */
  LddStdReturn = Eth_WaitForTimeout(ETH_E_TIMEOUT, ETH_TIMEOUT_COUNT,
    /* MISRA Violation: START Msg(4:0306)-2 */
    ETH_OS_COUNTER_ID, (uint32 *)(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TCCR),
    /* MISRA Violation: END Msg(4:0306)-2 */
      LulMask, ETH_ZERO_32);

  return LddStdReturn;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_TxBufferCheck
**
** Service ID            : NA
**
** Description           : This API checks the Tx buffers resource and releases
**                         if the buffer data has been fetched
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : Eth_GstBufferLock
**
** Functions invoked     : EthIf_TxConfirmation, Eth_TxDescEmptyCheck
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_143] */
/* TRACE[ETH_DDD_INF_120] */
FUNC(void, ETH_CODE_FAST) Eth_TxBufferCheck
(
  uint8 CtrlIdx
)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(boolean, AUTOMATIC) LblReturnValue;

  for (LucCount = ETH_ZERO_8; ETH_TX_BUF_TOTAL > LucCount; LucCount++)
  {
    /*
    * Check if the buffer is locked and confirmation is requested
    * by previous Eth_Transmit
    */
    if ((ETH_BUF_LOCKED == Eth_GstBufferLock[LucCount].bufLock) &&
      (ETH_TRUE == Eth_GstBufferLock[LucCount].bufTxFlag))
    {
      /* Check the descriptor is completed transferring data to FIFO */
      /* Cast to larger type to be compatible with type of input parameter */
      LblReturnValue = Eth_TxDescEmptyCheck((Eth_BufIdxType)LucCount);
      /* Release the buffer if the descriptor is completed processing or not */
      if (ETH_TRUE == LblReturnValue)
      {
        if (ETH_TRUE == Eth_GstBufferLock[LucCount].bufTxConfirm)
        {
          /* Issue transmit confirmation to upper layer */
          /* Cast to larger type to be compatible with type of input parameter*/
          EthIf_TxConfirmation(CtrlIdx, (Eth_BufIdxType)LucCount);
        }
        else
        {
          /* No action required */
        }
        /* Clear Tx confirmation request */
        /* TRACE[ETH_DDD_ACT_143_GLB002] */
        Eth_GstBufferLock[LucCount].bufTxConfirm = ETH_FALSE;
        /* Clear transmitting flag */
        Eth_GstBufferLock[LucCount].bufTxFlag = ETH_FALSE;
        /* Clear egress time-stamp flag */
        Eth_GstBufferLock[LucCount].benableTS = ETH_FALSE;
        /* Unlock the buffer */
        /* TRACE[ETH_DDD_ACT_143_GLB001] */
        Eth_GstBufferLock[LucCount].bufLock = ETH_BUF_UNLOCKED;
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
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/*******************************************************************************
** Function Name         : Eth_RxFilterProcess
**
** Service ID            : NA
**
** Description           : Check the received frames against filter addresses
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         pFrame  - Address of the Received Frame in URAM
**                         LucQIdx - Chanel index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddReturnValue
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstControllerFilterAddr, Eth_GenAddrFilterMode,
**                         Eth_GaaBroadcastAddr, Eth_GaaDevice
**
** Functions invoked     : Eth_CompareMac, Eth_IsRxFrameValid
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_163] */
/* TRACE[ETH_DDD_INF_141] */
FUNC(Std_ReturnType, ETH_CODE_FAST) Eth_RxFilterProcess
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(volatile Eth_RxSingleFrameType, AUTOMATIC, ETH_APPL_CONST)
    pFrame,
  uint8 LucQIdx
)
{
  #if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
  VAR(uint8, AUTOMATIC) LucLastElemIndex;
  VAR(Std_ReturnType, AUTOMATIC) LddCompareDestAddr;
  VAR(uint8, AUTOMATIC) LucCount;
  #endif
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(boolean, AUTOMATIC) LblRxFrameValid;

  #if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddCompareDestAddr = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  #endif

  /*
  * Check received frame is Broadcast or it's DA matches with the
  * Controller Mac Address
  */
  LblRxFrameValid = Eth_IsRxFrameValid(LucCtrlIdx,
    (const volatile Eth_RxSingleFrameType *)pFrame);

  #if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
  #if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
  /* Enter Critical */
  ETH_ENTER_CRITICAL_SECTION(GLOBAL_PROTECTION);
  #endif
  /* Get the current filter mode */
  if (ETH_FILTER_MODE == Eth_GenAddrFilterMode[LucCtrlIdx])
  {
    LucLastElemIndex =
      Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength - ETH_ONE_8;
    /* Check Received Frame DA against Filter Array */
    for (LucCount = ETH_ZERO_8; LucLastElemIndex >= LucCount; LucCount++)
    {
      LddCompareDestAddr = Eth_CompareMac(
        (const volatile uint8 *)(&Eth_GstControllerFilterAddr[LucCtrlIdx].
        RxFilterAddress[LucCount][ETH_ZERO_8]),
        /* MISRA Violation: START Msg(2:2814)-13 */
        /* MISRA Violation: START Msg(4:0306)-2 */
        (const uint8 *)(pFrame->FrameAddr));
        /* MISRA Violation: END Msg(4:0306)-2 */
        /* MISRA Violation: END Msg(2:2814)-13 */
      if (E_OK == LddCompareDestAddr)
      {
        break;
      }
      else
      {
        /* Continue searching */
      }
    }
    /*
    * Frame is valid only if it is Broadcast or it's DA matches with the
    * Controller Mac Address or matches with Filter Addresses
    */
    if ((E_OK == LddCompareDestAddr) || (ETH_TRUE == LblRxFrameValid))
    {
      /* Frame valid */
      LddReturnValue = E_OK;
    }
    else
    {
      /* Frame Not Valid */
      LddReturnValue = E_NOT_OK;
    }
  }
  else if (ETH_NORMAL_MODE == Eth_GenAddrFilterMode[LucCtrlIdx])
  {
  #endif
    if (ETH_BECHANNEL == LucQIdx)
    {
      /* Best Effort Queue */
      /*
      * Frame is valid only if it is Broadcast or it's DA matches with the
      * Controller Mac Address
      */
      if (ETH_TRUE == LblRxFrameValid)
      {
        /* Frame valid */
        LddReturnValue = E_OK;
      }
      else
      {
        /* Frame Not Valid */
        LddReturnValue = E_NOT_OK;
      }
    }
    else
    {
      /* NC Queue or Stream Queue */
      /* Frame valid */
      LddReturnValue = E_OK;
    }
  #if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
  }
  else
  {
    /* Promiscuous mode, accept all frames */
    LddReturnValue = E_OK;
  }
  #if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
  /* Exit Critical */
  ETH_EXIT_CRITICAL_SECTION(GLOBAL_PROTECTION);
  #endif
  #endif
  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_RxCallEthIf
**
** Service ID            : NA
**
** Description           : Wrapper for the Callback to the Ethernet Interface
**                         for each frame received
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         LucQIdx - Queue index
**                         pFrame  - Address of the Received Frame in URAM
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GaaDevice, Eth_GaaBroadcastAddr,
**                         Eth_GaaPayloadCrc
**
** Functions invoked     : Eth_CompareMac, EthIf_RxIndication,
**                         RCar_RfsoFaultControl, Dem_ReportErrorStatus
**
** Registers Used        : FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_110] */
/* TRACE[ETH_DDD_INF_031] */
FUNC(void, ETH_CODE_FAST) Eth_RxCallEthIf
(
  uint8 CtrlIdx,
  uint8 LucQIdx,
  CONSTP2VAR(volatile Eth_RxSingleFrameType, AUTOMATIC,ETH_APPL_DATA) pFrame
)
{
  VAR(boolean, AUTOMATIC) LblBroadcast;
  VAR(Eth_FrameType, AUTOMATIC) LddFrameType;
  VAR(uint8, AUTOMATIC) LaaSourceAddress[ETH_MACADDR_SIZE];
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucFirstByte;
  VAR(uint8, AUTOMATIC) LucSecondByte;
  VAR(Std_ReturnType, AUTOMATIC) LddBroadcastCheck;
  #if (ETH_GET_ETHER_STATS_API == STD_ON)
  VAR(uint16, AUTOMATIC) LusLenByte;
  #endif

  #if ((ETH_DEVICE_NAME != RCAR_H3) || \
  (ETH_PAYLOAD_CRC_ERROR_DETECT != STD_ON)) &&\
  (ETH_GLOBAL_TIME_SUPPORT != STD_ON)
  /* Suppress unused parameter warnings for the parameters below */
  /* MISRA Violation: START Msg(4:3112)-28 */
  (void) LucQIdx;
  /* MISRA Violation: END Msg(4:3112)-28 */
  #endif

  #if (ETH_GET_ETHER_STATS_API == STD_ON)
  /* TRACE[ETH_DDD_ACT_110_GLB001] */
  Eth_GaaDevice[ETH_AVB0].Stats.RxPacketNo++;
  #endif
  LddBroadcastCheck =
    /* MISRA Violation: START Msg(2:2814)-13 */
    /* MISRA Violation: START Msg(4:0306)-2 */
    Eth_CompareMac((const uint8 *)(pFrame->FrameAddr), Eth_GaaBroadcastAddr);
    /* MISRA Violation: END Msg(4:0306)-2 */
    /* MISRA Violation: END Msg(2:2814)-13 */
  if (E_OK == LddBroadcastCheck)
  {
    LblBroadcast = ETH_TRUE;

    #if (ETH_GET_ETHER_STATS_API == STD_ON)
    /* TRACE[ETH_DDD_ACT_110_GLB002] */
    Eth_GaaDevice[ETH_AVB0].Stats.BroadcastPktNo++;
    #endif
  }
  else
  {
    LblBroadcast = ETH_FALSE;
  }
  /* MISRA Violation: START Msg(4:0488)-5 */
  /* MISRA Violation: START Msg(4:0306)-2 */
  LucSecondByte = *((uint8 *)(pFrame->FrameAddr) + ETH_TWELVE);
  /* MISRA Violation: END Msg(4:0306)-2 */
  /* MISRA Violation: END Msg(4:0488)-5 */
  /* MISRA Violation: START Msg(4:0488)-5 */
  /* MISRA Violation: START Msg(4:0306)-2 */
  LucFirstByte = *((uint8 *)(pFrame->FrameAddr) + ETH_THIRTEEN);
  /* MISRA Violation: END Msg(4:0306)-2 */
  /* MISRA Violation: END Msg(4:0488)-5 */
  LddFrameType = ((uint16)((uint16)(LucSecondByte) << ETH_EIGHT) |
    ((uint16)(LucFirstByte)));

  for (LucCount = ETH_ZERO_8; ETH_MACADDR_SIZE > LucCount; LucCount++)
  {
    LaaSourceAddress[LucCount] =
      /* MISRA Violation: START Msg(4:0488)-5 */
      /* MISRA Violation: START Msg(4:0306)-2 */
      *((uint8 *)(pFrame->FrameAddr) + (ETH_SIX_8 + LucCount));
      /* MISRA Violation: END Msg(4:0306)-2 */
      /* MISRA Violation: END Msg(4:0488)-5 */
  }
  /* Evaluate Payload CRC status */
  #if (ETH_DEVICE_NAME == RCAR_H3)
  #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
  if (ETH_ZERO_8 == Eth_GaaPayloadCrc[LucQIdx])
  {
    /* The payload CRC does not match the original. */
    #if (ETH_GET_DROP_COUNT_API == STD_ON)
    Eth_GaaDevice[CtrlIdx].DropCount.PayloadCRCErrPacketNo++;
    #endif
    /* Report to DEM*/
    Dem_ReportErrorStatus(ETH_E_PAYLOAD_CRC, DEM_EVENT_STATUS_FAILED);
    #if (ETH_PAYLOAD_CRC_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_PAYLOAD_CRC_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
  }
  else
  {
    /* The payload CRC matches the original. */
    /* Clear Payload CRC status */
    /* TRACE[ETH_DDD_ACT_110_GLB003] */
    Eth_GaaPayloadCrc[LucQIdx] = ETH_ZERO_8;
  }
  #endif
  #endif /* (ETH_DEVICE_NAME == RCAR_H3) */
  #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
  /* Store the Queue which current time stamp belongs to */
  Eth_GucTsQueueIdx = LucQIdx;
  #endif

  #if (ETH_GET_ETHER_STATS_API == STD_ON)
  LusLenByte = (uint16)pFrame->FrameLength + ETH_FCS_LENGTH;
  Eth_GaaDevice[CtrlIdx].Stats.StatsOctetsNo += LusLenByte;
  if (ETH_PKTS_64_OCTETS == LusLenByte)
  {
    Eth_GaaDevice[CtrlIdx].Stats.Pkts64OctetsNo++;
  }
  else
  {
    /* No action required */
  }
  if ((ETH_PKTS_65_OCTETS <= LusLenByte) && (ETH_PKTS_127_OCTETS >= LusLenByte))
  {
    Eth_GaaDevice[CtrlIdx].Stats.Pkts65to127OctetsNo++;

  }
  else
  {
    /* No action required */
  }
  if ((ETH_PKTS_128_OCTETS <= LusLenByte) &&
    (ETH_PKTS_255_OCTETS >= LusLenByte))
  {
    Eth_GaaDevice[CtrlIdx].Stats.Pkts128to255OctetsNo++;
  }
  else
  {
    /* No action required */
  }
  if ((ETH_PKTS_256_OCTETS <= LusLenByte) &&
    (ETH_PKTS_511_OCTETS >= LusLenByte))
  {
    Eth_GaaDevice[CtrlIdx].Stats.Pkts256to511OctetsNo++;
  }
  else
  {
    /* No action required */
  }
  if ((ETH_PKTS_512_OCTETS <= LusLenByte) &&
    (ETH_PKTS_1023_OCTETS >= LusLenByte))
  {
    Eth_GaaDevice[CtrlIdx].Stats.Pkts512to1023OctetsNo++;
  }
  else
  {
    /* No action required */
  }
  if ((ETH_PKTS_1024_OCTETS <= LusLenByte) &&
    (ETH_PKTS_1518_OCTETS >= LusLenByte))
  {
    Eth_GaaDevice[CtrlIdx].Stats.Pkts1024to1518OctetsNo++;
  }
  else
  {
    /* No action required */
  }
  #endif /* (ETH_GET_ETHER_STATS_API == STD_ON) */

#if defined(USE_ETHER_CA_ENABLE) /* Use CA only */
  Eth_CacheFlush_RxBuff(pFrame->FrameAddr);
#endif

  /* Exclude the header before passing to upper layer */
  /* Cast to be compatible with type of input parameter */
  EthIf_RxIndication(CtrlIdx, LddFrameType, LblBroadcast, LaaSourceAddress,
    /* MISRA Violation: START Msg(4:0306)-2 */
    ((Eth_DataType *)((pFrame->FrameAddr) + ETH_HEADER_SIZE)),
    /* MISRA Violation: END Msg(4:0306)-2 */
    ((uint16)pFrame->FrameLength - ETH_HEADER_SIZE));
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
** Function Name         : Eth_IsRxFrameValid
**
** Service ID            : NA
**
** Description           : Compare Mac Address Frame just received with
**                         Broadcast and Controller Mac Address
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         pRxFrame - pointer to the Frame just received
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : ETH_TRUE  - Frame DA is Broadcast or matches with the
**                                     Source Address of this Controller
**                         ETH_FALSE - Otherwise
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBroadcastAddr, Eth_GaaDevice
**
** Functions invoked     : Eth_CompareMac
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */
/* TRACE[ETH_DDD_ACT_105] */
/* TRACE[ETH_DDD_INF_110] */
static FUNC(boolean, ETH_CODE_FAST) Eth_IsRxFrameValid
(
  uint8 CtrlIdx,
  CONSTP2CONST(volatile Eth_RxSingleFrameType, AUTOMATIC, ETH_APPL_CONST)
    pRxFrame
)
{
  VAR(boolean, AUTOMATIC) LblReturnValue;
  VAR(Std_ReturnType, AUTOMATIC) LddIsFrameAddrUnicast;
  VAR(Std_ReturnType, AUTOMATIC) LddIsFrameAddrBroadcast;

  /* MISRA Violation: START Msg(4:2982)-18 */
  LddIsFrameAddrUnicast = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  /* MISRA Violation: START Msg(4:2982)-18 */
  LddIsFrameAddrBroadcast = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-18 */
  if (NULL_PTR != pRxFrame)
  {
    /* Compare MAC address to filter address */
    LddIsFrameAddrUnicast = Eth_CompareMac(
      /* MISRA Violation: START Msg(4:0306)-2 */
      (P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST))(pRxFrame->FrameAddr),
      /* MISRA Violation: END Msg(4:0306)-2 */
      /* MISRA Violation: START Msg(4:0312)-4 */
      (const uint8 *)Eth_GaaDevice[CtrlIdx].MacAddr);
      /* MISRA Violation: END Msg(4:0312)-4 */
    /* Compare MAC address to Broadcast address */
    LddIsFrameAddrBroadcast = Eth_CompareMac(
      /* MISRA Violation: START Msg(4:0306)-2 */
      (P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST))(pRxFrame->FrameAddr),
      /* MISRA Violation: END Msg(4:0306)-2 */
      Eth_GaaBroadcastAddr);

    if ((E_OK == LddIsFrameAddrUnicast) || (E_OK == LddIsFrameAddrBroadcast))
    {
      /* Frame Accepted */
      LblReturnValue = ETH_TRUE;
    }
    else
    {
      /* Frame Refused */
      LblReturnValue = ETH_FALSE;
    }
  } /* (NULL_PTR != pRxFrame) */
  else
  {
    /* No Frame Received  */
    LblReturnValue = ETH_FALSE;
  } /* (NULL_PTR != pRxFrame) */
  return LblReturnValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-29 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-29 */

/*******************************************************************************
**                                            End of File                     **
*******************************************************************************/
