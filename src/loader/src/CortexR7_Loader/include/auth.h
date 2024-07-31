/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */


#ifndef AUTH_H__
#define AUTH_H__

void get_cert_info(uint32_t *boot_cert_addr, uint32_t *size);
void get_info_from_cert(uint32_t cert_addr, uint32_t *size,
	uintptr_t *dest_addr);

#endif /* AUTH_H__ */
