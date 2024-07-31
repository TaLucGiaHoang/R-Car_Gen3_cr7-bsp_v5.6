/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EthIf_Cbk.c                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright (c) 2015-2017 by Renesas Electronics Corporation                 */
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
 */
/******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "ether_api.h"
#include "EthIf_Cbk.h"
#include "Eth_Cbk.h"
#include "rcar-xos/osal/r_osal.h"
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

/* Create Resource ID */
#define ETH_RESOURCE_ID 0x8000U                     /* RESOURCE_ID_BSP                */
#define ETH_MQ_ID       (ETH_RESOURCE_ID + 41U)     /* OSAL_MQ_TAURUS_PARAETHER       */

#define ETH_MQ_NUM_OF_MSG    18
#define ETH_MQ_MSG_SIZE      60 /* This value should be sizeof(r_ether_mq_receive_t) = 20, but currently OSAL definition is 14 */
#define ETH_MQ_TIMEOUT_MS    1000

volatile uint8 RXEthHeader[ETH_NORMALMAC_HSIZE];
extern osal_mq_handle_t Ether_mq_handle_rx;
extern osal_mq_handle_t Ether_mq_handle_received;

// TODO fix me
//  This is the structure from CIO
//  But we shouldn't really be dependent on the CIO server app in a driver
//  so we don't include the header right now..
typedef struct {
    /* uint8 CtrlIdx;           */
    /* Eth_FrameType FrameType; */
    /* boolean IsBroadcast;     */
    /* uint8* PhysAddrPtr;      */
    Eth_DataType* DataPtr;
    uint16 LenByte;
} r_ether_mq_receive_t;
/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**                             EthIf_RxIndication                             **
*******************************************************************************/
#include "r_netif_api.h" // todo
extern volatile uint32 RXEthFrameAddr;
extern volatile uint16 EthRcvFrameType;
volatile uint8 EthRcvPhysAddr[R_NETIF_MACADDR_SIZE];
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
  r_ether_mq_receive_t mq_send_data;

  uint32 LulCount;

  /* Set Mq data */
  mq_send_data.DataPtr     = DataPtr;
  mq_send_data.LenByte     = LenByte;

  /* FrameRxcnt++; */
  Eth_Msg_length = LenByte;
  EthRcvBroadcastMsg = IsBroadcast;
  RXEthFrameAddr =(uint32)DataPtr;
  EthRcvFrameType = FrameType;
  for(LulCount = 0; LulCount < ETH_MACADDR_SIZE; LulCount++) {
    EthRcvPhysAddr[LulCount] = PhysAddrPtr[LulCount];
  }

  /* Send received data */
  // (void)R_OSAL_MqSendForTimePeriod(Ether_mq_handle_received, ETH_MQ_TIMEOUT_MS, &mq_send_data, ETH_MQ_MSG_SIZE);
}
/*******************************************************************************
**                             EthIf_TxConfirmation                           **
*******************************************************************************/
void EthIf_TxConfirmation( uint8 CtrlIdx, Eth_BufIdxType BufIdx)
{
  TxConfirmed = 1;
}
/*******************************************************************************
**                             EthIf_CtrlModeIndication                          **
*******************************************************************************/
void EthIf_CtrlModeIndication( uint8 CtrlIdx, Eth_ModeType CtrlMode)
{
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
**                             EthIf_TxConfirmation                           **
*******************************************************************************/
