/*
 * Copyright (c) 2015-2020, Renesas Electronics Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>

#include <lib/mmio.h>

#include "emmc_config.h"
#include "emmc_def.h"
#include "emmc_hal.h"
#include "emmc_registers.h"
#include "emmc_std.h"
#include "rcar_def.h"
#include "util_def.h"

static EMMC_ERROR_CODE emmc_trans_sector(uint32_t *buff_address_virtual);
static EMMC_ERROR_CODE emmc_scc_error_check();
static void emmc_scc_reset();
static EMMC_ERROR_CODE emmc_scc_auto_correction(void);
static EMMC_ERROR_CODE emmc_scc_manual_correction(void);

uint32_t emmc_interrupt(void)
{
	EMMC_ERROR_CODE result;
	uint32_t prr_data;
	uint32_t cut_ver;
	uint32_t end_bit = (1 << mmc_drv_obj.dma_tranend1);

	/* SD_INFO */
	mmc_drv_obj.error_info.info1 = GETR_32(SD_INFO1);
	mmc_drv_obj.error_info.info2 = GETR_32(SD_INFO2);

	/* SD_INFO EVENT */
	mmc_drv_obj.int_event1 =
	    mmc_drv_obj.error_info.info1 & GETR_32(SD_INFO1_MASK);
	mmc_drv_obj.int_event2 =
	    mmc_drv_obj.error_info.info2 & GETR_32(SD_INFO2_MASK);

	/* ERR_STS */
	mmc_drv_obj.error_info.status1 = GETR_32(SD_ERR_STS1);
	mmc_drv_obj.error_info.status2 = GETR_32(SD_ERR_STS2);

	/* DM_CM_INFO */
	mmc_drv_obj.error_info.dm_info1 = GETR_32(DM_CM_INFO1);
	mmc_drv_obj.error_info.dm_info2 = GETR_32(DM_CM_INFO2);

	/* DM_CM_INFO EVENT */
	mmc_drv_obj.dm_event1 =
	    mmc_drv_obj.error_info.dm_info1 & GETR_32(DM_CM_INFO1_MASK);
	mmc_drv_obj.dm_event2 =
	    mmc_drv_obj.error_info.dm_info2 & GETR_32(DM_CM_INFO2_MASK);

	/* ERR SD_INFO2 */
	if ((SD_INFO2_ALL_ERR & mmc_drv_obj.int_event2) != 0) {
		SETR_32(SD_INFO1_MASK, 0x00000000U);	/* interrupt disable */
		SETR_32(SD_INFO2_MASK, SD_INFO2_CLEAR);	/* interrupt disable */
		SETR_32(SD_INFO1, 0x00000000U);	/* interrupt clear */
		SETR_32(SD_INFO2, SD_INFO2_CLEAR);	/* interrupt clear */
		mmc_drv_obj.state_machine_blocking = FALSE;
	}

	/* PIO Transfer */
	/* BWE/BRE */
	else if (((SD_INFO2_BWE | SD_INFO2_BRE) & mmc_drv_obj.int_event2)) {
		/* BWE */
		if (SD_INFO2_BWE & mmc_drv_obj.int_event2) {
			SETR_32(SD_INFO2, (GETR_32(SD_INFO2) & ~SD_INFO2_BWE));
		}
		/* BRE */
		else {
			SETR_32(SD_INFO2, (GETR_32(SD_INFO2) & ~SD_INFO2_BRE));
		}

		result = emmc_trans_sector(mmc_drv_obj.buff_address_virtual);
		mmc_drv_obj.remain_size = 0;

		if (result != EMMC_SUCCESS) {
			/* data transfer error */
			emmc_write_error_info(EMMC_FUNCNO_NONE, result);

			/* Panic */
			SETR_32(SD_INFO1_MASK, 0x00000000U);
			SETR_32(SD_INFO2_MASK, SD_INFO2_CLEAR);
			SETR_32(SD_INFO1, 0x00000000U);
			/* interrupt clear */
			SETR_32(SD_INFO2, SD_INFO2_CLEAR);
			mmc_drv_obj.force_terminate = TRUE;
		} else {
			mmc_drv_obj.during_transfer = FALSE;
		}
		mmc_drv_obj.state_machine_blocking = FALSE;
	}

	/* DMA_TRANSFER */
	/* DM_CM_INFO1: DMA-ch0 transfer complete or error occurred */
	else if ((BIT16 & mmc_drv_obj.dm_event1) != 0) {
		SETR_32(DM_CM_INFO1, 0x00000000U);
		SETR_32(DM_CM_INFO2, 0x00000000U);
		/* interrupt clear */
		SETR_32(SD_INFO2, (GETR_32(SD_INFO2) & ~SD_INFO2_BWE));
		/* DM_CM_INFO2:  DMA-ch0 error occurred */
		if ((BIT16 & mmc_drv_obj.dm_event2) != 0) {
			mmc_drv_obj.dma_error_flag = TRUE;
		} else {
			mmc_drv_obj.during_dma_transfer = FALSE;
			mmc_drv_obj.during_transfer = FALSE;
		}
		/* wait next interrupt */
		mmc_drv_obj.state_machine_blocking = FALSE;
	}
	/* DM_CM_INFO1: DMA-ch1 transfer complete or error occurred */
	else if ((end_bit & mmc_drv_obj.dm_event1) != 0U) {
		SETR_32(DM_CM_INFO1, 0x00000000U);
		SETR_32(DM_CM_INFO2, 0x00000000U);
		/* interrupt clear */
		SETR_32(SD_INFO2, (GETR_32(SD_INFO2) & ~SD_INFO2_BRE));
		/* DM_CM_INFO2: DMA-ch1 error occurred */
		if ((BIT17 & mmc_drv_obj.dm_event2) != 0) {
			mmc_drv_obj.dma_error_flag = TRUE;
		} else {
			mmc_drv_obj.during_dma_transfer = FALSE;
			mmc_drv_obj.during_transfer = FALSE;
		}
		/* wait next interrupt */
		mmc_drv_obj.state_machine_blocking = FALSE;
	}

	/* Response end  */
	else if ((SD_INFO1_INFO0 & mmc_drv_obj.int_event1) != 0) {
		/* interrupt clear */
		SETR_32(SD_INFO1, (GETR_32(SD_INFO1) & ~SD_INFO1_INFO0));
		mmc_drv_obj.state_machine_blocking = FALSE;
	}
	/* Access end  */
	else if ((SD_INFO1_INFO2 & mmc_drv_obj.int_event1) != 0) {
		/* interrupt clear */
		SETR_32(SD_INFO1, (GETR_32(SD_INFO1) & ~SD_INFO1_INFO2));
		mmc_drv_obj.state_machine_blocking = FALSE;
	} else {
		/* nothing to do. */
	}

	/* Check SCC status */
	if (emmc_scc_error_check() != EMMC_SUCCESS) {
		emmc_scc_reset();
		/* This indicates to perform retune */
		mmc_drv_obj.tuning_status = EMMC_NEED_RETUNING;
	}

	return (uint32_t) 0;
}

