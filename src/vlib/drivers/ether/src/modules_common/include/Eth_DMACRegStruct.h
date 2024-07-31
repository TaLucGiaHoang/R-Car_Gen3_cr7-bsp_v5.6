/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_DMACRegStruct.h                                         */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of Controller register structure for DMAC.                       */
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
*                         Implement Error Detection Circuit code
*                         Unify version number
* V1.1.0:  08-Dec-2016  : Revised some macros after renew RCar_Types.h
* V1.1.1:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.2:  12-Sep-2017  : Remove the definition of ETH_BASE macro
* V1.1.3:  27-Nov-2017  : Remove unused macro definitions
* V1.2.0:  12-Dec-2017  : Added Eth_ApsrType and Eth_ApsrBitsType
* V1.2.1:  23-Apr-2020  : Updated MISRA-C violation messages
*******************************************************************************/

#ifndef ETH_DMACREGSTRUCT_H
#define ETH_DMACREGSTRUCT_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "Eth_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_DMACREGSTRUCT_AR_RELEASE_MAJOR_VERSION    \
  ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_DMACREGSTRUCT_AR_RELEASE_MINOR_VERSION    \
  ETH_AR_RELEASE_MINOR_VERSION
#define ETH_DMACREGSTRUCT_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_DMACREGSTRUCT_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_DMACREGSTRUCT_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

#define ETH_CCC        (uint32)0x0
#define ETH_DBAT       (uint32)0x4
#define ETH_DLR        (uint32)0x8
#define ETH_CSR        (uint32)0xC
#define ETH_CDARBASE   (uint32)0x10
#define ETH_ESR        (uint32)0x88
#define ETH_APSR       (uint32)0x8C
/* MISRA Violation: START Msg(4:0857)-4 */
#define ETH_PSR        (uint32)0x528
/* MISRA Violation: END Msg(4:0857)-4 */
#define ETH_RCR        (uint32)0x90
#define ETH_RQCBASE    (uint32)0x94
#define ETH_RPC        (uint32)0xB0
#define ETH_UFCVBASE   (uint32)0xC4
#define ETH_UFCDBASE   (uint32)0xE0
#define ETH_SFO        (uint32)0xFC
#define ETH_SFPBASE    (uint32)0x100U
#define ETH_SFM0       (uint32)0x1C0U
#define ETH_SFM1       (uint32)0x1C4U
#define ETH_PCRC       (uint32)0x1CC

#define ETH_TGC        (uint32)0x300
#define ETH_TCCR       (uint32)0x304
#define ETH_TSR        (uint32)0x308
#define ETH_TFA0       (uint32)0x310
#define ETH_TFA2       (uint32)0x318
#define ETH_CIVR0      (uint32)0x320
#define ETH_CIVR1      (uint32)0x324
#define ETH_CDVR0      (uint32)0x328
#define ETH_CDVR1      (uint32)0x32C
#define ETH_CUL0       (uint32)0x330
#define ETH_CUL1       (uint32)0x334
#define ETH_CLL0       (uint32)0x338
#define ETH_CLL1       (uint32)0x33C
#define ETH_CIE        (uint32)0x384
#define ETH_DIL        (uint32)0x440
#define ETH_VRR        (uint32)0x31C

#define ETH_DIC        (uint32)0x350
#define ETH_DIS        (uint32)0x354
#define ETH_EIC        (uint32)0x358
#define ETH_EIS        (uint32)0x35C
#define ETH_RIC0       (uint32)0x360
#define ETH_RIS0       (uint32)0x364
#define ETH_RIC1       (uint32)0x368
#define ETH_RIS1       (uint32)0x36C
#define ETH_RIC2       (uint32)0x370
#define ETH_RIS2       (uint32)0x374
#define ETH_RIC3       (uint32)0x388
#define ETH_RIS3       (uint32)0x38C
#define ETH_TIC        (uint32)0x378
#define ETH_TIS        (uint32)0x37C
#define ETH_GIS        (uint32)0x3B0
#define ETH_EIL        (uint32)0x444
#define ETH_TIL        (uint32)0x448
#define ETH_GIL        (uint32)0x3D4


