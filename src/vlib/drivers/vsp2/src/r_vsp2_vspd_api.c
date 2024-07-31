/*************************************************************************************************************
* vsp2_vspd_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "r_wm_common.h"
#include "r_vsp2_api.h"
#include "r_vsp2_rpf.h"
#include "r_vsp2_wpf.h"
#include "r_vsp2_dpr.h"
#include "r_vsp2_vspd_api.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"
#include "r_prr_api.h"
/* VSPD configuration */
#define LOC_VSPD_NUM        3
#define LOC_VSPD_LAYERS_NUM 5

typedef struct {
    r_wm_ColorFmt_t key;
    uint32_t format;
    uint32_t bpp;
    uint32_t data_swap;
}r_vspd_format_lookup_t;

/* swapping is rearranging fetched 16 bytes to put pixels in order
   swapping can be used support some reversed formats (like bgra888)
   but we're not doing that */
static const r_vspd_format_lookup_t r_vspd_lookup[] = {
    { R_WM_FMT_ARGB8888,    0x13, 32, 0x0C },
    { R_WM_FMT_RGB332,      0x00,  8, 0x0F }, /* (not support) */
    { R_WM_FMT_ARGB4444,    0x19, 16, 0x0E }, /* (not support) */
    { R_WM_FMT_XRGB4444,    0x01, 16, 0x0E }, /* (not support) */
    { R_WM_FMT_ARGB1555,    0x1B, 16, 0x0E }, /* (not support) */
    { R_WM_FMT_XRGB1555,    0x04, 16, 0x0E }, /* (not support) */
    { R_WM_FMT_RGB565,      0x06, 16, 0x0E },
    { R_WM_FMT_BGR888,      0x18, 24, 0x0C }, /* (not support) */
    { R_WM_FMT_RGB888,      0x15, 24, 0x0C }, /* (not support) */
    { R_WM_FMT_YCBCR420,    0x42,  8, 0x0F },
    { R_WM_FMT_YCBCR422,    0x41,  8, 0x0F },
    { R_WM_FMT_YCBCR444,    0x40,  8, 0x0F },
    { R_WM_FMT_YCBCR420ITL, 0x49,  8, 0x0F },
    { R_WM_FMT_YCBCR422ITL, 0x47, 16, 0x0F },
    { R_WM_FMT_YCBCR444ITL, 0x46, 24, 0x0F },
    { R_WM_FMT_YCBCR420P,   0x4C,  8, 0x0F },
    { R_WM_FMT_YCBCR422P,   0x4B,  8, 0x0F },
    { R_WM_FMT_YCBCR444P,   0x4A,  8, 0x0F },
};

typedef struct {
    _Bool enabled;
    uint32_t size_w;
    uint32_t size_h;
    uint32_t pos_x;
    uint32_t pos_y;
    uint32_t RfpId;
    uint32_t Crc;
} r_vspd_discom_data_t;

typedef struct {
    _Bool enabled;
    r_vsp2_Rpf_t rpf;
    uint32_t wpf_id;
} r_vspd_data_t;

typedef struct {
    _Bool allocated;
    _Bool flush;
    _Bool stage; /* stage is for updates, !stage is written to hardware */
    r_vspd_data_t data[2];
} r_vspd_LayerState_t;

/* This array holds the current state of VSPDs */
static r_vspd_LayerState_t loc_VspdState[LOC_VSPD_NUM][LOC_VSPD_LAYERS_NUM];

/* This array holds the parameter of DISCOM */
static r_vspd_discom_data_t loc_VspdDiscomParam =
{
    false,
    0,
    0,
    0,
    0,
    0,
    0
};

static int loc_Vsp2UnitIndex(r_vsp2_Unit_t Vsp2Unit);
static void loc_R_VSP2_PRV_DiscomSet_before_VSP2_setting(r_vsp2_Unit_t Vsp2Unit);
static bool loc_Reg_R_VSP2_VSPD_DiscomEnable(r_vsp2_Unit_t Vsp2Unit, bool Enable);
static void loc_Reg_R_VSP2_VSPD_DiscomSetPos(r_vsp2_Unit_t Vsp2Unit, uint32_t PosX, uint32_t PosY);
static void loc_Reg_R_VSP2_VSPD_DiscomSetSize(r_vsp2_Unit_t Vsp2Unit, uint32_t Width, uint32_t Height);
static uint32_t loc_Reg_R_VSP2_VSPD_DiscomGetCrc(r_vsp2_Unit_t Vsp2Unit);
static void loc_Reg_R_VSP2_VSPD_DiscomEnableIrq(r_vsp2_Unit_t Vsp2Unit, bool Enable);
static void loc_Reg_R_VSP2_VSPD_DiscomClearStatus(r_vsp2_Unit_t Vsp2Unit);
static void loc_Reg_R_VSP2_VSPD_DiscomSetDefaultColor(r_vsp2_Unit_t Vsp2Unit);
static uint32_t loc_DiscomParamChangeCheck(r_vsp2_Unit_t Vsp2Unit);
static void loc_R_VSP2_VSPD_DiscomAllPramSet(r_vsp2_Unit_t Vsp2Unit);

