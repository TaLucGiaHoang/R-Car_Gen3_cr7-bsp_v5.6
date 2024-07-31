/*
 * Copyright (c) 2015-2020, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <lib/mmio.h>
#include <string.h>

#include "emmc_config.h"
#include "emmc_def.h"
#include "emmc_hal.h"
#include "emmc_registers.h"
#include "emmc_std.h"
#include "micro_delay.h"
#include "rcar_def.h"

static EMMC_ERROR_CODE emmc_clock_ctrl(uint8_t mode);
static EMMC_ERROR_CODE emmc_card_init(void);
static EMMC_ERROR_CODE emmc_init_high_speed(void);
static EMMC_ERROR_CODE emmc_bus_width(uint32_t width, uint8_t ddr);
static uint32_t emmc_set_timeout_register_value(uint32_t freq);
static void set_sd_clk(uint32_t clkDiv);
static uint32_t emmc_calc_tran_speed(uint32_t *freq);
static void emmc_get_partition_access(void);
static void emmc_set_bootpartition(void);

// tuning block pattern for 4 bit mode
const uint8_t	emmc_tbp_4bit[] = {
	0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
	0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
	0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
	0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
	0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
	0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
	0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
	0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
};

// tuning block pattern for 8 bit mode
const uint8_t	emmc_tbp_8bit[] = {
	0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00,
	0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc, 0xcc,
	0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff,
	0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee, 0xff,
	0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd,
	0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb,
	0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff,
	0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 0xff,
	0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00,
	0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc,
	0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff,
	0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee,
	0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd,
	0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff,
	0xbb, 0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff,
	0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee,
};

static void emmc_set_bootpartition(void)
{
	uint32_t reg;

	reg = mmio_read_32(RCAR_PRR) & (PRR_PRODUCT_MASK | PRR_CUT_MASK);
	if (reg == PRR_PRODUCT_M3_CUT10) {
		mmc_drv_obj.boot_partition_en =
		    (EMMC_PARTITION_ID) ((mmc_drv_obj.ext_csd_data[179] &
					  EMMC_BOOT_PARTITION_EN_MASK) >>
					 EMMC_BOOT_PARTITION_EN_SHIFT);
	} else if ((reg == PRR_PRODUCT_H3_CUT20)
		   || (reg == PRR_PRODUCT_M3_CUT11)) {
		mmc_drv_obj.boot_partition_en = mmc_drv_obj.partition_access;
	} else {
		if ((mmio_read_32(MFISBTSTSR) & MFISBTSTSR_BOOT_PARTITION) !=
		    0U) {
			mmc_drv_obj.boot_partition_en = PARTITION_ID_BOOT_2;
		} else {
			mmc_drv_obj.boot_partition_en = PARTITION_ID_BOOT_1;
		}
	}
}

static EMMC_ERROR_CODE emmc_card_init(void)
{
	int32_t retry;
	uint32_t freq = MMC_400KHZ;	/* 390KHz */
	EMMC_ERROR_CODE result;
	uint32_t result_calc;

	/* state check */
	if ((mmc_drv_obj.initialize != TRUE)
	    || (mmc_drv_obj.card_power_enable != TRUE)
	    || ((GETR_32(SD_INFO2) & SD_INFO2_CBSY) != 0)
	    ) {
		emmc_write_error_info(EMMC_FUNCNO_CARD_INIT, EMMC_ERR_STATE);
		return EMMC_ERR_STATE;
	}

	/* clock on (force change) */
	mmc_drv_obj.current_freq = 0;
	mmc_drv_obj.max_freq = MMC_20MHZ;
	result = emmc_set_request_mmc_clock(freq);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return EMMC_ERR;
	}

	rcar_micro_delay(1000U);	/* wait 1ms */

	/* Get current access partition */
	emmc_get_partition_access();

	/* CMD0, arg=0x00000000 */
	result = emmc_send_idle_cmd(0x00000000);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	rcar_micro_delay(200U);	/* wait 74clock 390kHz(189.74us) */

	/* CMD1 */
	emmc_make_nontrans_cmd(CMD1_SEND_OP_COND, EMMC_HOST_OCR_VALUE);
	for (retry = 300; retry > 0; retry--) {
		result =
		    emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
		if (result != EMMC_SUCCESS) {
			emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
			return result;
		}

		if ((mmc_drv_obj.r3_ocr & EMMC_OCR_STATUS_BIT) != 0) {
			break;	/* card is ready. exit loop */
		}
		rcar_micro_delay(1000U);	/* wait 1ms */
	}

	if (retry == 0) {
		emmc_write_error_info(EMMC_FUNCNO_CARD_INIT, EMMC_ERR_TIMEOUT);
		return EMMC_ERR_TIMEOUT;
	}

	switch (mmc_drv_obj.r3_ocr & EMMC_OCR_ACCESS_MODE_MASK) {
	case EMMC_OCR_ACCESS_MODE_SECT:
		mmc_drv_obj.access_mode = TRUE;	/* sector mode */
		break;
	default:
		/* unknown value */
		emmc_write_error_info(EMMC_FUNCNO_CARD_INIT, EMMC_ERR);
		return EMMC_ERR;
	}

	/* CMD2 */
	emmc_make_nontrans_cmd(CMD2_ALL_SEND_CID_MMC, 0x00000000);
	mmc_drv_obj.response = (uint32_t *) (&mmc_drv_obj.cid_data[0]);	/* use CID special buffer */
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	/* CMD3 */
	emmc_make_nontrans_cmd(CMD3_SET_RELATIVE_ADDR, EMMC_RCA << 16);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	/* CMD9 (CSD) */
	emmc_make_nontrans_cmd(CMD9_SEND_CSD, EMMC_RCA << 16);
	mmc_drv_obj.response = (uint32_t *) (&mmc_drv_obj.csd_data[0]);	/* use CSD special buffer */
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	/* card version check */
	if (EMMC_CSD_SPEC_VARS() < 4) {
		emmc_write_error_info(EMMC_FUNCNO_CARD_INIT,
				      EMMC_ERR_ILLEGAL_CARD);
		return EMMC_ERR_ILLEGAL_CARD;
	}

	/* CMD7 (select card) */
	emmc_make_nontrans_cmd(CMD7_SELECT_CARD, EMMC_RCA << 16);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	mmc_drv_obj.selected = TRUE;

	/*
	 * card speed check
	 * Card spec is calculated from TRAN_SPEED(CSD)
	 */
	result_calc = emmc_calc_tran_speed(&freq);
	if (result_calc == 0) {
		emmc_write_error_info(EMMC_FUNCNO_CARD_INIT,
				      EMMC_ERR_ILLEGAL_CARD);
		return EMMC_ERR_ILLEGAL_CARD;
	}
	mmc_drv_obj.max_freq = freq;	/* max frequency (card spec) */

	result = emmc_set_request_mmc_clock(freq);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return EMMC_ERR;
	}

	/* set read/write timeout */
	mmc_drv_obj.data_timeout = emmc_set_timeout_register_value(freq);
	SETR_32(SD_OPTION,
		((GETR_32(SD_OPTION) & ~(SD_OPTION_TIMEOUT_CNT_MASK)) |
		 mmc_drv_obj.data_timeout));

	/* SET_BLOCKLEN(512byte) */
	/* CMD16 */
	emmc_make_nontrans_cmd(CMD16_SET_BLOCKLEN, EMMC_BLOCK_LENGTH);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	/* Transfer Data Length */
	SETR_32(SD_SIZE, EMMC_BLOCK_LENGTH);

	/* CMD8 (EXT_CSD) */
	emmc_make_trans_cmd(CMD8_SEND_EXT_CSD, 0x00000000,
			    (uint32_t *) (&mmc_drv_obj.ext_csd_data[0]),
			    EMMC_MAX_EXT_CSD_LENGTH, HAL_MEMCARD_READ,
			    HAL_MEMCARD_NOT_DMA);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		/*
		 * CMD12 is not send.
		 * If BUS initialization is failed, user must be execute Bus initialization again.
		 * Bus initialization is start CMD0(soft reset command).
		 */
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		return result;
	}

	/* Set boot partition */
	emmc_set_bootpartition();

	return EMMC_SUCCESS;
}

