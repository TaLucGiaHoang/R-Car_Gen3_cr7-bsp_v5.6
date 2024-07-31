/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_AUDIO_COMMON_H_
#define R_AUDIO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_audio_core.h"
#include "r_audio_regs.h"
#include "r_cpg_api.h"
#include "audmac.h"

#define BIT(nr)         (1UL << (nr))

void R_ADG_Init(r_audio_channel_t chNo, uint32_t rate);
void R_ADG_Release(void);
void InitAudioDMAC(uint8_t dev);
void ReleaseAudioDMAC(uint8_t dev);
void R_SSI_Init(r_audio_channel_t chNo);
void R_SSI_Release(r_audio_channel_t chNo);
void R_SSI_StartDmac(r_audio_channel_t chNo, uint32_t play, 
            uintptr_t addr, uint32_t len);
void R_SSI_StopDmac(r_audio_channel_t chNo);
int R_SSI_WaitForCompletion(r_audio_channel_t chNo);
void audioPfcConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* R_AUDIO_COMMON_H_ */