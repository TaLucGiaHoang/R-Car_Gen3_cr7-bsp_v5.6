/*************************************************************************************************************
* du_timings_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_DU_TIMINGS_H_
#define R_DU_TIMINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_du_config.h"
#include "r_du_api.h"
#include "r_prr_api.h"

/**
 * Configure the display timing for the given display unit.
 *
 * @param[in]   DuUnit      See @ref r_du_Unit_t
 * @param[in]   Timings     See @ref r_du_DispTimings_t
 * @param[in]   Dev         Device name, see @ref r_prr_DevName_t
 * @retval      0 in case of success, 1 in case or error
 *
 */
r_du_Error_t R_DU_PRV_SetTimings(r_du_Unit_t DuUnit, r_du_DispTimings_t *Timings,
                                 r_prr_DevName_t Dev);

#ifdef __cplusplus
}
#endif

#endif /* R_DU_TIMINGS_H_ */
