/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_dev.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver Basic Function Device Dependent Control Public header
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 08.06.2021 0.01     New create
 *         : 14.06.2021 0.02     Add header file include.
 *                               Add function prototype, macro definition and structure definition.
 *                               Modify structure member of st_imrdrv_basic_device_area_t.
 *         : 29.10.2021 0.03     Remove function prototype r_imrdrv_devctl_clear_opt_attr().
 *                               Remove structure st_imrdrv_option_device_area_t.
 *********************************************************************************************************************/

#ifndef R_IMRDRV_BASIC_DEV_H
#define R_IMRDRV_BASIC_DEV_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "r_imrdrv_osal_io.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Register's offset */
#define IMRDRV_REG_OFFSET_CR        (0x00000008U)      /* CR register's offset */
#define IMRDRV_REG_OFFSET_SR        (0x0000000CU)      /* SR register's offset */
#define IMRDRV_REG_OFFSET_SRCR      (0x00000010U)      /* SRCR register's offset */
#define IMRDRV_REG_OFFSET_ICR       (0x00000014U)      /* ICR register's offset */
#define IMRDRV_REG_OFFSET_IMR       (0x00000018U)      /* IMR register's offset. */
#define IMRDRV_REG_OFFSET_DLSAR     (0x00000030U)      /* DLSAR register's offset. */
#define IMRDRV_REG_OFFSET_DSAR      (0x00000034U)      /* DSAR register's offset */
#define IMRDRV_REG_OFFSET_SSAR      (0x00000038U)      /* SSAR register's offset */
#define IMRDRV_REG_OFFSET_DSTR      (0x0000003CU)      /* DSTR register's offset */
#define IMRDRV_REG_OFFSET_SSTR      (0x00000040U)      /* SSTR register's offset */
#define IMRDRV_REG_OFFSET_DSOR      (0x00000050U)      /* DSOR register's offset */
#define IMRDRV_REG_OFFSET_CMRCSR    (0x00000058U)      /* CMRCSR register's offset */
#define IMRDRV_REG_OFFSET_CMRCCR    (0x0000005CU)      /* CMRCCR register's offset */
#define IMRDRV_REG_OFFSET_TRIMSR    (0x00000064U)      /* TRIMSR register's offset */
#define IMRDRV_REG_OFFSET_TRIMCR    (0x00000068U)      /* TRIMCR register's offset */
#define IMRDRV_REG_OFFSET_TRICR     (0x0000006CU)      /* TRICR register's offset */
#define IMRDRV_REG_OFFSET_SUSR      (0x00000074U)      /* SUSR register's offset */
#define IMRDRV_REG_OFFSET_SVSR      (0x00000078U)      /* SVSR register's offset */
#define IMRDRV_REG_OFFSET_CMRCSR2   (0x000000E8U)      /* CMRCSR2 register's offset */
#define IMRDRV_REG_OFFSET_CMRCCR2   (0x000000ECU)      /* CMRCCR2 register's offset */
#define IMRDRV_REG_OFFSET_INVALID   (0xFFFFFFFFU)      /* Invalid register's offset */

#define IMRDRV_REG_VAL_IMR_RESERVE  (0x00000018U)      /* IMR register's setting for reserve. */
#define IMRDRV_REG_VAL_IMR_INM      (0x00000004U)      /* IMR register's setting value to mask INT interrupt. */
#define IMRDRV_REG_VAL_IMR_IEM      (0x00000002U)      /* IMR register's setting value to mask IER interrupt. */
#define IMRDRV_REG_VAL_IMR_TRAM     (0x00000001U)      /* IMR register's setting value to mask TRAP interrupt. */

/* IMR register's setting value to mask all interrupt. */
#define IMRDRV_REG_VAL_IMR_MASK     (IMRDRV_REG_VAL_IMR_RESERVE | \
                                     IMRDRV_REG_VAL_IMR_INM | \
                                     IMRDRV_REG_VAL_IMR_IEM | \
                                     IMRDRV_REG_VAL_IMR_TRAM)

#define IMRDRV_REG_VAL_SR_REN       (0x00000020U)      /* SR.REN register's value to mask SR register's value */
#define IMRDRV_REG_VAL_SR_INT       (0x00000004U)      /* SR.INT register's value to mask SR register's value */
#define IMRDRV_REG_VAL_SR_IER       (0x00000002U)      /* SR.IER register's value to mask SR register's value */
#define IMRDRV_REG_VAL_SR_TRA       (0x00000001U)      /* SR.TRA register's value to mask SR register's value */

