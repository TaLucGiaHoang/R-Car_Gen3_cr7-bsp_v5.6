/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */


OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Startup)

MEMORY {
	VECT (rxa): ORIGIN = 0x00000000, LENGTH = 0x00000100
	RAM (rwxa): ORIGIN = 0xE6304000, LENGTH = 0x0002A800
	/* I-TCM : 32KiB */
	ITCM (rxa): ORIGIN = 0xEB000000, LENGTH = 0x00008000
	/* D-TCM : 32KiB */
	DTCM (rwxa): ORIGIN = 0xEB020000, LENGTH = 0x00008000
}


SECTIONS
{
	ASSERT(. == ALIGN(4096),
		"CR7 Loader address is not aligned on a page boundary.")

	.loader_start : {
		__RO_START__ = .;
		loader/loader.o(.text*)
		. = NEXT(1024);
		*rcar_version.o(.version*)
		. = NEXT(64);
		*(.testdata)
		. = NEXT(16);
		__TCM_LOADER_COPY_START__ = .;
	} >RAM

	.tcm_loader : {
		__TCM_LOADER_START__ = .;
		KEEP(*tcm_loader.o(".text*" ".rodata*"))
		*tcm_loader_main.o(".text*" ".rodata*")
		*scif.o(".text*" ".rodata*")
		*cpsr_acc.o(".text*" ".rodata*")
		*report_exception.o(".text*" ".rodata*")
		*arm_gic.o(".text*" ".rodata*")
		*gic_v2.o(".text*" ".rodata*")
		*error_output.o(".text*" ".rodata*")
		*micro_wait.o(".text*" ".rodata*")
		*protection_setting.o(".text*" ".rodata*")
		*mem.o(".text*" ".rodata*")
		*rom_api.o(".text*" ".rodata*")
		*swdt.o(".text*" ".rodata*")
		*dma_driver.o(".text*" ".rodata*")
		*printf.o(".text*" ".rodata*")
		*rcar_pwrc.o(".text*" ".rodata*")
		*rpc_driver.o(".text*" ".rodata*")
		*auth.o(".text*" ".rodata*")
		*lifec_init.o(".text*" ".rodata*")
		*ecc_init.o(".text*" ".rodata*")
		*single_ecc.o(".text*" ".rodata*")
		*cpg_init.o(".text*" ".rodata*")
		*div.o(".text*" ".rodata*")
		__TCM_LOADER_END__ = .;
	} >ITCM AT > RAM

	__TCM_LOADER_SIZE__ = SIZEOF(.tcm_loader);

	.dummy_vect : {
		__DUMMY_VECTOR_START__ = .;
		KEEP(*loader/dummy_vector_table.o(.text*))
		__DUMMY_VECTOR_END__ = .;
	} >VECT AT > RAM

	__DUMMY_VECTOR_SIZE__ = SIZEOF(.dummy_vect);

	.loader_ro : {
		*(.text*)
		*(.rodata*)
		__RO_END_UNALIGNED__ = .;
		. = NEXT(32);	/* aligned with cache line */
		__RO_END__ = .;
		__DATA_COPY_START__ = .;
	} >RAM

	.data : {
		__DATA_START__ = .;
		*(.data*)
		. = NEXT(32);	/* aligned with cache line */
		__DATA_END__ = .;
	} >DTCM AT >RAM

	__DATA_SIZE__ = SIZEOF(.data);

	__ITCM_BASE__ = 0xEB000000;

	stacks (NOLOAD) : ALIGN(16) {
		__STACKS_START__ = .;
		KEEP(*(cr7_loader_stack))
		__STACKS_END__ = .;
	} >DTCM

	/*
	 * The .bss section gets initialised to 0 at runtime.
	 * Its base address must be 4-byte aligned.
	 */
	.bss : ALIGN(4) {
		__BSS_START__ = .;
		*(SORT_BY_ALIGNMENT(.bss*))
		*(COMMON)
		/* The bottom of .bss section must be aligned with 4byte. */
		. = NEXT(4);
		__BSS_END__ = .;
	} >DTCM

	__BSS_SIZE__ = SIZEOF(.bss);

	__CR7_END__ = .;
}
