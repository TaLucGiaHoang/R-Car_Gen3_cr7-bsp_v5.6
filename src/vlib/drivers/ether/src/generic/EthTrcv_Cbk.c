/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EthTrcv_Cbk.c                                               */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017 - 2020 Renesas Electronics Corporation                   */
/*============================================================================*/
/* Purpose:                                                                   */
/* This application file contains execution sequences to demonstrate the usage*/
/* of Eth Driver APIs.                                                        */
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
**                      Revision History                                      **
*******************************************************************************/
/*
 * V1.0.0:  03-Jan-2017  : Initial Version
 * V1.0.1:  17-Apr-2020  : Suppress unused parameter warnings in functions
 *                         EthTrcv_WriteMiiIndication,
 *                         EthTrcv_ReadMiiIndication
 */
/******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "EthTrcv_Cbk.h"
/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETHIF_C_AR_RELEASE_MAJOR_VERSION   ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETHIF_C_AR_RELEASE_MINOR_VERSION   ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETHIF_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETHIF_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETHIF_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION



/*******************************************************************************
**                         Global Data                                        **
*******************************************************************************/
uint8 EthTrcv_ReadMiiIndication_RegIdx;
uint16 EthTrcv_ReadMiiIndication_RegVal;
uint8 EthTrcv_WriteMiiIndication_RegIdx;
/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**                             EthTrcv_WriteMiiIndication                     **
*******************************************************************************/
#pragma weak EthTrcv_WriteMiiIndication
void EthTrcv_WriteMiiIndication
(
  uint8 CtrlIdx,
  uint8 TrcvIdx,
  uint8 RegIdx
)
{
  /* Suppress unused parameter warnings for the parameters below */
  (void) CtrlIdx;
  (void) TrcvIdx;

  EthTrcv_WriteMiiIndication_RegIdx = RegIdx;
}

/*******************************************************************************
**                             EthTrcv_ReadMiiIndication                      **
*******************************************************************************/
#pragma weak EthTrcv_ReadMiiIndication
void EthTrcv_ReadMiiIndication
(
  uint8 CtrlIdx,
  uint8 TrcvIdx,
  uint8 RegIdx,
  uint16 RegVal
)
{
  /* Suppress unused parameter warnings for the parameters below */
  (void) CtrlIdx;
  (void) TrcvIdx;

  EthTrcv_ReadMiiIndication_RegIdx = RegIdx;
  EthTrcv_ReadMiiIndication_RegVal = RegVal;
}
/*******************************************************************************
                          End of the file
*******************************************************************************/
