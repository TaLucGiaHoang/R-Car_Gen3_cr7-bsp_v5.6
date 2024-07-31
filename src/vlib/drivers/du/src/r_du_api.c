/*************************************************************************************************************
* du_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_du_config.h"
#include "r_du_plane.h"
#include "r_du_api.h"
#include "r_du_timings.h"
#include "r_du_regs.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

static r_du_Error_t locDu_CheckParam(r_du_Unit_t DuUnit,
                const r_du_DispTimings_t *Timings, uint32_t DuPlaneMsk,
                r_prr_DevName_t Dev);

/**
 * Test the validity of the input parameter of the R_DU_Init function
 *
 * @param: du_unit, display unit index
 * @param: timings, pointer to the display timings
 * @return: R_DU_SUCCESS or R_DU_FAIL
 */
static r_du_Error_t locDu_CheckParam(r_du_Unit_t DuUnit,
                                     const r_du_DispTimings_t *Timings, uint32_t DuPlaneMsk,
                                     r_prr_DevName_t Dev)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;

    if (0x2 < DuPlaneMsk) {
        /* plane_idx: 0x1 - du plane 1,
         *            0x2 - du plane 3
         *            0x3 - du planes 1 & 3, superimpose. Not supported
         */
        err = R_DU_ERR_INVALID_PLANE;
        R_PRINT_Log("err = R_DU_ERR_INVALID_PLANE\r\n");
        goto end;
    }

    if (0 == Timings) {
        err = R_DU_ERR_NULL_POINTER;
        R_PRINT_Log("err = R_DU_ERR_NULL_POINTER\r\n");
        goto end;
    }
    /* check if the DuUnit is valid */
    switch (Dev) {
    case R_PRR_RCARH3:
        if (R_DU_RCARH3_MAX_UNIT <= DuUnit) {
            err = R_DU_ERR_WRONG_UNIT;
            R_PRINT_Log("err = R_DU_ERR_WRONG_UNIT(H3)\r\n");
        }
        break;
    case R_PRR_RCARM3W:
        if (R_DU_RCARM3_MAX_UNIT <= DuUnit) {
            err = R_DU_ERR_WRONG_UNIT;
            R_PRINT_Log("err = R_DU_ERR_WRONG_UNIT(M3W)\r\n");
        }
        break;
    /* fall through */
    case R_PRR_RCARM3N:
        /* M3 is special as only DU0, DU1 and DU3 are supported
         * 3 Units but unit number can be 0, 1 and 3 only.
         */
        if ((R_DU_RCARM3_MAX_UNIT + 1) <= DuUnit) {
            err = R_DU_ERR_WRONG_UNIT;
            R_PRINT_Log("err = R_DU_ERR_WRONG_UNIT(M3N)\r\n");
        } else if (2 == DuUnit) {
            err = R_DU_ERR_WRONG_UNIT;
            R_PRINT_Log("err = R_DU_ERR_WRONG_UNIT(M3N DU2)\r\n");
        }
        break;
    case R_PRR_RCARE3:
        if (R_DU_RCARE3_MAX_UNIT <= DuUnit) {
            err = R_DU_ERR_WRONG_UNIT;
            R_PRINT_Log("err = R_DU_ERR_WRONG_UNIT(E3)\r\n");
        }
        break;
    default:
        err = R_DU_ERR_WRONG_DEVICE;
        R_PRINT_Log("err = R_DU_ERR_WRONG_DEVICE\r\n");
        break;
    }

end:
    return err;
}

void R_DU_Enable(r_du_Unit_t DuUnit, r_du_Enable_t Enable)
{
    uintptr_t du_group_reg_base;
    uint32_t val = 0;

    du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);

    val = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DSYSR);
    if (R_DU_ENABLE == Enable) {
        val |= R_DU_DSYSR_DEN;
    } else {
        val &= ~R_DU_DSYSR_DEN;
    }
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DSYSR, val);

    return;
}

void R_DU_UpdateByResetToggle(r_du_Unit_t DuUnit)
{
    uintptr_t du_group_reg_base;
    uint32_t reg_val = 0;
    uint32_t dsysr_orig = 0;

    du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);

    /* NOTE: R_DU_DEF8R (and many others) is updated ONLY upon reset bit toggle!!! */
    reg_val = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DSYSR);
    dsysr_orig = reg_val;

    /* To do: find a way to pass warning to user w/o printf */

#if 0
    if (dsysr_orig & R_DU_DSYSR_DEN) {
        Vlib_Log_Info("WARNING: du_update_by_reset_toggle(): DRES bit toggled while DU enabled");
    }
#endif
    reg_val |=  R_DU_DSYSR_DRES;
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DSYSR, reg_val); /* Set DRES bit */
    reg_val &= ~R_DU_DSYSR_DRES;
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DSYSR, reg_val); /* Clear DRES bit */

    /* Reset DRES bit to the original value */
    if (dsysr_orig & R_DU_DSYSR_DRES) {
        reg_val |= R_DU_DSYSR_DRES;
        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DSYSR, reg_val); /* Set DRES bit */
    }

    return;
}