static uint32_t emmc_tmpport_read(uint32_t addr)
{
	/* Read mode */
	SETR_32(SCC_TMPPORT5, SCC_TMPPORT5_DLL_RW_SEL_R |
				(SCC_TMPPORT5_DLL_ADR_MASK & addr));

	/* Access start and stop */
	SETR_32(SCC_TMPPORT4, SCC_TMPPORT4_DLL_ACC_START);
	SETR_32(SCC_TMPPORT4, 0);

	/* Read and return value */
	return GETR_32(SCC_TMPPORT7);
}

static void emmc_tmpport_write(uint32_t addr, uint32_t val)
{
	/* Write mode */
	SETR_32(SCC_TMPPORT5, SCC_TMPPORT5_DLL_RW_SEL_W |
		(SCC_TMPPORT5_DLL_ADR_MASK & addr));

	/* Write value */
	SETR_32(SCC_TMPPORT6, val);

	/* Access start and stop */
	SETR_32(SCC_TMPPORT4, SCC_TMPPORT4_DLL_ACC_START);
	SETR_32(SCC_TMPPORT4, 0);
}

void emmc_hs400_adjust_enable(void)
{
	uint32_t        calib_code;

	if(!mmc_drv_obj.adjust_hs400_calib_table)
		return;

	/* Release WriteProtect of DLL built-in register:
	 *  Instruct Write: Target register :MD(H’00)
	 *  Set WriteProtect release code(H’A500_0000)
	 */
	emmc_tmpport_write(0x00, SCC_TMPPORT_DISABLE_WP_CODE);

	/* Reading the calibration code value:
		*  Instruction of Read: Target register :DQSRMONH0(H’26)
		*/
	calib_code = emmc_tmpport_read(0x26);
	calib_code &= SCC_TMPPORT_CALIB_CODE_MASK;
	calib_code = mmc_drv_obj.adjust_hs400_calib_table[calib_code];

	/* Adjust internal DS signal:
	 *  Instruct Write: Target register :DQSRSETH0(H’22)
	 *  Set DQSRSETH0.DQS00RSET(m) and write calibration code
	 *  Set DelayLine Offset value(n) to TMPPORT3
	 */
	emmc_tmpport_write(0x22, SCC_TMPPORT_MANUAL_MODE | calib_code);
	SETR_32(SCC_TMPPORT3, mmc_drv_obj.adjust_hs400_offset);

	mmc_drv_obj.need_adjust_hs400_done = 1;

	/* Clear adjust HS400 mode flag */
	mmc_drv_obj.need_adjust_hs400 = 0;
}

static void emmc_hs400_adjust_disable()
{
	/* Release WriteProtect of DLL built-in register:
	 *  Instruct Write: Target register :MD(H’00)
	 *  Set WriteProtect release code(H’A500_0000)
	 */
	emmc_tmpport_write(0x00, SCC_TMPPORT_DISABLE_WP_CODE);

	/* Releases the adjustment of the internal DS signal
	 *  Disabled Manual Calibration W(addr=0x22, 0)
	 *  Clear offset value to TMPPORT3
	 */
	emmc_tmpport_write(0x22, 0);
	SETR_32(SCC_TMPPORT3, 0);

	mmc_drv_obj.need_adjust_hs400_done = 0;
}

