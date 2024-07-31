/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "audio.h"

/* Fork form https://github.com/graphitemaster/incbin.git
 * incbin: Include binary files in your C/C++ applications with ease
 */
#include "incbin.h"

#if !defined(R_USE_AUDIO)
#error "audio app requires the R_USE_AUDIO option. Please execute 'cmake -DR_USE_AUDIO=ON <build-dir>' to enable it."
#endif

#define AUDIO_TASK_THREAD_ID_00   0x9000u

typedef struct {
    uintptr_t PcmFileStart; /* Start address of pcm file */
    uint32_t  PcmFileSize;  /* Size of pcm file */
} audio_PcmFile_t;

INCBIN_EXTERN(Beeping);

static int loc_Load_Pcm_File(sound_Effect_Type effet, audio_PcmFile_t *pcm)
{
    switch (effet) {
    case SOUND_EFFECT_TYPE_BEEPING:
        pcm->PcmFileStart = (uintptr_t)gBeepingData;
        pcm->PcmFileSize  = gBeepingSize;
        break;

    default:
        R_PRINT_Log("Load sound effect file failed\r\n");
        return -1;
        break;
    }

    return 0;
}

void audio_main(const audio_Config_t * ConfigRef)
{
    r_audio_channel_t ch          = ConfigRef->SsiChannel;
    uint32_t          sample_rate = ConfigRef->SampleRate;
    sound_Effect_Type effet       = ConfigRef->SoundEffect;
    audio_PcmFile_t   pcm         = {0};

    R_PRINT_Log("Audio service is working...\r\n");

    R_Audio_Init(ch, sample_rate);
    loc_Load_Pcm_File(effet, &pcm);
    while(1) {
        R_Audio_start(ch, 1, pcm.PcmFileStart, pcm.PcmFileSize);
        R_Audio_stop(ch);
    }
    R_Audio_Release(ch);

    R_PRINT_Log("Audio service stoped\r\n");

}
