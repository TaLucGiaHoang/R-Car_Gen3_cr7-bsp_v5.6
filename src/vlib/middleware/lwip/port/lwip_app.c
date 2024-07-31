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

/* Standard includes. */
#include <string.h>

#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"
#include "Eth_Irq.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "netif/r_ethif.h"
#include "r_lwip_lib.h"

/* default netif parameters */
#define ETHADDR_STRLEN_MAX	18
#define DEFAULT_ETHADDR		"2e:09:0a:06:d4:72"
#define DEFAULT_IPADDR		"192.168.0.20"
#define DEFAULT_GW			"192.168.0.1"
#define DEFAULT_MASK		"255.255.255.0"

#define ETHER_RX_MQ_ID		(0x8000U + 66U)
#define ETHER_MQ_NUM_OF_MSG	1
#define ETHER_MQ_MSG_SIZE	sizeof(uint32_t)
#define ETHER_MQ_TIMEOUT_MS	1000U

#define LOC_LWIP_PRIO		220
static void vSetNetIfParam(struct netif *netif, ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	unsigned int hwaddr[ETH_HWADDR_LEN];
	int i;
		/* Set up the network interface with default parameter. */
	inet_aton(DEFAULT_IPADDR, ip);
	inet_aton(DEFAULT_MASK, mask);
	inet_aton(DEFAULT_GW, gw);
	netif->hwaddr_len = ETH_HWADDR_LEN;
	sscanf(DEFAULT_ETHADDR, "%2x:%2x:%2x:%2x:%2x:%2x",
			&hwaddr[0], &hwaddr[1], &hwaddr[2], &hwaddr[3], &hwaddr[4], &hwaddr[5]);
	for (i = 0; i < ETH_HWADDR_LEN; i++) {
		netif->hwaddr[i] = (uint8_t)hwaddr[i];
	}
}
void vStatusCallback( struct netif *pxNetIf )
{
	char pcMessage[20];
	struct in_addr* pxIPAddress;

	if( netif_is_up( pxNetIf ) != 0 )
	{
		strcpy( pcMessage, "IP=" );
		pxIPAddress = ( struct in_addr* ) &( pxNetIf->ip_addr );
		strcat( pcMessage, inet_ntoa( ( *pxIPAddress ) ) );
		printf( "%s\n", pcMessage );
	}
	else
	{
		printf( "Network is down" );
	}
}

osal_device_handle_t Ether_device_handle;
osal_mq_handle_t Ether_mq_handle_rx;
osal_mq_handle_t Ether_mq_handle_received;

static void *r_eth_input_thread(void *netif)
{
	e_osal_return_t osal_ret = OSAL_RETURN_OK;
	uint32_t recvdata = 0;
	struct netif* Netif = (struct netif*)netif;
	struct xemac_s *emac = (struct xemac_s *)Netif->state;

	while (1) {
		/* indication a message was received */
		osal_ret = R_OSAL_MqReceiveForTimePeriod(Ether_mq_handle_rx, ETHER_MQ_TIMEOUT_MS, (void *)&recvdata, ETHER_MQ_MSG_SIZE);
		if ((OSAL_RETURN_OK != osal_ret) && (OSAL_RETURN_TIME != osal_ret)) {
			R_PRINT_Log("[loc_Eth_RxIrqThread]: R_OSAL_MqReceiveForTimePeriod failed(%d)\r\n", osal_ret);
		}
		else if (OSAL_RETURN_OK == osal_ret) {
			r_ethif_input(netif, recvdata);
			osal_ret = R_OSAL_InterruptEnableIsr(Ether_device_handle, emac->rx_irq_num);
		}
	}
}

void lwIPAppsInit(void *pvArgument)
{
	ip_addr_t xIPAddr, xNetMask, xGateway;
	static struct netif xNetIf;
	st_osal_thread_config_t thread_config;
	osal_thread_handle_t in_thread;
	osal_thread_handle_t out_thread;
	e_osal_return_t osal_ret = OSAL_RETURN_FAIL;
	st_osal_mq_config_t Ether_Rx_mq_config;
	size_t numOfByte = 0;
	char devicelist[32];
	struct xemac_s *emac = (struct xemac_s *)xNetIf.state;

	/* Open device */
	osal_ret = R_OSAL_IoGetDeviceList("ethernet", &devicelist[0], sizeof(devicelist), &numOfByte);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_IoGetDeviceList failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}
	osal_ret = R_OSAL_IoDeviceOpen(&devicelist[0], &Ether_device_handle);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_IoDeviceOpen failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}

	/* Register Rx/Tx Interrupts*/
	osal_ret = R_OSAL_InterruptRegisterIsr(Ether_device_handle, 0, 0, (p_osal_isr_func_t)Eth_AvbCh00Isr, (void *)&xNetIf);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_InterruptRegisterIsr(Rx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}
	osal_ret = R_OSAL_InterruptRegisterIsr(Ether_device_handle, 18, 0, (p_osal_isr_func_t)Eth_AvbCh18Isr, (void *)&xNetIf);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_InterruptRegisterIsr(Tx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}

	/* Disable Rx/Tx Interrupts */
	osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, 0);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_InterruptDisableIsr(Rx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}
	osal_ret = R_OSAL_InterruptDisableIsr(Ether_device_handle, 18);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_InterruptDisableIsr(Tx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}

	ip_addr_set_zero(&xGateway);
	ip_addr_set_zero(&xIPAddr);
	ip_addr_set_zero(&xNetMask);
	vSetNetIfParam(&xNetIf, &xIPAddr, &xNetMask, &xGateway);
	netif_add(&xNetIf, &xIPAddr, &xNetMask, &xGateway, NULL, r_ethif_init, tcpip_input);
	netif_set_default(&xNetIf);
	netif_set_status_callback(&xNetIf, vStatusCallback);
	if (netif_is_link_up(&xNetIf)) {
#if LWIP_DHCP
		dhcp_start(&xNetIf);
#else
		netif_set_up(&xNetIf);
#endif
	}

	// TODO move this to app itself? maybe?
	Ether_Rx_mq_config.max_num_msg = ETHER_MQ_NUM_OF_MSG;
	Ether_Rx_mq_config.msg_size = ETHER_MQ_MSG_SIZE;
	osal_ret = R_OSAL_MqCreate(&Ether_Rx_mq_config, (osal_mq_id_t)ETHER_RX_MQ_ID, &Ether_mq_handle_rx);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_MqCreate(Ether_mq_handle_rx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}

	thread_config.priority = OSAL_THREAD_PRIORITY_TYPE13;
	thread_config.task_name = "lwIP_In";
	thread_config.func = (p_osal_thread_func_t)r_eth_input_thread;
	thread_config.userarg = (void *)&xNetIf;
	thread_config.stack_size = 0;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, (0x8000U + 89U), &in_thread);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_ThreadCreate(lwIP_In) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}

	/* Enable Rx/Tx Interrupts */
	osal_ret = R_OSAL_InterruptEnableIsr(Ether_device_handle, 0);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_InterruptEnableIsr(Rx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}
	osal_ret = R_OSAL_InterruptEnableIsr(Ether_device_handle, 18);
	if (OSAL_RETURN_OK != osal_ret) {
		R_PRINT_Log("[LWIP] %s: R_OSAL_InterruptEnableIsr(Tx) failed. ret=%d\r\n", __func__, osal_ret);
		return;
	}
}

void vLwipStart(void)
{
	tcpip_init(lwIPAppsInit, NULL);
	return;
}