/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = App_ETH_Specific_Sample.h                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains macros/function prototypes/variables required for       */
/* source application file.                                                   */
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
********************************************************************************
* V1.0.0:  8-Dec-2015  : Initial Version
* V1.0.1:  5-Aug-2016  : Remove Renesas specific number 59
*                        Unify version number
* V1.1.0:  6-Apr-2017  : Add extern function TMU_CH01_ISR
*                        Remove define PFC_BASE_ADDR
* V1.1.1:  18-Jul-2017 : Extern Gpt_TMU1_Start, Gpt_TMU1_Stop and TMU_CH01_ISR
*                        for common sample usage
* V1.1.2:  04-Apr-2018 : Add #define ETH_DESC_BASE_ADDR and ETH_DESC_SIZE
* V1.1.3:  26-Apr-2018 : Update file name
* V1.1.4:  22-Aug-2018 : Add function Wdg_Init
* V1.1.5:  21-Sep-2018 : Add function Eth_PhyExtReset, Gpt_TMU0_Start,
*                        Gpt_TMU0_Stop and EthTest_WaitMs
* V1.2.0:  23-Mar-2022 : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef APP_ETH_SPECIFIC_SAMPLE_H
#define APP_ETH_SPECIFIC_SAMPLE_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Det.h"
#include "ether_api.h"
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/*Address of register Relate to PFC, GPIO module*/

#define PFC_BASE_ADDR        (uint32)0xE6060000UL
#define PMMR_ETH_APP         *((volatile uint32 *)(PFC_BASE_ADDR + 0x00))
#define GPSR2                *((volatile uint32 *)(PFC_BASE_ADDR + 0x108))
#define IPSR0                *((volatile uint32 *)(PFC_BASE_ADDR + 0x200))
#define PFC_DRVCTRL2         *((volatile uint32 *)(PFC_BASE_ADDR + 0x308))
#define PFC_DRVCTRL3         *((volatile uint32 *)(PFC_BASE_ADDR + 0x30C))

#define GPSR2_ADDR           (PFC_BASE_ADDR + 0x108)
#define IPSR0_ADDR           (PFC_BASE_ADDR + 0x200)

#define OUTDT2      *((volatile uint32 *)((uint32)0xE6052008))

/* Bit MASK for ETH */
#define MASK_AVB             (uint32)0xFFFF0FF0UL
/* Define mask register (GP2_10) */
#define MASK_GP_2_10    (uint32)(0x01 << 10)

#define ETH_MII_KSZ9031_MOD_DATA_NO_POST_INC  0x4000
#define PHY_MDD_ACCES_CTRL_REG  0x0D
#define PHY_MDD_ACCES_DATA_REG  0x0E

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

/* Controller Port(s) Initialization */
extern void Port_Init(void);
/* System Initialization */
extern void Mcu_Init(void);

extern void Eth_PhyExtReset(void);

#if (ETH_CTRL_ENABLE_MII == STD_ON)
extern int Eth_ClockPadSkewSetup(int ether_cio_ch);
extern int Eth_PhyExtWrite(uint16 devAddr, uint16 regNum, uint16 mode, uint16 val, int ether_cio_ch);
#endif
#endif /* APP_ETH_DEVICE_SAMPLE_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
