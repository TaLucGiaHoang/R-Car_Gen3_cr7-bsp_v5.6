/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_PBcfg.c                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2016-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains post-build time parameters.                             */
/* AUTOMATICALLY GENERATED FILE - DO NOT EDIT                                 */
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
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "ether_api.h"
#include "Eth_PBTypes.h"
#include "Eth_Cbk.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_PBCFG_C_AR_RELEASE_MAJOR_VERSION  4U
#define ETH_PBCFG_C_AR_RELEASE_MINOR_VERSION  2U
#define ETH_PBCFG_C_AR_RELEASE_REVISION_VERSION  2U

/* File version information */
#define ETH_PBCFG_C_SW_MAJOR_VERSION   1
#define ETH_PBCFG_C_SW_MINOR_VERSION   5


/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

#if (ETH_PBTYPES_AR_RELEASE_MAJOR_VERSION != \
  ETH_PBCFG_C_AR_RELEASE_MAJOR_VERSION)
  #error "Eth_PBcfg.c : Mismatch in Release Major Version"
#endif

#if (ETH_PBTYPES_AR_RELEASE_MINOR_VERSION != \
  ETH_PBCFG_C_AR_RELEASE_MINOR_VERSION)
  #error "Eth_PBcfg.c : Mismatch in Release Minor Version"
#endif

#if (ETH_PBTYPES_AR_RELEASE_REVISION_VERSION != \
  ETH_PBCFG_C_AR_RELEASE_REVISION_VERSION)
  #error "Eth_PBcfg.c : Mismatch in Release Revision Version"
#endif

#if (ETH_PBTYPES_SW_MAJOR_VERSION != ETH_PBCFG_C_SW_MAJOR_VERSION)
  #error "Eth_PBcfg.c : Mismatch in Software Major Version"
#endif

#if (ETH_PBTYPES_SW_MINOR_VERSION != ETH_PBCFG_C_SW_MINOR_VERSION)
  #error "Eth_PBcfg.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

#define ETH_START_SEC_CONFIG_DATA_32
#include "Eth_MemMap.h"
/* Structure for ETH Init configuration */
CONST(Eth_ConfigType, ETH_CONFIG_DATA) Eth_GstConfiguration[] =
{
  /* Index: 0 - EthConfigSet0 */
  {
    /* ulStartOfDbToc */
    0xED60128U,

    /* MAC */
    {
      0x2EU,
      0x09U,
      0x0AU,
      0x06U,
      0xD4U,
      0x72U
    },

    /* Speed */
    ETH_100M,

    /* Duplex */
    ETH_FDUPLEX,

    /* MagicDetection */
    ETH_DISABLE,

    /* pMagicNotification */
    NULL_PTR,

    /* PHY_Interface */
    ETH_RGMII,

    /* QueueConfiguration */
    {
      /* FirstRxQueueIdx */
      0x00U,

      /* Address Tx Queue */
      &Eth_GstTxQueueConfig[0],

      /* Address Rx Queue */
      &Eth_GstRxQueueConfig[0],

      /* LastRxQueueIdx */
      0x00U,

      /* NumberOfTxQueue */
      0x01U,

      /* NumberOfRxQueue */
      0x01U
    },

    /* CommunicationConfiguration */
    {
      /* LoopBack */
      ETH_DISABLE,

      /* GPTPClock */
      ETH_GPTP_PBUS
    },

    /* ReceiveConfiguration */
    {
      /* Effs - enable faulty frame storage */
      ETH_DISABLE,

      /* Encf - enable network control filtering */
      ETH_DISABLE,

      /* Esf - enable stream filtering */
      ETH_SFDISABLE,

      /* Rfcl - reception FIFO critical level */
      0x0U
    },

    /* TransmitConfiguration */
    {
      /* Ecbs - Enable Credit Based Shaping */
      ETH_DISABLE,

      /* Tqp - transmission queues priority */
      ETH_DEFAULT
    },

    /* ClockInternalDelayConfig */
    {
      /* Tdm - Tx clock internal delay mode */
      ETH_ENABLE,

      /* Rdm - Rx clock internal delay mode */
      ETH_DISABLE
    }
  }
};


/* Tx Queue */
CONST(Eth_TxQueueType, ETH_CONFIG_DATA)
  Eth_GstTxQueueConfig[ETH_TX_QUEUE_COUNT] =
{
  /* TxQueueIndex: 0 - ConfigSet0 */
  {
    /* EthTxQueueIdx */
    0x00U,

    /* EthCtrlTxQueueShaper */
    {
      /* Policy */
      ETH_NONE,

      /* CIV */
      0U,

      /* CDV */
      0U
    },

    /* EthTxProcessing */
    ETH_INTERRUPT
  }
};


#define ETH_STOP_SEC_CONFIG_DATA_32
#include "Eth_MemMap.h"

#define ETH_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "Eth_MemMap.h"
/* Rx Queue */
CONST(Eth_RxQueueType, ETH_CONFIG_DATA)
  Eth_GstRxQueueConfig[ETH_RX_QUEUE_COUNT] =
{
  /* RxQueueIndex: 0 - ConfigSet0 */
  {
    /* EthRxQueueIdx */
    0x00U,

    /* EthPatternAddress */
    {
      0x00U,
      0x00U,
      0x00U,
      0x00U,
      0x00U,
      0x00U
    },

    /* EthRxProcessing */
    ETH_INTERRUPT
  }
};


#define ETH_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "Eth_MemMap.h"

#define ETH_START_SEC_CONFIG_DATA_8
#include "Eth_MemMap.h"
/* Traffic Priorities */
/* CONST(Eth_PriorityMappingType, ETH_CONFIG_DATA)
  Eth_GstCtrlPriorityTable[ETH_TX_PRIORITY_COUNT]; */
#define ETH_STOP_SEC_CONFIG_DATA_8
#include "Eth_MemMap.h"

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
