/*
 * Copyright (c) 2015-2018, Renesas Electronics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Renesas nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdint.h>
#include <mmio.h>
#include "debug.h"
#include "micro_wait.h"


/************************************************************************************************/
/*	Definitions										*/
/************************************************************************************************/


/************************************************************************************************/
/*	Unions											*/
/************************************************************************************************/


/************************************************************************************************/
/*	Structs											*/
/************************************************************************************************/


/************************************************************************************************/
/*	Globals											*/
/************************************************************************************************/


/************************************************************************************************/
/*	Macros											*/
/************************************************************************************************/
#define MIDR_PN_SHIFT			(0x4U)
#define MIDR_PN_MASK			(0X0FFFU)
#define MIDR_CA57			(0x0D07U << MIDR_PN_SHIFT)
#define MIDR_CA53			(0x0D03U << MIDR_PN_SHIFT)
#define MIDR_CR7			(0x0C17U << MIDR_PN_SHIFT)

#define	RCAR_PRR			(0xFFF00044U)	/* Product register */
#define RCAR_PRODUCT_MASK		(0x00007F00U)
#define RCAR_CUT_MASK			(0x000000FFU)
#define RCAR_PRODUCT_H3			(0x00004F00U)
#define RCAR_PRODUCT_M3			(0x00005200U)
#define RCAR_PRODUCT_M3_CUT11		(RCAR_PRODUCT_M3 | 0x10U)
#define RCAR_PRODUCT_M3N		(0x00005500U)
#define RCAR_PRODUCT_E3			(0x00005700U)
#define RCAR_CUT_ES10			(0x00000000U)
#define RCAR_CUT_ES11			(0x00000001U)
#define RCAR_CUT_ES20			(0x00000010U)
#define RCAR_MAJOR_MASK			(0x000000F0U)
#define RCAR_MINOR_MASK			(0x0000000FU)
#define RCAR_PRODUCT_SHIFT		(8U)
#define RCAR_MAJOR_SHIFT		(4U)
#define RCAR_MINOR_SHIFT		(0U)
#define RCAR_MAJOR_OFFSET		(1U)

/* Global Timer on CR7 is operated at 200MHz */
/* Wait about 1.442[s]                       */
#define WAIT_TIME			(1442000U)

/************************************************************************************************/
/*	Prototypes										*/
/************************************************************************************************/
uint32_t rtos_main(void);

static void start_message(void);
static void display_prr(void);
static void display_copro_reg(void);
static void end_message(void);

uint32_t rtos_main(void)
{

	micro_wait_init();
	micro_wait(WAIT_TIME);
	start_message();
	display_prr();
	display_copro_reg();
	end_message();

	return 0U;
}

static void start_message(void)
{
	const char *str;
	uint32_t midr;
	const char *cpu_ca57    = "CA57";
	const char *cpu_ca53    = "CA53";
	const char *cpu_cr7     = "CR7";
	const char *unknown     = "unknown";

	tf_printf("\n");

	__asm__ volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (midr));
	midr = midr & (uint32_t)(MIDR_PN_MASK << MIDR_PN_SHIFT);
	switch (midr) {
	case MIDR_CA57:
		str = cpu_ca57;
		break;
	case MIDR_CA53:
		str = cpu_ca53;
		break;
	case MIDR_CR7:
		str = cpu_cr7;
		break;
	default:
		str = unknown;
		break;
	}
	NOTICE("R-Car Gen3 Dummy RTOS(%s)\n", str);
}

static void display_prr(void)
{
	uint32_t prr;

	const char *str;
	const char *product_h3  = "H3";
	const char *product_m3  = "M3";
	const char *product_m3n = "M3N";
	const char *product_e3  = "E3";
	const char *unknown     = "unknown";

	/* R-Car Gen3 product display & check */
	prr = mmio_read_32(RCAR_PRR);
	switch (prr & RCAR_PRODUCT_MASK) {
	case RCAR_PRODUCT_H3:
		str = product_h3;
		break;
	case RCAR_PRODUCT_M3:
		str = product_m3;
		break;
	case RCAR_PRODUCT_M3N:
		str = product_m3n;
		break;
	case RCAR_PRODUCT_E3:
		str = product_e3;
		break;
	default:
		str = unknown;
		break;
	}

	if(RCAR_PRODUCT_M3_CUT11 ==
		(prr & (RCAR_PRODUCT_MASK | RCAR_CUT_MASK))) {
		/* M3 Ver.1.1 or Ver.1.2 */
		NOTICE("PRR is R-Car %s Ver.1.1 / Ver.1.2\n", str);
	} else {
		NOTICE("PRR is R-Car %s Ver.%d.%d\n", str,
			((prr & RCAR_MAJOR_MASK) >> RCAR_MAJOR_SHIFT)
			 + RCAR_MAJOR_OFFSET, (prr & RCAR_MINOR_MASK));
	}
}

static void display_copro_reg(void)
{
	uint32_t v;

	__asm__ volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (v));
	NOTICE("MIDR = (0x%x)\n", v);

	__asm__ volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (v));
	NOTICE("CBAR = (0x%x)\n", v);

	__asm__ volatile ("mrs %0, cpsr" : "=r" (v));
	NOTICE("CPSR = (0x%x)\n", v);

}

static void end_message(void)
{
	NOTICE("Dummy RTOS boot end\n");
}
