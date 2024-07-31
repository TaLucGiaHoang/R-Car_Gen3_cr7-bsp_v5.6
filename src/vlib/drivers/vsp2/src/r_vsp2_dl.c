/*************************************************************************************************************
* vsp2_dl_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_api.h"
#include "r_vsp2_dl.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"

uintptr_t r_vsp2_ModuleDlAddr[R_VSP2_LAST];

r_vsp2_Error_t R_VSP2_PRV_DlInit(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    int i;
    uint32_t offset;
    uint32_t *p;
    r_vsp2_DlHeader_t *dl_header = 0;
    uintptr_t vsp2_reg_base;

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_DlInit] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else {
        if ((WpfId != 0) && (WpfId != 1)) {
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_PRV_DlInit] : Wpf number is Invalid. Failed(%d)\r\n", e);
        }
    }

    if (0 != r_vsp2_ModuleDlAddr[Vsp2Unit]) {
        /* use DL */
        vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

        /* Init Display List header */
        dl_header = (r_vsp2_DlHeader_t *)r_vsp2_ModuleDlAddr[Vsp2Unit];
        dl_header->num_list_minus1 = 0;
        for (i = 0; i < 8; i++) {
            dl_header->lists[i].num_bytes = 0;
            dl_header->lists[i].list_addr = 0;
        }
        dl_header->next_header_addr = (uintptr_t)dl_header;
        dl_header->frame_int_en_next_auto = (1 << 1) | (1 << 0);


        dl_header->lists[0].num_bytes = 8;
        dl_header->lists[0].list_addr = (uintptr_t)dl_header + sizeof(r_vsp2_DlHeader_t);
        dl_header->lists[0].list_addr += 0x00000010;
        dl_header->lists[0].list_addr &= 0xfffffff0;

        p = (uint32_t *)(uintptr_t)dl_header->lists[0].list_addr;

        *p = 0x00000310;
        *(p + 1) = 0x00000000;
        p += 0x2;

        /* Display Liste header address */
        if (WpfId == 0) {
            offset = R_VSP2_VI6_DL_HDR_ADDR0;
        } else {
            offset = R_VSP2_VI6_DL_HDR_ADDR1;
        }
        R_VSP2_PRV_RegWrite(vsp2_reg_base + offset, (uint32_t)((uintptr_t)dl_header & 0xfffffff0));

        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DL_SWAP0, 0x4);

        /* Enable Display List for WPF0 */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DL_CTRL, (256 << 16) | (1 << 0));

    }

    return e;
}
