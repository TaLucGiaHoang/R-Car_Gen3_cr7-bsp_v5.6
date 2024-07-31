/*
 * Copyright (c) 2017-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

static uint64_t mstat_fix[] = {
	/* 0x0000, */ 0x0000000000000000UL,
	/* 0x0008, */ 0x0000000000000000UL,
	/* 0x0010, */ 0x0000000000000000UL,
	/* 0x0018, */ 0x0000000000000000UL,
	/* 0x0020, */ 0x0000000000000000UL,
	/* 0x0028, */ 0x0000000000000000UL,
	/* 0x0030, */ 0x001008630000FFFFUL,
	/* 0x0038, */ 0x001008630000FFFFUL,
	/* 0x0040, */ 0x001418BA0000FFFFUL,
	/* 0x0048, */ 0x0000000000000000UL,
	/* 0x0050, */ 0x001404070000FFFFUL,
	/* 0x0058, */ 0x001415270000FFFFUL,
	/* 0x0060, */ 0x001415270000FFFFUL,
	/* 0x0068, */ 0x0000000000000000UL,
	/* 0x0070, */ 0x001404070000FFFFUL,
	/* 0x0078, */ 0x0000000000000000UL,
	/* 0x0080, */ 0x0000000000000000UL,
	/* 0x0088, */ 0x0014083E0000FFFFUL,
	/* 0x0090, */ 0x0000000000000000UL,
	/* 0x0098, */ 0x0000000000000000UL,
	/* 0x00a0, */ 0x000C08390000FFFFUL,
	/* 0x00a8, */ 0x000C04110000FFFFUL,
	/* 0x00b0, */ 0x000C04150000FFFFUL,
	/* 0x00b8, */ 0x0000000000000000UL,
	/* 0x00c0, */ 0x000C08390000FFFFUL,
	/* 0x00c8, */ 0x000C04110000FFFFUL,
	/* 0x00d0, */ 0x000C04150000FFFFUL,
	/* 0x00d8, */ 0x0000000000000000UL,
	/* 0x00e0, */ 0x0000000000000000UL,
	/* 0x00e8, */ 0x0000000000000000UL,
	/* 0x00f0, */ 0x001045080000FFFFUL,
	/* 0x00f8, */ 0x000C0C9E0000FFFFUL,
	/* 0x0100, */ 0x0000000000000000UL,
	/* 0x0108, */ 0x0000000000000000UL,
	/* 0x0110, */ 0x001015080000FFFFUL,
	/* 0x0118, */ 0x000C43CB0000FFFFUL,
	/* 0x0120, */ 0x0000000000000000UL,
	/* 0x0128, */ 0x0000000000000000UL,
	/* 0x0130, */ 0x0000000000000000UL,
	/* 0x0138, */ 0x0010194A0000FFFFUL,
	/* 0x0140, */ 0x0000000000000000UL,
	/* 0x0148, */ 0x0000000000000000UL,
	/* 0x0150, */ 0x00101D910000FFFFUL,
	/* 0x0158, */ 0x0000000000000000UL,
	/* 0x0160, */ 0x0010194A0000FFFFUL,
	/* 0x0168, */ 0x0000000000000000UL,
	/* 0x0170, */ 0x0000000000000000UL,
	/* 0x0178, */ 0x00100CA50000FFFFUL,
	/* 0x0180, */ 0x0000000000000000UL,
	/* 0x0188, */ 0x0000000000000000UL,
	/* 0x0190, */ 0x001037390000FFFFUL,
	/* 0x0198, */ 0x0000000000000000UL,
	/* 0x01a0, */ 0x0010194A0000FFFFUL,
	/* 0x01a8, */ 0x0000000000000000UL,
	/* 0x01b0, */ 0x0000000000000000UL,
	/* 0x01b8, */ 0x0000000000000000UL,
	/* 0x01c0, */ 0x000C04010000FFFFUL,
	/* 0x01c8, */ 0x000C04010000FFFFUL,
	/* 0x01d0, */ 0x0000000000000000UL,
	/* 0x01d8, */ 0x0000000000000000UL,
	/* 0x01e0, */ 0x0000000000000000UL,
	/* 0x01e8, */ 0x000C04020000FFFFUL,
	/* 0x01f0, */ 0x000C04090000FFFFUL,
	/* 0x01f8, */ 0x0000000000000000UL,
	/* 0x0200, */ 0x0000000000000000UL,
	/* 0x0208, */ 0x000C04090000FFFFUL,
	/* 0x0210, */ 0x000C04090000FFFFUL,
	/* 0x0218, */ 0x0000000000000000UL,
	/* 0x0220, */ 0x0000000000000000UL,
	/* 0x0228, */ 0x0000000000000000UL,
	/* 0x0230, */ 0x0000000000000000UL,
	/* 0x0238, */ 0x0000000000000000UL,
	/* 0x0240, */ 0x0000000000000000UL,
	/* 0x0248, */ 0x0000000000000000UL,
	/* 0x0250, */ 0x0000000000000000UL,
	/* 0x0258, */ 0x0000000000000000UL,
	/* 0x0260, */ 0x000C0C2A0000FFFFUL,
	/* 0x0268, */ 0x001410040000FFFFUL,
	/* 0x0270, */ 0x001404020000FFFFUL,
	/* 0x0278, */ 0x000C08110000FFFFUL,
	/* 0x0280, */ 0x0000000000000000UL,
	/* 0x0288, */ 0x0000000000000000UL,
	/* 0x0290, */ 0x001410040000FFFFUL,
	/* 0x0298, */ 0x001404020000FFFFUL,
	/* 0x02a0, */ 0x000C04090000FFFFUL,
	/* 0x02a8, */ 0x000C04090000FFFFUL,
	/* 0x02b0, */ 0x00140C090000FFFFUL,
	/* 0x02b8, */ 0x000C04020000FFFFUL,
	/* 0x02c0, */ 0x0000000000000000UL,
	/* 0x02c8, */ 0x0000000000000000UL,
	/* 0x02d0, */ 0x000C04090000FFFFUL,
	/* 0x02d8, */ 0x000C04090000FFFFUL,
	/* 0x02e0, */ 0x00140C090000FFFFUL,
	/* 0x02e8, */ 0x000C04020000FFFFUL,
	/* 0x02f0, */ 0x0000000000000000UL,
	/* 0x02f8, */ 0x0000000000000000UL,
	/* 0x0300, */ 0x0000000000000000UL,
	/* 0x0308, */ 0x0000000000000000UL,
	/* 0x0310, */ 0x0000000000000000UL,
	/* 0x0318, */ 0x0000000000000000UL,
	/* 0x0320, */ 0x0000000000000000UL,
	/* 0x0328, */ 0x0000000000000000UL,
	/* 0x0330, */ 0x0000000000000000UL,
	/* 0x0338, */ 0x0000000000000000UL,
	/* 0x0340, */ 0x0000000000000000UL,
	/* 0x0348, */ 0x0000000000000000UL,
	/* 0x0350, */ 0x0000000000000000UL,
	/* 0x0358, */ 0x0000000000000000UL,
	/* 0x0360, */ 0x0000000000000000UL,
	/* 0x0368, */ 0x0000000000000000UL,
	/* 0x0370, */ 0x000C04020000FFFFUL,
	/* 0x0378, */ 0x000C04020000FFFFUL,
	/* 0x0380, */ 0x000C04090000FFFFUL,
	/* 0x0388, */ 0x000C04090000FFFFUL,
	/* 0x0390, */ 0x0000000000000000UL,
};

