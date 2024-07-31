/*************************************************************************************************************
* vsp2_dpr_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_DPR_H_
#define R_VSP2_DPR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"

/**
 * Initialise the DPR Unit
 *
 * @param[in]   Vsp2Unit        - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   Conf            - Dpr Config @ref r_vsp2_Dpr_Conf_t
 * @retval      See  @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_PRV_DprInit(r_vsp2_Unit_t Vsp2Unit, r_vsp2_Dpr_Conf_t *Conf);

/**
 * Change the settings of DPR for enable DISCOM
 *
 * @param[in]   Vsp2Unit        		- Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RPF ID used by DISCOM	- Dpr Config @ref r_vsp2_Dpr_Conf_t
 * @retval      See  @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_PRV_DprSetDiscomEnable(const r_vsp2_Unit_t Vsp2Unit, uint32_t Rpf_ID);

/**
 * Change the settings of DPR for disable DISCOM
 *
 * @param[in]   Vsp2Unit        		- Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RPF ID used by DISCOM	- Dpr Config @ref r_vsp2_Dpr_Conf_t
 * @retval      See  @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_PRV_DprSetDiscomDisable(const r_vsp2_Unit_t Vsp2Unit, uint32_t Rpf_ID);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_DPR_H_ */
