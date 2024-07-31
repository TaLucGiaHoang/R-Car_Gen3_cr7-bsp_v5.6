/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "string.h"
#include "mmio.h"
#include "cpg_register.h"
#include "cpg_init.h"
#include "dma_register.h"
#include "dma_driver.h"
#include "debug.h"
#include "rpc_driver.h"

#define DMA_PARALLEL_CH		(4U)

#define RDMOR_INITIAL		(0x0301U)
#define RDMTCR_CNT_SHIFT	(6U)
#define RDMCHCR_TRN_MODE	(0x00105409U)
#define RDMCHCR_FILL_TRN_MODE	(0x00104409U)
#define RDMCHCR_TE_BIT		(0x00000002U)
#define RDMCHCR_CAE_BIT		(0x80000000U)
#define RDMFIXDAR_DAR_SHIFT	(32U)
#define DAR_MASK		(0xFFU)

#define LOWER_32BIT_DST_ADDR	(0xFFFFFFFFU)
#define DST_ADDR_32BIT_SHIFT	(32U)

#define FILLDMA_FRACTION_SHIFT	(6U)

#define FILL_DATA_NUM_8BYTE(n)	((uint32_t)(n) >> 3U)
#define SRAM_FILL_ADDR(n)	((uint64_t)(n) * 8UL)

/* fraction mask for 256-byte units */
#define	DMA_FRACTION_MASK	(0xFFU)
/* transfer size units */
#define	DMA_SIZE_UNIT		(0x0100U)

static void enableDMA(void);
static void setupDMA(void);
static void startDMA(uint32_t dst, uint32_t src, uint32_t len);
static void endDMA(void);
static void start_fillDMA(const uint64_t *dst,
			  const uint32_t *len, uint32_t tbl_size);
static void end_fillDMA(uint32_t ch);

static void enableDMA(void)
{
	/* Enable clock supply to DMAC */
	mstpcr_write(CPG_RMSTPCR0, CPG_MSTPSR0, RT_DMAC_BIT);
}

static void setupDMA(void)
{
	/* DMA channel clear */
	mmio_write_32(DMA_RDMCHCLR, ((uint32_t)1U << DMACH));
}

static void startDMA(uint32_t dst, uint32_t src, uint32_t len)
{

	uint32_t reg;

	/* DMA operation */
	mmio_write_16(DMA_RDMOR, RDMOR_INITIAL);
	/* DMA secure control register */
	reg = mmio_read_32(DMA_RDMSEC);
	reg |= ((uint32_t)1U << DMACH);
	mmio_write_32(DMA_RDMSEC, reg);
	/* DMA destination address */
	mmio_write_32(DMA_RDMDAR, dst);
	/* DMA source address */
	mmio_write_32(DMA_RDMSAR, src);
	/* DMA 64bytes-unit transfer  count */
	mmio_write_32(DMA_RDMTCR, (len >> RDMTCR_CNT_SHIFT));
	/* DMA channel control */
	mmio_write_32(DMA_RDMCHCR, RDMCHCR_TRN_MODE);
}

static void endDMA(void)
{
	uint32_t reg;

	/* DMA channel control */
	while ((mmio_read_32(DMA_RDMCHCR) & RDMCHCR_TE_BIT) == 0x00000000U) {
		/* DMA channel control 32 */
		if ((mmio_read_32(DMA_RDMCHCR) & RDMCHCR_CAE_BIT) != 0U) {
			ERROR("DMA - Channel Address Error\n");
			panic();
			break;
		}
	}

	/* wait RPC transfer end */
	wait_endRPC();

	/* DMA channel clear 2 */
	mmio_write_32(DMA_RDMCHCLR, ((uint32_t)1U << DMACH));
	reg = mmio_read_32(DMA_RDMSEC);
	reg &= (uint32_t)(~((uint32_t)1U << DMACH));
	mmio_write_32(DMA_RDMSEC, reg);
}

void initDMA(void)
{
	enableDMA();
	setupDMA();
}

void execDMA(uint32_t dst, uint32_t src, uint32_t len)
{
	uint32_t	dmalen;

	if ((len & DMA_FRACTION_MASK) != 0U) {
		dmalen = (len + DMA_SIZE_UNIT) &
			~((uint32_t)DMA_FRACTION_MASK);
	} else {
		dmalen = len;
	}

	if ((src & DMA_FRACTION_MASK) != 0U) {
		/* source address is not 256bytes aligned */
		ERROR("DMA - Source address invalid\n" \
		      "      source address  = 0x%x\n", src);
		panic();
	}
	if ((dst & DMA_FRACTION_MASK) != 0U) {
		/* destination address is not 256bytes aligned */
		ERROR("DMA - Destination address invalid\n" \
		      "      destination address = 0x%x\n", dst);
		panic();
	}

	startDMA(dst, src, dmalen);
	endDMA();
}

/* Fill DRAM Fusa area with 0 */

static __attribute__((section(".testdata"))) uint64_t fill_data[8] = {0U};

