/*************************************************************************************************************
* vsp2_vspi_prv_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_VSPI_PRV_H_
#define R_VSP2_VSPI_PRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"
#include "r_vsp2_vspi_api.h"

/**
 * VSPI Frame End Callback from IRQ
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @retval      none
 */
void R_VSP2_VSPI_PRV_FrameEndCallback(r_vsp2_Unit_t Vsp2Unit);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_VSPI_PRV_H_ */