static void emmc_scc_hs400_reset()
{
	/* Reset HS4000 mode */
	SETR_32(SDIF_MODE, GETR_32(SDIF_MODE) & ~SDIF_MODE_HS400);
	SETR_32(SCC_DT2FF, mmc_drv_obj.tap);
	SETR_32(SCC_TMPPORT2,  GETR_32(SCC_TMPPORT2) &
			~(SCC_TMPPORT2_HS400OSEL | SCC_TMPPORT2_HS400EN));

	if (mmc_drv_obj.need_adjust_hs400_done == 1) {
		emmc_hs400_adjust_disable();
	}
}

static void emmc_scc_reset()
{
	/* Stop clock */
	SETR_32(SD_CLK_CTRL, GETR_32(SD_CLK_CTRL) & ~SD_CLK_CTRL_SCLKEN);

	/* Reset SCC */
	SETR_32(SCC_CKSEL, ~SCC_CKSEL_DTSEL & GETR_32(SCC_CKSEL));
	SETR_32(SCC_DTCNTL, ~SCC_DTCNTL_TAPEN & GETR_32(SCC_DTCNTL));

	emmc_scc_hs400_reset();

	/* Start clock */
	SETR_32(SD_CLK_CTRL, GETR_32(SD_CLK_CTRL) | SD_CLK_CTRL_SCLKEN);

	SETR_32(SCC_RVSCNTL, ~SCC_RVSCNTL_RVSEN & GETR_32(SCC_RVSCNTL));
	SETR_32(SCC_RVSCNTL, ~SCC_RVSCNTL_RVSEN & GETR_32(SCC_RVSCNTL));

	mmc_drv_obj.tuning_status = EMMC_TUNING_NONE;
}

static EMMC_ERROR_CODE emmc_select_tuning(void)
{
	int      tapcnt;    /* Count 'OK' TAP */
	int      tapset;    /* Tap position */
	int      tapstart;  /* Start position of OK' TAP */
	int      tapend;    /* End position of OK' TAP */
	int      ntap;      /* Number of 'OK' TAP */
	int      matchcnt;  /* Count of matching data */
	int      i;

	/* Clear adjust hs400 flag */
	mmc_drv_obj.need_adjust_hs400 = 0;
	mmc_drv_obj.tuning_status = EMMC_TUNING_DONE;

	/* Clear SCC_RVSREQ */
	SETR_32(SCC_RVSREQ, 0);

	/* If TAP is 'NG', force its repeated display index to 'NG' */
	for (i = 0; i < mmc_drv_obj.tap_num * 2; i++) {
		if (!mmc_drv_obj.taps[i]) {
			mmc_drv_obj.taps[i % mmc_drv_obj.tap_num] = 0;
			mmc_drv_obj.taps[(i % mmc_drv_obj.tap_num) + mmc_drv_obj.tap_num] = 0;
		}
		if (!mmc_drv_obj.smpcmp[i]) {
			mmc_drv_obj.smpcmp[i % mmc_drv_obj.tap_num] = 0;
			mmc_drv_obj.smpcmp[(i % mmc_drv_obj.tap_num) + mmc_drv_obj.tap_num] = 0;
		}
	}

	/* Find largest range of TAPs are 'OK'. The sampling clock position = (start position + end position) / 2 */
	tapset   = -1;
	tapcnt   = 0;
	ntap     = 0;
	tapstart = 0;
	tapend   = 0;
	for (i = 0; i < mmc_drv_obj.tap_num * 2; i++) {
		if (mmc_drv_obj.taps[i]) {
			ntap++;
		} else {
			if (ntap > tapcnt) {
				tapstart = i - ntap;
				tapend   = i - 1;
				tapcnt   = ntap;
			}
			ntap = 0;
		}
	}

	if (ntap > tapcnt) {
		tapstart = i - ntap;
		tapend   = i - 1;
		tapcnt   = ntap;
	}

	if((tapcnt < mmc_drv_obj.tap_num * 2) && (tapcnt >= EMMC_SDHI_MAX_TAP)) {
		tapset = ((tapstart + tapend) / 2) % mmc_drv_obj.tap_num;
	}
	else if (tapcnt == mmc_drv_obj.tap_num * 2) {
		/* If all TAP is 'OK', the sampling clock position is selected by
		 * identifying the change point of data */
		matchcnt = 0;
		ntap     = 0;
		tapstart = 0;
		tapend   = 0;
		for (i = 0; i < mmc_drv_obj.tap_num * 2; i++) {
			if (mmc_drv_obj.smpcmp[i]) {
				ntap++;
			} else {
				if (ntap > matchcnt) {
					tapstart = i - ntap;
					tapend   = i - 1;
					matchcnt = ntap;
				}
				ntap = 0;
			}
		}
		if (ntap > matchcnt) {
			tapstart = i - ntap;
			tapend   = i - 1;
			matchcnt = ntap;
		}
		if (matchcnt)
			tapset = ((tapstart + tapend) / 2) % mmc_drv_obj.tap_num;
	}

	/* Tuning is failed */
	if (tapset == -1) {
		return EMMC_ERR;
	}

	/* Set TAPSET */
	SETR_32(SCC_TAPSET, tapset);

	/* Save for HS400 case */
	mmc_drv_obj.tap_set = tapset;

	/* Enable auto-retuning */
	SETR_32(SCC_RVSCNTL, SCC_RVSCNTL_RVSEN | GETR_32(SCC_RVSCNTL));

	return EMMC_SUCCESS;
}

