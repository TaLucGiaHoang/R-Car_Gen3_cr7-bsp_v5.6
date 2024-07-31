/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
#ifndef _VOUT_H
#define _VOUT_H

int VoutInit(display_t *pDisplay);
int VoutDeinit(display_t *pDisplay);
int VoutSurfaceInit(layer_t *pLayer);
int VoutSetFB(layer_t *pLayer, uint32_t BufIndex);

osal_memory_manager_handle_t VoutGetMemhandle(void);

#endif /* _VOUT_H*/
