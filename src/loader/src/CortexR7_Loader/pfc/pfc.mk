#
# Copyright (c) 2015-2019, Renesas Electronics Corporation.
# All rights reserved.
#

ifeq (${RCAR_LSI},${RCAR_AUTO})
    OBJ_FILE += pfc/H3/pfc_init_h3_v1.o
    OBJ_FILE += pfc/H3/pfc_init_h3_v2.o
    OBJ_FILE += pfc/M3/pfc_init_m3.o
    OBJ_FILE += pfc/M3N/pfc_init_m3n.o

else ifdef RCAR_LSI_CUT_COMPAT
  ifeq (${RCAR_LSI},${RCAR_H3})
    OBJ_FILE += pfc/H3/pfc_init_h3_v1.o
    OBJ_FILE += pfc/H3/pfc_init_h3_v2.o
  endif
  ifeq (${RCAR_LSI},${RCAR_H3N})
    OBJ_FILE += pfc/H3/pfc_init_h3_v1.o
    OBJ_FILE += pfc/H3/pfc_init_h3_v2.o
  endif
  ifeq (${RCAR_LSI},${RCAR_M3})
    OBJ_FILE += pfc/M3/pfc_init_m3.o
  endif
  ifeq (${RCAR_LSI},${RCAR_M3N})
    OBJ_FILE += pfc/M3N/pfc_init_m3n.o
  endif
  ifeq (${RCAR_LSI},${RCAR_E3})
    OBJ_FILE += pfc/E3/pfc_init_e3.o
  endif
else
  ifeq (${RCAR_LSI},${RCAR_H3})
    ifeq (${LSI_CUT},10)
      OBJ_FILE += pfc/H3/pfc_init_h3_v1.o
    else ifeq (${LSI_CUT},11)
      OBJ_FILE += pfc/H3/pfc_init_h3_v1.o
    else
#     LSI_CUT 20
      OBJ_FILE += pfc/H3/pfc_init_h3_v2.o
    endif
  endif
  ifeq (${RCAR_LSI},${RCAR_H3N})
    OBJ_FILE += pfc/H3/pfc_init_h3_v2.o
  endif
  ifeq (${RCAR_LSI},${RCAR_M3})
    OBJ_FILE += pfc/M3/pfc_init_m3.o
  endif
  ifeq (${RCAR_LSI},${RCAR_M3N})
    OBJ_FILE += pfc/M3N/pfc_init_m3n.o
  endif
  ifeq (${RCAR_LSI},${RCAR_E3})
    OBJ_FILE += pfc/E3/pfc_init_e3.o
  endif
endif

OBJ_FILE += pfc/pfc_init.o
