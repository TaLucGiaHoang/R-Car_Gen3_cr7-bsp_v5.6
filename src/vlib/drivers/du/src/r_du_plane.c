/*************************************************************************************************************
* du_plane_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_du_config.h"
#include "r_du_plane.h"
#include "r_du_regs.h"
#include "r_du_api.h"
#include "r_prr_api.h"
#include "r_print_api.h"

static r_du_Error_t locDu_CheckPlaneConfig(r_du_Unit_t DuUnit,
                                           uint32_t PlaneId, const r_du_Plane_t *Plane,
                                           r_prr_DevName_t Dev);
static r_du_Error_t locDu_SetPlane(r_du_Unit_t DuUnit, uint32_t PlaneId,
                                   const r_du_Plane_t *Plane);
static r_du_Error_t locDu_EnablePlane(r_du_Unit_t DuUnit, uint32_t PlaneId,
                                      uint32_t Enable);

static r_du_Error_t locDu_CheckPlaneConfig(r_du_Unit_t DuUnit,
                                           uint32_t PlaneId, const r_du_Plane_t *Plane,
                                           r_prr_DevName_t Dev)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;

    if ((1 != PlaneId) && (3 != PlaneId)) {
        err = R_DU_ERR_INVALID_PARAMETER;
        R_PRINT_Log("err = R_DU_ERR_INVALID_PARAMETER(PlaneId %d)\r\n", PlaneId);
    } else if (0 == Plane) {
        err = R_DU_ERR_INVALID_PARAMETER;
        R_PRINT_Log("err = R_DU_ERR_INVALID_PARAMETER(Plane 0)\r\n");
    } else {
        if (R_PRR_RCARM3W == Dev) {
            if ((2 == DuUnit) && (1 != PlaneId)) {
                err = R_DU_ERR_INVALID_PARAMETER;
                R_PRINT_Log("err = R_DU_ERR_INVALID_PARAMETER(M3W PlaneId %d)\r\n", PlaneId);
            }
        } else if (R_PRR_RCARM3N == Dev) {
            if ((3 == DuUnit) && (3 != PlaneId)) {
                err = R_DU_ERR_INVALID_PARAMETER;
                R_PRINT_Log("err = R_DU_ERR_INVALID_PARAMETER(M3N PlaneId %d)\r\n", PlaneId);
            }
        }
    }

    return err;
}

static r_du_Error_t locDu_SetPlane(r_du_Unit_t DuUnit, uint32_t PlaneId,
                                   const r_du_Plane_t *Plane)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;
    uint32_t reg_val;
    uintptr_t du_group_reg_base;

    du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);

    /* Set Plane superimpose mode */
    reg_val = R_DU_PpMR_16BPP;
    switch (Plane->blending) {
    case R_DU_BLEND_OPAQUE:
        reg_val |= R_DU_PpMR_SPIM_OPAQUE;
        break;
    case R_DU_BLEND_ALPHA:
        reg_val |= R_DU_PpMR_SPIM_ALPHA;
        break;
    case R_DU_BLEND_EOR:
        reg_val |= R_DU_PpMR_SPIM_EOR;
        break;
    default:
        err = R_DU_ERR_BLEND_DEFAULT;
        R_PRINT_Log("err = R_DU_ERR_BLEND_DEFAULT\r\n");
        reg_val |= R_DU_PpMR_SPIM_OPAQUE;
        break;
    }

    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpMR + R_DU_PLANE_OFFSET(PlaneId), reg_val);

    /* Set Plane size */
    /* The size of Plane->size_w and Plane->size_h is up to 8191 */
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpDSXR + R_DU_PLANE_OFFSET(PlaneId),
                      Plane->size_w & 0x00001FFF);
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpDSYR + R_DU_PLANE_OFFSET(PlaneId),
                      Plane->size_h & 0x00001FFF);

    /* Set Plane position */
    /* The size of Plane->pos_x and Plane->pos_y is up to 8191 */
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpDPXR + R_DU_PLANE_OFFSET(PlaneId),
                      Plane->pos_x & 0x00001FFF);
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpDPYR + R_DU_PLANE_OFFSET(PlaneId),
                      Plane->pos_y & 0x00001FFF);

    /* Set Plane color format */
    switch (Plane->format) {
    case R_DU_FORMAT_ARGB8888:
        reg_val = R_DU_PpDDC4R_EDF_ARGB8888;
        break;
    case R_DU_FORMAT_RGB888:
        /* not support */
        reg_val = R_DU_PpDDC4R_EDF_RGB888;
        break;
    case R_DU_FORMAT_RGB666:
        /* not support */
        reg_val = R_DU_PpDDC4R_EDF_RGB666;
        break;
    case R_DU_FORMAT_YCBCR444:
        /* not support */
        reg_val = R_DU_PpDDC4R_EDF_YCBCR444;
        break;
    default:
        err = R_DU_ERR_FMT_DEFAULT;
        R_PRINT_Log("err = R_DU_ERR_FMT_DEFAULT\r\n");
        reg_val = R_DU_PpDDC4R_EDF_RGB888;
        break;
    }

    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpDDC4R + R_DU_PLANE_OFFSET(PlaneId),
                      R_DU_PpDDC4R_CODE | reg_val);

    return err;
}

