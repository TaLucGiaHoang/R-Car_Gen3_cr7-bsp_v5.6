/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_audio_regs.h"

void writel(const uint32_t Value, const uintptr_t Address)
{
       *((volatile unsigned int*) Address)  = Value;
}

uint32_t readl(const uintptr_t Address)
{
    return *((volatile unsigned int*)Address);
}

