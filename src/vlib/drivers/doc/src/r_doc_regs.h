/*************************************************************************************************************
* doc_regs_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef R_DOC_REGS_H_
#define R_DOC_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Section Includes
 */
#include "r_doc_api.h"

/**
 * Section Defines
 */

/**
 * DOC Module register base addresses
 */
#define R_DOC0_BASE             0xFEBA0000  /* RCAR_D3, E3, H3, M3-W, M3-W+, H3-N, M3-N */
#define R_DOC1_BASE             0xFEBB8000  /* RCAR_H3, M3-W, M3-W+, H3-N, M3-N */

/**
 * DOC register definition
 */
#define DOCnSTR                 0x0000
#define DOCnCTL                 0x0004
#define DOCnEN                  0x0008
#define DOCnCH                  0x000C
#define DOCnTIME(t)             (0x0010 + 0x0004*t)
#define DOCnOFFS(t)             (0x0020 + 0x0004*t)
#define DOCnDISP(t)             (0x0028 + 0x0004*t)
#define DOCnACT(t)              (0x0030 + 0x0004*t)
#define DOCnDIFF                0x0040
#define DOCnYCMODE(t)           (0x0060 + 0x0004*t)
#define DOCnMmCFG0(m)           (0x0100 + 0x0020*m)
#define DOCnMmCFG1(m)           (0x0104 + 0x0020*m)
#define DOCnMmCFG2(m)           (0x0108 + 0x0020*m)
#define DOCnMmCFG3(m)           (0x010C + 0x0020*m)
#define DOCnMmCFG4(m)           (0x0110 + 0x0020*m)
#define DOCnMmCFG5(m)           (0x0114 + 0x0020*m)
#define DOCnMmCFG6(m)           (0x0118 + 0x0020*m)
#define DOCnMmCFG7(m)           (0x011C + 0x0020*m)
#define DOCnEXPD0000to4095(k)   (0x0300 + 0x0004*k)
#define DOCnEXPD4096to16383(k)  (0x4300 + 0x0004*(k-4096))

#define DOCCH_VOCANCH_MASK       0x0000FFFF

#define DOCSTR_VOCANVOCSTR       0x00400000
#define DOCSTR_VOCANE18          0x00800000
#define DOCSTR_VOCANE17          0x00020000
#define DOCSTR_VOCANE16          0x00010000
#define DOCSTR_VOCANEM           0x0000FFFF
#define DOCSTR_VOCANACTSTR       0x01000000

#define DOCEN_VOCANEN_MASK       0x0000FFFF
#define DOCEN_VOCANEN16_ENABLE   0x00010000

#define DOCCTL_VOCANMKINT        0x40000000
#define DOCCTL_VOCANCL18         0x00040000
#define DOCCTL_VOCANCL17         0x00020000
#define DOCCTL_VOCANCL16         0x00010000
#define DOCCTL_VOCANCLM          0x0000FFFF

/**
 * Section Global Functions
 */
uint32_t R_DOC_PRV_RegRead(uintptr_t Address);
void R_DOC_PRV_RegWrite(uintptr_t Address, uint32_t Value);
uintptr_t R_DOC_PRV_GetRegBase(const r_doc_Unit_t DocUnit);

#ifdef __cplusplus
}
#endif

#endif /* R_DOC_REGS_H_ */
