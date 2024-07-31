/*************************************************************************************************************
* logo_display_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef DISPLAY_H_
#define DISPLAY_H_

int Disp_Init(uint8_t DisplayUnit, uint8_t TimingCfgIdx);
int Disp_Deinit(void);
int Disp_InitSurface(uint8_t DisplayUnit, uint8_t SurfaceIdx, uintptr_t pFrameBuffer);
void Disp_PrintDiscomError(void);
#endif /* DISPLAY_H_ */