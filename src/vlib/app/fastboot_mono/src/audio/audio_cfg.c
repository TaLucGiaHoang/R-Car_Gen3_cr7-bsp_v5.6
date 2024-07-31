/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : audio_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot_mono sub app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if FASTBOOT_CFG_APP_AUDIO_ENABLE

#include "fastboot_cfg.h"

#include "fastboot.h"
#include "audio.h"
/* Fork form https://github.com/graphitemaster/incbin.git
 * incbin: Include binary files in your C/C++ applications with ease
 */
#include "incbin.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define FASTBOOT_CFG_AUDIO_TSK_PRIO (OSAL_THREAD_PRIORITY_TYPE5)

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*
 * INCBIN generates the following symbols:
 * const unsigned char gBeepingData[];
 * const unsigned char *const gBeepingEnd;
 * const unsigned int gBeepingSize;
 *
 * audio files format:
 * Supported audio file format: 16bit, 44.1KHz
 * >> Convert WAV to PCM using FFmpeg
 * $ ffmpeg -i beeping.wav -f s16le -ar 44100 -acodec pcm_s16le beeping.raw
 */
INCBIN(Beeping, "..//..//fastboot_mono//src//audio//beeping.raw");

static const audio_Config_t loc_AudioConfig = {
    .SsiChannel  = R_AUDIO_SSI0,
    .SampleRate  = AUDIO_44_1kHz,
    .SoundEffect = SOUND_EFFECT_TYPE_BEEPING,
};

static void * loc_AudioThread(void * Arg)
{
    audio_main(&loc_AudioConfig);
    return 0;
}

static void loc_AudioEvtHandler(fastboot_Event_t Evt)
{
    switch (Evt.EventId) {
    case FASTBOOT_EVT_R_GEAR_ON:
        /* TODO: turn on warning beep */
        break;
    case FASTBOOT_EVT_R_GEAR_OFF:
        /* TODO: turn off warning beep */
        break;
    default:
        break;
    }
}

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const fastboot_SubApp_t fastboot_SubAppAudio = {
    .ThreadFunc       = loc_AudioThread,
    .ThreadPriority   = FASTBOOT_CFG_AUDIO_TSK_PRIO,
    .EventHandlerFunc = loc_AudioEvtHandler,
};

#endif /* FASTBOOT_CFG_APP_AUDIO_ENABLE */
