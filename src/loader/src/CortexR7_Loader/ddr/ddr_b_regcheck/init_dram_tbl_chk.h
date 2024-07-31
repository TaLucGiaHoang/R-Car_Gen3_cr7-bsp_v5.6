/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

static const uint32_t DDR_PHY_SLICE_REGCHK[4][4] = {
{/* H3 Ver.1.x */
	/* 0400-0457 */
	/* 0400-041f */	0xF9BFBFBF,
	/* 0420-043f */	0x800F0138,
	/* 0440-045f */	0x00E2E007,
	/* 0460-047f */	0x00000000
}
,
{/* M3-W  */
	/* 0800-0858 */
	/* 0800-081f */	0xF17FFF7E,
	/* 0820-083f */	0x8007013A,
	/* 0840-085f */	0x00F8C007,
	/* 0860-087f */	0x00000000
}
,
{/* H3 (other) */
	/* 0400-0460 */
	/* 0400-041f */	0x87FFEEFE,
	/* 0420-043f */	0x81E011D7,
	/* 0440-045f */	0xE16007C1,
	/* 0460-047f */	0x00000000
}
,
{/* M3-N/V3H */
	/* 0800-0864 */
	/* 0800-081f */	0x0FFFDEFE,
	/* 0820-083f */	0x078047AF,
	/* 0840-085f */	0xA5801F06,
	/* 0860-087f */	0x00000017
}
};

static const uint32_t DDR_PHY_ADR_V_REGCHK[4][2] = {
{/* H3 Ver.1.x */
	/* 0600-0624 */
	/* 0600-061f */	0x6FF7AFF7,
	/* 0620-063f */	0x00000018,
}
,
{/* M3-W */
	/* 0a00-0a24 */
	/* 0a00-0a1f */	0x6FF7AFF7,
	/* 0a20-0a3f */	0x00000018,
}
,
{/* H3 (other) */
	/* 0600-0624 */
	/* 0600-061f */	0x6FF7BFF7,
	/* 0620-063f */	0x00000018,
}
,
{/* M3-N/V3H */
	/* 0a00-0a24 */
	/* 0a00-0a1f */	0x6FFFBFF7,
	/* 0a20-0a3f */	0x00000018,
}
};

static const uint32_t DDR_PHY_ADR_G_REGCHK[4][3] = {
{/* H3 Ver.1.x*/
	/* 0700-073a */
	/* 0700-071f */	0x3E03DCFF,
	/* 0720-073f */	0x07FCFEA8,
	/* 0740-075f */	0x00000000,
}
,
{/* M3-W */
	/* 0b80-0bbf */
	/* 0b80-0b9f */	0xE31FFFFE,
	/* 0ba0-0bbf */	0x6FCFEA83,
	/* 0bc0-0bdf */	0x00002000,
}
,
{/* H3 (other) */
	/* 0680-06ce */
	/* 0680-069f */	0x1FFFFFFD,
	/* 06a0-06bf */	0x9FAA83E3,
	/* 06c0-06df */	0x00001CFD,
}
,
{/* M3-N/V3H */
	/* 0b80-0bd6 */
	/* 0b80-0b9f */	0x1FFFFDFF,
	/* 0ba0-0bbf */	0x6EAA0FC7,
	/* 0bc0-0bdf */	0x005CDFF6,
}
};


static const uint32_t DDR_PI_REGCHK[4][9] = {
{/* H3 Ver.1.x */
	/* 0200-02b4 */
	/* 0200-021f */	0xFFFFFFFE,
	/* 0220-023f */	0xFFFF15FF,
	/* 0240-025f */	0xFFF2DEFF,
	/* 0260-027f */	0x403B7FFF,
	/* 0280-029f */	0xF7FFFD88,
	/* 02a0-02bf */	0x001FBEFF,
	/* 02c0-02df */	0x00000000,
	/* 02e0-02ff */	0x00000000,
	/* 0300-031f */	0x00000000,
}
,
{/* M3-W */
	/* 0800-02c9 */
	/* 0200-021f */	0xFFBFFFFE,
	/* 0220-023f */	0xFFFC35FF,
	/* 0240-025f */	0xFFCF7DFF,
	/* 0260-027f */	0x4060FFFF,
	/* 0280-029f */	0xFFB02060,
	/* 02a0-02bf */	0xDFDFFDFF,
	/* 02c0-02df */	0x000003F7,
	/* 02e0-02ff */	0x00000000,
	/* 0300-031f */	0x00000000,
}
,
{/* H3 (other)*/
	/* 0200-02f4 */
	/* 0200-021f */	0xFEFFFFFE,
	/* 0220-023f */	0xFFA057FF,
	/* 0240-025f */	0x7FBF7FFF,
	/* 0260-027f */	0x3FFFFFFC,
	/* 0280-029f */	0xF82108B0,
	/* 02a0-02bf */	0x3DFFDFFF,
	/* 02c0-02df */	0xFFFFFFBF,
	/* 02e0-02ff */	0x000FFFFF,
	/* 0300-031f */	0x00000000,
}
,
{/* M3-N/V3H */
	/* 0200-031d */
	/* 0200-021f */	0xDFFFFFFE,
	/* 0220-023f */	0xE81AFFFF,
	/* 0240-025f */	0xFF7FFFFF,
	/* 0260-027f */	0xFFFFE7BF,
	/* 0280-029f */	0xC1601FFF,
	/* 02a0-02bf */	0xFEC0C0C0,
	/* 02c0-02df */	0x7DFFDFFF,
	/* 02e0-02ff */	0xFFFFFF7E,
	/* 0300-031f */	0x3EB5FFFF,
}
};

