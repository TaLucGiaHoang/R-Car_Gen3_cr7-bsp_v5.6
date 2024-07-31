/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Types.h                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of Ethernet Component specific types used within the module.     */
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
* V1.0.0:  08-Dec-2015  : Initial Version
* V1.0.1:  05-Aug-2016  : Remove Renesas specific number 59
*                         Unify version number
* V1.1.0:  30-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
*                         Added new types: Eth_MainFunctionType,
*                         Eth_StatisticsType, Eth_ReturnType
*                         Remove ETH_STATE_ACTIVE, ETH_STATE_STANDBY
* V1.1.1:  08-Dec-2016  : Change Compiler Abstraction ETH_VAR to
*                         ETH_VAR_<INIT_POLICY>
* V1.2.0:  07-Mar-2017  : Updated following Redmine ticket:
*                         #68800 Attribute PostBuildVariantValue is not
*                         respected for some parameters
* V1.2.1:  08-Mar-2017  : Updated MISRA-C checking
* V1.2.2:  22-Mar-2017  : Added bufTxConfirm to Eth_BufHandlerType (#77583)
*                         Added DropPacketNo to Eth_GaaDeviceType (#73275)
* V1.2.3:  04-Apr-2017  : Removed PromMode from Eth_MacModeType
*                         Removed enEthPromMode from Eth_ConfigType
*                         Added Eth_RxFilterAddrType, Eth_FilterModeType
* V1.2.4:  17-Apr-2017  : Update STag_Eth_MacIntType follow ECSIPR register
*                         description in latest HW user manual Rev.0.53
*                         Replace ETH_VAR_NO_INIT by TYPEDEF when
*                         defining pointer types within type definitions
* V1.2.5:  16-May-2017  : Updated source code based on FuSa review
* V1.2.6:  16-May-2017  : Added Magic Packet notification function pointer
*                         Removed Eth_ReturnType
* V1.2.7:  17-Jul-2017  : Updated Eth_MainFunctionType and Eth_ConfigType
* V1.2.8:  31-Aug-2017  : Updated Eth_ProcessingType
* V1.2.9:  12-Sep-2017  : Updated Eth_GaaDeviceType
* V1.3.0:  28-Sep-2017  : Change the bufAdd to descAddr element in
*                         Eth_BufHandlerType
* V1.3.1:  04-Dec-2017  : Remove unused macro and type definitions
*                         Group:
*                         - Operating status macros into operating status
*                           enum Eth_OpStatusType
*                         - ETH speed macros into speed enum Eth_SpeedType
*                         - ETH option macros into ETH option enum
*                           Eth_OptionType
* V1.3.2:  12-Dec-2017  : Added Eth_ClockDelayType
*                         Added new element stClkDelayConfig to Eth_ConfigType
* V1.3.3:  15-Dec-2017  : Update casting for macros
* V1.3.4:  23-Dec-2017  : Added new elements to Eth_GaaDeviceType
* V1.3.5:  26-Dec-2017  : Update Eth_StatisticsType to add more statistic Packet
*                         counters.
*                         Add Eth_DropCountType
* V1.3.6:  05-Feb-2018  : Remove last counter type definitions in
*                         Eth_StatisticsType and Eth_DropCountType
* V1.3.7:  20-Jul-2018  : Add flag bufTxFlag in Eth_BufHandlerType to
*                         identify the buffer in under transmission or not
* V1.3.8:  23-Apr-2020  : Updated MISRA-C violation messages
* V1.4.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_TYPES_H
#define ETH_TYPES_H

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/
/* Included for Type definitions */
#include "Eth_GeneralTypes.h"
#include "Eth_PBTypes.h"
#include "Std_Types.h"
/* Ethernet Configuration Header */
#include "Eth_Cfg.h"

/*******************************************************************************
**                        Version Information                                 **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_TYPES_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_TYPES_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_TYPES_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* File version information */
#define ETH_TYPES_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_TYPES_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      GLOBAL DATA TYPES                                     **
*******************************************************************************/

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      MISRA C Rule Violations                               **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (6:0635) [E] Bit-fields in this struct/union have been     */
/*                 declared with types other than int, signed int or          */
/*                 unsigned int.                                              */
/* Rule          : MISRA-C:2004 Rule 1.1, 6.4                                 */
/*                 REFERENCE - ISO:C90-6.5.2.1 Structure and Union Specifiers */
/*                                             - Semantics                    */
/* Justification : To access bit wise from register                           */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(6:0635)-1 and                           */
/*                 END Msg(6:0635)-1 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0750) A union type specifier has been defined.          */
/* Rule          : None                                                       */
/* Justification : Union is used to access bit wise from register. ETH driver */
/*                 always used unit32 to define the union. There is no risk of*/
/*                 using union in this case                                   */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0750)-3 and                           */
/*                 END Msg(4:0750)-3 tags in the code.                        */
/******************************************************************************/

/* Rx descriptor chain offset in descriptor base address table */
#define ETH_RXBEQ_OFFSET      (uint8)4

#define ETH_RXSQ_ID_OFFSET    (uint8)2

/* Descriptor Types */

#define ETH_DESC_FMID         4U
#define ETH_DESC_FSTART       5U
#define ETH_DESC_FEND         6U
#define ETH_DESC_FSINGLE      7U

#define ETH_DESC_LINK         8U
#define ETH_DESC_LINKFIX      9U
#define ETH_DESC_EOS          10U

#define ETH_DESC_FEMPTY       12U
#define ETH_DESC_FEMPTY_IS    (uint32)13

/* Frame size */
/* Normal MAC header size */
#define ETH_NORMALMAC_HSIZE   (uint16)14
/* Add 2 bytes to make 4-byte aligned */
#define ETH_VLFRAME_SIZE      (uint16)(1522 + 2)
/* Maximum frame length */
#define ETH_MAX_FRAME_SIZE    (uint32)1522

/* Size of Tx/Rx descriptor */
/* Normal descriptor size(in byte)*/
#define ETH_NORMAL_DESC_SIZE   (uint32)8
/* Special descriptor size(in byte)*/
#define ETH_SPECIAL_DESC_SIZE  (uint32)20

/* Rx FIFO Critical Level */
#define ETH_RX_FIFO_CRIT_LVL      0x1800U

#define ETH_AVB0     0U

/* General Ethernet MAC Address Size */
#define ETH_MACADDR_SIZE   (uint8)6U

/* Version of used EtherAVB-IF */
#define ETH_VRR_VALUE      (uint32)0xE300

/* ETH duplex mode macros */
#define  ETH_FDUPLEX    (uint8)1U

/* ETH Media interface macros */
#define  ETH_RGMII     (uint8)0U

/* ETH Maximum value of UFCV */
#define  ETH_UFCV_MAX_VAL     (uint8)63U
/*******************************************************************************
** Section: Enumeration                                                       **
*******************************************************************************/

/*******************************************************************************
**  Enum: Eth_ErrorType                                                       **
**                                                                            **
**  AVB macro driver error code.                                              **
**                                                                            **
**  If an error occurs these enums give information about the                **
**  reason.                                                                   **
**                                                                            **
**  Values:                                                                   **
**  ETH_ERR_OK           - No error                                           **
**  ETH_ERR_NG           - Unspecified error                                  **
**  ETH_ERR_RANGE        - Instance out of range                              **
**  ETH_ERR_OPMODE       - Wrong operation mode                               **
**  ETH_ERR_INVNUM       - Invalid number                                     **
**  ETH_ERR_LOCKED       - Device locked                                      **
**  ETH_ERR_NOTLOCKED    - Device not locked                                  **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_043] */
typedef enum ETag_Eth_ErrorType
{
  ETH_ERR_OK         = 0,
  ETH_ERR_NG         ,
  ETH_ERR_RANGE      ,
  ETH_ERR_OPMODE     ,
  ETH_ERR_INVNUM     ,
  ETH_ERR_LOCKED     ,
  ETH_ERR_NOTLOCKED
} Eth_ErrorType;

/*******************************************************************************
**  Enum: Eth_MainFunctionType                                                **
**                                                                            **
**  AVB mode transition status                                                **
**                                                                            **
**  Members:                                                                  **
**    AVB_IDLE       - No mode transition                                     **
**    AVB_OPER       - Operation mode transition                              **
**    AVB_CONFIG     - Configuration mode transition                          **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_102] */
typedef enum ETag_Eth_MainFunctionType
{
  AVB_IDLE      = 0,
  AVB_OPER,
  AVB_CONFIG
} Eth_MainFunctionType;

/*******************************************************************************
**  Enum: Eth_GPtpClkType                                                     **
**                                                                            **
**  gPTP clock source                                                         **
**                                                                            **
**  Members:                                                                  **
**    ETH_GPTP_DISABLE   - gPTP support disabled                              **
**    ETH_GPTP_PBUS      - gPTP based on peripheral bus clock                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_046] */
typedef enum ETag_Eth_GPtpClkType
{
  ETH_GPTP_DISABLE    = 0,
  ETH_GPTP_PBUS
} Eth_GPtpClkType;

/*******************************************************************************
**  Type: Eth_SFOptionType                                                    **
**                                                                            **
**  Stream filtering option                                                   **
**                                                                            **
**  Members:                                                                  **
**    ETH_SFDISABLE   - receive queues r>=2 are disabled                      **
**    ETH_SEPFILTER - separation filtering only                               **
**    ETH_AVBNMDISC - AVB stream with separation filtering, no matching       **
**                    frames discarded                                        **
**    ETH_AVBNMQUE0 - AVB stream with separation filtering, no matching       **
**                    frames into queue0                                      **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_049] */
typedef enum ETag_Eth_SFOptionType
{
  ETH_SFDISABLE    = 0,
  ETH_SEPFILTER,
  ETH_AVBNMDISC,
  ETH_AVBNMQUE0
} Eth_SFOptionType;

/*******************************************************************************
**  Type: Eth_UFCConfigType                                                   **
**                                                                            **
**  unread frame counter configuration                                        **
**                                                                            **
**  Members:                                                                  **
**    ETH_UFCC0 - unread frame counter configuration 0                        **
**    ETH_UFCC1 - unread frame counter configuration 1                        **
**    ETH_UFCC2 - unread frame counter configuration 2                        **
**    ETH_UFCC3 - unread frame counter configuration 3                        **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_050] */
typedef enum ETag_Eth_UFCConfigType
{
  ETH_UFCC0    = 0,
  ETH_UFCC1,
  ETH_UFCC2,
  ETH_UFCC3
} Eth_UFCConfigType;

/*******************************************************************************
**  Type: Eth_TSelType                                                        **
**                                                                            **
**  Truncation selection                                                      **
**                                                                            **
**  Members:                                                                  **
**    ETH_MAXF0 - maximum frame length defined by RTC0.MFL0                   **
**    ETH_MAXF1 - maximum frame length defined by RTC0.MFL1                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_051] */
typedef enum ETag_Eth_TSelType
{
  ETH_MAXF0    = 0,
  ETH_MAXF1
} Eth_TSelType;

/*******************************************************************************
** Type: Eth_RxSyncModeType                                                   **
**                                                                            **
** Receive synchronization mode                                               **
**                                                                            **
** Members:                                                                   **
**   ETH_RXNORMAL  - normal mode (full descriptor write back)                 **
**   ETH_RXKEEPDT  - keep DT mode (no update of DT field at descriptor write  **
**                   back)                                                    **
**   ETH_RXNOWB    - no write back (no descriptor write back)                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_052] */
typedef enum ETag_Eth_RxSyncModeType
{
  ETH_RXNORMAL    = 0,
  ETH_RXKEEPDT,
  ETH_RXNOWB
} Eth_RxSyncModeType;

/*******************************************************************************
**  Type: Eth_TxSyncModeType                                                  **
**                                                                            **
**  Transmission synchronization mode                                         **
**                                                                            **
**  Members:                                                                  **
**    ETH_TXNORMAL  - normal mode, full descriptor write back                 **
**    ETH_TXNOWB    - no write back, no descriptor write back                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_053] */
typedef enum ETag_Eth_TxSyncModeType
{
  ETH_TXNORMAL    = 0,
  ETH_TXNOWB
} Eth_TxSyncModeType;

/*******************************************************************************
**  Type: Eth_TxQPrioType                                                     **
**                                                                            **
**  Transmission queue priority                                               **
**                                                                            **
**  Members:                                                                  **
**    DEFAULT   - Default priority: Q3(CBS), Q2(CBS), Q1, Q0                  **
**    ALTERNATE - Alternate priority: Q1, Q3(CBS), Q2(CBS), Q0                **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_054] */
typedef enum ETag_Eth_TxQPrioType
{
  ETH_DEFAULT    = 0,
  ETH_ALTERNATE
} Eth_TxQPrioType;

/*******************************************************************************
**  Enum: Eth_TxChannelType                                                   **
**                                                                            **
**  Ethernet AVB Tx channel type                                              **
**                                                                            **
**  Members:                                                                  **
**    ETH_TX_BE  - Queue 0                                                    **
**    ETH_TX_NC  - Queue 1                                                    **
**    ETH_TX_SB  - Queue 2                                                    **
**    ETH_TX_SA  - Queue 3                                                    **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_060] */
typedef enum ETag_Eth_TxChannelType
{
  ETH_TX_BE      = 0,
  ETH_TX_NC,
  ETH_TX_SB,
  ETH_TX_SA
} Eth_TxChannelType;

/*******************************************************************************
**  Enum: Eth_RxBufModeType                                                   **
**                                                                            **
**  AVB Rx buffer mode                                                        **
**                                                                            **
**  Members:                                                                  **
**    ETH_RX_SFSB    - single frame single buffer                             **
**    ETH_RX_SFMB    - single frame multi buffer                              **
**    ETH_RX_SFMBINC - single frame multi buffer (incremental data area)      **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_061] */
typedef enum ETag_Eth_RxBufModeType
{
  ETH_RX_SFSB    = 0,
  ETH_RX_SFMB,
  ETH_RX_SFMBINC
} Eth_RxBufModeType;

/*******************************************************************************
**  Enum: Eth_RxChType                                                        **
**                                                                            **
**  AVB Rx Channel                                                            **
**                                                                            **
**  Members:                                                                  **
**    ETH_RX_BE    - best effort                                              **
**    ETH_RX_NC    - network control                                          **
**    ETH_RX_S     - stream                                                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_062] */
typedef enum ETag_Eth_RxChType
{
  ETH_RX_BE = 0,
  ETH_RX_NC,
  ETH_RX_S
} Eth_RxChType;

/*******************************************************************************
**  Enum: Eth_BufLockType                                                     **
**                                                                            **
**  Buffer Lock                                                               **
**                                                                            **
**  Members:                                                                  **
**    ETH_BUF_UNLOCKED - Buffer unlocked                                      **
**    ETH_BUF_LOCKED   - Buffer locked                                        **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_063] */
typedef enum ETag_Eth_BufLockType
{
  ETH_BUF_UNLOCKED = 0,
  ETH_BUF_LOCKED
} Eth_BufLockType;

/*******************************************************************************
**  Enum: Eth_ProcessingType                                                  **
**                                                                            **
**  Processing Type                                                           **
**                                                                            **
**  Members:                                                                  **
**    ETH_INTERRUPT  - Interrupt processing                                   **
**    ETH_POLLING    - Polling processing                                     **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_066] */
typedef enum ETag_Eth_ProcessingType
{
  ETH_INTERRUPT = 0,
  ETH_POLLING
} Eth_ProcessingType;

/*******************************************************************************
**  Enum: Eth_DirectionType                                                   **
**                                                                            **
**  Processing Type                                                           **
**                                                                            **
**  Members:                                                                  **
**    ETH_TX    - Transmit                                                    **
**    ETH_RX    - Receive                                                     **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_067] */
typedef enum ETag_Eth_DirectionType
{
  ETH_TX = 0,
  ETH_RX
} Eth_DirectionType;

/*******************************************************************************
**  Enum: Eth_TxQueuePolicyType                                               **
**                                                                            **
**  Shaper for a specific Queue                                               **
**                                                                            **
**  Members:                                                                  **
**    ETH_NONE   - No shaper                                                  **
**    ETH_CBS    - CBS shaper                                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_068] */
typedef enum ETag_Eth_TxQueuePolicyType
{
  ETH_NONE = 0,
  ETH_CBS
} Eth_TxQueuePolicyType;

/*******************************************************************************
** Section: Structures                                                        **
*******************************************************************************/

/*******************************************************************************
**  Type: Eth_TxCBSType                                                       **
**                                                                            **
**  Enable Credit Based Shaping and Transmission Queues Priority              **
**                                                                            **
**  Members:                                                                  **
**    Ecbs   - Enable Credit Based Shaping                                    **
**    Tqp    - Transmission Queues Priority                                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_104] */
typedef struct STag_Eth_TxCBSType
{
  uint8             Ecbs;
  Eth_TxQPrioType   Tqp;
} Eth_TxCBSType;

/*******************************************************************************
**  Type: Eth_TxConfigType                                                    **
**                                                                            **
**  Transmission configuration                                                **
**                                                                            **
**  Members:                                                                  **
**    Tsm0   - transmit synchronization mode 0 (BE class)                     **
**    Tsm1   - transmit synchronization mode 1 (NC class )                    **
**    Tsm2   - transmit synchronization mode 2 (stream class B)               **
**    Tsm3   - transmit synchronization mode 3 (stream class A)               **
**    Ecbs   - Enable Credit Based Shaping                                    **
**    Tqp    - transmission queues priority                                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_001] */
typedef struct STag_Eth_TxConfigType
{
  Eth_TxSyncModeType Tsm0;
  Eth_TxSyncModeType Tsm1;
  Eth_TxSyncModeType Tsm2;
  Eth_TxSyncModeType Tsm3;
  uint8              Ecbs;
  Eth_TxQPrioType    Tqp;
} Eth_TxConfigType;

/*******************************************************************************
**  Type: Eth_TimeStampStoreType                                              **
**                                                                            **
**  Timestamp structure                                                       **
**                                                                            **
**  Members:                                                                  **
**    Timestamp0  - timestamp[31:0]                                           **
**    Timestamp1  - timestamp[63:32]                                          **
**    Timestamp2  - timestamp[79:64]                                          **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_003] */
typedef struct STag_Eth_TimeStampStoreType
{
  uint32 Timestamp0;
  uint32 Timestamp1;
  uint16 Timestamp2;
} Eth_TimeStampStoreType;

/*******************************************************************************
**  Type: Eth_QConfigType                                                     **
**                                                                            **
**  Queue configuration                                                       **
**                                                                            **
**  Members:                                                                  **
**    Ufcc  - unread frame counter configuration                              **
**    Tsel  - truncation selection                                            **
**    Rsm   - receive synchronization mode                                    **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_007] */
typedef struct STag_Eth_QConfigType
{
  Eth_UFCConfigType  Ufcc;
  Eth_TSelType       Tsel;
  Eth_RxSyncModeType Rsm;
} Eth_QConfigType;

/*******************************************************************************
**  Type: Eth_PaddingConfigType                                               **
**                                                                            **
**  Rx padding configuration                                                  **
**                                                                            **
**  Members:                                                                  **
**    Dcnt  - data count value                                                **
**    Pcnt  - padding count value                                             **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_008] */
typedef struct STag_Eth_PaddingConfigType
{
  uint8 Dcnt;
  uint8 Pcnt;
} Eth_PaddingConfigType;

/*******************************************************************************
**  Type: Eth_ComConfigType                                                   **
**                                                                            **
**  Common configuration structure                                            **
**                                                                            **
**  Members:                                                                  **
**    Lbme    - loopback mode                                                 **
**    Csel    - clock selection, see <Eth_GPtpClkType>                        **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_011] */
typedef struct STag_Eth_ComConfigType
{
  uint8             Lbme;
  Eth_GPtpClkType   Csel;
} Eth_ComConfigType;

/*******************************************************************************
**  Type: Eth_RxConfigType                                                    **
**                                                                            **
**  Reception configuration structure                                         **
**                                                                            **
**  Members:                                                                  **
**    Effs    - enable faulty frame storage                                   **
**    Encf    - enable network control filtering                              **
**    Esf     - enable stream filtering                                       **
**    Rfcl    - reception FIFO critical level                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_012] */
typedef struct STag_Eth_RxConfigType
{
  uint8             Effs;
  uint8             Encf;
  Eth_SFOptionType  Esf;
  uint32            Rfcl;
} Eth_RxConfigType;

/*******************************************************************************
**  Type: Eth_ErrorIntType                                                    **
**                                                                            **
**  Error interrupt                                                           **
**                                                                            **
**  Members:                                                                  **
**    Mree    - MAC reception error interrupt                                 **
**    Mtee    - MAC transmission error interrupt                              **
**    Qee     - Queue error interrupt                                         **
**    See     - Separation error interrupt                                    **
**    Clle0   - CBS lower limit reached interrupt (class B)                   **
**    Clle1   - CBS lower limit reached interrupt (class A)                   **
**    Cule0   - CBS upper limit reached interrupt (class B)                   **
**    Cule1   - CBS upper limit reached interrupt (class A)                   **
**    Tffe    - Time stamp FIFO full interrupt                                **
**    Mffe    - MAC status FIFO full interrupt                                **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_013] */
typedef struct STag_Eth_ErrorIntType
{
  uint8    Mree;
  uint8    Mtee;
  uint8    Qee;
  uint8    See;
  uint8    Clle0;
  uint8    Clle1;
  uint8    Cule0;
  uint8    Cule1;
  uint8    Tffe;
  uint8    Mffe;
} Eth_ErrorIntType;

/*******************************************************************************
**  Type: Eth_AvbConfigType                                                   **
**                                                                            **
**  AVB part configuration structure                                          **
**                                                                            **
**  Members:                                                                  **
**    ComConfig   - see <Eth_ComConfigType>                                   **
**    RxConfig    - see <Eth_ComConfigType>                                   **
**    TxConfig    - see <Eth_TxConfigType>                                    **
**    ErrorInt    - see <Eth_ErrorIntType>                                    **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_014] */
typedef struct STag_Eth_AvbConfigType
{
  Eth_ComConfigType ComConfig;
  Eth_RxConfigType  RxConfig;
  Eth_TxConfigType  TxConfig;
  Eth_ErrorIntType  ErrorInt;
} Eth_AvbConfigType;

/*******************************************************************************
**  Type: Eth_MacModeType                                                     **
**                                                                            **
**  MAC configuration data structure                                          **
**                                                                            **
**  Members:                                                                  **
**    Speed           - MAC speed                                             **
**    Duplex          - duplex mode                                           **
**    MagicDetection  - magic packet detection                                **
**    TxFlowCtrl      - transmission flow control                             **
**    RxFlowCtrl      - reception flow control                                **
**    PFrameRxMode    - pause frame reception mode                            **
**    ZPFrameTx       - zero pause frame transmission with Time = 0           **
**    ZPFrameRx       - zero pause frame reception with Time = 0              **
**    DPad            - data padding                                          **
**    RxCrcMode       - reception checksum calculation                        **
**    Te              - transmission enable                                   **
**    Re              - reception enable                                      **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_015] */
typedef struct STag_Eth_MacModeType
{
  uint8     Speed;
  uint8     Duplex;
  uint8     MagicDection;
  uint8     TxFlowCtrl;
  uint8     RxFlowCtrl;
  uint8     PFrameRxMode;
  uint8     ZPFrameTx;
  uint8     ZPFrameRx;
  uint8     DPad;
  uint8     RxCrcMode;
  uint8     Te;
  uint8     Re;
} Eth_MacModeType;

/*******************************************************************************
**  Type: Eth_MacIntType                                                      **
**                                                                            **
**  MAC interrupt structure                                                   **
**                                                                            **
**  Members:                                                                  **
**    Icdip   - false carrier detect interrupt enable                         **
**    Mpdip   - magic packet detection interrupt enable                       **
**    Linkim  - link signal change interrupt enable                           **
**    Phyim   - PHY Pin interrupt enable                                      **
**    Pfrim   - PAUSE Frame Retry Interrupt Mask                              **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_016] */
typedef struct STag_Eth_MacIntType
{
  uint8  Icdip;
  uint8  Mpdip;
  uint8  Linkim;
  uint8  Phyim;
  uint8  Pfrim;
} Eth_MacIntType;

/*******************************************************************************
**  Type: Eth_MacConfigType                                                   **
**                                                                            **
**  MAC configuration data structure                                          **
**                                                                            **
**  Members:                                                                  **
**    MacAddr      - 48-bit MAC address                                       **
**    MaxFrameLen  - max. frame length                                        **
**    Mac_Mode     - see <Eth_MacModeType>                                    **
**    Mac_Int_Mask - Mac interrupt mask, see <Eth_MacIntType>                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_017] */
typedef struct STag_Eth_MacConfigType
{
  P2CONST(uint8, TYPEDEF, ETH_APPL_CONST) MacAddr;
  uint32           MaxFrameLen;
  Eth_MacModeType  Mac_Mode;
  Eth_MacIntType   Mac_Int_Mask;
} Eth_MacConfigType;

/*******************************************************************************
**  Type: Eth_OptionType                                                      **
**                                                                            **
**  Setting option                                                            **
**                                                                            **
**  Members:                                                                  **
**    ETH_DISABLE  - setting is disabled                                      **
**    ETH_ENABLE   - setting is enabled                                       **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_112] */
typedef enum ETag_Eth_OptionType
{
  ETH_DISABLE    = 0,
  ETH_ENABLE
} Eth_OptionType;

/*******************************************************************************
**  Type: Eth_TxSingleFrameType                                               **
**                                                                            **
**  Single frame structure for Tx                                             **
**                                                                            **
**  Members:                                                                  **
**    Timestamp  - timestamp function on/off                                  **
**    MacStatus  - mac status on/off                                          **
**    Tag        - Tag field                                                  **
**    Address    - frame address                                              **
**    Length     - frame length                                               **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_019] */
typedef struct STag_Eth_TxSingleFrameType
{
  uint8   Timestamp;
  Eth_OptionType MacStatus;
  uint16  Tag;
  uint32  Address;
  uint16  Length;
} Eth_TxSingleFrameType;

/*******************************************************************************
**  Type: Eth_RxSingleFrameType                                               **
**                                                                            **
**  Rx frame information structure (single frame single buffer)               **
**                                                                            **
**  Members:                                                                  **
**    FrameAddr    - frame address                                            **
**    FrameLength  - frame length                                             **
**    Timestamp    - timestamp if enabled                                     **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_020] */
typedef struct STag_Eth_RxSingleFrameType
{
  uint32                 FrameAddr;
  uint32                 FrameLength;
  Eth_TimeStampStoreType Timestamp;
} Eth_RxSingleFrameType;

/*******************************************************************************
**  Type: Eth_RxChConfigType                                                  **
**                                                                            **
**  Rx channel configuration structure                                        **
**                                                                            **
**  Members:                                                                  **
**    ChNum           - channel number                                        **
**    ChType          - channel type, see <Eth_RxChType>                      **
**    BufMode         - buffer mode, see <Eth_RxBufModeType>                  **
**    PHLength        - payload header length (for single frame multi buffer) **
**    PLength         - payload length (for single frame multi buffer)        **
**    PatternAddress  - Pattern Address to be written into SFPi               **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_022] */
typedef struct STag_Eth_RxChConfigType
{
  uint8             ChNum;
  Eth_RxChType      ChType;
  Eth_RxBufModeType BufMode;
  uint32            PHLength;
  uint32            PLength;
  uint8             PatternAddress[ETH_MACADDR_SIZE];
} Eth_RxChConfigType;

/*******************************************************************************
**  Type: Eth_RxCtrlTagType                                                   **
**                                                                            **
**  Rx specific control field structure                                       **
**                                                                            **
**  Members:                                                                  **
**    Tr   - truncation indication                                            **
**    Ei   - frame corruption detection                                       **
**    Ps   - padding selection                                                **
**    Msc  - MAC status code                                                  **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_023] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_RxCtrlTagType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Tr   : 1;
  uint32 Ei   : 1;
  uint32 Ps   : 2;
  uint32 Msc  : 8;
} Eth_RxCtrlTagType;

/*******************************************************************************
**  Type: Eth_TxCtrlTagType                                                   **
**                                                                            **
**  Tx specific control field structure                                       **
**                                                                            **
**  Members:                                                                  **
**    Tag  - frame tag                                                        **
**    Tsr  - timestamp storage request                                        **
**    Msr  - MAC status storage request                                       **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_024] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_TxCtrlTagType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Tag   : 10;
  uint32 Tsr   : 1;
  uint32 Msr   : 1;
} Eth_TxCtrlTagType;

/*******************************************************************************
**  Union: Eth_TxRxCtrlType                                                   **
**                                                                            **
**  Simplifies Bit and Byte access                                            **
**                                                                            **
**  Members:                                                                  **
**    RxCtrl  - Rx control field                                              **
**    TxCtrl  - Tx control field                                              **
**    Word    - Allow access of the whole word.                               **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_069] */
  /* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_TxRxCtrlType
  /* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_RxCtrlTagType  RxCtrl;
  Eth_TxCtrlTagType  TxCtrl;
  uint32             Word;
} Eth_TxRxCtrlType;

/*******************************************************************************
**  Type: Eth_DataDescLowType                                                 **
**                                                                            **
**  Data descriptor structure low word                                        **
**                                                                            **
**  Members:                                                                  **
**    Ds   - Size of descriptor data                                          **
**    Ctrl - content control code for Tx/Rx                                   **
**    Die  - descriptor interrupt enable                                      **
**    Dt   - descriptor type                                                  **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_025] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_DataDescLowType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Ds   : 12;
  uint32 Ctrl : 12;
  uint32 Die  : 4;
  uint32 Dt   : 4;
} Eth_DataDescLowType;

/*******************************************************************************
**  Type: Eth_LinkDescLowType                                                 **
**                                                                            **
**  Link descriptor structure low word                                        **
**                                                                            **
**  Members:                                                                  **
**    Res  - Reserved                                                         **
**    Die  - descriptor interrupt enable                                      **
**    Dt   - descriptor type                                                  **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_026] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_LinkDescLowType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Res  : 24;
  uint32 Die  : 4;
  uint32 Dt   : 4;
} Eth_LinkDescLowType;

/*******************************************************************************
**  Type: Eth_DataDescType                                                    **
**                                                                            **
**  Data descriptor structure                                                 **
**                                                                            **
**  Members:                                                                  **
**    Header - low word of the data descriptor                                **
**    Dptr   - point to the descriptor data                                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_028] */
typedef struct STag_Eth_DataDescType
{
  Eth_DataDescLowType Header;
  uint32              Dptr;
} Eth_DataDescType;

/*******************************************************************************
**  Type: Eth_ExtDataDescType                                                 **
**                                                                            **
**  Extended data descriptor structure                                        **
**                                                                            **
**  Members:                                                                  **
**    Header    - low word of the data descriptor                             **
**    Dptr      - point to the descriptor data                                **
**    Timestamp - timestamp for received frame                                **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_029] */
typedef struct STag_Eth_ExtDataDescType
{
  Eth_DataDescLowType    Header;
  uint32                 Dptr;
  Eth_TimeStampStoreType Timestamp;
} Eth_ExtDataDescType;

/*******************************************************************************
**  Type: Eth_LinkDescType                                                    **
**                                                                            **
**  Link descriptor structure                                                 **
**                                                                            **
**  Members:                                                                  **
**    Header    - low word of the link descriptor                             **
**    Dptr      - point to the next descriptor                                **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_030] */
typedef struct STag_Eth_LinkDescType
{
  Eth_LinkDescLowType Header;
  uint32              Dptr;
} Eth_LinkDescType;

/*******************************************************************************
**  Type: Eth_QueueShaperType                                                 **
**                                                                            **
**  Tx Queue Shaper                                                           **
**                                                                            **
**  Members:                                                                  **
**    civ       - Credit Increment value should be [1..65535]                 **
**    cdv       - Credit Decrement value should be [-65536 ..-1]              **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_033] */
typedef struct STag_Eth_QueueShaperType
{
  Eth_TxQueuePolicyType TxQPolicy;
  uint32 civ;
  sint32 cdv;
} Eth_QueueShaperType;

/*******************************************************************************
**  Type: Eth_TxQueueType                                                     **
**                                                                            **
**  Tx Queue Structure                                                        **
**                                                                            **
**  Members:                                                                  **
**    id                - Index of the Tx Queue                               **
**    TxQueueShaper     - Traffic Class mapped to the specified Tx Queue      **
**    QueueProcessing   - Interrupt or Polling Mode for Tx Management         **
**                          in the Respective Queue                           **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_034] */
typedef struct STag_Eth_TxQueueType
{
  uint8 id;
  Eth_QueueShaperType TxQueueShaper;
  Eth_ProcessingType  QueueProcessing;
} Eth_TxQueueType;

/*******************************************************************************
**  Type: Eth_RxQueueType                                                     **
**                                                                            **
**  Rx Queue Structure                                                        **
**                                                                            **
**  Members:                                                                  **
**    id                - Index of the Rx Queue                               **
**    EthPatternAddres  - Specify the pattern address to be used for filtering**
**    QueueProcessing   - Interrupt or Polling Mode for Rx Management         **
**                          in the Respective Queue                           **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_035] */
typedef struct STag_Eth_RxQueueType
{
  uint8 id;
  uint8 EthPatternAddres[ETH_SIX_8];
  Eth_ProcessingType QueueProcessing;
} Eth_RxQueueType;

/*******************************************************************************
**  Type: Eth_QueueConfigType                                                 **
**                                                                            **
**  Rx, Tx Queue Structure                                                    **
**                                                                            **
**  Members:                                                                  **
**    ucFirstRxQueueIdx - Index of the first Rx Queue                         **
**    pTxQueueConfig    - Tx Queue Structure pointer                          **
**    pRxQueueConfig    - Rx Queue Structure pointer                          **
**    ucLastRxQueueIdx  - Index of the last Rx Queue                          **
**    ucNumberOfTxQueue - Number of Tx Queue                                  **
**    ucNumberOfRxQueue - Number of Rx Queue                                  **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_036] */
typedef struct STag_Eth_QueueConfigType
{
  uint8 ucFirstRxQueueIdx;
  P2CONST(Eth_TxQueueType, TYPEDEF, ETH_APPL_CONST) pTxQueueConfig;
  P2CONST(Eth_RxQueueType, TYPEDEF, ETH_APPL_CONST) pRxQueueConfig;
  uint8 ucLastRxQueueIdx;
  uint8 ucNumberOfTxQueue;
  uint8 ucNumberOfRxQueue;
} Eth_QueueConfigType;

#if (ETH_QOS_SUPPORT == STD_ON)
/*******************************************************************************
**  Type: Eth_PriorityMappingType                                             **
**                                                                            **
**  Traffic Priorities Table                                                  **
**                                                                            **
**  Members:                                                                  **
**    PriorityValue     - Priority assigned to a particular traffic class     **
**    TxQueueId         - Reference to the Tx Queue associated to the         **
**                          specified Traffic Priority                        **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_037] */
typedef struct STag_Eth_PriorityMappingType
{
  uint8 PriorityValue ;
  uint8 TxQueueId;
} Eth_PriorityMappingType;

/*******************************************************************************
**  Type: Eth_CtrlPriorityType                                                **
**                                                                            **
**  Specify Controller Priority                                               **
**                                                                            **
**  Members:                                                                  **
**    DflTxQueue           - Default Tx Queue for unmapped traffic            **
**    pPriorityTable       - Array to collect all configured Priorities       **
**    ucNumberOfPriorities - Number of Traffic Priorities configured          **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_038] */
typedef struct STag_Eth_CtrlPriorityType
{
  uint8 DflTxQueue;
  P2CONST(Eth_PriorityMappingType, TYPEDEF, ETH_APPL_CONST)
    pPriorityTable;
  uint8 ucNumberOfPriorities;
} Eth_CtrlPriorityType;
#endif

/*******************************************************************************
**  Type: Eth_ClockDelayType                                                  **
**                                                                            **
**  Enable Tx/Rx clock internal Delay Mode                                    **
**                                                                            **
**  Members:                                                                  **
**    Tdm   - Enable Tx clock internal Delay Mode                             **
**    Rdm   - Enable Rx clock internal Delay Mode                             **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_105] */
typedef struct STag_Eth_ClockDelayType
{
  uint8             Tdm;
  uint8             Rdm;
} Eth_ClockDelayType;

/*******************************************************************************
**  Type: Eth_ConfigType                                                      **
**                                                                            **
**  Implementation specific structure of the post build configuration         **
**                                                                            **
**  Members:                                                                  **
**    ulStartOfDbToc    - Database start value                                **
**    ucEthMACAddr      - MAC address                                         **
**    ucEthSpeed        - Ethernet speed 100M/1G                              **
**    ucEthDuplex       - Mac duplex mode                                     **
**    ucEthMagicDection - Ethernet Magic Packet Detection                     **
**    ucEthPHYInterface - Enables / Disables Media Independent Interface (MII)**
**                          for transceiver access                            **
**    Eth_QueueConfig   - Rx, Tx Queue configuration                          **
**    stComConfig       - Common configuration                                **
**    stRxConfig        - Reception configuration structure                   **
**    stTxConfig        - Credit Based Shaping and                            **
**                          Transmission Queues Priority                      **
**    stEthPriority     - Specify Controller Priority                         **
**    stClkDelayConfig  - Clock delay configuration                           **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_039] */
typedef struct STag_Eth_ConfigType
{
  /* Database start value - ETH_DBTOC_VALUE */
  uint32                  ulStartOfDbToc;
  uint8                   ucEthMACAddr[ETH_MACADDR_SIZE];
  uint8                   ucEthSpeed;
  uint8                   ucEthDuplex;
  uint8                   ucEthMagicDection;
  P2FUNC(void, ETH_APPL_CODE, pMagicNotification)(void);
  uint8                   ucEthPHYInterface;
  #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
  uint8                   ucStartOfPayloadCrcArea;
  #endif
  Eth_QueueConfigType     Eth_QueueConfig;
  Eth_ComConfigType       stComConfig;
  Eth_RxConfigType        stRxConfig;
  Eth_TxCBSType           stTxConfig;
  #if (ETH_QOS_SUPPORT == STD_ON)
  Eth_CtrlPriorityType    stEthPriority;
  #endif
  Eth_ClockDelayType      stClkDelayConfig;
} Eth_ConfigType;

/*******************************************************************************
**  Type: Eth_BufHandlerType                                                  **
**                                                                            **
**  Setting option                                                            **
**                                                                            **
**  Members:                                                                  **
**    bufLock   - Flag To store info if the buffer is allocated or not        **
**    descAddr    - Descriptor used to transmit the associated buffer         **
**    EnableTS -  Flag to store if for the corresponding buffer TimeStamp     **
**                shall be stored                                             **
**    bufTxConfirm -  Flag to store Tx Confirmation for a transmit buffer     **
**    bufTxFlag -  Flag to identify the buffer in under transmission or not   **
**    ucChannel - Tx Queue used to transmit the associated buffer             **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_040] */
typedef struct STag_Eth_BufHandlerType
{
  Eth_BufLockType   bufLock;
  uint32            descAddr;
  boolean           benableTS;
  boolean           bufTxConfirm;
  boolean           bufTxFlag;
  Eth_TxChannelType ucChannel;
} Eth_BufHandlerType;

/*******************************************************************************
**  Type: Eth_StatisticsType                                                  **
**                                                                            **
**  Collects Statistics returned to the User via GetEtherStats                **
**                                                                            **
**  Members:                                                                  **
**    RxPacketNo                 - Counter Total Rx Packets Received          **
**    BroadcastPktNo             - Counter Total Rx Broadcast Packet          **
**    LastMulticastPktNo         - Last Counter Total Rx Multicast Packet     **
**    StatsOctetsNo              - Counter Total Rx Octets Number             **
**    Pkts64OctetsNo             - Counter Total Rx 64 Octets Packets         **
**    Pkts65to127OctetsNo        - Counter Total Rx 65 to 127 Octets Packets  **
**    Pkts128to255OctetsNo       - Counter Total Rx 128 to 255 Octets Packets **
**    Pkts256to511OctetsNo       - Counter Total Rx 256 to 511 Octets Packets **
**    Pkts512to1023OctetsNo      - Counter Total Rx 512 to 1023 Octets Packets**
**    Pkts1024to1518OctetsNo     - Counter Total Rx 1024 to 1518 Octets       **
**                                 Packets                                    **
*******************************************************************************/
#if (ETH_GET_ETHER_STATS_API == STD_ON)
/* TRACE[ETH_DDD_DAT_103] */
typedef struct STag_Eth_StatisticsType
{
  uint32 RxPacketNo;
  uint32 BroadcastPktNo;
  uint32 LastMulticastPktNo;
  uint32 StatsOctetsNo;
  uint32 Pkts64OctetsNo;
  uint32 Pkts65to127OctetsNo;
  uint32 Pkts128to255OctetsNo;
  uint32 Pkts256to511OctetsNo;
  uint32 Pkts512to1023OctetsNo;
  uint32 Pkts1024to1518OctetsNo;
} Eth_StatisticsType;
#endif
/*******************************************************************************
**  Type: Eth_DropCountType                                                   **
**                                                                            **
**  Collects Drop Counter returned to the User via Eth_GetDropCount           **
**                                                                            **
**  Members:                                                                  **
**    DiscardedPacketNo            - Counter Total Rx Discarded Packets       **
**    PayloadCRCErrPacketNo        - Counter Payload CRC Error packets        **
*******************************************************************************/
#if (ETH_GET_DROP_COUNT_API == STD_ON)
/* TRACE[ETH_DDD_DAT_113] */
typedef struct STag_Eth_DropCountType
{
  uint32             DiscardedPacketNo;
  #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
  uint32             PayloadCRCErrPacketNo;
  #endif
} Eth_DropCountType;
#endif
/*******************************************************************************
**  Type: Eth_GaaDeviceType                                                   **
**                                                                            **
**  Device instance specific data.                                            **
**                                                                            **
**  All instance specific data is kept within that structure.                 **
**                                                                            **
**  Members:                                                                  **
**    MacAddr                      - MAC address                              **
**    UFCOverflow                  - Unread frame counter overflow flag       **
**    NextRxDesc                   - Next free Rx descriptor address          **
**    Stats                        - Statistics (e.g. RxPacketNo, BroadcastNo)**
**    DropCount                    - Drop counters (e.g. DiscardedPacketNo,   **
**                                   ErrInboundPacketNo                       **
**    LastRxErrPacketMainApi       - Last Counter Receive Error packets       **
**                                   for Eth_MainFunction API                 **
**    LastResBitPacketMainApi      - Last Counter Residual-Bit packets        **
**                                   for Eth_MainFunction API                 **
**    LastRxErrPacketNo            - Last Counter Receive Error packets       **
**    LastResBitPacketNo           - Last Counter Residual-Bit packets        **
**    LastTxRetryOverPacketNo      - Last Counter Transmit retry over packets **
**    LastTxRetryOverPacketMainApi - Last Counter Transmit retry over packets **
**                                   for Eth_MainFunction API                 **
**    LastCRCPacketNo              - Last Counter CRC error packets           **
**    LastCRCPacketMainApi         - Last Counter CRC error packets           **
**                                   for Eth_MainFunction API                 **
**    DropPacketNo                 - Counter Total Rx Frames Lost             **
**    LastDropPacketNoMainApi      - Last Counter Total Rx Frames Lost        **
**                                   for Eth_MainFunction API                 **
**    LastOversizePacketNo         - Last Counter Oversize Packets            **
**    LastOversizePacketMainApi    - Last Counter Oversize Packets            **
**                                   for Eth_MainFunction API                 **
**    LastUndersizePacketNo        - Last Counter Undersize Packets           **
**    LastUndersizePacketMainApi   - Last Counter Undersize Packets           **
**                                   for Eth_MainFunction API                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_041] */
typedef struct STag_Eth_GaaDeviceType
{
  uint8              MacAddr[ETH_MACADDR_SIZE];
  boolean            UFCOverflow[ETH_RXQ_MAXNUM];
  uint32             NextRxDesc[ETH_RXQ_MAXNUM];
  #if (ETH_GET_ETHER_STATS_API == STD_ON)
  Eth_StatisticsType Stats;
  #endif
  #if (ETH_GET_DROP_COUNT_API == STD_ON)
  Eth_DropCountType  DropCount;
  uint32             LastRxErrPacketMainApi;
  uint32             LastResBitPacketMainApi;
  uint32             LastRxErrPacketNo;
  uint32             LastResBitPacketNo;
  uint32             LastTxRetryOverPacketNo;
  uint32             LastTxRetryOverPacketMainApi;
  #endif
  #if ((ETH_GET_DROP_COUNT_API == STD_ON) || \
    (ETH_GET_ETHER_STATS_API == STD_ON))
  uint32             LastCRCPacketNo;
  uint32             LastCRCPacketMainApi;
  uint32             LastOversizePacketNo;
  uint32             LastOversizePacketMainApi;
  uint32             LastUndersizePacketNo;
  uint32             LastUndersizePacketMainApi;
  #endif
  uint32             DropPacketNo;
  uint32             LastDropPacketNoMainApi;
} Eth_GaaDeviceType;

