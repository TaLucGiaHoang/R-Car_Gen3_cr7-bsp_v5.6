/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = App_ETH_Specific_Sample.c                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2016-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This application file contains execution sequences to demonstrate the usage*/
/* of ETH Driver APIs.                                                        */
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
/*
 * 1.0.0      25-Apr-2016      Initial Version
 * 1.0.1      05-Aug-2016      Remove Renesas specific number 59
 *                             Correct typo mistakes
 * 1.1.0      06-Apr-2017      Add TMU_CH01_ISR() function
 *                             Add include "scif.h"
 * 1.1.1      18-Jul-2017      Create Gpt_TMU1_Start, Gpt_TMU1_Stop and
 *                             TMU_CH01_ISR to call Eth_MainFunction
 *                             periodically
 * 1.1.2      11-Oct-2017      Enable Timers group 0
 * 1.2.0      26-Oct-2017      Added Clock Pad Skew Setting function
 * 1.2.1      25-Dec-2017      Remove unused port setting
 * 1.2.2      04-Apr-2018      Update descriptor RAM address
 * 1.2.3      26-Apr-2018      Update file name
 * 1.2.4      22-Aug-2018      Add function Wdg_Init
 * 1.2.5:     21-Sep-2018      Add function Eth_PhyExtReset, Gpt_TMU0_Start,
 *                             Gpt_TMU0_Stop and EthTest_WaitMs
 * 1.2.6:     09-Apr-2020      Include file "SchM_Eth.h" for the prototype
 *                             declaration of Eth_MainFunction()
 * 1.3.0:     23-Mar-2022      Modify for R-CarH3 CR7 Safe-Rendering
 */

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "app_eth_specific_sample.h"
#include "scif.h"
#include "ether_api.h"
#include "r_cio_ether_public.h"
#include "r_cio_api.h"
#include "SchM_Eth.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_cpg_api.h"
#include "r_print_api.h"
/*******************************************************************************
*                       Global Symbols                                        **
*******************************************************************************/
/***********************************************************************
**                     EXTERNAL FUNCTION PROTOTYPES
***********************************************************************/
/******************************************************************************
*                       Function Definitions                                  **
******************************************************************************/
/*******************************************************************************
*                       Global Variables                                      **
*******************************************************************************/
/******************************************************************************
*                       Port Initialization                                  **
******************************************************************************/
/* Perform the ETH Port Pin Configuration */
void Port_Init(void)
{
  volatile uint32 *reg;
  uint32 val;

  /* Set IPSR0 register to configure GP2_09, GP2_10, GP2_12 as AVB function */
  /* Set register PMMR_ETH_APP before use register IPSR0 */
  PMMR_ETH_APP = ~(IPSR0 & MASK_AVB);
  /* Set register IPSR0[15:12] and IPSR0[3:0] to 0 */
  reg = (volatile uint32 *)IPSR0_ADDR;
  *reg = (IPSR0 & MASK_AVB);

  /* Set the GPSR2 register to enable Peripheral function for AVB */
  /* Set register PMMR_ETH_APP before use register GPSR2 */
  PMMR_ETH_APP = ~(GPSR2 | ((0x01 << 9) | (0x01 << 12)));
  /*Configuration pin GP2_09 to GP2_14 as Peripheral */
  reg = (volatile uint32 *)GPSR2_ADDR;
  *reg = (GPSR2 | ((0x01 << 9) | (0x01 << 12)));

  /* Set the PFC_DRVCTRL2 to appropriate value */
  val = PFC_DRVCTRL2;
  val &= ~0x00000777;
  val |=  0x00000333;
  /* Set register PMMR_ETH_APP before use register PFC_DRVCTRL2 */
  PMMR_ETH_APP = ~val;
  /* Set register PFC_DRVCTRL2 */
  PFC_DRVCTRL2 = val;

  /* Set the PFC_DRVCTRL3 to appropriate value */
  val = PFC_DRVCTRL3;
  val &= ~0x77700000;
  val |=  0x33300000;
  /* Set register PMMR_ETH_APP before use register PFC_DRVCTRL3 */
  PMMR_ETH_APP = ~val;
  /* Set register PFC_DRVCTRL3 */
  PFC_DRVCTRL3 = val;

  /* Set the GPSR2 to configure GP2_10 as GPIO */
  val = GPSR2;
  val &= ~(0x01 << 10);
  /* Set register PMMR_ETH_APP before use register GPSR2 */
  PMMR_ETH_APP = ~val;
  /* Set register GPSR2 */
  GPSR2 = val;
}

/******************************************************************************
                              System Initialization
******************************************************************************/
/* Perform the System initialization */
void Mcu_Init(void)
{
  /* Enable supply clock for STBE */
  R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_1, R_CPG_BIT5, false);

  return;
}

