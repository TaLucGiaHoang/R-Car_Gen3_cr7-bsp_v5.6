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
#include <stdio.h>
#include <string.h>

#include "lwipopts.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"

#include "netif/etharp.h"

#include "rcar-xos/osal/r_osal.h"
#include "r_ethif.h"
#include "r_netif_api.h"
#include "r_print_api.h"

#include "ether_api.h"
#include "Eth_Irq.h"

/*we need a lower-level interface*/

/* Define those to better describe your network interface. */
#define IFNAME0 't'
#define IFNAME1 'e'

#define ENABLE_IRQ() asm("cpsie i\n\t")
#define DISABLE_IRQ() asm("cpsid i\n\t")

volatile uint16_t Eth_Msg_length = 0;
volatile uint8_t TxConfirmed = 0;
volatile uint32_t RXEthFrameAddr = 0;
volatile uint8_t EthRcvBroadcastMsg;
volatile uint16_t EthRcvFrameType;
volatile uint8_t EthRcvPhysAddr[R_NETIF_MACADDR_SIZE];

struct netif *NetIf;
static struct xemac_s loc_emac;

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{

    int err;
    uint32_t bufIdx = 0;
    uint16_t dataLen = 0;
    uint16_t lucCount;
    struct pbuf *q;
    struct eth_hdr *ethhdr;
    uint8_t *txBuf;
    uint8_t *buf;
    ethhdr = p->payload;
    dataLen = p->tot_len - sizeof(struct eth_hdr);
    err = R_NetIf_ProTxBuffer(&bufIdx, &txBuf, &dataLen);
    if(err != R_NETIF_ERR_OK) {
        R_PRINT_Log("e:%d len:%d %s %d\r\n",err, dataLen, __func__, __LINE__);
        return ERR_IF;
    }
    buf = (uint8_t *)p->payload;
    for(lucCount = 0; lucCount < (p->len - sizeof(struct eth_hdr)); lucCount++) {
        *txBuf = buf[lucCount + sizeof(struct eth_hdr)];
        txBuf ++;
    }
    q = p->next;
    while(q != NULL) {
        buf = (uint8_t *)q->payload;
        for(lucCount = 0; lucCount < q->len; lucCount++) {
            *txBuf = buf[lucCount];
            txBuf ++;
        }
        q = q->next;
    }
    err = R_NetIf_Transmit(bufIdx, PP_HTONS(ethhdr->type), dataLen, &(ethhdr->dest.addr[0]));
    if(err != R_NETIF_ERR_OK) {
        return ERR_IF;
    }
    R_NetIf_TxConfirmation();
    return ERR_OK;
}

extern osal_memory_manager_handle_t    g_hndl_mmngr;
static struct pbuf * low_level_input(struct netif *netif, uint32_t recvdata)
{   
    e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    uint16_t recvlen = 0;
    uint8_t *data;
    uint16_t offset = 0;

    Eth_ProcessRxInterrupt(recvdata);

    data = (uint8_t *)RXEthFrameAddr;
    recvlen = Eth_Msg_length + R_NETIF_HEADER_SIZE;
    if(NULL != data) {
        data = data - R_NETIF_HEADER_SIZE;
#if 1
        p = pbuf_alloc(PBUF_RAW, recvlen, PBUF_POOL);
        // TODO 
        // pretty sure we need to use Mmngr APIs instead of pbuf_alloc
        // because mmngr "maintains" the heap and pbuf_alloc isn't aware
        // so they stomp on each other
        // R_OSAL_MmngrAlloc(g_hndl_mmngr, recvlen + 4, 128, &p);
        if(NULL != p) {
            for (q = p; q != NULL; q = q->next)
            {
                memcpy(q->payload, (char *)&data[0] + offset, q->len);
                offset = offset + q->len;
            }
        }
#else
        p = pbuf_alloc(PBUF_RAW, recvlen, PBUF_REF);
        p->payload = (void*)data;      
#endif
    }
    RXEthFrameAddr = 0;
    Eth_Msg_length = 0;
    return p;
}


static err_t r_ethif_output(struct netif *netif, struct pbuf *p,
		struct ip4_addr *ipaddr)
{
	/* resolve hardware address, then send (or queue) packet */
	return etharp_output(netif, p, ipaddr);
}

void r_ethif_tx_isr(uint32_t channel)
{
    Eth_ProcessTxInterrupt(channel);
}

void r_ethif_input(struct netif *netif, uint32_t channel)
{
    err_t err;
    struct pbuf *p;
    DISABLE_IRQ();
    p = low_level_input(netif, channel);
    ENABLE_IRQ();
    /* no packet could be read, silently ignore this */
    if (p == NULL) {
        return;
    }
    DISABLE_IRQ();
    err = netif->input(p, netif);
    if (err != ERR_OK)
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(p);
        p = NULL;
    }
    ENABLE_IRQ();
	return;
}

static err_t low_level_init(struct netif *netif)
{   
    int i;
    uint8_t mac_address[R_NETIF_MACADDR_SIZE] = {0x2EU,0x09U,0x0AU,0x06U,0xD4U,0x72U};
    R_NetIf_HwInit();
    R_NetIf_HwStart();
    // TODO
    // R_OSAL_CountSemaCreate(&loc_emac.rx_sema);
    // R_OSAL_CountSemaCreate(&loc_emac.tx_sema);
    loc_emac.rx_irq_num = R_NETIF_AVBCH00;
    loc_emac.tx_irq_num = R_NETIF_AVBCH18;
    // TODO
    // R_OSAL_IrqRegisterSem(loc_emac.rx_irq_num, &loc_emac.rx_sema);
    // R_OSAL_IrqRegisterSem(loc_emac.tx_irq_num, &loc_emac.tx_sema);
    R_NetIf_HwLink();
    netif->mtu = R_NETIF_PRV_RXBUF_SIZE - R_NETIF_HEADER_SIZE;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    for(i = 0;i < R_NETIF_MACADDR_SIZE; i++)
    {
        netif->hwaddr[i] = mac_address[i];
    }
    netif->hwaddr_len = R_NETIF_MACADDR_SIZE;
    netif->state = &loc_emac;
    return ERR_OK;
}

err_t r_ethif_init(struct netif *netif)
{
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->output = (netif_output_fn)r_ethif_output;
	netif->linkoutput = low_level_output;

	low_level_init(netif);

	return ERR_OK;
}