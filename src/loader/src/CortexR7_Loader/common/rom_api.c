/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "mmio.h"
#include "rcar_def.h"
#include "rom_api.h"


typedef uint32_t (*ROM_SECURE_BOOT_API)(uint32_t *pKeyCert,
					uint32_t *pContentCert,
					ROM_FuncReadFlash pFuncReadFlash);
typedef uint32_t (*ROM_GETLCS_API)(uint32_t *pLcs);

static uint32_t get_table_index(void);


#define OLD_API_TABLE1	(0U)	/* H3 Ver.1.0/Ver.1.1 */
#define OLD_API_TABLE2	(1U)	/* H3 Ver.2.0 */
#define OLD_API_TABLE3	(2U)	/* M3 Ver.1.0 */
#define NEW_API_TABLE	(3U)	/* H3 Ver.3.0, M3 Ver.1.1 or later, M3N, E3 */
#define API_TABLE_MAX	(4U)	/* table max */



uint32_t ROM_SecureBootAPI(uint32_t *pKeyCert,
			   uint32_t *pContentCert,
			   ROM_FuncReadFlash pFuncReadFlash)
{
	/* Secure Boot API address table */
	static const uintptr_t ROM_SecureBootAPI_table[API_TABLE_MAX] = {
		0xEB101F54U,	/* H3 Ver.1.0/Ver.1.1 */
		0xEB1041D4U,	/* H3 Ver.2.0 */
		0xEB103EFCU,	/* M3 Ver.1.0 */
		0xEB100100U	/* H3 Ver.3.0, M3 Ver.1.1 or later, M3N, E3 */
	};

	ROM_SECURE_BOOT_API func;
	uint32_t index;

	index = get_table_index();
	func = (ROM_SECURE_BOOT_API)ROM_SecureBootAPI_table[index];

	return func(pKeyCert, pContentCert, pFuncReadFlash);
}


uint32_t ROM_GetLcs(uint32_t *pLcs)
{
	/* Get LCS stete API address table */
	static const uintptr_t ROM_GetLcs_table[API_TABLE_MAX] = {
		0xEB1021B4U,	/* H3 Ver.1.0/Ver.1.1 */
		0xEB104434U,	/* H3 Ver.2.0 */
		0xEB10415CU,	/* M3 Ver.1.0 */
		0xEB10010CU	/* H3 Ver.3.0, M3 Ver.1.1 or later, M3N, E3 */
	};

	ROM_GETLCS_API func;
	uint32_t index;

	index = get_table_index();
	func = (ROM_GETLCS_API)ROM_GetLcs_table[index];

	return func(pLcs);
}


static uint32_t get_table_index(void)
{
	uint32_t product;
	uint32_t cut_ver;
	uint32_t index;

	product = mmio_read_32(RCAR_PRR) & RCAR_PRODUCT_MASK;
	cut_ver = mmio_read_32(RCAR_PRR) & RCAR_CUT_MASK;

	switch (product) {
	case RCAR_PRODUCT_H3:
		if (cut_ver == RCAR_CUT_VER10) {
			/* H3 Ver.1.0 */
			index = OLD_API_TABLE1;
		} else if (cut_ver == RCAR_CUT_VER11) {
			/* H3 Ver.1.1 */
			index = OLD_API_TABLE1;
		} else if (cut_ver == RCAR_CUT_VER20) {
			/* H3 Ver.2.0 */
			index = OLD_API_TABLE2;
		} else {
			/* Later than H3 Ver.2.0 */
			index = NEW_API_TABLE;
		}
		break;
	case RCAR_PRODUCT_M3:
		if (cut_ver == RCAR_CUT_VER10) {
			/* M3 Ver.1.0 */
			index = OLD_API_TABLE3;
		} else {
			/* M3 Ver.1.1 or later */
			index = NEW_API_TABLE;
		}
		break;
	default:
		/* M3N and E3 */
		index = NEW_API_TABLE;
		break;
	}

	return index;
}