static EMMC_ERROR_CODE emmc_trans_sector(uint32_t *buff_address_virtual)
{
	uint32_t length, i;
	uint64_t *bufPtrLL;

	if (buff_address_virtual == NULL) {
		return EMMC_ERR_PARAM;
	}

	if ((mmc_drv_obj.during_transfer != TRUE)
	    || (mmc_drv_obj.remain_size == 0)) {
		return EMMC_ERR_STATE;
	}

	bufPtrLL = (uint64_t *) buff_address_virtual;
	length = mmc_drv_obj.remain_size;

	/* data transefer */
	for (i = 0; i < (length >> 3); i++) {
		/* Write */
		if (mmc_drv_obj.cmd_info.dir == HAL_MEMCARD_WRITE) {
			if (((i %
				(uint32_t) (EMMC_BLOCK_LENGTH >>
					EMMC_BUF_SIZE_SHIFT)) == 0U)
				&& (i != 0U)) {
				/* BWE check */
				while (((GETR_32(SD_INFO2)) & SD_INFO2_BWE) ==
					0U) {
					/* ERROR check */
					if (((GETR_32(SD_INFO2)) &
						SD_INFO2_ALL_ERR) != 0U) {
						return EMMC_ERR_TRANSFER;
					}
				}
				/* BWE clear */
				SETR_32(SD_INFO2,
					(uint32_t) (GETR_32(SD_INFO2) &
							~SD_INFO2_BWE));
			}
			SETR_64(SD_BUF0, *bufPtrLL);	/* buffer --> FIFO */
		}
		/* Read */
		else {
			/* Checks when the read data reaches SD_SIZE. */
			/* The BRE bit is cleared at emmc_interrupt function. */
			if (((i %
			      (uint32_t) (EMMC_BLOCK_LENGTH >>
					  EMMC_BUF_SIZE_SHIFT)) == 0U)
			    && (i != 0U)) {
				/* BRE check */
				while (((GETR_32(SD_INFO2)) & SD_INFO2_BRE) ==
				       0U) {
					/* ERROR check */
					if (((GETR_32(SD_INFO2)) &
					     SD_INFO2_ALL_ERR) != 0U) {
						return EMMC_ERR_TRANSFER;
					}
				}
				/* BRE clear */
				SETR_32(SD_INFO2,
					(uint32_t) (GETR_32(SD_INFO2) &
						    ~SD_INFO2_BRE));
			}
			*bufPtrLL = GETR_64(SD_BUF0);	/* FIFO --> buffer */
		}
		bufPtrLL++;
	}

	return EMMC_SUCCESS;
}

