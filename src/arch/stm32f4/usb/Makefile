TOP = ../../../..

SRC = ./core/usbd_core.c \
      ./core/usbd_ctlreq.c \
      ./core/usbd_ioreq.c \
      ./cdc/usbd_cdc.c \

INC = ./core
INC += ./cdc
INC += $(TOP)/src/arch/stm32f4/hal/inc
INC += $(TOP)/src/arch/stm32f4/hal/cmsis
INC += $(TOP)/src/boards/stm32f4discovery

DEFINES = -DSTM32F407xx

OBJ = $(patsubst %.c, %.o, $(SRC))

all: $(OBJ)

clean:
	-rm -rf $(OBJ)

include $(TOP)/mk/common.mk
