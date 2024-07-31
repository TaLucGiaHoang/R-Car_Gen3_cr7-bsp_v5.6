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


#ifndef R_NETIF_API_H
#define  R_NETIF_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define R_NETIF_ERR_OK                   (0)
#define R_NETIF_ERR_FAILED               (-1)

#define R_NETIF_HEADER_SIZE      (14)
#define R_NETIF_MACADDR_SIZE     (6)
/* ring buffer size */
#define R_NETIF_PRV_RXBUF_SIZE   (1500)                                       /**< Receive ring buffer number of messages */
#define R_NETIF_PRV_TXBUF_SIZE   (1500)                                       /**< Transmit ring buffer number of messages */

/* Interrupt ID */
#define R_NETIF_AVBCH00        (39 + 32 - 71)
#define R_NETIF_AVBCH01
#define R_NETIF_AVBCH02
#define R_NETIF_AVBCH03

#define R_NETIF_AVBCH18        (39 + 32 + 18 - 71)
#define R_NETIF_AVBCH19
#define R_NETIF_AVBCH20
#define R_NETIF_AVBCH21

#define R_NETIF_AVBCH22
#define R_NETIF_AVBCH23
#define R_NETIF_AVBCH24

int R_NetIf_HwInit(void);
int R_NetIf_HwStart(void);
int R_NetIf_HwLink(void);
int R_NetIf_HwStop(void);
int R_NetIf_ProTxBuffer(uint32_t * const bufIdx, uint8_t ** const bufPtr, uint16_t * const dataLen);
void R_NetIf_RxIrq(void);
int R_NetIf_Transmit(uint32_t bufIdx, uint16_t type, uint32_t dataLen, uint8_t *tgMacAddr);
void R_NetIf_TxIrq(void);
void R_NetIf_Get_MacAddr(uint8_t* addr);
int R_NetIf_TxConfirmation(void);
#ifdef __cplusplus
}
#endif

#endif /* R_NETIF_API_H */

/** @} */
