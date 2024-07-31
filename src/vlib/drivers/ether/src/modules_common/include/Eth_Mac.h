/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Mac.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of prototypes for internal functions of Ethernet MAC Component.  */
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
*                         Unify version number
* V1.1.0:  30-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
*                         Added new functions: Eth_RetrieveMacCounter,
*                         Eth_RetrieveStatistics, Eth_ReportExtDem,
*                         Eth_GetFrameLostStatus, Eth_GetControllerErrorSts
*                         Added new macros: ETH_RECEIVE_FIFO_FULL,
*                         ETH_NOT_AVAILABLE, ETH_MAC_MAXCOUNTERS,
*                         ETH_MAXSTATISTICS
* V1.1.1:  08-Dec-2016  : Implement read/write verify check
*                         Added macro ETH_MAC_MAXCOUNTER_VAL
* V1.1.2:  10-Jan-2017  : Remove functions: Eth_GetFrameLostStatus
*                         Eth_GetControllerErrorSts, Eth_GetCounterValue
* V1.1.3:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.4:  16-May-2017  : Updated source code based on FuSa review
* V1.1.5:  17-May-2017  : Changed maximum MAC counters available from 18 to 17
* V1.1.6:  18-Jun-2017  : Add extern for function Eth_SetRxFilterMode
* V1.1.7:  12-Sep-2017  : Remove Eth_MacStructConfig and modify prototype of
*                         Eth_MacInit function
* V1.1.8:  05-Oct-2017  : Rename Eth_CalcMacAddr to Eth_GetMacAddr
* V1.1.9:  04-Dec-2017  : Remove unused macro definitions
*                         Update memory mapping for function Eth_GetMacAddr
*                         Move extern declaration of function Eth_CompareMac
*                         from Eth.h to Eth_Mac.h
*                         Update type of LenOption from uint8 to Eth_OptionType
* V1.1.10: 15-Dec-2017    Update Compiler Abstraction
* V1.1.11: 14-Mar-2018  : Remove function Eth_SetRxFilterMode
*                         Add functions Eth_EnablePromMode, Eth_DisablePromMode
* V1.2.0 : 23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_MAC_H
#define ETH_MAC_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "ether_api.h"
#include "Eth_Types.h"
#include "Eth_MACRegStruct.h"
#include "Eth_RegReadWrite.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_MAC_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_MAC_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_MAC_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_MAC_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_MAC_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/* Macro to define a not available MAC counter to get a statistic */
#define ETH_NOT_AVAILABLE          (uint32)0xFFFFFFFFU

/* Maximum MAC COUNTERS available */
#define ETH_MAC_MAXCOUNTERS        17U

/*Maximum Statistics Required */
#define ETH_MAXSTATISTICS          17U

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern FUNC(void, ETH_CODE_SLOW) Eth_MacCommunicationSet
(
  uint8 LucCtrlIdx,
  Eth_DirectionType LenDirection,
  Eth_OptionType LenOption
);

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_MacInit
(
  uint8 LucCtrlIdx
);

extern FUNC(void, ETH_CODE_SLOW) Eth_StoreMacIntoReg
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) LpMacAddrPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_CopyMacAddress
(
  CONSTP2VAR(volatile uint8, AUTOMATIC, ETH_APPL_DATA) pDst,
  CONSTP2CONST(volatile uint8, AUTOMATIC, ETH_APPL_CONST) pSrc
);

/* Retrieve Mac Counter Register */
#if (ETH_GET_DROP_COUNT_API == STD_ON)
extern FUNC(void, ETH_CODE_SLOW) Eth_RetrieveMacCounter
(
  uint8 LucCtrlIdx,
  uint8 CountValues,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) LpDropCount
);
#endif

/* Retrieve Statistics Counter */
#if (ETH_GET_ETHER_STATS_API == STD_ON)
extern FUNC (void, ETH_CODE_SLOW) Eth_RetrieveStatistics
(
  uint8 LucCtrlIdx,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) LpEtherStats
);
#endif

extern FUNC(void, ETH_CODE_SLOW) Eth_ReportExtDem(uint8 LucCtrlIdx);

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
extern FUNC(void, ETH_CODE_SLOW) Eth_EnablePromMode(uint8 CtrlIdx);

extern FUNC(void, ETH_CODE_SLOW) Eth_DisablePromMode(uint8 CtrlIdx);
#endif

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_FAST
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_FAST) Eth_GetMacAddr
(
  uint8 LucCtrlIdx,
  CONSTP2VAR(uint8, AUTOMATIC, ETH_APPL_DATA)LpMacAddrPtr
);

extern FUNC(Std_ReturnType, ETH_CODE_FAST) Eth_CompareMac
(
  CONSTP2CONST(volatile uint8, AUTOMATIC, ETH_APPL_CONST) pSrcMac,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pDstMac
);
#define ETH_STOP_SEC_CODE_FAST
#include "Eth_MemMap.h"
#endif  /* ETH_MAC_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