static int loc_Vsp2UnitIndex(r_vsp2_Unit_t Vsp2Unit)
{
    int vspd;

    switch (Vsp2Unit) {
    case R_VSP2_VSPD0:
        vspd = 0;
        break;
    case R_VSP2_VSPD1:
        vspd = 1;
        break;
    case R_VSP2_VSPD2:
        vspd = 2;
        break;
    default:
        vspd = -1;
        R_PRINT_Log("[loc_Vsp2UnitIndex] : VSP2 Unit No is Invalid.\r\n");
        break;  /* wrong vspd */
    }

    return vspd;
}

static void loc_R_VSP2_PRV_DiscomSet_before_VSP2_setting(r_vsp2_Unit_t Vsp2Unit)
{
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
    static int first_flag = 0;

	/* write register */

    if( first_flag == 0 ){

		reg_val  = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMPMR);
		/* This parameter should be set to 9 (written in the HW manual) */
		reg_val |= (9 << 0);
		R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMPMR, reg_val);
		first_flag++;
    }
}

static bool loc_Reg_R_VSP2_VSPD_DiscomEnable(r_vsp2_Unit_t Vsp2Unit, bool Enable)
{
    bool Success = true;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

	reg_val  = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMCR);
	if( true == Enable ){
		/* Enable DISCOM */
		reg_val |= (1 << 0);
	} else {
		/* Disable DISCOM */
		reg_val &= ~(1 << 0);
	}
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMCR, reg_val);

    return Success;
}

static void loc_Reg_R_VSP2_VSPD_DiscomSetPos(r_vsp2_Unit_t Vsp2Unit, uint32_t PosX, uint32_t PosY)
{
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

	/* Specify horizontal offset of the CRC */
	reg_val = (PosX << 0);
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSPXR, reg_val);
	/* Specify vertical offset of the CRC */
	reg_val = (PosY << 0);
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSPYR, reg_val);
}

static void loc_Reg_R_VSP2_VSPD_DiscomSetSize(r_vsp2_Unit_t Vsp2Unit, uint32_t Width, uint32_t Height)
{
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

	/* Specify horizontal size of the CRC calculation area (is the same value of RPF's ) */
	reg_val = Width;
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSZXR, reg_val);
	/* Specify vertical size of the CRC calculation area (is the same value of RPF's ) */
	reg_val = Height;
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSZYR, reg_val);
}

static uint32_t loc_Reg_R_VSP2_VSPD_DiscomGetCrc(r_vsp2_Unit_t Vsp2Unit)
{
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
    uint32_t Crc = 0;

    Crc = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMCCRCR);

    return Crc;
}

static void loc_Reg_R_VSP2_VSPD_DiscomSetExpectedCrc(r_vsp2_Unit_t Vsp2Unit, uint32_t Crc)
{
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMECRCR, Crc);
}

static void loc_Reg_R_VSP2_VSPD_DiscomEnableIrq(r_vsp2_Unit_t Vsp2Unit, bool Enable)
{
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

    reg_val  = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMIENR);
	if( true == Enable ){
		/* Interruption enable */
		reg_val |= (1 << 0);
		R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMIENR, reg_val);
	} else {
		/* Interruption disable */
		reg_val &= ~(1 << 0);
		R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMIENR, reg_val);
	}
}

static void loc_Reg_R_VSP2_VSPD_DiscomClearStatus(r_vsp2_Unit_t Vsp2Unit)
{
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

    reg_val  = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMCLSTR);
	/* Interruption enable */
	reg_val |= (1 << 0);
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMCLSTR, reg_val);
}

static void loc_Reg_R_VSP2_VSPD_DiscomSetDefaultColor(r_vsp2_Unit_t Vsp2Unit)
{
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	/* write register */

    reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMPMR);
	/* Color format : ARGB8888, RGB888, or YCbCr444 */
	reg_val |= (0 << 17);
	/* Specify the default alpha value : 255 */
	reg_val |= (0xff << 8);
	/* Specify the using alpha value that default alpha value */
	reg_val |= (0 << 7);
	R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMPMR, reg_val);
}

