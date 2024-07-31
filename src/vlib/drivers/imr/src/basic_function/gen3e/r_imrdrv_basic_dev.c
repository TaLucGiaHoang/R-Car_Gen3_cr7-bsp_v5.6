/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_dev.c
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver function of Basic Function Device Dependent Control
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 08.06.2021 0.01     New create
 *         : 14.06.2021 0.02     Modify file name.
 *                               Add macro definitions.
 *                               Add structure definitions.
 *                               Modify function header of r_imrdrv_devctl_init() and r_imrdrv_devctl_quit().
 *                               Add function call of r_imrdrv_devctl_clear_opt_attr() to r_imrdrv_devctl_init().
 *                               Implement r_imrdrv_devctl_quit() and r_imrdrv_devctl_io_destroy().
 *                               Add r_imrdrv_devctl_reset_reg(), r_imrdrv_devctl_io_create(),
 *                               r_imrdrv_devctl_enable_irq(), r_imrdrv_devctl_write_reg(),
 *                               r_imrdrv_devctl_read_reg(), r_imrdrv_devctl_io_destroy(),
 *                               r_imrdrv_devctl_disable_irq(), r_imrdrv_devctl_chk_stride(),
 *                               r_imrdrv_devctl_chk_color_form(), r_imrdrv_devctl_chk_tri_mode(),
 *                               r_imrdrv_devctl_chk_comb_form(), r_imrdrv_devctl_check_write_reg(),
 *                               r_imrdrv_devctl_check_read_reg(), r_imrdrv_devctl_get_reg_val(),
 *                               r_imrdrv_devctl_clear_opt_attr(), r_imrdrv_devctl_set_cache(),
 *                               r_imrdrv_devctl_chk_w_reg_cmn(), r_imrdrv_devctl_get_attr_reg(),
 *                               r_imrdrv_devctl_get_tri_reg(), r_imrdrv_devctl_get_src_dst_reg(),
 *                               r_imrdrv_devctl_get_cmrcr(), r_imrdrv_devctl_get_yuv_reg(),
 *                               r_imrdrv_devctl_get_src_reg(), r_imrdrv_devctl_get_dst_reg(),
 *                               r_imrdrv_devctl_get_dl_reg(), r_imrdrv_devctl_get_cache_reg(),
 *                               r_imrdrv_devctl_update_val_ofs().
 *         : 29.10.2021 0.03     Add include "r_imrdrv_basic_dev_ch.h" and remove definition of channels.
 *                               Modify definitions for Gen3e.
 *                               Remove unused definitions in Gen3e.
 *                               Add r_imrdrv_devctl_get_int_yuv_val() and r_imrdrv_devctl_chk_int_yuv_swp().
 *                               Remove r_imrdrv_devctl_clear_opt_attr().
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_imrdrv_private.h"
#include "r_imrdrv_basic_dev.h"
#include "r_imrdrv_basic_dev_ch.h"
#include "r_imrdrv_osal_io.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define IMRDRV_RESET_REGISTER_NUM   (10U)   /* The number of reset register. */

/* The value of setting registers */
#define IMRDRV_REG_VAL_CR_SWRST     (0x00008000U)  /* CR register's setting value. */
#define IMRDRV_REG_VAL_CMRCCR_PAT1  (0x000FDBAEU)  /* CMRCCR register's 1st setting value. */
#define IMRDRV_REG_VAL_CMRCCR_PAT2  (0x00000050U)  /* CMRCCR register's 2nd setting value. */
#define IMRDRV_REG_VAL_CMRCCR2      (0x00009065U)  /* CMRCCR2 register's setting value. */
#define IMRDRV_REG_VAL_DSTR_CLEAR   (0x00000040U)  /* DSTR register's clear setting value. */
#define IMRDRV_REG_VAL_SSTR_CLEAR   (0x00000100U)  /* SSTR(SSTRn) register's clear setting value. */
#define IMRDRV_REG_VAL_CLEAR        (0x00000000U)  /* Register's setting value to clear. */

#define IMRDRV_REG_VAL_ICR_RESERVE  (0x00000018U)  /* ICR register's setting value for reserve. */
#define IMRDRV_REG_VAL_ICR_INTENB   (0x00000004U)  /* ICR register's setting value to enable INT interrupt. */
#define IMRDRV_REG_VAL_ICR_IERENB   (0x00000002U)  /* ICR register's setting value to enable IER interrupt. */
#define IMRDRV_REG_VAL_ICR_TRAENB   (0x00000001U)  /* ICR register's setting value to enable TRAP interrupt. */
/* ICR register's setting value to enable all interrupt. */
#define IMRDRV_REG_VAL_ICR_ALL      (IMRDRV_REG_VAL_ICR_RESERVE | \
                                     IMRDRV_REG_VAL_ICR_INTENB | \
                                     IMRDRV_REG_VAL_ICR_IERENB | \
                                     IMRDRV_REG_VAL_ICR_TRAENB)

#define IMRDRV_REG_VAL_TRIMCR_TCMC      (0x00000040U)  /* TRIMCR register's value to clear TRIMR register's TCM bit. */
#define IMRDRV_REG_VAL_TRIMCR_AUTOSGC   (0x00000008U)  /* TRIMCR register's value to clear TRIMR register's AUTOSG bit. */
#define IMRDRV_REG_VAL_TRIMCR_AUTODGC   (0x00000004U)  /* TRIMCR register's value to clear TRIMR register's AUTODG bit. */
#define IMRDRV_REG_VAL_TRIMCR_BFEC      (0x00000002U)  /* TRIMCR register's value to clear TRIMR register's BFE bit. */
#define IMRDRV_REG_VAL_TRIMCR_TMEC      (0x00000001U)  /* TRIMCR register's value to clear TRIMR register's TME bit. */
/* TRIMCR register's value to clear TRIMR register's all bit. */
#define IMRDRV_REG_VAL_TRIMCR_ALL       (IMRDRV_REG_VAL_TRIMCR_TCMC | \
                                         IMRDRV_REG_VAL_TRIMCR_AUTOSGC | \
                                         IMRDRV_REG_VAL_TRIMCR_AUTODGC | \
                                         IMRDRV_REG_VAL_TRIMCR_BFEC | \
                                         IMRDRV_REG_VAL_TRIMCR_TMEC)

#define IMRDRV_REG_VAL_TRIMSR_TCMS      (0x00000040U)  /* TRIMSR register's value to set TRIMR register's TCM bit. */
#define IMRDRV_REG_VAL_TRIMSR_AUTOSGS   (0x00000008U)  /* TRIMSR register's value to set TRIMR register's AUTOSG bit. */
#define IMRDRV_REG_VAL_TRIMSR_AUTODGS   (0x00000004U)  /* TRIMSR register's value to set TRIMR register's AUTODG bit. */
#define IMRDRV_REG_VAL_TRIMSR_BFES      (0x00000002U)  /* TRIMSR register's value to set TRIMR register's BFE bit. */
#define IMRDRV_REG_VAL_TRIMSR_TMES      (0x00000001U)  /* TRIMSR register's value to set TRIMR register's TME bit. */

