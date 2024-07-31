/*
 * Copyright (c) 2015-2022, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "string.h"
#include "tcm_loader_main.h"
#include "debug.h"
#include "mmio.h"
#include "ecc_init.h"
#include "rpc_driver.h"
#include "dma_driver.h"
#include "rom_api.h"
#include "auth.h"
#include "rcar_def.h"
#include "scif.h"
#include "swdt.h"
#include "protection_setting.h"
#include "lifec_init.h"
#include "rcar_pwrc.h"

/*****************************************************************************
 *	Definitions
 *****************************************************************************/
#define RTOS_MAX_SIZE			(1024U * 1024U *24U)
#define MAIN_LOADER_MAX_SIZE		(170U * 1024U)

#define FLASH_BASE			(0x08000000U)
#define FLASH_SIZE			(0x04000000U)
#define FLASH_RTOS_CERT_ADDR		(FLASH_BASE + 0x000C0000U)
#define FLASH_CA5X_CERT_ADDR		(FLASH_BASE + 0x000C0800U)

#if RCAR_CA5x_OS == OS_LINUX
#define FLASH_RTOS_IMAGE_ADDR		(FLASH_BASE + 0x00740000U)
#elif RCAR_CA5x_OS == OS_ANDROID
#define FLASH_RTOS_IMAGE_ADDR		(FLASH_BASE + 0x00780000U)
#endif

#define FLASH_CA5X_IMAGE_ADDR		(FLASH_BASE + 0x00140000U)
#define FLASH_CERT_SIZE			(0x480U)

#define SYSRAM_BASE			(0xE6300000U)
#if RCAR_LSI == RCAR_E3
#define SYSRAM_END			(0xE6320000U)	/* E3 128KB */
#else  /* RCAR_LSI == RCAR_E3 */
#define SYSRAM_END			(0xE6360000U)	/* H3/M3/M3N 384KB */
#endif /* RCAR_LSI == RCAR_E3 */
#define DRAM_BASE			(0x40000000U)
#define DRAM_END			(0xC0000000U)
#define SEC_BOOT_CERT_ADDR(a)		(SYSRAM_BASE + (a) + 0x400U)
#define RTOS_CERT_ADDR			(0xE6302000U)
#define IPL_CERT_ADDR			(0xE6302000U)

#define ADDR_RANGE_TYPE0		(0U)
#define ADDR_RANGE_TYPE1		(1U)

#define TARGET_MEM_DRAM			(0U)
#define TARGET_MEM_SYSRAM		(1U)

/********************* R-Car Seriese, 3rd Generation_MFI *********************/
#define	MFISOFTMDR		(0xE6260600U)	/* SOFTMD register */
#define SOFTMD_BIT0		(0x00000001U)

/********************* R-Car Seriese, 3rd Generation_MRST ********************/
#define	RST_MODEMR		(0xE6160060U)	/* Mode Monitor Register */
#define RST_MODEMR_MD5		(0x00000020U)
#define MD5_SHIFT_TO_BIT0	(5U)

typedef uint32_t (*SECURE_BOOT_API)(uint32_t a,
				     uint32_t b, ROM_FuncReadFlash c);


/*****************************************************************************
 *	Unions
 *****************************************************************************/


/*****************************************************************************
 *	Structs
 *****************************************************************************/

struct address_range {
	uint32_t	topadd;
	uint32_t	endadd;
};



/*****************************************************************************
 *	Globals
 *****************************************************************************/



/*****************************************************************************
 *	Macros
 *****************************************************************************/


/*****************************************************************************
 *	Prototypes
 *****************************************************************************/
static void check_load_area(uint32_t dst, uint32_t src,
			    uint32_t len, uint8_t load_check);

