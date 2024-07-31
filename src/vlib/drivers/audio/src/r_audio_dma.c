/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_audio_common.h"

void InitAudioDMAC(uint8_t dev)
{
    R_AUDMAC_Init(dev, DRV_DMAC_PRIO_FIX);
}

void ReleaseAudioDMAC(uint8_t dev)
{
    R_AUDMAC_Release(dev);
}