EMMC_ERROR_CODE emmc_tune(void)
{
	uint8_t         td[128] __attribute__ ((aligned (128)));
	const uint8_t   *tbp;
	int             tlen;
	int             status;
	int             i;
	int             tapnum;
	EMMC_ERROR_CODE result;

	/* return if not HS200 that requires tuning */
	if (mmc_drv_obj.hs_timing != TIMING_HS200) {
		return EMMC_ERR;
	}

	tapnum  = 8;
	if (mmc_drv_obj.bus_width == HAL_MEMCARD_DATA_WIDTH_8_BIT) {
		tlen = 128;
		tbp = emmc_tbp_8bit;
	}
	else {
		tlen = 64;
		tbp = emmc_tbp_4bit;
	}

	mmc_drv_obj.tuning_status = EMMC_TUNING_DOING;

	memset(mmc_drv_obj.taps, 0, EMMC_SDHI_TUNING_RETRIES);   // Initialize all tap  to 'NG'
	memset(mmc_drv_obj.smpcmp, 0, EMMC_SDHI_TUNING_RETRIES); // Initialize all smpcmp to 'NG'

	/* Clear SD flag status */
	SETR_32(SD_INFO1, 0);
	SETR_32(SD_INFO2, 0);

	/* Set SD size */
	SETR_32(SD_SIZE, tlen);

	/* Stop clock */
	SETR_32(SD_CLK_CTRL, GETR_32(SD_CLK_CTRL) & ~SD_CLK_CTRL_SCLKEN);

	/* Initialize SCC */
	SETR_32(SCC_DTCNTL, SCC_DTCNTL_TAPEN | (tapnum << 16));
	SETR_32(SCC_RVSCNTL, ~SCC_RVSCNTL_RVSEN & GETR_32(SCC_RVSCNTL));
	SETR_32(SCC_DT2FF, mmc_drv_obj.tap);
	SETR_32(SCC_CKSEL, SCC_CKSEL_DTSEL | GETR_32(SCC_CKSEL));

	/* Start clock */
	SETR_32(SD_CLK_CTRL, GETR_32(SD_CLK_CTRL) | SD_CLK_CTRL_SCLKEN);

	mmc_drv_obj.tap_num = (GETR_32(SCC_DTCNTL) >> 16) & 0xFF;

	/* Issue CMD19/CMD21, 2 times for each tap */
	for (i = 0; i < mmc_drv_obj.tap_num * 2; i++) {
		/* Clear tuning data buffer to avoid comparing old data after each unsuccessful transfer */
		memset(td, 0, tlen);

		/* Change the sampling clock position */
		SETR_32(SCC_TAPSET, i % mmc_drv_obj.tap_num);

		/* Setup tuning command */
		emmc_make_trans_cmd(CMD21, 0, (uint32_t*)&td, tlen, HAL_MEMCARD_READ, HAL_MEMCARD_DMA);

		/* Execute and wait for tuning command completed */
		result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);

		/* Tuning command is completed successfully, compare result data with tuning block pattern.
		 * It is to determine the sampling clock position */
		if (result == EMMC_SUCCESS) {
			if (!(memcmp(td, tbp, tlen))) {
				mmc_drv_obj.taps[i] = 1; //This TAP is 'OK'
			}
		} else { /* Fix tuning error on some hardware platforms */
			/* A delay of 3.75ms (=150ms/40) based on the specification that the device should complete 40 commands in 150msec */
			rcar_micro_delay(3750);
		}

		if (!GETR_32(SCC_SMPCMP)) {
			mmc_drv_obj.smpcmp[i] = 1; //smpcmp of this TAP is 'OK'
		}
	}

	status = emmc_select_tuning();

	if (status != EMMC_SUCCESS) {
		emmc_scc_reset();
	}

	/* CMD13 */
	emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
	result =
	    emmc_exec_cmd(EMMC_R1_ERROR_MASK_WITHOUT_CRC, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_HIGH_SPEED);
		return result;
	}

	return status;
}

static EMMC_ERROR_CODE emmc_init_high_speed(void)
{
	uint32_t freq;	      /* High speed mode clock frequency */
	EMMC_ERROR_CODE result;
	uint8_t cardType;

	/* state check */
	if (mmc_drv_obj.selected != TRUE) {
		emmc_write_error_info(EMMC_FUNCNO_HIGH_SPEED, EMMC_ERR_STATE);
		return EMMC_ERR_STATE;
	}

	// set buswidth to 8bit
	if ((result = emmc_bus_width(8, FALSE)) != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_HIGH_SPEED);
		return result;
	}

	/* max frequency */
	cardType = (uint8_t) mmc_drv_obj.ext_csd_data[EMMC_EXT_CSD_CARD_TYPE];
	if ((cardType & EMMC_EXT_CSD_CARD_TYPE_52MHZ) != 0)
		freq = MMC_52MHZ;
	else if ((cardType & EMMC_EXT_CSD_CARD_TYPE_26MHZ) != 0)
		freq = MMC_26MHZ;
	else
		freq = MMC_20MHZ;

	/* Hi-Speed-mode selection */
	if ((freq == MMC_52MHZ) || (freq == MMC_26MHZ)) {
		/* CMD6 */
		emmc_make_nontrans_cmd(CMD6_SWITCH, EMMC_SWITCH_HS_TIMING);
		result =
			emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
		if (result != EMMC_SUCCESS) {
			emmc_write_error_info_func_no(EMMC_FUNCNO_HIGH_SPEED);
			return result;
		}

		mmc_drv_obj.hs_timing = TIMING_HIGH_SPEED;	/* High-Speed */
	}

	/* set mmc clock */
	mmc_drv_obj.max_freq = freq;
	result = emmc_set_request_mmc_clock(freq);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_HIGH_SPEED);
		return EMMC_ERR;
	}

	/* set read/write timeout */
	mmc_drv_obj.data_timeout = emmc_set_timeout_register_value(freq);
	SETR_32(SD_OPTION,
		((GETR_32(SD_OPTION) & ~(SD_OPTION_TIMEOUT_CNT_MASK)) |
		 mmc_drv_obj.data_timeout));

	/* CMD13 */
	emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
	result =
	    emmc_exec_cmd(EMMC_R1_ERROR_MASK_WITHOUT_CRC, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_HIGH_SPEED);
		return result;
	}

	return EMMC_SUCCESS;
}

