/*************************************************************************************************************
* dmac_ctrl_h
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef _RDMAC_CTRL_H_
#define _RDMAC_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "drv.h"

uint16_t rcarDmacCtrlInit(uint8_t dev, rDmacPriorityMode_t mode);
uint16_t rcarDmacExec(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg, uintptr_t arg);
uint16_t rcarDmacStop(uint8_t dev, uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif  /* _RDMAC_CTRL_H_ */
