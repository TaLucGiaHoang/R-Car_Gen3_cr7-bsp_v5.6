/*************************************************************************************************************
* logo
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef LOGO_H
#define LOGO_H

#include "display.h"

typedef struct {
    r_wm_DisplayPort_t DisplayPort;
    uint32_t           LayerZIndex;
    uint32_t           LayerPosX;
    uint32_t           LayerPosY;
    uint32_t           ImageWidth;
    uint32_t           ImageHeight;
    const void *       ImageDataRef;
    void *             FrameBuffer;
} logo_Config_t;

int logo_main(const logo_Config_t *config);

#endif /* RVC_H */