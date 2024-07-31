/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */
SECTIONS
{
	. = 0x00000000;
	.rodata : {
		. = 0x00000154;
		KEEP(*(.sa3_rots_addr))
		. = 0x00000264;
		KEEP(*(.sa3_rtos_size))
		. = 0x00000954;
		KEEP(*(.sa3_bl2loader_addr))
		. = 0x00000A64;
		KEEP(*(.sa3_bl2loader_size))
	}

}
