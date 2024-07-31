/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Rte.h                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a stub for Rte Component                                      */
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
/*              Devices:        R-Car V3M                                     */
/*                              R-Car H3/M3                                   */
/*============================================================================*/
/*
 * V1.0.0:      30-Oct-2015:      Initial Version
 * V1.1.0:      07-Dec-2016:      Add RTE_E_COM_BUSY error
 * V1.2.0:      04-Jan-2018:      Update device name
 * V1.2.1:      24-Jan-2018:      Add device name M3 about comment.
 */

#ifndef RTE_H
#define RTE_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR Specification Version Information */
#define RTE_AR_RELEASE_MAJOR_VERSION         4
#define RTE_AR_RELEASE_MINOR_VERSION         2
#define RTE_AR_RELEASE_REVISION_VERSION      2

/* Software Version Information */
#define RTE_SW_MAJOR_VERSION         1
#define RTE_SW_MINOR_VERSION         0
#define RTE_SW_PATCH_VERSION         0
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/* Predifined values for Std_ReturnType */
#define RTE_E_OK               0U
#define RTE_E_INVALID          1U
#define RTE_E_COMMS_ERROR      128U
#define RTE_E_TIMEOUT          129U
#define RTE_E_LIMIT            130U
#define RTE_E_NO_DATA          131U
#define RTE_E_TRANSMIT_ACK     132U
#define RTE_E_COM_BUSY         141U
#define RTE_E_LOST_DATA        64U
#define RTE_E_MAX_AGE_EXCEEDED 64U

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* RTE_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
