/*************************************************************************************************************
* du_regs_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_DU_REGS_H_
#define R_DU_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define R_DU0_BASE             0xFEB00000  /* < Display Unit 0 base address */
#define R_DU1_BASE             0xFEB30000  /* Display Unit 1 base address */
#define R_DU2_BASE             0xFEB40000  /* Display Unit 2 base address */
#define R_DU3_BASE             0xFEB70000  /* Display Unit 3 base address */

#define R_DU_DSYSR             0x00000000  /* Display unit system control reg offset */
#define R_DU_DSYSR_DRES        (1 << 9)    /* Display Reset bit */
#define R_DU_DSYSR_DEN         (1 << 8)    /* Display Enable bit */
#define R_DU_DSYSR_TVM         (3 << 6)    /* TV Synchronized Mode mask */
#define R_DU_DSYSR_TVM_MASTER  (0 << 6)    /* Master mode */
#define R_DU_DSYSR_TVM_SWITCH  (1 << 6)    /* Switching mode */
#define R_DU_DSYSR_TVM_TV      (2 << 6)    /* TV Syncronized mode */

#define R_DU_DSMR              0x00000004  /* Display unit mode reg offset */
#define R_DU_DSMR_VSPM         (1 << 28)   /* VSYNC Pin Mode */
#define R_DU_DSMR_CSPM         (1 << 24)   /* CSYNC Pin Mode */
#define R_DU_DSMR_DIL          (1 << 19)   /* Display Polarity Select */
#define R_DU_DSMR_VSL          (1 << 18)   /* VSYNC Polarity Select */
#define R_DU_DSMR_HSL          (1 << 17)   /* HSYNC Polarity Select */

#define R_DU_DSSR              0x00000008  /* Display unit status reg offset */
#define R_DU_DSSR_TVR         (1 << 15)   /* TV Synchronization Signal Error Flag Status */
#define R_DU_DSSR_FRM         (1 << 14)   /* Frame Flag Status */
#define R_DU_DSSR_VBK         (1 << 11)   /* Vertical Blanking Flag Status */
#define R_DU_DSSR_RINT        (1 << 9)    /* Raster Interrupt Flag Status */
#define R_DU_DSSR_HBK         (1 << 8)    /* HBK Flag Status */

#define R_DU_DSRCR             0x0000000C  /* Display unit status reg offset clear reg offset */
#define R_DU_DSRCR_TVCL        (1 << 15)   /* TV Synchronization Signal Error Flag Clear */
#define R_DU_DSRCR_FRCL        (1 << 14)   /* Frame Flag Clear */
#define R_DU_DSRCR_VBCL        (1 << 11)   /* Vertical Blanking Flag Clear */
#define R_DU_DSRCR_RICL        (1 << 9)    /* Raster Interrupt Flag Clear */
#define R_DU_DSRCR_HBCL        (1 << 8)    /* HBK Flag Clear */

#define R_DU_DIER              0x00000010  /* Display unit interrupt enable reg offset */
#define R_DU_DIER_TVE         (1 << 15)   /* TV Synchronization Signal Error Flag Enable */
#define R_DU_DIER_FRE         (1 << 14)   /* Frame Flag Enable */
#define R_DU_DIER_VBE         (1 << 11)   /* Vertical Blanking Flag Enable */
#define R_DU_DIER_RIE         (1 << 9)    /* Raster Interrupt Flag Enable */
#define R_DU_DIER_HBE         (1 << 8)    /* HBK Flag Enable */

#define R_DU_DPPR              0x00000018  /* Display plane priority reg offset */

#define R_DU_DEFR              0x00000020  /* Display unit extensional function control reg offset */
#define R_DU_DEFR_CODE         (0x7773 << 16) /* Enabling Code */
#define R_DU_DEFR_DEFE         (1 << 0)    /* Display unit extensional function enable */

#define R_DU_DEF5R             0x000000E0  /* Display unit extensional function control 5 reg offset */
#define R_DU_DEF5R_CODE        (0x66 << 24) /* Enabling Code */
#define R_DU_DEF5R_DEFE5       (1 << 0)    /* Display unit extensional function enable 5 */

#define R_DU_DDLTR             0x000000E4  /* Display unit data latency adjustment reg offset  */

#define R_DU_DEF6R             0x000000E8  /* Display unit extensional function control 6 reg offset */
#define R_DU_DEF6R_CODE        (0x7778 << 16) /* Enabling Code */
#define R_DU_DEF6R_ODPM12      (3 << 10)   /* ODDF Pin Mode 12 mask */
#define R_DU_DEF6R_ODPM12_DISP (2 << 10)   /* DISP signal is output to the ODDF pin (DU1/3) */
#define R_DU_DEF6R_ODPM02      (3 <<  8)   /* ODDF Pin Mode 02 mask */
#define R_DU_DEF6R_ODPM02_DISP (2 <<  8)   /* DISP signal is output to the ODDF pin (DU0/2) */

