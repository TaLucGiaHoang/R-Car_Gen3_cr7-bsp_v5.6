/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Common.h                                                */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains header of common APIs for the module                    */
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
/*              Devices:        R-Car Series, 3rd Generation                  */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
********************************************************************************
* V1.0.0:  13-Dec-2017  : Initial Version
* V1.1.0:  25-Dec-2017  : Add function Eth_WaitForDelay
* V1.1.1:  26-Apr-2019  : Update type of OsCounterID from uint8 to CounterType
*                         in functions Eth_WaitForTimeout and Eth_WaitForDelay
* V1.2.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/
#ifndef ETH_COMMON_H
#define ETH_COMMON_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Standard header file */
#include "Std_Types.h"
/* Included for declaration of the function Dem_ReportErrorStatus() */
#include "Dem.h"
/* Os header file */
#include "Os.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_COMMON_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_COMMON_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_COMMON_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_COMMON_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_COMMON_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/* Zero */
#define ETH_COMMON_ZERO                   (uint32)0
#define alignment64(buff_size) ((((uintptr_t)(buff_size) % 64) == 0) ? (buff_size) : (((uint32)((uintptr_t)(buff_size) / 64) + 1) * 64))
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"
extern FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_WaitForTimeout
(
  const Dem_EventIdType TimeoutID,
  const TickType TimeoutValue,
  const CounterType OsCounterID,
  CONSTP2CONST(volatile uint32, AUTOMATIC, ETH_APPL_CONST)RegAddr,
  const uint32 Mask,
  const uint32 ExpectedValue
);
#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"
extern FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_WaitForDelay
(
  const TickType DelayValue,
  const CounterType OsCounterID
);
#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"
#endif /* ETH_COMMON_H */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