static EMMC_ERROR_CODE emmc_clock_ctrl(uint8_t mode)
{
	uint32_t value;

	/* busy check */
	if ((GETR_32(SD_INFO2) & SD_INFO2_CBSY) != 0) {
		emmc_write_error_info(EMMC_FUNCNO_SET_CLOCK,
				      EMMC_ERR_CARD_BUSY);
		return EMMC_ERR;
	}

	if (mode == TRUE) {
		/* clock ON */
		value =
		    ((GETR_32(SD_CLK_CTRL) | MMC_SD_CLK_START) &
		     SD_CLK_WRITE_MASK);
		SETR_32(SD_CLK_CTRL, value);	/* on  */
		mmc_drv_obj.clock_enable = TRUE;
	} else {
		/* clock OFF */
		value =
		    ((GETR_32(SD_CLK_CTRL) & MMC_SD_CLK_STOP) &
		     SD_CLK_WRITE_MASK);
		SETR_32(SD_CLK_CTRL, value);	/* off */
		mmc_drv_obj.clock_enable = FALSE;
	}

	return EMMC_SUCCESS;
}

static EMMC_ERROR_CODE emmc_bus_width(uint32_t width, uint8_t ddr)
{
	EMMC_ERROR_CODE result = EMMC_ERR;

	/* parameter check */
	if ((width != 8) && (width != 4) && (width != 1)) {
		emmc_write_error_info(EMMC_FUNCNO_BUS_WIDTH, EMMC_ERR_PARAM);
		return EMMC_ERR_PARAM;
	}

	/* state check */
	if (mmc_drv_obj.selected != TRUE) {
		emmc_write_error_info(EMMC_FUNCNO_BUS_WIDTH, EMMC_ERR_STATE);
		return EMMC_ERR_STATE;
	}

	/* 2 = 8bit, 1 = 4bit, 0 =1bit */
	mmc_drv_obj.bus_width = (HAL_MEMCARD_DATA_WIDTH) (width >> 2);

	/* CMD6 */
	emmc_make_nontrans_cmd(CMD6_SWITCH,
			       (EMMC_SWITCH_BUS_WIDTH_1 |
			       (mmc_drv_obj.bus_width << 8) |
				   ((ddr)?BUS_WIDTH_DDR:0)));
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		/* occurred error */
		mmc_drv_obj.bus_width = HAL_MEMCARD_DATA_WIDTH_1_BIT;
		goto EXIT;
	}

	switch (mmc_drv_obj.bus_width) {
	case HAL_MEMCARD_DATA_WIDTH_1_BIT:
		SETR_32(SD_OPTION,
			((GETR_32(SD_OPTION) & ~(BIT15 | BIT13)) | BIT15));
		break;
	case HAL_MEMCARD_DATA_WIDTH_4_BIT:
		SETR_32(SD_OPTION, (GETR_32(SD_OPTION) & ~(BIT15 | BIT13)));
		break;
	case HAL_MEMCARD_DATA_WIDTH_8_BIT:
		SETR_32(SD_OPTION,
			((GETR_32(SD_OPTION) & ~(BIT15 | BIT13)) | BIT13));
		break;
	default:
		goto EXIT;
	}

	/* CMD13 */
	emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		goto EXIT;
	}

	return EMMC_SUCCESS;

EXIT:
	emmc_write_error_info(EMMC_FUNCNO_BUS_WIDTH, result);
	ERROR("BL2: emmc bus_width error end\n");
	return result;
}

EMMC_ERROR_CODE emmc_init_hs200(void)
{
	EMMC_ERROR_CODE status;
	uint32_t freq = MMC_200MHZ;

	// Already in HS200
	if (mmc_drv_obj.hs_timing == TIMING_HS200) {
		return EMMC_SUCCESS;
	}

	// set buswidth to 8bit
	if ((status = emmc_bus_width(8, FALSE)) != EMMC_SUCCESS) {
		return status;
	}

	// set mode to HS200
	emmc_make_nontrans_cmd(CMD6_SWITCH, EMMC_SWITCH_HS200_TIMING);
	status =
		emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (status != EMMC_SUCCESS) {
		return status;
	}

	/* set mmc clock */
	mmc_drv_obj.max_freq = freq;
	status = emmc_set_request_mmc_clock(freq);
	if (status != EMMC_SUCCESS) {
		return EMMC_ERR;
	}

	/* set read/write timeout */
	mmc_drv_obj.data_timeout = emmc_set_timeout_register_value(freq);
	SETR_32(SD_OPTION,
		((GETR_32(SD_OPTION) & ~(SD_OPTION_TIMEOUT_CNT_MASK)) |
		 mmc_drv_obj.data_timeout));

	mmc_drv_obj.hs_timing = TIMING_HS200;

	/* SCC tuning */
	if ((status = emmc_tune()) != EMMC_SUCCESS) {
		return status;
	}
	return EMMC_SUCCESS;
}