/*******************************************************************************
**  Type: Eth_RxFilterAddrType                                                **
**                                                                            **
**  All physical filter addresses are kept within that structure.             **
**                                                                            **
**  Members:                                                                  **
**    RxFilterLength     - Store the number of filter addresses               **
**    RxFilterAddress    - Array that store all filter addresses              **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_108] */
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
typedef struct STag_Eth_RxFilterAddrType
{
  uint8 RxFilterLength;
  uint8 RxFilterAddress[ETH_MAX_PHYS_FILTER_ADDR][ETH_MACADDR_SIZE];
} Eth_RxFilterAddrType;
#endif
/*******************************************************************************
**  Enum: Eth_FilterModeType                                                  **
**                                                                            **
**  Physical Address Filter Mode                                              **
**                                                                            **
**  Members:                                                                  **
**    ETH_NORMAL_MODE    - No filter                                          **
**    ETH_FILTER_MODE    - Filter mode                                        **
**    ETH_PROM_MODE      - Promiscuous mode                                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_109] */
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
typedef enum ETag_Eth_FilterModeType
{
  ETH_NORMAL_MODE    = 0,
  ETH_FILTER_MODE,
  ETH_PROM_MODE
} Eth_FilterModeType;
#endif

/*******************************************************************************
**  Enum: Eth_OpStatusType                                                    **
**                                                                            **
**  AVB operation status                                                      **
**                                                                            **
**  Members:                                                                  **
**    ETH_RESET      - reset                                                  **
**    ETH_CONFIG     - configuration                                          **
**    ETH_OPERATION  - operation                                              **
**    ETH_STANDBY    - standby                                                **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_110] */
typedef enum ETag_Eth_OpStatusType
{
  ETH_RESET      = 0,
  ETH_CONFIG,
  ETH_OPERATION,
  ETH_STANDBY
} Eth_OpStatusType;
/*******************************************************************************
**  Enum: Eth_SpeedType                                                       **
**                                                                            **
**  Mac speed                                                                 **
**                                                                            **
**  Members:                                                                  **
**    ETH_100M  - 10/100MBps                                                  **
**    ETH_1G    - 1GBps                                                       **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_111] */
typedef enum ETag_Eth_SpeedType
{
  ETH_100M    = 0,
  ETH_1G
} Eth_SpeedType;
/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif  /* ETH_TYPES_H */

/*******************************************************************************
**                          End Of File                                       **
*******************************************************************************/