static uint32_t loc_DiscomParamChangeCheck(r_vsp2_Unit_t Vsp2Unit)
{
	uint32_t ParamChangedFlag = 1;
	uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

	if( R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMECRCR) == loc_VspdDiscomParam.Crc ){
		if( R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSPXR) == loc_VspdDiscomParam.pos_x ){
			if( R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSPYR) == loc_VspdDiscomParam.pos_y ){
				if( R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSZXR) == loc_VspdDiscomParam.size_w ){
					if( R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMSZYR) == loc_VspdDiscomParam.size_h ){
						ParamChangedFlag = 0;
					}
				}
			}
		}
	}

	return ParamChangedFlag;

}

static void loc_R_VSP2_VSPD_DiscomAllPramSet(r_vsp2_Unit_t Vsp2Unit)
{
	uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
	r_prr_DevName_t dev = R_PRR_GetDevice();
	uint32_t Discom_PosX = 0;
	uint32_t Discom_PosY = 0;
	uint32_t Discom_Width = 0;
	uint32_t Discom_Height = 0;

	/* check the value R_VSP2_VI6_UIF4_DISCOM_DOCMCR.CMPRU */
	if( ( R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_UIF4_DISCOM_DOCMCR) & (1 << 16) ) == 0 ) {
		/* Specify default color and format value */
		loc_Reg_R_VSP2_VSPD_DiscomSetDefaultColor(Vsp2Unit);

		/* Specify the expectation of the CRC */
		loc_Reg_R_VSP2_VSPD_DiscomSetExpectedCrc(Vsp2Unit, loc_VspdDiscomParam.Crc);

		if (R_PRR_RCARM3W == dev) {
			Discom_PosX = loc_VspdDiscomParam.pos_x / 2;
			Discom_PosY = loc_VspdDiscomParam.pos_y / 2;
			Discom_Width = loc_VspdDiscomParam.size_w / 2;
			Discom_Height = loc_VspdDiscomParam.size_h / 2;
		} else {
			Discom_PosX = loc_VspdDiscomParam.pos_x;
			Discom_PosY = loc_VspdDiscomParam.pos_y;
			Discom_Width = loc_VspdDiscomParam.size_w;
			Discom_Height = loc_VspdDiscomParam.size_h;
		}

		/* Specify horizontal offset and vertical offset of the CRC */
		loc_Reg_R_VSP2_VSPD_DiscomSetPos(Vsp2Unit, Discom_PosX, Discom_PosY);

		/* Specify horizontal size and vertical size of the CRC calculation area */
		loc_Reg_R_VSP2_VSPD_DiscomSetSize(Vsp2Unit, Discom_Width, Discom_Height);

		/* Clear the error status of DISCOM */
		loc_Reg_R_VSP2_VSPD_DiscomClearStatus(Vsp2Unit);

		/* Enable the interrupt of DISCOM */
		loc_Reg_R_VSP2_VSPD_DiscomEnableIrq(Vsp2Unit, loc_VspdDiscomParam.enabled);

		/* Enable DISCOM */
		loc_Reg_R_VSP2_VSPD_DiscomEnable(Vsp2Unit, loc_VspdDiscomParam.enabled);
	/* check DISCOM's parameter is changed previous ones or not */
	} else if( loc_DiscomParamChangeCheck(Vsp2Unit) != 0 ){
		/* Disable DISCOM */
		loc_Reg_R_VSP2_VSPD_DiscomEnable(Vsp2Unit, false);

		/* Enable the interrupt of DISCOM */
		loc_Reg_R_VSP2_VSPD_DiscomEnableIrq(Vsp2Unit, false);
	}
}

void R_VSP2_VSPD_LayerAddrSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                              uint32_t WpfId, uint32_t LayerAddr, uint32_t C0_Addr, uint32_t C1_Addr)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        pLayer = &loc_VspdState[vspd][RpfId];
        pStage = &pLayer->data[pLayer->stage];
        pStage->rpf.mem_addr_y_rgb = LayerAddr;
        pStage->rpf.mem_addr_c0 = C0_Addr;
        pStage->rpf.mem_addr_c1 = C1_Addr;
        pStage->wpf_id = WpfId;
    }else{
        R_PRINT_Log("[R_VSP2_VSPD_LayerAddrSet] : VSP2 Unit No is Invalid.\r\n");
    }
}

