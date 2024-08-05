#
# Copyright (c) 2015-2019, Renesas Electronics Corporation.
# All rights reserved.
#

###################################################
# makefile
###################################################

define add_define
DEFINES			+=	-D$(1)$(if $(value $(1)),=$(value $(1)),)
endef

INCLUDE_DIR	=	-Iinclude			\
			-Iddr

#output file name
FILE_NAME	=	cr7_loader
FILE_NAME_SA0   =	bootparam_sa0
FILE_NAME_SA3   =	cert_header_sa3

OUTPUT_FILE	=	$(FILE_NAME).elf
OUTPUT_FILE_SA0 =	$(FILE_NAME_SA0).elf
OUTPUT_FILE_SA3 =	$(FILE_NAME_SA3).elf

#object file name
OBJ_FILE	=	common/mem.o			\
			common/tcm_cpy.o		\
			common/scif.o			\
			common/div.o			\
			common/llsl.o			\
			common/llsr.o			\
			common/micro_wait.o		\
			common/printf.o			\
			common/dma_driver.o		\
			common/rpc_driver.o		\
			common/rcar_pwrc.o		\
			common/cpsr_acc.o		\
			common/ecc_init.o		\
			common/dram_dual_ecc.o		\
			common/single_ecc.o		\
			common/swdt.o			\
			common/arm_gic.o		\
			common/gic_v2.o			\
			common/error_output.o		\
			common/report_exception.o	\
			common/protection_setting.o	\
			common/lifec_init.o		\
			common/rom_api.o		\
			common/auth.o			\
			loader/loader.o			\
			loader/loader_main.o		\
			loader/rcar_version.o		\
			loader/cpg_init.o		\
			loader/dummy_vector_table.o	\
			loader/stack.o			\
			tcm_loader/tcm_loader.o		\
			tcm_loader/tcm_loader_main.o

OBJ_FILE_SA0	=	tools/dummy_create/sa0.o
OBJ_FILE_SA3	=	tools/dummy_create/sa3.o


#linker script name
MEMORY_DEF	=	loader/loader.ld.S
MEMORY_DEF_SA0	=	tools/dummy_create/sa0.ld.S
MEMORY_DEF_SA3	=	tools/dummy_create/sa3.ld.S

###################################################

# Debug build
DEBUG:=0

# LSI setting common define
RCAR_H3:=0
RCAR_M3:=1
RCAR_M3N:=2
RCAR_E3:=3
RCAR_H3N:=4
RCAR_AUTO:=99
$(eval $(call add_define,RCAR_H3))
$(eval $(call add_define,RCAR_M3))
$(eval $(call add_define,RCAR_M3N))
$(eval $(call add_define,RCAR_E3))
$(eval $(call add_define,RCAR_H3N))
$(eval $(call add_define,RCAR_AUTO))
RCAR_CUT_10:=0
RCAR_CUT_11:=1
RCAR_CUT_13:=3
RCAR_CUT_20:=10
RCAR_CUT_30:=20
$(eval $(call add_define,RCAR_CUT_10))
$(eval $(call add_define,RCAR_CUT_11))
$(eval $(call add_define,RCAR_CUT_13))
$(eval $(call add_define,RCAR_CUT_20))
$(eval $(call add_define,RCAR_CUT_30))

ifndef LSI
  $(error "Error: Unknown LSI. Please use LSI=<LSI name> to specify the LSI")
