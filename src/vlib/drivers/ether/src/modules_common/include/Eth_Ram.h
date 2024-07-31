/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Ram.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains Ethernet driver global variables and the                */
/* Ram Allocation functions                                                   */
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
*                         Unify version number
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  08-Dec-2016  : Change Compiler Abstraction and Memory Mapping
* V1.2.0:  07-Mar-2017  : Updated source code to support static memory
*                         allocation for descriptors and buffers
*                         via linker script (#68800)
* V1.2.1:  08-Mar-2017  : Updated MISRA-C checking
* V1.2.2:  22-Mar-2017  : Removed Eth_GblTxConfirm (#77583)
* V1.2.3:  04-Apr-2017  : Removed Eth_GucCurrentRxQueueIdx, Eth_GblRxFrameValid
*                         Added Eth_GstControllerFilterAddr and
*                         Eth_GenAddrFilterMode
* V1.2.4:  05-Apr-2017  : Added Eth_GucRxMultiBuffFrame, Eth_GucPayloadCrc,
*                         Eth_RamInit
* V1.2.5:  13-May-2017  : Added Eth_GetTxBuffAddr, Eth_GetRxBuffAddr functions
*                         Removed ETH_RXBUF_PTR, ETH_TXBUF_PTR macros
* V1.2.6:  17-May-2017  : Removed Eth_GulLastCdaDescriptor
* V1.2.7:  17-Jun-2017  : Added volatile to global variables which can be
*                         accessed in multiple context
* V1.2.8:  18-Jun-2017  : Remove Payload CRC Module from R-Car V3M device
*                         Update memory mapping for global variable
*                         Eth_GucPayloadCrc and functions which are related
*                         to interrupt
* V1.2.9:  12-Sep-2017  : Remove extern declaration of Eth_GstRxConfig,
*                         Eth_GstAvbConfig, Eth_GstMacConfig, Eth_GstQConfig
*                         and Eth_RamInit
* V1.2.10: 27-Nov-2017  : Add extern declaration of global arrays
*                         Eth_GaaBroadcastAddr, Eth_GaaNullAddr
* V1.2.11: 15-Dec-2017  : Update Compiler Abstraction
* V1.2.12: 23-Dec-2017  : Update size Eth_GucRxBuff, Eth_GucTxBuff and
*                         Eth_GucRxMultiBuffFrame to align 32 bytes
* V1.2.13: 15-Jan-2018  : Update Memory Mapping of Global data
* V1.2.14: 26-Jan-2018  : Add branch when Device name is M3.
* V1.2.15: 13-Mar-2018  : Add ETH_UNROLL_LOOP use for Eth_RamCopy
* V1.2.16: 10-May-2018  : Add pre-processor for M3N
* V1.2.17: 11-Jun-2018  : Invalidate "Payload CRC" for M3
* V1.2.18: 19-Aug-2018  : Update MISRA C rule violations
*                         Invalidate "Payload CRC" for M3N
* V1.3.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/******************************************************************************/
#ifndef ETH_RAM_H
#define ETH_RAM_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "ether_api.h"
#include "RCar_Types.h"

#include "rcar-xos/osal/r_osal.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_RAM_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_RAM_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_RAM_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_RAM_SW_MAJOR_VERSION  ETH_SW_MAJOR_VERSION
#define ETH_RAM_SW_MINOR_VERSION  ETH_SW_MINOR_VERSION
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define ETH_MEMORY_ALIGN_SIZE (uint32)(4)

#define ETH_CACHE_LINE_SIZE (uint32)(64)

#define ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN \
  (((uint32)ETH_CTRL_RX_BUF_LEN_BYTE) + \
  (ETH_CACHE_LINE_SIZE - ((uint32)ETH_CTRL_RX_BUF_LEN_BYTE % \
  ETH_CACHE_LINE_SIZE)))

#define ETH_CTRL_TX_BUF_LEN_CACHE_ALIGN \
  (((uint32)ETH_CTRL_TX_BUF_LEN_BYTE) + \
  (ETH_CACHE_LINE_SIZE - ((uint32)ETH_CTRL_TX_BUF_LEN_BYTE % \
  ETH_CACHE_LINE_SIZE)))

#define ETH_RX_BUF_SIZE_CACHE_ALIGN \
  (ETH_RX_QUEUE_COUNT * ETH_RX_BUF_TOTAL * ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN)

#define ETH_TX_BUF_SIZE_CACHE_ALIGN \
  (ETH_TX_BUF_TOTAL * ETH_CTRL_TX_BUF_LEN_CACHE_ALIGN)

#define ETH_VLFRAME_SIZE_CACHE_ALIGN \
  ((ETH_VLFRAME_SIZE) + \
  (ETH_CACHE_LINE_SIZE - (ETH_VLFRAME_SIZE % ETH_CACHE_LINE_SIZE)))

#define ETH_UNROLL_LOOP             (uint32) 0x00000004UL
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
#define ETH_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_MemMap.h"
/* Use to store a state change to notify upper layer */
/* TRACE [ETH_DDD_GBL_029] */
extern volatile VAR(Eth_MainFunctionType, ETH_VAR_CLEARED)
  Eth_GenMainFunctionState;
#define ETH_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_INIT_UNSPECIFIED
#include "Eth_MemMap.h"
/* Global variable to store  Controller State*/
/* TRACE [ETH_DDD_GBL_011] */
extern volatile VAR(Eth_StateType, ETH_VAR_INIT) Eth_GenDriverState;
#define ETH_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_INIT_UNSPECIFIED
#include "Eth_MemMap.h"
/* Global variable to store store the controller Mode */
/* TRACE [ETH_DDD_GBL_012] */
extern volatile VAR(Eth_ModeType, ETH_VAR_INIT) Eth_GenControllerMode;
#define ETH_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"
/* Global variable to store pointer to Configuration */
/* TRACE [ETH_DDD_GBL_001] */
extern P2CONST(Eth_ConfigType, ETH_VAR_CLEARED, ETH_APPL_CONST) Eth_GpConfigPtr;
#define ETH_STOP_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_003] */
extern volatile VAR(Eth_GaaDeviceType, ETH_VAR_CLEARED)
  Eth_GaaDevice[ETH_MACRO_NUM];
#define ETH_STOP_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_004] */
extern volatile VAR(Eth_BufHandlerType, ETH_VAR_CLEARED)
  Eth_GstBufferLock[ETH_TX_BUF_TOTAL];
#define ETH_STOP_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
#define ETH_START_SEC_VAR_CLEARED_8
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_023] */
extern volatile VAR(Eth_RxFilterAddrType, ETH_VAR_CLEARED)
  Eth_GstControllerFilterAddr[ETH_MAX_CTRLS_SUPPORTED];
#define ETH_STOP_SEC_VAR_CLEARED_8
#include "Eth_MemMap.h"
#endif

#define ETH_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_MemMap.h"
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
/* TRACE [ETH_DDD_GBL_024] */
extern volatile VAR(Eth_FilterModeType, ETH_VAR_CLEARED)
  Eth_GenAddrFilterMode[ETH_MAX_CTRLS_SUPPORTED];
#endif
#define ETH_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_8
#include "Eth_MemMap.h"
/* Flag to indicate the payload CRC status */
#if (ETH_DEVICE_NAME == RCAR_H3)
#if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
/* H3 TRACE [ETH_DDD_GBL_032] */
extern volatile VAR(uint8, ETH_VAR_CLEARED)
  Eth_GaaPayloadCrc[ETH_RX_QUEUE_COUNT];
#endif
#endif
#define ETH_STOP_SEC_VAR_CLEARED_8
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_DESCRIPTOR_32
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_016] */
extern VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstDescTable[ETH_TXQ_MAXNUM + ETH_RXQ_MAXNUM];
/* TRACE [ETH_DDD_GBL_015] */
#if (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
extern VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxBeQueue[ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#else
extern VAR(Eth_ExtDataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxBeQueue[ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#endif
#if (ETH_RX_NCQUEUE_NUM != ETH_QUEUE_SIZE_ZERO)
/* TRACE [ETH_DDD_GBL_017] */
extern VAR(Eth_ExtDataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxNcQueue[ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#endif
#if (ETH_RX_SQUEUE_NUM != ETH_QUEUE_SIZE_ZERO)
/* TRACE [ETH_DDD_GBL_018] */
#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF)
extern VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxStreamQueue[ETH_RX_SQUEUE_NUM]
  [ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#else
extern VAR(Eth_ExtDataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxStreamQueue[ETH_RX_SQUEUE_NUM]
  [ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#endif
#endif
#if (ETH_TX_QUEUE_COUNT != ETH_QUEUE_SIZE_ZERO)
/* TRACE [ETH_DDD_GBL_019] */
extern VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstTxQueue[ETH_TX_QUEUE_COUNT]
  [ETH_TX_BUF_TOTAL + ETH_EXTRA_FOR_EOSDESC + ETH_EXTRA_FOR_LINKDESC];
#endif
#define ETH_STOP_SEC_VAR_CLEARED_DESCRIPTOR_32
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_020] */
/*extern VAR(uint8, ETH_VAR_CLEARED) Eth_GucRxBuff
  [ETH_RX_BUF_SIZE_CACHE_ALIGN];*/
extern VAR(uint8,ETH_VAR_CLEARED) *Eth_GucRxBuff;
/* TRACE [ETH_DDD_GBL_021] */
/*extern VAR(uint8, ETH_VAR_CLEARED)
  Eth_GucTxBuff[ETH_TX_BUF_SIZE_CACHE_ALIGN];*/
/* TRACE [ETH_DDD_GBL_022] */
extern VAR(uint8,ETH_VAR_CLEARED) *Eth_GucTxBuff;
extern VAR(uint8, ETH_VAR_CLEARED)
  Eth_GucRxMultiBuffFrame[ETH_RX_QUEUE_COUNT][ETH_VLFRAME_SIZE_CACHE_ALIGN];
#define ETH_STOP_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"
#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
/* This is the address of the Descriptor which has a TimeStamp just received */
/* TRACE [ETH_DDD_GBL_033] */
extern P2VAR(volatile Eth_ExtDataDescType, ETH_VAR_CLEARED, ETH_APPL_DATA)
  Eth_GaaTempTsDescr[ETH_RX_QUEUE_COUNT];
/* This is the TimeStamp just read and stored */
/* TRACE [ETH_DDD_GBL_031] */
extern volatile VAR(Eth_TimeStampStoreType, ETH_VAR_CLEARED)
  Eth_GaaTempTimeStamp[ETH_RX_QUEUE_COUNT];
#endif
#define ETH_STOP_SEC_VAR_CLEARED_32
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_CLEARED_8
#include "Eth_MemMap.h"
#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
/* This stores the Queue index of TimeStamp */
/* TRACE [ETH_DDD_GBL_030] */
extern volatile VAR(uint8, ETH_VAR_CLEARED) Eth_GucTsQueueIdx;
#endif
#define ETH_STOP_SEC_VAR_CLEARED_8
#include "Eth_MemMap.h"

#define ETH_START_SEC_CONST_8
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_026] */
extern CONST(uint8, ETH_CONST) Eth_GaaBroadcastAddr[ETH_MACADDR_SIZE];

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
/* TRACE [ETH_DDD_GBL_027] */
extern CONST(uint8, ETH_CONST) Eth_GaaNullAddr[ETH_MACADDR_SIZE];
#endif
#define ETH_STOP_SEC_CONST_8
#include "Eth_MemMap.h"
/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern FUNC(uint32, ETH_CODE_SLOW) Eth_GetTxBuffAddr
(
  VAR(uint32, AUTOMATIC) ulBuffIdx
);

extern FUNC(uint32, ETH_CODE_SLOW) Eth_GetRxBuffAddr
(
  VAR(uint8, AUTOMATIC) ucQueueIdx,
  VAR(uint8, AUTOMATIC) ucBuffIdx
);

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_FAST
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_FAST) Eth_RamCopy
(
  CONSTP2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) pDst,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pSrc,
  VAR(uint32, AUTOMATIC) ulSize
);
#define ETH_STOP_SEC_CODE_FAST
#include "Eth_MemMap.h"
#endif /* ETH_RAM_H  */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
