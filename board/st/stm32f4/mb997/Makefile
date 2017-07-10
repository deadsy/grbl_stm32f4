TOP = ../../../..
include $(TOP)/mk/common.mk

OUTPUT = grbl

GRBL_DIR = $(TOP)/grbl/grbl-1.1f.20170131

#USB_SERIAL = 1

# hal sources
HAL_DIR = $(TOP)/board/st/stm32f4/hal/src
SRC += $(HAL_DIR)/stm32f4xx_hal.c \
       $(HAL_DIR)/stm32f4xx_hal_rcc.c \
       $(HAL_DIR)/stm32f4xx_hal_cortex.c \
       $(HAL_DIR)/stm32f4xx_hal_gpio.c \
       $(HAL_DIR)/stm32f4xx_hal_tim.c \
       $(HAL_DIR)/stm32f4xx_hal_tim_ex.c \

# board sources
BOARD_DIR = $(TOP)/board/st/stm32f4/mb997
SRC += $(BOARD_DIR)/main.c \
       $(BOARD_DIR)/system_stm32f4xx.c \
       $(BOARD_DIR)/stm32f4xx_it.c \
       $(BOARD_DIR)/syscalls.c \
       $(BOARD_DIR)/gpio.c \
       $(BOARD_DIR)/debounce.c \
       $(BOARD_DIR)/timers.c \

# rtt logging
RTT_DIR = $(TOP)/common/rtt
SRC += $(RTT_DIR)/SEGGER_RTT.c \
       $(RTT_DIR)/SEGGER_RTT_printf.c \

# common
COMMON_DIR = $(TOP)/common
SRC += $(COMMON_DIR)/logging.c \


# usb/uart sources
USB_DIR = $(TOP)/board/st/stm32f4/usb
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
         $(HAL_DIR)/stm32f4xx_hal_pcd_ex.c \
         $(HAL_DIR)/stm32f4xx_hal_dma.c \

else
  # uart sources
  SRC += $(BOARD_DIR)/usart.c \
         $(BOARD_DIR)/serial.c \

endif

OBJ = $(patsubst %.c, %.o, $(SRC))
OBJ += $(BOARD_DIR)/start.o

# include files

# include paths
INCLUDE += -I$(TOP)/board/st/stm32f4/hal/inc
INCLUDE += -I$(TOP)/board/st/stm32f4/cmsis
INCLUDE += -I$(GRBL_DIR)/grbl
INCLUDE += -I$(BOARD_DIR)
INCLUDE += -I$(RTT_DIR)
INCLUDE += -I$(COMMON_DIR)
INCLUDE += -I$(USB_DIR)/core
INCLUDE += -I$(USB_DIR)/cdc

#INC += $(GRBL_DIR)
#INC += $(USB_DIR)/cdc

# defines
DEFINE = -DSTM32F407xx
ifeq ($(USB_SERIAL), 1)
  DEFINE += -DUSB_SERIAL
endif

# linker flags
LDSCRIPT = stm32f407vg_flash.ld
X_LDFLAGS = -T$(LDSCRIPT) -Wl,-Map,$(OUTPUT).map -Wl,--gc-sections
X_LDFLAGS += -L$(GRBL_DIR)

# unused parameters in the HAL code
X_CFLAGS += -Wno-unused-parameter
X_CFLAGS += -Wno-strict-prototypes

.S.o:
	$(X_GCC) $(INCLUDE) $(DEFINE) $(X_CFLAGS) -c $< -o $@
.c.o:
	$(X_GCC) $(INCLUDE) $(DEFINE) $(X_CFLAGS) -c $< -o $@

.PHONY: all clean

all: $(OBJ)
	$(X_GCC) $(X_CFLAGS) $(X_LDFLAGS) $(OBJ) -lgrbl -lm -o $(OUTPUT)
	$(X_OBJCOPY) -O binary $(OUTPUT) $(OUTPUT).bin

clean:
	-rm $(OBJ)	
	-rm $(OUTPUT)
	-rm $(OUTPUT).map	
	-rm $(OUTPUT).bin	
