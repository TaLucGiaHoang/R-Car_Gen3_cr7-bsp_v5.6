###################################################
# makefile
###################################################

define add_define
DEFINES			+=	-D$(1)$(if $(value $(1)),=$(value $(1)),)
endef

INCLUDE_DIR	=	-Iinclude

#output file name
FILE_NAME	=	dummy_rtos

OUTPUT_FILE	=	$(FILE_NAME).elf

OUTPUT_DIR  = output
OBJECT_DIR  = obj

#object file name
OBJ_FILE	=	common/scif.o			\
			common/div.o			\
			common/printf.o			\
			common/micro_wait.o		\
			rtos/rtos.o			\
			rtos/rtos_main.o		

#linker script name
MEMORY_DEF	=	rtos/rtos.ld.S

###################################################
# Process LOG_LEVEL flag
ifndef LOG_LEVEL
LOG_LEVEL	:=	20
endif
$(eval $(call add_define,LOG_LEVEL))

###################################################

CC			= $(CROSS_COMPILE)gcc
CPP			= ${CROSS_COMPILE}cpp
AS			= ${CROSS_COMPILE}gcc
AR			= ${CROSS_COMPILE}ar
LD			= $(CROSS_COMPILE)ld
OC			= ${CROSS_COMPILE}objcopy
OD			= ${CROSS_COMPILE}objdump

ASFLAGS		=	-marm -march=armv7-r				\
			-nostdinc -ffreestanding -Wa,--fatal-warnings	\
			-Werror -Wmissing-include-dirs			\
			 -c -D__ASSEMBLY				\
			$(INCLUDE_DIR) $(DEFINES)

CFLAGS		=	-marm -march=armv7-r				\
			-nostdinc -ffreestanding -Wall			\
			-Werror -Wmissing-include-dirs			\
			-std=c99 -c -Os					\
			-ffunction-sections -fdata-sections		\
			$(INCLUDE_DIR) $(DEFINES)

CFLAGS			+= 	-g
ASFLAGS			+= 	-g -Wa,--gdwarf-2

LDFLAGS		=	--fatal-warnings -O1 --gc-sections

###################################################
.SUFFIXES : .s .c .o

###################################################
# command

.PHONY: all
all: $(OBJECT_DIR) $(OUTPUT_DIR) $(OUTPUT_FILE) 
###################################################
# Make Directory
###################################################
$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

###################################################
# Linker
###################################################
$(OUTPUT_FILE) : $(MEMORY_DEF) $(OBJ_FILE)
	$(LD) $(OBJ_FILE)			\
	-T $(MEMORY_DEF)			\
	-o $(OUTPUT_FILE)			\
	$(LDFLAGS)				\
	-Map $(FILE_NAME).map

	$(OC) -O srec --srec-forceS3  $(OUTPUT_FILE) $(FILE_NAME).srec
	$(OC) -O binary $(OUTPUT_FILE) $(FILE_NAME).bin
	$(OD) -dx $(OUTPUT_FILE) > $(FILE_NAME).dump


###################################################
# Compile
###################################################

%.o:../%.c
	$(CC) $(CFLAGS) -o $@ $<

%.o:../%.s
	$(AS) $(ASFLAGS) -o $@ $<


.PHONY: clean
clean:
	$(RM) $(OBJ_FILE) $(OUTPUT_FILE)
