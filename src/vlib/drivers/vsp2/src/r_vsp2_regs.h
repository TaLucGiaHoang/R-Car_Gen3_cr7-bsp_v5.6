/*************************************************************************************************************
* vsp2_regs_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_REGS_H_
#define R_VSP2_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "r_vsp2_api.h"

/**
 * VSP2 Module register base addresses
 */
#define R_VSPBC_BASE                          0xFE920000  // RCAR_H3
#define R_VSPBD_BASE                          0xFE960000  // RCAR_H3
#define R_VSPB_BASE                           0xFE960000  // RCAR_M3W, RCAR_M3N, RCAR_E3
#define R_VSPBS_BASE                          0xFE960000  // RCAR_D3
#define R_VSPI0_BASE                          0xFE9A0000  // RCAR_H3, RCAR_M3W, RCAR_M3N, RCAR_E3
#define R_VSPI1_BASE                          0xFE9B0000  // RCAR_H3
#define R_VSPD0_BASE                          0xFEA20000  //
#define R_VSPDL_BASE                          R_VSPD0_BASE  // VSPDL is VSPD0 for RCAR_H3 and RCAR_M3N
#define R_VSPD1_BASE                          0xFEA28000  // RCAR_H3, RCAR_M3W, RCAR_D3, RCAR_M3N, RCAR_E3
#define R_VSPD2_BASE                          0xFEA30000  // RCAR_H3, RCAR_M3W

/**
 * VSP2 CMD register definition
 */
#define R_VSP2_VI6_CMD0                       0x0000
#define R_VSP2_VI6_CMD1                       0x0004
#define R_VSP2_VI6_CMD_STRCMD                 (1 << 0)

/**
 * VSP2 CONTROL registers
 */
#define R_VSP2_VI6_STATUS                     0x0038

/**
 * VSP2 clock related definitions
 */
#define R_VSP2_VI6_CLK_CTRL0                  0x0010
#define R_VSP2_VI6_CLK_CTRL1                  0x0014
#define R_VSP2_VI6_CLK_DCSWT                  0x0018
#define R_VSP2_VI6_CLK_DCSM0                  0x001c
#define R_VSP2_VI6_CLK_DCSM1                  0x0020

/**
 * VSP2 reset related definitions
 */
#define R_VSP2_VI6_SRESET                     0x0028
#define R_VSP2_VI6_SRESET_SRST1               (1 << 1)
#define R_VSP2_VI6_SRESET_SRST0               (1 << 0)

#define R_VSP2_VI6_MRESET_ENB0                0x002c
#define R_VSP2_VI6_MRESET_ENB1                0x0030
#define R_VSP2_VI6_MRESET                     0x0034

/**
 * VSP2 WPF IRQ related definitions
 */
#define R_VSP2_VI6_WPF0_IRQ_ENB               0x0048
#define R_VSP2_VI6_WPF0_IRQ_STA               0x004c
#define R_VSP2_VI6_WPF1_IRQ_ENB               0x0054
#define R_VSP2_VI6_WPF1_IRQ_STA               0x0058
#define R_VSP2_VI6_WPF_IRQ_STA_UND            (1 << 16)
#define R_VSP2_VI6_WPF_IRQ_STA_DFE            (1 << 1)
#define R_VSP2_VI6_WPF_IRQ_STA_FRE            (1 << 0)

/**
 * VSP2 display list related definitions
 */
#define R_VSP2_VI6_DL_CTRL                    0x0100
#define R_VSP2_VI6_DL_HDR_ADDR0               0x0104
#define R_VSP2_VI6_DL_HDR_ADDR1               0x0108
#define R_VSP2_VI6_DL_SWAP0                   0x0114
#define R_VSP2_VI6_DL_EXT_CTRL0               0x011c
#define R_VSP2_VI6_DL_BODY_SIZE0              0x0120
#define R_VSP2_VI6_DL_EXT_CTRL1               0x0140
#define R_VSP2_VI6_VI6_DL_SWAP0               0x014c

/**
 * VSP2 RPF related definitions
 */
#define R_VSP2_VI6_RPFn_SRC_BSIZE(n)          (0x0300 + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRC_ESIZE(n)          (0x0304 + 0x100 * n)

