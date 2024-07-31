/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Stbe.h                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2016-2022 Renesas Electronics Corporation                     */
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
/*              Devices:        R-Car H3                                      */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
********************************************************************************
* V1.0.0:  08-Dec-2016  : Initial Version
* V1.1.0:  06-Apr-2017  : Change name of function Eth_EnableEDCProtection to
*                         Eth_StbeEnableEDCProtection
*                         Change name of macro ETH_EDC_ERROR_DETECT to
*                         ETH_STBE_EDC_ERROR_DETECT
*                         Add define ETH_STBE_RAM_SIZE
* V1.2.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                  MISRA C Rule Violations                                   **
*******************************************************************************/

#ifndef ETH_STBE_H
#define ETH_STBE_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "ether_api.h"
#include "Eth_Ram.h"
#include "Eth_Types.h"
#include "Eth_RegReadWrite.h"
#include "Eth_PBTypes.h"
/* Inclusion of RCar_Types.h file for FuSa related Macros */
#include "RCar_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_STBE_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_STBE_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_STBE_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_STBE_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_STBE_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/*******************************************************************************
**                      STBE Macros                                           **
*******************************************************************************/

/* Base Address for the STBE in R-Car H3 Soc */
#define ETH_STBE_RAM_BASE   0xE6A00000UL
#define ETH_STBE_REG_BASE   0xE6A10000UL
#define ETH_STBE_RAM_SIZE   0x00010000UL

/* CTLR*/
#define ETH_CTLR       (uint32)0x1c0
#define ETH_CTLR_EDCEN_BIT               0U

/* Macro for EDC error notification target disable */
#define EDC_DISABLE                      0U
/* Macro for external pin */
#define EXTERNAL_PIN                     1U
/* Macro for internal interrupt */
#define INTERNAL_IRQ                     2U

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#if (ETH_STBE_EDC_ERROR_DETECT == STD_ON)
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern FUNC(void, ETH_CODE_SLOW) Eth_StbeEnableEDCProtection(void);

#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"
#endif
#endif  /* ETH_STBE_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
