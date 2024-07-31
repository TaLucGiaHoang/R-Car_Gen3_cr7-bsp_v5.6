/*
 * Copyright (c) 2015-2020, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>

#include <lib/mmio.h>

#include "emmc_config.h"
#include "emmc_hal.h"
#include "emmc_std.h"
#include "emmc_registers.h"
#include "emmc_def.h"
#include "common.h"
#include "micro_delay.h"
#include "rcar_def.h"
#include "r_prr_api.h"
#include "r_cpg_api.h"

st_mmc_base mmc_drv_obj;

const uint8_t m3w_v13_hs400_calib_code[2][CALIB_TABLE_MAX] = {
    { 3,  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  7,  8,  9, 10, 15, 16, 16, 16, 16, 16, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 25 },
    { 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  7,  8, 11, 12, 17, 18, 18, 18, 18, 18, 18, 18, 19, 20, 21, 22, 23, 25, 25 }
};

const uint8_t m3n_hs400_calib_code[2][CALIB_TABLE_MAX] =  {
    { 1,  2,  6,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31 },
    { 2,  3,  4,  4,  5,  6,  7,  9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 17, 20, 21, 22, 23, 24, 25, 27, 28, 29, 30, 31, 31, 31, 31 }        
};

const uint8_t e3_mmc_hs400_calib_code[CALIB_TABLE_MAX] = 
    { 0,  0,  0,  1,  2,  3,  3,  4,  4,  4,  5,  5,  6,  8,  9, 10, 11, 12, 13, 15, 16, 17, 17, 18, 18, 19, 20, 22, 24, 25, 26, 26 };

EMMC_ERROR_CODE rcar_emmc_memcard_power(uint8_t mode)
{

	if (mode == TRUE) {
		/* power on (Vcc&Vccq is always power on) */
		mmc_drv_obj.card_power_enable = TRUE;
	} else {
		/* power off (Vcc&Vccq is always power on) */
		mmc_drv_obj.card_power_enable = FALSE;
		mmc_drv_obj.mount = FALSE;
		mmc_drv_obj.selected = FALSE;
	}

	return EMMC_SUCCESS;
}
static inline void emmc_set_retry_count(uint32_t retry)
{
	mmc_drv_obj.retries_after_fail = retry;
}

static inline void emmc_set_data_timeout(uint32_t data_timeout)
{
	mmc_drv_obj.data_timeout = data_timeout;
}

static void emmc_memset(uint8_t *buff, uint8_t data, uint32_t cnt)
{
	if (buff == NULL) {
		return;
	}

	while (cnt > 0) {
		*buff++ = data;
		cnt--;
	}
}

static void emmc_driver_config(void)
{
	emmc_set_retry_count(EMMC_RETRY_COUNT);
	emmc_set_data_timeout(EMMC_RW_DATA_TIMEOUT);
}

static void emmc_drv_init(void)
{
	emmc_memset((uint8_t *) (&mmc_drv_obj), 0, sizeof(st_mmc_base));
	mmc_drv_obj.card_present = HAL_MEMCARD_CARD_IS_IN;
	mmc_drv_obj.data_timeout = EMMC_RW_DATA_TIMEOUT;
	mmc_drv_obj.bus_width = HAL_MEMCARD_DATA_WIDTH_1_BIT;
}

static EMMC_ERROR_CODE emmc_dev_finalize(void)
{
	EMMC_ERROR_CODE result;
	uint32_t dataL;

	/*
	 * MMC power off
	 * the power supply of eMMC device is always turning on.
	 * RST_n : Hi --> Low level.
	 */
	result = rcar_emmc_memcard_power(FALSE);

	/* host controller reset */
	SETR_32(SD_INFO1, 0x00000000U);		/* all interrupt clear */
	SETR_32(SD_INFO2, SD_INFO2_CLEAR);	/* all interrupt clear */
	SETR_32(SD_INFO1_MASK, 0x00000000U);	/* all interrupt disable */
	SETR_32(SD_INFO2_MASK, SD_INFO2_CLEAR);	/* all interrupt disable */
	SETR_32(SD_CLK_CTRL, 0x00000000U);	/* MMC clock stop */

	/* Disable clock supply to eMMC */
	R_PMA_CPG_SetClockStopStat(CPG_MSTP_MMC_CLOCK, true);

	return result;
}

