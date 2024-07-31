/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef	MFIS_REGISTER_H__
#define	MFIS_REGISTER_H__

/* MFIS registers */

/* MFIS base address */
#define	MFIS_BASE	(0xE6260000U)

/* MFIS Error Control0 Register */
#define	MFIS_MFIERRCTLR0	(MFIS_BASE + 0x0200U)
/* MFIS Error Control1 Register */
#define	MFIS_MFIERRCTLR1	(MFIS_BASE + 0x0204U)
/* MFIS Error Control2 Register */
#define	MFIS_MFIERRCTLR2	(MFIS_BASE + 0x0208U)
/* MFIS Error Control3 Register */
#define	MFIS_MFIERRCTLR3	(MFIS_BASE + 0x020CU)
/* MFIS Error Control4 Register */
#define	MFIS_MFIERRCTLR4	(MFIS_BASE + 0x0210U)
/* MFIS Error Control5 Register */
#define	MFIS_MFIERRCTLR5	(MFIS_BASE + 0x0214U)
/* MFIS Error Control6 Register */
#define	MFIS_MFIERRCTLR6	(MFIS_BASE + 0x0218U)
/* MFIS Error Control7 Register */
#define	MFIS_MFIERRCTLR7	(MFIS_BASE + 0x0260U)
/* MFIS Error Control8 Register */
#define	MFIS_MFIERRCTLR8	(MFIS_BASE + 0x026CU)
/* MFIS Error Control9 Register */
#define	MFIS_MFIERRCTLR9	(MFIS_BASE + 0x0804U)
/* MFIS Error Control10 Register */
#define	MFIS_MFIERRCTLR10	(MFIS_BASE + 0x0808U)
/* MFIS Error Control11 Register */
#define	MFIS_MFIERRCTLR11	(MFIS_BASE + 0x080CU)

/* MFIS Error Target0 Register */
#define	MFIS_MFIERRTGTR0	(MFIS_BASE + 0x0280U)
/* MFIS Error Target1 Register */
#define	MFIS_MFIERRTGTR1	(MFIS_BASE + 0x0284U)
/* MFIS Error Target2 Register */
#define	MFIS_MFIERRTGTR2	(MFIS_BASE + 0x0288U)
/* MFIS Error Target3 Register */
#define	MFIS_MFIERRTGTR3	(MFIS_BASE + 0x028CU)
/* MFIS Error Target4 Register */
#define	MFIS_MFIERRTGTR4	(MFIS_BASE + 0x0290U)
/* MFIS Error Target5 Register */
#define	MFIS_MFIERRTGTR5	(MFIS_BASE + 0x0294U)
/* MFIS Error Target6 Register */
#define	MFIS_MFIERRTGTR6	(MFIS_BASE + 0x025CU)
/* MFIS Error Target7 Register */
#define	MFIS_MFIERRTGTR7	(MFIS_BASE + 0x0268U)
/* MFIS Error Target8 Register */
#define	MFIS_MFIERRTGTR8	(MFIS_BASE + 0x0274U)
/* MFIS Error Target9 Register */
#define	MFIS_MFIERRTGTR9	(MFIS_BASE + 0x081CU)
/* MFIS Error Target10 Register */
#define	MFIS_MFIERRTGTR10	(MFIS_BASE + 0x0820U)
/* MFIS Error Target11 Register */
#define	MFIS_MFIERRTGTR11	(MFIS_BASE + 0x0824U)

/* External Error Request Hold Control Register */
#define	MFIS_MFIEXTRQHLDCNTR	(MFIS_BASE + 0x08A0U)

/* Write Protection Control Register */
#define	MFIS_MFISWPCNTR		(MFIS_BASE + 0x0900U)

#endif	/* MFIS_REGISTER_H__ */