/* SR register's value to mask SR register's all bit value. */
#define IMRDRV_REG_VAL_SR_ALL       (IMRDRV_REG_VAL_SR_REN | \
                                     IMRDRV_REG_VAL_SR_INT | \
                                     IMRDRV_REG_VAL_SR_IER | \
                                     IMRDRV_REG_VAL_SR_TRA)

#define IMRDRV_REG_VAL_SRCR_INTCLR  (0x00000004U)      /* SRCR register's value to clear SR register's INT bit. */
#define IMRDRV_REG_VAL_SRCR_IERCLR  (0x00000002U)      /* SRCR register's value to clear SR register's IER bit. */
#define IMRDRV_REG_VAL_SRCR_TRACLR  (0x00000001U)      /* SRCR register's value to clear SR register's TRA bit. */
/* SRCR register's value to clear SR register's all bits. */
#define IMRDRV_REG_VAL_SRCR_ALL     (IMRDRV_REG_VAL_SRCR_INTCLR | \
                                     IMRDRV_REG_VAL_SRCR_IERCLR | \
                                     IMRDRV_REG_VAL_SRCR_TRACLR)

/* The number of attribute parameter setting register */
#define IMRDRV_ATTR_SET_REG_NUM      (12U)             /* The number of attribute parameter setting register. */

/* The upper limit of stride */
#define IMRDRV_UPPER_LIMIT_STRIDE    (8192U)           /* The upper limit of stride. */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    uint32_t    offset;     /* Register offset. */
    uint32_t    value;      /* Register value to write. */
} st_imrdrv_reg_offset_val_t;

typedef struct
{
    e_imrdrv_channelno_t           channel_no;          /* IMR-LX4 channel number. */
} st_imrdrv_basic_device_area_t;

typedef struct
{
    uint32_t                     set_num;   /* The number of setting values. */
    st_imrdrv_reg_offset_val_t   reg_offset_value[IMRDRV_ATTR_SET_REG_NUM];
                                            /* Register offset and value. */
} st_imrdrv_attr_register_value_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t r_imrdrv_devctl_init (
    const e_imrdrv_channelno_t      channel_no,
    st_imrdrv_basic_device_area_t   *const p_basic_device_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_check_ch (
    const e_imrdrv_channelno_t  channel_no
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_quit (
    st_imrdrv_basic_device_area_t   *const p_basic_device_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_reset_reg (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_io_create (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    st_imrdrv_osal_io_area_t            *const p_osal_io_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_enable_irq (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_write_reg (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const uint32_t                      offset,
    const uint32_t                      value,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_read_reg (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const uint32_t                      offset,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area,
    uint32_t                            *const p_value
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_io_destroy (
    st_imrdrv_osal_io_area_t    *const p_osal_io_area
);
extern e_imrdrv_errorcode_t r_imrdrv_devctl_disable_irq (
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    const st_imrdrv_osal_io_area_t      *const p_osal_io_area
);
e_imrdrv_errorcode_t r_imrdrv_devctl_chk_stride (
    const uint32_t              stride
);

e_imrdrv_errorcode_t r_imrdrv_devctl_chk_color_form (
    const e_imrdrv_color_format_t       color_format,
    const e_imrdrv_bpp_t                bpp
);

e_imrdrv_errorcode_t r_imrdrv_devctl_chk_tri_mode (
    const e_imrdrv_mode_t       uvshfval,
    const e_imrdrv_mode_t       shfval,
    const e_imrdrv_mode_t       uvshfe,
    const e_imrdrv_mode_t       shfe,
    const e_imrdrv_mode_t       rde
);

e_imrdrv_errorcode_t r_imrdrv_devctl_chk_comb_form (
    const e_imrdrv_color_format_t       src_color_format,
    const e_imrdrv_color_format_t       dst_color_format
);

e_imrdrv_errorcode_t r_imrdrv_devctl_get_reg_val (
    const st_imrdrv_attr_param_t        *const p_attr_param,
    const st_imrdrv_basic_device_area_t *const p_basic_device_area,
    st_imrdrv_attr_register_value_t     *const p_reg_value_offset
);

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_BASIC_DEV_H */

