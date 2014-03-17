
XTOOLS_DIR = /opt/gcc-arm-none-eabi-4_8-2013q4/bin
X_CC = $(XTOOLS_DIR)/arm-none-eabi-gcc
X_OBJCOPY = $(XTOOLS_DIR)/arm-none-eabi-objcopy
X_GDB = $(XTOOLS_DIR)/arm-none-eabi-gdb

PATCHFILES = $(sort $(wildcard patches/*.patch))

PATCH_CMD = \
        for f in $(PATCHFILES); do\
            echo $$f; \
            patch -s -d $(WORK_DIR) -p1 -b -z .original < $$f; \
        done

INCLUDE = $(addprefix -I,$(INC))

.c.o:
	$(X_CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $< -o $@

.PHONY: untar
untar:
	tar jxf $(TARFILE)
	$(PATCH_CMD)





