/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = App_ETH_Common_Sample.c                                     */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This application file contains execution sequences to demonstrate the      */
/* usage of ETH Driver APIs.                                                  */
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
* V1.0.0:  08-Dec-2015 : Initial Version
* V1.0.1:  05-Aug-2016 : Remove Renesas specific number 59
*                        Unify version number
* V1.1.0:  30-Sep-2016 : Merged Eth_ControllerInit into Eth_Init
* V1.2.0:  24-Jan-2017 : Changed speed to 100Mbps
*                        Added Eth_Receive API to capture message from Ostinato
* V1.2.1:  05-Apr-2017 : Revised RXEthHeader size
* V1.2.2:  28-Jul-2017 : Replace macro ETH_CONTROLLER0 by Symbolic Name of
*                        EthCtrlIdx parameter
* V1.2.3:  17-Aug-2017 : Support scheduling Eth_MainFunction to call every 1ms
*                        to check state changes after Eth_SetControllerMode is
*                        changed to Asynchronous
* V1.2.4:  28-Sep-2017 : Update following points :
*                        - Change timeout to 200000 for enabled cache.
*                        - Update log for start, success and fail.
* V1.3.0   26-Oct-2017 : Added Clock Pad Skew Setting
* V1.3.1:  13-Dec-2017 : Initialize value for local variables
* V1.3.2:  31-Jan-2018 : Update Eth_ProvideTxBuffer calling to disable
*                        the Transmission based on the priority (Qos)
* V1.3.3:  26-Jan-2018 : Add branch when Device name is M3.
* V1.3.4:  05-Mar-2018 : Update initialization for variables Eth_PassedCount and
*                        LucMacTgtAddr
* V1.3.5:  17-Jun-2018 : Add pre-compile option for V3H device.
* V1.3.6:  22-Aug-2018 : Add invoking Wdg_Init in main function to initialize
 *                       Watchdog timer.
* V1.3.7:  22-Sep-2018 : Add function Eth_PhyExtReset
*                        Remove device version checking
* V1.4.0:  23-Mar-2022 : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/
#include <string.h>
#include <stdbool.h>
#include "ether_api.h"
#include "r_cio_ether_public.h"
#include "r_cio_api.h"
#include "app_eth_common_sample.h"
#include "app_eth_specific_sample.h"
#include "Eth_Ram.h"
#include "Eth_Phy.h"
#include "Eth_Dma.h"
#include "Eth_Mac.h"
#include "Eth_Types.h"
#include "Std_Types.h"
#include "Eth_Cfg.h"
#include "SchM_Eth.h"

#include "EthIf_Cbk.h"

#include "rcar-xos/osal/r_osal.h"

#include "r_print_api.h"

/*******************************************************************************
**                            Macros                                          **
*******************************************************************************/
#define ETH_PHY_TRCV        0
#define ETH_PHY_TRCV_CTRL_REG  0
#define ETH_PHY_TRCV_STAT_REG  1
#define ETH_PHY_TRCV_1000T_CTRL_REG  9
#define ETH_REG_VALUE          (uint16)0x1300
#define ETH_AUTO_NEGOTIATION_ACTIVE 0x28
#define ETH_LINK_UP                 0x4

/* Time-out */
#define ETH_TIMEOUT        (uint32)0x200000
#define ETH_ARP_TYPE (uint16)0x806
#define ETH_SENDER_ADDR_OFFSET (uint8)6U

#define ETH_TGT_ADDRESS {0x08, 0xEE, 0x75, 0x3C, 0x61, 0x19}

/* Create Resource ID */
#define ETH_RESOURCE_ID 0x8000U                     /* RESOURCE_ID_BSP                */
#define ETH_MQ_ID       (ETH_RESOURCE_ID + 41U)     /* OSAL_MQ_TAURUS_PARAETHER       */

#define ETH_MQ_NUM_OF_MSG    18
#define ETH_MQ_MSG_SIZE      14 /* This value should be sizeof(r_ether_mq_receive_t) = 20, but currently OSAL definition is 14 */
#define ETH_MQ_TIMEOUT_MS    1000


/*******************************************************************************
**                        Global variables                                    **
*******************************************************************************/
volatile uint8 RXEthHeader[ETH_NORMALMAC_HSIZE];
extern osal_mq_handle_t Ether_mq_handle_received;

/*******************************************************************************
**                        User function prototypes                            **
*******************************************************************************/


/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/


