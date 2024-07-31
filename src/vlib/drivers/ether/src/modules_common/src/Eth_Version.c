/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Version.c                                               */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2016 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains code for version checking for modules included by Eth   */
/* Driver                                                                     */
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
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for Eth module version information and other types declarations */
#include "Eth_Version.h"

#if (ETH_DEV_ERROR_DETECT == STD_ON)
/* DET module version information is required only when DET is enabled */
#include "Det.h"
#endif

#if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
/*
* RTE module version information is required only when critical section
* protection is enabled
*/
#include "Rte.h"
#endif
/* DEM module version information */
#include "Dem.h"

/*******************************************************************************
**                      Version Information                                  **
*******************************************************************************/

/* AUTOSAR Release version information */
#define ETH_VERSION_C_AR_RELEASE_MAJOR_VERSION \
  ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_VERSION_C_AR_RELEASE_MINOR_VERSION \
  ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_VERSION_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_VERSION_C_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION_VALUE
#define ETH_VERSION_C_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/
#if (ETH_VERSION_AR_RELEASE_MAJOR_VERSION != \
  ETH_VERSION_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Version.c : Mismatch in Specification Major Version"
#endif

#if (ETH_VERSION_AR_RELEASE_MINOR_VERSION != \
  ETH_VERSION_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Version.c : Mismatch in Specification Minor Version"
#endif

#if (ETH_VERSION_AR_RELEASE_REVISION_VERSION != \
  ETH_VERSION_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Version.c : Mismatch in Specification Patch Version"
#endif

#if (ETH_VERSION_SW_MAJOR_VERSION != ETH_VERSION_C_SW_MAJOR_VERSION)
#error "Eth_Version.c : Mismatch in Major Version"
#endif

#if (ETH_VERSION_SW_MINOR_VERSION != ETH_VERSION_C_SW_MINOR_VERSION)
#error "Eth_Version.c : Mismatch in Minor Version"
#endif

#if (ETH_VERSION_CHECK_EXT_MODULES == STD_ON)

#if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
#if ((RTE_AR_RELEASE_MAJOR_VERSION != ETH_AR_RELEASE_MAJOR_VERSION) || \
  (RTE_AR_RELEASE_MINOR_VERSION != ETH_AR_RELEASE_MINOR_VERSION))
#error "The AR version of Rte.h does not match the expected version"
#endif
#endif /* (ETH_CRITICAL_SECTION_PROTECTION == STD_ON) */

#if ((DEM_AR_RELEASE_MAJOR_VERSION != ETH_AR_RELEASE_MAJOR_VERSION) || \
  (DEM_AR_RELEASE_MINOR_VERSION != ETH_AR_RELEASE_MINOR_VERSION))
#error "The AR version of Dem.h does not match the expected version"
#endif

#if (ETH_DEV_ERROR_DETECT == STD_ON)
#if ((DET_AR_RELEASE_MAJOR_VERSION != ETH_AR_RELEASE_MAJOR_VERSION) || \
  (DET_AR_RELEASE_MINOR_VERSION != ETH_AR_RELEASE_MINOR_VERSION))
#error "The AR version of Det.h does not match the expected version"
#endif
#endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

#endif /* (ETH_VERSION_CHECK_EXT_MODULES == STD_ON) */
/*******************************************************************************
**                         Global Data                                        **
*******************************************************************************/
/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/
/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
