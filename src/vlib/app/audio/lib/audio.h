/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef _H_AUDIO_MAIN_H_
#define _H_AUDIO_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_audio_core.h"

typedef enum {
    SOUND_EFFECT_TYPE_NONE,
    SOUND_EFFECT_TYPE_BEEPING,
} sound_Effect_Type;

typedef struct {
    r_audio_channel_t SsiChannel;
    uint32_t          SampleRate;
    sound_Effect_Type SoundEffect;
} audio_Config_t;

void audio_main(const audio_Config_t * ConfigRef);

#ifdef __cplusplus
}
#endif

#endif /* _H_AUDIO_MAIN_H_ */