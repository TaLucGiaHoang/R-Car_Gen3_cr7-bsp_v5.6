/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "debug.h"
#include "mmio.h"
#include "rcar_def.h"
#include "dma_driver.h"
#include "single_ecc.h"
#include "single_ecc_conf.h"
#include "single_ecc_reg.h"

#if RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE

/* MFIS setting */
#define MFIERRCTLR4_BUS_EDC		((uint32_t)1U << 24U)
#define MFIERRCTLR8_ECC_MULTI_BIT	((uint32_t)1U << 24U)
#define MFIERRCTLR8_ECC_1_BIT		((uint32_t)1U << 23U)
#define BUS_EDC_MASK			(0x01000000U)		/* bit24 */
#define ECC_MULTI_BIT_MASK		(0x01000000U)		/* bit24 */
#define ECC_1_BIT_MASK			(0x00800000U)		/* bit23 */

/* EDC setting */
#define EDCEN0_MEM			((uint32_t)1U << 15U)
#define EDCINTEN1_DRAM			((uint32_t)7U << 29U) /* bit[31:29]=1 */

/* Bit shift */
#define ADDR_32BIT_SHIFT		(32U)
#define ADDR_12BIT_SHIFT		(12U)
#define SIZE_24BIT_SHIFT		(24U)

/* DRAM Fusa size calculate */
#define DSIZE_CALC(n)			((uint32_t)1U << (20U + (n)))

/* System RAM Fusa size calculate */
#define SSIZE_CALC(n)			((uint32_t)1U << (12U + (n)))

/* Strings of ECC mode log output */
#define STR_8BIT_DATA_5BIT_ECC		"8bit Data/5bit ECC"
#define STR_64BIT_DATA_8BIT_ECC		"64bit Data/8bit ECC"

/* Strings of 0fill log output */
#define DO_FILL				"Fill with 0"
#define NO_FILL				"No fill"

/* compile option definitions */
#define SINGLE_CHANNEL_ECC_ENABLE	(1U)

struct single_ecc_table {
	uint64_t	fusa_addr;
	uint64_t	ecc_addr;
	uint32_t	enable;
	uint32_t	fusa_size;
	uint32_t	ecc_mode;
	uint32_t	fusa_fill;
};

