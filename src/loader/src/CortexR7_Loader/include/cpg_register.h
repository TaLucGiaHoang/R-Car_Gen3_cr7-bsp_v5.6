/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef	CPG_REGISTER_H__
#define	CPG_REGISTER_H__

/* CPG base address */
#define	CPG_BASE	(0xE6150000U)

/* CPG Realtime module stop control 0 */
#define CPG_RMSTPCR0	(CPG_BASE + 0x0110U)
/* CPG module stop status 0 */
#define CPG_MSTPSR0	(CPG_BASE + 0x0030U)

/* CPG write protect */
#define CPG_CPGWPR	(CPG_BASE + 0x0900U)
/* CPG write protect control */
#define CPG_CPGWPCR	(CPG_BASE + 0x0904U)
/* CPG realtime module stop control 9 */
#define CPG_RMSTPCR9    (CPG_BASE + 0x0984U)
/* CPG module stop status 9 */
#define CPG_MSTPSR9     (CPG_BASE + 0x09A4U)

/* CPG (REALTIME) registers */

/* Realtime Module Stop Control Register 0 */
#define	RMSTPCR0	(CPG_BASE + 0x0110U)
/* Realtime Module Stop Control Register 1 */
#define	RMSTPCR1	(CPG_BASE + 0x0114U)
/* Realtime Module Stop Control Register 2 */
#define	RMSTPCR2	(CPG_BASE + 0x0118U)
/* Realtime Module Stop Control Register 3 */
#define	RMSTPCR3	(CPG_BASE + 0x011CU)
/* Realtime Module Stop Control Register 4 */
#define	RMSTPCR4	(CPG_BASE + 0x0120U)
/* Realtime Module Stop Control Register 5 */
#define	RMSTPCR5	(CPG_BASE + 0x0124U)
/* Realtime Module Stop Control Register 6 */
#define	RMSTPCR6	(CPG_BASE + 0x0128U)
/* Realtime Module Stop Control Register 7 */
#define	RMSTPCR7	(CPG_BASE + 0x012CU)
/* Realtime Module Stop Control Register 8 */
#define	RMSTPCR8	(CPG_BASE + 0x0980U)
/* Realtime Module Stop Control Register 9 */
#define	RMSTPCR9	(CPG_BASE + 0x0984U)
/* Realtime Module Stop Control Register 10 */
#define	RMSTPCR10	(CPG_BASE + 0x0988U)
/* Realtime Module Stop Control Register 11 */
#define	RMSTPCR11	(CPG_BASE + 0x098CU)


#endif	/* CPG_REGISTER_H__ */
