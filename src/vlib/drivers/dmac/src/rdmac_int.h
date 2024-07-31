/*************************************************************************************************************
* dmac_int_h
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef _RDMAC_INT_H_
#define _RDMAC_INT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "drv.h"
#include "rcar-xos/osal/r_osal.h"

#define RTDMAC_MQ_TIMEOUT  3000
#define AUDMAC_MQ_TIMEOUT  3000

void rDmacInterruptHandler(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg);

#ifdef __cplusplus
}
#endif

#endif  /* _RDMAC_INT_H_ */
