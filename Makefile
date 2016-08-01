
# cross compilation tools
XTOOLS_DIR = /opt/gcc-arm-none-eabi-5_4-2016q2
X_LIBC_DIR = $(XTOOLS_DIR)/arm-none-eabi/lib/armv7e-m/fpu
X_LIBGCC_DIR = $(XTOOLS_DIR)/lib/gcc/arm-none-eabi/5.4.1/armv7e-m/fpu
X_CC = $(XTOOLS_DIR)/bin/arm-none-eabi-gcc
X_OBJCOPY = $(XTOOLS_DIR)/bin/arm-none-eabi-objcopy
X_AR = $(XTOOLS_DIR)/bin/arm-none-eabi-ar
X_LD = $(XTOOLS_DIR)/bin/arm-none-eabi-ld
X_GDB = $(XTOOLS_DIR)/bin/arm-none-eabi-gdb

OUTPUT = grbl_stm32f4

USB_SERIAL = 0

# grbl sources
GRBL_DIR = ./grbl/grbl-master

SRC = $(GRBL_DIR)/coolant_control.c \
      $(GRBL_DIR)/delay.c \
      $(GRBL_DIR)/eeprom.c \
      $(GRBL_DIR)/gcode.c \
      $(GRBL_DIR)/limits.c \
      $(GRBL_DIR)/main.c \
      $(GRBL_DIR)/motion_control.c \
      $(GRBL_DIR)/nuts_bolts.c \
      $(GRBL_DIR)/planner.c \
      $(GRBL_DIR)/print.c \
      $(GRBL_DIR)/protocol.c \
      $(GRBL_DIR)/report.c \
      $(GRBL_DIR)/settings.c \
      $(GRBL_DIR)/spindle_control.c \
      $(GRBL_DIR)/stepper.c \
      $(GRBL_DIR)/serial.c \

# hal sources
HAL_DIR = ./hal/src
SRC += $(HAL_DIR)/stm32f4xx_hal.c \
       $(HAL_DIR)/stm32f4xx_hal_rcc.c \
       $(HAL_DIR)/stm32f4xx_hal_cortex.c \
       $(HAL_DIR)/stm32f4xx_hal_gpio.c \
       $(HAL_DIR)/stm32f4xx_hal_tim.c \
       $(HAL_DIR)/stm32f4xx_hal_tim_ex.c \

# board sources
BOARD_DIR = ./board
SRC += $(BOARD_DIR)/main.c \
       $(BOARD_DIR)/system_stm32f4xx.c \
       $(BOARD_DIR)/stm32f4xx_it.c \
       $(BOARD_DIR)/syscalls.c \
       $(BOARD_DIR)/gpio.c \
       $(BOARD_DIR)/debounce.c \
       $(BOARD_DIR)/timers.c \
       $(BOARD_DIR)/stm32f4_regs.c \

# usb/uart sources
USB_DIR = ./usb
ifeq ($(USB_SERIAL), 1)
  SRC += $(USB_DIR)/core/usbd_core.c \
         $(USB_DIR)/core/usbd_ctlreq.c \
         $(USB_DIR)/core/usbd_ioreq.c \
         $(USB_DIR)/cdc/usbd_cdc.c \
         $(BOARD_DIR)/usbd_conf.c \
         $(BOARD_DIR)/usbd_desc.c \
         $(BOARD_DIR)/usbd_cdc_interface.c \
         $(HAL_DIR)/stm32f4xx_ll_usb.c \
         $(HAL_DIR)/stm32f4xx_hal_pcd.c \
         $(HAL_DIR)/stm32f4xx_hal_dma.c
else
  # uart sources
  SRC += $(BOARD_DIR)/usart.c
endif

OBJ = $(patsubst %.c, %.o, $(SRC))
OBJ += $(BOARD_DIR)/start.o

# include files
INC = .
INC += ./cmsis
INC += ./hal/inc
INC += $(BOARD_DIR)
INC += $(GRBL_DIR)
INC += $(USB_DIR)/core
INC += $(USB_DIR)/cdc

INCLUDE = $(addprefix -I,$(INC))

# compiler flags
CFLAGS = -Wall
CFLAGS += -O
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

# linker flags
LDSCRIPT = stm32f407vg_flash.ld
LDFLAGS = -T$(LDSCRIPT) -Wl,-Map,$(OUTPUT).map -Wl,--gc-sections

# defines
DEFINES = -DSTM32F407xx
DEFINES += -DENABLE_M7
ifeq ($(USB_SERIAL), 1)
  DEFINES += -DUSB_SERIAL
endif

.S.o:
	$(X_CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $< -o $@
.c.o:
	$(X_CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $< -o $@

.PHONY: all program grbl_src clean

all: grbl_src $(OBJ)
	$(X_CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -lm -o $(OUTPUT)
	$(X_OBJCOPY) -O binary $(OUTPUT) $(OUTPUT).bin

program: 
	st-flash write $(OUTPUT).bin 0x08000000

grbl_src:
	make -C grbl all

clean:
	-rm $(OBJ)	
	-rm $(OUTPUT)
	-rm $(OUTPUT).map	
	-rm $(OUTPUT).bin	
	make -C grbl clean

