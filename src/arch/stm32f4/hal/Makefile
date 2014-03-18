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

OUT = libhal.a

all: $(OBJ)
	$(X_AR) rcs $(OUT) $(OBJ)

clean:
	-rm -rf $(OBJ)
	-rm $(OUT)

include $(TOP)/mk/common.mk