else
  ifeq (${LSI},AUTO)
    RCAR_LSI:=${RCAR_AUTO}
  else ifeq (${LSI},H3)
    RCAR_LSI:=${RCAR_H3}
    ifndef LSI_CUT
      # enable compatible function.
      RCAR_LSI_CUT_COMPAT := 1
      $(eval $(call add_define,RCAR_LSI_CUT_COMPAT))
    else
      # disable compatible function.
      ifeq (${LSI_CUT},10)
        RCAR_LSI_CUT:=0
      else ifeq (${LSI_CUT},11)
        RCAR_LSI_CUT:=1
      else ifeq (${LSI_CUT},20)
        RCAR_LSI_CUT:=10
      else ifeq (${LSI_CUT},30)
        RCAR_LSI_CUT:=20
      else
        $(error "Error: ${LSI_CUT} is not supported.")
      endif
      $(eval $(call add_define,RCAR_LSI_CUT))
    endif
  else ifeq (${LSI},H3N)
    RCAR_LSI:=${RCAR_H3N}
    ifndef LSI_CUT
      # enable compatible function.
      RCAR_LSI_CUT_COMPAT := 1
      $(eval $(call add_define,RCAR_LSI_CUT_COMPAT))
    else
      # disable compatible function.
      ifeq (${LSI_CUT},30)
        RCAR_LSI_CUT:=20
      else
        $(error "Error: ${LSI_CUT} is not supported.")
      endif
      $(eval $(call add_define,RCAR_LSI_CUT))
    endif
  else ifeq (${LSI},M3)
    RCAR_LSI:=${RCAR_M3}
    ifndef LSI_CUT
      # enable compatible function.
      RCAR_LSI_CUT_COMPAT := 1
      $(eval $(call add_define,RCAR_LSI_CUT_COMPAT))
    else
      # disable compatible function.
      ifeq (${LSI_CUT},10)
        RCAR_LSI_CUT:=0
      else ifeq (${LSI_CUT},11)
        RCAR_LSI_CUT:=1
      else ifeq (${LSI_CUT},13)
        RCAR_LSI_CUT:=3
      else ifeq (${LSI_CUT},30)
        RCAR_LSI_CUT:=20
      else
        $(error "Error: ${LSI_CUT} is not supported.")
      endif
      $(eval $(call add_define,RCAR_LSI_CUT))
    endif
  else ifeq (${LSI},M3N)
    RCAR_LSI:=${RCAR_M3N}
    ifndef LSI_CUT
      # enable compatible function.
      RCAR_LSI_CUT_COMPAT := 1
      $(eval $(call add_define,RCAR_LSI_CUT_COMPAT))
    else
      # disable compatible function.
      ifeq (${LSI_CUT},10)
        RCAR_LSI_CUT:=0
      else ifeq (${LSI_CUT},11)
        RCAR_LSI_CUT:=0
      else
        $(error "Error: ${LSI_CUT} is not supported.")
      endif
      $(eval $(call add_define,RCAR_LSI_CUT))
    endif
  else ifeq (${LSI},E3)
    RCAR_LSI:=${RCAR_E3}
    ifndef LSI_CUT
      # enable compatible function.
      RCAR_LSI_CUT_COMPAT := 1
      $(eval $(call add_define,RCAR_LSI_CUT_COMPAT))
    else
      # disable compatible function.
      ifeq (${LSI_CUT},10)
        RCAR_LSI_CUT:=0
      else ifeq (${LSI_CUT},11)
        RCAR_LSI_CUT:=1
      else
        $(error "Error: ${LSI_CUT} is not supported.")
      endif
      $(eval $(call add_define,RCAR_LSI_CUT))
    endif
  else
    $(error "Error: ${LSI} is not supported.")
  endif
  $(eval $(call add_define,RCAR_LSI))
endif

# Process DEBUG flag
$(eval $(call assert_boolean,DEBUG))
$(eval $(call add_define,DEBUG))
ifeq (${DEBUG},0)
  $(eval $(call add_define,NDEBUG))
  # Use LOG_LEVEL_NOTICE by default for release builds
  LOG_LEVEL:=20
else
  # Use LOG_LEVEL_INFO by default for debug builds
  LOG_LEVEL:=40
  CFLAGS			+= 	-g
  ASFLAGS			+= 	-g -Wa,--gdwarf-2
endif

# Process LOG_LEVEL flag
ifndef LOG_LEVEL
LOG_LEVEL	:=	20
endif
$(eval $(call add_define,LOG_LEVEL))

# Process RCAR_SECURE_BOOT flag
ifndef RCAR_SECURE_BOOT
RCAR_SECURE_BOOT := 1
endif
$(eval $(call add_define,RCAR_SECURE_BOOT))

# Process RCAR_QOS_TYPE flag
ifndef RCAR_QOS_TYPE
RCAR_QOS_TYPE := 0
endif
$(eval $(call add_define,RCAR_QOS_TYPE))

# Process RCAR_DRAM_SPLIT flag
ifndef RCAR_DRAM_SPLIT
RCAR_DRAM_SPLIT := 0
endif
$(eval $(call add_define,RCAR_DRAM_SPLIT))

# Process RCAR_KICK_MAIN_CPU flag
ifndef RCAR_KICK_MAIN_CPU
RCAR_KICK_MAIN_CPU := 0
else ifeq (${RCAR_KICK_MAIN_CPU},1)
  ifeq (${LSI},E3)
      $(error "Error: R-Car ${LSI} does not have CA57.")
  endif
else ifeq (${RCAR_KICK_MAIN_CPU},2)
  ifeq (${LSI},M3N)
      $(error "Error: R-Car ${LSI} does not have CA53.")
  endif
endif
$(eval $(call add_define,RCAR_KICK_MAIN_CPU))

