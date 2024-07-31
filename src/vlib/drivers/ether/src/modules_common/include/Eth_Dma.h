/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Dma.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of prototypes for internal functions of Ethernet DMAC Component. */
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
* V1.0.1:  05-Aug-2016  : Implement Write Verify Check code
*                         Remove Renesas specific number 59
*                         Unify version number
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  08-Dec-2016  : Implement read/write verify check
*                         Added functional safety: Payload CRC support
*                         Include RCar_Types.h
* V1.2.0:  07-Mar-2017  : Updated source code to support static memory
*                         allocation for descriptors and buffers
*                         via linker script (#68800)
* V1.2.1:  08-Mar-2017  : Updated MISRA-C checking
*                         Change H3_ES2_0 to H3_WS2_0
* V1.2.2:  22-Mar-2017  : Added macro ETH_VRR_GECMR_MASK and
*                         Eth_ControllerAccessCheck function (#73275)
* V1.2.3:  04-Apr-2017  : Removed Eth_ReconfSepFilter, Eth_IsRxFrameValid
* V1.2.4:  05-Apr-2017  : Added Eth_UnreadDataCheck, Eth_RxNormalDescDataCheck,
*                         Eth_RxExtDescDataCheck
* V1.2.5:  05-Apr-2017  : Removed Eth_RxBeProcess, Eth_RxNcProcess,
*                         Eth_RxSProcess function
*                         Added Eth_RxNormalQueueProcess, Eth_RxExtQueueProcess
*                         Added Eth_GetRxNormalDescAddr, Eth_GetRxExtDescAddr
* V1.2.6:  17-Apr-2017  : Change name of Eth_PayloadCRCAreaStartset to
*                         Eth_PayloadCRCAreaStartSet
* V1.2.7:  06-May-2017  : Added Eth_GetIncrementFromRatio and
*                         Eth_AdjustPTPIncrement functions prototype
* V1.2.8:  16-May-2017  : Updated source code based on FuSa review
* V1.2.9:  20-May-2017  : Removed functions Eth_SetPTPOffset, Eth_CompareHeader
*                         Added functions Eth_AdjustPTPOffset,Eth_AdjustPTPTime
* V1.3.0:  18-Jun-2017  : Remove Payload CRC Module and Error Detection Circuit
*                         (EDC)for Stream Buffer from R-Car V3M device
*                         Add functions Eth_TxQueueEmptyCheck, Eth_DMACResetHW
*                         Update memory mapping for functions which are related
*                         to interrupt
* V1.3.1:  08-Sep-2017  : Replace 'STATIC' keyword by 'static' keyword
* V1.3.2:  12-Sep-2017  : Remove Eth_DMACStructConfig and
*                         Eth_EthernetAvbIfBaseAddr functions
*                         Update prototype of Eth_RxQueueProcess, Eth_RxSet
*                         Eth_RxExtQueueProcess, Eth_RxNormalQueueProcess,
*                         Eth_RxDescChainUpdate, Eth_DescUpdate, Eth_RxQueueSet,
*                         Eth_DescTsUpdate, Eth_RxDescChainConfig, Eth_TxSet
*                         Eth_UnreadDataCheck, Eth_ErrIntMaskSet,
*                         Eth_CommonConfig, Eth_ConfigureDMAC functions
* V1.3.3:  28-Sep-2017  : Update Eth_SingleDescFrameSend function prototype
*                         Added Eth_TxDescEmptyCheck and Eth_WaitTxQueueEmpty
* V1.3.4:  04-Dec-2017  : Remove unused macro definitions
*                         Update memory mapping for function
*                         Eth_TxQueueEmptyCheck
*                         Move extern declaration of function Eth_TxBufferCheck
*                         from Eth.h to Eth_Dma.h
*                         Add extern declaration of functions:
*                         Eth_RxFilterProcess, Eth_RxCallEthIf,
*                         Eth_IsRxFrameValid, Eth_GetRxFilterMode
*                         Update type of operating status variable from uint8
*                         to Eth_OpStatusType
* V1.3.5:  15-Dec-2017    Update Compiler Abstraction
*                         Remove redundant input parameters
*                         Move declaration of static function to source file
* V1.3.6:  15-Dec-2017  : Remove function Eth_TxQueueEmptyCheck
*                         Add function Eth_WaitForOpModeChange
* V1.3.7:  23-Dec-2017  : Update prototype of Eth_RxQueueProcess
* V1.3.8:  25-Dec-2017  : Update prototype of Eth_DMACResetHW
* V1.3.9:  25-Dec-2017  : Remove function Eth_GetTimeOutValue
* V1.4.0:  26-Jan-2018  : Add branch when Device name is M3.
* V1.4.1:  01-Mar-2018  : Change prototype of Eth_RxQueueProcess and
*                         Eth_RxDescChainUpdate
* V1.4.2:  05-Mar-2018  : Remove function Eth_WaitForOpModeChange
* V1.4.3:  26-Apr-2018  : Remove input parameter Action in function
*                         Eth_WriteIntoSFPReg
*                         Remove function Eth_RxDescChainConfig
* V1.4.4:  10-May-2018  : Add pre-processor for M3N
* V1.4.5:  11-Jun-2018  : Invalidate "Payload CRC" for M3
*                         Invalidate "EDC for EthernetAVB-IF" for M3N
* V1.4.6:  26-Jun-2018  : Update Eth_SingleDescFrameSend to move step trigger
*                         buffer confirmation right before step trigger frame
*                         transmission
* V1.4.7:  09-Aug-2018  : Change name of macro ETH_TIC_TDPF_OFFSET to
*                         ETH_TIS_TDPF_OFFSET
                          Remove safety mechanism EDC for EthernetAVB-IF
* V1.4.8:  22-Aug-2018  : Invalidate "Payload CRC" for M3N
* V1.5.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_DMA_H
#define ETH_DMA_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "ether_api.h"
#include "Eth_Types.h"
#include "Eth_PBTypes.h"
#include "Eth_DMACRegStruct.h"
#include "Eth_RegReadWrite.h"
#include "Os.h"
#include "Dem.h"
#include "RCar_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_DMA_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_DMA_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_DMA_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_DMA_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_DMA_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      gPTP Macros                                           **
*******************************************************************************/
#define ETH_ONEBILLION    1000000000UL

/* Minimum GTI value possible */
#define GTI_MIN_VALUE     0x00100000UL
/* Maximum GTI value possible */
#define GTI_MAX_VALUE     0x0FFFFFFFUL

#define ETH_GTI_MASK      0x0FFFFFFFUL

#define ETH_MAX_TIME_STAMP_DELTA      (uint32)0xFFFFFFFFU

#define ETH_MAX_NANOSECOND (uint32)0x3B9AC9FFU

#define ETH_NANOSECOND_MASK (uint32)0x3FFFFFFFU

#define CCC_CSEL_MASK     0x00030000UL

/* MASK */
/* TCR=11 TCSS=00 */
#define ETH_GPTP_CAPTURE_MASK      0x0003UL

/* BIT */
#define ETH_GPTP_LTI_BIT 3U
#define ETH_GPTP_LTO_BIT 2U
#define ETH_GCCR_TCR_BIT0 0U

/* TCSS */
#define GPTP_GCCR_TCSS_BIT0 8U
#define GPTP_GCCR_TCSS_MASK 0x00000300UL

/* TIC */
#define ETH_TIC_MFUE_MASK   0x00000400UL
#define ETH_TIC_MFWE_MASK   0x00000800UL
#define ETH_TIC_MFUE_BIT    10U
#define ETH_TIC_MFWE_BIT    11U
#define ETH_TIC_TDPE_OFFSET 16U

/* TIS */
#define ETH_TIS_TDPF_OFFSET 16U

/* VRR - GECMR common bits check mask */
#define ETH_VRR_GECMR_MASK   0xFFFFFFFEUL

/* TSR */
#define ETH_TSR_TFFL_MASK    0x00000700UL
#define ETH_TSR_TFFL_BIT     8U

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_ConfigureDMAC
(
  uint8 LucCtrlIdx
);

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_OpModeChange
(
  uint8 LucCtrlIdx,
  Eth_OpStatusType LenMode
);

extern FUNC(Eth_OpStatusType, ETH_CODE_SLOW) Eth_OpModeGet(uint8 LucCtrlIdx);

extern FUNC(void, ETH_CODE_SLOW) Eth_DescConfig(uint8 LucCtrlIdx);

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxQueueSet
(
  uint8 LucCtrlIdx,
  uint32 LulQIndex
);

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxPaddingSet
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(Eth_PaddingConfigType, AUTOMATIC, ETH_APPL_CONST) LpPadding
);

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_SingleDescFrameSend
(
  uint8 LucCtrlIdx,
  const boolean blTxConfirmation,
  Eth_BufIdxType BufIdx,
  CONSTP2CONST(Eth_TxSingleFrameType, AUTOMATIC, ETH_APPL_CONST) LpFrame
);

#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxIntMaskSet(uint8 LucCtrlIdx);
#endif
extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxWarningIntMaskSet
(
  uint8 LucCtrlIdx,
  uint32 LulMask
);

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxQFullIntMaskSet
(
  uint8 LucCtrlIdx
);

#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_TxIntMaskSet(uint8 LucCtrlIdx);
#endif

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_GPtpIntMaskSet
(
  uint8 LucCtrlIdx,
  uint32 LulMask
);
#if ((ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) || \
  (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON))
extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_CommonLineIntMaskSet
(
  uint8 LucCtrlIdx
);
#endif
extern FUNC(void, ETH_CODE_SLOW) Eth_DisableController(uint8 CtrlIdx);
extern FUNC(void, ETH_CODE_SLOW) Eth_EnableController(uint8 CtrlIdx);

extern FUNC(void, ETH_CODE_SLOW) Eth_SetRxFilter(uint8 CtrlIdx);

extern FUNC(void, ETH_CODE_SLOW) Eth_WriteIntoSFPReg
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) LpMacAddrPtr,
  VAR(uint8, AUTOMATIC) ucQueueIdx
);

#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetPTPTime
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetPTPTime
(
  uint8 CtrlIdx,
  P2CONST(Eth_TimeStampType, AUTOMATIC, ETH_APPL_CONST) timeStampPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AdjustPTPOffset
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeIntDiffType, AUTOMATIC, ETH_APPL_CONST) timeOffsetPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AdjustPTPTime
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeIntDiffType, AUTOMATIC, ETH_APPL_CONST) timeOffsetPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AdjustPTPIncrement
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_RateRatioType, AUTOMATIC, ETH_APPL_CONST) rateRatioPtr
);

extern FUNC(void, ETH_CODE_SLOW) Eth_EnableTSFifo(uint8 CtrlIdx);