int ether_sample_main(void)
{
  uint16 LucCount = ETH_ZERO;
#if defined(USE_ETHER_CR)
  Eth_DataType* LpEthBufPtr;
  Eth_DataType* LpTmpEthBufPtr;
  uint32 LulEthBufIdx;
  uint32 GulDelayCounter = ETH_ZERO;
  uint16 LusEthTxFramePayloadLength = ETH_ZERO;
  uint8 LucCheckCount = ETH_ZERO;
  uint8 LucMacTgtAddr[ETH_SIX_8] = ETH_TGT_ADDRESS;
  volatile uint8 Eth_PassedCount;
  bool receive_flag = false;

  r_cio_ether_version_info r_cio_ether_verInfo;
  r_cio_ether_mii r_cio_etherMii;
  r_cio_ether_mode r_cio_etherMode;
  r_cio_ether_tx_buff r_cio_ether_TxBuff;
  r_cio_ether_transmit r_cio_etherTransmit;

  int ether_cio_channel;
  ssize_t ret;
  st_osal_mq_config_t mq_config;
  osal_mq_handle_t ether_mq_handle;
  e_osal_return_t osal_ret;
  r_ether_mq_receive_t mq_receive_data;

  LulEthBufIdx = ETH_ZERO;
  LpEthBufPtr = (Eth_DataType*)(&RXEthFrame[ETH_ZERO]);
  LpTmpEthBufPtr = NULL;
  EthRcvBroadcastMsg = ETH_TRUE;
  Eth_PassedCount = ETH_ZERO;
#endif

  /* Initialize RXEthFrame*/
  for (LucCount = 0; LucCount < ETH_VLFRAME_SIZE; LucCount++)
  {
    RXEthFrame[LucCount] = 0xFF;
  }
  /*Initialize MCU */
  Mcu_Init();

  /*Initialize the Port Configuration for Ethernet*/
  Port_Init();

  /* Reset ETH external PHY */
  Eth_PhyExtReset();

#if defined(USE_ETHER_CR)
  /* Open ether channel and get handle */
  ether_cio_channel = R_CIO_Open("RCar-Ether:0", "rwb");
  if (0 == ether_cio_channel)
  {
    R_PRINT_Log("Ether: RCar-Ether CIO open failed.\r\n");
    while (1)
    {
      R_OSAL_ThreadSleepForTimePeriod(10);
    }
  }
#endif

  /* Print out PROGRAM START */
  R_PRINT_Log("ETH Sample Application START\r\n");

#if defined(USE_ETHER_CR)
  osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_config);
  if(OSAL_RETURN_OK != osal_ret)
  {
      R_PRINT_Log("[ETH Sample App] R_OSAL_MqInitializeMqConfigSt fail(%d)\r\n", osal_ret);
  }
  mq_config.max_num_msg = ETH_MQ_NUM_OF_MSG;
  mq_config.msg_size    = ETH_MQ_MSG_SIZE;
  osal_ret = R_OSAL_MqCreate(&mq_config, (osal_mq_id_t)ETH_MQ_ID, &ether_mq_handle);
  if (OSAL_RETURN_OK != osal_ret) {
      R_PRINT_Log("[ETH Sample App] R_OSAL_MqCreate fail(%d), max_num_msg = %lld, msg_size = %lld, mq_id = 0x%04X\r\n",
                  osal_ret, mq_config.max_num_msg, mq_config.msg_size, ETH_MQ_ID);
  }

  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_REGISTER_RX_EVENT, ether_mq_handle, NULL);
  if (0 != ret) {
      R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_REGISTER_RX_EVENT) error. return value = %d\r\n", ret);
  }

  /* Eth_GetVersionInfo(&versionInfo); */
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_GET_VERSION_INFO, (void *)&r_cio_ether_verInfo, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_GET_VERSION_INFO) error.\r\n");
  }
  /* Check for the correctness of version information */
  if((r_cio_ether_verInfo.versionInfo.vendorID != ETH_VENDOR_ID) ||
    (r_cio_ether_verInfo.versionInfo.moduleID != ETH_MODULE_ID) ||
    (r_cio_ether_verInfo.versionInfo.sw_major_version != ETH_SW_MAJOR_VERSION) ||
    (r_cio_ether_verInfo.versionInfo.sw_minor_version != ETH_SW_MINOR_VERSION) ||
    (r_cio_ether_verInfo.versionInfo.sw_patch_version != ETH_SW_PATCH_VERSION)
  )
  {
    /* If the version information is incorrect */
    R_PRINT_Log("[ETH Sample App] Wrong Version information.\r\n");
    R_PRINT_Log("                 Expected: Vendor=%d, Module=%d, SW=%d.%d.%d\r\n",
      ETH_VENDOR_ID, ETH_MODULE_ID, ETH_SW_MAJOR_VERSION, ETH_SW_MINOR_VERSION, ETH_SW_PATCH_VERSION);
    R_PRINT_Log("                 Current : Vendor=%d, Module=%d, SW=%d.%d.%d\r\n",
      r_cio_ether_verInfo.versionInfo.vendorID, r_cio_ether_verInfo.versionInfo.moduleID,
      r_cio_ether_verInfo.versionInfo.sw_major_version, r_cio_ether_verInfo.versionInfo.sw_minor_version,
      r_cio_ether_verInfo.versionInfo.sw_patch_version);
  }

  /* Invoke Eth_Init API with valid configuration pointer */
  /* Eth_Init(Eth_GstConfiguration); // Eth_Init will be called in CIO initialize */


  /* Set mode to ACTIVE */
  /* Eth_SetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_MODE_ACTIVE); */
  r_cio_etherMode.CtrlIdx  = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMode.CtrlMode = ETH_MODE_ACTIVE;
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_SET_MODE, (void *)&r_cio_etherMode, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_SET_MODE) error.\r\n");
  }
  /* Get controller mode */
  /* Eth_GetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0, &LenEthTestCtrlMode); */
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_GET_MODE, (void *)&r_cio_etherMode, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_GET_MODE) error.\r\n");
  }
  /* Check point 1 */
  if (ETH_MODE_ACTIVE == r_cio_etherMode.CtrlMode)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
