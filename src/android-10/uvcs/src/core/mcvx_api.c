/*************************************************************************/ /*
 VCP core module

 Copyright (C) 2015 - 2018 Renesas Electronics Corporation

 License        Dual MIT/GPLv2

 The contents of this file are subject to the MIT license as set out below.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 Alternatively, the contents of this file may be used under the terms of
 the GNU General Public License Version 2 ("GPL") in which case the provisions
 of GPL are applicable instead of those above.

 If you wish to allow use of your version of this file only under the terms of
 GPL, and not to allow others to use your version of this file under the terms
 of the MIT license, indicate your decision by deleting the provisions above
 and replace them with the notice and other provisions required by GPL as set
 out in the file called "GPL-COPYING" included in this distribution. If you do
 not delete the provisions above, a recipient may use your version of this file
 under the terms of either the MIT license or GPL.

 This License is also included in this distribution in the file called
 "MIT-COPYING".

 EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
 PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 GPLv2:
 If you wish to use this file under the terms of GPL, following terms are
 effective.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/ /*************************************************************************/

#include "mcvx_api.h"
#include <linux/kernel.h>
#include <linux/hwspinlock.h>
#include <asm/io.h>
#include <linux/delay.h>

/**
 *
 * \brief			Initialize CODEC IP
 *
 * \param[in]		config					: IP config information
 * \param[out]		ip						: IP context
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 * \retval			MCVX_ERR_SEVERE			: severe error
 *
******************************************************************************/
MCVX_RC mcvx_ip_init(
	const MCVX_IP_CONFIG_T	*config,
	MCVX_IP_CTX_T			*ip)
{
	MCVX_RC				rtn_code;
	MCVX_REG			reg_data;

	if (config == NULL) {
		rtn_code = MCVX_ERR_PARAM;
	} else if (ip == NULL) {
		rtn_code = MCVX_ERR_PARAM;
	} else if ((config->udf_reg_read  == NULL) || (config->udf_reg_write == NULL)) {
		rtn_code = MCVX_ERR_PARAM;
	} else if ((config->ip_vlc_base_addr == NULL) || (config->ip_ce_base_addr == NULL)) {
		rtn_code = MCVX_ERR_PARAM;
	} else if ((config->ip_arch != MCVX_ARCH_VCP3) && (config->ip_arch != MCVX_ARCH_BELZ)) {
		rtn_code = MCVX_ERR_PARAM;
	} else {
		ip->vlc_handled_udp		= NULL;
		ip->ce_handled_udp		= NULL;
		ip->fcv_handled_udp		= NULL;

		ip->udf_vlc_event		= NULL;
		ip->udf_ce_event		= NULL;
		ip->udf_fcv_event		= NULL;

		ip->hw_vlc_state		= MCVX_STATE_VLC_IDLE;
		ip->hw_ce_state			= MCVX_STATE_CE_IDLE;
		ip->hw_fcv_state		= MCVX_STATE_FCV_IDLE;

		ip->ip_id				= config->ip_id;
		ip->ip_arch				= config->ip_arch;
		ip->udf_reg_read		= config->udf_reg_read;
		ip->udf_reg_write		= config->udf_reg_write;

		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			ip->REG_VCP3_VLC		= (MCVX_REG_VCP3_VLC_T *)config->ip_vlc_base_addr;
			ip->REG_VCP3_CE			= (MCVX_REG_VCP3_CE_T *)config->ip_ce_base_addr;
			ip->REG_BELZ_VLC		= NULL;
			ip->REG_BELZ_CE			= NULL;
		} else { /* MCVX_ARCH_BELZ */
			ip->REG_VCP3_VLC		= NULL;
			ip->REG_VCP3_CE			= NULL;
			ip->REG_BELZ_VLC		= (MCVX_REG_BELZ_VLC_T *)config->ip_vlc_base_addr;
			ip->REG_BELZ_CE			= (MCVX_REG_BELZ_CE_T *)config->ip_ce_base_addr;
		}

		ip->vlc_hung_up			= MCVX_FALSE;
		ip->ce_hung_up			= MCVX_FALSE;

		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			/* check VLC base address by VCID */
			reg_data					= 0u;
			ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_VCRH, &reg_data, MCVX_REGS_SINGLE);
			if ((reg_data & MCVX_M_16BIT) != MCVX_VCID) {
				rtn_code				= MCVX_ERR_SEVERE;
			} else {
				/* check CE base address by VCID */
				reg_data				= 0u;
				ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_VCRH, &reg_data, MCVX_REGS_SINGLE);
				if ((reg_data & MCVX_M_16BIT) != MCVX_VCID) {
					rtn_code			= MCVX_ERR_SEVERE;
				} else { /* VLC base address and CE base address are ok */

					/* clear interrupt related register begin */
					reg_data			= 0u;
					/* VLC */
					ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_IRQ_ENB, &reg_data, MCVX_REGS_SINGLE);
					ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_IRQ_STA, &reg_data, MCVX_REGS_SINGLE);

					/* CE */
					ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_IRQ_ENB, &reg_data, MCVX_REGS_SINGLE);
					ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_IRQ_STA, &reg_data, MCVX_REGS_SINGLE);
					/* clear interrupt related register end */

					/* reset vlc */ /* rtn_code can be ignored */
					(void)mcvx_vlc_reset(ip, MCVX_RESET_FORCED);

					/* reset ce */  /* rtn_code can be ignored */
					(void)mcvx_ce_reset(ip, MCVX_RESET_FORCED);

					/* store IP version, capability */
					ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_VCRL, &ip->ip_version,	MCVX_REGS_SINGLE);
					ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_CFG,  &ip->ip_capability, MCVX_REGS_SINGLE);

					/* store stepping */
					ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_STEPPING,	&ip->vlc_stepping,	MCVX_REGS_SINGLE);
					ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_STEPPING,		&ip->ce_stepping,	MCVX_REGS_SINGLE);

					rtn_code = MCVX_NML_END;
				}
			}
		} else {
			/* check VLC base address by VCID */
			reg_data					= 0u;
			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_VCRH, &reg_data, MCVX_REGS_SINGLE);
			if ((reg_data & MCVX_M_16BIT) != MCVX_VCID) {
				rtn_code				= MCVX_ERR_SEVERE;
			} else {
				/* check CE base address by VCID */
				reg_data				= 0u;
				ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_VCRH, &reg_data, MCVX_REGS_SINGLE);
				if ((reg_data & MCVX_M_16BIT) != MCVX_VCID) {
					rtn_code			= MCVX_ERR_SEVERE;
				} else { /* VLC base address and CE base address are ok */

					/* clear interrupt related register begin */
					reg_data			= 0u;
					/* VLC */
					ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_IRQ_ENB, &reg_data, MCVX_REGS_SINGLE);
					ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_IRQ_STA, &reg_data, MCVX_REGS_SINGLE);

					/* CE */
					ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_IRQ_ENB, &reg_data, MCVX_REGS_SINGLE);
					ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_IRQ_STA, &reg_data, MCVX_REGS_SINGLE);
					/* clear interrupt related register end */

					/* reset vlc */ /* rtn_code can be ignored */
					(void)mcvx_vlc_reset(ip, MCVX_RESET_FORCED);

					/* reset ce */  /* rtn_code can be ignored */
					(void)mcvx_ce_reset(ip, MCVX_RESET_FORCED);

					/* store IP version, capability */
					ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_VCRL, &ip->ip_version,	MCVX_REGS_SINGLE);
					ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_CFG,	 &ip->ip_capability, MCVX_REGS_SINGLE);

					/* store stepping */
					ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_STEPPING,	&ip->vlc_stepping,	MCVX_REGS_SINGLE);
					ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_STEPPING,		&ip->ce_stepping,	MCVX_REGS_SINGLE);

					rtn_code = MCVX_NML_END;
				}
			}
		}
	}

	return rtn_code;
}