#define IMRDRV_REG_VAL_CMRCSR_CP16ES    (0x00008000U)  /* CMRCSR register's value to set CMRCR register's CP32E bit. */
#define IMRDRV_REG_VAL_CMRCSR_YCMS      (0x00004000U)  /* CMRCSR register's value to set CMRCR register's YCM bit. */
#define IMRDRV_REG_VAL_CMRCSR_SY12S     (0x00001000U)  /* CMRCSR register's value to set CMRCR register's SY12 bit. */
#define IMRDRV_REG_VAL_CMRCSR_SY10S     (0x00000800U)  /* CMRCSR register's value to set CMRCR register's SY10 bit. */
#define IMRDRV_REG_VAL_CMRCSR_Y12S      (0x00000200U)  /* CMRCSR register's value to set CMRCR register's Y12 bit. */
#define IMRDRV_REG_VAL_CMRCSR_Y10S      (0x00000100U)  /* CMRCSR register's value to set CMRCR register's Y10 bit. */
#define IMRDRV_REG_VAL_CMRCSR_YISMS     (0x00000080U)  /* CMRCSR register's value to set CMRCR register's YISM bit. */
#define IMRDRV_REG_VAL_CMRCSR_SUVS_10   (0x00000020U)  /* CMRCSR register's value to set CMRCR register's SUVS bit for 10 bpp. */
#define IMRDRV_REG_VAL_CMRCSR_SUVS_12   (0x00000040U)  /* CMRCSR register's value to set CMRCR register's SUVS bit for 12 bpp. */
#define IMRDRV_REG_VAL_CMRCSR_DUVS_10   (0x00000008U)  /* CMRCSR register's value to set CMRCR register's DUVS bit for 10 bpp. */
#define IMRDRV_REG_VAL_CMRCSR_DUVS_12   (0x00000010U)  /* CMRCSR register's value to set CMRCR register's DUVS bit for 12 bpp. */

#define IMRDRV_REG_VAL_CMRCSR2_UVFORMS  (0x00000040U)  /* CMRCSR2 register's value to set CMRCR2 register's UVFORM bit. */
#define IMRDRV_REG_VAL_CMRCSR2_422FORMS (0x00000020U)  /* CMRCSR2 register's value to set CMRCR2 register's YUV422FORM bit. */
#define IMRDRV_REG_VAL_CMRCSR2_YUV422ES (0x00000004U)  /* CMRCSR2 register's value to set CMRCR2 register's YUV422E bit. */

#define IMRDRV_REG_VAL_TRICR_YCFORM     (0x80000000U)  /* TRICR register's value to set YCFORM bit. */

#define IMRDRV_WRITE_REGISTER_NUM   (19U)   /* The number of write register. */

/* The number of registers related to triangle mode */
#define IMRDRV_WRITE_TRIM_REGISTER_NUM (2U)         /* The number of registers related to triangle mode. */

/* The index of register for triangle mode */
#define IMRDRV_TRIM_REG_IDX_TRIMCR     (0U)         /* The index of TRIMCR register. */
#define IMRDRV_TRIM_REG_IDX_TRIMSR     (1U)         /* The index of TRIMSR register. */

/* The number of registers related to source data */
#define IMRDRV_WRITE_SRC_REGISTER_NUM  (4U)         /* The number of registers related to source data. */

/* The index of register for source data */
#define IMRDRV_SRC_REG_IDX_SSTR        (0U)         /* The index of SSTR register. */
#define IMRDRV_SRC_REG_IDX_SSAR        (1U)         /* The index of SSAR register. */
#define IMRDRV_SRC_REG_IDX_SUSR        (2U)         /* The index of SUSR register. */
#define IMRDRV_SRC_REG_IDX_SVSR        (3U)         /* The index of SVSR register. */

/* The threshold of SUSR/SVSR value */
#define IMRDRV_REG_SUSR_SUW_THRESHOLD  (2U)         /* The threshold of SUSR register's SUW bit. */
#define IMRDRV_REG_SUSR_SVW_THRESHOLD  (1U)         /* The threshold of SUSR register's SVW bit. */
#define IMRDRV_REG_SVSR_SVS_THRESHOLD  (1U)         /* The threshold of SVSR register's SVS bit. */

/* The number of registers related to destination data */
#define IMRDRV_WRITE_DST_REGISTER_NUM  (3U)         /* The number of registers related to destination data. */

/* The index of register for destination data */
#define IMRDRV_DST_REG_IDX_DSTR        (0U)         /* The index of DSTR register. */
#define IMRDRV_DST_REG_IDX_DSAR        (1U)         /* The index of DSAR register. */
#define IMRDRV_DST_REG_IDX_DSOR        (2U)         /* The index of DSOR register. */

/* The number of registers related to DL data */
#define IMRDRV_WRITE_DL_REGISTER_NUM   (1U)         /* The number of registers related to DL data. */

/* The index of register for DL data */
#define IMRDRV_DL_REG_IDX_DLSAR        (0U)         /* The index of DLSAR register. */

/* The number of registers related to YUV color format */
#define IMRDRV_WRITE_YUV_REGISTER_NUM  (2U)         /* The number of registers related to YUV color format. */

/* The index of register for YUV color format */
#define IMRDRV_YUV_REG_IDX_CMRCSR2     (0U)         /* The index of CMRCSR2 register. */
#define IMRDRV_YUV_REG_IDX_TRICR       (1U)         /* The index of TRICR register. */

/* Shift value */
#define IMRDRV_SHIFT_16BIT             (16U)        /* 16bit shift. */


/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    e_imrdrv_channelno_t    channel_no;                                             /* IMR-LX4 channel number. */
    char_t                  device_type[IMRDRV_OSAL_DEV_TYPE_MAX_SIZE];             /* Device type. */
    char_t                  device_name[IMRDRV_OSAL_DEV_NAME_MAX_SIZE];             /* Device name. */
} st_imrdrv_device_type_name_t;

typedef struct
{
    e_imrdrv_color_format_t color_format;   /* Color format. */
    bool                    bpp_8_valid;    /* 8bpp is valid or not. (true : valid. false : invalid) */
    bool                    bpp_10_valid;   /* 10bpp is valid or not. (true : valid. false : invalid) */
    bool                    bpp_12_valid;   /* 12bpp is valid or not. (true : valid. false : invalid) */
    bool                    bpp_14_valid;   /* 14bpp is valid or not. (true : valid. false : invalid) */
    bool                    bpp_16_valid;   /* 16bpp is valid or not. (true : valid. false : invalid) */
    bool                    bpp_32_valid;   /* 32bpp is valid or not. (true : valid. false : invalid) */
} st_imrdrv_color_format_valid_t;