#define R_DU_DEF7R             0x000000EC  /* Display unit extensional function control 7 reg offset */
#define R_DU_DD1SSR0           0x00020008  /* Display unit domain 1 status reg offset */
#define R_DU_DD1SRCR0          0x0002000C  /* Display unit domain 1 status reg offset clear reg offset */
#define R_DU_DD1IER0           0x00020010  /* Display unit domain 1 interrupt enable reg offset */

#define R_DU_DEF8R             0x00020020  /* Display unit extensional function control 8 reg offset */
#define R_DU_DEF8R_CODE        (0x7790 << 16) /* Enabling Code */
#define R_DU_DEF8R_DRGBS       (3 << 4)    /* Digital RGB Output Select */
#define R_DU_DEF8R_DRGBS_DU0   (0 << 4)    /* Digital RGB signal from DU0 is selected */
#define R_DU_DEF8R_DRGBS_DU1   (1 << 4)    /* Digital RGB signal from DU1 is selected */
#define R_DU_DEF8R_DRGBS_DU2   (2 << 4)    /* Digital RGB signal from DU2 is selected */
#define R_DU_DEF8R_DRGBS_DU3   (3 << 4)    /* Digital RGB signal from DU3 is selected */

#define R_DU_DIDSR             0x00020028  /* Display unit input dot clock select reg offset */
#define R_DU_DIDSR_LDCS1       (1 << 11)   // DU1 LVDS-IF Dot Clock Select [R-Car D3/E3]
#define R_DU_DIDSR_LDCS0       (1 << 9)    // DU0 LVDS-IF Dot Clock Select [R-Car D3/E3]
#define R_DU_DIDSR_PDCS1       (1 << 2)    /* DU Pad Dot Clock Select */
#define R_DU_DIDSR_PDCS0       (1 << 0)    /* DU Pad Dot Clock Select */

#define R_DU_DEF10R            0x00020038  /* Display unit extensional function control 10 reg offset */
#define R_DU_DEF10R_CODE       (0x7795 << 16) /* Enabling Code */
#define R_DU_DEF10R_DEFE10     (1 << 0)    /* Display unit extensional function enable 10 */

#define R_DU_DDTHCR            0x0002003C  /* Display unit dither control  reg offset */

#define R_DU_DPLLCR            0x00020044  /* Display unit PLL control  reg offset */
#define R_DU_DPLLCR_INCS       (3 << 0)    /* DPLL Input Clock Select */
#define R_DU_DPLLCR_INCS_DOTCLKIN02 (0 << 0)    /* DPLL0/1 Input Clock is R_DU_DOTCLKIN0/2 pin */
#define R_DU_DPLLCR_INCS_DOTCLKIN13 (2 << 0)    /* DPLL0/1 Input Clock is R_DU_DOTCLKIN1/3 pin */

#define R_DU_DPLLC2R           0x00020048  /* Display unit PLL control 2 reg offset */

#define R_DU_HDSR              0x00000040  /* Horizontal display start reg offset */
#define R_DU_HDER              0x00000044  /* Horizontal display end reg offset */
#define R_DU_VDSR              0x00000048  /* Vertical display start reg offset */
#define R_DU_VDER              0x0000004C  /* Vertical display end reg offset */
#define R_DU_HCR               0x00000050  /* Horizontal cycle reg offset */
#define R_DU_HSWR              0x00000054  /* Horizontal sync width reg offset */
#define R_DU_VCR               0x00000058  /* Vertical cycle reg offset */
#define R_DU_VSPR              0x0000005C  /* Vertical sync point reg offset */
#define R_DU_EQWR              0x00000060  /* Equal pulse width reg offset */
#define R_DU_SPWR              0x00000064  /* Serration width reg offset */
#define R_DU_CLAMPSR           0x00000070  /* CLAMP signal start reg offset */
#define R_DU_CLAMPWR           0x00000074  /* CLAMP signal width reg offset */
#define R_DU_DESR              0x00000078  /* DE signal start reg offset */
#define R_DU_DEWR              0x0000007C  /* DE signal width reg offset */
#define R_DU_DOOR              0x00000090  /* Display off mode output reg offset */
#define R_DU_CDER              0x00000094  /* Color detection reg offset */
#define R_DU_BPOR              0x00000098  /* Background plane output reg offset */
#define R_DU_RINTOFSR          0x0000009C  /* Raster interrupt offset reg offset */