#if (ETH_CTRL_ENABLE_MII == STD_ON)
  if(0 == Eth_ClockPadSkewSetup(ether_cio_channel))
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
#endif
  /* Setting PHY to auto-negotiate 100Mbps*/
  /* Remove 1000Mbps speed */
  /* LddEthReturnvalue = Eth_WriteMii(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_PHY_TRCV, ETH_PHY_TRCV_CTRL_REG, ETH_ZERO_16); */

  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = ETH_PHY_TRCV;
  r_cio_etherMii.RegIdx  = ETH_PHY_TRCV_CTRL_REG;
  r_cio_etherMii.RegVal  = ETH_ZERO_16;
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error.\r\n");
  }
  /* Check point 2 */
  /* if (E_OK == LddEthReturnvalue) */
  if(ret == 0)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  /* Remove auto-negotiation advertisement for 1000Mbps full/half duplex  */
  /* LddEthReturnvalue = Eth_WriteMii (EthConf_EthCtrlConfig_EthCtrlConfig0,
    ETH_PHY_TRCV, ETH_PHY_TRCV_1000T_CTRL_REG, ETH_ZERO_16); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = ETH_PHY_TRCV;
  r_cio_etherMii.RegIdx  = ETH_PHY_TRCV_1000T_CTRL_REG;
  r_cio_etherMii.RegVal  = ETH_ZERO_16;
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error.\r\n");
  }
  /* Check point 3 */
  /* if (E_OK == LddEthReturnvalue) */
  if(ret == 0)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  /* Force restart auto-negotiation */
  /* LddEthReturnvalue = Eth_WriteMii(EthConf_EthCtrlConfig_EthCtrlConfig0,
    ETH_PHY_TRCV, ETH_PHY_TRCV_CTRL_REG, ETH_REG_VALUE); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = ETH_PHY_TRCV;
  r_cio_etherMii.RegIdx  = ETH_PHY_TRCV_CTRL_REG;
  r_cio_etherMii.RegVal  = ETH_REG_VALUE;
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error.\r\n");
  }
  /* Check point 4 */
  /* if (E_OK == LddEthReturnvalue) */
  if(ret == 0)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  /* Check whether the Link is Up */
  while (GulDelayCounter < ETH_TIMEOUT)
  {
    GulDelayCounter++;
    /* Check the Phy interface is Initialized */
    /* Eth_ReadMii(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_PHY_TRCV, ETH_PHY_TRCV_STAT_REG, &LusEthRegValue); */
    r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
    r_cio_etherMii.TrcvIdx = ETH_PHY_TRCV;
    r_cio_etherMii.RegIdx  = ETH_PHY_TRCV_STAT_REG;
    ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_READ_MII, (void *)&r_cio_etherMii, 0);
    if (0 != ret)
    {
      R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_READ_MII) error.\r\n");
    }
    if ((r_cio_etherMii.RegVal & (ETH_AUTO_NEGOTIATION_ACTIVE | ETH_LINK_UP)) == (ETH_AUTO_NEGOTIATION_ACTIVE | ETH_LINK_UP))
    {
      /* Check point 5 */
      Eth_PassedCount++;
      break;
    }
  }
  LucCheckCount++;
  /* Wait to receive message */
  receive_flag = false;
  do
  {
    do {
      osal_ret = R_OSAL_MqReceiveForTimePeriod(Ether_mq_handle_received,
                                              (osal_milli_sec_t)ETH_MQ_TIMEOUT_MS,
                                              (void*)&mq_receive_data,
                                              ETH_MQ_MSG_SIZE);
    } while (OSAL_RETURN_TIME == osal_ret);
    if (OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("[ETH Sample App] R_OSAL_MqReceiveForTimePeriod error. return value = %d\r\n", osal_ret);
    }

    receive_flag = true;
    for (LucCount = 0; LucCount < ETH_MACADDR_SIZE; LucCount++)
    {
      if (RXEthHeader[LucCount] != Eth_GstConfiguration[0].ucEthMACAddr[LucCount])
      {
        receive_flag = false;
        break;
      }
    }
  } while (receive_flag != true);
  R_PRINT_Log("Receive data:\n");
  /* display rx buff */
  for (LucCount = 0; LucCount < 30; LucCount++)
  {
    R_PRINT_Log("0x%02X, ", RXEthFrame[LucCount]);
    if((LucCount + 1) % 10 == 0)
    {
      R_PRINT_Log("\n");
    }
  }
  LusEthTxFramePayloadLength = Eth_Msg_length;
  /* Copy DA */
  for (LucCount = 0; LucCount < ETH_MACADDR_SIZE; LucCount++)
  {
    LucMacTgtAddr[LucCount] = RXEthHeader[ETH_SENDER_ADDR_OFFSET+LucCount];
  }

  /* Provides access to a transmit buffer */
  /*LenEthBufRet = Eth_ProvideTxBuffer(EthConf_EthCtrlConfig_EthCtrlConfig0,
    &LulEthBufIdx, &LpEthBufPtr, &LusEthTxFramePayloadLength);*/
  r_cio_ether_TxBuff.CtrlIdx    = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_ether_TxBuff.BufIdxPtr  = &LulEthBufIdx;
  r_cio_ether_TxBuff.BufPtr     = &LpEthBufPtr;
  r_cio_ether_TxBuff.LenBytePtr = &LusEthTxFramePayloadLength;

  R_PRINT_Log("r_cio_ether_TxBuff\n");
  R_PRINT_Log("\tCtrlIdx\t = %d\n"  ,r_cio_ether_TxBuff.CtrlIdx);
  R_PRINT_Log("\tBufIdxPtr\t = %p\n",r_cio_ether_TxBuff.BufIdxPtr);
  R_PRINT_Log("\tBufPtr\t = %p\n"    ,r_cio_ether_TxBuff.BufPtr);
  R_PRINT_Log("\tLenBytePtr\t = %p\n"    ,r_cio_ether_TxBuff.LenBytePtr);
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_PROVIDE_TX_BUFF, (void *)&r_cio_ether_TxBuff, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_PROVIDE_TX_BUFF) error.\r\n");
  }


  /* Check point 6 */
  if (BUFREQ_OK == ret)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  /* Copy Transmit data to the buffer to transmit */
  LpTmpEthBufPtr = LpEthBufPtr;
  for(LucCount = 0; LucCount < LusEthTxFramePayloadLength; LucCount++)
  {
    *LpEthBufPtr = RXEthFrame[LucCount];
    LpEthBufPtr++;
  }

  for(LucCount = 0; LucCount < 30; LucCount++)
  {
    R_PRINT_Log("0x%02X, ", LpTmpEthBufPtr[LucCount]);
    if((LucCount + 1) % 10 == 0)
    {
      R_PRINT_Log("\n");
    }
  }
  /* Start transmit */
  /* LddEthReturnvalue = Eth_Transmit(EthConf_EthCtrlConfig_EthCtrlConfig0,
    LulEthBufIdx, ETH_ARP_TYPE, ETH_TRUE, LusEthTxFramePayloadLength,
    &LucMacTgtAddr[0]); */
  r_cio_etherTransmit.CtrlIdx        = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherTransmit.BufIdx         = LulEthBufIdx;
  r_cio_etherTransmit.FrameType      = ETH_ARP_TYPE;
  r_cio_etherTransmit.TxConfirmation = ETH_TRUE;
  r_cio_etherTransmit.LenByte        = LusEthTxFramePayloadLength;
  r_cio_etherTransmit.PhysAddrPtr    = &LucMacTgtAddr[0];
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_TRANSMIT, (void *)&r_cio_etherTransmit, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_TRANSMIT) error.\r\n");
  }
  /* Check point 7 */
  /* if (E_OK == LddEthReturnvalue) */
  if(ret == E_OK)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  GulDelayCounter = ETH_ZERO;
  while ((TxConfirmed==ETH_ZERO) && GulDelayCounter < ETH_TIMEOUT)
  {
    GulDelayCounter++;
    /* Polling for Tx confirmation */
    /* Eth_TxConfirmation(EthConf_EthCtrlConfig_EthCtrlConfig0); */
    ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_TX_CONFIRMATION, EthConf_EthCtrlConfig_EthCtrlConfig0, 0);
    if (0 != ret)
    {
      R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_TX_CONFIRMATION) error.\r\n");
    }
  }
  /* Clear Tx Flag */
  TxConfirmed = 0;
  /* Check point 8 */
  if (GulDelayCounter < ETH_TIMEOUT)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  /* Set mode to DOWN */
  /* Eth_SetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_MODE_DOWN); */
  r_cio_etherMode.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMode.CtrlMode= ETH_MODE_DOWN;
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_SET_MODE, (void *)&r_cio_etherMode, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_SET_MODE) error.\r\n");
  }

  EthModeDownCnt = ETH_ZERO;
  GulDelayCounter = ETH_ZERO;
  while (GulDelayCounter < ETH_TIMEOUT)
  {
    GulDelayCounter++;
    if (EthModeDownCnt > ETH_ZERO)
    {
      /* Mode transition completed */
      break;
    }
  }
  /* Get controller mode */
  /* Eth_GetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0,
    &LenEthTestCtrlMode); */
  ret = R_CIO_IoCtl(ether_cio_channel, (void *)R_CIO_ETHER_IOC_GET_MODE, (void *)&r_cio_etherMode, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_GET_MODE) error.\r\n");
  }
  /* Check point 9 */
  if (ETH_MODE_DOWN == r_cio_etherMode.CtrlMode)
  {
    Eth_PassedCount++;
  }
  LucCheckCount++;
  if (Eth_PassedCount == LucCheckCount)
  {
    /* Print out PROGRAM PASSED */
    R_PRINT_Log("ETH Sample Application PASSED\r\n");
  }
  else
  {
    /* Print out PROGRAM FAILED */
    R_PRINT_Log("ETH Sample Application FAILED\r\n");
  }

  ret = R_CIO_Close(ether_cio_channel);
  if (0 != ret) {
      R_PRINT_Log("[ETH Sample App] R_CIO_Close error. return value = %d\r\n", ret);
  }

  osal_ret = R_OSAL_MqDelete(ether_mq_handle);
  if (OSAL_RETURN_OK != osal_ret) {
      R_PRINT_Log("[ETH Sample App] R_OSAL_MqDelete error. return value = %d\r\n", osal_ret);
  }
