/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_audio_common.h"
#include "cs2000.h"

/*
 * sel  : AUDIO_CLKA, AUDIO_CLKB, AUDIO_CLKC
 * div  : 2=1/2, 4=1/4, 6=1/6, ... , 510=1/510, 512=1/512 (can't set 0 or 1)
 */
static void InitAudioClkoutAndBrgA(uint32_t sel, uint32_t div)
{
	/* AUDIO_CLK_A(xxxxxxxMHz) --|     BRGA    |--+-->SSIx */
	/* AUDIO_CLK_B(xxxxxxxMHz) --| x1/2~x1/512 |  +-->AUDIO_CLKOUT3 */
	/* AUDIO_CLK_C(xxxxxxxMHz) --|             | */

	/* Clock Select Register(SSICKR) */
	/* [31]   =1'B0           : BRGA output clock */
	/* [30:23]=8'B01000110    : Reserved (Fixed value) */
	/* [22:20]=3'B000/001/100 : BRGA input clock = AUDIO_CLKA/AUDIO_CLKB/AUDIO_CLKC */
	/* [18:16]=3'B101         : BRGB input clock = Fixed at 0 */
	switch (sel) {
	case AUDIO_CLKA:
		writel(0x23050000, ADG_SSICKR);
		break;  /* AUDIO_CLKA */
	case AUDIO_CLKB:
		writel(0x23150000, ADG_SSICKR);
		break;  /* AUDIO_CLKB */
	case AUDIO_CLKC:
		writel(0x23450000, ADG_SSICKR);
		break;  /* AUDIO_CLKC */
	}

	/* BRGA Baud Rate Setting Register (BRRA) */
	/* [9:8]=2'B00 : ACLK_A */
	/* [7:0]=8'Hxx : division ratio */
	writel((div >> 1)-1, ADG_BRRA);  /* div ratio = 1/(2(N+1)): N=0~255 */

	/* Clock Select Register1 (SSICKR1) */
	/* [27:24]=4'b0000 : AUDIO_CLKOUT3 = BRGA/BRGB select */
	/* [19:16]=4'b0000 : AUDIO_CLKOUT2 = BRGA/BRGB select */
	/* [11:08]=4'b0000 : AUDIO_CLKOUT1 = BRGA/BRGB select */
	/* [03:00]=4'b0000 : AUDIO_CLKOUT  = BRGA/BRGB select */
	writel(0x00000000, ADG_SSICKR1);
}

/*
 * Enable SSIx input clock (Select only BRGA)
 * chNo : SSI0, SSI1, SSI2, SSI3, SSI4, SSI5, SSI6, SSI7, SSI9(except SSI8)
 */
