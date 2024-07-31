/*************************************************************************************************************
* lvds_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_lvds_api.h"
#include "r_lvds_regs.h"
#include "r_delay_api.h"
#include "r_prr_api.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

/* R-Car E3 LVDPLLCR specifics */
#define R_E3_LVDCR0_PHY_ENABLE              (1 << 1)    /* LVEN */

#define R_E3_LVDPLLCR_PLLON                 (1 << 22)
#define R_E3_LVDPLLCR_PLLSEL_PLL0           (0 << 20)
#define R_E3_LVDPLLCR_PLLSEL_PLL1           (2 << 20)
#define R_E3_LVDPLLCR_CKSEL_CPG             (1 << 17)
#define R_E3_LVDPLLCR_CKSEL_EXTAL           (3 << 17)
#define R_E3_LVDPLLCR_CKSEL_DU_DOTCLKIN0    (5 << 17)
#define R_E3_LVDPLLCR_CKSEL_DU_DOTCLKIN1    (7 << 17)
#define R_E3_LVDPLLCR_OCKSEL                (1 << 16)
#define R_E3_LVDPLLCR_STP_CLKOUTE           (1 << 14)
#define R_E3_LVDPLLCR_OUTCLKSEL             (1 << 12)
#define R_E3_LVDPLLCR_CLKOUT                (1 << 11)
#define R_E3_LVDPLLCR_E_POS                 10
#define R_E3_LVDPLLCR_N_POS                 3
#define R_E3_LVDPLLCR_M_POS                 0

#define R_E3_LVDDIV_DIVSEL                  (1 << 8)
#define R_E3_LVDDIV_DIVRESET                (1 << 7)
#define R_E3_LVDDIV_DIVSTP                  (1 << 6)

#define R_LVDCR0_OUTPUT_ON                  (1 << 0)    /* LVRES */
#define R_LVDCR0_ENABLE                     (1 << 1)    /* LVEN (V3M/D3/E3 only) */
#define R_LVDCR0_POWER_UP                   (1 << 2)    /* PWD */
#define R_LVDCR0_PLLON                      (1 << 4)    /* PLLON */
#define R_LVDCR0_LVMD_VESA                  (1 << 10)   /* LVMD (mode 4: VESA) */

#define PLL_N_MIN           60U
#define PLL_N_MAX           120U
#define PLL_M_MIN           1U
#define PLL_M_MAX           8U
#define PLL_E_MIN           1U
#define PLL_E_MAX           4U
#define PLL_DIV_MIN         1U
#define PLL_DIV_MAX         64U

#define min(a, b)           (((a) < (b)) ? (a) : (b))
#define max(a, b)           (((a) > (b)) ? (a) : (b))
#define div_round(a, b)     (((a) + ((b) / 2)) / (b))
#define div_round_up(a, b)  (((a) + ((b) - 1)) / (b))
#define abs(a, b)           (((a) > (b)) ? (a) - (b) : (b) - (a))

typedef struct R_LVDS_PllSettings {
    unsigned int delta;
    unsigned int n;
    unsigned int m;
    unsigned int e;
    unsigned int fixed_div;
    unsigned int div;
    uint32_t outclk;
} R_LVDS_PllSettings_t;

