/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef DMA_REGISTER_H__
#define DMA_REGISTER_H__

#include "cpg_register.h"

#define	DMACH	(uint32_t)(1U)	/* DMA CH setting (1-15) */

#define CPG_RMSTPCR0	(CPG_BASE + 0x0110U)
#define RT_DMAC_BIT	(0x00200000U)
#define DMA_BASE	(0xFFC10000U)

/* DMA operation 0 */
#define DMA_RDMOR	(DMA_BASE + 0x0060U)
/* DMA secure control */
#define	DMA_RDMSEC	(DMA_BASE + 0x0030U)
/* DMA channel clear */
#define DMA_RDMCHCLR	(DMA_BASE + 0x0080U)
/* DMA source address */
#define DMA_RDMSAR	(DMA_BASE + (DMACH * 0x80U) + 0x8000U)
/* DMA destination address */
#define DMA_RDMDAR	(DMA_BASE + (DMACH * 0x80U) + 0x8004U)
/* DMA transfer count */
#define DMA_RDMTCR	(DMA_BASE + (DMACH * 0x80U) + 0x8008U)
/* DMA channel control */
#define DMA_RDMCHCR	(DMA_BASE + (DMACH * 0x80U) + 0x800CU)
/* DMA fixed destination address */
#define DMA_RDMFIXDAR	(DMA_BASE + (DMACH * 0x80U) + 0x8014U)

#define DMA_RDMSAR2	(DMA_BASE + (2U * 0x80U) + 0x8000U)
/* DMA destination address */
#define DMA_RDMDAR2	(DMA_BASE + (2U * 0x80U) + 0x8004U)
/* DMA transfer count */
#define DMA_RDMTCR2	(DMA_BASE + (2U * 0x80U) + 0x8008U)
/* DMA channel control */
#define DMA_RDMCHCR2	(DMA_BASE + (2U * 0x80U) + 0x800CU)
/* DMA fixed destination address */
#define DMA_RDMFIXDAR2	(DMA_BASE + (2U * 0x80U) + 0x8014U)

#define DMA_RDMSAR3	(DMA_BASE + (3U * 0x80U) + 0x8000U)
/* DMA destination address */
#define DMA_RDMDAR3	(DMA_BASE + (3U * 0x80U) + 0x8004U)
/* DMA transfer count */
#define DMA_RDMTCR3	(DMA_BASE + (3U * 0x80U) + 0x8008U)
/* DMA channel control */
#define DMA_RDMCHCR3	(DMA_BASE + (3U * 0x80U) + 0x800CU)
/* DMA fixed destination address */
#define DMA_RDMFIXDAR3	(DMA_BASE + (3U * 0x80U) + 0x8014U)

#define DMA_RDMSAR4	(DMA_BASE + (4U * 0x80U) + 0x8000U)
/* DMA destination address */
#define DMA_RDMDAR4	(DMA_BASE + (4U * 0x80U) + 0x8004U)
/* DMA transfer count */
#define DMA_RDMTCR4	(DMA_BASE + (4U * 0x80U) + 0x8008U)
/* DMA channel control */
#define DMA_RDMCHCR4	(DMA_BASE + (4U * 0x80U) + 0x800CU)
/* DMA fixed destination address */
#define DMA_RDMFIXDAR4	(DMA_BASE + (4U * 0x80U) + 0x8014U)

#define DMA_RDMSAR5	(DMA_BASE + (5U * 0x80U) + 0x8000U)
/* DMA destination address */
#define DMA_RDMDAR5	(DMA_BASE + (5U * 0x80U) + 0x8004U)
/* DMA transfer count */
#define DMA_RDMTCR5	(DMA_BASE + (5U * 0x80U) + 0x8008U)
/* DMA channel control */
#define DMA_RDMCHCR5	(DMA_BASE + (5U * 0x80U) + 0x800CU)

#define DMA_RDMSAR6	(DMA_BASE + (6U * 0x80U) + 0x8000U)
/* DMA destination address */
#define DMA_RDMDAR6	(DMA_BASE + (6U * 0x80U) + 0x8004U)
/* DMA transfer count */
#define DMA_RDMTCR6	(DMA_BASE + (6U * 0x80U) + 0x8008U)
/* DMA channel control */
#define DMA_RDMCHCR6	(DMA_BASE + (6U * 0x80U) + 0x800CU)

#endif	/* DMA_REGISTER_H__ */