/**
 *
 * \brief			Get IP version
 *
 * \param[in]		ip						: IP context
 * \param[out]		ip_version				: IP version (from VP_VLC_VCRL register)
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_get_ip_version(
	const MCVX_IP_CTX_T	*ip,
	MCVX_U32			*ip_version)
{
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (ip_version == NULL)) {
		rtn_code	= MCVX_ERR_PARAM;
	} else {
		*ip_version = ip->ip_version;
		rtn_code	= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Get IP capability
 *
 * \param[in]		ip						: IP context
 * \param[out]		ip_capability			: IP Capability (value depends on IP)
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_get_ip_capability(
	const MCVX_IP_CTX_T	*ip,
	MCVX_U32			*ip_capability)
{
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (ip_capability == NULL)) {
		rtn_code	= MCVX_ERR_PARAM;
	} else {
		*ip_capability = ip->ip_capability;
		rtn_code	= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			VLC interrupt handler
 *
 * \param[in]		ip						: IP context
 * \param[in]		sw_timeout				: sw_timeout
 *			<br> MCVX_FALSE					: normal hw interrupt procedure
 *			<br> MCVX_TRUE					: forced sw timeout procedure
 *
 * \retval			void					: none
 *
******************************************************************************/
void mcvx_vlc_interrupt_handler(
	MCVX_IP_CTX_T	*ip,
	MCVX_U32		sw_timeout)
{
	MCVX_REG		irq_sta;
	MCVX_REG		edt_reset;

	irq_sta			= MCVX_NO_EVENT;
	edt_reset		= 0u;

	if (ip != NULL) {
		if (sw_timeout == MCVX_TRUE) {
			/*-------------------------------*/
			/* forced sw timeout procedure   */
			/*-------------------------------*/

			/* error termination */
			ip->vlc_hung_up	= MCVX_TRUE;

			ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_REQ_VLC_STOP, &irq_sta);

			/* VLC module reset */
			(void)mcvx_vlc_reset(ip, MCVX_RESET_FORCED);
		} else {
			/*-------------------------------*/
			/* normal hw interrupt procedure */
			/*-------------------------------*/

			if (ip->ip_arch == MCVX_ARCH_VCP3) {

				/* read IRQ status */
				ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

				/* cleaer EDT counter */
				ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_EDT, &edt_reset, MCVX_REGS_SINGLE);

				/*-----------------------------------------------------------------------------------------------------------------------*/
				/* Note that the VL (VLCS termination) bit and VED (VLCS system error detection) bit might be asserted at the same time. */
				/* In this case the VED flag can be ignored and software can treat as a normal termination.                              */
				/*-----------------------------------------------------------------------------------------------------------------------*/
				if ((irq_sta & MCVX_IRQ_VCP3_VLC_END) != MCVX_NO_EVENT) { /* VLC end */
					/* normal termination */
					ip->vlc_hung_up	= MCVX_FALSE;

					ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_REQ_VLC_STOP, &irq_sta);

				} else if ((irq_sta & (MCVX_IRQ_VCP3_VLC_EDT | MCVX_IRQ_VCP3_VLC_TO)) != MCVX_NO_EVENT) {	/* VLC error detect */
					/* error termination */
					ip->vlc_hung_up	= MCVX_TRUE;

					ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_REQ_VLC_STOP, &irq_sta);

					/* VLC module reset */
					(void)mcvx_vlc_reset(ip, MCVX_RESET_FORCED);
				} else {
					/* Nothing to do */
				}

				/* clear IRQ status */
				if (irq_sta != MCVX_NO_EVENT) {
					irq_sta	= ~irq_sta; /* 0:clear, 1:stay */

					/* clear interrupt status */
					ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

					/* dummy read */
					ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);
				}
			} else { /* MCVX_ARCH_BELZ */

				/* read IRQ status */
				ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

				/* cleaer EDT counter */
				ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_EDT, &edt_reset, MCVX_REGS_SINGLE);

				/*-----------------------------------------------------------------------------------------------------------------------*/
				/* Note that the VL (VLCS termination) bit and VED (VLCS system error detection) bit might be asserted at the same time. */
				/* In this case the VED flag can be ignored and software can treat as a normal termination.                              */
				/*-----------------------------------------------------------------------------------------------------------------------*/
				if ((irq_sta & MCVX_IRQ_BELZ_VLC_END) != MCVX_NO_EVENT) { /* VLC end */
					/* normal termination */
					ip->vlc_hung_up	= MCVX_FALSE;

					ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_REQ_VLC_STOP, &irq_sta);
				} else if ((irq_sta & (MCVX_IRQ_BELZ_VLC_EDT | MCVX_IRQ_BELZ_VLC_TO)) != MCVX_NO_EVENT) {	/* VLC error detect */
					/* error termination */
					ip->vlc_hung_up	= MCVX_TRUE;

					ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_REQ_VLC_STOP, &irq_sta);

					/* VLC module reset */
					(void)mcvx_vlc_reset(ip, MCVX_RESET_FORCED);
				} else {
					/* Nothing to do */
				}

				/* clear IRQ status */
				if (irq_sta != MCVX_NO_EVENT) {
					irq_sta	= ~irq_sta; /* 0:clear, 1:stay */

					/* clear interrupt status */
					ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

					/* dummy read */
					ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);
				}
			}
		}
	}
}

