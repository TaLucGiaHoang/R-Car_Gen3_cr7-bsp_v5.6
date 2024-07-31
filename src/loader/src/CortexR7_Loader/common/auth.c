/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "mmio.h"
#include "rcar_def.h"
#include "debug.h"
#include "auth.h"

/* byte address set : must 4byte alignment */
#define RCAR_CERT_INFO_SIZE_OFFSET	(0x264U)
#define RCAR_CERT_INFO_DST_OFFSET	(0x154U)
#define RCAR_CERT_INFO_SIZE_OFFSET1	(0x364U)
#define RCAR_CERT_INFO_DST_OFFSET1	(0x1D4U)
#define RCAR_CERT_INFO_SIZE_OFFSET2	(0x464U)
#define RCAR_CERT_INFO_DST_OFFSET2	(0x254U)

#define RCAR_BOOT_KEY_CERT_NEW	(0xE6300F00U)
#define	RCAR_CERT_MAGIC_NUM	(0xE291F358U)

#define FLASH_SEC_BOOT_CERT_ADDR	(0x00000800U)
#define FLASH_SEC_BOOT_CERT_ADDR2	(0x00000B00U)
#define FLASH_SEC_BOOT_CERT_SIZE	(0x300U)
#define FLASH_SEC_BOOT_CERT_SIZE2	(0x1100U)

static uint32_t magicNumber;

void get_cert_info(uint32_t *boot_cert_addr, uint32_t *size)
{
	magicNumber = mmio_read_32((uintptr_t)RCAR_BOOT_KEY_CERT_NEW);

	if (magicNumber == RCAR_CERT_MAGIC_NUM) {
		*boot_cert_addr = FLASH_SEC_BOOT_CERT_ADDR2;
		*size = FLASH_SEC_BOOT_CERT_SIZE2;
	} else {
		*boot_cert_addr = FLASH_SEC_BOOT_CERT_ADDR;
		*size = FLASH_SEC_BOOT_CERT_SIZE;
	}
}

void get_info_from_cert(uint32_t cert_addr, uint32_t *size,
	uintptr_t *dest_addr)
{
	uint32_t	val;
	uint32_t	certInfo1;
	uintptr_t	pSize;
	uintptr_t	pDestL;

	val = mmio_read_32((uintptr_t)cert_addr + 0xCU);
	certInfo1 = (val >> 21U) & 0x3U;

	if (magicNumber == RCAR_CERT_MAGIC_NUM) {
		if (certInfo1 == 0x2U) {
			pSize = cert_addr + RCAR_CERT_INFO_SIZE_OFFSET2;
			*size = mmio_read_32(pSize) * 4U;
			pDestL = cert_addr + RCAR_CERT_INFO_DST_OFFSET2;
			*dest_addr = (uintptr_t)mmio_read_32(pDestL);
		} else if (certInfo1 == 0x1U) {
			pSize = cert_addr + RCAR_CERT_INFO_SIZE_OFFSET1;
			*size = mmio_read_32(pSize) * 4U;
			pDestL = cert_addr + RCAR_CERT_INFO_DST_OFFSET1;
			*dest_addr = (uintptr_t)mmio_read_32(pDestL);
		} else if (certInfo1 == 0x0U) {
			pSize = cert_addr + RCAR_CERT_INFO_SIZE_OFFSET;
			*size = mmio_read_32(pSize) * 4U;
			pDestL = cert_addr + RCAR_CERT_INFO_DST_OFFSET;
			*dest_addr = (uintptr_t)mmio_read_32(pDestL);
		} else /* if (0x3U == certInfo2) */{
			ERROR("Cert is invalid.\n");
			*size = 0x0U;
			*dest_addr = 0x0U;
		}
	} else {
		pSize = cert_addr + RCAR_CERT_INFO_SIZE_OFFSET;
		*size = mmio_read_32(pSize) * 4U;
		pDestL = cert_addr + RCAR_CERT_INFO_DST_OFFSET;
		*dest_addr = (uintptr_t)mmio_read_32(pDestL);
	}
}
