/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth.h                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains macros, ETH type definitions, structure data types and  */
/* API function prototypes of ETH Driver                                      */
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
*                         Update Eth_GetVersionInfo API
*                         Unify version number
* V1.1.0:  30-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
*                         Added new 4.2.2 APIs: Eth_GetDropCount,
*                         Eth_GetEtherStats, Eth_MainFunction
*                         Merged Eth_ControllerInit into Eth_Init
* V1.1.1:  08-Dec-2016  : Implement read/write verify check
* V1.2.0:  06-Apr-2017  : Add define ETH_MODULESTOPCHECK_SID,
*                         ETH_GETCOUNTERSTATE_SID
*                         Add extern Eth_ModuleStopCheck, Eth_GetCounterState
*                         Remove define ETH_RXIRQHDLR_01_SID and
*                         ETH_TXIRQHDLR_01_SID
*                         Update DET Error Codes
*                         Update MISRA C RULE VIOLATION
*                         Update pre-compile option ETH_UPDATE_PHYS_ADDR_FILTER
*                         for Eth_GucBroadcastAddr and Eth_CompareMac
* V1.2.1:  16-May-2017  : Add Eth_DMACReset prototype and service id
*                         Included Eth_GeneralTypes.h
* V1.2.2:  17-May-2017  : Changed the name of Eth_TxBufferRelease to
*                         Eth_TxBufferCheck
* V1.2.3:  17-May-2017  : Update Service ID of Eth_MainFunction and
*                         Eth_DMACReset
* V1.2.4:  18-Jun-2017  : Update memory mapping for functions which are related
*                         to interrupt
* V1.2.5:  12-Sep-2017  : Remove the extern declaration of Eth_GucBroadcastAddr
* V1.2.6:  27-Nov-2017  : Move extern declaration of functions:
*                         - Eth_TxBufferCheck from Eth.h to Eth_Dma.h
*                         - Eth_RxIrqHdlr_01, Eth_TxIrqHdlr_01 from Eth.h to
*                           Eth_Irq.h
*                         - Eth_CompareMac from Eth.h to Eth_Mac.h
* V1.2.7:  15-Dec-2017    Update Compiler Abstraction
*                         Remove redundant input parameters
* V1.2.8:  23-Dec-2017  : Extern cache flush and cache invalidate functions
* V1.2.9:  25-Dec-2017  : Added ETH_E_INITIALIZED DET error code
* V1.2.10: 15-Jan-2018  : Update Memory Mapping of Global data
* V1.2.11: 22-Mar-2018  : Add ETH_AVBCH23ISR_SID and ETH_E_TIMESTAMP_FIFO_FULL
* V1.2.12: 22-Jan-2020  : Remove prototype declaration  Eth_MainFunction
* V1.3.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_H
#define ETH_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* To publish the type Eth_ConfigType */
#include "Eth_Types.h"
/* Included for general Eth type declarations */
#include "Eth_GeneralTypes.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* Version identification */
#define ETH_VENDOR_ID          ETH_VENDOR_ID_VALUE
#define ETH_MODULE_ID          (uint16)ETH_MODULE_ID_VALUE
#define ETH_INSTANCE_ID        (uint8)ETH_INSTANCE_ID_VALUE


/* Functionality related to R4 */
/* AUTOSAR release version information */
#define ETH_AR_RELEASE_MAJOR_VERSION    4
#define ETH_AR_RELEASE_MINOR_VERSION    2
#define ETH_AR_RELEASE_REVISION_VERSION 2


/* Module Software version information */
#define ETH_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION_VALUE
#define ETH_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION_VALUE
#define ETH_SW_PATCH_VERSION    ETH_SW_PATCH_VERSION_VALUE
/*******************************************************************************
**                      API Service Id Macros                                 **
*******************************************************************************/
/* Service Id of Eth_Init */
#define ETH_INIT_SID                        (uint8)0x01
/* Service Id of Eth_ControllerInit */
#define ETH_CONTROLLERINIT_SID              (uint8)0x02
/* Service Id of Eth_SetControllerMode */
#define ETH_SETCONTROLLERMODE_SID           (uint8)0x03
/* Service Id of Eth_GetControllerMode */
#define ETH_GETCONTROLLERMODE_SID           (uint8)0x04
/* Service Id of Eth_WriteMii */
#define ETH_WRITEMII_SID                    (uint8)0x05
/* Service Id of Eth_ReadMii */
#define ETH_READMII_SID                     (uint8)0x06
/* Service Id of Eth_GetPhysAddr */
#define ETH_GETPHYSADDR_SID                 (uint8)0x08
/* Service Id of Eth_ProvideTxBuffer */
#define ETH_PROVIDETXBUFFER_SID             (uint8)0x09
/* Service Id of Eth_Transmit */
#define ETH_TRANSMIT_SID                    (uint8)0xA
/* Service Id of Eth_Receive */
#define ETH_RECEIVE_SID                     (uint8)0xB
/* Service Id of Eth_TxConfirmation */
#define ETH_TXCONFIRMATION_SID              (uint8)0xC
/* Service Id of Eth_GetVersionInfo */
#define ETH_GET_VERSION_INFO_SID            (uint8)0xD
/*Service Id of Eth_UpdatePhysAddrFilter */
#define ETH_UPDATEPHYSADDRFILTER_SID        (uint8)0x12
/* Service Id of Eth_SetPhysAddr */
#define ETH_SETPHYSADDR_SID                 (uint8)0x13

