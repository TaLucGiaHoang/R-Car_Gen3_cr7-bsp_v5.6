/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Cfg.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains pre-compile time parameters.                            */
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
/*============================================================================*/
/* Environment:                                                               */
/*              Devices:        R-Car H3                                      */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V1.0.0:  02-Jan-2015    1) Initial Version                                  *
 * V1.0.1   05-Aug-2016    1) Added new parameters: EthGetDropCountApi,        *
 *                            EthGetEtherStatsApi, EthEdcErrorDetect,          *
 *                            EthVersionInfoApi, ETH_E_ALIGNMENT, ETH_E_CRC,   *
 *                            ETH_E_INTERRUPT_CTL_CHECK, ETH_E_LATECOLLISION   *
 *                            ETH_E_MULTIPLECOLLISION, ETH_E_OVERSIZEFRAME     *
 *                            ETH_E_RX_FRAMES_LOST, ETH_E_SINGLECOLLISION,     *
 *                            ETH_E_TIMEOUT, ETH_E_UNDERSIZEFRAME              *
 *                         2) Removed parameter: EthVersionInfoApiMacro        *
 *                         3) Replace ETH_59_, Eth_59_ by ETH_, Eth_           *
 * V1.1.0   19-Sep-2016    1) Added new parameters: ETH_OS_COUNTER_ID          *
 *                            ETH_OS_COUNTER_MAX                               *
 * V1.1.1   03-Oct-2016    1) Added new parameters: ETH_E_TIMERINC_FAILED      *
 * V1.1.2   04-Nov-2016    1) Added new parameters: ETH_E_REG_READ_VERIFY      *
 *                            ETH_E_REG_WRITE_VERIFY, EthReadVerifyCheck       *
 *                            ETH_E_PAYLOAD_CRC, EthWriteVerifyCheck           *
 *                            EthPayloadCrcErrorDetect                         *
 * V1.1.3   05-Dec-2016    1) Added new parameter:                             *
 *                            EthEdcErrNotificationTgt                         *
 * V1.1.4   24-Jan-2017    1) Add new parameter:                               *
 *                            EthWriteVerifyErrNotificationTgt,                *
 *                            EthReadVerifyErrNotificationTgt,                 *
 *                            EthEdcErrNotificationForEthAvb,                  *
 *                            EthPayloadCrcErrNotificationTgt,                 *
 *                            EthInterruptCtrlCheckErrNotificationTgt,         *
 *                            EthModuleStopCheck, EthDeviceVersion,            *
 *                            ETH_E_MODULE_STOP_CHECK,                         *
 *                            EthModuleStopCheckErrNotificationTgt,            *
 *                         2) Update parameter:                                *
 *                            EthErrorDetectionCircuitForStbe,                 *
 *                            EthEdcErrNotificationForStbe                     *
 *                         3) Add new parameter:                               *
 *                            EthDeviceName, ETH_E_RESIDUAL_BIT_FRAME          *
 *                            ETH_E_FRAME_RECEIVE, ETH_E_TRANSMIT_RETRY_OVER   *
 * V1.1.5   08-Mar-2017    1) Add new parameter:                               *
 *                            EthRxBeQueueTimestamp,                           *
 *                            EthRxSQueueTimeStamp,                            *
 *                            ETH_RX_SQUEUE_NUM,                               *
 *                            ETH_RX_NCQUEUE_NUM,                              *
 *                         2) Removed parameter: EthRamBaseAddress,            *
 *                            EthRamSize                                       *
 * V1.1.6   13-Mar-2017    1) Change device version ES2.0 to WS2.0             *
 * V1.1.7   04-Apr-2017    1) Add new parameter EthMaxPhysFilterAddr           *
 *                         2) Add new parameter: ETH_E_DESC_DATA               *
 * V1.1.8   10-Jun-2017    1) Remove parameter EthMainFunctionPeriod           *
 * V1.1.9   18-Jun-2017    1) Add pre-compile option                           *
 *                            ETH_INTERRUPT_CTL_CHK_ERROR_DETECT               *
 * V1.1.10  28-Jul-2017    1) Add Symbolic Name definition for EthCtrlIdx      *
 *                            parameter                                        *
 * V1.1.11  21-Sep-2017    1) Update generated name of Configuration Set       *
 *                            Handles                                          *
 * V1.1.12  05-Oct-2017    1) Add pre-compile option ETH_MDC_DELAY             *
 * V1.1.13  25-Oct-2017    1) Add new parameter: ETH_E_OPMODE                  *
 * V1.1.14  21-Mar-2018    1) Add new parameter ETH_E_AVB_DMAC and             *
 *                            EthRuntimeErrorDetect                            *
 * V1.1.15  09-Aug-2018    1) Remove parameter EthEdcErrNotificationForEthAvb  *
 */