static EMMC_ERROR_CODE emmc_dev_init(void)
{
	/* Enable clock supply to eMMC. */
	R_PMA_CPG_SetClockStopStat(CPG_MSTP_MMC_CLOCK, false);

	/* Set SD clock */
	mmio_write_32(CPG_CPGWPR, ~((uint32_t) (BIT9 | BIT0)));	/* SD phy 200MHz */

	// Configure clock appropriately for the specific SoC
	/**
	 * [R-CarH3(Ver.2.0), R-CarM3-W(Ver.1.3)]
	 * 		SDnSRCFC[2:0] = B'001(SDnH = 400MHz) , SDnFC[1:0] = B'00(SDn = 200MHz)
	 * [R-CarH3(Ver.3.0), R-CarM3-W+, R-CarM3-N and R-CarE3]
	 * 		SDnSRCFC[2:0] = B'000(SDnH = 800MHz) , SDnFC[1:0] = B'01(SDn = 200MHz)
	*/
	if (mmc_drv_obj.hs400_use_4tap) {
		/* SDnH=400MHz, SDn=200MHz */
		mmio_write_32(CPG_SDxCKCR, BIT2);
	}
	else {
		/* SDnH=800MHz, SDn=200MHz */
		mmio_write_32(CPG_SDxCKCR, BIT0);
	}

	/* MMCIF initialize */
	SETR_32(SD_INFO1, 0x00000000U);		/* all interrupt clear */
	SETR_32(SD_INFO2, SD_INFO2_CLEAR);	/* all interrupt clear */
	SETR_32(SD_INFO1_MASK, 0x00000000U);	/* all interrupt disable */
	SETR_32(SD_INFO2_MASK, SD_INFO2_CLEAR);	/* all interrupt disable */

	SETR_32(HOST_MODE, 0x00000000U);	/* SD_BUF access width = 64-bit */
	SETR_32(SD_OPTION, 0x0000C0EEU);	/* Bus width = 1bit, timeout=MAX */
	SETR_32(SD_CLK_CTRL, 0x00000000U);	/* Disable Automatic Control & Clock Output */

	return EMMC_SUCCESS;
}


