/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "debug.h"
#include "error_output.h"
#include "swdt.h"

void undef_error(uint32_t occ_add)
{
	/* Clear the interrupt request	*/
	swdt_release();

	ERROR("\n");
	ERROR("CR7: Undefined Instruction occurred.\n");
	ERROR("     Occurred address : 0x%x\n", occ_add);
	ERROR("\n");

	/* Endless loop		*/
	panic();
}

void fiq_error(void)
{
	/* Clear the interrupt request	*/
	swdt_release();

	ERROR("\n");
	ERROR("CR7: FIQ handled.\n");

	/* Endless loop		*/
	panic();
}

void interrupt_error_id(uint32_t int_id, uint32_t occ_add)
{

	/* Clear the interrupt request	*/
	swdt_release();

	ERROR("\n");
	ERROR("CR7: Unhandled exception occurred.\n");
	ERROR("     Occurred address : 0x%x\n", occ_add);
	ERROR("     Interrupt ID : %d\n", int_id);
	ERROR("\n");
	/* Endless loop		*/
	panic();

}

void dabort_error(uint32_t occ_add, uint32_t dfsr, uint32_t dfar)
{
	/* Clear the interrupt request	*/
	swdt_release();

	ERROR("\n");
	ERROR("CR7: data abort.\n");
	ERROR("     data abort occurrred address : 0x%x\n", occ_add);
	ERROR("     DFSR : 0x%x DFAR : 0x%x\n", dfsr, dfar);
	ERROR("\n");

	/* Endless loop		*/
	panic();
}

void pabort_error(uint32_t ifsr, uint32_t ifar)
{
	/* Clear the interrupt request	*/
	swdt_release();

	ERROR("\n");
	ERROR("CR7: prefetch abort.\n");
	ERROR("     IFSR : 0x%x IFAR : 0x%x\n", ifsr, ifar);
	ERROR("\n");

	/* Endless loop		*/
	panic();
}