void R_VSP2_VSPD_LayerFormatSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                r_wm_ColorFmt_t LayerFormat)
{
    int vspd;
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;
    int i;

    switch (Vsp2Unit) {
    case R_VSP2_VSPD0:
        vspd = 0;
        break;
    case R_VSP2_VSPD1:
        vspd = 1;
        break;
    case R_VSP2_VSPD2:
        vspd = 2;
        break;
    default:
        vspd = -1;
        R_PRINT_Log("[R_VSP2_VSPD_LayerFormatSet] : VSP2 Unit No is Invalid.\r\n");
        break;  /* wrong vspd */
    }

    if (0 <= vspd) {
        pLayer = &loc_VspdState[vspd][RpfId];
        pStage = &pLayer->data[pLayer->stage];

        for (i=0; i < sizeof(r_vspd_lookup)/sizeof(r_vspd_lookup[0]); i++) {
            if (LayerFormat == r_vspd_lookup[i].key) {
                pStage->rpf.format = r_vspd_lookup[i].format;
                pStage->rpf.bpp = r_vspd_lookup[i].bpp;
                pStage->rpf.data_swap = r_vspd_lookup[i].data_swap;
            }
        }
    }
}

bool R_VSP2_VSPD_LayerAllocate(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, bool Allocate)
{
    bool ret = false;
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;

    if (0 <= vspd) {
        pLayer = &loc_VspdState[vspd][RpfId];
        if (Allocate != pLayer->allocated) {
            /* allocation change */
            pLayer->allocated = Allocate;
            ret = true;
        } else {
            /* Layer already in use */
            R_PRINT_Log("[R_VSP2_VSPD_LayerAllocate] : Layer already in use. Failed(%d)\r\n",ret);
        }
    }
    return ret;
}

void R_VSP2_VSPD_LayerPosXSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, uint32_t PosX)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        if (PosX <= 8189) {
            pLayer = &loc_VspdState[vspd][RpfId];
            pStage = &pLayer->data[pLayer->stage];
            pStage->rpf.pos_x = PosX;
        } else {
            R_PRINT_Log("[R_VSP2_VSPD_LayerPosXSet] : PosX(%d) range Failed\r\n", PosX);
        }
    }
}

void R_VSP2_VSPD_LayerPosYSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, uint32_t PosY)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        if (PosY <= 8189) {
            pLayer = &loc_VspdState[vspd][RpfId];
            pStage = &pLayer->data[pLayer->stage];
            pStage->rpf.pos_y = PosY;
        } else {
            R_PRINT_Log("[R_VSP2_VSPD_LayerPosYSet] : PosY(%d) range Failed\r\n", PosY);
        }
    }
}

void R_VSP2_VSPD_LayerWidthSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, uint32_t Width, uint16_t StrideY, uint16_t StrideC)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        if ((0 < Width) && (Width <= 8190)) {
            pLayer = &loc_VspdState[vspd][RpfId];
            pStage = &pLayer->data[pLayer->stage];
            pStage->rpf.size_w = Width;
            pStage->rpf.stride_y = StrideY;
            pStage->rpf.stride_c = StrideC;
        } else {
            R_PRINT_Log("[R_VSP2_VSPD_LayerWidthSet] : Width(%d) range Failed\r\n", Width);
        }
    }
}

void R_VSP2_VSPD_LayerHeightSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                                uint32_t Height)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        if ((0 < Height) && (Height <= 8190)) {
            pLayer = &loc_VspdState[vspd][RpfId];
            pStage = &pLayer->data[pLayer->stage];
            pStage->rpf.size_h = Height;
        } else {
            R_PRINT_Log("[R_VSP2_VSPD_LayerHeightSet] : Height(%d) range Failed\r\n", Height);
        }
    }
}

void R_VSP2_VSPD_LayerEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                             bool Enable)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        pLayer = &loc_VspdState[vspd][RpfId];
        pStage = &pLayer->data[pLayer->stage];
        pStage->enabled = Enable;
    }
}

