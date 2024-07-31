/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Version.h                                               */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains macros required for checking versions of modules        */
/* included by Eth Driver                                                     */
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
* V1.0.0:  8-Dec-2015  : Initial Version
* V1.0.1:  5-Aug-2016  : Remove Renesas specific number 59
*                        Unify version number
* V1.1.0: 23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_VERSION_H
#define ETH_VERSION_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for Eth.h inclusion and macro definitions */
#include "ether_api.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR Release version information */
#define ETH_VERSION_AR_RELEASE_MAJOR_VERSION     ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_VERSION_AR_RELEASE_MINOR_VERSION     ETH_AR_RELEASE_MINOR_VERSION
#define ETH_VERSION_AR_RELEASE_REVISION_VERSION  ETH_AR_RELEASE_REVISION_VERSION

/* File version information */
#define ETH_VERSION_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_VERSION_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* ETH_VERSION_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