#define ETH_GCCR       (uint32)0x390
#define ETH_GTI        (uint32)0x39C
#define ETH_GTO0       (uint32)0x3A0
#define ETH_GTO1       (uint32)0x3A4
#define ETH_GTO2       (uint32)0x3A8
#define ETH_GIC        (uint32)0x3AC
#define ETH_GCTBASE    (uint32)0x3B8

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
/* 4. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0857) [L] Number of macro definitions exceeds 1024      */
/*                 - program does not conform strictly to ISO:C90.            */
/* Rule          : MISRA-C:2004 Rule 1.1                                      */
/* Justification : Macros are used to easy verify and maintain the source     */
/*                 code, many macros are defined because ETH has many specific*/
/*                 keywords need to define                                    */
/* Verification  : Number of macro definitions exceeds 1024 does not affect to*/
/*                 ETH operation. All macro definitions have been reviewed    */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0857)-4 and                           */
/*                 END Msg(4:0857)-4 tags in the code.                        */
/******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
/*******************************************************************************
**  Type: Eth_CccBitsType                                                     **
**                                                                            **
**  Content of register CCC                                                   **
**                                                                            **
**  Members:                                                                  **
**    Opc     - operation command                                             **
**    Gac     - Gptp Active in Configuration                                  **
**    Dtsr    - data transfer suspend request                                 **
**    Csel    - clock select                                                  **
**    Lbme    - loopback mode enable                                          **
**    Fce     - Flow Control Enable                                           **
**    Res1-4  - reserved                                                      **
**                                                                            **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_086] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_CccBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Opc   : 2;
  uint32 Res1  : 5;
  uint32 Gac   : 1;
  uint32 Dtsr  : 1;
  uint32 Res2  : 7;
  uint32 Csel  : 2;
  uint32 Res3  : 6;
  uint32 Lbme  : 1;
  uint32 Fce   : 1;
  uint32 Res4  : 6;
} Eth_CccBitsType;
/*******************************************************************************
**  Union: Eth_CccType                                                        **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_087] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_CccType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_CccBitsType  Bit;
  uint32           Word;
} Eth_CccType;

/*******************************************************************************
**  Type: Eth_CsrBitsType                                                     **
**                                                                            **
**  Content of register CSR                                                   **
**                                                                            **
**  Members:                                                                  **
**    Ops     - operation status                                              **
**    Dts     - data transfer suspended                                       **
**    Tpo0    - transmission process ongoing (best effort)                    **
**    Tpo1    - transmission process ongoing (network control)                **
**    Tpo2    - transmission process ongoing (stream class B)                 **
**    Tpo3    - transmission process ongoing (stream class A)                 **
**    Rpo     - reception process ongoing                                     **
**    Res1-3  - reserved                                                      **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_088] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_CsrBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Ops   : 4;
  uint32 Res1  : 4;
  uint32 Dts   : 1;
  uint32 Res2  : 7;
  uint32 Tpo0  : 1;
  uint32 Tpo1  : 1;
  uint32 Tpo2  : 1;
  uint32 Tpo3  : 1;
  uint32 Rpo   : 1;
  uint32 Res3  : 11;
} Eth_CsrBitsType;
/*******************************************************************************
**  Union: Eth_CsrType                                                        **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_089] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_CsrType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_CsrBitsType Bit;
  uint32          Word;
} Eth_CsrType;

/*******************************************************************************
**  Type: Eth_RcrBitsType                                                     **
**                                                                            **
**  Content of register RCR                                                   **
**                                                                            **
**  Members:                                                                  **
**    Effs    - enable faulty frame storage                                   **
**    Encf    - enable network control filtering                              **
**    Esf     - enable stream filtering                                       **
**    Ets0    - enable timestamp storage 0 (best effort)                      **
**    Ets2    - enable timestamp storage 2 (stream)                           **
**    Rfcl    - reception FIFO critical level                                 **
**    Res1-2  - reserved                                                      **
**                                                                            **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_090] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_RcrBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Effs  : 1;
  uint32 Encf  : 1;
  uint32 Esf   : 2;
  uint32 Ets0  : 1;
  uint32 Ets2  : 1;
  uint32 Res1  : 10;
  uint32 Rfcl  : 13;
  uint32 Res2  : 3;
} Eth_RcrBitsType;
/*******************************************************************************
**  Union: Eth_RcrType                                                        **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_091] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_RcrType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_RcrBitsType  Bit;
  uint32           Word;
} Eth_RcrType;

/*******************************************************************************
**  Type: Eth_TgcBitsType                                                     **
**                                                                            **
**  Content of register TGC                                                   **
**                                                                            **
**  Members:                                                                  **
**    Tsm0   - transmit synchronization mode0 (best effort)                   **
**    Tsm1   - transmit synchronization mode1 (network control)               **
**    Tsm2   - transmit synchronization mode2 (stream class B)                **
**    Tsm3   - transmit synchronization mode3 (stream class A)                **
**    Ecbs   - Enable Credit Based Shaping                                    **
**    Tqp    - transmission queues priority                                   **
**    Tbd0   - transmit buffer depth 0 (best effort)                          **
**    Tbd1   - transmit buffer depth 1 (network control)                      **
**    Tbd2   - transmit buffer depth 2 (stream class B)                       **
**    Tbd3   - transmit buffer depth 3 (stream class A)                       **
**    Res1-5 - reserved                                                       **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_094] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_TgcBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Tsm0   : 1;
  uint32 Tsm1   : 1;
  uint32 Tsm2   : 1;
  uint32 Tsm3   : 1;
  uint32 Ecbs   : 1;
  uint32 Tqp    : 1;
  uint32 Res1   : 2;
  uint32 Tbd0   : 2;
  uint32 Res2   : 2;
  uint32 Tbd1   : 2;
  uint32 Res3   : 2;
  uint32 Tbd2   : 2;
  uint32 Res4   : 2;
  uint32 Tbd3   : 2;
  uint32 Res5   : 10;
} Eth_TgcBitsType;
/*******************************************************************************
**  Union: Eth_TgcType                                                        **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_095] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_TgcType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_TgcBitsType  Bit;
  uint32           Word;
} Eth_TgcType;

/*******************************************************************************
**  Type: Eth_TccrBitsType                                                    **
**                                                                            **
**  Content of register TCCR                                                  **
**                                                                            **
**  Members:                                                                  **
**    Tsrq0   - transmission start request queue0 (best effort)               **
**    Tsrq1   - transmission start request queue1 (network control)           **
**    Tsrq2   - transmission start request queue2 (stream class B)            **
**    Tsrq3   - transmission start request queue3 (stream class A)            **
**    Tfen    - timestamp FIFO enable                                         **
**    Tfr     - timestamp FIFO release                                        **
**    Mfen    - MAC status FIFO enable                                        **
**    Mfr     - MAC status FIFO release                                       **
**    Res1-3  - reserved                                                      **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_096] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_TccrBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Tsrq0   : 1;
  uint32 Tsrq1   : 1;
  uint32 Tsrq2   : 1;
  uint32 Tsrq3   : 1;
  uint32 Res1    : 4;
  uint32 Tfen    : 1;
  uint32 Tfr     : 1;
  uint32 Res2    : 6;
  uint32 Mfen    : 1;
  uint32 Mfr     : 1;
  uint32 Res3    : 14;
} Eth_TccrBitsType;
/*******************************************************************************
**  Union: Eth_TccrType                                                       **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_097] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_TccrType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_TccrBitsType  Bit;
  uint32            Word;
} Eth_TccrType;

/*******************************************************************************
**  Type: Eth_EicBitsType                                                     **
**                                                                            **
**  Content of register EIC                                                   **
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
**    Tffe    - Timerstamp FIFO full interrupt                                **
**    Mffe    - MAC status FIFO full interrupt                                **
**    Res     - reserved                                                      **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_100] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_EicBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Mree  : 1;
  uint32 Mtee  : 1;
  uint32 Qee   : 1;
  uint32 See   : 1;
  uint32 Clle0 : 1;
  uint32 Clle1 : 1;
  uint32 Cule0 : 1;
  uint32 Cule1 : 1;
  uint32 Tffe  : 1;
  uint32 Mffe  : 1;
  uint32 Res   : 22;
} Eth_EicBitsType;
/*******************************************************************************
**  Union: Eth_EicType                                                        **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_101] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_EicType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_EicBitsType  Bit;
  uint32           Word;
} Eth_EicType;

/*******************************************************************************
**  Type: Eth_ApsrBitsType                                                    **
**                                                                            **
**  Content of register APSR                                                  **
**                                                                            **
**  Members:                                                                  **
**    Rsta    - rx Signals Timing Adjuster                                    **
**    Mem     - memory Error Mode Selection                                   **
**    Cmsw    - select width of internal compare match signal                 **
**    Rdm     - Rx clock internal Delay Mode                                  **
**    Tdm     - Tx clock internal Delay Mode                                  **
**    Erren   - dummy error setting                                           **
**    Res1-4  - reserved                                                      **
**                                                                            **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_106] */
/* MISRA Violation: START Msg(6:0635)-1 */
typedef struct STag_Eth_ApsrBitsType
/* MISRA Violation: END Msg(6:0635)-1 */
{
  uint32 Rsta  : 1;
  uint32 Mem   : 1;
  uint32 Res1  : 2;
  uint32 Cmsw  : 1;
  uint32 Res2  : 8;
  uint32 Rdm   : 1;
  uint32 Tdm   : 1;
  uint32 Res3  : 1;
  uint32 Erren : 4;
  uint32 Res4  : 12;
} Eth_ApsrBitsType;
/*******************************************************************************
**  Union: Eth_ApsrType                                                       **
**                                                                            **
**  Simplifies Bit and Byte access to register                                **
**                                                                            **
**  Members:                                                                  **
**    Bit   - Allow bit access.                                               **
**    Word  - Allow access of the whole word.                                 **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_107] */
/* MISRA Violation: START Msg(4:0750)-3 */
typedef union UTag_Eth_ApsrType
/* MISRA Violation: END Msg(4:0750)-3 */
{
  Eth_ApsrBitsType Bit;
  uint32           Word;
} Eth_ApsrType;
/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif  /* ETH_DMACREGSTRUCT_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