/**
 *
 * \brief			CE interrupt handler
 *
 * \param[in]		ip						: IP context
 * \param[in]		sw_timeout				: sw_timeout
 *			<br> MCVX_FALSE					: normal hw interrupt procedure
 *			<br> MCVX_TRUE					: forced sw timeout procedure
 *
 * \retval			void					: none
 *
******************************************************************************/
void mcvx_ce_interrupt_handler(
	MCVX_IP_CTX_T	*ip,
	MCVX_U32		sw_timeout)
{
	MCVX_REG		irq_sta;
	MCVX_REG		edt_reset;

	irq_sta			= MCVX_NO_EVENT;
	edt_reset		= 0u;

	if (ip != NULL) {
		if (sw_timeout == MCVX_TRUE) {
			/*-------------------------------*/
			/* forced sw timeout procedure   */
			/*-------------------------------*/

			/* error termination */
			ip->ce_hung_up	= MCVX_TRUE;

			ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_REQ_CE_STOP, &irq_sta);

			/* CE module reset */
			(void)mcvx_ce_reset(ip, MCVX_RESET_FORCED);
		} else {
			/*-------------------------------*/
			/* normal hw interrupt procedure */
			/*-------------------------------*/

			if (ip->ip_arch == MCVX_ARCH_VCP3) {

				/* read IRQ status */
				ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

				/* cleaer EDT counter */
				ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_EDT, &edt_reset, MCVX_REGS_SINGLE);

				/*-----------------------------------------------------------------------------------------------------------------------*/
				/* Note that the CE (CE termination) bit and CED (CE system error detection) bit might be asserted at the same time.     */
				/* In this case the CED flag can be ignored and software can treat as a normal termination.                              */
				/*-----------------------------------------------------------------------------------------------------------------------*/
				if ((irq_sta & MCVX_IRQ_VCP3_CE_END) != MCVX_NO_EVENT) {	/* CE end */
					/* normal termination */
					ip->ce_hung_up	= MCVX_FALSE;

					ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_REQ_CE_STOP, &irq_sta);
				} else if ((irq_sta & (MCVX_IRQ_VCP3_CE_EDT | MCVX_IRQ_VCP3_CE_TO)) != MCVX_NO_EVENT) {	/* CE error detect */
					/* error termination */
					ip->ce_hung_up	= MCVX_TRUE;

					ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_REQ_CE_STOP, &irq_sta);

					/* CE module reset */
					(void)mcvx_ce_reset(ip, MCVX_RESET_FORCED);
				} else {
					/* Nothing to do */
				}

				/* FCV end */
				if ((irq_sta & MCVX_IRQ_VCP3_FCV_END) != MCVX_NO_EVENT) {
					ip->udf_fcv_event(ip->fcv_handled_udp, MCVX_EVENT_REQ_FCV_STOP, &irq_sta);
				}

				/* clear IRQ status */
				if (irq_sta != MCVX_NO_EVENT) {

					irq_sta	= ~irq_sta; /* 0:clear, 1:stay */

					/* clear interrupt status */
					ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

					/* dummy read */
					ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);
				}

			} else { /* MCVX_ARCH_BELZ */

				/* read IRQ status */
				ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

				/* cleaer EDT counter */
				ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_EDT, &edt_reset, MCVX_REGS_SINGLE);

				/*-----------------------------------------------------------------------------------------------------------------------*/
				/* Note that the CE (CE termination) bit and CED (CE system error detection) bit might be asserted at the same time.     */
				/* In this case the CED flag can be ignored and software can treat as a normal termination.                              */
				/*-----------------------------------------------------------------------------------------------------------------------*/
				if ((irq_sta & MCVX_IRQ_BELZ_CE_END) != MCVX_NO_EVENT) {	/* CE end */
					/* normal termination */
					ip->ce_hung_up	= MCVX_FALSE;

					ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_REQ_CE_STOP, &irq_sta);
				} else if ((irq_sta & (MCVX_IRQ_BELZ_CE_EDT | MCVX_IRQ_BELZ_CE_TO)) != MCVX_NO_EVENT) {	/* CE error detect */
					/* error termination */
					ip->ce_hung_up	= MCVX_TRUE;

					ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_REQ_CE_STOP, &irq_sta);

					/* CE module reset */
					(void)mcvx_ce_reset(ip, MCVX_RESET_FORCED);
				} else {
					/* Nothing to do */
				}

				/* clear IRQ status */
				if (irq_sta != MCVX_NO_EVENT) {

					irq_sta	= ~irq_sta; /* 0:clear, 1:stay */

					/* clear interrupt status */
					ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);

					/* dummy read */
					ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_IRQ_STA, &irq_sta, MCVX_REGS_SINGLE);
				}
			}
		}
	}
}

/**
 *
 * \brief			Reset VLC
 *
 * \param[in]		ip						: IP context
 * \param[in]		reset_mode				: reset mode
 *			<br> MCVX_RESET_FORCED			: forced reset
 *			<br> MCVX_RESET_NML				: normal reset
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_vlc_reset(
	MCVX_IP_CTX_T	*ip,
	MCVX_U32		reset_mode)
{
	MCVX_REG		reg_data;
	MCVX_RC			rtn_code;

	if (ip == NULL) {
		rtn_code	= MCVX_ERR_PARAM;
	} else {
		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			/* reset */
			reg_data	= (reset_mode == MCVX_RESET_FORCED) ? MCVX_CMD_VCP3_VLC_RESET_MODULE : MCVX_CMD_VCP3_VLC_RESET_SOFT;
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_CMD, &reg_data, MCVX_REGS_SINGLE);

			/* dummy read */
			ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_STATUS, &reg_data, MCVX_REGS_SINGLE);

			/*----------------------*/
			/* VP_VLC_CLK_STOP      */
			/*----------------------*/
			reg_data	= 0u;
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_CLK_STOP, &reg_data, MCVX_REGS_SINGLE);
		} else {
			/* reset */
			reg_data	= (reset_mode == MCVX_RESET_FORCED) ? MCVX_CMD_BELZ_VLC_RESET_MODULE : MCVX_CMD_BELZ_VLC_RESET_SOFT;
			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_CMD, &reg_data, MCVX_REGS_SINGLE);

			/* dummy read */
			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_STATUS, &reg_data, MCVX_REGS_SINGLE);

			/*----------------------*/
			/* VP_VLC_CLK_STOP      */
			/*----------------------*/
			reg_data	= 0u;
			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_CLK_STOP, &reg_data, MCVX_REGS_SINGLE);
		}
		rtn_code	= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Reset CE
 *
 * \param[in]		ip						: IP context
 * \param[in]		reset_mode				: reset mode
 *			<br> MCVX_RESET_FORCED			: forced reset
 *			<br> MCVX_RESET_NML				: normal reset
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_ce_reset(
	MCVX_IP_CTX_T	*ip,
	MCVX_U32		reset_mode)
{
	MCVX_REG		reg_data;
	MCVX_RC			rtn_code;

	if (ip == NULL) {
		rtn_code	= MCVX_ERR_PARAM;
	} else {
		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			/* reset */
			reg_data	= (reset_mode == MCVX_RESET_FORCED) ? MCVX_CMD_VCP3_CE_RESET_MODULE : MCVX_CMD_VCP3_CE_RESET_SOFT;
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CMD, &reg_data, MCVX_REGS_SINGLE);

			/* dummy read */
			ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_STATUS, &reg_data, MCVX_REGS_SINGLE);

			/*----------------------*/
			/* VP_CE_CLK_STOP       */
			/*----------------------*/
			reg_data	= 0u;
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CLK_STOP, &reg_data, MCVX_REGS_SINGLE);
		} else {
			/* reset */
			reg_data	= (reset_mode == MCVX_RESET_FORCED) ? MCVX_CMD_BELZ_CE_RESET_MODULE : MCVX_CMD_BELZ_CE_RESET_SOFT;
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_CMD, &reg_data, MCVX_REGS_SINGLE);

			/* dummy read */
			ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_STATUS, &reg_data, MCVX_REGS_SINGLE);

			/*----------------------*/
			/* VP_CE_CLK_STOP       */
			/*----------------------*/
			reg_data	= 0u;
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_CLK_STOP, &reg_data, MCVX_REGS_SINGLE);
		}
		rtn_code	= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Start VLC
 *
 * \param[in]		ip						: IP context
 * \param[in]		udp						: user define pointer
 * \param[in]		udf_vlc_event			: VLC event callback function
 * \param[in]		vlc_exe					: VLC execute parameter
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_vlc_start(
	MCVX_IP_CTX_T			*ip,
	void					*udp,
	MCVX_UDF_VLC_EVENT_T	udf_vlc_event,
	MCVX_VLC_EXE_T			*vlc_exe)
{
	MCVX_REG		reg_data_table[MCVX_REG_TABLE_SIZE];
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (udf_vlc_event == NULL) || (vlc_exe == NULL)) {
		rtn_code			= MCVX_ERR_PARAM;
	} else if (vlc_exe->ip_arch != ip->ip_arch) {
		rtn_code			= MCVX_ERR_PARAM;
	} else {
		ip->vlc_hung_up		= MCVX_FALSE;
		ip->vlc_handled_udp	= udp;
		ip->udf_vlc_event	= udf_vlc_event;

		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			reg_data_table[0]	= vlc_exe->vp_vlc_list_init;
			reg_data_table[1]	= vlc_exe->vp_vlc_list_en;
			reg_data_table[2]	= vlc_exe->vp_vlc_list_lden;
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_LIST_INIT,	reg_data_table, 3u);

			reg_data_table[0]	= vlc_exe->vp_vlc_pbah;
			reg_data_table[1]	= vlc_exe->vp_vlc_pbal;
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_PBAH,		reg_data_table, 2u);

			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_EDT,		&vlc_exe->vp_vlc_edt,		MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_IRQ_ENB,	&vlc_exe->vp_vlc_irq_enb,	MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_CLK_STOP,	&vlc_exe->vp_vlc_clk_stop,	MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_CTRL,		&vlc_exe->vp_vlc_ctrl,		MCVX_REGS_SINGLE);

			reg_data_table[0]	= 0x00000001u;	/* bit0 */ /* #76073 */
			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_DCTRL,		reg_data_table,		MCVX_REGS_SINGLE);

			/* call back */
			ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_VLC_START, NULL);

			/* update HW-VLC-state */
			ip->hw_vlc_state	= MCVX_STATE_VLC_RUNNING;

			ip->udf_reg_write(&ip->REG_VCP3_VLC->VP_VLC_CMD, &vlc_exe->vp_vlc_cmd, MCVX_REGS_SINGLE);
		} else {
			reg_data_table[0]	= vlc_exe->vp_vlc_pbah;
			reg_data_table[1]	= vlc_exe->vp_vlc_pbal;
			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_PBAH,		reg_data_table, 2u);

			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_EDT,		&vlc_exe->vp_vlc_edt,		MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_IRQ_ENB,	&vlc_exe->vp_vlc_irq_enb,	MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_CLK_STOP,	&vlc_exe->vp_vlc_clk_stop,	MCVX_REGS_SINGLE);

			reg_data_table[0]	= 0x00000000u;	/* bit16 */ /* #76073 */ /* set 0 for VDPB's VLC */ /* #79073 */
			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_DCTRL,		reg_data_table,		MCVX_REGS_SINGLE);

			/* call back */
			ip->udf_vlc_event(ip->vlc_handled_udp, MCVX_EVENT_VLC_START, NULL);

			/* update HW-VLC-state */
			ip->hw_vlc_state	= MCVX_STATE_VLC_RUNNING;

			ip->udf_reg_write(&ip->REG_BELZ_VLC->VP_VLC_CMD, &vlc_exe->vp_vlc_cmd, MCVX_REGS_SINGLE);
		}

		rtn_code			= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Get VLC result
 *
 * \param[in]		ip						: IP context
 * \param[out]		vlc_res					: VLC-result information
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_vlc_stop(
	MCVX_IP_CTX_T	*ip,
	MCVX_VLC_RES_T	*vlc_res)
{
	MCVX_REG		reg_data_table[MCVX_REG_TABLE_SIZE];
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (vlc_res == NULL)) {
		rtn_code	= MCVX_ERR_PARAM;
	} else {
		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			/* read register */
			ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_TB, reg_data_table, MCVX_REGS_RES_VLC);

			vlc_res->pic_bytes			= reg_data_table[0];
			/* reg_data_table[1] VP_VLC_ERR0H (ignored) */
			/* reg_data_table[2] VP_VLC_ERR0L (ignored) */
			vlc_res->is_byte			= reg_data_table[3];
			/* reg_data_table[4] resereved */
			vlc_res->codec_info			= reg_data_table[5];

			/* highest error info */
			/* VP_VLC_ERR1H */
			vlc_res->error_block		= MCVX_M_02BIT & (reg_data_table[6] >> 29);
			if (ip->vlc_hung_up == MCVX_FALSE) {
				vlc_res->error_level	= MCVX_M_02BIT & (reg_data_table[6] >> 27);
			} else {
				vlc_res->error_level	= MCVX_VLC_ERR_LEVEL_HUNGUP;
			}
			vlc_res->error_pos_x		= MCVX_M_12BIT & (reg_data_table[6] >> 12);
			vlc_res->error_pos_y		= MCVX_M_12BIT & (reg_data_table[6] >>  0);
			/* VP_VLC_ERR1L */
			vlc_res->error_code			= MCVX_M_08BIT & (reg_data_table[7] >> 24);

			ip->udf_reg_read(&ip->REG_VCP3_VLC->VP_VLC_CNT, reg_data_table, 1);
			vlc_res->vlc_cycle			= reg_data_table[0];
		} else {
			/* highest error info */
			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_ERR1H, reg_data_table, MCVX_REGS_SINGLE);
			vlc_res->error_block		= MCVX_M_02BIT & (reg_data_table[0] >> 16);
			if (ip->vlc_hung_up == MCVX_FALSE) {
				vlc_res->error_level	= MCVX_M_02BIT & (reg_data_table[0] >> 24);
			} else {
				vlc_res->error_level	= MCVX_VLC_ERR_LEVEL_HUNGUP;
			}
			vlc_res->error_code			= MCVX_M_08BIT & (reg_data_table[0] >>  0);

			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_ERR1L, reg_data_table, MCVX_REGS_SINGLE);
			vlc_res->error_pos_x		= MCVX_M_16BIT & (reg_data_table[0] >> 16);
			vlc_res->error_pos_y		= MCVX_M_16BIT & (reg_data_table[0] >>  0);

			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_TB, reg_data_table, MCVX_REGS_SINGLE);
			vlc_res->pic_bytes			= reg_data_table[0];

			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_TB_IS0, reg_data_table, MCVX_REGS_SINGLE);
			vlc_res->is_byte			= reg_data_table[0];

			ip->udf_reg_read(&ip->REG_BELZ_VLC->VP_VLC_CYCLE_CNT, reg_data_table, MCVX_REGS_SINGLE);
			vlc_res->vlc_cycle			= reg_data_table[0];
		}

		/* update HW-VLC-state */
		ip->hw_vlc_state			= MCVX_STATE_VLC_IDLE;
		ip->vlc_hung_up				= MCVX_FALSE; /* clear */

		rtn_code					= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Start CE
 *
 * \param[in]		ip						: IP context
 * \param[in]		udp						: user define pointer
 * \param[in]		udf_ce_event			: CE event callback function
 * \param[in]		ce_exe					: CE execute parameter
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_ce_start(
	MCVX_IP_CTX_T			*ip,
	void					*udp,
	MCVX_UDF_CE_EVENT_T		udf_ce_event,
	MCVX_CE_EXE_T			*ce_exe)
{
	MCVX_REG		reg_data_table[MCVX_REG_TABLE_SIZE];
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (udf_ce_event == NULL) || (ce_exe == NULL)) {
		rtn_code			= MCVX_ERR_PARAM;
	} else if (ce_exe->ip_arch != ip->ip_arch) {
		rtn_code			= MCVX_ERR_PARAM;
	} else {
		ip->ce_hung_up		= MCVX_FALSE;
		ip->ce_handled_udp	= udp;
		ip->udf_ce_event	= udf_ce_event;

		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			reg_data_table[0]	= ce_exe->vp_ce_list_init;
			reg_data_table[1]	= ce_exe->vp_ce_list_en;
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_LIST_INIT,	reg_data_table, 2u);

			reg_data_table[0]	= ce_exe->vp_ce_pbah;
			reg_data_table[1]	= ce_exe->vp_ce_pbal;
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_PBAH,			reg_data_table, 2u);

			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_EDT,			&ce_exe->vp_ce_edt,			MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_IRQ_ENB,		&ce_exe->vp_ce_irq_enb,		MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CLK_STOP,		&ce_exe->vp_ce_clk_stop,	MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CTRL,			&ce_exe->vp_ce_ctrl,		MCVX_REGS_SINGLE);

			reg_data_table[0]	= 0x00000001u;	/* bit0 */ /* #76073 */
			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_DCTRL,		reg_data_table,		MCVX_REGS_SINGLE);

			/*----------------------*/
			/* VP_CE_CMD            */
			/*----------------------*/
			/* call back */
			ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_CE_START, NULL);

			/* update HW-CE-state */
			ip->hw_ce_state		= MCVX_STATE_CE_RUNNING;

			ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CMD, &ce_exe->vp_ce_cmd, MCVX_REGS_SINGLE);
		} else {
			reg_data_table[0]	= ce_exe->vp_ce_pbah;
			reg_data_table[1]	= ce_exe->vp_ce_pbal;
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_PBA0H,		reg_data_table, 2u);

			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_EDT,			&ce_exe->vp_ce_edt,			MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_IRQ_ENB,		&ce_exe->vp_ce_irq_enb,		MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_CLK_STOP,		&ce_exe->vp_ce_clk_stop,	MCVX_REGS_SINGLE);
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_CTRL,			&ce_exe->vp_ce_ctrl,		MCVX_REGS_SINGLE);

			reg_data_table[0]	= 0x00010000u;	/* bit16 */ /* #76073 */
			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_DCTRL,		reg_data_table,		MCVX_REGS_SINGLE);

			/*----------------------*/
			/* VP_CE_CMD            */
			/*----------------------*/
			/* call back */
			ip->udf_ce_event(ip->ce_handled_udp, MCVX_EVENT_CE_START, NULL);

			/* update HW-CE-state */
			ip->hw_ce_state		= MCVX_STATE_CE_RUNNING;

			ip->udf_reg_write(&ip->REG_BELZ_CE->VP_CE_CMD, &ce_exe->vp_ce_cmd, MCVX_REGS_SINGLE);
		}

		rtn_code			= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Get CE result
 *
 * \param[in]		ip						: IP context
 * \param[out]		ce_res					: CE-result information
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_ce_stop(
	MCVX_IP_CTX_T	*ip,
	MCVX_CE_RES_T	*ce_res)
{
	MCVX_REG		reg_data_table[MCVX_REG_TABLE_SIZE];
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (ce_res == NULL)) {
		rtn_code					= MCVX_ERR_PARAM;
	} else {
		if (ip->ip_arch == MCVX_ARCH_VCP3) {
			/* read register */
			ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_REF_LOG, reg_data_table, MCVX_REGS_SINGLE);
			ce_res->ref_baa_log			= reg_data_table[0];

			/* read register */
			ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_ERRH, reg_data_table, 5u);
			if (ip->ce_hung_up == MCVX_FALSE) {
				/* mask this field with 7bit, not 8bit */
				ce_res->error_code		= MCVX_M_07BIT & (reg_data_table[0] >> 24);
			} else {
				ce_res->error_code		= MCVX_CE_ERR_CODE_HUNGUP;
			}
			ce_res->error_pos_y			= MCVX_M_12BIT & (reg_data_table[0] >> 12);
			ce_res->error_pos_x			= MCVX_M_12BIT & (reg_data_table[0] >>  0);
			ce_res->is_byte				= reg_data_table[2];
			/* reg_data_table[3] reserved  */
			ce_res->intra_mbs			= reg_data_table[4];

			/* read register */
			ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_QP_SUM, reg_data_table, 9u);
			ce_res->sum_of_qp			= MCVX_M_22BIT & (reg_data_table[0] >>  0);
			/* reg_data_table[1] reserved  */
			ce_res->sum_of_act			= MCVX_M_24BIT & (reg_data_table[2] >>  0);
			ce_res->act_max				= MCVX_M_10BIT & (reg_data_table[3] >> 16);
			ce_res->act_min				= MCVX_M_10BIT & (reg_data_table[3] >>  0);
			/* reg_data_table[4] reserved  */
			/* reg_data_table[5] reserved  */
			/* reg_data_table[6] reserved  */
			ce_res->sum_of_min_sad256	= reg_data_table[6];
			ce_res->sum_of_intra_sad256	= reg_data_table[7];
			ce_res->sum_of_inter_sad256	= reg_data_table[8];

			ip->udf_reg_read(&ip->REG_VCP3_CE->VP_CE_CNT, reg_data_table, MCVX_REGS_SINGLE);
			ce_res->ce_cycle			= reg_data_table[0];
		} else {
			/* read register */
			ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_REF_LOG, reg_data_table, MCVX_REGS_SINGLE);
			ce_res->ref_baa_log			= reg_data_table[0];

			ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_TB_IS0, reg_data_table, MCVX_REGS_SINGLE);
			ce_res->is_byte				= reg_data_table[0];

			if (ip->ce_hung_up == MCVX_FALSE) {
				ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_ERROR, reg_data_table, MCVX_REGS_SINGLE);
				ce_res->error_code		= reg_data_table[0];
			} else {
				ce_res->error_code		= MCVX_CE_ERR_CODE_HUNGUP;
			}

			ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_EPOS, reg_data_table, MCVX_REGS_SINGLE);
			ce_res->error_pos_y			= MCVX_M_16BIT & (reg_data_table[0] >> 16);
			ce_res->error_pos_x			= MCVX_M_16BIT & (reg_data_table[0] >>  0);

			ip->udf_reg_read(&ip->REG_BELZ_CE->VP_CE_CYCLE_CNT, reg_data_table, MCVX_REGS_SINGLE);
			ce_res->ce_cycle			= reg_data_table[0];
		}

		/* override error pos */
		if (ce_res->error_code == 0u) {
			ce_res->error_pos_y			= 0u;
			ce_res->error_pos_x			= 0u;
		}

		/* update HW-CE-state */
		ip->hw_ce_state				= MCVX_STATE_CE_IDLE;
		ip->ce_hung_up				= MCVX_FALSE; /* clear */

		rtn_code					= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Start FCV(in CE)
 *
 * \param[in]		ip						: IP context
 * \param[in]		udp						: user define pointer
 * \param[in]		udf_fcv_event			: FCV event callback function
 * \param[in]		fcv_exe					: FCV execute parameter
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_fcv_start(
	MCVX_IP_CTX_T			*ip,
	void					*udp,
	MCVX_UDF_FCV_EVENT_T	udf_fcv_event,
	MCVX_FCV_EXE_T			*fcv_exe)
{
	MCVX_RC			rtn_code;
	MCVX_REG		reg_data;

	if ((ip == NULL) || (udf_fcv_event == NULL) || (fcv_exe == NULL)) {
		rtn_code			= MCVX_ERR_PARAM;
	} else if (fcv_exe->ip_arch != ip->ip_arch) {
		rtn_code			= MCVX_ERR_PARAM;
	} else {
		ip->fcv_handled_udp	= udp;
		ip->udf_fcv_event	= udf_fcv_event;

		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_LIST_INIT,	&fcv_exe->vp_ce_list_init,		MCVX_REGS_SINGLE);
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_LIST_EN,		&fcv_exe->vp_ce_list_en,		MCVX_REGS_SINGLE);
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_PBAH,			&fcv_exe->vp_ce_pbah,			MCVX_REGS_SINGLE);
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_PBAL,			&fcv_exe->vp_ce_pbal,			MCVX_REGS_SINGLE);
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_IRQ_ENB,		&fcv_exe->vp_ce_irq_enb,		MCVX_REGS_SINGLE);
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CTRL,			&fcv_exe->vp_ce_ctrl,			MCVX_REGS_SINGLE);
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_EDT,			&fcv_exe->vp_ce_edt,			MCVX_REGS_SINGLE);

		reg_data	= 0x00000001u;	/* #76073 */
		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_DCTRL,		&reg_data,		MCVX_REGS_SINGLE);

		/*----------------------*/
		/* VP_CE_CMD            */
		/*----------------------*/
		/* call back */
		ip->udf_fcv_event(ip->fcv_handled_udp, MCVX_EVENT_FCV_START, NULL);

		/* update HW-FCV-state */
		ip->hw_fcv_state	= MCVX_STATE_FCV_RUNNING;

		ip->udf_reg_write(&ip->REG_VCP3_CE->VP_CE_CMD, &fcv_exe->vp_ce_cmd, MCVX_REGS_SINGLE);

		rtn_code			= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Get FCV result
 *
 * \param[in]		ip						: IP context
 * \param[out]		fcv_res					: FCV-result information
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_fcv_stop(
	MCVX_IP_CTX_T	*ip,
	MCVX_FCV_RES_T	*fcv_res)
{
	MCVX_RC			rtn_code;

	if ((ip == NULL) || (fcv_res == NULL)) {
		rtn_code			= MCVX_ERR_PARAM;
	} else {
		/* reserved (there is no fcv result information) */
		fcv_res->reserved	= 0u;

		/* update HW-FCV-state */
		ip->hw_fcv_state	= MCVX_STATE_FCV_IDLE;

		rtn_code			= MCVX_NML_END;
	}

	return rtn_code;
}

