/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#define STACK_SIZE	(8U)*(1024U)

	.section cr7_loader_stack, "aw", %nobits
	.align 5
stacks:
	.space STACK_SIZE, 0

	.end