typedef struct
{
    e_imrdrv_color_format_t src_color_format; /* Color format of the source image data */
    e_imrdrv_color_format_t dst_color_format; /* Color format of the destination image data*/
} st_imrdrv_comb_color_valid_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_chk_w_reg_cmn (
    const uint32_t  offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_check_write_reg (
    const e_imrdrv_channelno_t  channel_no,
    const uint32_t              offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_check_read_reg (
    const e_imrdrv_channelno_t  channel_no,
    const uint32_t              offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_attr_reg (
    const st_imrdrv_attr_param_t    *const p_attr_param,
    st_imrdrv_attr_register_value_t      *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_tri_reg (
    const st_imrdrv_triangle_mode_t *const p_triangle_mode,
    st_imrdrv_attr_register_value_t       *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_src_dst_reg (
    const st_imrdrv_data_t          *const p_src_data,
    const st_imrdrv_data_t          *const p_dst_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_cmrcr (
    const st_imrdrv_data_t          *const p_src_data,
    const st_imrdrv_data_t          *const p_dst_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_yuv_reg (
    const e_imrdrv_color_format_t       src_color_format,
    const e_imrdrv_color_format_t       dst_color_format,
    st_imrdrv_attr_register_value_t     *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_src_reg (
    const st_imrdrv_data_t          *const p_src_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_dst_reg (
    const st_imrdrv_data_t          *const p_src_data,
    const st_imrdrv_data_t          *const p_dst_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_dl_reg (
    const st_imrdrv_dl_t            *const p_dl_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_update_val_ofs (
    const st_imrdrv_reg_offset_val_t      *const p_reg_settings,
    const uint32_t                        reg_setting_num,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_int_yuv_val (
    const e_imrdrv_color_format_t       src_color_format,
    const e_imrdrv_color_format_t       dst_color_format,
          uint32_t                      *const p_value
);
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_chk_int_yuv_swp (
    const e_imrdrv_color_format_t       color_format,
          bool                          *const p_y_uv_swap,
          bool                          *const p_u_v_swap
);


/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_init
* Description  : Clear and initialize the Basic Function Device Dependent Control area in the work area.
* Arguments    : channel_no             - Channel number
*              : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_03_001]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_init (
    const e_imrdrv_channelno_t      channel_no,
    st_imrdrv_basic_device_area_t   *const p_basic_device_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if (NULL != p_basic_device_area)
    {
        /* Check channel number */
        func_ret = r_imrdrv_devctl_check_ch(channel_no);

        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Set Basic Function Device Dependent Control area in the work area */
            p_basic_device_area->channel_no = channel_no;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_init()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_check_ch
* Description  : Check the channel number is within the range.
* Arguments    : channel_no             - Channel number
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_002]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_check_ch (
    const e_imrdrv_channelno_t  channel_no
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    switch (channel_no)
    {
        case IMRDRV_CHANNELNO_0:
            func_ret = IMRDRV_DEV_CHANNEL_0_ISVALID;
            break;
        case IMRDRV_CHANNELNO_1:
            func_ret = IMRDRV_DEV_CHANNEL_1_ISVALID;
            break;
        case IMRDRV_CHANNELNO_2:
            func_ret = IMRDRV_DEV_CHANNEL_2_ISVALID;
            break;
        case IMRDRV_CHANNELNO_3:
            func_ret = IMRDRV_DEV_CHANNEL_3_ISVALID;
            break;
        default:
            /* Channel number is invalid.
             * Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
            break;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_check_ch()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_quit
* Description  : Clear the Basic Function Device Dependent Control area in the work area.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_03_003]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_quit (
    st_imrdrv_basic_device_area_t   *const p_basic_device_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if (NULL != p_basic_device_area)
    {
        /* Clear Basic Function Device Dependent Control area in the work area */
        p_basic_device_area->channel_no = IMRDRV_CHANNELNO_INVALID;
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_quit()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_reset_reg
* Description  : Execute software reset and initialize register that is not initialized by software reset.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_03_004]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_reset_reg (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        uint32_t    table_idx = 0U;
        static const st_imrdrv_reg_offset_val_t s_reg_setting_table[IMRDRV_RESET_REGISTER_NUM] = 
        {
            {
                IMRDRV_REG_OFFSET_CR,       IMRDRV_REG_VAL_CR_SWRST
            },
            {
                IMRDRV_REG_OFFSET_CR,       IMRDRV_REG_VAL_CLEAR
            },
            {
                IMRDRV_REG_OFFSET_CMRCCR,   IMRDRV_REG_VAL_CMRCCR_PAT1
            },
            {
                IMRDRV_REG_OFFSET_CMRCCR,   IMRDRV_REG_VAL_CMRCCR_PAT2
            },
            {
                IMRDRV_REG_OFFSET_CMRCCR2,  IMRDRV_REG_VAL_CMRCCR2
            },
            {
                IMRDRV_REG_OFFSET_DSAR,     IMRDRV_REG_VAL_CLEAR
            },
            {
                IMRDRV_REG_OFFSET_SSAR,     IMRDRV_REG_VAL_CLEAR
            },
            {
                IMRDRV_REG_OFFSET_DSTR,     IMRDRV_REG_VAL_DSTR_CLEAR
            },
            {
                IMRDRV_REG_OFFSET_SSTR,     IMRDRV_REG_VAL_SSTR_CLEAR
            },
            {
                IMRDRV_REG_OFFSET_DSOR,     IMRDRV_REG_VAL_CLEAR
            },
        };

        /* Check the index is not out of the range */
        while ((IMRDRV_ERROR_OK == func_ret) && (table_idx < IMRDRV_RESET_REGISTER_NUM))
        {
            /* Write specified value to specified register */
            func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area, s_reg_setting_table[table_idx].offset,
                                                 s_reg_setting_table[table_idx].value, p_osal_io_area);

            table_idx++;
        }

        /* Check the result of writing to register */
        if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }   /* end of if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area)) */
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_reset_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_io_create
* Description  : Create IO handle for specified channel.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_03_005]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_io_create (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    st_imrdrv_osal_io_area_t            *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_PAR;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        uint32_t    table_idx = 0U;
        bool        loop_flag = true;
        static const st_imrdrv_device_type_name_t s_device_type_name_table[IMRDRV_IMR_CHANNEL_NUM] = IMRDRV_DEVICE_TYPE_NAME;

        /* Check the loop flag is true and the index is not out of range */
        while ((true == loop_flag) && (table_idx < IMRDRV_IMR_CHANNEL_NUM))
        {
            /* Compare the channel number */
            if (p_basic_device_area->channel_no == s_device_type_name_table[table_idx].channel_no)
            {
                /* Create the IO handle */
                func_ret = r_imrdrv_osal_io_create(s_device_type_name_table[table_idx].device_type,
                                                   s_device_type_name_table[table_idx].device_name,
                                                   p_osal_io_area);

                /* Check the result of creating IO resource */
                if (IMRDRV_ERROR_OK != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }

                loop_flag = false;
            }

            table_idx++;
        }
    }   /* end of if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area)) */

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_io_create()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_enable_irq
* Description  : Enable IMR interrupt.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_03_006]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_enable_irq (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        /* Write specified value to specified register */
        func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area, IMRDRV_REG_OFFSET_ICR, IMRDRV_REG_VAL_ICR_ALL,
                                                p_osal_io_area);

        /* Check the result of writing to register */
        if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_enable_irq()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_write_reg
* Description  : Write specified value to specified register.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : offset                 - Register offset
*              : value                  - Register value to write
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_03_007]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_write_reg (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const uint32_t                      offset,
    const uint32_t                      value,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        /* Check valid offset according to channel number */
        func_ret = r_imrdrv_devctl_check_write_reg(p_basic_device_area->channel_no, offset);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Write value to register */
            func_ret = r_imrdrv_osal_io_write_reg(offset, value, p_osal_io_area);

            /* Check the result of writing to register */
            if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
            {
                /* Set unexpected error to return value */
                func_ret = IMRDRV_ERROR_FAIL;
            }
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_write_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_read_reg
* Description  : Read value to specified register.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : offset                 - Register offset
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
*              : p_value                - Read register value
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_03_018]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_read_reg (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const uint32_t                      offset,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area,
    uint32_t                            *const p_value
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area) && (NULL != p_value))
    {
        /* Check valid offset according to channel number */
        func_ret = r_imrdrv_devctl_check_read_reg(p_basic_device_area->channel_no, offset);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            uint32_t reg_val;

            /* Read register's value */
            func_ret = r_imrdrv_osal_io_read_reg(offset, p_osal_io_area, &reg_val);

            /* Check the result of read register */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Set read register value to [a]p_value */
                *p_value = reg_val;
            }
            else
            {
                if (IMRDRV_ERROR_IO != func_ret)
                {
                    /* Set unexpected error to return value */
                    func_ret = IMRDRV_ERROR_FAIL;
                }
            }
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_read_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_io_destroy
* Description  : Destroy IO handle for specified channel.
* Arguments    : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
* [Covers: UD_RD_UD01_01_03_040]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_io_destroy (
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if (NULL != p_osal_io_area)
    {
        /* Destroy the IO handle */
        func_ret = r_imrdrv_osal_io_destroy(p_osal_io_area);

        /* Check the result of destroying IO handle */
        if (IMRDRV_ERROR_OK != func_ret)
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_io_destroy()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_disable_irq
* Description  : Disable IMR interrupt.
* Arguments    : p_basic_device_area    - Address of Basic Function Device Dependent Control area in the work area
*              : p_osal_io_area         - Address of OSAL Wrapper IO area in the work area
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
*                IMRDRV_ERROR_FAIL
*                IMRDRV_ERROR_IO
* [Covers: UD_RD_UD01_01_03_041]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_disable_irq (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret;

    /* Check parameter */
    if ((NULL != p_basic_device_area) && (NULL != p_osal_io_area))
    {
        /* Write specified value to specified register */
        func_ret = r_imrdrv_devctl_write_reg(p_basic_device_area, IMRDRV_REG_OFFSET_ICR, IMRDRV_REG_VAL_ICR_RESERVE,
                                                p_osal_io_area);

        /* Check the result of writing to register */
        if ((IMRDRV_ERROR_OK != func_ret) && (IMRDRV_ERROR_IO != func_ret))
        {
            /* Set unexpected error to return value */
            func_ret = IMRDRV_ERROR_FAIL;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_disable_irq()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_chk_stride
* Description  : Check the stride is within the upper limit.
* Arguments    : stride         - Stride
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_010]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_chk_stride (
    const uint32_t              stride
)
{
	/* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if (IMRDRV_UPPER_LIMIT_STRIDE < stride )
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}

/***********************************************************************************************************************
* End of function r_imrdrv_devctl_chk_stride()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_chk_color_form
* Description  : Check the color format and bpp are within the range and check the combination of color format and bpp
*                is valid.
* Arguments    : color_format   - Color format
*              : bpp            - Bpp
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_011]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_chk_color_form (
    const e_imrdrv_color_format_t       color_format,
    const e_imrdrv_bpp_t                bpp
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;
    static const st_imrdrv_color_format_valid_t s_color_format_valid[] = 
    /*      color_format               bpp_8   bpp_10  bpp_12  bpp_14  bpp_16 bpp_32 */
    {
        {
            IMRDRV_COLOR_FORM_Y,       true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_UV,      true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_SEP_Y,   true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_SEP_UV,  true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_DATA,    false,  false,  false,  false,  true,  false
        },
        {
            IMRDRV_COLOR_FORM_UYVY,    true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_YVYU,    true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_VYUY,    true,   true,   true,   false,  false, false
        },
        {
            IMRDRV_COLOR_FORM_YUYV,    true,   true,   true,   false,  false, false
        }
    };
    uint32_t cnt;
    uint32_t cnt_max;
    bool is_valid;

    /* Search color_format in s_color_format_valid[]. */
    cnt_max = sizeof(s_color_format_valid) / sizeof(s_color_format_valid[0]);

    for (cnt = 0; cnt < cnt_max; cnt++)
    {
        if (color_format == s_color_format_valid[cnt].color_format)
        {
            break;
        }
    }

    /* Check color_format and bpp are valid. */
    if (cnt < cnt_max)
    {
        switch (bpp)
        {
            case IMRDRV_BPP_8:
                is_valid = s_color_format_valid[cnt].bpp_8_valid;
                break;
            case IMRDRV_BPP_10:
                is_valid = s_color_format_valid[cnt].bpp_10_valid;
                break;
            case IMRDRV_BPP_12:
                is_valid = s_color_format_valid[cnt].bpp_12_valid;
                break;
            case IMRDRV_BPP_14:
                is_valid = s_color_format_valid[cnt].bpp_14_valid;
                break;
            case IMRDRV_BPP_16:
                is_valid = s_color_format_valid[cnt].bpp_16_valid;
                break;
            case IMRDRV_BPP_32:
                is_valid = s_color_format_valid[cnt].bpp_32_valid;
                break;
            default:
                is_valid = false;
                break;
        }

        if (false == is_valid)
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_chk_color_form()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_chk_tri_mode
* Description  : Check the triangle mode for depending on device.
* Arguments    : uvshfval       - On/off setting of UVSHFVAL
*              : shfval         - On/off setting of SHFVAL
*              : uvshfe         - On/off setting of UVSHFE
*              : shfe           - On/off setting of SHFE
*              : rde            - On/off setting of RDE
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_012]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_chk_tri_mode (
    const e_imrdrv_mode_t       uvshfval,
    const e_imrdrv_mode_t       shfval,
    const e_imrdrv_mode_t       uvshfe,
    const e_imrdrv_mode_t       shfe,
    const e_imrdrv_mode_t       rde
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;
    uint32_t cnt;
    uint32_t cnt_max;
    static const e_imrdrv_mode_t s_uvshfval_valid[] =
    {
        IMRDRV_MODE_OFF
    };
    static const e_imrdrv_mode_t s_shfval_valid[] =
    {
        IMRDRV_MODE_OFF
    };
    static const e_imrdrv_mode_t s_uvshfe_valid[] =
    {
        IMRDRV_MODE_OFF
    };
    static const e_imrdrv_mode_t s_shfe_valid[] =
    {
        IMRDRV_MODE_OFF
    };
    static const e_imrdrv_mode_t s_rde_valid[] =
    {
        IMRDRV_MODE_OFF
    };

    /* Check uvshfval is valid. */
    cnt_max = sizeof(s_uvshfval_valid) / sizeof(s_uvshfval_valid[0]);

    for (cnt = 0; cnt < cnt_max; cnt++)
    {
        if (uvshfval == s_uvshfval_valid[cnt])
        {
            break;
        }
    }

    if (cnt != cnt_max)
    {
        /* Check shfval is valid. */
        cnt_max = sizeof(s_shfval_valid) / sizeof(s_shfval_valid[0]);

        for (cnt = 0; cnt < cnt_max; cnt++)
        {
            if (shfval == s_shfval_valid[cnt])
            {
                break;
            }
        }

        if (cnt != cnt_max)
        {
            /* Check uvshfe is valid. */
            cnt_max = sizeof(s_uvshfe_valid) / sizeof(s_uvshfe_valid[0]);

            for (cnt = 0; cnt < cnt_max; cnt++)
            {
                if (uvshfe == s_uvshfe_valid[cnt])
                {
                    break;
                }
            }

            if (cnt != cnt_max)
            {
                /* Check shfe is valid. */
                cnt_max = sizeof(s_shfe_valid) / sizeof(s_shfe_valid[0]);

                for (cnt = 0; cnt < cnt_max; cnt++)
                {
                    if (shfe == s_shfe_valid[cnt])
                    {
                        break;
                    }
                }

                if (cnt != cnt_max)
                {
                    /* Check rde is valid. */
                    cnt_max = sizeof(s_rde_valid) / sizeof(s_rde_valid[0]);

                    for (cnt = 0; cnt < cnt_max; cnt++)
                    {
                        if (rde == s_rde_valid[cnt])
                        {
                            break;
                        }
                    }

                    if (cnt != cnt_max)
                    {
                        /* Nothing */
                    }
                    else
                    {
                        /* Set parameter error to return value */
                        func_ret = IMRDRV_ERROR_PAR;
                    }
                }
                else
                {
                    /* Set parameter error to return value */
                    func_ret = IMRDRV_ERROR_PAR;
                }
            }
            else
            {
                /* Set parameter error to return value */
                func_ret = IMRDRV_ERROR_PAR;
            }
        }
        else
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
        }

    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_chk_tri_mode()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_chk_comb_form
* Description  : Check the combination of the color format between the source image data and the destination image data.
* Arguments    : src_color_format - Color format of the source image data
*              : dst_color_format - Color format of the destination image data
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_013]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_chk_comb_form (
    const e_imrdrv_color_format_t       src_color_format,
    const e_imrdrv_color_format_t       dst_color_format
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;
    static const st_imrdrv_comb_color_valid_t s_comb_color_form_valid[] = 
    /*      Source                     Destination               */
    {
        {
            IMRDRV_COLOR_FORM_Y,       IMRDRV_COLOR_FORM_Y
        },
        {
            IMRDRV_COLOR_FORM_UV,      IMRDRV_COLOR_FORM_UV
        },
        {
            IMRDRV_COLOR_FORM_SEP_Y,   IMRDRV_COLOR_FORM_SEP_Y
        },
        {
            IMRDRV_COLOR_FORM_SEP_UV,  IMRDRV_COLOR_FORM_SEP_UV
        },
        {
            IMRDRV_COLOR_FORM_DATA,    IMRDRV_COLOR_FORM_DATA
        },
        {
            IMRDRV_COLOR_FORM_UYVY,    IMRDRV_COLOR_FORM_UYVY
        },
        {
            IMRDRV_COLOR_FORM_UYVY,    IMRDRV_COLOR_FORM_YVYU
        },
        {
            IMRDRV_COLOR_FORM_UYVY,    IMRDRV_COLOR_FORM_VYUY
        },
        {
            IMRDRV_COLOR_FORM_UYVY,    IMRDRV_COLOR_FORM_YUYV
        },
        {
            IMRDRV_COLOR_FORM_UYVY,    IMRDRV_COLOR_FORM_SEP_Y
        },
        {
            IMRDRV_COLOR_FORM_YVYU,    IMRDRV_COLOR_FORM_UYVY
        },
        {
            IMRDRV_COLOR_FORM_YVYU,    IMRDRV_COLOR_FORM_YVYU
        },
        {
            IMRDRV_COLOR_FORM_YVYU,    IMRDRV_COLOR_FORM_VYUY
        },
        {
            IMRDRV_COLOR_FORM_YVYU,    IMRDRV_COLOR_FORM_YUYV
        },
        {
            IMRDRV_COLOR_FORM_YVYU,    IMRDRV_COLOR_FORM_SEP_Y
        },
        {
            IMRDRV_COLOR_FORM_VYUY,    IMRDRV_COLOR_FORM_UYVY
        },
        {
            IMRDRV_COLOR_FORM_VYUY,    IMRDRV_COLOR_FORM_YVYU
        },
        {
            IMRDRV_COLOR_FORM_VYUY,    IMRDRV_COLOR_FORM_VYUY
        },
        {
            IMRDRV_COLOR_FORM_VYUY,    IMRDRV_COLOR_FORM_YUYV
        },
        {
            IMRDRV_COLOR_FORM_VYUY,    IMRDRV_COLOR_FORM_SEP_Y
        },
        {
            IMRDRV_COLOR_FORM_YUYV,    IMRDRV_COLOR_FORM_UYVY
        },
        {
            IMRDRV_COLOR_FORM_YUYV,    IMRDRV_COLOR_FORM_YVYU
        },
        {
            IMRDRV_COLOR_FORM_YUYV,    IMRDRV_COLOR_FORM_VYUY
        },
        {
            IMRDRV_COLOR_FORM_YUYV,    IMRDRV_COLOR_FORM_YUYV
        },
        {
            IMRDRV_COLOR_FORM_YUYV,    IMRDRV_COLOR_FORM_SEP_Y
        }
    };

    uint32_t cnt;
    uint32_t cnt_max;

    /* Check src_color_format and dst_color_format is valid. */
    cnt_max = sizeof(s_comb_color_form_valid) / sizeof(s_comb_color_form_valid[0]);

    for (cnt = 0; cnt < cnt_max; cnt++)
    {
        if ((src_color_format == s_comb_color_form_valid[cnt].src_color_format) &&
            (dst_color_format == s_comb_color_form_valid[cnt].dst_color_format))
        {
            break;
        }
    }

    if (cnt == cnt_max)
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_chk_comb_form()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_check_write_reg
* Description  : Check the offset range of the register to write.
* Arguments    : channel_no     - Channel number
*              : offset         - Register offset
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_008]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_check_write_reg (
    const e_imrdrv_channelno_t  channel_no,
    const uint32_t              offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_OK;
    e_imrdrv_errorcode_t    chk_w_reg_cmn_ret;

    /* Check channel number */
    func_ret = r_imrdrv_devctl_check_ch(channel_no);

    if (IMRDRV_ERROR_OK == func_ret)
    {
        chk_w_reg_cmn_ret = r_imrdrv_devctl_chk_w_reg_cmn(offset);

        /* Check the return values are ERROR_PAR */
        if (IMRDRV_ERROR_PAR == chk_w_reg_cmn_ret)
        {
            /* Set parameter error */
            func_ret = IMRDRV_ERROR_PAR;
        }
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_check_write_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_check_read_reg
* Description  : Check the offset range of the register to read.
* Arguments    : channel_no     - Channel number
*              : offset         - Register offset
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_042]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_check_read_reg (
    const e_imrdrv_channelno_t  channel_no,
    const uint32_t              offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_OK;

    (void)offset;

    /* Check channel number */
    func_ret = r_imrdrv_devctl_check_ch(channel_no);

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_check_read_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_reg_val
* Description  : Decide the setting value of registers.
* Arguments    : p_attr_param               - Attribute parameter
*              : p_basic_device_area        - Address of Basic Function Device Dependent Control area
*              : p_reg_value_offset         - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_017]
**********************************************************************************************************************/
e_imrdrv_errorcode_t r_imrdrv_devctl_get_reg_val (
    const st_imrdrv_attr_param_t        *const p_attr_param,
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    st_imrdrv_attr_register_value_t     *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if ((NULL != p_attr_param) && (NULL != p_basic_device_area) && (NULL != p_reg_value_offset))
    {
        p_reg_value_offset->set_num = 0U;

        /* Decide setting value of registers for attrribute parameter. */
        func_ret = r_imrdrv_devctl_get_attr_reg(p_attr_param, p_reg_value_offset);
    }
    else
    {
        /* Set parameter error */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_reg_val()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_chk_w_reg_cmn
* Description  : Check the offset range of the common register to write.
* Arguments    : offset     - Register offset
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_009]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_chk_w_reg_cmn (
    const uint32_t  offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_PAR;
    uint32_t                table_idx = 0U;
    static const uint32_t   s_reg_offset_table[IMRDRV_WRITE_REGISTER_NUM] = 
    {
        IMRDRV_REG_OFFSET_CR,
        IMRDRV_REG_OFFSET_SRCR,
        IMRDRV_REG_OFFSET_ICR,
        IMRDRV_REG_OFFSET_IMR,
        IMRDRV_REG_OFFSET_DLSAR,
        IMRDRV_REG_OFFSET_DSAR,
        IMRDRV_REG_OFFSET_SSAR,
        IMRDRV_REG_OFFSET_DSTR,
        IMRDRV_REG_OFFSET_SSTR,
        IMRDRV_REG_OFFSET_DSOR,
        IMRDRV_REG_OFFSET_CMRCSR,
        IMRDRV_REG_OFFSET_CMRCCR,
        IMRDRV_REG_OFFSET_TRIMSR,
        IMRDRV_REG_OFFSET_TRIMCR,
        IMRDRV_REG_OFFSET_TRICR,
        IMRDRV_REG_OFFSET_SUSR,
        IMRDRV_REG_OFFSET_SVSR,
        IMRDRV_REG_OFFSET_CMRCSR2,
        IMRDRV_REG_OFFSET_CMRCCR2
    };

    /* Check the return value is not OK and the index is not out of range */
    while ((IMRDRV_ERROR_OK != func_ret) && (table_idx < IMRDRV_WRITE_REGISTER_NUM))
    {
        /* Compare the register's offset */
        if (offset == s_reg_offset_table[table_idx])
        {
            /* Set successful completion to return value */
            func_ret = IMRDRV_ERROR_OK;
        }

        table_idx++;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_chk_w_reg_cmn()
***********************************************************************************************************************/
/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_attr_reg
* Description  : Decide the setting value of registers for attrribute parameter.
* Arguments    : p_attr_param          - Attribute parameter
*              : p_reg_value_offset    - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_024]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_attr_reg (
    const st_imrdrv_attr_param_t    *const p_attr_param,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if ((NULL != p_attr_param) && (NULL != p_reg_value_offset))
    {
        /* Get register setting of triangle mode */
        func_ret = r_imrdrv_devctl_get_tri_reg(&p_attr_param->triangle_mode, p_reg_value_offset);
        
        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Get register setting related to source/destination image data */
            func_ret = r_imrdrv_devctl_get_src_dst_reg(&p_attr_param->src_data,
                                                       &p_attr_param->dst_data,
                                                       p_reg_value_offset);
            
            /* Check the return value is OK */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Get register setting related to DL data */
                func_ret = r_imrdrv_devctl_get_dl_reg(&p_attr_param->dl_data, p_reg_value_offset);
            }
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_attr_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_tri_reg
* Description  : Decide the setting value of the following registers.
*                 TRIMCR
*                 TRIMR
* Arguments    : p_triangle_mode       - Triangle mode
*              : p_reg_value_offset    - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_025]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_tri_reg (
    const st_imrdrv_triangle_mode_t *const p_triangle_mode,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t        func_ret;
    st_imrdrv_reg_offset_val_t  tmp_reg_setting[IMRDRV_WRITE_TRIM_REGISTER_NUM];

    /* Check parameter */
    if ((NULL != p_triangle_mode) && (NULL != p_reg_value_offset))
    {
        /* Set value of TRIMCR to tmp_reg_setting */
        tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMCR].offset = IMRDRV_REG_OFFSET_TRIMCR;
        tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMCR].value  = 0U;
        tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMCR].value  |= IMRDRV_REG_VAL_TRIMCR_ALL;

        /* Set value of TRIMSR to tmp_reg_setting */
        tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].offset = IMRDRV_REG_OFFSET_TRIMSR;
        tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].value  = 0U;

        /* Check setting of TCM is ON */
        if (IMRDRV_MODE_ON == p_triangle_mode->tcm)
        {
            tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].value |= IMRDRV_REG_VAL_TRIMSR_TCMS;
        }

        /* Check setting of AUTOSG is ON */
        if (IMRDRV_MODE_ON == p_triangle_mode->autosg)
        {
            tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].value |= IMRDRV_REG_VAL_TRIMSR_AUTOSGS;
        }

        /* Check setting of AUTODG is ON */
        if (IMRDRV_MODE_ON ==p_triangle_mode->autodg)
        {
            tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].value |= IMRDRV_REG_VAL_TRIMSR_AUTODGS;
        }

        /* Check setting of BFE is ON */
        if (IMRDRV_MODE_ON == p_triangle_mode->bfe)
        {
            tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].value |= IMRDRV_REG_VAL_TRIMSR_BFES;
        }

        /* Check setting of TME is ON */
        if (IMRDRV_MODE_ON == p_triangle_mode->tme)
        {
            tmp_reg_setting[IMRDRV_TRIM_REG_IDX_TRIMSR].value |= IMRDRV_REG_VAL_TRIMSR_TMES;
        }

        /* Set value and offset of register */
        func_ret = r_imrdrv_devctl_update_val_ofs(tmp_reg_setting, IMRDRV_WRITE_TRIM_REGISTER_NUM, p_reg_value_offset);
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_tri_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_src_dst_reg
* Description  : Decide the setting value from the source / destination image data.
* Arguments    : p_src_data            - Source image data
*              : p_dst_data            - Destination image data
*              : p_reg_value_offset    - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_026]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_src_dst_reg (
    const st_imrdrv_data_t          *const p_src_data,
    const st_imrdrv_data_t          *const p_dst_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if ((NULL != p_src_data) && (NULL != p_dst_data) && (NULL != p_reg_value_offset))
    {
        /* Decide the setting value of the registers CMRCR */
        func_ret = r_imrdrv_devctl_get_cmrcr(p_src_data, p_dst_data, p_reg_value_offset);
        
        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
           /* Decide the setting value of the registers CMRCR2 and TRICR */
            func_ret = r_imrdrv_devctl_get_yuv_reg(p_src_data->color_format,
                                                   p_dst_data->color_format,
                                                   p_reg_value_offset);
            
            /* Check the return value is OK */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                /* Decide the setting value of the registers SSTR,SSAR,SUSR and SVSR */
                func_ret = r_imrdrv_devctl_get_src_reg(p_src_data, p_reg_value_offset);
                
                /* Check the return value is OK */
                if (IMRDRV_ERROR_OK == func_ret)
                {
                    /* Decide the setting value of the registers DSTR and DSAR */
                    func_ret = r_imrdrv_devctl_get_dst_reg(p_src_data, p_dst_data, p_reg_value_offset);
                }
            }
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_src_dst_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_cmrcr
* Description  : Decide the setting value of the following registers.
*                 CMRCR
* Arguments    : p_src_data            - Source image data
*              : p_dst_data            - Destination image data
*              : p_reg_value_offset    - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_030]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_cmrcr (
    const st_imrdrv_data_t          *const p_src_data,
    const st_imrdrv_data_t          *const p_dst_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Declare internal variables */
    e_imrdrv_errorcode_t        func_ret;
    st_imrdrv_reg_offset_val_t  reg_settings;

    /* Check parameter */
    if ((NULL != p_src_data) && (NULL != p_dst_data) && (NULL != p_reg_value_offset))
    {
        reg_settings.value = 0;
        
        /* Check the color format is data. */
        if (IMRDRV_COLOR_FORM_DATA == p_src_data->color_format)
        {
            if (IMRDRV_BPP_16 == p_src_data->bpp)
            {
                reg_settings.value = IMRDRV_REG_VAL_CMRCSR_CP16ES;
            }
        }
        else
        {
            /* check the bpp of source image data */
            switch (p_src_data->bpp)
            {
                case IMRDRV_BPP_12:
                    reg_settings.value |= (IMRDRV_REG_VAL_CMRCSR_SY12S | IMRDRV_REG_VAL_CMRCSR_SUVS_12);
                    break;
                case IMRDRV_BPP_10:
                    reg_settings.value |= (IMRDRV_REG_VAL_CMRCSR_SY10S | IMRDRV_REG_VAL_CMRCSR_SUVS_10);
                    break;
                default:
                    /* Do nothing because there is no corresponding value */
                    break;
            }
            /* check the bpp of destination image data */
            switch (p_dst_data->bpp)
            {
                case IMRDRV_BPP_12:
                    reg_settings.value |= (IMRDRV_REG_VAL_CMRCSR_Y12S | IMRDRV_REG_VAL_CMRCSR_DUVS_12);
                    break;
                case IMRDRV_BPP_10:
                    reg_settings.value |= (IMRDRV_REG_VAL_CMRCSR_Y10S | IMRDRV_REG_VAL_CMRCSR_DUVS_10);
                    break;
                default:
                    /* Do nothing because there is no corresponding value */
                    break;
            }
            /* check the color format of source image data */
            switch (p_src_data->color_format)
            {
                case IMRDRV_COLOR_FORM_UV:
                    /* Fall through */
                case IMRDRV_COLOR_FORM_SEP_UV:
                    reg_settings.value |= IMRDRV_REG_VAL_CMRCSR_YCMS;
                    break;
                case IMRDRV_COLOR_FORM_UYVY:
                    /* Fall through */
                case IMRDRV_COLOR_FORM_YVYU:
                    /* Fall through */
                case IMRDRV_COLOR_FORM_VYUY:
                    /* Fall through */
                case IMRDRV_COLOR_FORM_YUYV:
                    if ((IMRDRV_COLOR_FORM_Y     == p_dst_data->color_format) ||
                        (IMRDRV_COLOR_FORM_SEP_Y == p_dst_data->color_format))
                    {
                        reg_settings.value |= IMRDRV_REG_VAL_CMRCSR_YISMS;
                    }
                    break;
                default:
                    /* Do nothing because there is no corresponding value */
                    break;
            }
        }

        reg_settings.offset = IMRDRV_REG_OFFSET_CMRCSR;

        /* Update setting value and offset of registers by specified arguments */
        func_ret = r_imrdrv_devctl_update_val_ofs(&reg_settings, 1U, p_reg_value_offset);
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_cmrcr()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_yuv_reg
* Description  : Decide the setting value of registers related to YUV color format.
* Arguments    : src_color_format       - Source image color format
*              : dst_color_format       - Destination image color format
*              : p_reg_value_offset     - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_031]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_yuv_reg (
    const e_imrdrv_color_format_t       src_color_format,
    const e_imrdrv_color_format_t       dst_color_format,
    st_imrdrv_attr_register_value_t     *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t            func_ret = IMRDRV_ERROR_OK;
    st_imrdrv_reg_offset_val_t      reg_settings[IMRDRV_WRITE_YUV_REGISTER_NUM] = {
                                        {IMRDRV_REG_OFFSET_INVALID, 0U},
                                        {IMRDRV_REG_OFFSET_INVALID, 0U} };
    uint32_t                        cmrcr2_value;
    
    /* Check parameter */
    if (NULL != p_reg_value_offset)
    {
        switch (src_color_format)
        {
            case IMRDRV_COLOR_FORM_UYVY:
                /* Fall through */
            case IMRDRV_COLOR_FORM_YVYU:
                /* Fall through */
            case IMRDRV_COLOR_FORM_VYUY:
                /* Fall through */
            case IMRDRV_COLOR_FORM_YUYV:
                /* Fall through */
                func_ret = r_imrdrv_devctl_get_int_yuv_val(src_color_format, dst_color_format, &cmrcr2_value);

                /* Check the return value is OK */
                if (IMRDRV_ERROR_OK == func_ret)
                {
                    reg_settings[IMRDRV_YUV_REG_IDX_CMRCSR2].value = cmrcr2_value;
                    reg_settings[IMRDRV_YUV_REG_IDX_CMRCSR2].offset = IMRDRV_REG_OFFSET_CMRCSR2;
                }

                /* Check if the order of adjacent Y and U/V is swapped */
                if ((IMRDRV_COLOR_FORM_UYVY == dst_color_format) ||
                    (IMRDRV_COLOR_FORM_VYUY == dst_color_format))
                {
                    /* Set temporary TRICR valuable which is used for updating registers */
                    reg_settings[IMRDRV_YUV_REG_IDX_TRICR].value = IMRDRV_REG_VAL_TRICR_YCFORM;
                    reg_settings[IMRDRV_YUV_REG_IDX_TRICR].offset = IMRDRV_REG_OFFSET_TRICR;
                }
                else
                {
                    reg_settings[IMRDRV_YUV_REG_IDX_TRICR].offset = IMRDRV_REG_OFFSET_INVALID;
                }
                break;
            default:
                /* Set temporary CMRCSR2 valuable which is used for updating registers */
                reg_settings[IMRDRV_YUV_REG_IDX_CMRCSR2].offset = IMRDRV_REG_OFFSET_INVALID;
                /* Set temporary TRICR valuable which is used for updating registers */
                reg_settings[IMRDRV_YUV_REG_IDX_TRICR].offset = IMRDRV_REG_OFFSET_INVALID;
                break;
        }
        
        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            /* Update setting value and offset of registers */
            func_ret = r_imrdrv_devctl_update_val_ofs(&reg_settings[0], IMRDRV_WRITE_YUV_REGISTER_NUM, p_reg_value_offset);
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    
    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_yuv_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_src_reg
* Description  : Decide the setting value of registers related to source data.
* Arguments    : p_src_data             - Source image data
*              : p_reg_value_offset     - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_027]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_src_reg (
    const st_imrdrv_data_t          *const p_src_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t            func_ret;
    st_imrdrv_reg_offset_val_t      tmp_reg_setting[IMRDRV_WRITE_SRC_REGISTER_NUM];
    uint32_t                        susr_suw_val;
    uint32_t                        susr_svw_val;
    
    /* Check parameter */
    if ((NULL != p_src_data) && (NULL != p_reg_value_offset))
    {
        /* Set temporary SSTR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SSTR].offset = IMRDRV_REG_OFFSET_SSTR;
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SSTR].value = p_src_data->stride;
        
        /* Set temporary SSAR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SSAR].offset = IMRDRV_REG_OFFSET_SSAR;
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SSAR].value = p_src_data->phys_addr;
        
        /* Set temporary SUSR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SUSR].offset = IMRDRV_REG_OFFSET_SUSR;
        susr_suw_val = p_src_data->width - IMRDRV_REG_SUSR_SUW_THRESHOLD;
        susr_svw_val = p_src_data->width - IMRDRV_REG_SUSR_SVW_THRESHOLD;
        if ((IMRDRV_COLOR_FORM_Y == p_src_data->color_format) ||
            (IMRDRV_COLOR_FORM_SEP_Y == p_src_data->color_format) ||
            ((IMRDRV_COLOR_FORM_DATA == p_src_data->color_format) && (IMRDRV_BPP_16 == p_src_data->bpp)))
        {
            tmp_reg_setting[IMRDRV_SRC_REG_IDX_SUSR].value = susr_svw_val;
        }
        else
        {
            tmp_reg_setting[IMRDRV_SRC_REG_IDX_SUSR].value = (susr_suw_val <<  IMRDRV_SHIFT_16BIT) | susr_svw_val;
        }
        
        /* Set temporary SVSR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SVSR].offset = IMRDRV_REG_OFFSET_SVSR;
        tmp_reg_setting[IMRDRV_SRC_REG_IDX_SVSR].value = p_src_data->height - IMRDRV_REG_SVSR_SVS_THRESHOLD;
        
        /* Update setting value and offset of registers */
        func_ret = r_imrdrv_devctl_update_val_ofs(tmp_reg_setting, IMRDRV_WRITE_SRC_REGISTER_NUM, p_reg_value_offset);
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    
    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_src_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_dst_reg
* Description  : Decide the setting value of registers related to destination data.
* Arguments    : p_src_data             - Source image data
*              : p_dst_data             - Destination image data
*              : p_reg_value_offset     - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_028]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_dst_reg (
    const st_imrdrv_data_t          *const p_src_data,
    const st_imrdrv_data_t          *const p_dst_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t            func_ret;
    st_imrdrv_reg_offset_val_t      tmp_reg_setting[IMRDRV_WRITE_DST_REGISTER_NUM];
    
    /* Check parameter */
    if ((NULL != p_src_data) && (NULL != p_dst_data) && (NULL != p_reg_value_offset))
    {
        /* Set temporary DSTR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_DST_REG_IDX_DSTR].offset = IMRDRV_REG_OFFSET_DSTR;
        tmp_reg_setting[IMRDRV_DST_REG_IDX_DSTR].value = p_dst_data->stride;
        
        /* Set temporary DSAR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_DST_REG_IDX_DSAR].offset = IMRDRV_REG_OFFSET_DSAR;
        tmp_reg_setting[IMRDRV_DST_REG_IDX_DSAR].value = p_dst_data->phys_addr;
        
        /* Check the color format is changed from interleaved format to separate format */
        if ((IMRDRV_COLOR_FORM_SEP_Y == p_dst_data->color_format) &&
            ((IMRDRV_COLOR_FORM_UYVY == p_src_data->color_format) ||
             (IMRDRV_COLOR_FORM_YVYU == p_src_data->color_format) ||
             (IMRDRV_COLOR_FORM_VYUY == p_src_data->color_format) ||
             (IMRDRV_COLOR_FORM_YUYV == p_src_data->color_format)))
        {
            /* Set temporary DSOR valuable which is used for updating registers */
            tmp_reg_setting[IMRDRV_DST_REG_IDX_DSOR].offset = IMRDRV_REG_OFFSET_DSOR;
            tmp_reg_setting[IMRDRV_DST_REG_IDX_DSOR].value = p_dst_data->stride * p_dst_data->height;
        }
        else
        {
            tmp_reg_setting[IMRDRV_DST_REG_IDX_DSOR].offset = IMRDRV_REG_OFFSET_INVALID;
        }
        
        /* Update setting value and offset of registers */
        func_ret = r_imrdrv_devctl_update_val_ofs(tmp_reg_setting, IMRDRV_WRITE_DST_REGISTER_NUM, p_reg_value_offset);
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    
    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_dst_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_dl_reg
* Description  : Decide the setting value of registers related to DL data.
* Arguments    : p_dl_data             - DL data
*              : p_reg_value_offset    - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_029]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_dl_reg (
    const st_imrdrv_dl_t            *const p_dl_data,
    st_imrdrv_attr_register_value_t *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t            func_ret;
    st_imrdrv_reg_offset_val_t      tmp_reg_setting[IMRDRV_WRITE_DL_REGISTER_NUM];
    
    /* Check parameter */
    if ((NULL != p_dl_data) && (NULL != p_reg_value_offset))
    {
        /* Set temporary DLSAR valuable which is used for updating registers */
        tmp_reg_setting[IMRDRV_DL_REG_IDX_DLSAR].offset = IMRDRV_REG_OFFSET_DLSAR;
        tmp_reg_setting[IMRDRV_DL_REG_IDX_DLSAR].value = p_dl_data->phys_addr;
        
        /* Update setting value and offset of registers */
        func_ret = r_imrdrv_devctl_update_val_ofs(tmp_reg_setting, IMRDRV_WRITE_DL_REGISTER_NUM, p_reg_value_offset);
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    
    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_dl_reg()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_update_val_ofs
* Description  : Update setting value and offset of registers by specified arguments.
* Arguments    : p_reg_settings        - Register settings
*              : reg_setting_num       - The number of register settings
*              : p_reg_value_offset    - Setting value and offset of registers
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_039]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_update_val_ofs (
    const st_imrdrv_reg_offset_val_t      *const p_reg_settings,
    const uint32_t                        reg_setting_num,
    st_imrdrv_attr_register_value_t       *const p_reg_value_offset
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t    func_ret = IMRDRV_ERROR_OK;
    
    /* Check parameter */
    if ((NULL != p_reg_settings) && (NULL != p_reg_value_offset))
    {
        uint32_t local_set_num = p_reg_value_offset->set_num;
        
        /* Check the number of register settings is not out of range */
        if ((local_set_num + reg_setting_num) <= IMRDRV_ATTR_SET_REG_NUM)
        {
            uint32_t settings_idx = 0U;
            uint32_t update_cnt = 0U;
            
            /* Check the index is less than the number of register settings */
            while (settings_idx < reg_setting_num)
            {
                /* Check the register offset is not invalid */
                if (IMRDRV_REG_OFFSET_INVALID != p_reg_settings[settings_idx].offset)
                {
                    /* Update setting value and offset of registers */
                    p_reg_value_offset->reg_offset_value[local_set_num + update_cnt].offset = p_reg_settings[settings_idx].offset;
                    p_reg_value_offset->reg_offset_value[local_set_num + update_cnt].value = p_reg_settings[settings_idx].value;
                    
                    update_cnt++;
                }
                settings_idx++;
            }
            
            /* Set the number of setting values */
            p_reg_value_offset->set_num = local_set_num + update_cnt;
        }
        else
        {
            /* Set parameter error to return value */
            func_ret = IMRDRV_ERROR_PAR;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }
    
    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_update_val_ofs()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_get_int_yuv_val
* Description  : Get the value of YUV setting in CMRCR2 about interleaved format
* Arguments    : src_color_format      - Source image color format
*              : dst_color_format      - Destination image color format
*              : p_value               - Value of YUV setting in CMRCR2
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_045]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_get_int_yuv_val (
    const e_imrdrv_color_format_t       src_color_format,
    const e_imrdrv_color_format_t       dst_color_format,
          uint32_t                      *const p_value
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret;

    /* Check parameter */
    if (NULL != p_value)
    {
        bool src_u_v_swap;
        bool src_y_uv_swap;

        e_imrdrv_color_format_t tmp_dst_format;
        /* Check destination fomat is interleaved format*/
        if ((IMRDRV_COLOR_FORM_UYVY == dst_color_format) ||
            (IMRDRV_COLOR_FORM_YVYU == dst_color_format) ||
            (IMRDRV_COLOR_FORM_VYUY == dst_color_format) ||
            (IMRDRV_COLOR_FORM_YUYV == dst_color_format))
        {
            /* Set Destination image color format to tmp_dst_format */
            tmp_dst_format = dst_color_format;
        }
        else
        {
            /* Set interleave format witch is the basic YUV order to tmp_dst_format */
            tmp_dst_format = IMRDRV_COLOR_FORM_YUYV;
        }

        /* Check the order of YUV in the source image color format */
        func_ret = r_imrdrv_devctl_chk_int_yuv_swp(src_color_format,
                                                   &src_y_uv_swap,
                                                   &src_u_v_swap);

        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            bool dst_u_v_swap;
            bool dst_y_uv_swap;
            /* Check the order of YUV in the destination image color format */
            func_ret = r_imrdrv_devctl_chk_int_yuv_swp(tmp_dst_format,
                                                       &dst_y_uv_swap,
                                                       &dst_u_v_swap);
            /* Check the return value is OK */
            if (IMRDRV_ERROR_OK == func_ret)
            {
                uint32_t value = IMRDRV_REG_VAL_CMRCSR2_YUV422ES;
                
                /* Check the order of Y and UV */
                if ((IMRDRV_COLOR_FORM_UYVY == src_color_format) ||
                    (IMRDRV_COLOR_FORM_VYUY == src_color_format))
                {
                    value |= IMRDRV_REG_VAL_CMRCSR2_422FORMS;
                }
                
                /* Check the order of U and V are not same */
                if (src_u_v_swap != dst_u_v_swap)
                {
                    value |= IMRDRV_REG_VAL_CMRCSR2_UVFORMS;
                }

                *p_value = value;
            }
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;

    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_get_int_yuv_val()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: r_imrdrv_devctl_chk_int_yuv_swp
* Description  : Check the order of YUV is swapped from the basic YUV order (YUYV)
* Arguments    : color_format          - color format
*              : p_y_uv_swap           - the order of Y and UV are swapped
*              : p_u_v_swap            - the order of U and V are swapped
* Return Value : IMRDRV_ERROR_OK
*                IMRDRV_ERROR_PAR
* [Covers: UD_RD_UD01_01_03_046]
**********************************************************************************************************************/
IMRDRV_STATIC e_imrdrv_errorcode_t r_imrdrv_devctl_chk_int_yuv_swp (
    const e_imrdrv_color_format_t color_format,
          bool                    *const p_y_uv_swap,
          bool                    *const p_u_v_swap
)
{
    /* Initialize internal variables */
    e_imrdrv_errorcode_t func_ret = IMRDRV_ERROR_OK;

    /* Check parameter */
    if ((NULL != p_y_uv_swap) && (NULL != p_u_v_swap))
    {
        bool y_uv_swap;
        bool u_v_swap;

        /* Check color format */
        switch (color_format)
        {
            case IMRDRV_COLOR_FORM_UYVY:
                y_uv_swap = true;
                u_v_swap  = false;
                break;
            case IMRDRV_COLOR_FORM_YVYU:
                y_uv_swap = false;
                u_v_swap  = true;
                break;
            case IMRDRV_COLOR_FORM_VYUY:
                y_uv_swap = true;
                u_v_swap  = true;
                break;
            case IMRDRV_COLOR_FORM_YUYV:
                y_uv_swap = false;
                u_v_swap  = false;
                break;
            default:
                /* Set parameter error to return value */
                func_ret = IMRDRV_ERROR_PAR;
                break;
        }
        
        /* Check the return value is OK */
        if (IMRDRV_ERROR_OK == func_ret)
        {
            *p_y_uv_swap = y_uv_swap;
            *p_u_v_swap = u_v_swap;
        }
    }
    else
    {
        /* Set parameter error to return value */
        func_ret = IMRDRV_ERROR_PAR;
    }

    /* Return value */
    return func_ret;
}
/***********************************************************************************************************************
* End of function r_imrdrv_devctl_chk_int_yuv_swp()
***********************************************************************************************************************/
