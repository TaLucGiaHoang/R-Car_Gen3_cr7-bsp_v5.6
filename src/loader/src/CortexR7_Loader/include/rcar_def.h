/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015-2022, Renesas Electronics Corporation.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RCAR_DEF_H__
#define RCAR_DEF_H__

/* Product register	*/
#define	RCAR_PRR			(0xFFF00044U)

/*******************************************************************************
 *  RCAR product and cut information
 ******************************************************************************/
#define RCAR_PRODUCT_MASK		(0x00007F00U)
#define RCAR_CUT_MASK			(0x000000FFU)
#define RCAR_PRODUCT_H3			(0x00004F00U)
#define RCAR_PRODUCT_M3			(0x00005200U)
#define RCAR_PRODUCT_M3N		(0x00005500U)
#define RCAR_PRODUCT_E3			(0x00005700U)
#define RCAR_CUT_VER10			(0x00000000U)
/* H3/M3N/E3 Ver.1.1 */
#define RCAR_CUT_VER11			(0x00000001U)
/* M3 Ver.1.1/Ver.1.2 */
#define RCAR_M3_CUT_VER11		(0x00000010U)
#define RCAR_CUT_VER20			(0x00000010U)
#define RCAR_CUT_VER30			(0x00000020U)
#define RCAR_MAJOR_MASK			(0x000000F0U)
#define RCAR_MINOR_MASK			(0x0000000FU)
#define RCAR_PRODUCT_SHIFT		(8U)
#define RCAR_MAJOR_SHIFT		(4U)
#define RCAR_MINOR_SHIFT		(0U)
#define RCAR_MAJOR_OFFSET		(1U)
#define RCAR_M3_MINOR_OFFSET		(2U)
/* H3 Ver1.0 */
#define RCAR_PRODUCT_H3_CUT10		(RCAR_PRODUCT_H3 | 0x00U)
/* H3 Ver1.1 */
#define RCAR_PRODUCT_H3_CUT11		(RCAR_PRODUCT_H3 | 0x01U)
/* H3 Ver2.0 */
#define RCAR_PRODUCT_H3_CUT20		(RCAR_PRODUCT_H3 | 0x10U)
/* H3 Ver3.0 */
#define RCAR_PRODUCT_H3_CUT30		(RCAR_PRODUCT_H3 | 0x20U)
/* M3 Ver1.0 */
#define RCAR_PRODUCT_M3_CUT10		(RCAR_PRODUCT_M3 | 0x00U)
/* M3 Ver1.1 / Ver.1.2 */
#define RCAR_PRODUCT_M3_CUT11		(RCAR_PRODUCT_M3 | 0x10U)
/* M3 Ver1.3 */
#define RCAR_PRODUCT_M3_CUT13		(RCAR_PRODUCT_M3 | 0x11U)
/* M3 Ver3.0 */
#define RCAR_PRODUCT_M3_CUT30		(RCAR_PRODUCT_M3 | 0x20U)
/* M3N Ver1.0 */
#define RCAR_PRODUCT_M3N_CUT10		(RCAR_PRODUCT_M3N | 0x00U)
/* E3 Ver1.0 */
#define RCAR_PRODUCT_E3_CUT10		(RCAR_PRODUCT_E3 | 0x00U)
/* E3 Ver1.1 */
#define RCAR_PRODUCT_E3_CUT11		(RCAR_PRODUCT_E3 | 0x01U)

/*******************************************************************************
 *  RCAR MD pin information
 ******************************************************************************/
/* Mode pin register */
#define RCAR_MODEMR			(0xE6160060U)
#define MODEMR_BOOT_CPU_MASK		(0x000000C0U)
#define MODEMR_BOOT_CPU_CR7		(0x000000C0U)
#define MODEMR_BOOT_CPU_CA57		(0x00000000U)
#define MODEMR_BOOT_CPU_CA53		(0x00000040U)
#define MODEMR_BOOT_DEV_MASK		(0x0000001EU)
#define MODEMR_BOOT_DEV_HYPERFLASH160	(0x00000004U)
#define MODEMR_BOOT_DEV_HYPERFLASH80	(0x00000006U)
#define MODEMR_BOOT_DEV_QSPI_FLASH40	(0x00000008U)
#define MODEMR_BOOT_DEV_QSPI_FLASH80	(0x0000000CU)

/*******************************************************************************
 * Memory mapped Generic timer interfaces
 ******************************************************************************/
#define ARM_SYS_CNTCTL_BASE		RCAR_CNTC_BASE

/*******************************************************************************
 *  Boot CPU
 ******************************************************************************/
#define	RCAR_BOOT_CA5X			(0U)	/* Master boot CPU is CA57/53 */
#define	RCAR_BOOT_CR7			(1U)	/* Master boot CPU is CR7 */

/*******************************************************************************
 *  Main CPU
 ******************************************************************************/
#define MAIN_CPU_NONE			(0U)
#define MAIN_CPU_CA57			(1U)
#define MAIN_CPU_CA53			(2U)

/*******************************************************************************
 *  CR Loader GIC Base
 ******************************************************************************/
#define GICD_BASE		(0xF1110000U)
#define GICC_BASE		(0xF1120000U)

/*******************************************************************************
 *  Interrupt ID
 ******************************************************************************/
/* System watch dog timer */
#define	ARM_IRQ_WDT			(173U)


/*******************************************************************************
 *  LifeC Setting
 ******************************************************************************/
#define LIFEC_SETTING_INVALID        (0U)     /* LifeC Setting invalid */

/*******************************************************************************
 *  Shared Data
 ******************************************************************************/


#endif /* RCAR_DEF_H__ */