static void start_fillDMA(const uint64_t *dst,
			  const uint32_t *len, uint32_t tbl_size)
{
	uint32_t reg;
	uint32_t reg_DMA_RDMDAR[] = {DMA_RDMDAR, DMA_RDMDAR2,
				     DMA_RDMDAR3, DMA_RDMDAR4};
	uint32_t reg_DMA_RDMFIXDAR[] = {DMA_RDMFIXDAR, DMA_RDMFIXDAR2,
					DMA_RDMFIXDAR3, DMA_RDMFIXDAR4};
	uint32_t reg_DMA_RDMSAR[] = {DMA_RDMSAR, DMA_RDMSAR2,
				     DMA_RDMSAR3, DMA_RDMSAR4};
	uint32_t reg_DMA_RDMTCR[] = {DMA_RDMTCR, DMA_RDMTCR2,
				     DMA_RDMTCR3, DMA_RDMTCR4};
	uint32_t reg_DMA_RDMCHCR[] = {DMA_RDMCHCR, DMA_RDMCHCR2,
				      DMA_RDMCHCR3, DMA_RDMCHCR4};
	uint32_t loop;

	/* DMA operation */
	mmio_write_16(DMA_RDMOR, RDMOR_INITIAL);
	/* DMA secure control register */
	reg = mmio_read_32(DMA_RDMSEC);
	for (loop = 0U; loop < tbl_size; loop++) {
		reg |= ((uint32_t)1U << (DMACH + loop));
	}
	mmio_write_32(DMA_RDMSEC, reg);

	for (loop = 0U; loop < tbl_size; loop++) {
		/* DMA destination address */
		mmio_write_32(reg_DMA_RDMDAR[loop], (uint32_t)(dst[loop]
					& LOWER_32BIT_DST_ADDR));
		/* DMA fixed destination address */
		mmio_write_32(reg_DMA_RDMFIXDAR[loop], (uint32_t)((dst[loop]
					 >> RDMFIXDAR_DAR_SHIFT) & DAR_MASK));
		/* DMA source address */
		mmio_write_32(reg_DMA_RDMSAR[loop], (uint32_t)&fill_data[0]);
		/* DMA 64bytes-unit transfer  count */
		mmio_write_32(reg_DMA_RDMTCR[loop], (len[loop]
					 >> RDMTCR_CNT_SHIFT));
		/* DMA channel control */
		mmio_write_32(reg_DMA_RDMCHCR[loop], RDMCHCR_FILL_TRN_MODE);
	}
}

static void end_fillDMA(uint32_t ch)
{
	uint32_t reg;
	uint32_t reg_DMA_RDMCHCR[] = {DMA_RDMCHCR, DMA_RDMCHCR2,
				      DMA_RDMCHCR3, DMA_RDMCHCR4};
	uint32_t loop;
	uint32_t result;

	/* DMA channel control */
	while (1) {
		result = ((uint32_t)1U << ch) - 1U;
		for (loop = 0U; loop < ch; loop++) {
			if ((mmio_read_32(reg_DMA_RDMCHCR[loop])
				& RDMCHCR_TE_BIT)
				== RDMCHCR_TE_BIT) {
				result &= ~((uint32_t)1U << loop);
			}
			/* DMA channel control 32 */
			if ((mmio_read_32(reg_DMA_RDMCHCR[loop])
				& RDMCHCR_CAE_BIT)
				!= 0U) {
				ERROR("DMA - Channel Address Error\n");
				panic();
				break;
			}
		}
		if (result == 0U) {
			break;
		}
	}

	reg = mmio_read_32(DMA_RDMSEC);
	for (loop = 0U; loop < ch; loop++) {
		/* DMA channel clear 2 */
		mmio_write_32(DMA_RDMCHCLR, ((uint32_t)1U << (DMACH + loop)));
		reg &= (~(uint32_t)1U << (DMACH + loop));
	}
	mmio_write_32(DMA_RDMSEC, reg);
}

void exec_fillDMA(uint64_t dst, uint32_t len)
{
	uint32_t dmalen[DMA_PARALLEL_CH];
	uint64_t dmadst[DMA_PARALLEL_CH];
	uint32_t rem_len;
	uint32_t loop;

	dmalen[0] = ((len / DMA_PARALLEL_CH) &
			~(uint32_t)(((uint32_t)1U << RDMTCR_CNT_SHIFT) - 1U));
	rem_len = ((len % dmalen[0])
			+ (((uint32_t)1U << RDMTCR_CNT_SHIFT) - 1U))
			& ~(uint32_t)(((uint32_t)1U << RDMTCR_CNT_SHIFT) - 1U);
	dmadst[0] = dst;

	for (loop = 1U; loop < DMA_PARALLEL_CH; loop++) {
		dmalen[loop] = dmalen[0U];
		dmadst[loop] = (dmadst[0U]
				 + ((uint64_t)dmalen[0] * (uint64_t)loop));
	}

	dmalen[DMA_PARALLEL_CH - 1U] += rem_len;

	start_fillDMA(dmadst, dmalen, DMA_PARALLEL_CH);
	end_fillDMA(DMA_PARALLEL_CH);
}

#if RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE
void exec_fill_sram(uint64_t saddr, uint32_t size)
{
	uint32_t i;

	if (((uint32_t)fill_data >= (uint32_t)saddr) &&
	    ((uint32_t)fill_data < ((uint32_t)saddr + size))) {
		/*
		 * If fill_data[] exists inside Fusa area,
		 * fill Fusa area with zero by CPU
		 */
		for (i = 0U; i < FILL_DATA_NUM_8BYTE(size); i++) {
			mmio_write_64((uintptr_t)(saddr + SRAM_FILL_ADDR(i)),
					0UL);
		}
	} else {
	/* Fill Fusa Area with zero by DMA transfer */
		exec_fillDMA(saddr, size);
	}
}
#endif /* RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE */
