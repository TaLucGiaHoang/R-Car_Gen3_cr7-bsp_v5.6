/*************************************************************************************************************
* audmac
* Copyright (c) [2022-2023] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef __AUDMAC_H__
#define __AUDMAC_H__

#include "drv.h"
#include "rdmac_common.h"

typedef enum {
    RCAR_AUDIO_DMAC_NONE,
	RCAR_AUDIO_DMAC0 = 0,
	RCAR_AUDIO_DMAC1,
    RCAR_AUDIO_DMAC_MAX
} R_AUDMAC_Dev_t;

typedef enum {
    AUIDO_DMAC_CH0 = 0,
    AUIDO_DMAC_CH1,
    AUIDO_DMAC_CH2,
    AUIDO_DMAC_CH3,
    AUIDO_DMAC_CH4,
    AUIDO_DMAC_CH5,
    AUIDO_DMAC_CH6,
    AUIDO_DMAC_CH7,
    AUIDO_DMAC_CH8,
    AUIDO_DMAC_CH9,
    AUIDO_DMAC_CH10,
    AUIDO_DMAC_CH11,
    AUIDO_DMAC_CH12,
    AUIDO_DMAC_CH13,
    AUIDO_DMAC_CH14,
    AUIDO_DMAC_CH15,
    AUIDO_DMAC_CH_MAX
} R_AUDMAC_Dev_Ch_t;

/* API for the AUDIO-DMAC driver */
extern uint16_t R_AUDMAC_Init(R_AUDMAC_Dev_t dev, rDmacPriorityMode_t mode);
extern uint16_t R_AUDMAC_Exec(R_AUDMAC_Dev_t dev, uint8_t chNo, rDmacCfg_t *cfg,
				  rDmacDescCfg_t *descCfg, uintptr_t arg);
extern uint16_t R_AUDMAC_Stop(R_AUDMAC_Dev_t dev, uint8_t chNo);
extern void R_AUDMAC_Release(R_AUDMAC_Dev_t dev);
extern void R_AUDMAC_Isr(R_AUDMAC_Dev_t dev, uint8_t chNo);
extern void R_AUDMAC_Wait(R_AUDMAC_Dev_t dev, uint8_t chNo);


#endif  /* __AUDMAC_H__ */

