/*************************************************************************************************************
* du_timings_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_du_config.h"
#include "r_du_regs.h"
#include "r_du_plane.h"
#include "r_du_api.h"
#ifdef USE_HDMI
#include "r_hdmi_api.h"
#endif
#include "r_lvds_api.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_prr_api.h"
#include "r_print_api.h"

#define ESCR_FRQSEL_MIN     2U
#define ESCR_FRQSEL_MAX     64U

#define DPLL_FDPLL_MIN      2U
#define DPLL_FDPLL_MAX      128U
#define DPLL_N_MIN          40U
#define DPLL_N_MAX          120U
#define DPLL_M_MIN          1U
#define DPLL_M_MAX          8U

#define DPLL_CODE           0x95U
#define DPLL_PLCS1          (1 << 23)
#define DPLL_PLCS0          (1 << 21)       /* 1 - DPLL1, 0 - DCLKIN */
#define DPLL_CLKE           (1 << 18)       /* DPLL1 clk enable */
#define DPLL_STBY           (1 << 2)        /* 1 - Enable operation */

#define DPLL_FPFD_MIN_FREQ   20000000ULL
#define DPLL_FPFD_MAX_FREQ   51200000ULL
#define DPLL_FVCO_MIN_FREQ   2000000000ULL
#define DPLL_FVCO_MAX_FREQ   4096000000ULL

#define min(a, b)           (((a) < (b)) ? (a) : (b))
#define max(a, b)           (((a) > (b)) ? (a) : (b))
#define div_round(a, b)     (((a) + ((b) / 2)) / (b))
#define div_round_up(a, b)  (((a) + ((b) - 1)) / (b))
#define abs(a, b)           (((a) > (b)) ? (a) - (b) : (b) - (a))

typedef struct {
    uint32_t dotclkin_idx;
    uint32_t n;
    uint32_t m;
    uint32_t fdpll;
    uint32_t delta;
    uint32_t outclk;
    uint32_t escr_div;
} r_du_DpllSettings_t;

static uint32_t loc_GetBestDotClkSrc(r_du_Unit_t DuUnit, const uint32_t DotClkIn[4], uint32_t TargetClk);
static uint32_t locDu_SelectDotClock(r_du_Unit_t DuUnit, r_du_DispTimings_t *Timings);
static void locDu_SetSignalPolarity(r_du_Unit_t DuUnit, const r_du_DispTimings_t *Timings);