# Process RCAR_ECC_ENABLE flag
ifndef RCAR_ECC_ENABLE
RCAR_ECC_ENABLE := 0
else ifeq (${RCAR_ECC_ENABLE},1)
  ifeq (${LSI},H3)
    ifeq (${LSI_CUT},10)
      $(error "Error: ${LSI} Ver.1.0 is not supported Single Channel ECC.")
    else ifeq (${LSI_CUT},11)
      $(error "Error: ${LSI} Ver.1.1 is not supported Single Channel ECC.")
    endif
  else ifeq (${LSI},M3)
    ifeq (${LSI_CUT},10)
      $(error "Error: ${LSI} Ver.1.0 is not supported Single Channel ECC.")
    else ifeq (${LSI_CUT},11)
      $(error "Error: ${LSI} Ver.1.1 / Ver.1.2 is not supported Single Channel ECC.")
    else ifeq (${LSI_CUT},13)
      $(error "Error: ${LSI} Ver.1.3 is not supported Single Channel ECC.")
    endif
  endif
else ifeq (${RCAR_ECC_ENABLE},2)
  ifeq (${LSI},M3N)
      $(error "Error: ${LSI} is not supported Dual Channel ECC.")
  else ifeq (${LSI},E3)
      $(error "Error: ${LSI} is not supported Dual Channel ECC.")
  else ifeq (${LSI},H3N)
      $(error "Error: ${LSI} is not supported Dual Channel ECC.")
  endif
endif
$(eval $(call add_define,RCAR_ECC_ENABLE))

# Process ARM_GIC_ARCH flag
ARM_GIC_ARCH		:=	2
$(eval $(call add_define,ARM_GIC_ARCH))

# Process RCAR_LIFEC_NON_SECURE_MASTER
ifndef RCAR_LIFEC_NON_SECURE_MASTER
RCAR_LIFEC_NON_SECURE_MASTER	:= 1
endif
$(eval $(call add_define,RCAR_LIFEC_NON_SECURE_MASTER))

# Process RCAR_GEN3_ULCB flag
ifndef RCAR_GEN3_ULCB
RCAR_GEN3_ULCB := 0
endif
$(eval $(call add_define,RCAR_GEN3_ULCB))

# Process RCAR_DRAM_LPDDR4_MEMCONF flag
ifndef RCAR_DRAM_LPDDR4_MEMCONF
RCAR_DRAM_LPDDR4_MEMCONF :=1
endif
$(eval $(call add_define,RCAR_DRAM_LPDDR4_MEMCONF))

# Process RCAR_DRAM_CHANNEL flag
ifndef RCAR_DRAM_CHANNEL
RCAR_DRAM_CHANNEL :=15
endif
$(eval $(call add_define,RCAR_DRAM_CHANNEL))

# Process RCAR_DRAM_DDR3L_MEMCONF flag
ifndef RCAR_DRAM_DDR3L_MEMCONF
RCAR_DRAM_DDR3L_MEMCONF :=1
endif
$(eval $(call add_define,RCAR_DRAM_DDR3L_MEMCONF))

# Process RCAR_DRAM_DDR3L_MEMDUAL flag
ifndef RCAR_DRAM_DDR3L_MEMDUAL
RCAR_DRAM_DDR3L_MEMDUAL :=1
endif
$(eval $(call add_define,RCAR_DRAM_DDR3L_MEMDUAL))

# Process RCAR_REF_INT flag
ifndef RCAR_REF_INT
RCAR_REF_INT :=0
endif
$(eval $(call add_define,RCAR_REF_INT))

# Process RCAR_REWT_TRAINING flag
ifndef RCAR_REWT_TRAINING
RCAR_REWT_TRAINING := 1
endif
$(eval $(call add_define,RCAR_REWT_TRAINING))

# Process RCAR_SA0_SIZE flag
ifndef RCAR_SA0_SIZE
RCAR_SA0_SIZE := 1
else
ifeq (${RCAR_SA0_SIZE},0)
RCAR_SA0_SIZE := 0
else
RCAR_SA0_SIZE := 1
endif
endif
$(eval $(call add_define,RCAR_SA0_SIZE))

# Process RCAR_DDR_REG_CHECK flag
ifndef RCAR_DDR_REG_CHECK
RCAR_DDR_REG_CHECK :=0
endif
$(eval $(call add_define,RCAR_DDR_REG_CHECK))

# Process RCAR_LIFEC_SETTING flag
ifndef RCAR_LIFEC_SETTING
RCAR_LIFEC_SETTING := 0
endif
$(eval $(call add_define,RCAR_LIFEC_SETTING))

# Process RCAR_CA5x_OS flag
#default is LINUX
# OS setting common define
OS_LINUX:=1
OS_ANDROID:=2
ifndef RCAR_CA5x_OS
  RCAR_CA5x_OS := OS_LINUX
else 
ifeq (${RCAR_CA5x_OS},1)
  RCAR_CA5x_OS := OS_LINUX
