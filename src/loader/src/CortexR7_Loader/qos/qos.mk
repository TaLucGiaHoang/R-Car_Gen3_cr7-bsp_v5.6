#
# Copyright (c) 2015-2019, Renesas Electronics Corporation. All rights reserved.
#

ifeq (${RCAR_LSI},${RCAR_AUTO})
#   E3,H3N not available for LSI_AUTO
    OBJ_FILE += qos/H3/qos_init_h3_v10.o
    OBJ_FILE += qos/H3/qos_init_h3_v11.o
    OBJ_FILE += qos/H3/qos_init_h3_v20.o
    OBJ_FILE += qos/H3/qos_init_h3_v30.o
    OBJ_FILE += qos/M3/qos_init_m3_v10.o
    OBJ_FILE += qos/M3/qos_init_m3_v11.o
    OBJ_FILE += qos/M3/qos_init_m3_v30.o
    OBJ_FILE += qos/M3N/qos_init_m3n_v10.o
else ifdef RCAR_LSI_CUT_COMPAT
  ifeq (${RCAR_LSI},${RCAR_H3})
    OBJ_FILE += qos/H3/qos_init_h3_v10.o
    OBJ_FILE += qos/H3/qos_init_h3_v11.o
    OBJ_FILE += qos/H3/qos_init_h3_v20.o
    OBJ_FILE += qos/H3/qos_init_h3_v30.o
  endif
  ifeq (${RCAR_LSI},${RCAR_H3N})
    OBJ_FILE += qos/H3/qos_init_h3n_v30.o
  endif
  ifeq (${RCAR_LSI},${RCAR_M3})
    OBJ_FILE += qos/M3/qos_init_m3_v10.o
    OBJ_FILE += qos/M3/qos_init_m3_v11.o
    OBJ_FILE += qos/M3/qos_init_m3_v30.o
  endif
  ifeq (${RCAR_LSI},${RCAR_M3N})
    OBJ_FILE += qos/M3N/qos_init_m3n_v10.o
  endif
  ifeq (${RCAR_LSI},${RCAR_E3})
    OBJ_FILE += qos/E3/qos_init_e3_v10.o
  endif
else
  ifeq (${RCAR_LSI},${RCAR_H3})
    ifeq (${LSI_CUT},10)
      OBJ_FILE += qos/H3/qos_init_h3_v10.o
    else ifeq (${LSI_CUT},11)
      OBJ_FILE += qos/H3/qos_init_h3_v11.o
    else ifeq (${LSI_CUT},20)
      OBJ_FILE += qos/H3/qos_init_h3_v20.o
    else ifeq (${LSI_CUT},30)
      OBJ_FILE += qos/H3/qos_init_h3_v30.o
    else
#     LSI_CUT 30 or later
      OBJ_FILE += qos/H3/qos_init_h3_v30.o
    endif
  endif
  ifeq (${RCAR_LSI},${RCAR_H3N})
    ifeq (${LSI_CUT},30)
      OBJ_FILE += qos/H3/qos_init_h3n_v30.o
    else
#     LSI_CUT 30 or later
      OBJ_FILE += qos/H3/qos_init_h3n_v30.o
    endif
  endif
  ifeq (${RCAR_LSI},${RCAR_M3})
    ifeq (${LSI_CUT},10)
     OBJ_FILE += qos/M3/qos_init_m3_v10.o
    else ifeq (${LSI_CUT},11)
     OBJ_FILE += qos/M3/qos_init_m3_v11.o
    else ifeq (${LSI_CUT},13)
     OBJ_FILE += qos/M3/qos_init_m3_v11.o
    else ifeq (${LSI_CUT},30)
     OBJ_FILE += qos/M3/qos_init_m3_v30.o
    else
#    LSI_CUT 30 or later
     OBJ_FILE += qos/M3/qos_init_m3_v11.o
    endif
  endif
  ifeq (${RCAR_LSI},${RCAR_M3N})
    ifeq (${LSI_CUT},10)
     OBJ_FILE += qos/M3N/qos_init_m3n_v10.o
    else
#    LSI_CUT 10 or later
     OBJ_FILE += qos/M3N/qos_init_m3n_v10.o
    endif
  endif
  ifeq (${RCAR_LSI},${RCAR_E3})
    ifeq (${LSI_CUT},10)
     OBJ_FILE += qos/E3/qos_init_e3_v10.o
    else
#    LSI_CUT 10 or later
     OBJ_FILE += qos/E3/qos_init_e3_v10.o
    endif
  endif
endif

OBJ_FILE += qos/qos_init.o
