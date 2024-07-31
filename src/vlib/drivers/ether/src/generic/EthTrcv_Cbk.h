/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EthTrcv_Cbk.h                                               */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of external declaration of constants, global data, type          */
/* definitions, APIs and service IDs.                                         */
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
**                      Revision History                                      **
*******************************************************************************/
/*
 * V1.0.0:  03-Jan-2017  : Initial Version
 * V1.1.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
 */

#ifndef ETHTRCV_CBK_H
#define ETHTRCV_CBK_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Eth Driver Header File */
#include "ether_api.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
/* ReadMiiIndication event referring to a Ethernet Network */
extern void EthTrcv_ReadMiiIndication
(
  uint8 CtrlIdx,
  uint8 TrcvIdx,
  uint8 RegIdx,
  uint16 RegVal
);

/* WriteMiiIndication event referring to a Ethernet Network */
extern void EthTrcv_WriteMiiIndication
(
  uint8 CtrlIdx,
  uint8 TrcvIdx,
  uint8 RegIdx
);

#endif /* ETHTRCV_CBK_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