void R_VSP2_VSPD_PRV_LayersUpdate(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId)
{
    r_vsp2_Error_t ret = R_VSP2_ERR_SUCCESS;
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    uint32_t rpf;
    uint32_t work_stride_y;
    uint32_t work_stride_c;
    r_vspd_LayerState_t *pLayer;
    r_vspd_data_t *pStage;

    if (0 <= vspd) {
        for (rpf = 0; rpf < LOC_VSPD_LAYERS_NUM; rpf++) {
            pLayer = &loc_VspdState[vspd][rpf];
            pStage = &pLayer->data[pLayer->stage];
            /* Do not do anything if anyone didn't explicitly request to
             * flush the changes */
            if ((0 != pLayer->flush) && (pStage->wpf_id == WpfId)) {
                /* set stride after we know format and width */
                work_stride_y = (uint32_t)(pStage->rpf.stride_y);
                work_stride_c = (uint32_t)(pStage->rpf.stride_c);
                pStage->rpf.mem_stride = ((work_stride_y << 16) | work_stride_c);

                /* Set Alpha stride */
                pStage->rpf.mem_stride_alpha = 0;

                /* If DISCOM is enable/disable, change the settings of DPR */
                if( loc_VspdDiscomParam.enabled == true ){
                	R_VSP2_PRV_DprSetDiscomEnable(Vsp2Unit, loc_VspdDiscomParam.RfpId);

                	/* Initial setting for DISCOM */
                	loc_R_VSP2_PRV_DiscomSet_before_VSP2_setting(Vsp2Unit);
                } else {
                	R_VSP2_PRV_DprSetDiscomDisable(Vsp2Unit, loc_VspdDiscomParam.RfpId);
                }

                /* Update RPF registers */
                ret = R_VSP2_PRV_RpfSet(Vsp2Unit, rpf, &(pStage->rpf));
                if (R_VSP2_ERR_SUCCESS == ret) {
                    /* Enable/disable layer */
                    R_VSP2_PRV_WpfEnableRpf(Vsp2Unit, WpfId, rpf, pStage->enabled);

                    /* DISCOM settings */
                    loc_R_VSP2_VSPD_DiscomAllPramSet(Vsp2Unit);

                    /* Clear flush flag */
                    pLayer->flush = 0;
                } else {
                    R_PRINT_Log("[R_VSP2_VSPD_PRV_LayersUpdate] : R_VSP2_PRV_RpfSet Failed(%d)\r\n",ret);
                }
            }
        }
    }
}

/* just for internal (CR7) usage */
void R_VSP2_VSPD_Flush(r_vsp2_Unit_t Vsp2Unit, uint32_t Mask)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    uint32_t rpf;

    if (0 <= vspd) {
        /* mark the requested layers for flushing */
        for (rpf = 0; rpf < LOC_VSPD_LAYERS_NUM; rpf++) {
            if (0 != (Mask & (1 << rpf))) {
                loc_VspdState[vspd][rpf].flush = 1;
            }
        }
    }
}

/**
 * Section Global functions
 */

bool R_VSP2_VSPD_DiscomEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, bool Enable)
{
    int vspd = loc_Vsp2UnitIndex(Vsp2Unit);
    bool Success = true;

	if (0 <= vspd) {
		loc_VspdDiscomParam.RfpId = RpfId;
		loc_VspdDiscomParam.enabled = Enable;
	} else {
		Success = false;
	}

    return Success;
}

void R_VSP2_VSPD_DiscomSetPos(r_vsp2_Unit_t Vsp2Unit, uint32_t PosX, uint32_t PosY)
{
	int vspd = loc_Vsp2UnitIndex(Vsp2Unit);

	if (0 <= vspd) {
		loc_VspdDiscomParam.pos_x = PosX;
		loc_VspdDiscomParam.pos_y = PosY;
	}

}

void R_VSP2_VSPD_DiscomSetSize(r_vsp2_Unit_t Vsp2Unit, uint32_t Width, uint32_t Height)
{
	int vspd = loc_Vsp2UnitIndex(Vsp2Unit);

	if (0 <= vspd) {
		loc_VspdDiscomParam.size_w = Width;
		loc_VspdDiscomParam.size_h = Height;
	}
}

uint32_t R_VSP2_VSPD_DiscomGetCrc(r_vsp2_Unit_t Vsp2Unit)
{
    return loc_Reg_R_VSP2_VSPD_DiscomGetCrc(Vsp2Unit);
}

void R_VSP2_VSPD_DiscomSetExpectedCrc(r_vsp2_Unit_t Vsp2Unit, uint32_t Crc)
{
	int vspd = loc_Vsp2UnitIndex(Vsp2Unit);

	if (0 <= vspd) {
		loc_VspdDiscomParam.Crc = Crc;
	}
}

void R_VSP2_VSPD_DiscomEnableIrq(r_vsp2_Unit_t Vsp2Unit, bool Enable)
{
	loc_Reg_R_VSP2_VSPD_DiscomEnableIrq(Vsp2Unit, Enable);
}

void R_VSP2_VSPD_DiscomClearStatus(r_vsp2_Unit_t Vsp2Unit)
{

	loc_Reg_R_VSP2_VSPD_DiscomClearStatus(Vsp2Unit);
}
