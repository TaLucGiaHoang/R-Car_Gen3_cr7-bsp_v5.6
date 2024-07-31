/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_PBTypes.h                                               */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains the type definitions of Post Build time Parameters      */
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
*                         Implement Interrupt Controller Check code
*                         Unify version number
* V1.1.0:  08-Dec-2016  : Add macro ETH_REG_MASK
*                         Change behavior of Eth_IsQueueConfigured function.
*                         Adding additional Interrupt handlers for testing.
* V1.1.1:  22-Mar-2017  : Add macro ETH_HEADER_SIZE (#77452)
*                         Updated ETH_EIS_ERROR_MASK based on latest
*                         HW User Manual v0.53 (#73275)
* V1.1.2:  17-Mar-2017  : Remove unused macros ETH_SCHANNEL17, ETH_TIC_IRQ_MASK
*                         ETH_GTO_RANGE, ETH_UNINITIALIZED, ETH_INITIALIZED
* V1.1.3:  16-May-2017  : Updated source code based on FuSa review
* V1.1.4:  16-May-2017  : Remove ETH_ECSR_MASK macro
*                         Add new macros ETH_MDP_BIT_MASK and ETH_MDPIP_BIT_MASK
* V1.1.5:  12-Sep-2017  : Add new ETH_OCTET_MAX_VAL macro
* V1.1.6:  27-Nov-2017  : Remove unused macro definitions
* V1.1.7:  15-Dec-2017  : Update name and add new general macros
* V1.1.8:  15-Dec-2017  : Add masks for CSR register and descriptor type
* V1.1.9:  26-Dec-2017  : Add macros for statistic Packet counters
* V1.1.10: 22-Mar-2018  : Add macros for EIS, EIC, EIL and TIS
* V1.1.11: 21-Aug-2018  : Add macros for TIS, TCCR, RIS2 and EIS registers
*                         Change name of macro ETH_EIS_ERROR_MASK to
*                         ETH_EIS_QEF_MASK
*******************************************************************************/
#ifndef ETH_PBTYPES_H
#define ETH_PBTYPES_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Platform_Types.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_PBTYPES_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_PBTYPES_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_PBTYPES_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* File version information */
#define ETH_PBTYPES_SW_MAJOR_VERSION  ETH_SW_MAJOR_VERSION
#define ETH_PBTYPES_SW_MINOR_VERSION  ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/* General defines */
#define ETH_DBTOC_VALUE \
  ((ETH_VENDOR_ID_VALUE << 22) | \
  (ETH_MODULE_ID_VALUE << 14) | \
  (ETH_SW_MAJOR_VERSION_VALUE << 8) | \
  (ETH_SW_MINOR_VERSION_VALUE << 3))

/* ETH register mask */
#define ETH_REG_MASK                   (uint32)0xFFFFFFFFUL
/* ETH GTO0 register mask */
#define ETH_GTO0_REG_MASK              (uint32)0x3FFFFFFFUL
/* ETH GTO2 register mask */
#define ETH_GTO2_REG_MASK              (uint32)0x0000FFFFUL
/* ETH SFM1 register mask */
#define ETH_SFM1_REG_MASK              (uint32)0x0000FFFFUL
#define ETH_LOW_BYTE_32                0x0000FFFFUL

#define ETH_16_HIGH_BYTE_64            (uint64)0x0000FFFF00000000U
#define ETH_LOW_BYTE_64                (uint64)0x00000000FFFFFFFFU

#define ETH_OCTET_MAX_VAL              (uint8)(255U)
#define ETH_UNIT32_MAX_VAL             (0xFFFFFFFFU)

/* Flag not set */
#define ETH_FALSE                      (boolean)FALSE
/* Flag set */
#define ETH_TRUE                       (boolean)TRUE

/* General macros to be used in the programming */
#define ETH_ZERO                       0U
#define ETH_ONE                        1U
#define ETH_TWO                        2U
#define ETH_THREE                      3U
#define ETH_FOUR                       4U
#define ETH_FIVE                       5U
#define ETH_EIGHT                      8U
#define ETH_TWELVE                     12U
#define ETH_THIRTEEN                   13U
#define ETH_SIXTEEN                    16U
#define ETH_TWENTYFOUR                 24U
#define ETH_THIRTYTWO                  32U

#define ETH_ZERO_8                     (uint8)(0U)
#define ETH_ONE_8                      (uint8)(1U)
#define ETH_TWO_8                      (uint8)(2U)
#define ETH_THREE_8                    (uint8)(3U)
#define ETH_FOUR_8                     (uint8)(4U)
#define ETH_FIVE_8                     (uint8)(5U)
#define ETH_SIX_8                      (uint8)(6U)
#define ETH_SEVEN_8                    (uint8)(7U)
#define ETH_EIGHT_8                    (uint8)(8U)
#define ETH_NINE_8                     (uint8)(9U)
#define ETH_ELEVEN_8                   (uint8)(11U)
#define ETH_TWELVE_8                   (uint8)(12U)
#define ETH_THIRTEEN_8                 (uint8)(13U)
#define ETH_FOURTEEN_8                 (uint8)(14U)
#define ETH_FIFTEEN_8                  (uint8)(15U)
#define ETH_SIXTEEN_8                  (uint8)(16U)
#define ETH_TWENTY_8                   (uint8)(20U)
#define ETH_THIRTYTWO_8                (uint8)(32U)

#define ETH_ZERO_16                    (uint16)(0U)
#define ETH_ONE_16                     (uint16)(1U)
#define ETH_TWO_16                     (uint16)(2U)
#define ETH_SEVEN_16                   (uint16)(7U)
#define ETH_EIGHT_16                   (uint16)(8U)
#define ETH_TWELVE_16                  (uint16)(12U)
#define ETH_THIRTEEN_16                (uint16)(13U)
#define ETH_FOURTEEN_16                (uint16)(14U)

#define ETH_ZERO_32                    (uint32)0x00000000U
#define ETH_ONE_32                     (uint32)0x00000001U
#define ETH_TWO_32                     (uint32)0x00000002U
#define ETH_THREE_32                   (uint32)0x00000003U
#define ETH_FOUR_32                    (uint32)0x00000004U
#define ETH_SIX_32                     (uint32)0x00000006U
#define ETH_SEVEN_32                   (uint32)0x00000007U
#define ETH_EIGHT_32                   (uint32)0x00000008U
#define ETH_TWENTYEIGHT_32             (uint32)0x0000001CU

#define ETH_BIT0_SET_32                (uint32)0x00000001U
#define ETH_BIT1_SET_32                (uint32)0x00000002U
#define ETH_BIT2_SET_32                (uint32)0x00000004U
#define ETH_BIT3_SET_32                (uint32)0x00000008U
#define ETH_BIT4_SET_32                (uint32)0x00000010U
#define ETH_BIT5_SET_32                (uint32)0x00000020U
#define ETH_BIT6_SET_32                (uint32)0x00000040U
#define ETH_BIT7_SET_32                (uint32)0x00000080U
#define ETH_BIT8_SET_32                (uint32)0x00000100U
#define ETH_BIT9_SET_32                (uint32)0x00000200U
#define ETH_BIT10_SET_32               (uint32)0x00000400U
#define ETH_BIT11_SET_32               (uint32)0x00000800U
#define ETH_BIT12_SET_32               (uint32)0x00001000U
#define ETH_BIT13_SET_32               (uint32)0x00002000U
#define ETH_BIT14_SET_32               (uint32)0x00004000U
#define ETH_BIT15_SET_32               (uint32)0x00008000U
#define ETH_BIT16_SET_32               (uint32)0x00010000U
#define ETH_BIT17_SET_32               (uint32)0x00020000U
#define ETH_BIT18_SET_32               (uint32)0x00040000U
#define ETH_BIT19_SET_32               (uint32)0x00080000U
#define ETH_BIT31_SET_32               (uint32)0x80000000U

#define ETH_BIT15_SET_16               (uint16)0x8000U
/*CBS maximum (65535) */
#define ETH_MAX_CIV                    (uint32)0x0000FFFFU
/*CBS minimum ( -65536) */
#define ETH_MIN_CDV                    (uint32)0xFFFF0000U
/*CBS maximum ( -1) */
#define ETH_MAX_CDV                    (uint32)0xFFFFFFFFU

/*******************************************************************************
**  Constant: ETH_MACRO_NUM
**
**  Number of macros in the device.
*******************************************************************************/
#define ETH_MACRO_NUM                  (uint8)1U
#define ETH_MII_REG_MAX_INDEX          (uint8)31U

/*******************************************************************************
  Rx Channel number
*******************************************************************************/
/* Best effort channel */
#define ETH_BECHANNEL          (uint8)0
/* Network control channel */
#define ETH_NCCHANNEL          (uint8)1
/* Stream channel 1 */
#define ETH_SCHANNEL1          (uint8)2
/* Stream channel 2 */
#define ETH_SCHANNEL2          (uint8)3
/* Stream channel 3 */
#define ETH_SCHANNEL3          (uint8)4
/* Stream channel 4 */
#define ETH_SCHANNEL4          (uint8)5
/* Stream channel 5 */
#define ETH_SCHANNEL5          (uint8)6
/* Stream channel 6 */
#define ETH_SCHANNEL6          (uint8)7
/* Stream channel 7 */
#define ETH_SCHANNEL7          (uint8)8
/* Stream channel 8 */
#define ETH_SCHANNEL8          (uint8)9
/* Stream channel 9 */
#define ETH_SCHANNEL9          (uint8)10
/* Stream channel 10 */
#define ETH_SCHANNEL10         (uint8)11
/* Stream channel 11 */
#define ETH_SCHANNEL11         (uint8)12
/* Stream channel 12 */
#define ETH_SCHANNEL12         (uint8)13
/* Stream channel 13 */
#define ETH_SCHANNEL13         (uint8)14
/* Stream channel 14 */
#define ETH_SCHANNEL14         (uint8)15
/* Stream channel 15 */
#define ETH_SCHANNEL15         (uint8)16
/* Stream channel 16 */
#define ETH_SCHANNEL16         (uint8)17

/*******************************************************************************
**  number of Tx/Rx queues
*******************************************************************************/
/* Tx queue number */
#define ETH_TXQ_MAXNUM         (uint8)4
/* Maximum Rx queue number */
#define ETH_RXQ_MAXNUM         (uint8)18
/* Link descriptor number */
#define ETH_EXTRA_FOR_LINKDESC (uint8)1
/* EOS descriptor */
#define ETH_EXTRA_FOR_EOSDESC  (uint8)1
/* Queue size zero */
#define ETH_QUEUE_SIZE_ZERO     0U
/* Queue NOT found */
#define ETH_QUEUE_NOT_FOUND     0xFFU
/*******************************************************************************
**  Minimum Buffer Size
*******************************************************************************/
#define ETH_MIN_BUFFER_SIZE      (uint16)64
/*******************************************************************************
**  Ethernet frame header size
*******************************************************************************/
#define ETH_HEADER_SIZE          (uint16)14U
/*******************************************************************************
** Mask for the Status Register
*******************************************************************************/
/*AVB DMAC */
#define ETH_RXQ_INT_MASK       0x8003FFFFUL
#define ETH_RX_INT_MASK        0x0003FFFFUL
#define ETH_DPL_INT_MASK       0x0000FFFCUL
#define ETH_DP_INT_MASK        0x0000FFFEUL
#define ETH_EI_INT_MASK        0x000007FFUL

#define ETH_TX_IRQ_MASK        0x0000000FUL

/* Mask for Error Interrupt Status Register */
#define ETH_EIS_QEF_MASK       0x00000004UL
/* Mask for Time Stamp FIFO Full Error Interrupt Status */
#define ETH_EIS_TFFF_MASK      0x00000100UL
/* Mask for all Interrupt Status bits of register EIS */
#define ETH_EIS_INT_MASK       0x000107FFUL
/* Mask for bit 8,9,10,11 of TIS register */
#define ETH_TIS_MNG_MASK       0x00000F00UL
/* Mask for all Interrupt Status bits of TIS register */
#define ETH_TIS_INT_MASK       0x0000F0F0FUL
/* Mask for GIS register */
#define ETH_GIS_MNG_MASK       0xFFFF03FFUL
/* Mask for bit DTS of CSR register */
#define ETH_CSR_DTS_MASK       0x00000100UL
/* Mask for bits OPS of CSR register */
#define ETH_CSR_OPS_MASK       0x0000000FUL
/* Mask for descriptor type */
#define ETH_DESC_DT_MASK       0xF0000000UL
/* Mask for bits ET of ESR register */
#define ETH_ESR_ET_MASK        0x00000F00UL
/* Mask for Transmit Start Request, bits TSRQ0 -> TSRQ3 of TCCR register */
#define ETH_TCCR_TSRQ_MASK     0x0000000FUL
/* Mask for bit TFR of TCCR register */
#define ETH_TCCR_TFR_MASK      0x00000200UL
/* Mask for bit TFEN of TCCR register */
#define ETH_TCCR_TFEN_MASK     0x00000100UL
/* Mask for bit RFFF of RIS2 register */
#define ETH_RIS2_RFFF_MASK     0x80000000UL
/*******************************************************************************
** Register Range
*******************************************************************************/
/*Range of Unread frame decrement value for reception queue */
#define ETH_UFCD_DV_RANGE      (uint32)0x0000003FU
/*Range of unread frames in reception queue number */
#define ETH_UFCV_CV_RANGE      (uint32)0x3FU
/*******************************************************************************
** Bit of Register
*******************************************************************************/
/* Receive Warning Flag Bit (RIS1.RFWF) */
#define ETH_RIS1_RFWF_BIT      31U
/* Receive FIFO Full Interrupt Status (RIS2.RFFF) */
#define ETH_RIS2_RFFF_BIT      31U
/* Common Receive Interrupt enable (CIE.CRIE) */
#define ETH_CIE_CRIE_BIT       0U
/* Receive FIFO Full Interrupt Enable (RIC2.RFFE) */
#define ETH_RIC2_RFFE_BIT      31U
/* Common Transmit Interrupt Enable (CIE.CTIE) */
#define ETH_CIE_CTIE_BIT       8U
/* Common Line 0 Mode (CIE.CL0M) */
#define ETH_CIE_CL0M_BIT       17U
/* Time Stamp FIFO Full Error Interrupt Status */
#define ETH_EIS_TFFF_BIT       8U
/* Timestamp FIFO Full interrupt Line select */
#define ETH_EIL_TFFL_BIT       8U
/* Transmit Descriptor Processed Flag 0 */
#define ETH_TIS_TDPF0_BIT      16U
/* Transmit Descriptor Processed Flag 1 */
#define ETH_TIS_TDPF1_BIT      17U
/* Transmit Descriptor Processed Flag 2 */
#define ETH_TIS_TDPF2_BIT      18U
/* Transmit Descriptor Processed Flag 3 */
#define ETH_TIS_TDPF3_BIT      19U
/*******************************************************************************
** Mask
*******************************************************************************/
#define ETH_MDP_BIT_MASK       0x00000002UL
#define ETH_MDPIP_BIT_MASK     0x00000002UL
#define ETH_HIGH_BYTE_16       0xFF00U
#define ETH_LOW_BYTE_16        0x00FFU
/*******************************************************************************
** statistic Packet macros
*******************************************************************************/
#define ETH_PKTS_64_OCTETS              (uint16)(64U)
#define ETH_PKTS_65_OCTETS              (uint16)(65U)
#define ETH_PKTS_127_OCTETS             (uint16)(127U)
#define ETH_PKTS_128_OCTETS             (uint16)(128U)
#define ETH_PKTS_255_OCTETS             (uint16)(255U)
#define ETH_PKTS_256_OCTETS             (uint16)(256U)
#define ETH_PKTS_511_OCTETS             (uint16)(511U)
#define ETH_PKTS_512_OCTETS             (uint16)(512U)
#define ETH_PKTS_1023_OCTETS            (uint16)(1023U)
#define ETH_PKTS_1024_OCTETS            (uint16)(1024U)
#define ETH_PKTS_1518_OCTETS            (uint16)(1518U)
#define ETH_FCS_LENGTH                  (uint16)(4U)
/*******************************************************************************
**               SchM Critical Section Protection Macros                      **
*******************************************************************************/
#define ETH_ENTER_CRITICAL_SECTION(Exclusive_Area) \
  SchM_Enter_Eth_##Exclusive_Area()
#define ETH_EXIT_CRITICAL_SECTION(Exclusive_Area) \
  SchM_Exit_Eth_##Exclusive_Area()
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**             Extern declarations for Global Arrays Of Structures            **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* ETH_PBTYPES_H  */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