static uint32_t loc_GetBestDotClkSrc(r_du_Unit_t DuUnit, const uint32_t DotClkIn[4], uint32_t TargetClk)
{
    r_prr_DevName_t dev = R_PRR_GetDevice();
    uintptr_t du_reg_base = R_DU_PRV_GetRegBase(DuUnit);
    uintptr_t du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);
    uint32_t base_ch;
    uint32_t in_pclk;
    r_du_DpllSettings_t current = { 0 }, best = { .delta = UINT32_MAX };
    uint32_t val;
    uint32_t escr = 0;
    int use_bus_clk_src = 0;
    int use_dpll = 0;

    R_PRINT_Log("DU%d: Target dotclk is %ld Hz\n", DuUnit, TargetClk);

    /*
     * Some DUs use one of the group's dotclkin or the internal bus clock, that then
     * goes into the FRQSEL divider.
     * The other DUs use a PLL using one of the group's dotclkin as the input.
     */
    if ((DuUnit == 0) ||
        ((dev == R_PRR_RCARH3)  && (DuUnit == 3)) ||
        ((dev == R_PRR_RCARM3N) && (DuUnit == 3)) ||
        ((dev == R_PRR_RCARM3W) && (DuUnit == 2)) ||
        ((dev == R_PRR_RCARE3)  && (DuUnit == 1)) ||
        ((dev == R_PRR_RCARD3)  && (DuUnit == 1))) {

        /* First, try the internal bus clock for the dotclkin */
        if ((dev == R_PRR_RCARE3) || (dev == R_PRR_RCARD3)) {
            in_pclk = 533333333; /* S1D1phi, 533.33MHz (actually 1.6GHz / 3) */
        } else {
            in_pclk = 400000000; /* S2D1phi, 400MHz */
        }
        R_PRINT_Log("DU%d: Trying internal bus %ld Hz\n", DuUnit, in_pclk);
        best.escr_div = div_round(in_pclk, TargetClk);
        best.escr_div = max(best.escr_div, ESCR_FRQSEL_MIN);
        best.escr_div = min(best.escr_div, ESCR_FRQSEL_MAX);
        best.outclk = in_pclk / best.escr_div;
        best.delta = abs(best.outclk, TargetClk);
        use_bus_clk_src = 1;

        /* Try both DU dotclkin external clock sources (i.e. dotclkin0 and dotclkin1 for both DU0 and DU1,
         * dotclkin2 and dotclkin3 for both DU2 and DU3 */
        base_ch = DuUnit & ~1;  /* i.e. DU0 or DU2 */
        for (current.dotclkin_idx = base_ch; current.dotclkin_idx <= base_ch + 1; current.dotclkin_idx++) {
            in_pclk = DotClkIn[current.dotclkin_idx];

            R_PRINT_Log("DU%d: Trying dotclkin%ld %ld Hz\n", DuUnit, current.dotclkin_idx, in_pclk);

            if (in_pclk == 0) {
                continue;
            }

            current.escr_div = div_round(in_pclk, TargetClk);
            current.escr_div = max(current.escr_div, ESCR_FRQSEL_MIN);
            current.escr_div = min(current.escr_div, ESCR_FRQSEL_MAX);
            current.outclk = in_pclk / current.escr_div;
            current.delta = abs(current.outclk, TargetClk);

            if (current.delta < best.delta) {
                use_bus_clk_src = 0;
                best = current;
            }
        }
    } else {
        /*
         * The parameters in DPLLCR are:
         *  N[11:5] - Multiplier setting of the DPLLx feedback clock
         * The parameters in DPLLCR2 are:
         *  FDPLL[6:0] - Freq division of the output DPLLx block
         *  M[10:8] - Input clock divider
         * Fin = DPLLx Input Freq
         * Fpfd = Fin / (M+1)
         * Fvco = Fpfd * (N+1)
         * DPLLx Output Freq = Fvco / (FDPLL+1)
         */

        /* Try both DU dotclkin external clock sources */
        base_ch = DuUnit & ~1;  /* i.e. DU0 or DU2 */
        for (current.dotclkin_idx = base_ch; current.dotclkin_idx <= base_ch + 1; current.dotclkin_idx++) {
            uint32_t m_min;
            uint32_t m_max;

            in_pclk = DotClkIn[current.dotclkin_idx];

            R_PRINT_Log("DU%d: Trying dotclkin%ld %ld Hz\n", DuUnit, current.dotclkin_idx, in_pclk);

            if (in_pclk == 0) {
                continue;
            }

            /* Ensure Fpfd is within specified range */
            m_min = div_round_up(in_pclk, DPLL_FPFD_MAX_FREQ);
            m_max = in_pclk / DPLL_FPFD_MIN_FREQ;
            m_min = max(m_min, DPLL_M_MIN);
            m_max = min(m_max, DPLL_M_MAX);

            /* For all FDPLL and M values, calculate the best value of N. Keep the
             * best combination that give the closest output dot clock to requested
             * value */
            /* For all valid M */
            for (current.m = m_min; current.m <= m_max; current.m++) {
                uint64_t Fpfd = (uint64_t)in_pclk / current.m;
                /* For all valid FDPLL */
                for (current.fdpll = DPLL_FDPLL_MIN; current.fdpll <= DPLL_FDPLL_MAX; current.fdpll++) {
                    uint64_t Fvco;

                    /* Work out the value of N */
                    current.n = div_round((uint64_t)TargetClk * current.fdpll, Fpfd);
                    current.n = max(current.n, DPLL_N_MIN);
                    current.n = min(current.n, DPLL_N_MAX);

                    Fvco = Fpfd * current.n;

                    /* Check Fvco is within specified range */
                    if (Fvco > DPLL_FVCO_MAX_FREQ || Fvco < DPLL_FVCO_MIN_FREQ) {
                        continue;
                    }

                    current.outclk = (uint32_t)(Fvco  / current.fdpll);
                    current.delta = abs(current.outclk, TargetClk);

                    if (current.delta < best.delta) {
                        use_dpll = 1;
                        best = current;
                    }
                }
            }
        }
    }

    if (best.delta == UINT32_MAX) {
        R_PRINT_Log("DU%d: Cannot calculate PLL settings! Check DotClkIn[].\n", DuUnit);
        return 0;
    }

    if (use_dpll == 0) {
        escr = best.escr_div - 1;
        if (use_bus_clk_src != 0) {
            R_PRINT_Log("DU%d: Using internal bus clock, div %ld, dotclk is %ld Hz\n",
                        DuUnit, best.escr_div, best.outclk);
            escr |= R_DU_ESCR_DCLKSEL;
        } else {
            R_PRINT_Log("DU%d: Using dotclkin%ld, div %ld, dotclk is %ld Hz\n",
                          DuUnit, best.dotclkin_idx, best.escr_div, best.outclk);
            val = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DIDSR);
            val |= 0x77900000;

            if ((R_DU_DU0 == DuUnit) || (R_DU_DU2 == DuUnit)) {
                /* PDCS0 */
                val &= ~1;
                val |= best.dotclkin_idx;
            } else {
                /* PDCS1 */
                val &= ~(1 << 2);
                best.dotclkin_idx = ~best.dotclkin_idx & 1;
                val |= (best.dotclkin_idx % 2) << 2;
            }

            R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DIDSR, val);
        }

        /* External synchronization control registers */
        if ((R_DU_DU1 == DuUnit) || (R_DU_DU3 == DuUnit)) {
            R_DU_PRV_WriteReg(du_reg_base + R_DU_ESCR13, escr);
            R_DU_PRV_WriteReg(du_reg_base + R_DU_OTAR13, 0);
        } else {
            R_DU_PRV_WriteReg(du_reg_base + R_DU_ESCR02, escr);
            if ((dev == R_PRR_RCARE3) || (dev == R_PRR_RCARD3)) {
                R_DU_PRV_WriteReg(du_reg_base + R_DU_OTAR02, 0);
            }
        }
    } else {
        R_PRINT_Log("DU%d: Using dotclkin%d %d Hz, DPLL, N %d, FDPLL %d, M %d, dotclk is %d Hz\n",
                      DuUnit, best.dotclkin_idx, DotClkIn[best.dotclkin_idx], best.n, best.fdpll, best.m, best.outclk);

        val = DPLL_CODE << 24 | DPLL_CLKE | DPLL_STBY;
        val |= (best.n - 1) << 5;

        if (DuUnit <= R_DU_DU1) {
            /* PLCS1: DU1 input dot clock source is DPLL0 */
            val |= DPLL_PLCS1 | (best.dotclkin_idx << 1);
        } else {
            if (dev == R_PRR_RCARH3) {
                /* PLCS0: DU2 input dot clock source is DPLL1 */
                val |= DPLL_PLCS0;
            }
            val |= ((best.dotclkin_idx % 2) << 1);
        }

        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DPLLCR, val);

        val = DPLL_CODE << 24 | (1 << 12);
        val |= (best.fdpll - 1);
        val |= (best.m - 1) << 8;
        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DPLLC2R, val);
    }

    return best.outclk;
}