extern FUNC(void, ETH_CODE_SLOW) Eth_ReadTSFifo
(
  VAR(uint8, AUTOMATIC) CtrlIdx ,
  VAR(uint8, AUTOMATIC) BufIdx,
  P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
);
#endif

extern FUNC(void, ETH_CODE_SLOW) Eth_ReadTSDescriptor
(
  P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
);

#if (ETH_QOS_SUPPORT == STD_ON)
extern FUNC(void, ETH_CODE_SLOW) Eth_SetCbsParameter(uint8 CtrlIdx);
#endif

#if (ETH_DEVICE_NAME == RCAR_H3)
#if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
extern FUNC(void, ETH_CODE_SLOW) Eth_PayloadCRCAreaStartSet
(
  uint8 ucCtrlIdx,
  uint8 ucCrcAreaStart
);
#endif
#endif
extern FUNC(uint8, ETH_CODE_SLOW) Eth_UnreadDataCheck
(
  uint8 LucQIdx
);
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ControllerAccessCheck
(
  uint8 ucCtrlIdx
);

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_FAST
#include "Eth_MemMap.h"

extern FUNC(void, ETH_CODE_FAST) Eth_UFCounterDecrement
(
  uint8 ucCtrlIdx,
  uint8 ucRxQueueIdx
);

extern FUNC(Eth_ErrorType, ETH_CODE_FAST) Eth_RxQueueProcess
(
  uint8 LucCtrlIdx,
  uint8 LucQIdx,
  uint32 LulRxFramePtr,
  uint32 LulDescAddr
);

extern FUNC(uint32, ETH_CODE_FAST) Eth_UFCounterGet
(
  uint8 LucCtrlIdx,
  uint8 LucQIndex
);

extern FUNC(boolean, ETH_CODE_FAST) Eth_TxDescEmptyCheck
(
  Eth_BufIdxType TxBuffIdx
);

extern FUNC(void, ETH_CODE_FAST) Eth_TxBufferCheck( uint8 CtrlIdx );

extern FUNC(Std_ReturnType, ETH_CODE_FAST) Eth_RxFilterProcess
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(volatile Eth_RxSingleFrameType, AUTOMATIC, ETH_APPL_CONST)
    pFrame,
  uint8 LucQIdx
);

extern FUNC(void, ETH_CODE_FAST) Eth_RxCallEthIf
(
  uint8 CtrlIdx,
  uint8 LucQIdx,
  CONSTP2VAR(volatile Eth_RxSingleFrameType, AUTOMATIC,ETH_APPL_DATA) pFrame
);
extern FUNC(void, ETH_CODE_FAST) Eth_RxDescChainUpdate
(
  uint32 LulDescPtr,
  uint8 LucQIdx
);

extern FUNC(Std_ReturnType, ETH_CODE_FAST) Eth_DMACResetHW(uint8 CtrlIdx);

extern void Eth_CacheInvalidate
(
  uint32 arrayAddr,
  size_t Size
);

extern void Eth_CacheFlush
(
  uint32 arrayAddr,
  size_t Size
);

#define ETH_STOP_SEC_CODE_FAST
#include "Eth_MemMap.h"
#endif /* ETH_DMA_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