/* Service Id of Eth_GetDropCount */
#define ETH_GETDROPCOUNT_SID                (uint8)0x14

/* Service Id of Eth_GetEtherStats */
#define ETH_GETETHERSTATS_SID               (uint8)0x15

/*********gPTP TimeStamp APIs *****************/
/*Service Id of Eth_GetCurrentTime*/
#define ETH_GETCURRENTTIME_SID              (uint8)0x16

/*Service Id of Eth_EnableEgressTimeStamp  */
#define ETH_ENEGRESSTS_SID                  (uint8)0x17

/*Service Id of Eth_GetEgressTimeStamp  */
#define ETH_GETEGRESSTS_SID                 (uint8)0x18

/*Service Id of Eth_GetIngressTimeStamp  */
#define ETH_GETINGRESSTS_SID                (uint8)0x19

/*Service Id of Eth_SetCorrectionTime  */
#define ETH_SETCORRECTIONTIME_SID           (uint8)0x1A

/*Service Id of Eth_SetGlobalTime  */
#define ETH_SETGLOBALTIME_SID               (uint8)0x1B

/*Service Id of Eth_ModuleStopCheck  */
#define ETH_MODULESTOPCHECK_SID             (uint8)0x1C

/* Service Id of Eth_MainFunction */
#define ETH_MAINFUNCTION_SID                (uint8)0x20

/*Service Id of Eth_DMACReset  */
#define ETH_DMACRESET_SID                   (uint8)0x21

/*Service Id of Eth_AvbCh22Isr  */
#define ETH_AVBCH23ISR_SID                  (uint8)0x22

/*******************************************************************************
**                      DET Error Codes                                       **
*******************************************************************************/
/* Invalid controller index */
#define ETH_E_INV_CTRL_IDX                  (uint8)0x01

/* Ethernet module or controller was not initialized */
#define ETH_E_NOT_INITIALIZED               (uint8)0x02

/* Invalid pointer in parameter list. */
#define ETH_E_PARAM_POINTER                 (uint8)0x03

/* A wrong parameter passed to the APIs */
#define ETH_E_INV_PARAM                     (uint8)0x04

/* Invalid mode */
#define ETH_E_INV_MODE                      (uint8)0x05

/* Ethernet module or controller has already been initialized */
#define ETH_E_INITIALIZED                   (uint8)0x06

/* Ethernet module failed to secure memory*/
#define ETH_E_GET_MEMORY                    (uint8)0x07

/* When valid Database is not available */
#define ETH_E_INVALID_DATABASE              (uint8)0xEF

/* Timestamp FIFO Full */
#define ETH_E_TIMESTAMP_FIFO_FULL           (uint8)0x07
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define OSAL_ALIGNMENT (size_t)64u
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/


/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
extern void CR7_Invalidate_DCache_By_Addr(uint32 start_addr, uint32 size);
extern void CR7_Flush_DCache_By_Addr(uint32 start_addr, uint32 size);
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#if (ETH_VERSION_INFO_API == STD_ON)
extern FUNC(void, ETH_CODE_SLOW) Eth_GetVersionInfo
(
  CONSTP2VAR(Std_VersionInfoType, AUTOMATIC, ETH_APPL_DATA) VersionInfoPtr
);
#endif

extern FUNC(void, ETH_CODE_SLOW) Eth_Init
(
  CONSTP2CONST(Eth_ConfigType, AUTOMATIC, ETH_APPL_CONST) CfgPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetControllerMode
(
  uint8 CtrlIdx, Eth_ModeType CtrlMode
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetControllerMode
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_ModeType, AUTOMATIC, ETH_APPL_DATA) CtrlModePtr
);

extern FUNC(void, ETH_CODE_SLOW) Eth_GetPhysAddr
(
  uint8 CtrlIdx, CONSTP2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr
);

