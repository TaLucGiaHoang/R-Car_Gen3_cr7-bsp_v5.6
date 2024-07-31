/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_AUDIO_CORE_H_
#define R_AUDIO_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "r_i2c_api.h"
#include "r_pinctrl_api.h"

/**
 * \defgroup AUDIOgroup   Audio driver
 * @ingroup drivers
 * @brief Audio driver interface
 * @{
 * @}
 */

/**
 * \defgroup AUDIOconst   Constants
 * @ingroup AUDIOgroup
 * @{
 */

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

/* sampling rate */
#define AUDIO_44_1kHz		0x0  /*  44.1kHz */
#define AUDIO_48_0kHz		0x3  /*  48.0kHz */

/* ADG */
#define AUDIO_CLKA			0x1
#define AUDIO_CLKB			0x2
#define AUDIO_CLKC 			0x3

/* Board-level configuration start */
// TODO: support 48KHz
#define SAMPLE_RATE         AUDIO_44_1kHz

#define AUDIO_SSI_USED      R_AUDIO_SSI0
#define AUIDO_DMAC_DEV      RCAR_AUDIO_DMAC0

#define CODEC_I2C_BUS       R_I2C_IF2
#define CS2000_I2C_BUS      R_I2C_IF2

#define AUDIO_44_1kHz_ADG   AUDIO_CLKA
#define AUDIO_48_0kHz_ADG   AUDIO_CLKB

/** @} constants */

/**
 * \defgroup AUDIOtypes   Data Types
 * @ingroup AUDIOgroup
 * @{
 */

/**
 * @enum   PFC for SSI/ADG
 */
typedef enum {
    SSI_SDATA0 = 0,  
	SSI_WS01239,     
    SSI_SCK01239,    
	SSI_SDATA1_A,    
	SSI_SDATA1_B,    
	SSI_WS1_A,       
	SSI_WS1_B,       
	SSI_SCK1_A,      
	SSI_SCK1_B,      
	SSI_SDATA2_A,    
	SSI_SDATA2_B,    
    SSI_WS2_A,       
    SSI_WS2_B,       
    SSI_SCK2_A,      
    SSI_SCK2_B,      
    SSI_SDATA3,      
    SSI_WS349,       
    SSI_SCK349,      
	SSI_SDATA4,      
    SSI_WS4,         
    SSI_SCK4,        
    SSI_SDATA5,      
    SSI_WS5,         
    SSI_SCK5,        
    SSI_SDATA6,      
    SSI_WS6,         
    SSI_SCK6,        
    SSI_SDATA7,      
    SSI_WS78,        
    SSI_SCK78,       
    SSI_SDATA8,      
    SSI_SDATA9_A,    
    SSI_SDATA9_B,    
    SSI_WS9_A,       
    SSI_WS9_B,       
    SSI_SCK9_A,      
    SSI_SCK9_B,      
    SSI_PIN_ALL
} ssi_pin_desc_t;

/**
 * @enum   List of ADG clocks
 */
typedef enum {
    AUDIO_CLKA_A = 0,
    AUDIO_CLKA_B,
    AUDIO_CLKA_C,
    AUDIO_CLKB_A,
    AUDIO_CLKB_B,
    AUDIO_CLKC_A,
    AUDIO_CLKC_B,
    AUDIO_CLKOUT_A,
    AUDIO_CLKOUT_B,
    AUDIO_CLKOUT_C,
    AUDIO_CLKOUT_D,
    AUDIO_CLKOUT1_A,
    AUDIO_CLKOUT1_B,
    AUDIO_CLKOUT2_A,
    AUDIO_CLKOUT2_B,
    AUDIO_CLKOUT3_A,
    AUDIO_CLKOUT3_B
} adg_pin_desc_t;

/* r_audio_pfc.c defines pin control. 
 * xxx_pin_groups needs to be modified according to the board. 
 *
 * ssi_pin_groups[]
 * adg_pin_groups[]
 */

/* Board-level configuration end */

/**
 * @enum  Audio SSI Channels
 */
typedef enum {
    R_AUDIO_SSI0 = 0,
    R_AUDIO_SSI1,    
    R_AUDIO_SSI2,   
    R_AUDIO_SSI3,    
    R_AUDIO_SSI4,
    R_AUDIO_SSI5,
    R_AUDIO_SSI6,
    R_AUDIO_SSI7,
    R_AUDIO_SSI8,
    R_AUDIO_SSI9,
    R_AUDIO_SSI_ALL
} r_audio_channel_t;

/** @} data types */
/**
 * \defgroup AUDIOapi   API
 * @ingroup AUDIOgroup
 * @{
 */

/**
 * Function   R_Audio_Init
 * @brief     Initial setting of Audio driver
 * @param[in] ch         - Audio channel, range R_AUDIO_SSI0 - R_AUDIO_SSI9. See @ref r_audio_channel_t
 * @param[in] rate       - Audio output data rate.
 *
 * @note Rate - Only @ref SAMPLE_RATE support right now.
 *
 */
void R_Audio_Init(r_audio_channel_t ch, uint32_t rate);

/**
 * Function   R_Audio_Release
 * @brief     Release Audio channel and DMAC
 * @param[in] ch         - Audio channel, range R_AUDIO_SSI0 - R_AUDIO_SSI9. See @ref r_audio_channel_t
 *
 * @note Rate - Only @ref SAMPLE_RATE support right now.
 *
 */
void R_Audio_Release(r_audio_channel_t ch);

/**
 * Function   R_Audio_start
 * @brief     Start Audio playback
 * @param[in] ch        - Audio channel, range R_AUDIO_SSI0 - R_AUDIO_SSI9. See @ref r_audio_channel_t
 * @param[in] play      - 1=play
 * @param[in] addr      - Address of buffer with audio data for playback
 * @param[in] len       - Length of buffer of audio data
 *  
 *
 * @note play - only value of 1 is supported at this time
 *
 */
void R_Audio_start(r_audio_channel_t ch, uint32_t play,
            uintptr_t addr, uint32_t len);

/**
 * Function   R_Audio_stop
 * @brief     Stop Audio playback
 * @param[in] ch        - Audio channel, range R_AUDIO_SSI0 - R_AUDIO_SSI9. See @ref r_audio_channel_t
 *
 */
void R_Audio_stop(r_audio_channel_t ch);

/** @} api */

#ifdef __cplusplus
}
#endif

#endif /* R_AUDIO_CORE_H_ */