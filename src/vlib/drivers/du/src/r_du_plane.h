/*************************************************************************************************************
* du_plane_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_DU_PLANE_H_
#define R_DU_PLANE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_du_api.h"
#include "r_prr_api.h"

/**
 * Initialises the DU plane
 *
 * @param: du_unit - display unit index
 * @param: du_plane - plane index
 * @param: plane_cfg - plane configuration
 * @return: error
 */
r_du_Error_t R_DU_PRV_InitPlane(r_du_Unit_t DuUnit, uint32_t DuPlane,
                                const r_du_Plane_t *PlaneCfg, r_prr_DevName_t Dev);

#ifdef __cplusplus
}
#endif

#endif /* R_DU_PLANE_H_ */