#define R_DU_PLANE_OFFSET(p)   ((p) * 0x100)
#define R_DU_PpMR              0x00000000  /* Plane p mode reg offset */
#define R_DU_PpMR_SPIM         (7 << 12)   /* Plane p SuperImpose Mode mask */
#define R_DU_PpMR_SPIM_OPAQUE  (4 << 12)   /* Transparent color processing is not performed */
#define R_DU_PpMR_SPIM_ALPHA   (5 << 12)   /* alpha blending of plane p and the lower is performed */
#define R_DU_PpMR_SPIM_EOR     (6 << 12)   /* EOR is performed */
#define R_DU_PpMR_16BPP        (1 << 0)    /* Display Data Format is 16-bit/pixel */
#define R_DU_PpMR_YC           (3 << 0)    /* Display Data Format is YC */

#define R_DU_PpDSXR            0x00000010  /* Plane p display size X reg offset */
#define R_DU_PpDSYR            0x00000014  /* Plane p display size Y reg offset */
#define R_DU_PpDPXR            0x00000018  /* Plane p display position X reg offset */
#define R_DU_PpDPYR            0x0000001C  /* Plane p display position Y reg offset */
#define R_DU_PpDDC4R           0x00000090  /* Plane p display data control 4 reg offset */
#define R_DU_PpDDC4R_CODE      (0x7766 << 16) /* Enabling code */
#define R_DU_PpDDC4R_SDFS      (7 << 4)    /* Superimpose Data Format Select mask */
#define R_DU_PpDDC4R_SDFS_RGB  (0 << 4)    /* RGB composite */
#define R_DU_PpDDC4R_SDFS_YC   (5 << 4)    /* YC composite */
#define R_DU_PpDDC4R_EDF       (7 << 0)    /* Superimpose Data Format Select mask */
#define R_DU_PpDDC4R_EDF_ARGB8888 (1 << 0) /* ARGB8888 */
#define R_DU_PpDDC4R_EDF_RGB888   (2 << 0) /* RGB888 (not support) */
#define R_DU_PpDDC4R_EDF_RGB666   (3 << 0) /* RGB666 (not support)*/
#define R_DU_PpDDC4R_EDF_YCBCR444 (4 << 0) /* YCbCr444 (not support)*/

#define R_DU_ESCR02            0x00010000  /* DU0/2 External synchronization control reg offset */
#define R_DU_ESCR13            0x00001000  /* DU1/3 External synchronization control reg offset */
#define R_DU_ESCR_DCLKSEL      (1 << 20)   /* DCLKIN Select */

#define R_DU_OTAR02            0x00010004  /* DU0/2 External synchronization control reg offset */
#define R_DU_OTAR13            0x00001004  /* DU1/3 External synchronization control reg offset */

#define R_DU_DORCR             0x00011000  /* Display unit output route control reg offset */
#define R_DU_DORCR_DPRS        (1 << 0)    /* Display Pritority Register Select */

#define R_DU_DPTSR             0x00011004  /* Display plane timing select reg offset */
#define R_DU_DS0PR             0x00011020  /* Display superimpose 0 priority reg offset */
#define R_DU_DS1PR             0x00011024  /* Display superimpose 1 priority reg offset */
#define R_DU_YNCR              0x00014080  /* Y normalization coefficient reg offset */
#define R_DU_YNOR              0x00014084  /* Y normalization offset reg offset */
#define R_DU_CRNOR             0x00014088  /* Cr normalization offset reg offset */
#define R_DU_CBNOR             0x0001408C  /* Cb normalization offset reg offset */
#define R_DU_RCRCR             0x00014090  /* Red Cr coefficient reg offset */
#define R_DU_GCRCR             0x00014094  /* Green Cr coefficient reg offset */
#define R_DU_GCBCR             0x00014098  /* Green Cb coefficient reg offset */
#define R_DU_BCBCR             0x0001409C  /* Blue Cb coefficient reg offset */


/**
 * General purpose function to read a DU register
 *
 * @param: Address -the register address to read from
 * @return: value of the register
 */
uint32_t R_DU_PRV_ReadReg(uintptr_t Address);

/**
 * General purpose function to Write to a DU register
 *
 * @param: Address -the register address to write to
 * @param: Value - value to be written to the given register
 */
void R_DU_PRV_WriteReg(uintptr_t Address, uint32_t Value);

/**
 * General purpose function to get the register base address of a
 * given display unit
 *
 * @param: DuUnit - display unit index
 * @return: Register base address
 */
uintptr_t R_DU_PRV_GetRegBase(r_du_Unit_t DuUnit);

/**
 * General purpose function to get the base address of a register
 * group. Some registers are shared between units.
 *
 * @param: DuUnit - display unit index
 * @return: Register group base address
 */
uintptr_t R_DU_PRV_GetGroupRegBase(r_du_Unit_t DuUnit);

#ifdef __cplusplus
}
#endif

#endif /* R_DU_REGS_H_ */
