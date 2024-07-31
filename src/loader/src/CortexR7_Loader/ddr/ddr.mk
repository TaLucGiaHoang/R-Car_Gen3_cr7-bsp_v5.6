#
# Copyright (c) 2015-2019, Renesas Electronics Corporation.
# All rights reserved.
#

ifeq (${RCAR_LSI},${RCAR_E3})
    include ddr/ddr_a/ddr_a.mk
else
    include ddr/ddr_b_regcheck/ddr_b.mk
endif

OBJ_FILE += ddr/dram_sub_func.o

