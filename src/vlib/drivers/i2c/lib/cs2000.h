/*************************************************************************************************************
* i2c_cs2000_h
* Copyright (c) [2020-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_AK4613_H_
#define R_AK4613_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* CS2000-CP Ratio value */
							/* Ratio0 = output_clk/input_clk * 2^20 */
#define RATIO_49152000Hz		0x00200000	/* [out=49.1520MHz][in=24.5760MHz] : 49152000/24576000 * 2^20 = 2097152   = 	0x00200000 (96.0kHz*256*2) */
#define RATIO_24576000Hz		0x00100000	/* [out=24.5760MHz][in=24.5760MHz] : 24576000/24576000 * 2^20 = 1048576   = 	0x00100000 (48.0kHz*256*2) */
#define RATIO_12288000Hz		0x00080000	/* [out=12.2880MHz][in=24.5760MHz] : 12288000/24576000 * 2^20 =  524288   = 	0x00080000 (24.0kHz*256*2) */
#define RATIO_45158400Hz		0x001D6666	/* [out=45.1584MHz][in=24.5760MHz] : 45158400/24576000 * 2^20 = 1926758.4 = 	0x001D6666 (88.2kHz*256*2) */
#define RATIO_22579200Hz		0x000EB333	/* [out=22.5792MHz][in=24.5760MHz] : 22579200/24576000 * 2^20 =  963379.2 = 	0x000EB333 (44.1kHz*256*2) */
#define RATIO_11289600Hz		0x00075999	/* [out=11.2896MHz][in=24.5760MHz] : 11289600/24576000 * 2^20 =  481689.6 = 	0x00075999 (22.05kHz*256*2) */

void StartCS2000CP(uint32_t ratio);

#ifdef __cplusplus
}
#endif

#endif /* R_AK4613_H_ */