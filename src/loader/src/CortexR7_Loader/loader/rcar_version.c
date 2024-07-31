/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "rcar_version.h"

const uint8_t version_of_renesas[VER_SIZE]
	__attribute__((__section__(".version"))) = VERSION_OF_RENESAS;