/**
 *
 * \brief			Initialize FCPC
 *
 * \param[in]		config					: FCPC config information
 * \param[out]		fcpc					: FCPC context
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_fcpc_init(
	const MCVX_FCPC_CONFIG_T	*config,
	MCVX_FCPC_CTX_T				*fcpc)
{
	MCVX_RC				rtn_code;
	MCVX_REG			reg_data;
	struct hwspinlock *hwlock = NULL;
	int lTimes = 0;
	volatile unsigned int *addr = NULL;
	volatile unsigned int lFcpcInited = 0;

	/* remap STBCHR4 reg*/
	addr = (volatile unsigned int*)ioremap(0xE6160120, 4);
	if (!addr) {
		rtn_code = MCVX_ERR_WORK;
		return rtn_code;
	}

	lFcpcInited = ioread32(addr) & (0x1 << 1);
	if(lFcpcInited != 0) {
		/* FCPCS has been initialized*/
		rtn_code = MCVX_NML_END;
	} else {
		hwlock = hwspin_lock_request_specific(63);
		if(NULL != hwlock){

			while((0 != hwspin_trylock(hwlock)) && (lTimes < 2)) {
				msleep(1u);
				lTimes++;
			}

			if(lTimes < 2) {
				/* FCPCS init */
				/* check arguments */
				if (config == NULL) {
					rtn_code = MCVX_ERR_PARAM;
				} else if (fcpc == NULL) {
					rtn_code = MCVX_ERR_PARAM;
				} else if ((config->udf_reg_read == NULL) || (config->udf_reg_write == NULL)) {
					rtn_code = MCVX_ERR_PARAM;
				} else if (config->fcpc_base_addr == NULL) {
					rtn_code = MCVX_ERR_PARAM;
				} else if ((config->fcpc_arch != MCVX_FCPC_S) && (config->fcpc_arch != MCVX_FCPC_I)) {
					rtn_code = MCVX_ERR_PARAM;
				} else {
					fcpc->fcpc_arch		= config->fcpc_arch;
					fcpc->shared_mode	= config->shared_mode;
					fcpc->udf_reg_read	= config->udf_reg_read;
					fcpc->udf_reg_write	= config->udf_reg_write;
					fcpc->REG_FCPC		= (MCVX_REG_FCPC_T *)config->fcpc_base_addr;

					/* get revision */
					reg_data	= 0u;
					fcpc->udf_reg_read(&fcpc->REG_FCPC->FCP_VCR, &reg_data, MCVX_REGS_SINGLE);
					fcpc->revision = reg_data & MCVX_M_08BIT;
					/* revision 1 : H3  ES1.x */
					/* revision 2 : M3W ES1   */
					/* revision 3 : H3  ES2   */

					(void)mcvx_fcpc_reset(fcpc, MCVX_RESET_FORCED);

					iowrite32(lFcpcInited | (0x1<<1), addr);

					rtn_code = MCVX_NML_END;
				}

				hwspin_unlock(hwlock);
			} else {
				pr_err("can't get the lock.\n");
			}

			hwspin_lock_free(hwlock);
		}
	}

	iounmap(addr);
	addr = NULL;

	return rtn_code;
}

