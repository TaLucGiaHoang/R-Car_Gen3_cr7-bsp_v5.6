/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_audio_common.h"
#include "r_pinctrl_api.h"

static ssi_pin_desc_t ssi_pin_groups[] = {SSI_SDATA0, SSI_WS01239, SSI_SCK01239};
static adg_pin_desc_t adg_pin_groups[] = {AUDIO_CLKA_A, AUDIO_CLKOUT3_A};

// TODO: Add all SSI pin configurations.
static const RcarPin_t ssi_groups[] = {
    [SSI_SDATA0] =  {
        .name = "SSI_SDATA0",
        .ipsr = 14,
		.ipsrVal = 0,
		.ipsrPos = 28,
		.gpsr = 6,
		.gpsrPos = 2,
        .modsel = 0xFF,     /* nothing to do */
    },
    [SSI_WS01239] =  {
        .name = "SSI_WS01239",
		.ipsr = 14,
		.ipsrVal = 0,
		.ipsrPos = 24,
		.gpsr = 6,
		.gpsrPos = 1,
        .modsel = 0xFF,     /* nothing to do */
    },
	[SSI_SCK01239] = {
		.name = "SSI_SCK01239",
		.ipsr = 14,
		.ipsrVal = 0,
		.ipsrPos = 20,
		.gpsr = 6,
		.gpsrPos = 0,
        .modsel = 0xFF,     /* nothing to do */
	},
};

// TODO: Add all ADG pin configurations.
static const RcarPin_t adg_groups[] = {
    [AUDIO_CLKA_A] = {
		.name = "AUDIO_CLKA_A",
		.ipsr = 17,
		.ipsrVal = 0,
		.ipsrPos = 0,
		.gpsr = 6,
		.gpsrPos = 22,
        .modsel = 0,
        .modselVal = 0,
        .modselMask = ~(BIT(4) | BIT(3)),
	},
	[AUDIO_CLKOUT3_A] = {
		.name = "AUDIO_CLKOUT3_A",
		.ipsr = 14,
		.ipsrVal = 8,
		.ipsrPos = 0,
		.gpsr = 5,
		.gpsrPos = 19,
        .modsel = 0xFF,     /* nothing to do */
	},
};

static void ssiPfcConfig(void)
{
    int i;
    ssi_pin_desc_t pin = 0;

    for ( i = 0; i < ARRAY_SIZE(ssi_pin_groups); i++)
    {
        pin = ssi_pin_groups[i];
        pfcConfig(&ssi_groups[pin]);
    }
}

static void adgPfcConfig(void)
{
    int i;
    adg_pin_desc_t pin = 0;

    for ( i = 0; i < ARRAY_SIZE(adg_pin_groups); i++)
    {
        pin = adg_pin_groups[i];
        pfcConfig(&adg_groups[pin]);
    }    
}

void audioPfcConfig(void)
{
    ssiPfcConfig();
    adgPfcConfig();
}