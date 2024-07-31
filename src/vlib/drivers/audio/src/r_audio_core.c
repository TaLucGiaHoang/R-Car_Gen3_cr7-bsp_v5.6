/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_audio_core.h"
#include "r_audio_common.h"
#include "ak4613.h"

void R_Audio_Init(r_audio_channel_t ch, uint32_t rate)
{
    audioPfcConfig();

    // TODO: Support for dynamic audio stream clock switching
    R_ADG_Init(ch, rate);

    ak4613_init(rate);
    /* In order not to use the I2C bus again.
     * I2C devices belonging to linux or android are mounted on the same I2C bus.
     * This avoids the issue of competition with linux or android for I2C bus.
     */
	StartCodec();

    // TODO: Support for different board level configurations
    R_SSI_Init(ch);
}

void R_Audio_Release(r_audio_channel_t ch)
{
    R_SSI_Release(ch);

    /* To allow the Codec to be initialized only once 
     * and then no longer use the I2C bus 
     */
    R_ADG_Release();
}

void R_Audio_start(r_audio_channel_t ch, uint32_t play,
            uintptr_t addr, uint32_t len)
{
   R_SSI_StartDmac(ch, play, addr, len);
}

void R_Audio_stop(r_audio_channel_t ch)
{
   R_SSI_StopDmac(ch);
}