/**
 *
 * \brief			Reset FCPC
 *
 * \param[in]		fcpc					: FCPC context
 * \param[in]		reset_mode				: reset mode
 *			<br> MCVX_RESET_FORCED			: forced reset
 *			<br> MCVX_RESET_NML				: normal reset
 *
 * \retval			MCVX_NML_END			: normal end
 * \retval			MCVX_ERR_PARAM			: parameter error
 *
******************************************************************************/
MCVX_RC mcvx_fcpc_reset(
	MCVX_FCPC_CTX_T	*fcpc,
	MCVX_U32		reset_mode)
{
	MCVX_REG		reg_data;
	MCVX_RC			rtn_code;

	if (fcpc == NULL) {
		rtn_code	= MCVX_ERR_PARAM;
	} else {
		/* reset */
		reg_data	= (reset_mode == MCVX_RESET_FORCED) ? MCVX_FCPC_RESET_MODULE : MCVX_FCPC_RESET_SOFT;
		fcpc->udf_reg_write(&fcpc->REG_FCPC->FCP_RST, &reg_data, MCVX_REGS_SINGLE);

		/* set id(cache_size) */
		reg_data	= fcpc->fcpc_arch;
		fcpc->udf_reg_write(&fcpc->REG_FCPC->FCP_CFG0, &reg_data, MCVX_REGS_SINGLE);

		/* set shared mode (M3W only) */
		if (fcpc->revision == 2u) { /* M3W */
			if (fcpc->shared_mode == MCVX_FCPC_SM_DEFAULT) {
				/* VDPB/VCPLF <-> FCPCS <-> AXI */
				/* (iVDP1C)   <-> FCPCS <-> AXI */
				reg_data	= 0x00000000u;
			} else { /* MCVX_FCPC_SM_1 */
				/* VDPB  <-> FCPCS <-> AXI */
				/* VCPLF <-> FCPCI <-> AXI */
				/* (iVDP1C) disabled */

				/* bit0: OPMDSEL, bit4 SELIP */
				reg_data	= (fcpc->fcpc_arch == MCVX_FCPC_S) ? 0x00000001u : 0x00000010u;
			}
		} else {
			reg_data	= 0x00000000u;
		}
		fcpc->udf_reg_write(&fcpc->REG_FCPC->FCP_CFG1, &reg_data, MCVX_REGS_SINGLE);

		/* init routine paremeter endian */
#ifdef _BIG
		reg_data = MCVX_FCPC_IRP_BIG;
#else
		reg_data = MCVX_FCPC_IRP_LIT;
#endif
		fcpc->udf_reg_write(&fcpc->REG_FCPC->FCP_INIT_ENDIAN, &reg_data, MCVX_REGS_SINGLE);

		rtn_code = MCVX_NML_END;
	}

	return rtn_code;
}