static void EnableAudioClkSsi(r_audio_channel_t ssiChNo)
{
	uint32_t dataL;
	uint32_t setData;

	/* AUDIO_CLK select data */
	setData = 0x10;  /* No divided, BRGA output clock */

	/* AUDIO_CLK_SEL0[31:24][23:16][15:08][07:00] = SSI3, SSI2, SSI1, RCAR_SSI0 */
	/* AUDIO_CLK_SEL1[31:24][23:16][15:08][07:00] = SSI7, SSI6, SSI5, SSI4 */
	/* AUDIO_CLK_SEL2              [15:08]        = SSI9 */
	switch (ssiChNo) {
	case R_AUDIO_SSI0:
		dataL = readl(ADG_AUDIO_CLK_SEL0);
		dataL &= ~(0xFF << 0);
		dataL |= (setData << 0);
		writel(dataL, ADG_AUDIO_CLK_SEL0);
		break;
	case R_AUDIO_SSI1:
		dataL = readl(ADG_AUDIO_CLK_SEL0);
		dataL &= ~(0xFF << 8);
		dataL |= (setData << 8);
		writel(dataL, ADG_AUDIO_CLK_SEL0);
		break;
	case R_AUDIO_SSI2:
		dataL = readl(ADG_AUDIO_CLK_SEL0);
		dataL &= ~(0xFF << 16);
		dataL |= (setData << 16);
		writel(dataL, ADG_AUDIO_CLK_SEL0);
		break;
	case R_AUDIO_SSI3:
		dataL = readl(ADG_AUDIO_CLK_SEL0);
		dataL &= ~(0xFF << 24);
		dataL |= (setData << 24);
		writel(dataL, ADG_AUDIO_CLK_SEL0);
		break;
	case R_AUDIO_SSI4:
		dataL = readl(ADG_AUDIO_CLK_SEL1);
		dataL &= ~(0xFF << 0);
		dataL |= (setData << 0);
		writel(dataL, ADG_AUDIO_CLK_SEL1);
		break;
	case R_AUDIO_SSI5:
		dataL = readl(ADG_AUDIO_CLK_SEL1);
		dataL &= ~(0xFF << 8);
		dataL |= (setData << 8);
		writel(dataL, ADG_AUDIO_CLK_SEL1);
		break;
	case R_AUDIO_SSI6:
		dataL = readl(ADG_AUDIO_CLK_SEL1);
		dataL &= ~(0xFF << 16);
		dataL |= (setData << 16);
		writel(dataL, ADG_AUDIO_CLK_SEL1);
		break;
	case R_AUDIO_SSI7:
		dataL = readl(ADG_AUDIO_CLK_SEL1);
		dataL &= ~(0xFF << 24);
		dataL |= (setData << 24);
		writel(dataL, ADG_AUDIO_CLK_SEL1);
		break;
	case R_AUDIO_SSI9:
		dataL = readl(ADG_AUDIO_CLK_SEL2);
		dataL &= ~(0xFF << 8);
		dataL |= (setData << 8);
		writel(dataL, ADG_AUDIO_CLK_SEL2);
		break;

    default:
        /* R_AUDIO_SSI8 or others */
        break;
	}
}

static void clkEnable(void)
{
    R_PMA_CPG_SetClockStopStat(R_CPG_ADG_CLOCK, false);
}

static void clkDisable(void)
{
    R_PMA_CPG_SetClockStopStat(R_CPG_ADG_CLOCK, true);
}

void R_ADG_Init(r_audio_channel_t chNo, uint32_t rate)
{
    clkEnable();

    /* Set Audio Clock Frequency */
	switch (rate) {
	case AUDIO_44_1kHz:  /* AUDIO_CLKA = Oscillator (Auto output) */
		/* AUDIO_CLKA=22.5792MHz */
		break;
	case AUDIO_48_0kHz:  /* AUDIO_CLKB = CS2000-CP (Clock Synthesizer) */
		/* AUDIO_CLKB=24.5760MHz */
		StartCS2000CP(RATIO_24576000Hz);
		/* PLL Lock Time - REF_CLK : 3ms(MAX) */
		/* mdelay(10);   wait 10ms */
		break;
	}

    switch (rate) {
	case AUDIO_44_1kHz:
		/* 22.5792MHz * 1/1 = 22.5792MHz (44.1kHz*256*2) */
		InitAudioClkoutAndBrgA(AUDIO_44_1kHz_ADG,2);  /* AUDIO_CLKOUT3 and BRGA = AUDIO_CLKA * 1/2 */
		EnableAudioClkSsi(chNo);
		break;
	case AUDIO_48_0kHz:
		/* 24.5760MHz * 1/1 = 24.5760MHz (48.0kHz*256*2) */
		InitAudioClkoutAndBrgA(AUDIO_48_0kHz_ADG,2);  /* AUDIO_CLKOUT3 and BRGA = AUDIO_CLKB * 1/2 */
		EnableAudioClkSsi(chNo);
		break;
	}
}

// TODO: Add support for disabling SSI clocks
void R_ADG_Release(void)
{
    clkDisable();
}