/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#define RCAR_SA0_SIZE_SMALL	(0U)	/* for E3 */
#define RCAR_SA0_SIZE_NORMAL	(1U)	/* for H3/M3/M3N */

#define CR7_LOADER_ADDRESS	(0xE6304000U)	/* CR7_Loader start address */

#if (RCAR_SA0_SIZE == RCAR_SA0_SIZE_SMALL)
/* CR7_Loader size is 80KB(0x00005000) */
#define CR7_LOADER_SIZE	(80U*1024U/4U)

#else  /* (RCAR_SA0_SIZE == RCAR_SA0_SIZE_SMALL) */
/* CR7_Loader size is 170KB(0x0000AA00) */
#define CR7_LOADER_SIZE	(170U*1024U/4U)

#endif /* (RCAR_SA0_SIZE == RCAR_SA0_SIZE_SMALL) */

/* SA0 */
/* 0x00000000 */
const unsigned int __attribute__ ((section(".sa0_bootrom")))
		bootrom_paramA = 0x00000100U;
/* 0x00000C00 */
/* 0x00000D54 */
const unsigned int __attribute__ ((section(".sa0_dst_addr1")))
		dst_addr1 = CR7_LOADER_ADDRESS;
/* 0x00000E64 */
const unsigned int __attribute__ ((section(".sa0_dst_size1")))
		dst_size1 = CR7_LOADER_SIZE;

