TOP = .
include $(TOP)/mk/common.mk

BOARD_DIR = $(TOP)/board/st/stm32f4/mb997
BIN_FILE = $(BOARD_DIR)/grbl.bin

.PHONY: all program clean

all:
	make -C grbl $@
	make -C $(BOARD_DIR) $@

program: 
	st-flash write $(BIN_FILE) 0x08000000

clean:
	make -C grbl $@
	make -C $(BOARD_DIR) $@