static r_du_Error_t locDu_EnablePlane(r_du_Unit_t DuUnit, uint32_t PlaneId,
                                      uint32_t Enable)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;
    uintptr_t du_group_reg_base;
    uint32_t val = 0;
    uintptr_t address = 0;

    du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);

    /* INFO: Currently this function does not handle priorities and only
     * one Plane at a time can be Enabled. It is also assumed that DPRS
     * bit has already been set in DU DORCR register */
    if (R_DU_ENABLE == Enable) {
        val = PlaneId;
    } else {
        val = 0;
    }

    switch (DuUnit) {
    case R_DU_DU0:
    case R_DU_DU2:
        address = du_group_reg_base + R_DU_DS0PR;
        break;
    case R_DU_DU1:
    case R_DU_DU3:
        address = du_group_reg_base + R_DU_DS1PR;
        break;
    default:
        err = R_DU_ERR_WRONG_UNIT;
        R_PRINT_Log("err = R_DU_ERR_WRONG_UNIT\r\n");
        break;
    }

    if (R_DU_ERR_SUCCESS == err) {
        R_DU_PRV_WriteReg(address, val);
    }

    return err;
}

r_du_Error_t R_DU_PRV_InitPlane(r_du_Unit_t DuUnit, uint32_t DuPlane,
                                const r_du_Plane_t *PlaneCfg, r_prr_DevName_t Dev)
{
    r_du_Error_t err;
    uint32_t val;
    uintptr_t du_group_reg_base;

    err = locDu_CheckPlaneConfig(DuUnit, DuPlane, PlaneCfg, Dev);

    if (R_DU_ERR_SUCCESS == err) {
        du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);
        /* Display Priority Register Select
         * Enable DPRS so that priority for planes can
         * be set in DS0PRm and DS1PRm */
        val = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DORCR);
        val |= R_DU_DORCR_DPRS;
        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DORCR, val);

        /* DU Plane Timing Select
         * Plane3 Dot Clock -> Dot Clock generator 1
         * Plane1 Dot Clock -> Dot Clock generator 0
         * Plane3 Timing    -> Display Timing generator 1
         * Plane1 Timing    -> Display Timing generator 0
         *
         * For RCAR_M3W we need to set the above only for DU0/DU1
         * (because DU2 has only one plane) */
        switch (Dev) {
        case R_PRR_RCARH3:
        case R_PRR_RCARM3N:
            R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DPTSR, 0x00040004);
            break;
        case R_PRR_RCARM3W:
        case R_PRR_RCARE3:
            if ((0 == DuUnit) || (1 == DuUnit)) {
                R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DPTSR, 0x00040004);
            }
            break;
        case R_PRR_RCARLAST:
        default:
            err = R_DU_ERR_WRONG_DEVICE;
            R_PRINT_Log("err = R_DU_ERR_WRONG_DEVICE\r\n");
            break;
        }

        /* plane Id shall be fixed by CR7 RGL */
        err = locDu_SetPlane(DuUnit, DuPlane, PlaneCfg);
        if ((R_DU_ERR_SUCCESS == err) || (R_DU_ERR_BLEND_DEFAULT == err)
            || (R_DU_ERR_FMT_DEFAULT == err)) {
            /** INFO: how to pass a warning to user that we moved to default blending mode.**/
            err = locDu_EnablePlane(DuUnit, DuPlane, R_DU_ENABLE); /* Enable plane */
        }
    }

    return err;
}
