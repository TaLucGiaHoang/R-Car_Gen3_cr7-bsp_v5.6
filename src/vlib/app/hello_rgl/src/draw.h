/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
#ifndef _DRAW_H
#define _DRAW_H

int DrawInit(display_t *pDisplay);
int DrawDeinit(display_t *pDisplay);
int DrawLoop(void);

#endif /* _DRAW_H*/