static int loc_E3_CalcPllSettings(unsigned int Fin, unsigned int Ftarget, R_LVDS_PllSettings_t *pll)
{
    R_LVDS_PllSettings_t current;
    const unsigned int Fin_min = 12000000;
    const unsigned int Fin_max = 192000000;
    int ret = 1;

    /* Check input clock is allowed */
    if ((Fin < Fin_min) | (Fin > Fin_max)) {
        R_PRINT_Log("[loc_E3_CalcPllSettings] Fin value is invalid. Failed(1)\r\n");
        return 1;
    }

    /*
     * PLL settings for the non Dual-link mode.
     * Fpfd = Fin / M, where M is (1..8). Fpfd must be between 12MHz and 24MHz.
     * Fvco = Fpfd * N, where N is (60..120). Fvco must be between 900MHz and 1800MHz.
     * Fout = Fvco / E, where E is (1, 2, 4). Fout must be less than 1039.5MHz.
     * Divide by 7 for LVDS output as LVDS outputs 7 bits per lane, divide by 1 for TCON or DPAB.
     *
     * Then the clock goes via a programmable divider controlled by LVDDIV register.
     */

    const unsigned int Fpfd_min = 12000000;
    const unsigned int Fpfd_max = 24000000;

    /* Check for all N, M and E values to find the ones that give the closest output dot clock to out requested value */

    /* For all valid M */
    unsigned int m_min = div_round_up(Fin, Fpfd_max);
    unsigned int m_max = Fin / Fpfd_min;

    current.fixed_div = 7;
    unsigned int Fout_target = Ftarget * current.fixed_div;

    m_min = max(m_min, PLL_M_MIN);
    m_max = min(m_max, PLL_M_MAX);

    for (current.m = m_min; current.m <= m_max; current.m++) {
        /* Fvco must be between 900MHz and 1800MHz. */
        const unsigned int Fvco_min = 900000000;
        const unsigned int Fvco_max = 1800000000;
        unsigned int Fpfd = Fin / current.m;
        unsigned int n_min = div_round_up(Fvco_min, Fpfd);
        unsigned int n_max = Fvco_max / Fpfd;

        /* Fvco = Fpfd * N, where N is (60..120) */
        n_min = max(n_min, PLL_N_MIN);
        n_max = min(n_max, PLL_N_MAX);

        /* For all valid N */
        for (current.n = n_min; current.n <= n_max; current.n++) {
            /* Fout must be less than 1039.5MHz. */
            const unsigned int Fout_max = 1039500000;
            unsigned int Fvco = Fpfd * current.n;
            unsigned int e_max = Fout_max / Fvco;

            /* Fout = Fvco / E, where E is (1, 2, 4).
             * 3 will be skipped by the increment in the for loop below */
            e_max = max(e_max, PLL_E_MIN);
            e_max = min(e_max, PLL_E_MAX);

            /* For all valid E */
            for (current.e = PLL_E_MIN; current.e <= e_max; current.e *= 2) {
                unsigned int Fout = Fvco / current.e;

                /* Work out the Fout divider */
                current.div = div_round(Fout, Fout_target);
                current.div = max(current.div, PLL_DIV_MIN);
                current.div = min(current.div, PLL_DIV_MAX);

                current.outclk = Fout / current.div;
                current.delta = abs(current.outclk, Fout_target);

                if (current.delta < pll->delta) {
                    *pll = current;
                    ret = 0;
                }
            }
        }
    }

    if (0 != ret)
    {
        R_PRINT_Log("[loc_E3_CalcPllSettings]  Failed(1)\r\n");
    }

    return ret;
}

static int32_t loc_ConfigPll(uint32_t LvdsUnit, r_du_DispTimings_t *Timings)
{
    uintptr_t base_addr = R_LVDS_PRV_GetRegBase(LvdsUnit);
    uint32_t LvdsPllLvdpllcr = Timings->LvdsPllLvdpllcr;
    uint32_t LvdsPllLvddiv = Timings->LvdsPllLvddiv;
    R_LVDS_PllSettings_t pll = { 0 };
    uint32_t du_DotClkInIndex = 0;
    int32_t ret =  0;

    R_PRINT_Log("LVDS%ld: Target dotclk is %ld Hz\n", LvdsUnit, Timings->TargetDispClock);

    /* If the app has not provided any settings for the PLL, calculate them */
    if (LvdsPllLvdpllcr == 0) {
        /* To start with, we are as far away from the target clock as possible */
        pll.delta = UINT32_MAX;

        /* Try DU_DOTCLKIN0 */
        if (loc_E3_CalcPllSettings(Timings->DotClkIn[0], Timings->TargetDispClock, &pll) == 0) {
            du_DotClkInIndex = 0;
        }

        /* Try DU_DOTCLKIN1 */
        if (loc_E3_CalcPllSettings(Timings->DotClkIn[1], Timings->TargetDispClock, &pll) == 0) {
            du_DotClkInIndex = 1;
        }

        /* Try EXTAL */
        if (loc_E3_CalcPllSettings(Timings->Extal, Timings->TargetDispClock, &pll) == 0) {
            R_PRINT_Log("LVDS%ld: Using EXTAL, pixel clock is %ld Hz\n", LvdsUnit, pll.outclk / 7);
            LvdsPllLvdpllcr |= R_E3_LVDPLLCR_CKSEL_EXTAL;
        } else {
            R_PRINT_Log("LVDS%ld: Using dotclkin%ld, pixel clock is %ld Hz\n", LvdsUnit, du_DotClkInIndex, pll.outclk / 7);

            if (du_DotClkInIndex == 0) {
                LvdsPllLvdpllcr |= R_E3_LVDPLLCR_CKSEL_DU_DOTCLKIN0;
            } else {
                LvdsPllLvdpllcr |= R_E3_LVDPLLCR_CKSEL_DU_DOTCLKIN1;
            }
        }

        /* TODO add support for LV0ϕ, this require a CPG function to modify LV0CK divider  */

        if (pll.delta == UINT32_MAX) {
            R_PRINT_Log("LVDS%ld: Could not calculate LVDS PLL settings!\n", LvdsUnit);
            return 1;
        }

        Timings->CurrentDispClock = pll.outclk;

        LvdsPllLvdpllcr |= (pll.m - 1) << R_E3_LVDPLLCR_M_POS;
        LvdsPllLvdpllcr |= (pll.n - 1) << R_E3_LVDPLLCR_N_POS;

        /* Only enable the E divider if used */
        if (pll.e > 1) {
            LvdsPllLvdpllcr |= R_E3_LVDPLLCR_STP_CLKOUTE | R_E3_LVDPLLCR_OUTCLKSEL;
            LvdsPllLvdpllcr |= (pll.e / 4) << R_E3_LVDPLLCR_E_POS;
        }

        LvdsPllLvdpllcr |= R_E3_LVDPLLCR_PLLON;

        /* Selector: LVDPLLCR.PLLSEL
         * 00: PLL from this LVDS channel
         * 10: PLL from the other LVDS channel (dual link mode)
         * x1: CPG
         */

        /* Optional divide by 7 : LVDPLLCR.OCKSEL: 1: bypass, 0: divide by 7
         * LVDS-IF: Select divide by 7
         * TCON or DPAD: Select bypass
         */
        if (pll.fixed_div)
            LvdsPllLvdpllcr |= R_E3_LVDPLLCR_CLKOUT;

        /* Only enable the main divider if used */
        if (pll.div > 1)
            LvdsPllLvddiv = R_E3_LVDDIV_DIVSEL | R_E3_LVDDIV_DIVRESET | (pll.div - 1);
    }

    R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDPLLCR, LvdsPllLvdpllcr);
    delay_us(300);
    R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDDIV, LvdsPllLvddiv);

    return ret;
}