EMMC_ERROR_CODE _emmc_init_hs400(void)
{
	EMMC_ERROR_CODE status;
	uint32_t new_tap;

	/** 
	 * Go to HS200 first to perform tuning because 
	 * the tuning is necessary for SDCMD to sync with
	 * SDCLK in HS400 mode
	*/
	if ((status = emmc_init_hs200()) != EMMC_SUCCESS) {
		return status;
	}

	// Switch to High Speed 52MHz
	if ((status = emmc_init_high_speed()) != EMMC_SUCCESS) {
		return status;
	}

	// Set buswidth to 8bit DDR
	if ((status = emmc_bus_width(8, TRUE)) != EMMC_SUCCESS) {
		return status;
	}

	// Set mode to HS400
	emmc_make_nontrans_cmd(CMD6_SWITCH, EMMC_SWITCH_HS400_TIMING);
	status =
		emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (status != EMMC_SUCCESS) {
		return status;
	}

	emmc_clock_ctrl(FALSE);

	// Configure HS400 timing registers
	SETR_32(SDIF_MODE, GETR_32(SDIF_MODE) | SDIF_MODE_HS400);
	SETR_32(SCC_DT2FF, mmc_drv_obj.tap_hs400);
	if (mmc_drv_obj.hs400_manual_correction) {
		// Disable SCC sampling clock position correction
		SETR_32(SCC_RVSCNTL, GETR_32(SCC_RVSCNTL) & ~SCC_RVSCNTL_RVSEN);
	}
	SETR_32(SCC_TMPPORT2, GETR_32(SCC_TMPPORT2) | SCC_TMPPORT2_HS400OSEL | SCC_TMPPORT2_HS400EN);
	SETR_32(SCC_DTCNTL, SCC_DTCNTL_TAPEN | (0x4 << 16));

	// Avoid bad TAP
	if (mmc_drv_obj.hs400_bad_tap & (1 << mmc_drv_obj.tap_set)) {
		new_tap = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num + 1) % mmc_drv_obj.tap_num;

		if (mmc_drv_obj.hs400_bad_tap & (1 << new_tap)) {
			new_tap = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num - 1) % mmc_drv_obj.tap_num;
		}

		if (mmc_drv_obj.hs400_bad_tap & (1 << new_tap)) {
			// Three consecutive bad taps, cannot change tap
			new_tap = mmc_drv_obj.tap_set;
		}

		mmc_drv_obj.tap_set = new_tap;
	}
	// Replace the tuning result of 8TAP with 4TAP
	SETR_32(SCC_TAPSET, mmc_drv_obj.tap_set / (mmc_drv_obj.hs400_use_4tap ? 2 : 1));

	SETR_32(SCC_CKSEL, GETR_32(SCC_CKSEL) | SCC_CKSEL_DTSEL);

	/* Apply HS400 SW work around */
	if (mmc_drv_obj.adjust_hs400_enable) {
		/* Set this flag to execute adjust hs400 offset after setting to HS400 mode and after CMD13 */
		mmc_drv_obj.need_adjust_hs400 = 1;
	}

	/* set read/write timeout */
	mmc_drv_obj.data_timeout = emmc_set_timeout_register_value(MMC_200MHZ);
	SETR_32(SD_OPTION,
		((GETR_32(SD_OPTION) & ~(SD_OPTION_TIMEOUT_CNT_MASK)) |
		 mmc_drv_obj.data_timeout));

	/* set mmc clock */
	mmc_drv_obj.max_freq = MMC_100MHZ;
	status = emmc_set_request_mmc_clock(MMC_100MHZ);
	if (status != EMMC_SUCCESS) {
		return EMMC_ERR;
	}

	emmc_clock_ctrl(TRUE);

	mmc_drv_obj.hs_timing = TIMING_HS400;

	return status;
}

EMMC_ERROR_CODE emmc_init_hs400(void)
{
	EMMC_ERROR_CODE status;
	int retries = 20;

	if ((status = _emmc_init_hs400()) != EMMC_SUCCESS) {
		return status;
	}

	/* check status */
	do {
		if (mmc_drv_obj.tuning_status == EMMC_NEED_RETUNING)
			emmc_retune();
		emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
		status =
			emmc_exec_cmd(EMMC_R1_ERROR_MASK_WITHOUT_CRC, mmc_drv_obj.response);
	} while (/* (status != EMMC_SUCCESS) && */ (--retries > 0)
			&& (mmc_drv_obj.tuning_status == EMMC_NEED_RETUNING));
	if (status != EMMC_SUCCESS) {
		return status;
	}
	return EMMC_SUCCESS;
}

void emmc_disable_scc(void)
{
	if (mmc_drv_obj.hs_timing ==  TIMING_HS200 ||
		mmc_drv_obj.hs_timing == TIMING_HS400)
		return;

	SETR_32(SCC_CKSEL, GETR_32(SCC_CKSEL) & ~SCC_CKSEL_DTSEL);
	SETR_32(SCC_DTCNTL, GETR_32(SCC_DTCNTL) & ~SCC_DTCNTL_TAPEN);
}

EMMC_ERROR_CODE emmc_retune(void)
{
	EMMC_ERROR_CODE result;
	int return_to_hs400 = 0;
	if (mmc_drv_obj.hs_timing == TIMING_HS400) {
		/* reduce clock */
		if ((result = emmc_set_request_mmc_clock(MMC_52MHZ)) != EMMC_SUCCESS) {
			return result;
		}

		/* switch to high speed mode */
		emmc_make_nontrans_cmd(CMD6_SWITCH, EMMC_SWITCH_HS_TIMING);
		result =
			emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
		if (result != EMMC_SUCCESS) {
			emmc_write_error_info_func_no(EMMC_FUNCNO_HIGH_SPEED);
			return result;
		}
		mmc_drv_obj.hs_timing = TIMING_HIGH_SPEED;


		emmc_clock_ctrl(FALSE);	/* clock off */

		/* disable SCC */
		emmc_disable_scc();

		/* reset HS400 mode */
		emmc_scc_hs400_reset();

		emmc_clock_ctrl(TRUE);	/* clock on */

		/* check card status */
		emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
		result =
			emmc_exec_cmd(EMMC_R1_ERROR_MASK_WITHOUT_CRC, mmc_drv_obj.response);
		if (result != EMMC_SUCCESS)
			return result;

		/* switch to HS200 and retune */
		if ((result = emmc_init_hs200()) != EMMC_SUCCESS) {
			return result;
		}

		/* return to HS400 as it was before */
		result = _emmc_init_hs400();
	}
	/* already in HS200 */
	else if ((result = emmc_tune()) != EMMC_SUCCESS) {
		return result;
	}

	// check card status
	emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
	result =
		emmc_exec_cmd(EMMC_R1_ERROR_MASK_WITHOUT_CRC, mmc_drv_obj.response);

	return result;
}