static uint32_t locDu_SelectDotClock(r_du_Unit_t DuUnit, r_du_DispTimings_t *Timings)
{
    uintptr_t du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);
    r_prr_DevName_t dev = R_PRR_GetDevice();
    uint32_t actualDispClock = 0;
    uint32_t didsr;

    if (((R_PRR_RCARD3 != dev) && (R_PRR_RCARE3 != dev)) || (Timings->VoutType != R_DU_VOUT_LVDS)) {
        Timings->CurrentDispClock = loc_GetBestDotClkSrc(DuUnit, Timings->DotClkIn, Timings->TargetDispClock);
    }

    /* DU input dot clock selector */
    didsr = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DIDSR);
    didsr |= 0x77900000;
    switch (DuUnit) {
    case R_DU_DU0:
        if (R_DU_VOUT_LVDS == Timings->VoutType) {
            if ((R_PRR_RCARD3 == dev) || (R_PRR_RCARE3 == dev)) {
                didsr |= R_DU_DIDSR_LDCS0; /* DU0 input dotclk source is the LVDS-IF pin */
            } else {
                didsr &= ~R_DU_DIDSR_LDCS0; /* reserved for H3/M3/M3N, always write 0 */
            }
        }
        break;
    case R_DU_DU1:
        if (R_DU_VOUT_LVDS == Timings->VoutType) {
            didsr |= R_DU_DIDSR_LDCS1; /* DU1 input dotclk source is the LVDS-IF pin */
        }
        break;
    default:
        break;  /* DIDSR not modified */
    }
    R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DIDSR, didsr);

    return actualDispClock;
}

