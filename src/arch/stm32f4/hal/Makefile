TOP = ../../../..

SRC = ./src/stm32f4xx_hal.c \
      ./src/stm32f4xx_hal_rcc.c \
      ./src/stm32f4xx_hal_cortex.c \
      ./src/stm32f4xx_hal_gpio.c \

INC = ./inc
INC += ./cmsis
INC += $(TOP)/src/boards/stm32f4discovery

DEFINES = -DSTM32F407xx

OBJ = $(patsubst %.c, %.o, $(SRC))

all: $(OBJ)

clean:
	-rm -rf $(OBJ)

include $(TOP)/mk/common.mk