EMMC_ERROR_CODE emmc_init_bus(void)
{
	uint8_t cardType;
	EMMC_ERROR_CODE result;
	cardType = (uint8_t) mmc_drv_obj.ext_csd_data[EMMC_EXT_CSD_CARD_TYPE];
	if (((cardType & EMMC_EXT_CSD_CARD_TYPE_HS400_18V) != 0) && (mmc_drv_obj.hs400_capable))
		result = emmc_init_hs400();
	else if ((cardType & EMMC_EXT_CSD_CARD_TYPE_HS200_18V) != 0)
		result = emmc_init_hs200();
	else
		result = emmc_init_high_speed();
	return result;
}

EMMC_ERROR_CODE emmc_select_partition(EMMC_PARTITION_ID id)
{
	EMMC_ERROR_CODE result;
	uint32_t arg;
	uint32_t partition_config;

	/* state check */
	if (mmc_drv_obj.mount != TRUE) {
		emmc_write_error_info(EMMC_FUNCNO_NONE, EMMC_ERR_STATE);
		return EMMC_ERR_STATE;
	}

	/* id = PARTITION_ACCESS(Bit[2:0]) */
	if ((id & ~PARTITION_ID_MASK) != 0) {
		emmc_write_error_info(EMMC_FUNCNO_NONE, EMMC_ERR_PARAM);
		return EMMC_ERR_PARAM;
	}

	/* EXT_CSD[179] value */
	partition_config =
	    (uint32_t) mmc_drv_obj.ext_csd_data[EMMC_EXT_CSD_PARTITION_CONFIG];
	if ((partition_config & PARTITION_ID_MASK) == id) {
		result = EMMC_SUCCESS;
	} else {

		partition_config =
		    (uint32_t) ((partition_config & ~PARTITION_ID_MASK) | id);
		arg = EMMC_SWITCH_PARTITION_CONFIG | (partition_config << 8);

		result = emmc_set_ext_csd(arg);
	}

	return result;
}

static void set_sd_clk(uint32_t clkDiv)
{
	uint32_t dataL;

	dataL = (GETR_32(SD_CLK_CTRL) & (~SD_CLK_CTRL_CLKDIV_MASK));

	switch (clkDiv) {
	case 1:
		dataL |= 0x000000FFU;
		break;		/* 1/1   */
	case 2:
		dataL |= 0x00000000U;
		break;		/* 1/2   */
	case 4:
		dataL |= 0x00000001U;
		break;		/* 1/4   */
	case 8:
		dataL |= 0x00000002U;
		break;		/* 1/8   */
	case 16:
		dataL |= 0x00000004U;
		break;		/* 1/16  */
	case 32:
		dataL |= 0x00000008U;
		break;		/* 1/32  */
	case 64:
		dataL |= 0x00000010U;
		break;		/* 1/64  */
	case 128:
		dataL |= 0x00000020U;
		break;		/* 1/128 */
	case 256:
		dataL |= 0x00000040U;
		break;		/* 1/256 */
	case 512:
		dataL |= 0x00000080U;
		break;		/* 1/512 */
	}

	SETR_32(SD_CLK_CTRL, dataL);
	mmc_drv_obj.current_freq = (uint32_t) clkDiv;
}

static void emmc_get_partition_access(void)
{
	uint32_t reg;
	EMMC_ERROR_CODE result;

	reg = mmio_read_32(RCAR_PRR) & (PRR_PRODUCT_MASK | PRR_CUT_MASK);
	if ((reg == PRR_PRODUCT_H3_CUT20) || (reg == PRR_PRODUCT_M3_CUT11)) {
		SETR_32(SD_OPTION, 0x000060EEU);	/* 8 bits width */
		/* CMD8 (EXT_CSD) */
		emmc_make_trans_cmd(CMD8_SEND_EXT_CSD, 0x00000000U,
				    (uint32_t *) (&mmc_drv_obj.ext_csd_data[0]),
				    EMMC_MAX_EXT_CSD_LENGTH,
				    HAL_MEMCARD_READ, HAL_MEMCARD_NOT_DMA);
		mmc_drv_obj.get_partition_access_flag = TRUE;
		result =
		    emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
		mmc_drv_obj.get_partition_access_flag = FALSE;
		if (result == EMMC_SUCCESS) {
			mmc_drv_obj.partition_access =
			    (EMMC_PARTITION_ID) (mmc_drv_obj.ext_csd_data[179]
						 & PARTITION_ID_MASK);
		} else if (result == EMMC_ERR_CMD_TIMEOUT) {
			mmc_drv_obj.partition_access = PARTITION_ID_BOOT_1;
		} else {
			emmc_write_error_info(EMMC_FUNCNO_GET_PERTITION_ACCESS,
					      result);
			panic();
		}
		SETR_32(SD_OPTION, 0x0000C0EEU);	/* Initialize */
	}
}