/******************************************************************************/

/*******************************************************************************
**                   Generation Tool Version                                  **
*******************************************************************************/
/*
 * TOOL VERSION:  1.3.20
 */

/*******************************************************************************
**                          Input File                                        **
*******************************************************************************/
/*
 * INPUT FILE:     D:\RCar\common_platform\generic\stubs\4.2.2\Mcu\xml\
 *                  Mcu_Eth_H3.arxml
 *                 D:\RCar\H3\modules\eth\sample_application\4.2.2\config\
 *                  App_ETH_Sample.arxml
 *                 D:\RCar\common_platform\generic\stubs\4.2.2\Dem\xml\
 *                  Dem_Eth.arxml
 *                 D:\RCar\common_platform\generic\stubs\4.2.2\Os\xml\
 *                  Os_Eth.arxml
 *                 D:\RCar\H3\modules\eth\generator\
 *                  R422_ETH_H3_BSWMDT.arxml
 * GENERATED ON:    5 Nov 2021 - 19:16:34
 */


#ifndef ETH_CFG_H
#define ETH_CFG_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_CFG_AR_RELEASE_MAJOR_VERSION  4U
#define ETH_CFG_AR_RELEASE_MINOR_VERSION  2U
#define ETH_CFG_AR_RELEASE_REVISION_VERSION  2U

/* File version information */
#define ETH_CFG_SW_MAJOR_VERSION   1
#define ETH_CFG_SW_MINOR_VERSION   5


/*******************************************************************************
**                       Common Published Information                         **
*******************************************************************************/

#define ETH_AR_RELEASE_MAJOR_VERSION_VALUE  4
#define ETH_AR_RELEASE_MINOR_VERSION_VALUE  2
#define ETH_AR_RELEASE_REVISION_VERSION_VALUE  2

#define ETH_SW_MAJOR_VERSION_VALUE  1
#define ETH_SW_MINOR_VERSION_VALUE  5
#define ETH_SW_PATCH_VERSION_VALUE  23

#define ETH_VENDOR_ID_VALUE  59
#define ETH_MODULE_ID_VALUE  88

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/* Pre-compile option for Version information */
#define ETH_AR_VERSION                                ETH_AR_HIGHER_VERSION

/* This parameter contains the supported device name. */
#define ETH_DEVICE_NAME                               RCAR_H3

/* This parameter contains the supported device version. */
#define ETH_DEVICE_VERSION                            H3_WS2_0

/* Instance ID of the ETH Driver */
#define ETH_INSTANCE_ID_VALUE                         0U

/* Pre-compile option for Development Error Detect */
#define ETH_DEV_ERROR_DETECT                          STD_ON

/* Pre-compile option for Version Info API */
#define ETH_VERSION_INFO_API                          STD_ON

/* Pre-compile option for critical section protection */
#define ETH_CRITICAL_SECTION_PROTECTION               STD_ON

/*
* Pre-compile option for enable or disable version check of inter-module
* dependencies
*/
#define ETH_VERSION_CHECK_EXT_MODULES                 STD_ON

/*
* Pre-compile option for selection. register read-back functionality
* of static and dynamic registers
*/
#define ETH_WRITE_VERIFY_CHECK                        WV_DISABLE

/*
* Pre-compile option for selection. Select notification target
* in case of Write verify error detection
*/
#define ETH_WRITE_VERIFY_ERR_NOTIFICATION_TGT         RFSO_DISABLE

/*
* Pre-compile option for selection register read verify functionality
* of static and dynamic registers
*/
#define ETH_READ_VERIFY_CHECK                         RV_DISABLE

/*
* Pre-compile option for selection. Select notification target
* in case of read verify error detection
*/
#define ETH_READ_VERIFY_ERR_NOTIFICATION_TGT          RFSO_DISABLE

/*
* Pre-compile option for enable or disable unintended interrupt
* detection
*/
#define ETH_INTERRUPT_CTL_CHK_ERROR_DETECT            STD_OFF

