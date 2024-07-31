/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */
SECTIONS
{
	. = 0x00000000;
	.rodata : {
		KEEP(*(.sa0_bootrom))
		/* Map Type 2 or Map Type 3 */
		/* A-side IPL content cert "Start Address" */
		. = 0x000001D4;		/* H'00000080 + H'00000154 */
		KEEP(*(.sa0_dst_addr0))
		/* A-side IPL content cert "Size" */
		. = 0x000002E4;		/* H'00000080 + H'00000264 */
		KEEP(*(.sa0_dst_size0))
		/* Map Type 1 */
		/* A-side IPL content cert "Start Address" */
		. = 0x00000D54;		/* H'00000C00 + H'00000154 */
		KEEP(*(.sa0_dst_addr1))
		/* A-side IPL content cert "Size" */
		. = 0x00000E64;		/* H'00000C00 + H'00000264 */
		KEEP(*(.sa0_dst_size1))
	}

}
