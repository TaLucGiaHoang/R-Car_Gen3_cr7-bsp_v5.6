/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef SCIF_H__
#define SCIF_H__


#define	SCIF_INTERNAL_CLK	0	/* Internal clock(S3D4:66.66MHz) */
#define	SCIF_EXTARNAL_CLK	1	/* External clock(SCK2:14.7456MHz) */
#define	SCIF_CLK		SCIF_INTERNAL_CLK


#ifndef __ASSEMBLY
uint32_t console_init(void);
uint32_t console_putc(uint8_t c);
uint32_t console_wait(void);
#endif /* __ASSEMBLY */

#endif	/* __SCIF_H__ */