#if (ETH_CTRL_ENABLE_MII == STD_ON)
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_WriteMii
(
  uint8 CtrlIdx, uint8 TrcvIdx, uint8 RegIdx, uint16 RegVal
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ReadMii
(
  uint8 CtrlIdx, uint8 TrcvIdx, uint8 RegIdx,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) RegValPtr
);
#endif

#if (ETH_GET_DROP_COUNT_API == STD_ON)
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetDropCount
(
  uint8 CtrlIdx,
  uint8 CountValues,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) DropCount
);
#endif

#if (ETH_GET_ETHER_STATS_API == STD_ON)
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetEtherStats
(
  uint8 CtrlIdx,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) etherStats
);
#endif

#if (ETH_QOS_SUPPORT == STD_ON)
extern FUNC(BufReq_ReturnType, ETH_CODE_SLOW) Eth_ProvideTxBuffer
(
  uint8 CtrlIdx,
  uint8 Priority,
  CONSTP2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
  CONSTP2VAR(P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA),
    AUTOMATIC, ETH_APPL_DATA) BufPtr,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr
);
#else
extern FUNC(BufReq_ReturnType, ETH_CODE_SLOW) Eth_ProvideTxBuffer
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
  CONSTP2VAR(P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA),
    AUTOMATIC, ETH_APPL_DATA) BufPtr,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr);
#endif

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_Transmit
(
  uint8 CtrlIdx,
  Eth_BufIdxType BufIdx,
  Eth_FrameType FrameType,
  boolean TxConfirmation,
  uint16 LenByte,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) PhysAddrPtr
);

#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_OFF)
extern FUNC(void, ETH_CODE_SLOW) Eth_Receive
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_RxStatusType, AUTOMATIC, ETH_APPL_DATA) RxStatusPtr
);
#endif

extern FUNC(void, ETH_CODE_SLOW) Eth_TxConfirmation( uint8 CtrlIdx );

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_FAST
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_FAST) Eth_DMACReset(void);
#define ETH_STOP_SEC_CODE_FAST
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_SLOW) Eth_SetPhysAddr
(
  uint8 CtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) PhysAddrPtr
);

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_UpdatePhysAddrFilter
(
  uint8 CtrlIdx, CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) PhysAddrPtr,
  Eth_FilterActionType Action
);
#endif

#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetCurrentTime
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
);

extern FUNC(void, ETH_CODE_SLOW) Eth_EnableEgressTimeStamp
(
  uint8 CtrlIdx,
  uint8 BufIdx
);

extern FUNC(void, ETH_CODE_SLOW) Eth_GetEgressTimeStamp
(
  uint8 CtrlIdx, uint8 BufIdx,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
);

extern FUNC(void, ETH_CODE_SLOW) Eth_GetIngressTimeStamp
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_DataType, AUTOMATIC, ETH_APPL_CONST) DataPtr,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
);

extern FUNC(void, ETH_CODE_SLOW) Eth_SetCorrectionTime
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeIntDiffType, AUTOMATIC, ETH_APPL_CONST) timeOffsetPtr,
  CONSTP2CONST(Eth_RateRatioType, AUTOMATIC, ETH_APPL_CONST) rateRatioPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetGlobalTime
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeStampType, AUTOMATIC, ETH_APPL_CONST) timeStampPtr
);
#endif /* (ETH_GLOBAL_TIME_SUPPORT==STD_ON) */

#if (ETH_MODULE_STOP_CHECK == STD_ON)
extern FUNC(void, ETH_CODE_SLOW) Eth_ModuleStopCheck(void);
#endif

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CONFIG_DATA_32
#include "Eth_MemMap.h"
/* Declaration for ETH Database */
extern CONST (Eth_ConfigType, ETH_CONFIG_DATA)
  Eth_GstConfiguration[ETH_TOTAL_CONFIG];
extern CONST (Eth_TxQueueType, ETH_CONFIG_DATA)
  Eth_GstTxQueueConfig[ETH_TX_QUEUE_COUNT];
#define ETH_STOP_SEC_CONFIG_DATA_32
#include "Eth_MemMap.h"

#define ETH_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Eth_MemMap.h"
extern CONST (Eth_RxQueueType, ETH_CONFIG_DATA)
  Eth_GstRxQueueConfig[ETH_RX_QUEUE_COUNT];
#define ETH_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_CONFIG_DATA_8
#include "Eth_MemMap.h"
#if (ETH_QOS_SUPPORT == STD_ON)
extern CONST (Eth_PriorityMappingType, ETH_CONFIG_DATA)
  Eth_GstCtrlPriorityTable[ETH_TX_PRIORITY_COUNT];
#endif
#define ETH_STOP_SEC_CONFIG_DATA_8
#include "Eth_MemMap.h"

#endif /* ETH_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