int32_t R_LVDS_Setup(uint32_t LvdsUnit, r_du_DispTimings_t *Timings)
{
    uintptr_t base_addr = R_LVDS_PRV_GetRegBase(LvdsUnit);
    r_prr_DevName_t dev = R_PRR_GetDevice();
    int32_t ret =  0;
    uint32_t lvdcr0;

    if ((R_PRR_RCARE3 == dev) || (R_PRR_RCARD3 == dev)) {
        ret = loc_ConfigPll(LvdsUnit, Timings);
        if (ret != 0) {
            R_PRINT_Log("[R_LVDS_Setup] loc_ConfigPll : Return value is %d. Failed(1)\r\n",ret);
            return 1;
        }

        /* Enable LVDS PHY */
        lvdcr0 = R_LVDCR0_ENABLE;

    } else if ((R_PRR_RCARH3 == dev) || (R_PRR_RCARM3W == dev) || (R_PRR_RCARM3N == dev)) {
        uint32_t divcnt;

        /* Enable LVDS module clock : done by R_CPG_LvdsSetup in wmdrv. */

        /* Set PLL */
        if (Timings->CurrentDispClock < 42000000) {
            divcnt = 0x14CB;
        } else if (Timings->CurrentDispClock < 85000000) {
            divcnt = 0xA45;
        } else if (Timings->CurrentDispClock < 128000000) {
            divcnt = 0x6C3;
        } else {
            divcnt = 0x46C1;
        }
        R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDPLLCR, divcnt);

        /* initialize */
        R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCTRCR, 0);
        R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCHCR, 0);
        R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCR0, 0);

        /* Turn on the PLL */
        lvdcr0 = R_LVDCR0_PLLON;
        R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCR0, lvdcr0);

        /* Turn on the LVDS BIAS logic */
        lvdcr0 |= R_LVDCR0_POWER_UP;

#ifdef USE_WIDE_HD_AUO
        /* Change the LVMD mode to mode 4 (VESA display) */
        lvdcr0 |= R_LVDCR0_LVMD_VESA;
#endif        
        R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCR0, lvdcr0);

	R_TICK_DelayUs(100);
    } else {
        R_PRINT_Log("[R_LVDS_Setup] R_PRR_GetDevice : Return value(dev) is %d. Failed(1)\r\n",(int)dev);
        return 1;
    }

    /* Turn output on */
    lvdcr0 |= R_LVDCR0_OUTPUT_ON;
    R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCR0, lvdcr0);

    /* Enable CLK pin and CH[0-3] pins */
    R_LVDS_PRV_RegWrite(base_addr + R_LVDS_LVDCR1, 0x3ff);

    return ret;
}
