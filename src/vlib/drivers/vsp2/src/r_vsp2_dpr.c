/*************************************************************************************************************
* vsp2_dpr_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_dpr.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"

#define DISCOM_ENABLE_ROOT_NUM 6

static r_vsp2_Dpr_Route_t FirstDprSettings[DISCOM_ENABLE_ROOT_NUM] = {0};

/**
 * Note: This function needs to be clean-up
 * we shall have a default intialisation which is valid for all VSP2 unit
 * then a switch case for to initialise the specific modules needed by a given
 * VSP2 Unit
 */
r_vsp2_Error_t R_VSP2_PRV_DprInit(r_vsp2_Unit_t Vsp2Unit, r_vsp2_Dpr_Conf_t *Conf)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_DprInit] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else if (0 == Conf) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_DprInit] : VSP2 DPR configuration information is NULL. Failed(%d)\r\n", e);
    } else {
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_BRU_ROUTE, Conf->Dpr_Bru_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF0_ROUTE, Conf->Dpr_Rpf0_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF1_ROUTE, Conf->Dpr_Rpf1_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF2_ROUTE, Conf->Dpr_Rpf2_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF3_ROUTE, Conf->Dpr_Rpf3_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF4_ROUTE, Conf->Dpr_Rpf4_Route);

        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_SRU_ROUTE, Conf->Dpr_Sru_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_UDS_ROUTE, Conf->Dpr_Uds_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_LUT_ROUTE, Conf->Dpr_Lut_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_CLU_ROUTE, Conf->Dpr_Clu_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_HST_ROUTE, Conf->Dpr_Hst_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_HSI_ROUTE, Conf->Dpr_Hsi_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_ILV_BRS_ROUTE, Conf->Dpr_Ilv_Brs_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_SHP_ROUTE, Conf->Dpr_Shp_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_UIF4_ROUTE, Conf->Dpr_Uif4_Route);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_UIF5_ROUTE, Conf->Dpr_Uif5_Route);

        /* save the first settings of DPR for disable DISCOM  */
        FirstDprSettings[0] = Conf->Dpr_Rpf0_Route;
        FirstDprSettings[1] = Conf->Dpr_Rpf1_Route;
        FirstDprSettings[2] = Conf->Dpr_Rpf2_Route;
        FirstDprSettings[3] = Conf->Dpr_Rpf3_Route;
        FirstDprSettings[4] = Conf->Dpr_Rpf4_Route;
        FirstDprSettings[5] = Conf->Dpr_Uif4_Route;
    }

    return e;
}

r_vsp2_Error_t R_VSP2_PRV_DprSetDiscomEnable(const r_vsp2_Unit_t Vsp2Unit, uint32_t Rpf_ID)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
    uint32_t UIF4_root = 0;

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
    } else {
		switch(Rpf_ID){
			case 0:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF0_ROUTE, R_VSP_DPR_ROUTE_UIF4);
				UIF4_root = R_VSP_DPR_ROUTE_BRU0;
				break;
			case 1:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF1_ROUTE, R_VSP_DPR_ROUTE_UIF4);
				UIF4_root = R_VSP_DPR_ROUTE_BRU1;
				break;
			case 2:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF2_ROUTE, R_VSP_DPR_ROUTE_UIF4);
				UIF4_root = R_VSP_DPR_ROUTE_BRU2;
				break;
			case 3:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF3_ROUTE, R_VSP_DPR_ROUTE_UIF4);
				UIF4_root = R_VSP_DPR_ROUTE_BRU3;
				break;
			case 4:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF4_ROUTE, R_VSP_DPR_ROUTE_UIF4);
				UIF4_root = R_VSP_DPR_ROUTE_BRU4;
				break;
			default:
				e = R_VSP2_ERR_INVALID_PARAMETER;
		}
		R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_UIF4_ROUTE, UIF4_root);
    }

    return e;
}

r_vsp2_Error_t R_VSP2_PRV_DprSetDiscomDisable(const r_vsp2_Unit_t Vsp2Unit, uint32_t Rpf_ID)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
    } else {
		switch(Rpf_ID){
			case 0:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF0_ROUTE, FirstDprSettings[0]);
				break;
			case 1:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF1_ROUTE, FirstDprSettings[1]);
				break;
			case 2:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF2_ROUTE, FirstDprSettings[2]);
				break;
			case 3:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF3_ROUTE, FirstDprSettings[3]);
				break;
			case 4:
				R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_RPF4_ROUTE, FirstDprSettings[4]);
				break;
			default:
				e = R_VSP2_ERR_INVALID_PARAMETER;
		}
		R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_UIF4_ROUTE, FirstDprSettings[5]);
    }

    return e;
}