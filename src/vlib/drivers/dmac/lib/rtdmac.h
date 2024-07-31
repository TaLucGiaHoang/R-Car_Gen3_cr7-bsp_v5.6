/*************************************************************************************************************
* dmac
* Copyright (c) [2018-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef __RTDMAC_H__
#define __RTDMAC_H__

#include "drv.h"
#include "rdmac_common.h"

typedef enum {
    RCAR_RT_DMAC0,
    RCAR_RT_DMAC1,
    RCAR_RT_DMAC_NONE
} R_RTDMAC_Dev_t;

/* API of the RT-DMAC driver */
extern uint16_t R_RTDMAC_Init(R_RTDMAC_Dev_t dev, rDmacPriorityMode_t mode);
extern uint16_t R_RTDMAC_Exec(R_RTDMAC_Dev_t dev, uint8_t chNo, rDmacCfg_t *cfg,
                              rDmacDescCfg_t *descCfg, uintptr_t arg);
extern uint16_t R_RTDMAC_Stop(R_RTDMAC_Dev_t dev, uint8_t chNo);
extern void R_RTDMAC_Release(R_RTDMAC_Dev_t dev);


#endif  /* __RTDMAC_H__ */


