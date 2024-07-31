/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EthIf_Cbk.c                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2015-2022 by Renesas Electronics Corporation                 */
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
 * V1.0.0:  02-Jan-2015  : Initial Version
 * V1.0.1:  05-Aug-2016  : Remove Renesas specific number 59
 * V1.1.0:  30-Sep-2016  : Added EthIf_CtrlModeIndication function
 * V1.1.1:  21-Dec-2016  : Added #pragma directives
 * V1.1.2:  03-May-2017  : Added Magic Packet Callback
 * V1.1.3:  23-Dec-2017  : Remove the code that is used to copy header
 * V1.1.4:  17-Apr-2020  : Suppress unused parameter warnings in functions
 *                         EthIf_CtrlModeIndication, EthIf_TxConfirmation,
 *                         EthIf_RxIndication
 * V1.1.5:  24-Feb-2021  : Correct global variable position
 * V1.2.0:  22-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
 */
/******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "EthIf_Cbk.h"
#include "Eth_Cbk.h"
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
/* Ethernet Controller 0 macro */
#define ETH_CONTROLLER0 0

volatile uint16 Eth_Msg_length;
volatile boolean EthRcvBroadcastMsg;
volatile uint8 RXEthFrame[ETH_VLFRAME_SIZE];
volatile uint8 TxConfirmed;
volatile uint8 EthModeDownCnt;
volatile uint8 EthModeActiveCnt;


/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**                             EthIf_RxIndication                             **
*******************************************************************************/
#ifndef ETH_MODULE_SAMPLE_SPECIFIC
#pragma weak EthIf_RxIndication
void EthIf_RxIndication
(
  uint8 CtrlIdx,
  Eth_FrameType FrameType,
  boolean IsBroadcast,
  uint8* PhysAddrPtr,
  Eth_DataType* DataPtr,
  uint16 LenByte
)
{
  uint32 LulCount;
  /* Suppress unused parameter warnings for the parameters below */
  (void) CtrlIdx;
  (void) FrameType;
  (void) PhysAddrPtr;

  /* FrameRxcnt++; */
  Eth_Msg_length = LenByte;
  EthRcvBroadcastMsg = IsBroadcast;
  /* Copy Received message */
  for(LulCount = 0; LulCount < Eth_Msg_length; LulCount++)
  {
    RXEthFrame[LulCount] = *(DataPtr + LulCount);
  }
}
#else
/* EthIf_RxIndication() is implemented by application */
#endif
/*******************************************************************************
**                             EthIf_TxConfirmation                           **
*******************************************************************************/
#pragma weak EthIf_TxConfirmation
void EthIf_TxConfirmation( uint8 CtrlIdx, Eth_BufIdxType BufIdx)
{
  /* Suppress unused parameter warnings for the parameters below */
  (void) CtrlIdx;
  (void) BufIdx;

  TxConfirmed = 1;
}

/*******************************************************************************
**                             EthIf_TxConfirmation                           **
*******************************************************************************/
void EthIf_CtrlModeIndication( uint8 CtrlIdx, Eth_ModeType CtrlMode)
{
  /* Suppress unused parameter warnings for the parameters below */
  (void) CtrlIdx;

  if (CtrlMode == ETH_MODE_DOWN)
  {
    /* Increment Down Counter */
    EthModeDownCnt++;
  }
  else
  {
    /* Increment Active Counter */
    EthModeActiveCnt++;
  }
}

/*******************************************************************************
**                             Eth_MagicPacketNotification                    **
*******************************************************************************/
void Eth_MagicPacketCallback(void)
{
  Eth_DMACReset();
}
/*******************************************************************************
                          End of the file
*******************************************************************************/