uint32_t tcm_loader_main(void)
{
#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
	uint32_t ca5x_load_addr;
#endif /* RCAR_KICK_MAIN_CPU */
	uint32_t rtos_load_addr;
	uint32_t boot_cert_addr;
	uint32_t size;
	uint32_t ret;
	SECURE_BOOT_API	sbrom_SecureBootAPI;


	get_cert_info(&boot_cert_addr, &size);

/*****************************************************************************
 *	Initialize Hardware
 *****************************************************************************/
	ecc_init();			/* ECC setting    */
	initRPC();			/* Initialize RPC */
	initDMA();			/* Initialize DMA */

/*****************************************************************************
 *	LCS judgement for secure boot
 *****************************************************************************/
#if RCAR_SECURE_BOOT == 1
{
	uint32_t lcs;
	uint32_t md     = (mmio_read_32(RST_MODEMR)
				& RST_MODEMR_MD5) >> MD5_SHIFT_TO_BIT0;
	uint32_t softmd = (mmio_read_32(MFISOFTMDR)
				& SOFTMD_BIT0);

	/* Get LCS */
	ret = ROM_GetLcs(&lcs);
	if (ret != 0U) {
		/* error check */
		ERROR("CR7: Failed to get the LCS. (%d)\n", ret);
		panic();
	}

	if (lcs == LCS_SE) { /* LCS=Secure */
		if (softmd == 0x1U) {
			/* LCS=Secure + Normal boot (temp setting) */
			sbrom_SecureBootAPI = NULL;
		} else {/* else is LCS=Secure + Secure boot */
			sbrom_SecureBootAPI =
					(SECURE_BOOT_API)ROM_SecureBootAPI;
		}
	} else if (lcs == LCS_SD) {
		/* LCS=SD */
		sbrom_SecureBootAPI = NULL;
	} else if (md == 0U) {	/* MD5=0 => LCS=CM/DM/FA + Secure boot */
		sbrom_SecureBootAPI = (SECURE_BOOT_API)ROM_SecureBootAPI;
	} else {
		/* MD5=1 => LCS=CM/DM/SD/FA + Normal boot */
		sbrom_SecureBootAPI = NULL;
	}
}
#else /* RCAR_SECURE_BOOT */
	sbrom_SecureBootAPI = NULL;
#endif /* RCAR_SECURE_BOOT */

	if (sbrom_SecureBootAPI != NULL) {
		NOTICE("Secure boot(CR7)\n");
		execDMA(SEC_BOOT_CERT_ADDR(boot_cert_addr),
			FLASH_BASE + boot_cert_addr, size);
	} else {
		NOTICE("Normal boot(CR7)\n");
	}

/*****************************************************************************
 *	Load CR7 RTOS certificate from HyperFlash / QSPI Flash
 *****************************************************************************/
	execDMA(RTOS_CERT_ADDR, FLASH_RTOS_CERT_ADDR, FLASH_CERT_SIZE);

/*****************************************************************************
 *	Load RTOS from HyperFlash / QSPI Flash
 *****************************************************************************/
	get_info_from_cert(RTOS_CERT_ADDR, &size, &rtos_load_addr);

	if (size == 0U) {
		ERROR("RTOS image size error\n");
		panic();
	} else if (size > RTOS_MAX_SIZE) {
		ERROR("RTOS image size error\n");
		panic();
	} else {
		/* No else processing QAC compliant */
	}

	check_load_area(rtos_load_addr, FLASH_RTOS_IMAGE_ADDR,
			size, ADDR_RANGE_TYPE0);

	NOTICE("RTOS load address=0x%x RTOS image size=0x%x\n",
		rtos_load_addr, size);
	execDMA(rtos_load_addr, FLASH_RTOS_IMAGE_ADDR, size);

/*****************************************************************************
 *	CR7 RTOS authentication
 *****************************************************************************/
	if (sbrom_SecureBootAPI != NULL) {
		ret = sbrom_SecureBootAPI(SEC_BOOT_CERT_ADDR(boot_cert_addr),
			RTOS_CERT_ADDR, NULL);
		if (ret != 0U) {
			ERROR("RTOS image Verification Failed!!!(0x%x)\n", ret);
			panic();
		}
	}

#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
/*****************************************************************************
 *	Load CA57 Loader certificate from HyperFlash / QSPI Flash
 *****************************************************************************/
	execDMA(IPL_CERT_ADDR, FLASH_CA5X_CERT_ADDR, FLASH_CERT_SIZE);

/*****************************************************************************
 *	Load CA57 Loader from HyperFlash / QSPI Flash
 *****************************************************************************/
	get_info_from_cert(IPL_CERT_ADDR, &size, &ca5x_load_addr);

	if (size == 0U) {
		ERROR("CA5x Loader image size error\n");
		panic();
	} else if (size > MAIN_LOADER_MAX_SIZE) {
		ERROR("CA5x Loader image size error\n");
		panic();
	} else {
		/* No else processing QAC compliant */
	}

	check_load_area(ca5x_load_addr, FLASH_CA5X_IMAGE_ADDR,
			size, ADDR_RANGE_TYPE1);

	NOTICE("CA5x Loader load address=0x%x CA5x Loader image size=0x%x\n",
		ca5x_load_addr, size);
	execDMA(ca5x_load_addr, FLASH_CA5X_IMAGE_ADDR, size);

/*****************************************************************************
 *	CA57 Loader authentication
 *****************************************************************************/
	if (sbrom_SecureBootAPI != NULL) {
		ret = sbrom_SecureBootAPI(SEC_BOOT_CERT_ADDR(boot_cert_addr),
					  IPL_CERT_ADDR, NULL);
		if (ret != 0U) {
			ERROR("CA5x Loader image "
				"Verification Failed!!!(0x%x)\n", ret);
			panic();
		}
	}

/*****************************************************************************
 *	Display which CPU is kicked.
 *****************************************************************************/

#if (RCAR_KICK_MAIN_CPU == MAIN_CPU_CA57)
	NOTICE("Kick CA57 as main CPU\n");

#elif (RCAR_KICK_MAIN_CPU == MAIN_CPU_CA53)
	NOTICE("Kick CA53 as main CPU\n");

#endif /* (RCAR_KICK_MAIN_CPU == MAIN_CPU_CA57) */

#endif /* (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE) */

/*****************************************************************************
 *	Serial output wait
 *****************************************************************************/
	(void)console_wait();

/*****************************************************************************
 *	SWDT Release
 *****************************************************************************/
	swdt_release();

/*****************************************************************************
 *	GIC Non Secure setting
 *****************************************************************************/
	gic_security_setting();

/*****************************************************************************
 *	SRAM/DRAM protection setting
 *****************************************************************************/
	axi_security_setting();

/*****************************************************************************
 *	LifeC setting
 *****************************************************************************/
#if(RCAR_LIFEC_SETTING != LIFEC_SETTING_INVALID)
	lifec_init();
#else
	NOTICE("Skipping LifeC setting for testing\n");
#endif /* RCAR_LIFEC_SETTING */

/*****************************************************************************
 *	CA57/53 bootup
 *****************************************************************************/
#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
	rcar_pwrc_cpuon(ca5x_load_addr);
#endif /* RCAR_KICK_MAIN_CPU */

/*****************************************************************************
 *	LifeC setting for CR7
 *****************************************************************************/
	lifec_cr7_setting();

	return rtos_load_addr;

}

static void check_load_area(uint32_t dst, uint32_t src,
			uint32_t len, uint8_t load_check)
{
	static const struct address_range add_list[] = {
		{DRAM_BASE,	DRAM_END},
		{SYSRAM_BASE,	SYSRAM_END}
	};

	/* check source address range (FLASH) */
	if ((src < FLASH_BASE) || ((FLASH_BASE + FLASH_SIZE) < (src + len))) {
		ERROR("CR7: check load area (source address)\n");
		ERROR("CR7: source address = 0x%x image size = 0x%x\n",
			src, len);
		panic();
	}

	/* check destination address range (SystemRAM or SDRAM) */
	if (dst < add_list[load_check].topadd) {
		ERROR("CR7: check load area (destination address)\n");
		ERROR("CR7: destination address = 0x%x image size = 0x%x\n",
			dst, len);
		panic();
	}
	if (add_list[load_check].endadd < (dst + len)) {
		ERROR("CR7: check load area (destination address)\n");
		ERROR("CR7: destination address = 0x%x image size = 0x%x\n",
			dst, len);
		panic();
	}
}
