/*
 * Copyright (c) 2017-2020, Renesas Electronics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "r_avb_api.h"
#include "r_tick_api.h"
#include "r_gpio_api.h"
#include "r_cpg_api.h"

/* PFC */
#define PFC_BASE_ADDR      (0xE6060000)
#define PFC_PMMR           ((volatile uint32_t *)(PFC_BASE_ADDR + 0x0000))
#define PFC_GPSR5          ((volatile uint32_t *)(PFC_BASE_ADDR + 0x0114))
#define PFC_IPSR11         ((volatile uint32_t *)(PFC_BASE_ADDR + 0x022C))
#define PFC_MOD_SEL1       ((volatile uint32_t *)(PFC_BASE_ADDR + 0x0504))

#define PFC_GPSR5_RX1_A_MASK    (1 << 5)
#define PFC_GPSR5_TX1_A_MASK    (1 << 6)
#define PFC_IPSR11_RX1_A_MASK   (1 << 12)
#define PFC_IPSR11_TX1_A_MASK   (1 << 16)
#define MOD_SEL1_SEL_SCIF1_MASK (1 << 11)

#define GPIO_BASE_ADDR       (uint32_t)0xE6050000UL

#define CPGWPR               *((volatile uint32_t *)(0xE6150900))
#define PMMR                 *((volatile uint32_t *)(PFC_BASE_ADDR + 0x00))
#define GPSR2                *((volatile uint32_t *)(PFC_BASE_ADDR + 0x108))
#define IPSR0                *((volatile uint32_t *)(PFC_BASE_ADDR + 0x200))
#define IPSR1                *((volatile uint32_t *)(PFC_BASE_ADDR + 0x204))
#define IPSR2                *((volatile uint32_t *)(PFC_BASE_ADDR + 0x208))
#define IPSR3                *((volatile uint32_t *)(PFC_BASE_ADDR + 0x20C))
#define MOD_SEL0             *((volatile uint32_t *)(PFC_BASE_ADDR + 0x500))
#define PFC_DRVCTRL2         *((volatile uint32_t *)(PFC_BASE_ADDR + 0x308))
#define PFC_DRVCTRL3         *((volatile uint32_t *)(PFC_BASE_ADDR + 0x30C))

#define GPSR2_ADDR           (PFC_BASE_ADDR + 0x108)
#define IPSR0_ADDR           (PFC_BASE_ADDR + 0x200)
#define IPSR1_ADDR           (PFC_BASE_ADDR + 0x204)
#define IPSR2_ADDR           (PFC_BASE_ADDR + 0x208)
#define IPSR3_ADDR           (PFC_BASE_ADDR + 0x20C)
#define MOD_SEL0_ADDR        (PFC_BASE_ADDR + 0x500)
#define PFC_DRVCTRL2_ADDR    (PFC_BASE_ADDR + 0x308)
#define PFC_DRVCTRL3_ADDR    (PFC_BASE_ADDR + 0x30C)

#define IOINTSEL2   *((volatile uint32_t *)((uint32_t)0xE6052000))
#define INOUTSEL2   *((volatile uint32_t *)((uint32_t)0xE6052004))
#define OUTDT2      *((volatile uint32_t *)((uint32_t)0xE6052008))
#define POSNEG2     *((volatile uint32_t *)((uint32_t)0xE6052020))
#define OUTDT2      *((volatile uint32_t *)((uint32_t)0xE6052008))


/* Define GPIO register (GP1) */
#define IOINTSEL1           *((volatile uint32_t *)((uint32_t)0xE6051000))
#define INOUTSEL1           *((volatile uint32_t *)((uint32_t)0xE6051004))
#define POSNEG1             *((volatile uint32_t *)((uint32_t)0xE6051020))
#define OUTDT1              *((volatile uint32_t *)((uint32_t)0xE6051008))
#define MASK_GPIO           ((uint32_t)((1 << 21) | (1 << 22) | (1 << 26)))

/* Bit MASK for ETH */
#define MASK_AVB             (uint32_t)0xFFFF0FF0UL
#define MASK_AVB_AVTP_PPS    (uint32_t)0x000F0000UL
#define AVB_AVTP_MATCH_B     (uint32_t)0xF0000000UL
#define AVB_AVTP_CAPTURE_B   (uint32_t)0x00000F00UL
/* Define mask register (GP2_10) */
#define MASK_GP_2_10    (uint32_t)(0x01 << 10)

/******************************************************************************
*                       Port Initialization                                  **
******************************************************************************/
/* Perform the ETH Port Pin Configuration */
void loc_PfcInit(void)
{
    volatile uint32_t *reg;
    uint32_t val;

    /* Set IPSR0 register to configure GP2_09, GP2_10, GP2_12 as AVB function */
    /* Set register PMMR before use register IPSR0 */
    PMMR = ~(IPSR0 & MASK_AVB);
    /* Set register IPSR0[15:12] and IPSR0[3:0] to 0 */
    reg = (volatile uint32_t *)IPSR0_ADDR;
    *reg = (IPSR0 & MASK_AVB);

    /* Set the GPSR2 register to enable Peripheral function for AVB */
    /* Set register PMMR before use register GPSR2 */
    PMMR = ~(GPSR2 | ((0x01 << 9) | (0x01 << 12)));
    /*Configuration pin GP2_09 to GP2_14 as Peripheral */
    reg = (volatile uint32_t *)GPSR2_ADDR;
    *reg = (GPSR2 | ((0x01 << 9) | (0x01 << 12)));

    /* Set the PFC_DRVCTRL2 to appropriate value */
    val = PFC_DRVCTRL2;
    val &= ~0x00000777;
    val |=  0x00000333;
    /* Set register PMMR before use register PFC_DRVCTRL2 */
    PMMR = ~val;
    /* Set register PFC_DRVCTRL2 */
    PFC_DRVCTRL2 = val;

    /* Set the PFC_DRVCTRL3 to appropriate value */
    val = PFC_DRVCTRL3;
    val &= ~0x77700000;
    val |=  0x33300000;
    /* Set register PMMR before use register PFC_DRVCTRL3 */
    PMMR = ~val;
    /* Set register PFC_DRVCTRL3 */
    PFC_DRVCTRL3 = val;

    /* Set the GPSR2 to configure GP2_10 as GPIO */
    val = GPSR2;
    val &= ~(0x01 << 10);
    /* Set register PMMR before use register GPSR2 */
    PMMR = ~val;
    /* Set register GPSR2 */
    GPSR2 = val;
}


/******************************************************************************
                              System Initialization
******************************************************************************/
/* Perform the System initialization */
void loc_CpgInit(void)
{
    r_cpg_Error_t cpg_ret;
    cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_8, R_CPG_BIT12, false);
}

void R_AVB_Init(void)
{
    loc_PfcInit();
    loc_CpgInit();
}

/******************************************************************************
                              Reset ETH external PHY
******************************************************************************/
/* Reset ETH external PHY */
void R_AVB_PhyExtReset(void)
{
    /* Set the GPSR2 to configure GP2_10 as GPIO */
    R_GPIO_SetGpio(2, 10, 1);
    /* Set GP2_10 to low level to trigger phy reset via pin RESET_N */
    R_GPIO_SetOutput(2, 10 ,0);
    /* Delay 10ms for stabling supply voltages to de-assertion of reset */
    R_TICK_DelayUs(10000);
    /* Set GP2_10 to high level to stop reset processing */
    R_GPIO_SetOutput(2, 10 ,1);
}