static void locDu_SetSignalPolarity(r_du_Unit_t DuUnit, const r_du_DispTimings_t *Timings)
{
    uintptr_t du_reg_base;
    uint32_t reg_val;

    du_reg_base = R_DU_PRV_GetRegBase(DuUnit);

    /* Set DSMR */
    reg_val = 0;
    if (0 == (Timings->VoutFlags & R_DU_FLAG_DESYNC_ACTHI)) {
        reg_val |= R_DU_DSMR_DIL;
    }
    if (0 != (Timings->VoutFlags & R_DU_FLAG_VSYNC_ACTHI)) {
        reg_val |= R_DU_DSMR_VSL;
    }
    if (0 != (Timings->VoutFlags & R_DU_FLAG_HSYNC_ACTHI)) {
        reg_val |= R_DU_DSMR_HSL;
    }
    if (0 != (Timings->VoutFlags & R_DU_FLAG_CSYNC)) {
        reg_val |= R_DU_DSMR_VSPM;
    } else {
        reg_val |= R_DU_DSMR_CSPM;
    }

    R_DU_PRV_WriteReg(du_reg_base + R_DU_DSMR, reg_val);

    return;
}

r_du_Error_t R_DU_PRV_SetTimings(r_du_Unit_t DuUnit, r_du_DispTimings_t *Timings,
                                 r_prr_DevName_t Dev)
{
    r_du_Error_t err = R_DU_ERR_SUCCESS;
    uintptr_t du_reg_base;
    uintptr_t du_group_reg_base;
    uint32_t xw;
    uint32_t yw;
    uint32_t hc;
    uint32_t hsw;
    uint32_t xs;
    uint32_t vc;
    uint32_t vsw;
    uint32_t ys;
    uint32_t reg_val;

    if ((0 == Timings) || (R_DU_LAST <= DuUnit)) {
        R_PRINT_Log("DU%d: R_DU_ERR_INVALID_PARAMETER\n", DuUnit);
        return R_DU_ERR_INVALID_PARAMETER;
    }

    du_reg_base = R_DU_PRV_GetRegBase(DuUnit);
    du_group_reg_base = R_DU_PRV_GetGroupRegBase(DuUnit);
    xw = Timings->ScreenWidth;
    yw = Timings->ScreenHeight;
    hc = Timings->VoutHc;
    hsw = Timings->VoutHsw;
    xs = Timings->VoutXs;
    vc = Timings->VoutVc;
    vsw = Timings->VoutVsw;
    ys = Timings->VoutYs;

    /* Select the best input dot clock, calculate PLL/divider settings and
     * program them */
    locDu_SelectDotClock(DuUnit, Timings);

    /* Adjust scan frequency if the generated display clock doesn't match
     * the correct frequency. */
    if ((Timings->VoutType != R_DU_VOUT_LVDS) && (Timings->CurrentDispClock != Timings->TargetDispClock)) {
        int scale = div_round(Timings->CurrentDispClock, Timings->TargetDispClock / 1024);

        if (scale != 1024) {
            uint32_t new_hc = div_round((int)hc * scale, 1024);

            R_PRINT_Log("DU%d: modifying porch to meet timing, hc %ld, was %ld\n", DuUnit, new_hc, hc);
            hc = new_hc;
            if (hc <= (hsw + xs + xw)) {
                R_PRINT_Log("DU%d: R_DU_ERR_SLOW_CLOCK\n", DuUnit);
                return R_DU_ERR_SLOW_CLOCK;
            }
        }
    }

    R_PRINT_Log("DU%d: %ld x %ld at %ld Hz\n", DuUnit, xw, yw, Timings->TargetDispClock / (hc * vc));

    R_DU_PRV_WriteReg(du_reg_base + R_DU_HDSR, hsw + xs - 19);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_HDER, hsw + xs + xw - 19);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_HCR,  hc - 1);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_HSWR, hsw - 1);

    R_DU_PRV_WriteReg(du_reg_base + R_DU_VDSR, ys - 2);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_VDER, ys - 2 + yw);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_VCR,  vc - 1);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_VSPR, vc - 1 - vsw);

    /* Clear following registers */
    R_DU_PRV_WriteReg(du_reg_base + R_DU_EQWR, 0);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_SPWR, 0);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_CLAMPSR, 0);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_CLAMPWR, 0);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_DESR, 0);
    R_DU_PRV_WriteReg(du_reg_base + R_DU_DEWR, 0);

    switch (Timings->VoutType) {
    case R_DU_VOUT_HDMI:
    {
#ifdef USE_HDMI
        uint32_t ret;
        ret = (uint32_t)R_HDMI_Setup(DuUnit, Timings);
        if (0 < ret) {
            err = R_DU_ERR_HDMI_FAILED;    /* Failed to initialise HDMI */
            R_PRINT_Log("DU%d: err = R_DU_ERR_HDMI_FAILED\r\n", DuUnit);
        }
#else
        err = R_DU_ERR_HDMI_NOT_AVAILABLE; /* HDMI not available */
        R_PRINT_Log("DU%d: err = R_DU_ERR_HDMI_NOT_AVAILABLE\r\n", DuUnit);
#endif
        break;
    }
    case R_DU_VOUT_VGA:
    {
        /* ARGB setup */
        reg_val = R_DU_PRV_ReadReg(du_group_reg_base + R_DU_DEF8R) & 0xFFFF;
        reg_val &= ~R_DU_DEF8R_DRGBS; /* Clear DRGBS bits */
        reg_val |= R_DU_DEF8R_CODE;
        if (R_DU_DU0 == DuUnit) {
            reg_val |= R_DU_DEF8R_DRGBS_DU0;
        } else if (R_DU_DU1 == DuUnit) {
            reg_val |= R_DU_DEF8R_DRGBS_DU1;
        } else if (R_DU_DU2 == DuUnit) {
            reg_val |= R_DU_DEF8R_DRGBS_DU2;
        } else if (R_DU_DU3 == DuUnit) {
            reg_val |= R_DU_DEF8R_DRGBS_DU3;
        }
        R_DU_PRV_WriteReg(du_group_reg_base + R_DU_DEF8R, reg_val);
        break;
    }
    case R_DU_VOUT_LVDS:
    {
        uint32_t ret = 0;
        if (R_PRR_RCARE3 == Dev) {
            if (R_DU_DU0 == DuUnit) {
                ret = R_LVDS_Setup(0, Timings);
            } else if (R_DU_DU1 == DuUnit) {
                ret = R_LVDS_Setup(1, Timings);
            } else {
                err = R_DU_ERR_WRONG_UNIT;
                R_PRINT_Log("DU%d: err = R_DU_ERR_WRONG_UNIT(E3)\r\n", DuUnit);
            }
        } else if ((R_PRR_RCARH3 == Dev) || (R_PRR_RCARM3W == Dev) || (R_PRR_RCARM3N == Dev)) {
            if (R_DU_DU0 == DuUnit) {
                ret = R_LVDS_Setup(0, Timings);
            } else {
                err = R_DU_ERR_WRONG_UNIT;
                R_PRINT_Log("DU%d: err = R_DU_ERR_WRONG_UNIT(Dev = %d)\r\n", DuUnit, Dev);
            }
        } else {
            err = R_DU_ERR_WRONG_DEVICE;
            R_PRINT_Log("DU%d: err = R_DU_ERR_WRONG_DEVICE(Dev = %d)\r\n", DuUnit, Dev);
        }
        if (1 == ret) {
            err = R_DU_ERR_LVDS_FAILED;
            R_PRINT_Log("DU%d: err = R_DU_ERR_LVDS_FAILED\r\n", DuUnit);
        }
        break;
    }
    default:
        /* Should never ever come here as we already tested for wrong output type */
        err = R_DU_ERR_UNKNOWN_VOUT_TYPE;
        R_PRINT_Log("DU%d: err = R_DU_ERR_UNKNOWN_VOUT_TYPE\r\n", DuUnit);
        break;
    }

    if (R_DU_ERR_SUCCESS == err) {
        /* Set syncs polarity */
        locDu_SetSignalPolarity(DuUnit, Timings);

        /* Update register */
        R_DU_UpdateByResetToggle(DuUnit);
    }

    return err;
}