#define R_VSP2_VI6_RPFn_INFMT(n)              (0x0308 + 0x100 * n)
#define R_VSP2_VI6_RPFn_INFMT_VIR             (1 << 28)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT           (0x7f << 0) //mask
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_RGB332    (0x00 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_XRGB4444  (0x01 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_XRGB1555  (0x04 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_RGB565    (0x06 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_ARGB8888  (0x13 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_RGBA8888  (0x14 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_RGB888    (0x15 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_BGR888    (0x18 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_ARGB4444  (0x19 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_ARGB1555  (0x1B << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV444    (0x40 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV422    (0x41 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV420    (0x42 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV444ITL (0x46 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV422ITL (0x47 << 0)
#define R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV420ITL (0x49 << 0)

#define R_VSP2_VI6_RPFn_DSWAP(n)              (0x030c + 0x100 * n)
#define R_VSP2_VI6_RPFn_LOC(n)                (0x0310 + 0x100 * n)
#define R_VSP2_VI6_RPFn_ALPH_SEL(n)           (0x0314 + 0x100 * n)
#define R_VSP2_VI6_RPFn_VRTCOL_SET(n)         (0x0318 + 0x100 * n)
#define R_VSP2_VI6_RPFn_MSKCTRL(n)            (0x031C + 0x100 * n)
#define R_VSP2_VI6_RPFn_MSKSET0(n)            (0x0320 + 0x100 * n)
#define R_VSP2_VI6_RPFn_MSKSET1(n)            (0x0324 + 0x100 * n)
#define R_VSP2_VI6_RPFn_CKEY_CTRL(n)          (0x0328 + 0x100 * n)
#define R_VSP2_VI6_RPFn_CKEY_SET0(n)          (0x032C + 0x100 * n)
#define R_VSP2_VI6_RPFn_CKEY_SET1(n)          (0x0330 + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRCM_PSTRIDE(n)       (0x0334 + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRCM_ASTRIDE(n)       (0x0338 + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRCM_ADDR_Y(n)        (0x033c + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRCM_ADDR_C0(n)       (0x0340 + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRCM_ADDR_C1(n)       (0x0344 + 0x100 * n)
#define R_VSP2_VI6_RPFn_SRCM_ADDR_AI(n)       (0x0348 + 0x100 * n)
#define R_VSP2_VI6_RPFn_MULT_ALPH(n)          (0x036C + 0x100 * n)

/**
 * VSP2 WPF related definitions
 */
#define R_VSP2_VI6_WPFn_SRCRPF(n)             (0x1000 + 0x100 * n)
#define R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT        (3 << 28)
#define R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT_SUB    (1 << 28)
#define R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT_MASTER (2 << 28)
#define R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT2        (3 << 24)
#define R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT2_SUB    (1 << 24)
#define R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT2_MASTER (2 << 24)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF4_ACT        (3 << 8)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF4_ACT_SUB    (1 << 8)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF4_ACT_MASTER (2 << 8)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF3_ACT        (3 << 6)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF3_ACT_SUB    (1 << 6)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF3_ACT_MASTER (2 << 6)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF2_ACT        (3 << 4)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF2_ACT_SUB    (1 << 4)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF2_ACT_MASTER (2 << 4)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF1_ACT        (3 << 2)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF1_ACT_SUB    (1 << 2)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF1_ACT_MASTER (2 << 2)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF0_ACT        (3 << 0)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF0_ACT_SUB    (1 << 0)
#define R_VSP2_VI6_WPFn_SRCRPF_RPF0_ACT_MASTER (2 << 0)

#define R_VSP2_VI6_WPF0_ROT_CTRL              0x1018
#define R_VSP2_VI6_WPFn_HSZCLIP(n)            (0x1004 + 0x100 * n)
#define R_VSP2_VI6_WPFn_VSZCLIP(n)            (0x1008 + 0x100 * n)

#define R_VSP2_VI6_WPFn_OUTFMT(n)             (0x100c + 0x100 * n)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT          (0x7f << 0) //mask
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGB332   (0x00 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_XRGB4444 (0x01 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_XRGB1555 (0x04 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGB565   (0x06 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB8888 (0x13 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGBA8888 (0x14 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGB888   (0x15 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_BGR888   (0x18 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB4444 (0x19 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB1555 (0x1B << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV444   (0x40 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV422   (0x41 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV420   (0x42 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV444ITL (0x46 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV422ITL (0x47 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV420ITL (0x49 << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV444P  (0x4A << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV422P  (0x4B << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV420P  (0x4C << 0)
#define R_VSP2_VI6_WPFn_OUTFMT_PXA_PDV        (0 << 23)
#define R_VSP2_VI6_WPFn_OUTFMT_PXA_DPR        (1 << 23)

#define R_VSP2_VI6_WPFn_DSTM_STRIDE_Y(n)      (0x101c + 0x100 * n)
#define R_VSP2_VI6_WPFn_DSTM_STRIDE_C(n)      (0x1020 + 0x100 * n)
#define R_VSP2_VI6_WPFn_DSTM_ADDR_Y(n)        (0x1024 + 0x100 * n)
#define R_VSP2_VI6_WPFn_DSTM_ADDR_C0(n)       (0x1028 + 0x100 * n)
#define R_VSP2_VI6_WPFn_DSTM_ADDR_C1(n)       (0x102c + 0x100 * n)

#define R_VSP2_VI6_WPF1_WRBCK_CTRL(n)         (0x1034 + 0x100 * n)

#define R_VSP2_VI6_WPFn_DSWAP(n)              (0x1010 + 0x100 * n)

/**
 * VSP2 LIF related definitions
 */
#define R_VSP2_VI6_LIF0_CTRL                  0x3b00
#define R_VSP2_VI6_LIF_CTRL_OBTH_VSPD         (3000u << 16)
#define R_VSP2_VI6_LIF_CTRL_OBTH_VSPDL        (1500u << 16)

#define R_VSP2_VI6_LIF0_LBA                   0x3b0c
#define R_VSP2_VI6_LIF0_CSBTH                 0x3b04
#define R_VSP2_VI6_LIF1_CTRL                  0x3a00
#define R_VSP2_VI6_LIF1_CSBTH                 0x3a04

/**
 * VSP2 BRU related definitions
 */
#define R_VSP2_VI6_BRU_VIRRPF_SIZE            0x2c04
#define R_VSP2_VI6_BRU_VIRRPF_LOC             0x2c08
#define R_VSP2_VI6_BRU_VIRRPF_COL             0x2c0c
#define R_VSP2_VI6_BRUA_CTRL                  0x2c10
#define R_VSP2_VI6_BRUA_BLD                   0x2c14
#define R_VSP2_VI6_BRUB_CTRL                  0x2c18
#define R_VSP2_VI6_BRUB_BLD                   0x2c1c
#define R_VSP2_VI6_BRUC_CTRL                  0x2c20
#define R_VSP2_VI6_BRUC_BLD                   0x2c24
#define R_VSP2_VI6_BRUD_CTRL                  0x2c28
#define R_VSP2_VI6_BRUD_BLD                   0x2c2c
#define R_VSP2_VI6_BRU_ROP                    0x2c30
#define R_VSP2_VI6_BRUE_CTRL                  0x2c34
#define R_VSP2_VI6_BRUE_BLD                   0x2c38

/**
 * VSP2 BRS related definitions
 */
#define R_VSP2_VI6_BRS_VIRRPF_SIZE            0x3904
#define R_VSP2_VI6_BRS_VIRRPF_LOC             0x3908
#define R_VSP2_VI6_BRS_VIRRPF_COL             0x390c
#define R_VSP2_VI6_BRSA_CTRL                  0x3910
#define R_VSP2_VI6_BRSA_BLD                   0x3914
#define R_VSP2_VI6_BRSB_CTRL                  0x3918
#define R_VSP2_VI6_BRSB_BLD                   0x391c

/**
 * VSP2 DPR related definitions
 */
#define R_VSP2_VI6_DPR_RPF0_ROUTE             0x2000
#define R_VSP2_VI6_DPR_RPF1_ROUTE             0x2004
#define R_VSP2_VI6_DPR_RPF2_ROUTE             0x2008
#define R_VSP2_VI6_DPR_RPF3_ROUTE             0x200c
#define R_VSP2_VI6_DPR_RPF4_ROUTE             0x2010
#define R_VSP2_VI6_DPR_WPF0_FPORCH            0x2014
#define R_VSP2_VI6_DPR_WPF1_FPORCH            0x2018
#define R_VSP2_VI6_DPR_SRU_ROUTE              0x2024
#define R_VSP2_VI6_DPR_UDS_ROUTE              0x2028
#define R_VSP2_VI6_DPR_LUT_ROUTE              0x203c
#define R_VSP2_VI6_DPR_CLU_ROUTE              0x2040
#define R_VSP2_VI6_DPR_HST_ROUTE              0x2044
#define R_VSP2_VI6_DPR_HSI_ROUTE              0x2048
#define R_VSP2_VI6_DPR_BRU_ROUTE              0x204c
#define R_VSP2_VI6_DPR_ILV_BRS_ROUTE          0x2050
#define R_VSP2_VI6_DPR_SHP_ROUTE              0x2060
#define R_VSP2_VI6_DPR_UIF4_ROUTE             0x2074
#define R_VSP2_VI6_DPR_UIF5_ROUTE             0x2078

/**
 * VSP2 DISCOM related definitions
 */
/* UIF4 */
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCR             	0x1C00
#define R_VSP2_VI6_UIF4_DISCOM_DOCMSTR           	0x1C04
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCLSTR          	0x1C08
#define R_VSP2_VI6_UIF4_DISCOM_DOCMIENR           	0x1C0C
#define R_VSP2_VI6_UIF4_DISCOM_DOCMMDR            	0x1C10
#define R_VSP2_VI6_UIF4_DISCOM_DOCMPMR            	0x1C14
#define R_VSP2_VI6_UIF4_DISCOM_DOCMECRCR          	0x1C18
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCCRCR          	0x1C1C
#define R_VSP2_VI6_UIF4_DISCOM_DOCMSPXR           	0x1C20
#define R_VSP2_VI6_UIF4_DISCOM_DOCMSPYR           	0x1C24
#define R_VSP2_VI6_UIF4_DISCOM_DOCMSZXR           	0x1C28
#define R_VSP2_VI6_UIF4_DISCOM_DOCMSZYR           	0x1C2C
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCRCIR          	0x1C30
#define R_VSP2_VI6_UIF4_DISCOM_DOCMSTR_CMPST_CMPI    (1 << 0)
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCLSTR_CMPST_CLEAR (1 << 0)
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCR_CMPRU        	0x00010000
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCR_CMPR_EXECUTE 	0x00000001
#define R_VSP2_VI6_UIF4_DISCOM_DOCMCR_CMPR_STOP    	0x00000000
/* UIF5 */
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCR             	0x1D00
#define R_VSP2_VI6_UIF5_DISCOM_DOCMSTR           	0x1D04
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCLSTR          	0x1D08
#define R_VSP2_VI6_UIF5_DISCOM_DOCMIENR           	0x1D0C
#define R_VSP2_VI6_UIF5_DISCOM_DOCMMDR            	0x1D10
#define R_VSP2_VI6_UIF5_DISCOM_DOCMPMR            	0x1D14
#define R_VSP2_VI6_UIF5_DISCOM_DOCMECRCR          	0x1D18
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCCRCR          	0x1D1C
#define R_VSP2_VI6_UIF5_DISCOM_DOCMSPXR           	0x1D20
#define R_VSP2_VI6_UIF5_DISCOM_DOCMSPYR           	0x1D24
#define R_VSP2_VI6_UIF5_DISCOM_DOCMSZXR           	0x1D28
#define R_VSP2_VI6_UIF5_DISCOM_DOCMSZYR           	0x1D2C
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCRCIR          	0x1D30
#define R_VSP2_VI6_UIF5_DISCOM_DOCMSTR_CMPST_CMPI    (1 << 0)
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCLSTR_CMPST_CLEAR (1 << 0)
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCR_CMPRU        	0x00010000
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCR_CMPR_EXECUTE 	0x00000001
#define R_VSP2_VI6_UIF5_DISCOM_DOCMCR_CMPR_STOP    	0x00000000

uint32_t R_VSP2_PRV_RegRead(uintptr_t Address);
void R_VSP2_PRV_RegWrite(uintptr_t Address, uint32_t Value);
uintptr_t R_VSP2_PRV_GetRegBase(r_vsp2_Unit_t Vsp2Unit);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_REGS_H_ */