else 
ifeq (${RCAR_CA5x_OS},2)
  RCAR_CA5x_OS := OS_ANDROID
else
  $(error "Error: ${RCAR_CA5x_OS} is not supported, enter 1 for Linux and 2 for Android.")
endif
endif
endif

$(eval $(call add_define,RCAR_CA5x_OS))
$(eval $(call add_define,OS_LINUX))
$(eval $(call add_define,OS_ANDROID))

include pfc/pfc.mk
include ddr/ddr.mk
include qos/qos.mk

###################################################

CC			= $(CROSS_COMPILE)gcc
CPP			= ${CROSS_COMPILE}cpp
AS			= ${CROSS_COMPILE}gcc
AR			= ${CROSS_COMPILE}ar
LD			= $(CROSS_COMPILE)ld
OC			= ${CROSS_COMPILE}objcopy
OD			= ${CROSS_COMPILE}objdump

ASFLAGS		+=	-marm -march=armv7-r				\
			-nostdinc -ffreestanding -Wa,--fatal-warnings	\
			-Werror -Wmissing-include-dirs			\
			 -c -D__ASSEMBLY				\
			$(INCLUDE_DIR) $(DEFINES)

CFLAGS		+=	-marm -march=armv7-r				\
			-nostdinc -ffreestanding -Wall			\
			-Werror -Wmissing-include-dirs			\
			-std=c99 -c -Os					\
			-ffunction-sections -fdata-sections		\
			$(INCLUDE_DIR) $(DEFINES)

LDFLAGS		=	--fatal-warnings -O1 --gc-sections

BUILD_MESSAGE_TIMESTAMP ?= __TIME__", "__DATE__

###################################################
.SUFFIXES : .s .c .o

###################################################
# command

.PHONY: all
all: $(OUTPUT_FILE) $(OUTPUT_FILE_SA0) $(OUTPUT_FILE_SA3)

###################################################
# Linker
###################################################
$(OUTPUT_FILE) : $(MEMORY_DEF) $(OBJ_FILE)
	@echo 'const char build_message[] = "Built : "$(BUILD_MESSAGE_TIMESTAMP);' | \
		$(CC) $(CFLAGS) -xc - -o build_message.o

	$(LD) $(OBJ_FILE) build_message.o	\
	-T $(MEMORY_DEF)		\
	-o $(OUTPUT_FILE)		\
	$(LDFLAGS)				\
	-Map $(FILE_NAME).map

	$(OC) -O srec --srec-forceS3  $(OUTPUT_FILE) $(FILE_NAME).srec
	$(OC) -O binary $(OUTPUT_FILE) $(FILE_NAME).bin
	$(OD) -dx $(OUTPUT_FILE) > $(FILE_NAME).dump

$(OUTPUT_FILE_SA0) : $(MEMORY_DEF_SA0) $(OBJ_FILE_SA0)
	$(LD) $(OBJ_FILE_SA0)		 	\
	-T $(MEMORY_DEF_SA0)			\
	-o $(OUTPUT_FILE_SA0)			\
	-Map $(FILE_NAME_SA0).map 		\

	$(OC) -O srec --adjust-vma=0xE6320000 --srec-forceS3  $(OUTPUT_FILE_SA0) $(FILE_NAME_SA0).srec
	$(OC) -O binary --adjust-vma=0xE6320000 --srec-forceS3  $(OUTPUT_FILE_SA0) $(FILE_NAME_SA0).bin

$(OUTPUT_FILE_SA3) : $(MEMORY_DEF_SA3) $(OBJ_FILE_SA3)
	$(LD) $(OBJ_FILE_SA3)		 	\
	-T $(MEMORY_DEF_SA3)			\
	-o $(OUTPUT_FILE_SA3)			\
	-Map $(FILE_NAME_SA3).map 		\

	$(OC) -O srec --adjust-vma=0xE6320000 --srec-forceS3  $(OUTPUT_FILE_SA3) $(FILE_NAME_SA3).srec
	$(OC) -O binary --adjust-vma=0xE6320000 --srec-forceS3  $(OUTPUT_FILE_SA3) $(FILE_NAME_SA3).bin


###################################################
# Compile
###################################################

%.o:../%.c
	$(CC) $(CFLAGS) -o $@ $<

%.o:../%.s
	$(AS) $(ASFLAGS) -o $@ $<


.PHONY: clean
clean:
	$(RM) $(OBJ_FILE) $(OUTPUT_FILE) $(OBJ_FILE_SA0) $(OBJ_FILE_SA3) $(OUTPUT_FILE_SA0) $(OUTPUT_FILE_SA3) \
	$(FILE_NAME).* $(FILE_NAME_SA0).* $(FILE_NAME_SA3).* *.o
