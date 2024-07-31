/*************************************************************************************************************
* csi2_regs
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef R_CSI2_REGS_H_
#define R_CSI2_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RCAR_CSI2_TREF                                   (0x0000)      // R/W  32
#define RCAR_CSI2_SRST                                   (0x0004)      // R/W  32
#define RCAR_CSI2_PHYCNT                                 (0x0008)      // R/W  32
#define RCAR_CSI2_CHKSUM                                 (0x000C)      // R/W  32
#define RCAR_CSI2_VCDT                                   (0x0010)      // R/W  32
#define RCAR_CSI2_VCDT2                                  (0x0014)      // R/W  32
#define RCAR_CSI2_FRDT                                   (0x0018)      // R/W  32
#define RCAR_CSI2_FLD                                    (0x001C)      // R/W  32
#define RCAR_CSI2_ASTBY                                  (0x0020)      // R/W  32
#define RCAR_CSI2_OUT                                    (0x0024)      // R/W  32
#define RCAR_CSI2_LNGDT0                                 (0x0028)      // R/W  32
#define RCAR_CSI2_LNGDT1                                 (0x002C)      // R/W  32
#define RCAR_CSI2_INTEN                                  (0x0030)      // R/W  32
#define RCAR_CSI2_INTCLOSE                               (0x0034)      // R/W  32
#define RCAR_CSI2_INTSTATE                               (0x0038)      // R/(W)32
#define RCAR_CSI2_INTERRSTATE                            (0x003C)      // R/(W)32
#define RCAR_CSI2_SHPDAT                                 (0x0040)      // R    32
#define RCAR_CSI2_SHPCNT                                 (0x0044)      // R    32
#define RCAR_CSI2_LINKCNT                                (0x0048)      // R/W  32
#define RCAR_CSI2_LSWAP                                  (0x004C)      // R/W  32
#define RCAR_CSI2_PHTW                                   (0x0050)      // R/W  32
#define RCAR_CSI2_PHTR                                   (0x0054)      // R/W  32
#define RCAR_CSI2_PHTC                                   (0x0058)      // R/W  32
#define RCAR_CSI2_PHYPLL                                 (0x0068)      // R/W  32
#define RCAR_CSI2_PHEERM                                 (0x0074)      // R    32
#define RCAR_CSI2_PHCLM                                  (0x0078)      // R    32
#define RCAR_CSI2_PHDLM                                  (0x007C)      // R    32
#define RCAR_CSI2_PH0M0                                  (0x00F0)      // R    32
#define RCAR_CSI2_PH0M1                                  (0x00F4)      // R    32
#define RCAR_CSI2_PH1M0                                  (0x00F8)      // R    32
#define RCAR_CSI2_PH1M1                                  (0x00FC)      // R    32
#define RCAR_CSI2_PH2M0                                  (0x0100)      // R    32
#define RCAR_CSI2_PH2M1                                  (0x0104)      // R    32
#define RCAR_CSI2_PH3M0                                  (0x0108)      // R    32
#define RCAR_CSI2_PH3M1                                  (0x010C)      // R    32
#define RCAR_CSI2_PHRM0                                  (0x0110)      // R    32
#define RCAR_CSI2_PHRM1                                  (0x0114)      // R    32
#define RCAR_CSI2_PHRM2                                  (0x0118)      // R    32
#define RCAR_CSI2_PHCM0                                  (0x0120)      // R    32
#define RCAR_CSI2_PHCM1                                  (0x0124)      // R    32
#define RCAR_CSI2_CRCM0                                  (0x0128)      // R    32
#define RCAR_CSI2_CRCM1                                  (0x012C)      // R    32
#define RCAR_CSI2_SERRCNT                                (0x0140)      // R    32
#define RCAR_CSI2_SSERRCNT                               (0x0144)      // R    32
#define RCAR_CSI2_ECCCM                                  (0x0148)      // R    32
#define RCAR_CSI2_ECECM                                  (0x014C)      // R    32
#define RCAR_CSI2_CRCECM                                 (0x0150)      // R    32
#define RCAR_CSI2_LCNT                                   (0x0160)      // R    32
#define RCAR_CSI2_LCNT2                                  (0x0164)      // R    32
#define RCAR_CSI2_LCNTM                                  (0x0168)      // R    32
#define RCAR_CSI2_LCNTM2                                 (0x016C)      // R    32
#define RCAR_CSI2_FCNTM                                  (0x0170)      // R    32
#define RCAR_CSI2_PHYDIM                                 (0x0180)      // R    32
#define RCAR_CSI2_PHYIM                                  (0x0184)      // R    32
#define RCAR_CSI2_RCUDM                                  (0x018C)      // R    32
#define RCAR_CSI2_RCUSM1                                 (0x0190)      // R    32
#define RCAR_CSI2_RCUSM2                                 (0x0194)      // R    32
#define RCAR_CSI2_RCUSM3                                 (0x0198)      // R    32
#define RCAR_CSI2_PHYOM                                  (0x019C)      // R    32
#define RCAR_CSI2_PHM1                                   (0x01C0)      // R    32
#define RCAR_CSI2_PHM2                                   (0x01C4)      // R    32
#define RCAR_CSI2_PHM3                                   (0x01C8)      // R    32
#define RCAR_CSI2_PHM4                                   (0x01CC)      // R    32
#define RCAR_CSI2_PHM5                                   (0x01D0)      // R    32
#define RCAR_CSI2_PHM6                                   (0x01D4)      // R    32
#define RCAR_CSI2_PHM7                                   (0x01D8)      // R    32
#define RCAR_CSI2_PHM8                                   (0x01DC)      // R    32
#define RCAR_CSI2_PPM1                                   (0x01E0)      // R    32
#define RCAR_CSI2_PPM2                                   (0x01E4)      // R    32
#define RCAR_CSI2_PPM3                                   (0x01E8)      // R    32
#define RCAR_CSI2_PPM4                                   (0x01EC)      // R    32
#define RCAR_CSI2_PPM5                                   (0x01F0)      // R    32
#define RCAR_CSI2_PPM6                                   (0x01F4)      // R    32
#define RCAR_CSI2_PPM7                                   (0x01F8)      // R    32
#define RCAR_CSI2_PPM8                                   (0x01FC)      // R    32
#define RCAR_CSI2_CSI0CLKFCPR                            (0x0260)      // R    32      CSI0CLK Frequency Configu

#ifdef __cplusplus
}
#endif

#endif /* R_CPG_API_H_ */
