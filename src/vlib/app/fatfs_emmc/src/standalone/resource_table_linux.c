/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : resource_table_linux.c
 * Version      : 1.0
 * Description  : List remote processor firmware information that will be accessed by Linux kernel.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "resource_table.h"
#include "virtio_ids.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define NUM_RES_ENTRIES 1

#define CR7_RPMSG1_ID 0

#define CR7_RPMSG1_VRING0_DA   0
#define CR7_RPMSG1_VRING0_SIZE 512
#define CR7_RPMSG1_VRING0_ID   1
#define CR7_RPMSG1_VRING0_PA   0

#define CR7_RPMSG1_VRING1_DA   0
#define CR7_RPMSG1_VRING1_SIZE 512
#define CR7_RPMSG1_VRING1_ID   2
#define CR7_RPMSG1_VRING1_PA   0

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
__attribute__((section(".resource_table")))
__attribute__((aligned(4096)))
struct cr7_resource_table resource_table = {
    /* struct resource_table base */
    .base = {
        .ver = 1,
        .num = NUM_RES_ENTRIES,
        {0, 0},
    },

    /* u32 offsets[NUM_RES_ENTRIES] */
    .offsets = {
        offsetof(struct cr7_resource_table, rpmsg1_header),
    },

    /* struct fw_rsc_hdr rpmsg1_header */
    .rpmsg1_header = {
        .type = RSC_VDEV,
    },
    /* struct fw_rsc_vdev rpmsg1 */
    .rpmsg1 = {
        .id = VIRTIO_ID_RPMSG,
        .notifyid = CR7_RPMSG1_ID,
        .dfeatures = 1, //fw features, VIRTIO_RPMSG_F_NS
        .gfeatures = 0, //negotiated features place holder
        .config_len = 0,
        .status = 0,
        .num_of_vrings = 2,
        .reserved = {0},
    },
    /* struct fw_rsc_vdev_vring rpmsg1_vring0 RX */
    .rpmsg1_vring0 = {
        .da = CR7_RPMSG1_VRING0_DA,
        .align = 4096,
        .num = CR7_RPMSG1_VRING0_SIZE,
        .notifyid = CR7_RPMSG1_VRING0_ID,
        .pa = CR7_RPMSG1_VRING0_PA,
    },
    /* struct fw_rsc_vdev_vring rpmsg1_vring1 TX */
    .rpmsg1_vring1 = {
        .da = CR7_RPMSG1_VRING1_DA,
        .align = 4096,
        .num = CR7_RPMSG1_VRING1_SIZE,
        .notifyid = CR7_RPMSG1_VRING1_ID,
        .pa = CR7_RPMSG1_VRING1_PA,
    },
};