/*
* Pre-compile option for selection. Select notification target
* in case of unintended Interrupt detection
*/
#define ETH_INTERRUPT_CTL_CHK_ERR_NOTIFICATION_TGT    RFSO_DISABLE

/*
* Pre-compile option for enable or disable Error Detection Circuit
* protection
*/
#define ETH_STBE_EDC_ERROR_DETECT                     STD_OFF

/* Pre-compile option for selection EDC error notification target */
#define ETH_STBE_EDC_ERR_NOTIFICATION                 STD_OFF

/*
* Pre-compile option for selection. Enable/Disable unintended module stop
* detection
*/
#define ETH_MODULE_STOP_CHECK                         STD_OFF

/*
* Pre-compile option for selection. Select notification target
* in case of Unintended module stop detection
*/
#define ETH_MODULE_STOP_CHK_ERR_NOTIFICATION_TGT      RFSO_DISABLE

/* Pre-compile option for Control Enable for Address Filtering */
#define ETH_UPDATE_PHYS_ADDR_FILTER                   STD_ON

/* Pre-compile option for GlobalTime APIs */
#define ETH_GLOBAL_TIME_SUPPORT                       STD_ON

/* Pre-compile option for MII Control Enable */
#define ETH_CTRL_ENABLE_MII                           STD_ON

/* Pre-compile option for Control Enable for receive Interrupt */
#define ETH_CTRL_ENABLE_RX_INTERRUPT                  STD_ON

/* Pre-compile option for Control Enable for transmit Interrupt */
#define ETH_CTRL_ENABLE_TX_INTERRUPT                  STD_ON

/* PreCompile Option to enable / disable the Priority in Transmission */
#define ETH_QOS_SUPPORT                               STD_OFF

/* Pre-compile option for TimeStamp storage for best effort Queue */
#define ETH_RX_BEQUEUE_TIMESTAMP                      STD_OFF

/* Pre-compile option for TimeStamp storage for stream Queues */
#define ETH_RX_SQUEUE_TIMESTAMP                       STD_OFF

/* Ethernet EthRxBufTotal */
#define ETH_RX_BUF_TOTAL                              (uint8)0xFF

/* Ethernet TxBufTotal */
#define ETH_TX_BUF_TOTAL                              (uint8)0xFF

/* Maximum number of physical filter addresses */
#define ETH_MAX_PHYS_FILTER_ADDR                      (uint8)0x01

/* Ethernet RxBufLenByte */
#define ETH_CTRL_RX_BUF_LEN_BYTE                      0x5F2U

/* Ethernet TxBufLenByte */
#define ETH_CTRL_TX_BUF_LEN_BYTE                      0x5F2U

/* Maximum Ethernet Controllers Supported */
#define ETH_MAX_CTRLS_SUPPORTED                       (uint8)0x01

/* Maximum Ethernet Timeout Count */
#define ETH_TIMEOUT_COUNT                             (TickType)13158U

/* Ethernet Management Data Clock Delay */
#define ETH_MDC_DELAY                                 (TickType)1U

/* Total number of Ethernet config available */
#define ETH_TOTAL_CONFIG                              (uint8)0x01

/* Peripheral clock value for the ETH Macro in Hz */
#define ETH_PERI_CLOCK_HZ                             (uint32)133333333U

/* Total Number of Tx queues configured for all configset */
#define ETH_TX_QUEUE_COUNT                            1U

/* Total Number of Rx queues configured for all configset */
#define ETH_RX_QUEUE_COUNT                            1U

/* Total Number of Traffic Priorities */
#define ETH_TX_PRIORITY_COUNT                         0U

/* Total Number of Rx Stream queues configured for all configset */
#define ETH_RX_SQUEUE_NUM                             0U

/* Total Number of Rx NC queues configured for all configset */
#define ETH_RX_NCQUEUE_NUM                            0U

/* Enables / Disables Eth_GetDropCount API */
#define ETH_GET_DROP_COUNT_API                        STD_ON

/* Enables / Disables Eth_GetEtherStats API */
#define ETH_GET_ETHER_STATS_API                       STD_ON

/* Os Counter ID */
#define ETH_OS_COUNTER_ID                             OsCounter0

/* OS Counter Max Value */
#define ETH_OS_COUNTER_MAX                            (TickType)65535U

/* Pre-compile option for Runtime Error Detect */
#define ETH_RUNTIME_ERROR_DETECT                      STD_ON

