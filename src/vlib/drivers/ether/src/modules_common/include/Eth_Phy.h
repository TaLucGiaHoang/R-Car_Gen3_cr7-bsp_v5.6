/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Phy.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of prototypes for internal functions of Ethernet Physical        */
/* Interface  Component.                                                      */
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
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  08-Dec-2016  : Removed unnecessary write 1 after Eth_PhyBusRelease()
*                         in Eth_ReadMii API
*                         Replaced Eth_PhyBusRelease by
*                         Eth_PhyIndependentBusRelease in  Eth_WriteMii API
*                         Added Eth_PhyIndependentBusRelease function
* V1.1.2:  27-Nov-2017  : Remove unused macro definitions
* V1.1.3:  15-Dec-2017  : Move declaration of static function to source file
* V1.1.4:  19-Dec-2017  : Add macro to check for Transceiver Index
* V1.1.5:  25-Dec-2017  : Add return value for Eth_PhyWrite0, Eth_PhyWrite1
*                         Eth_PhyWrite, Eth_PhyRead, Eth_MIIFrameHeaderSet,
*                         Eth_PhyBusRelease
* V1.2.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_PHY_H
#define ETH_PHY_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "ether_api.h"
#include "Eth_Types.h"
#include "Eth_RegReadWrite.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_PHY_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_PHY_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_PHY_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_PHY_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_PHY_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

#define ETH_TRCV_MAX_INDEX    (uint8)31

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyWrite1(uint8 LucCtrlIdx);
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyWrite0(uint8 LucCtrlIdx);
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyRead
(
  uint8 LucCtrlIdx,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) RegValPtr
);
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyWrite
(
  uint8 LucCtrlIdx,
  uint16 LusValue
);
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_MIIFrameHeaderSet
(
  uint8 LucCtrlIdx,
  uint8 LucPhyAddr,
  uint8 LucRegAddr,
  uint8 LucRW
);
extern FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyBusRelease( uint8 LucCtrlIdx);
extern FUNC(void, ETH_CODE_SLOW) Eth_PhyIndependentBusRelease(uint8 LucCtrlIdx);
#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"
#endif  /* ETH_PHY_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
