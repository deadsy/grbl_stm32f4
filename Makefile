
# cross compilation tools
XTOOLS_DIR = /opt/gcc-arm-none-eabi-4_8-2013q4
X_LIBC_DIR = $(XTOOLS_DIR)/arm-none-eabi/lib/armv7e-m/fpu
X_LIBGCC_DIR = $(XTOOLS_DIR)/lib/gcc/arm-none-eabi/4.8.3/armv7e-m/fpu
X_CC = $(XTOOLS_DIR)/bin/arm-none-eabi-gcc
X_OBJCOPY = $(XTOOLS_DIR)/bin/arm-none-eabi-objcopy
X_AR = $(XTOOLS_DIR)/bin/arm-none-eabi-ar
X_LD = $(XTOOLS_DIR)/bin/arm-none-eabi-ld
X_GDB = $(XTOOLS_DIR)/bin/arm-none-eabi-gdb

OUTPUT = grbl_stm32f4

# grbl sources
GRBL_SRC_DIR = ./grbl/grbl-master
SRC = $(GRBL_SRC_DIR)/coolant_control.c \
      $(GRBL_SRC_DIR)/delay.c \
      $(GRBL_SRC_DIR)/eeprom.c \
      $(GRBL_SRC_DIR)/gcode.c \
      $(GRBL_SRC_DIR)/limits.c \
      $(GRBL_SRC_DIR)/main.c \
      $(GRBL_SRC_DIR)/motion_control.c \
      $(GRBL_SRC_DIR)/nuts_bolts.c \
      $(GRBL_SRC_DIR)/planner.c \
      $(GRBL_SRC_DIR)/print.c \
      $(GRBL_SRC_DIR)/protocol.c \
      $(GRBL_SRC_DIR)/report.c \
      $(GRBL_SRC_DIR)/serial.c \
      $(GRBL_SRC_DIR)/settings.c \
      $(GRBL_SRC_DIR)/spindle_control.c \
      $(GRBL_SRC_DIR)/stepper.c \

# hal sources
HAL_SRC_DIR = ./hal/src
SRC += $(HAL_SRC_DIR)/stm32f4xx_hal.c \
       $(HAL_SRC_DIR)/stm32f4xx_hal_rcc.c \
       $(HAL_SRC_DIR)/stm32f4xx_hal_cortex.c \
       $(HAL_SRC_DIR)/stm32f4xx_hal_gpio.c \

# usb sources
USB_SRC_DIR = ./usb
#SRC += $(USB_SRC_DIR)/core/usbd_core.c \
#       $(USB_SRC_DIR)/core/usbd_ctlreq.c \
#       $(USB_SRC_DIR)/core/usbd_ioreq.c \
#       $(USB_SRC_DIR)/cdc/usbd_cdc.c \

# board sources
SRC += main.c \
       system_stm32f4xx.c \
       stm32f4xx_it.c \

OBJ = $(patsubst %.c, %.o, $(SRC))
OBJ += start.o

# include files
INC = .
INC += ./cmsis
INC += ./hal/inc
INC += ./usb/core
INC += ./usb/cdc

INCLUDE = $(addprefix -I,$(INC))

# compiler flags
CFLAGS = -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

# linker flags
LDSCRIPT = stm32f407vg_flash.ld
LDFLAGS = -T$(LDSCRIPT) -Wl,-Map,$(OUTPUT).map -Wl,--gc-sections

DEFINES = -DSTM32F407xx

.S.o:
	$(X_CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $< -o $@
.c.o:
	$(X_CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $< -o $@

all: grbl_src $(OBJ)
	$(X_CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -lm -o $(OUTPUT)
	$(X_OBJCOPY) -O binary $(OUTPUT) $(OUTPUT).bin

.PHONY: program
program: 
	st-flash write $(OUTPUT).bin 0x08000000

.PHONY: grbl_src
grbl_src:
	make -C grbl all

clean:
	-rm $(OBJ)	
	-rm $(OUTPUT)
	-rm $(OUTPUT).map	
	-rm $(OUTPUT).bin	
	make -C grbl clean