static EMMC_ERROR_CODE emmc_scc_error_check(void)
{
	if (!(mmc_drv_obj.hs_timing == TIMING_HS200 || mmc_drv_obj.hs_timing == TIMING_HS400)) {
		return EMMC_SUCCESS;
	}

	if (mmc_drv_obj.tuning_status == EMMC_TUNING_DOING || 
		mmc_drv_obj.tuning_status == EMMC_NEED_RETUNING) {
		return EMMC_SUCCESS;
	}

	if (mmc_drv_obj.int_event2 & (SD_INFO2_ERR0 | SD_INFO2_ERR1 | SD_INFO2_ERR2 | SD_INFO2_ERR3 | SD_INFO2_ERR6)) {
		/* Clear SCC error */
		SETR_32(SCC_RVSREQ, 0);
		ERROR("[eMMC] ERROR SD_INFO2 0x%08x , Need retune\n", mmc_drv_obj.int_event2);
		return EMMC_ERR; /* Need re-tune */
	}

    if (GETR_32(SCC_RVSCNTL) & SCC_RVSCNTL_RVSEN) {
        /* Automatic correction */
        return emmc_scc_auto_correction();
    } else {
        /* Manual correction */
        return emmc_scc_manual_correction();
    }
}

static void emmc_scc_reset(void)
{
	/* Stop clock */
	SETR_32(SD_CLK_CTRL, GETR_32(SD_CLK_CTRL) & ~SD_CLK_CTRL_SCLKEN);

    /* Reset SCC */
    SETR_32(SCC_CKSEL, ~SCC_CKSEL_DTSEL & GETR_32(SCC_CKSEL));
    SETR_32(SCC_DTCNTL, ~SCC_DTCNTL_TAPEN & GETR_32(SCC_DTCNTL));

    /* Start clock */
    SETR_32(SD_CLK_CTRL, GETR_32(SD_CLK_CTRL) | SD_CLK_CTRL_SCLKEN);

    SETR_32(SCC_RVSCNTL, ~SCC_RVSCNTL_RVSEN & GETR_32(SCC_RVSCNTL));
    SETR_32(SCC_RVSCNTL, ~SCC_RVSCNTL_RVSEN & GETR_32(SCC_RVSCNTL));

    mmc_drv_obj.tuning_status = EMMC_TUNING_NONE;
}

