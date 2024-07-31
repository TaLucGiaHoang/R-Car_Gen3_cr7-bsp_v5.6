/*
 * Copyright (c) 2015-2022, Renesas Electronics Corporation.
 * All rights reserved.
 */

#define RCAR_SA0_SIZE_SMALL	(0U)	/* for E3 */
#define RCAR_SA0_SIZE_NORMAL	(1U)	/* for H3/M3/M3N */

#define RTOS_ADDRESS	(0x70000000U)	/* RTOS start address */
#define RTOS_SIZE	(0x00600000U)	/* RTOS size is 24MB */
#define BL2_ADDRESS	(0xE6304000U)	/* BL2 start address */

#if (RCAR_SA0_SIZE == RCAR_SA0_SIZE_SMALL)
/* BL2 size is 80KB(0x00005000) */
#define BL2_SIZE	(80U*1024U/4U)

#else  /* (RCAR_SA0_SIZE == RCAR_SA0_SIZE_SMALL) */
/* BL2 size is 170KB(0x0000AA00) */
#define BL2_SIZE	(170U*1024U/4U)

#endif /* (RCAR_SA0_SIZE == RCAR_SA0_SIZE_SMALL) */

/* SA3 */
/* 0x000C0154 */
const unsigned int __attribute__ ((section(".sa3_rots_addr")))
		rtos_addr = RTOS_ADDRESS;
/* 0x000C0264 */
const unsigned int __attribute__ ((section(".sa3_rtos_size")))
		rtos_size = RTOS_SIZE;
/* 0x000C0954 */
const unsigned int __attribute__ ((section(".sa3_bl2loader_addr")))
		bl2loader_addr = BL2_ADDRESS;
/* 0x000C0A64 */
const unsigned int __attribute__ ((section(".sa3_bl2loader_size")))
		bl2loader_size = BL2_SIZE;