#endif
  return 0;
}

/******************************************************************************
**                           Notification Functions                          **
******************************************************************************/
#define Eth_MAIN_FUNCTION_FREQUENCY 1000
void loc_EtherMainFunctionLoop(void)
{
  osal_milli_sec_t wait_time = 1000 / Eth_MAIN_FUNCTION_FREQUENCY;
  while(1)
  {
    Eth_MainFunction();
    R_OSAL_ThreadSleepForTimePeriod(wait_time);
 }
}

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
  /* mq_send_data.CtrlIdx     = CtrlIdx;     */
  /* mq_send_data.FrameType   = FrameType;   */
  /* mq_send_data.IsBroadcast = IsBroadcast; */
  /* mq_send_data.PhysAddrPtr = PhysAddrPtr; */
  mq_send_data.DataPtr     = DataPtr;
  mq_send_data.LenByte     = LenByte;

  /* FrameRxcnt++; */
  Eth_Msg_length = LenByte;
  EthRcvBroadcastMsg = IsBroadcast;
  /* Copy Received message */
  for (LulCount = 0; LulCount < Eth_Msg_length; LulCount++)
  {
    RXEthFrame[LulCount] = *(DataPtr + LulCount);
  }
  for (LulCount = 0; LulCount < ETH_NORMALMAC_HSIZE; LulCount++)
  {
    RXEthHeader[LulCount] = *(DataPtr - ETH_NORMALMAC_HSIZE + LulCount);
  }

  /* Send received data */
  (void)R_OSAL_MqSendForTimePeriod(Ether_mq_handle_received, ETH_MQ_TIMEOUT_MS, &mq_send_data, ETH_MQ_MSG_SIZE);

}
/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/