void emmc_parse_hs400_support(void)
{
	r_prr_DevName_t prr_dev = R_PRR_GetDevice();
	r_prr_Rev_t prr_rev = R_PRR_GetRevision();

	mmc_drv_obj.tap                      = 0x00000300;
	mmc_drv_obj.tap_hs400                = 0x00000300;
	mmc_drv_obj.hs400_use_4tap           = 0;
	mmc_drv_obj.dma_tranend1             = 20;
	mmc_drv_obj.adjust_hs400_calib_table = NULL;
	mmc_drv_obj.adjust_hs400_enable      = 0;
	mmc_drv_obj.adjust_hs400_offset      = SCC_TMPPORT3_OFFSET_0;

	mmc_drv_obj.hs400_capable = TRUE;

	/* R-CarGen3 SCC use manual sampling clock position correction in HS400 (Following eMMC_restriction_WA_Ver5.4) */
	mmc_drv_obj.hs400_manual_correction = 1;

	/* R-CarGen3 SCC doesn't use DAT signal correction error status in HS400 (Following eMMC_restriction_WA_Ver5.4) */
	mmc_drv_obj.hs400_ignore_dat_correction = 1;

	switch (prr_dev) {
		case R_PRR_RCARV3M:
		case R_PRR_RCARD3:
			/* D3 & V3M doesn't support HS400 speed. */
			mmc_drv_obj.hs400_capable = FALSE;
			break;
		case R_PRR_RCARV3H:
			/* V3H HS400 mode is a specification limitation (Following R-Car Gen3 W/A of eMMC restriction #109). */
			switch (prr_rev) {
				case R_PRR_REV21:
					/* FIXME : Need workaround for V3H 2.1 ? */
					mmc_drv_obj.hs400_manual_correction = 0;
					mmc_drv_obj.hs400_ignore_dat_correction = 0;
					break;
				default:
					/* V3H before rev.2.1 doesn't support HS400 speed */
					mmc_drv_obj.hs400_capable = FALSE;
					break;
			}
		case R_PRR_RCARH3:
			switch (prr_rev) {
				case R_PRR_REV10:
				case R_PRR_REV11:
					mmc_drv_obj.dma_tranend1 = 17;
					mmc_drv_obj.hs400_use_4tap = 1;
					/* H3 1.0 and 1.1 HS400 mode are a specification limitation (Following R-Car Gen3 W/A of eMMC restriction #109) */
					mmc_drv_obj.hs400_capable = FALSE;
					break;
				case R_PRR_REV20:
					/* H3 2.0 uses 4TAP in HS400 mode */
					mmc_drv_obj.hs400_use_4tap = 1;
					/* H3 2.0 doesn't use TAP 2, 3, 6, 7 in HS400 (Following eMMC_restriction_WA_Ver5.4) */
					mmc_drv_obj.hs400_bad_tap = (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7);
					/* 4tap use 0x0100 for DT2FF */
					mmc_drv_obj.tap_hs400 = 0x00000100;
					break;
				case R_PRR_REV30: // ES3.0
					/* H3 3.0 doesn't use TAP 2, 3, 6, 7 in HS400 (Following eMMC_restriction_WA_Ver5.4) */
					mmc_drv_obj.hs400_bad_tap = (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7);
					break;
				default: 
					break;
			}
			break;
		case R_PRR_RCARM3W:
			switch (prr_rev) {
				case R_PRR_REV10: // ES1.0
					mmc_drv_obj.dma_tranend1 = 17;
				case R_PRR_REV11: // ES1.1 & ES1.2
					/* M3 1.0 and 1.1 and 1.2 HS400 mode are a specification limitation (Following R-Car Gen3 W/A of eMMC restriction #109) */
					mmc_drv_obj.hs400_capable = FALSE;
					break;
				case R_PRR_REV13: // ES1.3
					/* M3 1.3 uses 4TAP in HS400 mode */
					mmc_drv_obj.hs400_use_4tap = 1;
					/* M3 1.3 doesn't use TAP 1, 3 in HS400 mode (Following eMMC_restriction_WA_Ver5.4) */
					mmc_drv_obj.hs400_bad_tap = (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7); //(TAP=2,TAP=3)/2=(TAP=1) and (TAP=6,TAP=7)/2=(TAP=3)
					/* 4tap use 0x0100 for DT2FF */
					mmc_drv_obj.tap_hs400 = 0x00000100;
					/* M3 1.3 needs to apply Manual Calibration Adjustment in HS400 mode (Following R-Car Gen3 W/A of eMMC restriction #109) */
					mmc_drv_obj.adjust_hs400_enable = 1;
					if (USE_MMC_CH == MMC_CH0) // MMC0
						mmc_drv_obj.adjust_hs400_calib_table = m3w_v13_hs400_calib_code[0];
					else // MMC1
						mmc_drv_obj.adjust_hs400_calib_table = m3w_v13_hs400_calib_code[1];
					break;
				case R_PRR_REV30: // ES3.0
					/* M3 3.0 doesn't use TAP 1, 3, 5, 7 in HS400 mode (Following eMMC_restriction_WA_Ver5.4) */
					mmc_drv_obj.hs400_bad_tap = (1 << 1) | (1 << 3) | (1 << 5) | (1 << 7);
					break;
				default: 
					break;
			}
			break;
		case R_PRR_RCARM3N:
			/* M3N doesn't use TAP 2, 3, 6, 7 in HS400 (Following eMMC_restriction_WA_Ver5.4) */
			mmc_drv_obj.hs400_bad_tap = (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7);
			/* M3N needs to apply Manual Calibration Adjustment in HS400 mode (Following R-Car Gen3 W/A of eMMC restriction #109) */
			mmc_drv_obj.adjust_hs400_enable = 1;
			if (USE_MMC_CH == MMC_CH0) // MMC0
				mmc_drv_obj.adjust_hs400_calib_table = m3n_hs400_calib_code[0];
			else
				mmc_drv_obj.adjust_hs400_calib_table = m3n_hs400_calib_code[1];
				
			break;
		case R_PRR_RCARE3:
			/* There is no inhibit TAP for  R-Car E3. */
			mmc_drv_obj.hs400_bad_tap = 0;
			/* E3 SCC can use Auto sampling clock position correction in HS400 (Following eMMC_restriction_WA_Ver5.4) */
			//mmc_drv_obj.hs400_manual_correction = 0;
			/* E3 needs to apply Manual Calibration Adjustment in HS400 mode (Following R-Car Gen3 W/A of eMMC restriction #109) */
			mmc_drv_obj.adjust_hs400_enable = 1;
			if (USE_MMC_CH == MMC_CH1) // MMC1
				mmc_drv_obj.adjust_hs400_calib_table = e3_mmc_hs400_calib_code;
			break;
		default:
			break;
	}
}

static EMMC_ERROR_CODE emmc_reset_controller(void)
{
	EMMC_ERROR_CODE result;

	/* initialize mmc driver */
	emmc_drv_init();

	/* parse hs400 support for specific SoC */
	emmc_parse_hs400_support();

	/* initialize H/W */
	result = emmc_dev_init();
	if (result == EMMC_SUCCESS) {
		mmc_drv_obj.initialize = TRUE;
	}

	return result;

}

EMMC_ERROR_CODE emmc_terminate(void)
{
	EMMC_ERROR_CODE result;

	result = emmc_dev_finalize();

	emmc_memset((uint8_t *) (&mmc_drv_obj), 0, sizeof(st_mmc_base));

	return result;
}

EMMC_ERROR_CODE rcar_emmc_init(void)
{
	EMMC_ERROR_CODE result;

	micro_wait_init();
	result = emmc_reset_controller();
	if (result == EMMC_SUCCESS) {
		emmc_driver_config();
	}

	return result;
}
