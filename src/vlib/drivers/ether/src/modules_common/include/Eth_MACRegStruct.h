/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_MACRegStruct.h                                          */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of Controller register structure for MAC.                        */
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
* V1.1.0:  30-Sep-2016  : Added ETH_CDCR, ETH_LCCR.
* V1.1.2:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.3:  17-Apr-2017  : Update Eth_EcsiprBitsType follow ECSIPR register
*                         description in latest HW user manual Rev.0.53
* V1.1.4:  27-Nov-2017  : Remove unused macro definitions
* V1.1.5:  23-Apr-2020  : Updated MISRA-C violation messages
*******************************************************************************/

#ifndef ETH_MACREGSTRUCT_H
#define ETH_MACREGSTRUCT_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "Eth_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_MACREGSTRUCT_AR_RELEASE_MAJOR_VERSION \
  ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_MACREGSTRUCT_AR_RELEASE_MINOR_VERSION \
  ETH_AR_RELEASE_MINOR_VERSION
#define ETH_MACREGSTRUCT_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_MACREGSTRUCT_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_MACREGSTRUCT_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

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
/******************************************************************************/

/*******************************************************************************
  Constants: MAC Register Offsets
*******************************************************************************/
/* MAC mode register */
#define ETH_ECMR    (uint32)0x500
/* Receive frame length register */
#define ETH_RFLR    (uint32)0x508
/* MAC status register */
#define ETH_ECSR    (uint32)0x510
/* MAC interrupt permission register */
#define ETH_ECSIPR  (uint32)0x518
/* PHY interface register */
#define ETH_PIR     (uint32)0x520
/* MAC mode register 2 */
#define ETH_GECMR   (uint32)0x5B0
/* MAC address high register */
#define ETH_MAHR    (uint32)0x5C0
/* MAC address low register */
#define ETH_MALR    (uint32)0x5C8
/* Transmit retry over count register */
#define ETH_TROCR   (uint32)0x700
/* CRC error frame receive counter */
#define ETH_CEFCR   (uint32)0x740
/* Frame receive error counter register */
#define ETH_FRECR   (uint32)0x748
/* Too short frame receive counter */
#define ETH_TSFRCR  (uint32)0x750
/* Too long frame receive counter */
#define ETH_TLFRCR  (uint32)0x758
/* Residual-bit frame receive counter */
#define ETH_RFCR    (uint32)0x760
/* Multicast address frame counter */
#define ETH_MAFCR   (uint32)0x778
/* ECMR.PRM Bit position */
#define ETH_ECMR_PRM_BIT    (uint8)0x00
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**  Type: Eth_EcmrBitsType
**
**  Content of register ECMR
**
**  Members:
**    Prm         - Promiscuous mode
**    Dm          - duplex mode
**    Te          - transmission enable
**    Re          - reception enable
**    Mpde        - magic packet detection
**    Cer         - debugging function bit
**    Txf         - transmission flow control
**    Rxf         - reception flow control
**    Pfr         - pause frame reception mode
**    Tzpf        - zero pause frame transmission with Time = 0
**    Rzpf        - zero pause frame reception with Time = 0
**    Dpad        - data padding disable (1 for disable, 0 for enable!!!!!!)
**    Rcsc        - reception checksum calculation
**    Trccm       - Counter clear mode
**    Res1 - Res7 - reserved
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_080] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_EcmrBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Prm   : 1;
  uint32 Dm    : 1;
  uint32 Res1  : 3;
  uint32 Te    : 1;
  uint32 Re    : 1;
  uint32 Res2  : 2;
  uint32 Mpde  : 1;
  uint32 Res3  : 2;
  uint32 Cer   : 1;
  uint32 Res4  : 3;
  uint32 Txf   : 1;
  uint32 Rxf   : 1;
  uint32 Pfr   : 1;
  uint32 Tzpf  : 1;
  uint32 Rzpf  : 1;
  uint32 Dpad  : 1;
  uint32 Res5  : 1;
  uint32 Rcsc  : 1;
  uint32 Res6  : 2;
  uint32 Trccm : 1;
  uint32 Res7  : 5;
} Eth_EcmrBitsType;

/*******************************************************************************
**  Union: Eth_EcmrType
**
**  Simplifies Bit and Byte access to register
**
**  Members:
**    Bit   - Allow bit access.
**    Word  - Allow access of the whole word.
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_081] */
/* START Msg(4:0750)-3*/
typedef union UTag_Eth_EcmrType
{
  Eth_EcmrBitsType Bit;
  uint32           Word;
} Eth_EcmrType;
/* END Msg(4:0750)-3*/

/*******************************************************************************
**  Type: Eth_EcsiprBitsType
**
**  Content of register Ecsipr
**
**  Members:
**    Icdip  - illegal carrier detection interrupt enable
**    Mpdip  - magic packet detection interrupt enable
**    Linkim - link signal change interrupt enable
**    Phyim  - PHY Pin interrupt enable
**    Pfrim  - PAUSE Frame Retry Interrupt Mask
**    Res1   - reserved
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_082] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_EcsiprBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Icdip  : 1;
  uint32 Mpdip  : 1;
  uint32 Linkim : 1;
  uint32 Phyim  : 1;
  uint32 Pfrim  : 1;
  uint32 Res1   : 27;
} Eth_EcsiprBitsType;

/*******************************************************************************
**  Union: Eth_EcsiprType
**
**  Simplifies Bit and Byte access to register
**
**  Members:
**    Bit   - Allow bit access.
**    Word  - Allow access of the whole word.
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_083] */
/* START Msg(4:0750)-3*/
typedef union UTag_Eth_EcsiprType
{
  Eth_EcsiprBitsType Bit;
  uint32             Word;
} Eth_EcsiprType;
/* END Msg(4:0750)-3*/

/*******************************************************************************
**  Type: Eth_GecmrBitsType
**
**  Content of register Gecmr
**
**  Members:
**    Speed   - transmission speed
**    Bse     - burst transmission enable
**    Res1    - reserved
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_084] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_GecmrBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32    Speed : 1;
  uint32    Bse   : 1;
  uint32    Res1  : 30;
} Eth_GecmrBitsType;

/*******************************************************************************
**  Union: Eth_GecmrType
**
**  Simplifies Bit and Byte access to register
**
**  Members:
**    Bit   - Allow bit access.
**    Word  - Allow access of the whole word.
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_085] */
/* START Msg(4:0750)-3*/
typedef union UTag_Eth_GecmrType
{
  Eth_GecmrBitsType Bit;
  uint32            Word;
} Eth_GecmrType;
/* END Msg(4:0750)-3*/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif  /* ETH_MACREGSTRUCT_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
