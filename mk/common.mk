
XTOOLS_DIR = /opt/gcc-arm-none-eabi-4_8-2013q4/bin
X_CC = $(XTOOLS_DIR)/arm-none-eabi-gcc
X_OBJCOPY = $(XTOOLS_DIR)/arm-none-eabi-objcopy
X_AR = $(XTOOLS_DIR)/arm-none-eabi-ar
X_GDB = $(XTOOLS_DIR)/arm-none-eabi-gdb

PATCHFILES = $(sort $(wildcard patches/*.patch))

PATCH_CMD = \
        for f in $(PATCHFILES); do\
            echo $$f; \
            patch -s -d $(WORK_DIR) -p1 -b -z .original < $$f; \
        done

INCLUDE = $(addprefix -I,$(INC))

CFLAGS = -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

.c.o:
	$(X_CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $< -o $@

.PHONY: untar_patch_overlay
untar_patch_overlay:
	tar jxf $(TARFILE)
	$(PATCH_CMD)
	cd files; tar -c -f - * | (cd ../$(WORK_DIR) ; tar xfp -)