/* Ethernet Controller Access Failure */
#define ETH_E_ACCESS                                  \
  DemConf_DemEventParameter_ETH_E_ACCESS

/* Alignment Error */
#define ETH_E_ALIGNMENT                               \
  DemConf_DemEventParameter_ETH_E_ALIGNMENT

/* CRC Failure Error */
#define ETH_E_CRC                                     \
  DemConf_DemEventParameter_ETH_E_CRC

/*
* Pre-compile option for selection. Select notification target
* in case of Payload CRC Error detection
*/
#define ETH_PAYLOAD_CRC_ERR_NOTIFICATION_TGT          RFSO_DISABLE

/*
* Pre-compile option for enable or disable Payload CRC Error
* detection
*/
#define ETH_PAYLOAD_CRC_ERROR_DETECT                  STD_OFF

/* Report Interrupt Consistency Check */
#define ETH_E_INTERRUPT_CTL_CHECK                     \
  DemConf_DemEventParameter_ETH_E_INTERRUPT_CTL_CHECK

/* Late Collisions Error */
#define ETH_E_LATECOLLISION                           \
  DemConf_DemEventParameter_ETH_E_LATECOLLISION

/* Multiple Collisions Error */
#define ETH_E_MULTIPLECOLLISION                       \
  DemConf_DemEventParameter_ETH_E_MULTIPLECOLLISION

/* Oversized Frame Error */
#define ETH_E_OVERSIZEFRAME                           \
  DemConf_DemEventParameter_ETH_E_OVERSIZEFRAME

/* Payload CRC Error */
#define ETH_E_PAYLOAD_CRC                             \
  DemConf_DemEventParameter_ETH_E_PAYLOAD_CRC

/* Report Read Verify Check */
#define ETH_E_REG_READ_VERIFY                         \
  DemConf_DemEventParameter_ETH_E_REG_READ_VERIFY

/* Report Write Verify Check */
#define ETH_E_REG_WRITE_VERIFY                        \
  DemConf_DemEventParameter_ETH_E_REG_WRITE_VERIFY

/* Module Stop Check Error */
#define ETH_E_MODULE_STOP_CHECK                       \
  DemConf_DemEventParameter_ETH_E_MODULE_STOP_CHECK

/* Transmit retry error */
#define ETH_E_TRANSMIT_RETRY_OVER                     \
  DemConf_DemEventParameter_ETH_E_TRANSMIT_RETRY_OVER

/* Frame receive error */
#define ETH_E_FRAME_RECEIVE                           \
  DemConf_DemEventParameter_ETH_E_FRAME_RECEIVE

/* Residual bit frame error */
#define ETH_E_RESIDUAL_BIT_FRAME                      \
  DemConf_DemEventParameter_ETH_E_RESIDUAL_BIT_FRAME

/* Descriptor data error */
#define ETH_E_DESC_DATA                               \
  DemConf_DemEventParameter_ETH_E_DESC_DATA

/* Operating mode transition error */
#define ETH_E_OPMODE                                  \
  DemConf_DemEventParameter_ETH_E_OPMODE

/* AVB-DMAC-related error */
#define ETH_E_AVB_DMAC                                \
  DemConf_DemEventParameter_ETH_E_AVB_DMAC

/* Receive frames lost error */
#define ETH_E_RX_FRAMES_LOST                          \
  DemConf_DemEventParameter_ETH_E_RX_FRAMES_LOST

/* Single Collisions Error */
#define ETH_E_SINGLECOLLISION                         \
  DemConf_DemEventParameter_ETH_E_SINGLECOLLISION

/* Ethernet Timeout Failure */
#define ETH_E_TIMEOUT                                 \
  DemConf_DemEventParameter_ETH_E_TIMEOUT

/* Error in setting the gPTP timer increment configuration register */
#define ETH_E_TIMERINC_FAILED                         \
  DemConf_DemEventParameter_ETH_E_TIMERINC_FAILED

/* Undersized Frame Error */
#define ETH_E_UNDERSIZEFRAME                          \
  DemConf_DemEventParameter_ETH_E_UNDERSIZEFRAME


/* Configuration Set Handles */
#define EthConfigSet0                                 \
  (&Eth_GstConfiguration[0])


/* Configuration Controller Index Handles */
#define EthConf_EthCtrlConfig_EthCtrlConfig0          0U

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* ETH_CFG_H  */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
