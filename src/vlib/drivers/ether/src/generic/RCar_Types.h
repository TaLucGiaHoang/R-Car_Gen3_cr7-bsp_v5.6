/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* File name    = RCar_Types.h                                                */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
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
/*              Devices:        R-Car V3M                                     */
/*                              R-Car H3/M3                                   */
/*                              R-Car M3N                                     */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V1.0.0:  30-Oct-2015  : Initial version
 *
 * V1.0.1:  30-Sep-2016  : Add definition for number in macros:
 *                         GICD_WRITE_TO_REGISTER, GICD_READ_FROM_REGISTER.
 * V1.1.1   11-Nov-2016    Following changes are made:
 *                         1. Add definition macro: GICD_WRITE_TO_ITARGETSR for
 *                            GIC access implementation.
 *                         2. Macros GIC_CHECK_REGISTER and GIC_SET_REGISTER are
 *                            removed.
 * V1.2.0   1-Dec-2016     Following changes are made:
 *                         Added FuSa related Macros: RFSO10_FSO_CTL_ADDR,
 *                         MFIERRTGTR1_ADDR, MFIERRCTLR1_ADDR, SAMSTPCR9_ADDR,
 *                         MFIERRTGTR1, MFIERRCTLR1, SAMSTPCR9, FSO_CFEO_0_BIT,
 *                         FSO_CFEO_1_BIT, FSO_CFEO_0_ASSERT, FSO_CFEO_1_ASSERT,
 *                         MFIS_CAN_ECC_BIT_CFG, SAMSTPCR9_CAN_MASK,
 *                         RFSO10_FSO_CTL.
 * V1.2.1   06-Feb-2017    Following changes are made:
 *                         1. FuSa related Macros are removed.
 *                            Macros device version H3_WS1_0, H3_WS1_1, H3_ES2_0
 *                            are added.
 *                         2. Macros device version H3_ES2_0 is changed to
 *                            H3_WS2_0
 *                            Macro device version for RCar V3M V3M_WS1_0 is
 *                            added.
 * V1.2.2   24-Jan-2017    Following changes are made:
 *                         Macros device name RCAR_H3, RCAR_V3M are added.
 *
 * V1.2.3   03-Nov-2017    Macros device name RCAR_V3H, RCAR_V3M are added.
 *                         Macros device version V3M_WS1_0, V3M_ES2_0 are added.
 * V1.2.4   04-Jan-2018    Update device name
 * V1.2.5   24-Jan-2018    Macros device name RCAR_M3 is added.
 *                         Macros device version M3_WS1_1 is added.
 * V1.2.6   24-Apr-2018    Macros device name RCAR_M3N is added.
 */

#ifndef RCAR_TYPES_H
#define RCAR_TYPES_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/*
 * File version information
 */
#define RCAR_TYPES_SW_MAJOR_VERSION  1
#define RCAR_TYPES_SW_MINOR_VERSION  0
#define RCAR_TYPES_SW_PATCH_VERSION  0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
/*******************************************************************************
**                      Macro                                                 **
*******************************************************************************/
/* RCar device name definition */
#define RCAR_H3                  0x00U
#define RCAR_V3M                 0x02U
#define RCAR_V3H                 0x03U
#define RCAR_M3                  0x04U
#define RCAR_M3N                 0x05U

/* RCar device version definition */
#define H3_WS1_0                 0x00U
#define H3_WS1_1                 0x01U
#define H3_WS2_0                 0x10U

#define V3M_WS1_0                0x11U
#define V3M_WS2_0                0x12U

#define M3_WS1_1                 0x21U

#define GIC_MASK_VALUE           0xFFFFFFFCUL
#define GIC_ENABLE_INDICATOR     (uint32)0x1
#define GIC_SPI_OFFSET           0x20U
#define GIC_SPI_MOD_32           0x1FU
#define GIC_SPI_MOD_4            0x03U
#define GIC_SPI_DIV_8            0x03U
#define GIC_SPI_MUL              0x03U

/* GIC400 Distributor Base address */
#define GICD_BASE_ADDR           0xF1110000UL
/* GIC400 CPU Interface Base address*/
#define GICC_BASE_ADDR           0xF1120000UL
/* GIC400 Distributor Registers Offset */
#define GICD_OFFSET_ISENABLER    0xF1110100UL
#define GICD_OFFSET_ICENABLER    0xF1110180UL
#define GICD_OFFSET_ISPENDR      0xF1110200UL
#define GICD_OFFSET_ICPENDR      0xF1110280UL
#define GICD_OFFSET_ITARGETSR    0xF1110800UL

/*******************************************************************************
** Macro Name            : GICD_WRITE_TO_REGISTER
**
** Description           : This Macro performs write to GIC Distributor
**                         following registers:
**                         GICD_ISENABLER
**                         GICD_ICENABLER
**                         GICD_ISPENDR
**                         GICD_ICPENDR
**
** Input Parameters      : GIC_Index, REG_ADDR
**                         GIC_Index : Shared Peripheral Interrupt index in GIC.
**                         REG_ADDR  : Offset address of the targeted register.
*******************************************************************************/
#define GICD_WRITE_TO_REGISTER(GIC_Index, REG_ADDR) \
  *((uint32*)(REG_ADDR + ((((uint32)GIC_Index + GIC_SPI_OFFSET) \
  >> GIC_SPI_DIV_8) & GIC_MASK_VALUE))) |= \
  (GIC_ENABLE_INDICATOR << ((GIC_Index + GIC_SPI_OFFSET) & GIC_SPI_MOD_32))

/*******************************************************************************
** Macro Name            : GICD_READ_FROM_REGISTER
**
** Description           : This Macro performs read from GIC Distributor
**                         following registers:
**                         GICD_ISENABLER
**                         GICD_ICENABLER
**                         GICD_ISPENDR
**                         GICD_ICPENDR
**
** Input Parameters      : GIC_Index, REG_ADDR
**                         GIC_Index : Shared Peripheral Interrupt index in GIC.
**                         REG_ADDR  : Offset address of the targeted register.
*******************************************************************************/
#define GICD_READ_FROM_REGISTER(GIC_Index, REG_ADDR) \
  *((uint32*)(REG_ADDR + ((((uint32)GIC_Index + GIC_SPI_OFFSET) \
  >> GIC_SPI_DIV_8) & GIC_MASK_VALUE))) &= \
  (GIC_ENABLE_INDICATOR << ((GIC_Index + GIC_SPI_OFFSET) & GIC_SPI_MOD_32))

/*******************************************************************************
** Macro Name            : GICD_WRITE_TO_ITARGETSR
**
** Description           : This Macro performs write to GIC Distributor
**                         following registers:
**                         GICD_ITARGETSR
**
** Input Parameters      : GIC_Index, CPU_Index
**                         GIC_Index : Shared Peripheral Interrupt index in GIC.
**                         CPU_Index : CPU interface index to which the
**                                     Distributor forwards the interrupt.
*******************************************************************************/
#define GICD_WRITE_TO_ITARGETSR(GIC_Index, CPU_Index) \
  *((uint32*)(GICD_OFFSET_ITARGETSR + (((uint32)GIC_Index + \
  GIC_SPI_OFFSET) & GIC_MASK_VALUE))) |= (GIC_ENABLE_INDICATOR << \
  (CPU_Index + ((((uint32)GIC_Index + GIC_SPI_OFFSET) & GIC_SPI_MOD_4) \
  << GIC_SPI_MUL)))

#endif
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
