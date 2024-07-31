/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* File name    = RCar_FuSa.h                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017 - 2018 Renesas Electronics Corporation                   */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision for platform dependent types                                     */
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
 * 1.0.0      09-Jan-2017      Initial Version
 * 1.0.1      10-Mar-2017      Definition FSO_KEYCODE is added
 * 1.0.2      31-May-2017      CFEO_1 bit is written after CFEO_0 bit is set
 * 1.0.3      19-Aug-2017      Add macro MFIERRTGTR8_ADDR and MFIERRCTLR8_ADDR
 * 1.0.4      13-Dec-2017      Define STATIC keyword
 * 1.0.5      26-Mar-2018      Update memory mapping for RCar_RfsoFaultControl()
 *                             function
 * 1.0.6      05-Jun-2018      Remove STATIC keyword definition
 * 1.0.7      10-Aug-2018      Remove macro MFIS_DISABLE, MFIS_EXTERNAL_PIN,
 *                             CAN_MEC_REG_CFG, MFIS_CAN_EDC_BIT_CFG,
 *                             MEC710CTL, MEC710CTL_ADDR, MFIERRTGTR1,
 *                             MFIERRCTLR1, MFIERRTGTR1_ADDR, MFIERRCTLR1_ADDR.
 *                             Above macro value define for CAN module EDC
 *                             HW Safety mechanism but CAN HW driver no longer
 *                             support EDC from HW-SAN v0.79.
 */

#ifndef RCAR_FUNC_SAFETY_H
#define RCAR_FUNC_SAFETY_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* File version information */
#define RCAR_FUNC_SAFETY_SW_MAJOR_VERSION  1
#define RCAR_FUNC_SAFETY_SW_MINOR_VERSION  0
#define RCAR_FUNC_SAFETY_SW_PATCH_VERSION  1

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Macro                                                 **
*******************************************************************************/

/* MFIS Error Target8 Register address */
#define MFIERRTGTR8_ADDR         0xE6260274UL
/* MFIS Error Control8 Register address */
#define MFIERRCTLR8_ADDR         0xE626026CUL
/* Safety Module Stop Control Register address */
#define SAMSTPCR2_ADDR           0xE6150C28UL
#define SAMSTPCR9_ADDR           0xE6150C44UL

/* MFIS Error Target Register 7 address */
#define MFIERRTGTR7_ADDR         0xE6260268UL
/* MFIS Error Control Register 7 address */
#define MFIERRCTLR7_ADDR         0xE6260260UL
/* Safety Module Stop Control Register */
#define SAMSTPCR2                *((volatile uint32 *)(SAMSTPCR2_ADDR))
#define SAMSTPCR9                *((volatile uint32 *)(SAMSTPCR9_ADDR))

/* SAMSTPCR2[13] bit mask */
#define SAMSTPCR2_MFIS_MASK      (uint32)0x00002000ul
/* SAMSTPCR9[14] bit mask */
#define SAMSTPCR9_CAN_MASK       (uint32)0x00004000ul

#define RFSO_FAULT_RESET      0u
#define RFSO_FAULT_TRANSIENT  1u
#define RFSO_FAULT_PERMANENT  2u
#define RFSO_MAX_CHANNEL      0x0Au
#define RFSO_0                0x00U
#define RFSO_1                0x01U
#define RFSO_2                0x02U
#define RFSO_3                0x03U
#define RFSO_4                0x04U
#define RFSO_5                0x05U
#define RFSO_6                0x06U
#define RFSO_7                0x07U
#define RFSO_8                0x08U
#define RFSO_9                0x09U
#define RFSO_10               0x0AU
#define RFSO_DISABLE          0xFFU
#define FSO_CTL_CFEO_1        0x00000004ul
#define FSO_CTL_CFEO_0        0x00000002ul
#define FSO_KEYCODE           0x77950000ul
#define RFSO_CH_OOR(c)        ((c) <= RFSO_MAX_CHANNEL)
#define RFSO_CH_BASE(c)       (0xFFE80000ul + (0x00001000ul * (c)))
#define FSO_CTL(c)            (RFSO_CH_BASE(c) + 0x00000008ul)

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define FUSA_START_SEC_CODE_FAST
#include "FuSa_MemMap.h"

extern FUNC(void, FUSA_CODE_FAST) RCar_RfsoFaultControl
(
  VAR(uint8, AUTOMATIC) ch,
  VAR(uint8, AUTOMATIC) fault_type
);

#define FUSA_STOP_SEC_CODE_FAST
#include "FuSa_MemMap.h"

#endif
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