/******************************************************************************
                              Reset ETH external PHY
******************************************************************************/
/* Reset ETH external PHY */
void Eth_PhyExtReset(void)
{
  uint32 LulRegVal;

  /* Set the GPSR2 to configure GP2_10 as GPIO */
  LulRegVal = GPSR2;
  /* Clear bit 10 of GPSR2 to configure GP2_10 as GPIO */
  LulRegVal &= ~MASK_GP_2_10;
  /* Set register PMMR_ETH_APP before use register GPSR2 */
  PMMR_ETH_APP = ~LulRegVal;
  /* Set register GPSR2 */
  GPSR2 = LulRegVal;

  /* Set GP2_10 to low level to trigger phy reset via pin RESET_N */
  LulRegVal = OUTDT2;
  LulRegVal &= ~MASK_GP_2_10;
  OUTDT2 = LulRegVal;

  /* Delay 10ms for stabling supply voltages to de-assertion of reset */
  R_OSAL_ThreadSleepForTimePeriod(11);

  /* Set GP2_10 to high level to stop reset processing */
  LulRegVal = OUTDT2;
  LulRegVal |= MASK_GP_2_10;
  OUTDT2 = LulRegVal;
}

/******************************************************************************
                              RGMII clock pad skew setting
******************************************************************************/
#if (ETH_CTRL_ENABLE_MII == STD_ON)
int Eth_ClockPadSkewSetup(int ether_cio_ch)
{
  /* Setting (900+600ps delay) */
  return Eth_PhyExtWrite(0x0002, 0x0008,
      ETH_MII_KSZ9031_MOD_DATA_NO_POST_INC, ((0x0f<<5) | 0x19), ether_cio_ch);
}
int Eth_PhyExtWrite(uint16 devAddr, uint16 regNum, uint16 mode, uint16 val, int ether_cio_ch)
{
  r_cio_ether_mii r_cio_etherMii;
  ssize_t ret;

  /*select register addr for mmd*/
  /* Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, devAddr); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_CTRL_REG;
  r_cio_etherMii.RegVal  = devAddr;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error(%d)\r\n", ret);
    return -1;
  }

  /* Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, &LusEthRegValue); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_CTRL_REG;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_READ_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_READ_MII) error(%d)\r\n", ret);
    return -1;
  }
  if (r_cio_etherMii.RegVal != devAddr)
  {
    R_PRINT_Log("[ETH Sample App] Expected:0x%04X, Current:0x%04X)\r\n", devAddr, r_cio_etherMii.RegVal);
    return -1;
  }

  /*select register for mmd*/
  /* Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, regNum); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_DATA_REG;
  r_cio_etherMii.RegVal  = regNum;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error(%d)\r\n", ret);
    return -1;
  }

  /* Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, &LusEthRegValue); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_DATA_REG;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_READ_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_READ_MII) error(%d)\r\n", ret);
    return -1;
  }
  if (r_cio_etherMii.RegVal != regNum)
  {
    R_PRINT_Log("[ETH Sample App] Expected:0x%04X, Current:0x%04X)\r\n", regNum, r_cio_etherMii.RegVal);
    return -1;
  }

  /*setup mode*/
  /* Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, (mode | devAddr)); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_CTRL_REG;
  r_cio_etherMii.RegVal  = (mode | devAddr);
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error(%d)\r\n", ret);
    return -1;
  }

  /* Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, &LusEthRegValue); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_CTRL_REG;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_READ_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_READ_MII) error(%d)\r\n", ret);
    return -1;
  }
  if (r_cio_etherMii.RegVal != (mode | devAddr))
  {
    R_PRINT_Log("[ETH Sample App] Expected:0x%04X, Current:0x%04X)\r\n", (mode | devAddr), r_cio_etherMii.RegVal);
    return -1;
  }

  /*write the value*/
  /* Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, val); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_DATA_REG;
  r_cio_etherMii.RegVal  = val;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_WRITE_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_WRITE_MII) error(%d)\r\n", ret);
    return -1;
  }

  /* Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, &LusEthRegValue); */
  r_cio_etherMii.CtrlIdx = EthConf_EthCtrlConfig_EthCtrlConfig0;
  r_cio_etherMii.TrcvIdx = 0x00;
  r_cio_etherMii.RegIdx  = PHY_MDD_ACCES_DATA_REG;
  ret = R_CIO_IoCtl(ether_cio_ch, (void *)R_CIO_ETHER_IOC_READ_MII, (void *)&r_cio_etherMii, 0);
  if (0 != ret)
  {
    R_PRINT_Log("[ETH Sample App] R_CIO_IoCtl(R_CIO_ETHER_IOC_READ_MII) error(%d)\r\n", ret);
    return -1;
  }
  if (r_cio_etherMii.RegVal != val)
  {
    R_PRINT_Log("[ETH Sample App] Expected:0x%04X, Current:0x%04X)\r\n", val, r_cio_etherMii.RegVal);
    return -1;
  }

  return 0;
}
#endif

/*******************************************************************************
                          End of the file
*******************************************************************************/
