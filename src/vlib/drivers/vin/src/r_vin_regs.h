/*************************************************************************************************************
* vin_regs
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VIN_REGS_H_
#define R_VIN_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define R_VIN0_BASE (0xE6EF0000)
#define R_VIN1_BASE (0xE6EF1000)
#define R_VIN2_BASE (0xE6EF2000)
#define R_VIN3_BASE (0xE6EF3000)
#define R_VIN4_BASE (0xE6EF4000)
#define R_VIN5_BASE (0xE6EF5000)
#define R_VIN6_BASE (0xE6EF6000)
#define R_VIN7_BASE (0xE6EF7000)

#define R_VIN_VMC       (0x0000)      // R/W  32      Video n main control register
#define R_VIN_VMS       (0x0004)      // R    32      Video n module status register
#define R_VIN_VFC       (0x0008)      // R/W  32      Video n frame capture register
#define R_VIN_VSLPrC    (0x000C)      // R/W  32      Video n start line pre-clip register
#define R_VIN_VELPrC    (0x0010)      // R/W  32      Video n end line pre-clip register
#define R_VIN_VSPPrC    (0x0014)      // R/W  32      Video n start pixel pre-clip register
#define R_VIN_VEPPrC    (0x0018)      // R/W  32      Video n end pixel pre-clip register
#define R_VIN_VCSI_IFMD (0x0020)      // R/W  32      Video n CSI input control register
#define R_VIN_VIS       (0x002C)      // R/W  32      Video n image stride register
#define R_VIN_VMB1      (0x0030)      // R/W  32      Video n memory base 1 register
#define R_VIN_VMB2      (0x0034)      // R/W  32      Video n memory base 2 register
#define R_VIN_VMB3      (0x0038)      // R/W  32      Video n memory base 3 register
#define R_VIN_VLC       (0x003C)      // R    32      Video n line count register
#define R_VIN_VIE       (0x0040)      // R/W  32      Video n interrupt enable register
#define R_VIN_VINTS     (0x0044)      // R/W  32      Video n interrupt status register
#define R_VIN_VSI       (0x0048)      // R/W  32      Video n scanline interrupt register
#define R_VIN_VDMR      (0x0058)      // R/W  32      Video n data mode register
#define R_VIN_VDMR2     (0x005C)      // R/W  32      Video n data mode register 2
#define R_VIN_VUVAOF    (0x0060)      // R/W  32      Video n UV address offset register
#define R_VIN_VCSCC1    (0x0064)      // R/W  32      Video n color space change coefficient 1 register
#define R_VIN_VCSCC2    (0x0068)      // R/W  32      Video n color space change coefficient 2 register
#define R_VIN_VCSCC3    (0x006C)      // R/W  32      Video n color space change coefficient 3 register
#define R_VIN_VUDS_CTRL (0x0080)      // R/W  32      Video n scaling control register
#define R_VIN_VUDS_SCALE    (0x0084)  // R/W  32      Video n scaling factor register
#define R_VIN_VUDS_PASS_BWIDTH  (0x0090)      // R/W  32      Video n passband register
#define R_VIN_VUDS_IPC  (0x0098)      // R/W  32      Video n 2D IPC setting register
#define R_VIN_VUDS_CLIP_SIZE    (0x00A4)      // R/W  32      Video n UDS output size clipping register
#define R_VIN_VLUTP     (0x0100)      // R/W  32      Video n lookup table pointer
#define R_VIN_VLUTD     (0x0104)      // R/W  32      Video n lookup table data register
#define R_VIN_VYCCR1    (0x0228)      // R/W  32      Video n RGB->Y calculation setting register 1
#define R_VIN_VYCCR2    (0x022C)      // R/W  32      Video n RGB->Y calculation setting register 2
#define R_VIN_VYCCR3    (0x0230)      // R/W  32      Video n RGB->Y calculation setting register 3
#define R_VIN_VCBCCR1   (0x0234)      // R/W  32      Video n RGB->Cb calculation setting register 1
#define R_VIN_VCBCCR2   (0x0238)      // R/W  32      Video n RGB->Cb calculation setting register 2
#define R_VIN_VCBCCR3   (0x023C)      // R/W  32      Video n RGB->Cb calculation setting register 3
#define R_VIN_VCRCCR1   (0x0240)      // R/W  32      Video n RGB->Cr calculation setting register 1
#define R_VIN_VCRCCR2   (0x0244)      // R/W  32      Video n RGB->Cr calculation setting register 2
#define R_VIN_VCRCCR3   (0x0248)      // R/W  32      Video n RGB->Cr calculation setting register 3
#define R_VIN_VCSCE1    (0x0300)      // R/W  32      Video n YC->RGB calculation setting register 1
#define R_VIN_VCSCE2    (0x0304)      // R/W  32      Video n YC->RGB calculation setting register 2
#define R_VIN_VCSCE3    (0x0308)      // R/W  32      Video n YC->RGB calculation setting register 3
#define R_VIN_VCSCE4    (0x030C)      // R/W  32      Video n YC->RGB calculation setting register 4

#ifdef __cplusplus
}
#endif

#endif /* R_VIN_REGS_H_ */