static uint32_t emmc_calc_tran_speed(uint32_t *freq)
{
	const uint32_t unit[8] = { 10000U, 100000U, 1000000U, 10000000U,
				   0U, 0U, 0U, 0U }; /* frequency unit (1/10) */
	const uint32_t mult[16] = { 0U, 10U, 12U, 13U, 15U, 20U, 26U, 30U, 35U,
				    40U, 45U, 52U, 55U, 60U, 70U, 80U };
	uint32_t tran_speed = EMMC_CSD_TRAN_SPEED();
	uint32_t max_freq;
	uint32_t result;

	/*
	 * tran_speed = 0x32
	 * unit[tran_speed&0x7] = uint[0x2] = 1000000
	 * mult[(tran_speed&0x78)>>3] = mult[0x30>>3] = mult[6] = 26
	 * 1000000 * 26 = 26000000 (26MHz)
	 */

	result = 1;
	max_freq =
	    unit[tran_speed & EMMC_TRANSPEED_FREQ_UNIT_MASK] *
	    mult[(tran_speed & EMMC_TRANSPEED_MULT_MASK) >>
		 EMMC_TRANSPEED_MULT_SHIFT];

	if (max_freq == 0) {
		result = 0;
	} else if (max_freq >= MMC_FREQ_52MHZ) {
		*freq = MMC_52MHZ;
	} else if (max_freq >= MMC_FREQ_26MHZ) {
		*freq = MMC_26MHZ;
	} else if (max_freq >= MMC_FREQ_20MHZ) {
		*freq = MMC_20MHZ;
	} else {
		*freq = MMC_400KHZ;
	}

	return result;
}

static uint32_t emmc_set_timeout_register_value(uint32_t freq)
{
	uint32_t timeout_cnt;	/* SD_OPTION   - Timeout Counter  */

	switch (freq) {
	case 1U:
		timeout_cnt = 0xE0U;
		break;		/* SDCLK * 2^27 */
	case 2U:
		timeout_cnt = 0xE0U;
		break;		/* SDCLK * 2^27 */
	case 4U:
		timeout_cnt = 0xD0U;
		break;		/* SDCLK * 2^26 */
	case 8U:
		timeout_cnt = 0xC0U;
		break;		/* SDCLK * 2^25 */
	case 16U:
		timeout_cnt = 0xB0U;
		break;		/* SDCLK * 2^24 */
	case 32U:
		timeout_cnt = 0xA0U;
		break;		/* SDCLK * 2^23 */
	case 64U:
		timeout_cnt = 0x90U;
		break;		/* SDCLK * 2^22 */
	case 128U:
		timeout_cnt = 0x80U;
		break;		/* SDCLK * 2^21 */
	case 256U:
		timeout_cnt = 0x70U;
		break;		/* SDCLK * 2^20 */
	case 512U:
		timeout_cnt = 0x70U;
		break;		/* SDCLK * 2^20 */
	default:
		timeout_cnt = 0xE0U;
		break;		/* SDCLK * 2^27 */
	}

	return timeout_cnt;
}

EMMC_ERROR_CODE emmc_set_ext_csd(uint32_t arg)
{
	EMMC_ERROR_CODE result;

	/* CMD6 */
	emmc_make_nontrans_cmd(CMD6_SWITCH, arg);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		return result;
	}

	/* CMD13 */
	emmc_make_nontrans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		return result;
	}

	/* CMD8 (EXT_CSD) */
	emmc_make_trans_cmd(CMD8_SEND_EXT_CSD, 0x00000000,
			    (uint32_t *) (&mmc_drv_obj.ext_csd_data[0]),
			    EMMC_MAX_EXT_CSD_LENGTH, HAL_MEMCARD_READ,
			    HAL_MEMCARD_NOT_DMA);
	result = emmc_exec_cmd(EMMC_R1_ERROR_MASK, mmc_drv_obj.response);
	if (result != EMMC_SUCCESS) {
		return result;
	}
	return EMMC_SUCCESS;
}

EMMC_ERROR_CODE emmc_set_request_mmc_clock(uint32_t freq)
{
	/* state check */
	if ((mmc_drv_obj.initialize != TRUE)
	    || (mmc_drv_obj.card_power_enable != TRUE)) {
		emmc_write_error_info(EMMC_FUNCNO_SET_CLOCK, EMMC_ERR_STATE);
		return EMMC_ERR_STATE;
	}

	/* clock is already running in the desired frequency. */
	if ((mmc_drv_obj.clock_enable == TRUE)
	    && (mmc_drv_obj.current_freq == freq)) {
		return EMMC_SUCCESS;
	}

	/* busy check */
	if ((GETR_32(SD_INFO2) & SD_INFO2_CBSY) != 0) {
		emmc_write_error_info(EMMC_FUNCNO_SET_CLOCK,
				      EMMC_ERR_CARD_BUSY);
		return EMMC_ERR;
	}

	emmc_clock_ctrl(FALSE);	/* clock off */

	set_sd_clk(freq);
	mmc_drv_obj.clock_enable = FALSE;

	return emmc_clock_ctrl(TRUE);	/* clock on */
}

EMMC_ERROR_CODE rcar_emmc_mount(void)
{
	EMMC_ERROR_CODE result;

	/* state check */
	if ((mmc_drv_obj.initialize != TRUE)
	    || (mmc_drv_obj.card_power_enable != TRUE)
	    || ((GETR_32(SD_INFO2) & SD_INFO2_CBSY) != 0)
	    ) {
		emmc_write_error_info(EMMC_FUNCNO_MOUNT, EMMC_ERR_STATE);
		return EMMC_ERR_STATE;
	}

	/* initialize card (IDLE state --> Transfer state) */
	result = emmc_card_init();
	if (result != EMMC_SUCCESS) {
		emmc_write_error_info_func_no(EMMC_FUNCNO_CARD_INIT);
		if (emmc_clock_ctrl(FALSE) != EMMC_SUCCESS) {
			/* nothing to do. */
		}
		return result;
	}

	/* initialize bus for max transfer speed */
	result = emmc_init_bus();
	if (result != EMMC_SUCCESS) {
		if (emmc_clock_ctrl(FALSE) != EMMC_SUCCESS) {
			/* nothing to do. */
		}
		return result;
	}

	/* mount complete */
	mmc_drv_obj.mount = TRUE;

	return EMMC_SUCCESS;
}
