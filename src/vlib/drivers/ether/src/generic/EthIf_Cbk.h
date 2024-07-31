/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EthIf_Cbk.h                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
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
 * V1.0.0:  02-Jan-2015  : Initial Version
 * V1.0.1:  05-Aug-2016  : Remove Renesas specific number 59
 * V1.1.0:  30-Sep-2016  : Added EthIf_CtrlModeIndication function
 * V1.1.1:  05-Apr-2017  : Revised RXEthHeader size
 * V1.2.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
 */

#ifndef ETHIF_CBK_H
#define ETHIF_CBK_H
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

extern volatile uint8 RXEthFrame[ETH_VLFRAME_SIZE];
extern volatile uint16 Eth_Msg_length;
extern volatile uint8 TxConfirmed;
extern volatile uint8 RXEthHeader[ETH_NORMALMAC_HSIZE];
/* Mode Active Counter  */
extern volatile uint8 EthModeActiveCnt;

/* Mode Down Counter  */
extern volatile uint8 EthModeDownCnt;
extern volatile boolean EthRcvBroadcastMsg;
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
/* RxIndication event referring to a Ethernet Network */
extern void EthIf_RxIndication
(
  uint8 CtrlIdx,
  Eth_FrameType FrameType,
  boolean IsBroadcast,
  uint8* PhysAddrPtr,
  Eth_DataType* DataPtr,
  uint16 LenByte
);

/* TxConfirmaton event referring to a Ethernet Network */
extern void EthIf_TxConfirmation( uint8 CtrlIdx, Eth_BufIdxType BufIdx);

extern void EthIf_CtrlModeIndication( uint8 CtrlIdx, Eth_ModeType CtrlMode);

#endif /* ETHIF_CBK_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