r_du_Error_t R_DU_Init(r_du_Unit_t DuUnit, r_du_DispTimings_t *Timings,
                       uint32_t DuPlaneMsk, const r_du_Plane_t *DuPlaneCfg,
                       r_prr_DevName_t Dev)
{
    r_du_Error_t err;
    uintptr_t du_reg_base;
    uintptr_t du_group_reg_base;
    uint32_t val;

    err = locDu_CheckParam(DuUnit, Timings, DuPlaneMsk, Dev);

    if (R_DU_ERR_SUCCESS == err) {
        du_reg_base = R_DU_PRV_GetRegBase(DuUnit);
        du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);

        /* Stop display unit operation */
        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DSYSR, R_DU_DSYSR_DRES);

        /* Reset display unit mode to initial values*/
        R_DU_PRV_WriteReg(du_reg_base + R_DU_DSMR, 0);

        /* Clear display unit flags */
        R_DU_PRV_WriteReg(du_reg_base + R_DU_DSRCR,
                          R_DU_DSRCR_TVCL |
                          R_DU_DSRCR_FRCL |
                          R_DU_DSRCR_VBCL |
                          R_DU_DSRCR_RICL |
                          R_DU_DSRCR_HBCL);

        /* Disable display unit interrupts */
        R_DU_PRV_WriteReg(du_reg_base + R_DU_DIER, 0);

        /* Enable DEFR Extensional functions */
        R_DU_PRV_WriteReg((du_group_reg_base + R_DU_DEFR), (R_DU_DEFR_CODE | R_DU_DEFR_DEFE));

        /* Enable DEF5R Extensional functions */
        R_DU_PRV_WriteReg((du_group_reg_base + R_DU_DEF5R), (R_DU_DEF5R_CODE | R_DU_DEF5R_DEFE5));

        /* Enable DEF10R Extensional functions */
        R_DU_PRV_WriteReg((du_group_reg_base + R_DU_DEF10R), (R_DU_DEF10R_CODE | R_DU_DEF10R_DEFE10));

        /* Set display unit timings */
        err = R_DU_PRV_SetTimings(DuUnit, Timings, Dev);

        if (R_DU_ERR_SUCCESS == err) {
            /* Set Master mode for this DU channel */
            val = R_DU_PRV_ReadReg(du_reg_base + R_DU_DSYSR);
            val &= ~R_DU_DSYSR_TVM; /* Clear TVM bits */
            R_DU_PRV_WriteReg(du_reg_base + R_DU_DSYSR, (val | R_DU_DSYSR_TVM_SWITCH)); /* Switching mode first...*/
            R_DU_PRV_WriteReg(du_reg_base + R_DU_DSYSR, (val | R_DU_DSYSR_TVM_MASTER)); /* ...then Master mode */

            /* Set ODDF pin mode */
            val = (R_DU_DEF6R_CODE | R_DU_DEF6R_ODPM12_DISP | R_DU_DEF6R_ODPM02_DISP);
            R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DEF6R, val);

            /* Set color for Display Off mode */
            switch (DuUnit) {
            case R_DU_DU0:
                val = 0x0000FCFC; /* cyan */
                break;
            case R_DU_DU1:
                val = 0x00FC00FC; /* magenta */
                break;
            case R_DU_DU2:
                val = 0x00FCFC00; /* yellow */
                break;
            case R_DU_DU3:
                val = 0x0000FC00; /* green */
                break;
            default:
                val = 0x00FC0000; /* red */
                break;
            }
            R_DU_PRV_WriteReg(du_reg_base + R_DU_DOOR, val);

            /* Note: Current driver do not support superimpose planes for DU
             * An error shall have occurred before arriving here if plane index
             * was different than 0 or 1 which correspond to DU plane 1 and 3
             */
            if (1 == DuPlaneMsk) {
                err = R_DU_PRV_InitPlane(DuUnit, 1, &DuPlaneCfg[0], Dev);
            } else { /* 2 == DuPlaneMsk */
                err = R_DU_PRV_InitPlane(DuUnit, 3, &DuPlaneCfg[1], Dev);
            }
            /* Clear Reset Flag */
            val = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DSYSR);
            //val &= ~(R_DU_DSYSR_DEN | R_DU_DSYSR_DRES); /* Clear DEN and DRES bits */
            val &= ~(R_DU_DSYSR_DRES); /* Clear DEN and DRES bits */
            R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DSYSR, val);
        }
    }
    return err;
}

r_du_Error_t R_DU_ColorFmtSet(r_du_Unit_t DuUnit, uint32_t DuPlaneMsk,
                              r_du_PlaneFormat_t DuFmt)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;
    uint32_t plane_id;
    uintptr_t du_group_reg_base;
    uint32_t reg_val;

    /* Set plane color format */
    switch (DuFmt) {
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
        reg_val = R_DU_PpDDC4R_EDF_RGB888;
        break;
    }

    if ((1 == DuPlaneMsk) || (2 == DuPlaneMsk)) {
        if (1 == DuPlaneMsk) {
            plane_id = 1;
        } else {
            /* 2 == DuPlaneMsk */
            plane_id = 3;
        }

        du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);
        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_PpDDC4R + R_DU_PLANE_OFFSET(plane_id),
                          R_DU_PpDDC4R_CODE | reg_val);
        R_DU_UpdateByResetToggle(DuUnit);

    } else {
        err = R_DU_ERR_INVALID_PLANE;
        R_PRINT_Log("err = R_DU_ERR_INVALID_PLANE\r\n");
    }

    return err;
}

r_du_Error_t R_DU_BkgColorSet(r_du_Unit_t DuUnit, uint32_t Color)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;
    uintptr_t du_reg_base;

    du_reg_base = R_DU_PRV_GetRegBase(DuUnit);
    /* remove any alpha channel information */
    R_DU_PRV_WriteReg(du_reg_base + R_DU_DOOR, (Color & 0x00FFFFFF));

    R_DU_UpdateByResetToggle(DuUnit);

    return err;
}
