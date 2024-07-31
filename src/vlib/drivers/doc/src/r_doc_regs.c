/*************************************************************************************************************
* doc_regs_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**
PROJECT : CR7 RGL
FILE   : $Id: r_doc_regs.h
Created by :
Version :
Update :
  - 16.11.2020 created
============================================================================
DESCRIPTION
DOC Register Access Interface
============================================================================*/

/**
 * Section Includes
 */
#include "r_doc_regs.h"
#include <stdint.h>

/**
 * Section global functions
 */
uint32_t R_DOC_PRV_RegRead(uintptr_t Address)
{
    return *((volatile unsigned int*)Address);
}

void R_DOC_PRV_RegWrite(uintptr_t Address, uint32_t Value)
{
    *((volatile unsigned int*) Address)  = Value;
}

uintptr_t R_DOC_PRV_GetRegBase(const r_doc_Unit_t DocUnit)
{
    uintptr_t base_address = 0;
    switch (DocUnit)
    {
    case R_DOC_DOC0:
        base_address = R_DOC0_BASE;
        break;
    case R_DOC_DOC1:
        base_address = R_DOC1_BASE;
        break;
    default:
        break;
    }
    return base_address;
}