static uint64_t mstat_be[] = {
	/* 0x0000, */ 0x0000000000000000UL,
	/* 0x0008, */ 0x0000000000000000UL,
	/* 0x0010, */ 0x0000000000000000UL,
	/* 0x0018, */ 0x0000000000000000UL,
	/* 0x0020, */ 0x0000000000000000UL,
	/* 0x0028, */ 0x0012001005E03401UL,
	/* 0x0030, */ 0x0000000000000000UL,
	/* 0x0038, */ 0x0000000000000000UL,
	/* 0x0040, */ 0x0000000000000000UL,
	/* 0x0048, */ 0x0000000000000000UL,
	/* 0x0050, */ 0x0000000000000000UL,
	/* 0x0058, */ 0x0000000000000000UL,
	/* 0x0060, */ 0x0000000000000000UL,
	/* 0x0068, */ 0x0000000000000000UL,
	/* 0x0070, */ 0x0000000000000000UL,
	/* 0x0078, */ 0x0000000000000000UL,
	/* 0x0080, */ 0x0000000000000000UL,
	/* 0x0088, */ 0x0000000000000000UL,
	/* 0x0090, */ 0x0000000000000000UL,
	/* 0x0098, */ 0x0000000000000000UL,
	/* 0x00a0, */ 0x0000000000000000UL,
	/* 0x00a8, */ 0x0000000000000000UL,
	/* 0x00b0, */ 0x0000000000000000UL,
	/* 0x00b8, */ 0x0000000000000000UL,
	/* 0x00c0, */ 0x0000000000000000UL,
	/* 0x00c8, */ 0x0000000000000000UL,
	/* 0x00d0, */ 0x0000000000000000UL,
	/* 0x00d8, */ 0x0000000000000000UL,
	/* 0x00e0, */ 0x0000000000000000UL,
	/* 0x00e8, */ 0x0000000000000000UL,
	/* 0x00f0, */ 0x0000000000000000UL,
	/* 0x00f8, */ 0x0000000000000000UL,
	/* 0x0100, */ 0x0000000000000000UL,
	/* 0x0108, */ 0x0000000000000000UL,
	/* 0x0110, */ 0x0000000000000000UL,
	/* 0x0118, */ 0x0000000000000000UL,
	/* 0x0120, */ 0x0000000000000000UL,
	/* 0x0128, */ 0x0000000000000000UL,
	/* 0x0130, */ 0x0000000000000000UL,
	/* 0x0138, */ 0x0000000000000000UL,
	/* 0x0140, */ 0x0000000000000000UL,
	/* 0x0148, */ 0x0000000000000000UL,
	/* 0x0150, */ 0x0000000000000000UL,
	/* 0x0158, */ 0x0000000000000000UL,
	/* 0x0160, */ 0x0000000000000000UL,
	/* 0x0168, */ 0x0000000000000000UL,
	/* 0x0170, */ 0x0000000000000000UL,
	/* 0x0178, */ 0x0000000000000000UL,
	/* 0x0180, */ 0x0000000000000000UL,
	/* 0x0188, */ 0x0000000000000000UL,
	/* 0x0190, */ 0x0000000000000000UL,
	/* 0x0198, */ 0x0000000000000000UL,
	/* 0x01a0, */ 0x0000000000000000UL,
	/* 0x01a8, */ 0x0000000000000000UL,
	/* 0x01b0, */ 0x0000000000000000UL,
	/* 0x01b8, */ 0x0000000000000000UL,
	/* 0x01c0, */ 0x0021060005EFFC01UL,
	/* 0x01c8, */ 0x0021060005EFFC01UL,
	/* 0x01d0, */ 0x0000000000000000UL,
	/* 0x01d8, */ 0x0000000000000000UL,
	/* 0x01e0, */ 0x0000000000000000UL,
	/* 0x01e8, */ 0x0000000000000000UL,
	/* 0x01f0, */ 0x0021010005E79401UL,
	/* 0x01f8, */ 0x0000000000000000UL,
	/* 0x0200, */ 0x0000000000000000UL,
	/* 0x0208, */ 0x0000000000000000UL,
	/* 0x0210, */ 0x0021010005E79401UL,
	/* 0x0218, */ 0x0011010005E79401UL,
	/* 0x0220, */ 0x0011010005E79401UL,
	/* 0x0228, */ 0x0000000000000000UL,
	/* 0x0230, */ 0x0011010005E79401UL,
	/* 0x0238, */ 0x0011010005E79401UL,
	/* 0x0240, */ 0x0012010005E79401UL,
	/* 0x0248, */ 0x0011010005E79401UL,
	/* 0x0250, */ 0x0012010005E79401UL,
	/* 0x0258, */ 0x0011010005E79401UL,
	/* 0x0260, */ 0x0000000000000000UL,
	/* 0x0268, */ 0x0000000000000000UL,
	/* 0x0270, */ 0x0000000000000000UL,
	/* 0x0278, */ 0x0000000000000000UL,
	/* 0x0280, */ 0x0000000000000000UL,
	/* 0x0288, */ 0x0000000000000000UL,
	/* 0x0290, */ 0x0000000000000000UL,
	/* 0x0298, */ 0x0000000000000000UL,
	/* 0x02a0, */ 0x0000000000000000UL,
	/* 0x02a8, */ 0x0000000000000000UL,
	/* 0x02b0, */ 0x0000000000000000UL,
	/* 0x02b8, */ 0x0000000000000000UL,
	/* 0x02c0, */ 0x0000000000000000UL,
	/* 0x02c8, */ 0x0000000000000000UL,
	/* 0x02d0, */ 0x0000000000000000UL,
	/* 0x02d8, */ 0x0000000000000000UL,
	/* 0x02e0, */ 0x0000000000000000UL,
	/* 0x02e8, */ 0x0000000000000000UL,
	/* 0x02f0, */ 0x0011060005EFFC01UL,
	/* 0x02f8, */ 0x0011060005EFFC01UL,
	/* 0x0300, */ 0x0000000000000000UL,
	/* 0x0308, */ 0x0000000000000000UL,
	/* 0x0310, */ 0x0000000000000000UL,
	/* 0x0318, */ 0x0012001005E03401UL,
	/* 0x0320, */ 0x0000000000000000UL,
	/* 0x0328, */ 0x0000000000000000UL,
	/* 0x0330, */ 0x0000000000000000UL,
	/* 0x0338, */ 0x0000000000000000UL,
	/* 0x0340, */ 0x0000000000000000UL,
	/* 0x0348, */ 0x0000000000000000UL,
	/* 0x0350, */ 0x0000000000000000UL,
	/* 0x0358, */ 0x0012060005EFFC01UL,
	/* 0x0360, */ 0x0012060005EFFC01UL,
	/* 0x0368, */ 0x0012001005E03401UL,
	/* 0x0370, */ 0x0000000000000000UL,
	/* 0x0378, */ 0x0000000000000000UL,
	/* 0x0380, */ 0x0000000000000000UL,
	/* 0x0388, */ 0x0000000000000000UL,
	/* 0x0390, */ 0x0012001005E03401UL,
};