void single_ecc_init(void)
{
/*
 * DRAM Single Channel ECC setteing array
 * setting values are configured in single_ecc_conf.h
 */
static const struct single_ecc_table dram_ecc_table[DRAM_AREA_NUM] = {
	{DFUSA0_SADDR, DECC0_SADDR, DFUSA0_EN,
	 DFUSA0_SIZE, DECC0_MODE, DFUSA0_FILL},
	{DFUSA1_SADDR, DECC1_SADDR, DFUSA1_EN,
	 DFUSA1_SIZE, DECC1_MODE, DFUSA1_FILL},
	{DFUSA2_SADDR, DECC2_SADDR, DFUSA2_EN,
	 DFUSA2_SIZE, DECC2_MODE, DFUSA2_FILL},
	{DFUSA3_SADDR, DECC3_SADDR, DFUSA3_EN,
	 DFUSA3_SIZE, DECC3_MODE, DFUSA3_FILL},
	{DFUSA4_SADDR, DECC4_SADDR, DFUSA4_EN,
	 DFUSA4_SIZE, DECC4_MODE, DFUSA4_FILL},
	{DFUSA5_SADDR, DECC5_SADDR, DFUSA5_EN,
	 DFUSA5_SIZE, DECC5_MODE, DFUSA5_FILL},
	{DFUSA6_SADDR, DECC6_SADDR, DFUSA6_EN,
	 DFUSA6_SIZE, DECC6_MODE, DFUSA6_FILL},
	{DFUSA7_SADDR, DECC7_SADDR, DFUSA7_EN,
	 DFUSA7_SIZE, DECC7_MODE, DFUSA7_FILL},
	{DFUSA8_SADDR, DECC8_SADDR, DFUSA8_EN,
	 DFUSA8_SIZE, DECC8_MODE, DFUSA8_FILL},
	{DFUSA9_SADDR, DECC9_SADDR, DFUSA9_EN,
	 DFUSA9_SIZE, DECC9_MODE, DFUSA9_FILL},
	{DFUSA10_SADDR, DECC10_SADDR, DFUSA10_EN,
	 DFUSA10_SIZE, DECC10_MODE, DFUSA10_FILL},
	{DFUSA11_SADDR, DECC11_SADDR, DFUSA11_EN,
	 DFUSA11_SIZE, DECC11_MODE, DFUSA11_FILL},
	{DFUSA12_SADDR, DECC12_SADDR, DFUSA12_EN,
	 DFUSA12_SIZE, DECC12_MODE, DFUSA12_FILL},
	{DFUSA13_SADDR, DECC13_SADDR, DFUSA13_EN,
	 DFUSA13_SIZE, DECC13_MODE, DFUSA13_FILL},
	{DFUSA14_SADDR, DECC14_SADDR, DFUSA14_EN,
	 DFUSA14_SIZE, DECC14_MODE, DFUSA14_FILL},
	{DFUSA15_SADDR, DECC15_SADDR, DFUSA15_EN,
	 DFUSA15_SIZE, DECC15_MODE, DFUSA15_FILL},
};

/*
 * System RAM Single Channel ECC setteing array
 * setting values are configured in single_ecc_conf.h
 */
static const struct single_ecc_table sram_ecc_table[SRAM_AREA_NUM] = {
	{SFUSA0_SADDR, SECC0_SADDR, SFUSA0_EN,
	 SFUSA0_SIZE, SECC0_MODE, SFUSA0_FILL},
	{SFUSA1_SADDR, SECC1_SADDR, SFUSA1_EN,
	 SFUSA1_SIZE, SECC1_MODE, SFUSA1_FILL},
	{SFUSA2_SADDR, SECC2_SADDR, SFUSA2_EN,
	 SFUSA2_SIZE, SECC2_MODE, SFUSA2_FILL},
	{SFUSA3_SADDR, SECC3_SADDR, SFUSA3_EN,
	 SFUSA3_SIZE, SECC3_MODE, SFUSA3_FILL},
	{SFUSA4_SADDR, SECC4_SADDR, SFUSA4_EN,
	 SFUSA4_SIZE, SECC4_MODE, SFUSA4_FILL},
	{SFUSA5_SADDR, SECC5_SADDR, SFUSA5_EN,
	 SFUSA5_SIZE, SECC5_MODE, SFUSA5_FILL},
	{SFUSA6_SADDR, SECC6_SADDR, SFUSA6_EN,
	 SFUSA6_SIZE, SECC6_MODE, SFUSA6_FILL},
	{SFUSA7_SADDR, SECC7_SADDR, SFUSA7_EN,
	 SFUSA7_SIZE, SECC7_MODE, SFUSA7_FILL},
};

	uint32_t reg;
	uint32_t i;
	char msg[128];
	const char *str;

	/*
	 * Error Detection Setting
	 * Bus EDC error
	 */
	reg = mmio_read_32(MFIS_MFIERRTGTR4);
	reg = (reg & ~BUS_EDC_MASK);
	mmio_write_32(MFIS_MFIERRTGTR4, reg);

	reg = mmio_read_32(MFIS_MFIERRCTLR4);
	reg = (reg | MFIERRCTLR4_BUS_EDC);
	mmio_write_32(MFIS_MFIERRCTLR4, reg);

	/* ECC Multi-bit/1bit error */
	reg = mmio_read_32(MFIS_MFIERRTGTR8);
	reg = ((reg & ~ECC_MULTI_BIT_MASK) & ~ECC_1_BIT_MASK);
	mmio_write_32(MFIS_MFIERRTGTR8, reg);

	reg = mmio_read_32(MFIS_MFIERRCTLR8);
	reg = (reg | MFIERRCTLR8_ECC_MULTI_BIT | MFIERRCTLR8_ECC_1_BIT);
	mmio_write_32(MFIS_MFIERRCTLR8, reg);

	/* log output */
	NOTICE("Single Channel ECC enable\n");

	/* DRAM Single Channel ECC setting */
	for (i = 0U; i < DRAM_AREA_NUM; i++) {
		if (dram_ecc_table[i].enable == SINGLE_ECC_ENABLE) {
			/* write to DFUSAAREACRn */
			reg = (dram_ecc_table[i].enable
				| (dram_ecc_table[i].fusa_size
					<< SIZE_24BIT_SHIFT)
				| (uint32_t)(dram_ecc_table[i].fusa_addr
					>> ADDR_12BIT_SHIFT));
			mmio_write_32(DFUSAAREACR(i), reg);
			/* write to DECCAREACRn */
			reg = (dram_ecc_table[i].ecc_mode
				| (uint32_t)(dram_ecc_table[i].ecc_addr
					>> ADDR_12BIT_SHIFT));
			mmio_write_32(DECCAREACR(i), reg);

			str = NO_FILL;
			if (dram_ecc_table[i].fusa_fill == FUSA_0FILL_ENABLE) {
			/* fill DRAM Fusa Area with 0 to generate ECC code */
				exec_fillDMA(dram_ecc_table[i].fusa_addr,
				DSIZE_CALC(dram_ecc_table[i].fusa_size));
				str = DO_FILL;
			}

			/* output Fusa Area top address and size */
			(void)sprintf(msg, "DRAM Fusa Area[%d]=0x%x_%x"
					   " Size=0x%x %s\n",
				i,
				(uint32_t)(dram_ecc_table[i].fusa_addr
					>> ADDR_32BIT_SHIFT),
				(uint32_t)(dram_ecc_table[i].fusa_addr),
				DSIZE_CALC(dram_ecc_table[i].fusa_size),
				str);
			NOTICE("%s", msg);

			/* output ECC Area top address and ECC Mode */
			if (dram_ecc_table[i].ecc_mode == MODE_8_5) {
				/* 8bit data / 5bit ECC mode */
				str = STR_8BIT_DATA_5BIT_ECC;
			} else {
				/* 64bit data / 8bit ECC mode */
				str = STR_64BIT_DATA_8BIT_ECC;
			}
			(void)sprintf(msg, "DRAM ECC  Area[%d]=0x%x_%x"
					   " Mode=%s\n",
				i,
				(uint32_t)(dram_ecc_table[i].ecc_addr
					>> ADDR_32BIT_SHIFT),
				(uint32_t)(dram_ecc_table[i].ecc_addr),
				str);
			NOTICE("%s", msg);
		}
	}

	/* SRAM Single Channel ECC setting */
	for (i = 0U; i < SRAM_AREA_NUM; i++) {
		if (sram_ecc_table[i].enable == SINGLE_ECC_ENABLE) {
			/* write to SFUSAAREACRn */
			reg = (sram_ecc_table[i].enable
				| (sram_ecc_table[i].fusa_size
					<< SIZE_24BIT_SHIFT)
				| (uint32_t)(sram_ecc_table[i].fusa_addr
					>> ADDR_12BIT_SHIFT));
			mmio_write_32(SFUSAAREACR(i), reg);
			/* write to SECCAREACRn */
			reg = (sram_ecc_table[i].ecc_mode
				| (uint32_t)(sram_ecc_table[i].ecc_addr
					>> ADDR_12BIT_SHIFT));
			mmio_write_32(SECCAREACR(i), reg);

			str = NO_FILL;
			if (sram_ecc_table[i].fusa_fill == FUSA_0FILL_ENABLE) {
			/* fill SystemRAM Fusa Area with 0 to create ECC code */
				exec_fill_sram(sram_ecc_table[i].fusa_addr,
				SSIZE_CALC(sram_ecc_table[i].fusa_size));
				str = DO_FILL;
			}

			/* output Fusa Area top address and size */
			(void)sprintf(msg, "SRAM Fusa Area[%d]=0x%x"
					   " Size=0x%x %s\n",
				i,
				(uint32_t)(sram_ecc_table[i].fusa_addr),
				SSIZE_CALC(sram_ecc_table[i].fusa_size),
				str);
			NOTICE("%s", msg);

			/* output ECC Area top address and ECC Mode */
			if (sram_ecc_table[i].ecc_mode == MODE_8_5) {
				/* 8bit data / 5bit ECC mode */
				str = STR_8BIT_DATA_5BIT_ECC;
			} else {
				/* 64bit data / 8bit ECC mode */
				str = STR_64BIT_DATA_8BIT_ECC;
			}
			(void)sprintf(msg, "SRAM ECC  Area[%d]=0x%x"
					   " Mode=%s\n",
				i,
				(uint32_t)(sram_ecc_table[i].ecc_addr),
				str);
			NOTICE("%s", msg);
		}
	}

	/* Error Detection Circuit enable */
	reg = mmio_read_32(EDC_EDCEN0);
	reg = (reg | EDCEN0_MEM);
	mmio_write_32(EDC_EDCEN0, reg);

	/* EDC Interrupt enable */
	reg = mmio_read_32(EDC_EDCINTEN1);
	reg = (reg | EDCINTEN1_DRAM);
	mmio_write_32(EDC_EDCINTEN1, reg);
}

#endif /* RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE */

