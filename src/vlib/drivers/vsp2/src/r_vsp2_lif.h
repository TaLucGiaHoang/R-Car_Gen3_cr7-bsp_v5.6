/*************************************************************************************************************
* vsp2_lif_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_LIF_H_
#define R_VSP2_LIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"

#define R_VSP2_LIF0 0
#define R_VSP2_LIF1 1

r_vsp2_Error_t R_VSP2_PRV_LifEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t LifId,
                                    uint32_t Enable);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_LIF_H_ */