static EMMC_ERROR_CODE emmc_scc_manual_correction(void)
{
    uint32_t      rvsreq;
    uint32_t      smpcmp;
    uint32_t      new_tap = mmc_drv_obj.tap_set;
    uint32_t      err_tap = mmc_drv_obj.tap_set;

    rvsreq = GETR_32(SCC_RVSREQ);

    /* No error */
    if(!rvsreq)
        return EMMC_SUCCESS;

    SETR_32(SCC_RVSREQ, 0);

    /* Change TAP position according to correction status */
    if (mmc_drv_obj.hs400_ignore_dat_correction && mmc_drv_obj.hs_timing == TIMING_HS400) {
        /*
         * Correction Error Status contains CMD and DAT signal status.
         * In HS400, DAT signal based on DS signal, not CLK.
         * Therefore, use only CMD status.
         */
        smpcmp = GETR_32(SCC_SMPCMP) & SCC_SMPCMP_CMD_ERR;

        switch (smpcmp) {
        case 0:
            return EMMC_SUCCESS;   /* No error in CMD signal */
        case SCC_SMPCMP_CMD_REQUP:
            new_tap = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num + 1) % mmc_drv_obj.tap_num;
            err_tap = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num - 1) % mmc_drv_obj.tap_num;
            break;
        case SCC_SMPCMP_CMD_REQDOWN:
            new_tap = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num - 1) % mmc_drv_obj.tap_num;
            err_tap = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num + 1) % mmc_drv_obj.tap_num;
            break;
        default:
            return EMMC_ERR;   /* Need re-tune */
        }

        if (mmc_drv_obj.hs400_bad_tap & (1 << new_tap)) {
            /* New tap is bad tap (cannot change).
               Compare  with HS200 tunning result, if smpcmp[err_tap] is OK, perform retune */

            if (mmc_drv_obj.smpcmp[err_tap])
                return EMMC_ERR;   /* Need re-tune */

            return EMMC_SUCCESS;   /* cannot change */
        }

        mmc_drv_obj.tap_set = new_tap;
    } else {
        if (rvsreq & SCC_RVSREQ_RVSERR) {
            return EMMC_ERR;    /* Need re-tune */
        } else if (rvsreq & SCC_RVSREQ_REQTAPUP) {
            mmc_drv_obj.tap_set = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num + 1) % mmc_drv_obj.tap_num;
        } else if (rvsreq & SCC_RVSREQ_REQTAPDWN) {
            mmc_drv_obj.tap_set = (mmc_drv_obj.tap_set + mmc_drv_obj.tap_num - 1) % mmc_drv_obj.tap_num;
        } else {
            return EMMC_SUCCESS;
        }
    }

    /* Set TAP position */
    if (mmc_drv_obj.hs400_use_4tap) {
        SETR_32(SCC_TAPSET, mmc_drv_obj.tap_set / 2);
    } else {
        SETR_32(SCC_TAPSET, mmc_drv_obj.tap_set);
    }

    return EMMC_SUCCESS;
}

static EMMC_ERROR_CODE emmc_scc_auto_correction(void)
{
    uint32_t     rvsreq;

    /* Check SCC error */
    if ( (rvsreq = GETR_32(SCC_RVSREQ)) & SCC_RVSREQ_RVSERR) {
        /* Clear SCC error */
        SETR_32(SCC_RVSREQ, 0);

        return EMMC_ERR; /* Need re-tune */
    }

    return EMMC_SUCCESS; /* No error */
}